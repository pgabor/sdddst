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

#include "fixed_rate_protocol_wrapper.h"

#include "StressProtocols/fixed_rate_protocol.h"

PySdddstCore::PyFixedRateProtocol::PyFixedRateProtocol():
    PySdddstCore::PyStressProtocol()
{
    // Nothing to do
}

PySdddstCore::PyFixedRateProtocol::~PyFixedRateProtocol()
{
    // Nothing to do
}

void PySdddstCore::PyFixedRateProtocol::init()
{
    stress.reset(new sdddstCore::FixedRateProtocol);
}

void PySdddstCore::PyFixedRateProtocol::setRate(double rate)
{
    if (valid())
    {
        static_cast<sdddstCore::FixedRateProtocol*>(stress.get())->setRate(rate);
    }
}

double PySdddstCore::PyFixedRateProtocol::getRate() const
{
    if (valid())
    {
        return static_cast<sdddstCore::FixedRateProtocol*>(stress.get())->getRate();
    }
    return 0;
}

std::string PySdddstCore::PyFixedRateProtocol::name() const
{
    return "Fixed Rate Protocol";
}
