#ifndef ESDATATYPES_H_
#define ESDATATYPES_H_

#include <cstdint>

namespace ESSynth {

// Basic types
typedef float ESFloatType;
typedef int32_t ESInt32Type;
typedef uint8_t ESByteType;

typedef union ESData {
    ESInt32Type data_int32;
    ESFloatType data_float;
} ESData;

enum class ESDataType { Integer, Float, Bool, Sample, Opaque };

}  // namespace ESSynth

#endif /* ESDATATYPES_H_ */
