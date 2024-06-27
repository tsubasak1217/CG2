#include "Vector3.h"
#include "Vector4.h"

Vector4 Vector3::ToVec4()
{
    return {x,y,z,1.0f};
}
