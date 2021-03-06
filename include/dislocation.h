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

#ifndef SDDDST_CORE_DISLOCATION_H
#define SDDDST_CORE_DISLOCATION_H

#include <string>

namespace sdddstCore {

struct Dislocation
{
    double x;
    double y;
    double b;
    bool operator==(Dislocation a)
    {
        if (a.x == x &&
            a.y == y &&
            a.b == b)
        {
            return true;
        }
        return false;
    }

#ifdef BUILD_PYTHON_BINDINGS
    std::string __str__() const
    {
        return "x: " + std::to_string(this->x) +
               " y: " + std::to_string(this->y) +
               " b: " + std::to_string(this->b);
    }
    std::string __repr__() const
    {
       return __str__();
    }
#endif
};

}

#endif
