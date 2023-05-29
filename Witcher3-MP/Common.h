#pragma once

#include <cmath>

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed __int64      int64;
typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned __int64    uint64;

struct Vector4F
{
    float x = 0;
    float y = 0;
    float z = 0;
    float o = 1;

    float DistanceTo(Vector4F pos)
    {
        float d = sqrt(pow(pos.x - x, 2.f) + pow(pos.y - y, 2.f) + pow(pos.z - z, 2.f) * 1.f);
        return d;
    }

    bool null()
    {
        if (x && y && z)
            return false;
        return true;
    }
};