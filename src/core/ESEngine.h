#ifndef ESENGINE_H_
#define ESENGINE_H_

#include <algorithm>
#include <functional>
#include <map>
#include <vector>

#include "ESDataTypes.h"
#include "ESModule.h"

namespace ESSynth {

typedef struct ESModuleData {
    ESInt32Type id;
    ESProcessFunction process;
    ESInitializeFunction initialize;
    ESInt32Type inputs;
    ESInt32Type internals;
    ESInt32Type outs;
} ESModuleData;

typedef struct ESConnectionInfo {
    ESInt32Type out_module;
    ESInt32Type output;
    ESInt32Type in_module;
    ESInt32Type input;
} ESConnectionInfo;

typedef struct ESConstValue {
    ESInt32Type module;
    ESInt32Type input;
    ESData data;
} ESConstValue;

namespace Impl {
template <typename ModuleType>
static ESInt32Type ProcessModule(ESData* data, ESOutputRuntime* outputs, const ESInt32Type& flags) {
    return ModuleType::Process(data, outputs, &data[ModuleType::GetNumInputs()], flags);
}

// SFINAE: check if the module has an initialize function
template <class T>
class ESModuleHasInitializer {
    template <class ModuleType, class = typename std::enable_if<!std::is_member_pointer<
                                    decltype(&ModuleType::Initialize)>::value>::type>
    static std::true_type check(int);
    template <class>
    static std::false_type check(...);

   public:
    static constexpr bool value = decltype(check<T>(0))::value;
};

template <typename ModuleType, bool HasInitializer, typename... InitArgs>
struct InitializeModuleHelper {
    static auto InitializeFunction(InitArgs...) {
        return [=](ESModuleRuntimeData*, ESData*) {};
    }
};

template <typename ModuleType>
struct InitializeModuleHelper<ModuleType, true> {
    static auto InitializeFunction() {
        return [=](ESModuleRuntimeData* data, ESData* all_data) {
            ModuleType::Initialize(data, &all_data[ModuleType::GetNumInputs()]);
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
    ESEngine() : current_index(0) {}

    template <typename ModuleType>
    ESInt32Type CreateModule() {
        // Add a new structure with the module information to the vector
        modules_.emplace_back(
            ESModuleData{current_index,                         // Id of the module
                         Impl::ProcessModule<ModuleType>,       // Processing function
                         Impl::InitializeModule<ModuleType>(),  // Initialization function
                         ModuleType::GetNumInputs(), ModuleType::GetNumInternals(),
                         ModuleType::GetNumOutputs()});

        return current_index++;
    }

    void DeleteModule(ESInt32Type id) {
        std::remove_if(modules_.begin(), modules_.end(),
                       [id](const ESModuleData& data) { return data.id == id; });
    }

    void Process() {
        ESData* data_ptr = &data_[0];
        ESOutputRuntime* out_ptr = &outputs_[0];
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

    void SetConstData(ESInt32Type module, ESInt32Type input, const ESData& value) {
        std::remove_if(const_values_.begin(), const_values_.end(),
                       [module, input](const ESConstValue& data) {
                           return (data.module == module) && (data.input == input);
                       });
        const_values_.emplace_back(ESConstValue{module, input, value});
    }

    void Prepare() {
        Reorder();
        Pack();
        Initialize();
        SendConstValues();
    }

    void SendEvent(ESInt32Type module, ESInt32Type input, const ESData& value) {
        ESData* data_ptr = &data_[0];
        for (auto& mod_data : modules_) {
            if (mod_data.id == module) {
                data_ptr[input] = value;
                break;
            }
            data_ptr += mod_data.inputs + mod_data.internals;
        }
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
                outputs_.emplace_back(ESOutputRuntime{0, ptr_conn_data});
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
        static_assert(std::is_pod<ESOutputRuntime>::value, "ESOutputRuntime is not POD");
        ESData* data_ptr = &data_[0];
        ESModuleRuntimeData* mod_data_runtime = &modules_runtime_[0];
        for (auto& mod_data : modules_) {
            mod_data.initialize(mod_data_runtime, data_ptr);
            data_ptr += mod_data.inputs + mod_data.internals;
            mod_data_runtime++;
        }
    }

    void SendConstValues() {
        for (auto& value : const_values_) {
            SendEvent(value.module, value.input, value.data);
        }
    }

    std::vector<ESModuleData> modules_;
    std::vector<ESConnectionInfo> connections_;
    std::vector<ESConstValue> const_values_;

    // Runtime data is put in this way to improve cache efficiency.
    // Access to runtime is serialized an put in order so the probability
    // that the next data will be in cache is very high
    std::vector<ESModuleRuntimeData> modules_runtime_;
    std::vector<ESData> data_;
    std::vector<ESOutputRuntime> outputs_;
    std::vector<ESConnectionData> connection_data_;

    // Current index being generated
    ESInt32Type current_index;
};

}  // namespace ESSynth

#endif /* ESENGINE_H_ */
