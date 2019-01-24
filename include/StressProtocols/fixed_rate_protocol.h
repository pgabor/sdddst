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

#ifndef SDDDST_CORE_FIXED_RATE_PROTOCOL_H
#define SDDDST_CORE_FIXED_RATE_PROTOCOL_H

#include "stress_protocol.h"

namespace sdddstCore {

class FixedRateProtocol : public StressProtocol
{
public:
    FixedRateProtocol();
    virtual ~FixedRateProtocol();

    virtual void setData(std::shared_ptr<SimulationData> _sD);
    virtual double getExternalStress(char stepID);
    virtual std::string getType();

    double getRate() const;
    void setRate(double value);

private:
    std::shared_ptr<SimulationData> sD;
    double rate;
};

}

#endif
