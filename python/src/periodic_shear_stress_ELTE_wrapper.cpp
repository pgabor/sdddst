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

#include "periodic_shear_stress_ELTE_wrapper.h"

#include "Fields/PeriodicShearStressELTE.h"

PySdddstCore::PyPeriodicShearStressFieldELTE::PyPeriodicShearStressFieldELTE():
    PyField()
{
    // Nothing to do
}

PySdddstCore::PyPeriodicShearStressFieldELTE::~PyPeriodicShearStressFieldELTE()
{
    // Nothing to do
}

void PySdddstCore::PyPeriodicShearStressFieldELTE::setPWD(std::string path)
{
    pwd = path;
}

void PySdddstCore::PyPeriodicShearStressFieldELTE::init()
{
    std::unique_ptr<sdddstCoreELTE::PeriodicShearStressELTE> tmp(new sdddstCoreELTE::PeriodicShearStressELTE);
    tmp->loadStress(pwd, "xy", 1024);
    tmp->loadStress(pwd, "xy_diff_x", 1024);
    field.reset(tmp.release());
}

std::string PySdddstCore::PyPeriodicShearStressFieldELTE::name() const
{
    return "Periodic Shear Stress Field ELTE";
}
