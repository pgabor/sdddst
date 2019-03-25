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

#include "StressProtocols/stress_protocol.h"

sdddstCore::StressProtocol::StressProtocol()
{
    //Nothing to do
}

sdddstCore::StressProtocol::~StressProtocol()
{
    //Nothing to do
}

void sdddstCore::StressProtocol::calculateStress(double, const std::vector<sdddstCore::Dislocation>&, sdddstCore::StressProtocolStepType)
{
    //Nothing to do
}

double sdddstCore::StressProtocol::getStress(sdddstCore::StressProtocolStepType)
{
    return 0;
}

bool sdddstCore::StressProtocol::getTriggerState()
{
    return false;
}

std::string sdddstCore::StressProtocol::getType()
{
    return "zero-stress";
}
