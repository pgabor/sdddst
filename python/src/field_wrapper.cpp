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

#include "field_wrapper.h"

PySdddstCore::PyField::PyField()
{
    // Nothing to do
}

PySdddstCore::PyField::~PyField()
{
    // Nothing to do
}

void PySdddstCore::PyField::init()
{
    field = std::unique_ptr<sdddstCore::Field>(new sdddstCore::Field);
}

sdddstCore::Field *PySdddstCore::PyField::release()
{
    return field.release();
}

bool PySdddstCore::PyField::valid() const
{
    if (field.get() == nullptr)
    {
        return false;
    }
    return true;
}

std::string PySdddstCore::PyField::name() const
{
    return "Field";
}

std::string PySdddstCore::PyField::__str__() const
{
    std::string result = name() + " - valid: ";
    if (valid())
    {
        result += "true";
    }
    else
    {
        result += "false";
    }

    return result;
}

std::string PySdddstCore::PyField::__repr__() const
{
    return  __str__();
}
