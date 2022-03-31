/*
 * SDDDST Simple Discrete Dislocation Dynamics Toolkit
 * Copyright (C) 2015-2019  Gábor Péterffy <peterffy95@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef SDDDST_CORE_UTILITY_H
#define SDDDST_CORE_UTILITY_H

#include <cmath>
#include <vector>
#include "dislocation.h"
#include <time.h>
#include <sys/time.h>

inline void normalize(double &n)
{
    while (n < -0.5)
    {
        n += 1.0;
    }

    while (n >= 0.5)
    {
        n -= 1.0;
    }
}

inline double X(const double & x)
{
    return sin(2.0 * M_PI * x) * 0.5 / M_PI;
}

inline double X2(const double & x)
{
    return (1.0 - cos(2.0 * M_PI * x)) * 0.5 / M_PI / M_PI;
}

inline double E(const double & x, const double & y, const double & K)
{
    return exp(-K*(X2(x)+X2(y)));
}

inline double X_dx(const double & x)
{
    return cos(2.0*M_PI*x);
}

inline double X2_dx(const double & x)
{
    return sin(2.0*M_PI*x)/M_PI;
}

inline double E_dx(const double & x, const double & y, const double & K)
{
    return -E(x, y, K) * K * X2_dx(x);
}


// From:
// https://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows
inline double get_wall_time()
{
    struct timeval time;
    if (gettimeofday(&time, NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

// From:
// https://stackoverflow.com/questions/17432502/how-can-i-measure-cpu-time-and-wall-clock-time-on-both-linux-windows
inline double get_cpu_time()
{
    return (double)clock() / CLOCKS_PER_SEC;
}

inline double calculateStrainIncrement(const std::vector<sdddstCore::Dislocation> &old, const std::vector<sdddstCore::Dislocation> &newD)
{
    double result = 0;
    for (size_t i = 0; i < old.size(); i++)
    {
        result += newD[i].b * (newD[i].x - old[i].x);
    }
    return result;
}

#endif
