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

#ifndef SDDDST_CORE_ANALYTIC_FIELD_H
#define SDDDST_CORE_ANALYTIC_FIELD_H

#include "Fields/Field.h"
#include "constants.h"
#include <fstream>

#include <cmath>

namespace sdddstCore {

class AnalyticField: public Field
{
public:
    AnalyticField();
    virtual ~AnalyticField();

    virtual void outPutStress();
    virtual double xy(double dx, double dy);
    virtual double xy_diff_x(double dx, double dy);
protected:
    std::ofstream fout;
};

}

#endif
