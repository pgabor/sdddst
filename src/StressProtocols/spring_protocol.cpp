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

#include "StressProtocols/spring_protocol.h"

sdddstCore::SpringProtocol::SpringProtocol():
    FixedRateProtocol(),
    springConstant(1.0)
{
}

sdddstCore::SpringProtocol::~SpringProtocol()
{
    // Nothing to do
}

void sdddstCore::SpringProtocol::calculateStress(double simulationTime, const std::vector<Dislocation> &, sdddstCore::StressProtocolStepType type, double strain)
{
    double value = springConstant*(simulationTime * rate - strain);
    if (value < 0) {
        value = 0;
    }
    switch (type)
    {
    case sdddstCore::StressProtocolStepType::Original:
        stressValues[0] = value;
        break;
    case sdddstCore::StressProtocolStepType::EndOfBigStep:
        stressValues[1] = value;
        break;
    case sdddstCore::StressProtocolStepType::EndOfFirstSmallStep:
        stressValues[2] = value;
        break;
    case sdddstCore::StressProtocolStepType::EndOfSecondSmallStep:
        stressValues[3] = value;
        break;
    }
}

std::string sdddstCore::SpringProtocol::getType()
{
    return "spring-stress";
}


double sdddstCore::SpringProtocol::getSpringConstant() const
{
    return springConstant;
}

void sdddstCore::SpringProtocol::setSpringConstant(double newSpringConstant)
{
    springConstant = newSpringConstant;
}

