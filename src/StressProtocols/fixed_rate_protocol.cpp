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

#include "StressProtocols/fixed_rate_protocol.h"

sdddstCore::FixedRateProtocol::FixedRateProtocol():
    StressProtocol(),
    rate(0),
    stressValues(new double[4])
{
    stressValues[0] = 0;
    stressValues[1] = 0;
    stressValues[2] = 0;
    stressValues[3] = 0;
}

sdddstCore::FixedRateProtocol::~FixedRateProtocol()
{
    delete[] stressValues;
    stressValues = nullptr;
}

void sdddstCore::FixedRateProtocol::calculateStress(double simulationTime, const std::vector<Dislocation> &, sdddstCore::StressProtocolStepType type)
{
    double value = simulationTime * rate;
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

double sdddstCore::FixedRateProtocol::getStress(sdddstCore::StressProtocolStepType type)
{
    switch (type)
    {
    case sdddstCore::StressProtocolStepType::Original:
        return stressValues[0];
    case sdddstCore::StressProtocolStepType::EndOfBigStep:
        return stressValues[1];
    case sdddstCore::StressProtocolStepType::EndOfFirstSmallStep:
        return stressValues[2];
    case sdddstCore::StressProtocolStepType::EndOfSecondSmallStep:
        return stressValues[3];
    }
    return 0;
}

std::string sdddstCore::FixedRateProtocol::getType()
{
    return "fixed-rate-stress";
}

double sdddstCore::FixedRateProtocol::getRate() const
{
    return rate;
}

void sdddstCore::FixedRateProtocol::setRate(double value)
{
    rate = value;
}
