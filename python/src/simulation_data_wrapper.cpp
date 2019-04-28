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

#include "simulation_data_wrapper.h"

PySdddstCore::PySimulationData::PySimulationData(const std::string & dislocationDataFilePath, const std::string & pointDefectDataFilePath):
    ptr(new sdddstCore::SimulationData(dislocationDataFilePath, pointDefectDataFilePath))
{
    // Nothing to do
}

PySdddstCore::PySimulationData::~PySimulationData()
{
    // Nothing to do
}

std::shared_ptr<sdddstCore::SimulationData> PySdddstCore::PySimulationData::get()
{
    return ptr;
}

sdddstCore::SimulationData &PySdddstCore::PySimulationData::resolve()
{
    return *ptr.get();
}

std::string PySdddstCore::PySimulationData::__str__() const
{
    return "Simulation Data Container Object";
}

std::string PySdddstCore::PySimulationData::__repr__() const
{
    return __str__();
}
