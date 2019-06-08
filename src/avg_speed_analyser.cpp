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

#include "avg_speed_analyser.h"

sdddstCore::AvgSpeedAnalyser::AvgSpeedAnalyser(int N):
    values(static_cast<size_t>(N), 0),
    in(new double[N]),
    out(static_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * static_cast<size_t>(N/2+1)))),
    plan(fftw_plan_dft_r2c_1d(N, in, out, FFTW_MEASURE))
{
    // Nothing to do
}

sdddstCore::AvgSpeedAnalyser::~AvgSpeedAnalyser()
{
    fftw_destroy_plan(plan);
    fftw_free(out);
}

void sdddstCore::AvgSpeedAnalyser::addValue(double speed)
{
    values.pop_front();
    values.push_back(speed);
}

void sdddstCore::AvgSpeedAnalyser::analyze()
{
    for (unsigned int i = 0; i < values.size(); i++)
    {
        in[i] = values[i];
    }

    fftw_execute(plan);
}
