#include "OW_CIMDataType.hpp"
#include "PyOW_Array.hpp"

void registerOW_ArrayImpl_OW_CIMDataType(const char* name)
{
    registerOW_ArrayImpl<OW_CIMDataType>(name);
}
