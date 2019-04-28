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

#include "stress_protocol_wrapper.h"

PySdddstCore::PyStressProtocol::PyStressProtocol()
{
    // Nothing to do
}

PySdddstCore::PyStressProtocol::~PyStressProtocol()
{
    // Nothing to do
}

void PySdddstCore::PyStressProtocol::init()
{
    stress.reset(new sdddstCore::StressProtocol);
}

sdddstCore::StressProtocol *PySdddstCore::PyStressProtocol::release()
{
    return stress.release();
}

bool PySdddstCore::PyStressProtocol::valid() const
{
    if (stress.get())
    {
        return true;
    }
    return false;
}

std::string PySdddstCore::PyStressProtocol::name() const
{
    return "Stress Protocol";
}

std::string PySdddstCore::PyStressProtocol::__str__() const
{
    std::string result = name() + " - valid: ";
    if (valid())
    {
        result += "true";
    }
    else {
        result += "false";
    }
    return result;
}

std::string PySdddstCore::PyStressProtocol::__repr__() const
{
    return  __str__();
}
