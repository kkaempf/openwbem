#include "OW_CIMValue.hpp"
#include "PyOW_Array.hpp"

void registerOW_ArrayImpl_OW_CIMValue(const char* name)
{
    registerOW_ArrayImpl<OW_CIMValue>(name);
}
