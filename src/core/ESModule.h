#ifndef ESMODULE_H_
#define ESMODULE_H_

#include <array>
#include <type_traits>

#include "ESDataTypes.h"

namespace ESSynth {

static constexpr ESInt32Type ESSynthMaxInputs = 10;
static constexpr ESInt32Type ESSynthMaxOutputs = 10;
static constexpr ESInt32Type ESSynthMaxInternals = 10;

template <typename KeyType>
struct ESModuleIo {
    ESDataType dataType;
    const char* name;
    KeyType key;
};

template <typename IoType, ESInt32Type size>
using ESIoList = std::array<IoType, size>;

enum class ESEmptyKeyType {};

struct ESOutputRuntime;
struct ESModuleRuntimeData;

using ESProcessFunction = ESInt32Type (*)(ESData* moduleData, ESOutputRuntime* outputs,
                                          const ESInt32Type& flags);
using ESInitializeFunction = std::function<void(ESModuleRuntimeData*, ESData*)>;

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

typedef struct ESOutputRuntime {
    ESInt32Type count;
    ESConnectionData* connections;
} ESOutputRuntime;

namespace Impl {

template <typename T>
void GetDataHelper(ESData* data);

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Integer, ESInt32Type&>::type GetDataHelper(
    ESData* data) {
    return data->data_int32;
}

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Integer, const ESInt32Type&>::type GetDataHelper(
    const ESData* data) {
    return data->data_int32;
}

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Float, ESFloatType&>::type GetDataHelper(
    ESData* data) {
    return data->data_float;
}

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Float, const ESFloatType&>::type GetDataHelper(
    const ESData* data) {
    return data->data_float;
}

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Opaque, ESData&>::type GetDataHelper(ESData* data) {
    return *data;
}

template <ESDataType DataType>
typename std::enable_if<DataType == ESDataType::Opaque, const ESData&>::type GetDataHelper(
    const ESData* data) {
    return *data;
}

}  // namespace Impl

template <typename ModuleType, typename InputKeyType = ESEmptyKeyType,
          typename OutputKeyType = ESEmptyKeyType, typename InternalKeyType = ESEmptyKeyType>
struct ESModule {
    using TIn = InputKeyType;
    using TOut = OutputKeyType;
    using TInt = InternalKeyType;

    // Construct IO list using already constructed IOs
    template <typename... IOs>
    static constexpr auto MakeIoList(IOs&&... ios)
        -> ESIoList<typename std::common_type<IOs...>::type, sizeof...(ios)> {
        return {{std::forward<IOs>(ios)...}};
    }

    // Special case for empty io list
    static constexpr ESIoList<ESModuleIo<InputKeyType>, 0> MakeIoList() { return {}; }

    static constexpr ESModuleIo<InputKeyType> MakeInput(ESDataType dataType, const char* name,
                                                        InputKeyType key) {
        return ESModuleIo<InputKeyType>{dataType, name, key};
    }

    static constexpr ESModuleIo<OutputKeyType> MakeOutput(ESDataType dataType, const char* name,
                                                          OutputKeyType key) {
        return ESModuleIo<OutputKeyType>{dataType, name, key};
    }

    static constexpr ESModuleIo<InternalKeyType> MakeInternal(ESDataType dataType, const char* name,
                                                              InternalKeyType key) {
        return ESModuleIo<InternalKeyType>{dataType, name, key};
    }

    static constexpr ESModuleIo<InputKeyType> GetInput(InputKeyType key) {
        for (const auto it : ModuleType::GetInputList()) {
            if (it.key == static_cast<ESInt32Type>(key)) {
                return it;
            }
        }
    }

    static constexpr ESInt32Type GetNumInputs() { return ModuleType::GetInputList().size(); }

    static constexpr ESModuleIo<OutputKeyType> GetOutput(OutputKeyType key) {
        for (const auto it : ModuleType::GetOutputList()) {
            if (it.key == static_cast<ESInt32Type>(key)) {
                return it;
            }
        }
    }

    static constexpr ESInt32Type GetNumOutputs() { return ModuleType::GetOutputList().size(); }

    static constexpr ESModuleIo<InternalKeyType> GetInternal(InternalKeyType key) {
        for (const auto it : ModuleType::GetInternalList()) {
            if (it.key == static_cast<ESInt32Type>(key)) {
                return it;
            }
        }
    }

    static constexpr ESInt32Type GetNumInternals() { return ModuleType::GetInternalList().size(); }

    template <InputKeyType Key>
    static constexpr ESInt32Type InputIndex() {
        constexpr auto inputList = ModuleType::GetInputList();
        ESInt32Type index = 0;
        for (int i = 0; i < 9; ++i) {
            if (inputList[index].key == Key) {
                break;
            }
            index++;
        }
        return index;
    }

    template <OutputKeyType Key>
    static constexpr ESInt32Type OutputIndex() {
        constexpr auto outputList = ModuleType::GetOutputList();
        ESInt32Type index = 0;
        for (int i = 0; i < 9; ++i) {
            if (outputList[index].key == Key) {
                break;
            }
            index++;
        }
        return index;
    }

    template <InternalKeyType Key>
    static constexpr ESInt32Type InternalIndex() {
        constexpr auto internalList = ModuleType::GetInternalList();
        ESInt32Type index = 0;
        for (int i = 0; i < 9; ++i) {
            if (internalList[index].key == Key) {
                break;
            }
            index++;
        }
        return index;
    }

    template <InputKeyType Key>
    static constexpr const auto& Input(const ESData* inputs) {
        constexpr auto inputList = ModuleType::GetInputList();
        constexpr ESInt32Type index = InputIndex<Key>();
        return Impl::GetDataHelper<inputList[index].dataType>(&inputs[index]);
    }

    template <OutputKeyType Key>
    static constexpr auto& Output(ESData* data) {
        constexpr auto outputList = ModuleType::GetOutputList();
        constexpr ESInt32Type index = OutputIndex<Key>();
        return Impl::GetDataHelper<outputList[index].dataType>(&data[index]);
    }

    template <InternalKeyType Key>
    static constexpr auto& Internal(ESData* internals) {
        constexpr auto internalList = ModuleType::GetInternalList();
        constexpr ESInt32Type index = InternalIndex<Key>();
        return Impl::GetDataHelper<internalList[index].dataType>(&internals[index]);
    }

    template <OutputKeyType Key>
    static constexpr auto& NoIndexOutput(ESData* data) {
        constexpr auto outputList = ModuleType::GetOutputList();
        constexpr ESInt32Type index = OutputIndex<Key>();
        return Impl::GetDataHelper<outputList[index].dataType>(data);
    }

    static constexpr ESInt32Type InputFlag(const InputKeyType input) {
        return 1 << static_cast<ESInt32Type>(input);
    }

    template <OutputKeyType key, typename DataType>
    static void WriteOutput(const ESOutputRuntime* outputs, const DataType& data) {
        ESInt32Type i = 0;
        ESInt32Type index = static_cast<ESInt32Type>(key);
        while (i < outputs[index].count) {
            NoIndexOutput<key>(outputs[index].connections[i].data) = data;
            outputs[index].connections[i].module->flags |=
                InputFlag(static_cast<InputKeyType>(outputs[index].connections[i].input));
            ++i;
        }
    }
};

}  // namespace ESSynth

#endif /* ESMODULE_H_ */
