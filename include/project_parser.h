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

#ifndef SDDDST_CORE_PROJECT_PARSER_H
#define SDDDST_CORE_PROJECT_PARSER_H

#include "simulation_data.h"
#include "data_time_series.h"

#include <memory>

#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>

namespace sdddstCore {

enum ProjectType {
    NONE,
    SIMULATION,
    EV_ANALYZATION
};

class ProjectParser
{
public:
    ProjectParser(int argc, char ** argv);
    ~ProjectParser();

    std::shared_ptr<SimulationData> getSimulationData();

    void printLicense();

    ProjectType getPType() const;
    std::shared_ptr<DataTimeSeries> getDataTimeSeries();

    sdddstCore::StressProtocol * getStressProtocol();

private:
    void processInput(boost::program_options::variables_map & vm);
    std::shared_ptr<SimulationData> sD;
    ProjectType pType;
    boost::program_options::variables_map vm;
};

}

#endif
