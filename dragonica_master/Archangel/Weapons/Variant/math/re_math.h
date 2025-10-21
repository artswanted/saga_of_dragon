#pragma once

namespace reoil
{

template<typename T>
inline T clamp(const T t_value, const T t_min, const T t_max)
{
    if (t_value >= t_max)
        return t_max;
    if (t_value <= t_min)
        return t_min;
    return t_value;
}

}
