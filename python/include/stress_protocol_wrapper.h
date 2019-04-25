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

#include <memory>
#include <string>

#ifndef PYSDDDST_CORE_STRESS_PROTOCOL_WRAPPER_H
#define PYSDDDST_CORE_STRESS_PROTOCOL_WRAPPER_H

namespace PySdddstCore
{

class PyStressProtocol
{
public:
    PyStressProtocol();
    virtual ~PyStressProtocol();

    virtual void init();
    virtual sdddstCore::StressProtocol *release();

    virtual bool valid() const;
    virtual std::string name() const;
    virtual std::string __str__() const;
    virtual std::string __repr__() const;

protected:
    std::unique_ptr<sdddstCore::StressProtocol> stress;
};

}

#endif
