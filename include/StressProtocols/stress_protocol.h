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

#ifndef SDDDST_CORE_STRESS_PROTOCOL_H
#define SDDDST_CORE_STRESS_PROTOCOL_H

#include "simulation_data.h"

#include <memory>
#include <string>

namespace sdddstCore {

/**
 * @brief The StressProtocol class is the base class for all external stress protocol classes,
 * all parameters for calculation is used from a SimulationData object or should be preset
 */
class StressProtocol
{
public:
    StressProtocol();
    virtual ~StressProtocol();

    /**
     * @brief setData sets the simulation data to be used for the external stress calculation
     */
    virtual void setData(std::shared_ptr<SimulationData>);

    /**
     * @brief getExternalStress is used to calculate the external stress value
     * @param stepID 0: current time, 1: big step, 2: first substep, 3: second substep
     * @return the external stress at the current simulation time (before step)
     */
    virtual double getExternalStress(char stepID);

    /**
     * @brief getTriggerState can be used to check if there is an ongoing avalanche or not
     * @return trigger state
     */
    virtual bool getTriggerState();

    /**
     * @brief getType
     * @return returns with the type of the applied stress field
     */
    virtual std::string getType();
};

}

#endif
