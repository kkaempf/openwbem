#include "OW_CIMProperty.hpp"
#include "PyOW_Array.hpp"

void registerOW_ArrayImpl_OW_CIMProperty(const char* name)
{
    registerOW_ArrayImpl<OW_CIMProperty>(name);
}
