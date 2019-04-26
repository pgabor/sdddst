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

#ifndef PYSDDDST_CORE_SIMULATION_DATA_WRAPPER_H
#define PYSDDDST_CORE_SIMULATION_DATA_WRAPPER_H

#include "simulation_data.h"

#include <string>
#include <memory>

namespace PySdddstCore
{

class PySimulationData
{
public:
    PySimulationData(const std::string &dislocationDataFilePath, const std::string &pointDefectDataFilePath);
    virtual ~PySimulationData();

    virtual std::shared_ptr<sdddstCore::SimulationData> get();
    virtual sdddstCore::SimulationData& resolve();
    virtual std::string __str__() const;
    virtual std::string __repr__() const;

protected:
    std::shared_ptr<sdddstCore::SimulationData> ptr;
};

}

#endif
