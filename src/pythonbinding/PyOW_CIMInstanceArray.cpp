#include "OW_CIMInstance.hpp"
#include "PyOW_Array.hpp"

void registerOW_ArrayImpl_OW_CIMInstance(const char* name)
{
    registerOW_ArrayImpl<OW_CIMInstance>(name);
}
