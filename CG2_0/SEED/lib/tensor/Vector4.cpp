#include "Vector4.h"
#include "Vector3.h"


Vector3 Vector4::ToVec3()
{
    assert(w != 0.0f);
    return { x / w,y / w ,z / w };
}
