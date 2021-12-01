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

#ifndef SDDDST_SPRING_PROTOCOL_H
#define SDDDST_SPRING_PROTOCOL_H

#include "stress_protocol.h"
#include "StressProtocols/fixed_rate_protocol.h"

namespace sdddstCore {

class SpringProtocol : public FixedRateProtocol
{
public:
    SpringProtocol();
    virtual ~SpringProtocol();

    virtual void calculateStress(double simulationTime, const std::vector<Dislocation> &dislocations, StressProtocolStepType type, double strain);

    virtual std::string getType();

    double getSpringConstant() const;
    void setSpringConstant(double newSpringConstant);

protected:
    double springConstant;
};

}

#endif
