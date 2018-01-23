#ifndef ESENGINE_H_
#define ESENGINE_H_

#include <algorithm>
#include <functional>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "ESDataTypes.h"

namespace ESSynth {

struct ESModuleData;
struct ESOutput;
struct ESModuleRuntimeData;
using ESProcessFunction = ESInt32Type (*)(ESData* moduleData, ESOutput* outputs,
                                          const ESInt32Type& flags);
using ESInitializeFunction = std::function<void(ESModuleRuntimeData*, ESData*)>;

typedef struct ESModuleData {
    ESInt32Type id;
    ESProcessFunction process;
    ESInitializeFunction initialize;
    ESInt32Type inputs;
    ESInt32Type internals;
    ESInt32Type outs;
} ESModuleData;

typedef struct ESModuleRuntimeData {
    ESProcessFunction process;
    ESInt32Type size;
    ESInt32Type outs;
    ESInt32Type flags;
} ESModuleRuntimeData;

typedef struct ESConnectionData {
    ESModuleRuntimeData* module;
    ESData* data;
    ESInt32Type input;
} ESConnectionData;

typedef struct ESConnectionInfo {
    ESInt32Type out_module;
    ESInt32Type output;
    ESInt32Type in_module;
    ESInt32Type input;
} ESConnectionInfo;

typedef struct ESOutput {
    ESInt32Type count;
    ESConnectionData* connections;
} ESOutput;
namespace Impl {

template <typename ModuleType>
static ESInt32Type ProcessModule(ESData* data, ESOutput* outputs, const ESInt32Type& flags) {
    return ModuleType::Process(data, outputs, &data[ModuleType::num_inputs], flags);
}

// SFINAE: check if the module has an initialize function
template <class T>
class ESModuleHasInitializer {
    template <class ModuleType,
              class = typename std::enable_if<
                  !std::is_member_pointer<decltype(&ModuleType::Initialize)>::value>::type>
    static std::true_type check(int);
    template <class>
    static std::false_type check(...);

   public:
    static constexpr bool value = decltype(check<T>(0))::value;
};

template <typename ModuleType, bool HasInitializer, typename... InitArgs>
struct InitializeModuleHelper {
    static auto InitializeFunction(InitArgs... args) {
        return [=](ESModuleRuntimeData*, ESData*) {};
    }
};

template <typename ModuleType, typename... InitArgs>
struct InitializeModuleHelper<ModuleType, true, InitArgs...> {
    static auto InitializeFunction(InitArgs... args) {
        return [=](ESModuleRuntimeData* data, ESData* all_data) {
            ModuleType::Initialize(data, &all_data[ModuleType::num_inputs], args...);
        };
    }
};

// Use SFINAE to check whether the module has an initialize function or not
template <typename ModuleType, typename... InitArgs>
static auto InitializeModule(InitArgs... args) {
    return InitializeModuleHelper<ModuleType, ESModuleHasInitializer<ModuleType>::value,
                                  InitArgs...>::InitializeFunction(args...);
}

}  // namespace Impl

class ESEngine {
   public:
    template <typename ModuleType, typename... InitArgs>
    ESInt32Type CreateModule(InitArgs... args) {
        ESInt32Type index = modules_.size();

        // Add a new structure with the module information to the vector
        modules_.emplace_back(ESModuleData{
            index,                                        // Id of the module
            Impl::ProcessModule<ModuleType>,              // Processing function
            Impl::InitializeModule<ModuleType>(args...),  // Initialization function
            ModuleType::num_inputs, ModuleType::num_internals, ModuleType::num_outputs});

        return index;
    }

    void Process() {
        ESData* data_ptr = &data_[0];
        ESOutput* out_ptr = &outputs_[0];
        for (auto& mod_data : modules_runtime_) {
            mod_data.flags = mod_data.process(data_ptr, out_ptr, mod_data.flags);
            data_ptr += mod_data.size;
            out_ptr += mod_data.outs;
        }
    }

    void Connect(ESInt32Type outModule, ESInt32Type outIndex, ESInt32Type inModule,
                 ESInt32Type inIndex) {
        auto it = std::find_if(
            connections_.begin(), connections_.end(), [&](const ESConnectionInfo& connection) {
                return (connection.in_module == inModule) && (connection.input == inIndex);
            });
        if (it != connections_.end()) {
            throw std::runtime_error("Input connection already exists");
        }
        connections_.emplace_back(ESConnectionInfo{outModule, outIndex, inModule, inIndex});
    }

    void Prepare() {
        Reorder();
        Pack();
        Initialize();
    }

   private:
    template <typename TempExec>
    void ResolveExecution(ESInt32Type moduleId, TempExec& execData) {
        for (auto& connection : connections_) {
            if (connection.out_module != moduleId) {
                continue;
            }
            execData[connection.in_module] += 1;
        }
    }

    ESInt32Type CountConnections(ESInt32Type moduleId) {
        ESInt32Type result = 0;
        for (auto& connection : connections_) {
            if (connection.out_module == moduleId) {
                result++;
            }
        }
        return result;
    }

    void Reorder() {
        std::map<ESInt32Type, ESInt32Type> exec_vec;
        for (auto& module : modules_) {
            exec_vec[module.id] = 0;
        }

        for (auto it = modules_.begin(); it != modules_.end(); ++it) {
            auto& module = *it;
            auto num_connections = CountConnections(module.id);
            if (exec_vec[module.id] >= num_connections) {
                ResolveExecution(module.id, exec_vec);
                continue;
            }

            auto cngIt = it + 1;
            for (; cngIt != modules_.end(); ++cngIt) {
                auto& moduleToChange = *cngIt;
                auto num_connections_cng = CountConnections(moduleToChange.id);
                if (exec_vec[moduleToChange.id] >= num_connections_cng) {
                    break;
                }
            }
            if (cngIt != modules_.end()) {
                std::iter_swap(it, cngIt);
            }
            ResolveExecution(it->id, exec_vec);
        }
    }

    void Pack() {
        modules_runtime_.clear();
        data_.clear();
        outputs_.clear();
        connection_data_.clear();

        modules_runtime_.resize(modules_.size());
        connection_data_.resize(connections_.size());

        ESInt32Type index = 0;
        for (auto& module : modules_) {
            modules_runtime_[index].process = module.process;
            modules_runtime_[index].size = module.inputs + module.internals;
            modules_runtime_[index].outs = module.outs;
            modules_runtime_[index].flags = 0;
            data_.resize(data_.size() + module.inputs + module.internals);
            index++;
        }

        index = 0;
        ESConnectionData* ptr_conn_data = &connection_data_[0];
        // TODO organize deeply nested code
        for (auto& module : modules_) {
            for (ESInt32Type i = 0; i < module.outs; ++i) {
                outputs_.emplace_back(ESOutput{0, ptr_conn_data});
                for (auto& connection : connections_) {
                    if ((connection.out_module == module.id) && (connection.output == i)) {
                        ESData* data_ptr = &data_[0];
                        ESInt32Type inIndex = 0;
                        for (auto& inModule : modules_) {
                            if (connection.in_module == inModule.id) {
                                ptr_conn_data->module = &modules_runtime_[inIndex];
                                ptr_conn_data->data = &data_ptr[connection.input];
                                ptr_conn_data->input = connection.input;
                                ptr_conn_data++;
                                outputs_.back().count++;
                            }
                            data_ptr += inModule.inputs + inModule.internals;
                            inIndex++;
                        }
                    }
                }
            }

            index++;
        }
    }

    void Initialize() {
        static_assert(std::is_pod<ESModuleRuntimeData>::value, "ESModuleData is not POD");
        static_assert(std::is_pod<ESOutput>::value, "ESOutput is not POD");
        ESData* data_ptr = &data_[0];
        ESModuleRuntimeData* mod_data_runtime = &modules_runtime_[0];
        for (auto& mod_data : modules_) {
            mod_data.initialize(mod_data_runtime, data_ptr);
            data_ptr += mod_data.inputs + mod_data.internals;
            mod_data_runtime++;
        }
    }

    std::vector<ESModuleData> modules_;
    std::vector<ESConnectionInfo> connections_;

    // Runtime data is put in this way to improve cache efficiency.
    // Access to runtime is serialized an put in order so the probability
    // that the next data will be in cache is very high
    std::vector<ESModuleRuntimeData> modules_runtime_;
    std::vector<ESData> data_;
    std::vector<ESOutput> outputs_;
    std::vector<ESConnectionData> connection_data_;
};

inline ESInt32Type InputFlag(const ESInt32Type input) { return 1 << input; }

template <typename DataType>
void WriteOutput(const ESInt32Type index, const ESOutput* outputs, const DataType& data);

template <>
inline void WriteOutput<ESInt32Type>(const ESInt32Type index, const ESOutput* outputs,
                                     const ESInt32Type& data) {
    ESInt32Type i = 0;
    while (i < outputs[index].count) {
        outputs[index].connections[i].data->data_int32 = data;
        outputs[index].connections[i].module->flags |=
            InputFlag(outputs[index].connections[i].input);
        ++i;
    }
}

template <>
inline void WriteOutput<ESFloatType>(const ESInt32Type index, const ESOutput* outputs,
                                     const ESFloatType& data) {
    ESInt32Type i = 0;
    while (i < outputs[index].count) {
        outputs[index].connections[i].data->data_float = data;
        outputs[index].connections[i].module->flags |=
            InputFlag(outputs[index].connections[i].input);
        ++i;
    }
}

template <>
inline void WriteOutput<ESData>(const ESInt32Type index, const ESOutput* outputs,
                                const ESData& data) {
    ESInt32Type i = 0;
    while (i < outputs[index].count) {
        *outputs[index].connections[i].data = data;
        outputs[index].connections[i].module->flags |=
            InputFlag(outputs[index].connections[i].input);
        ++i;
    }
}

}  // namespace ESSynth

#endif /* ESENGINE_H_ */
