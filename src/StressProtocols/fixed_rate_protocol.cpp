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
    sD(nullptr),
    rate(0)
{
    // Nothing to do
}

sdddstCore::FixedRateProtocol::~FixedRateProtocol()
{
    //Nothing to do
}

void sdddstCore::FixedRateProtocol::setData(std::shared_ptr<sdddstCore::SimulationData> _sD)
{
    sD = _sD;
}

double sdddstCore::FixedRateProtocol::getExternalStress(char stepID)
{
    double currentTime = sD->simTime;
    if (3 == stepID)
    {
        currentTime += sD->stepSize * 0.5;
    }

    return rate * currentTime;
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
