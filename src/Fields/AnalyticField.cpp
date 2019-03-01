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

#include "Fields/AnalyticField.h"

using namespace sdddstCore;

double f(const double & dx, const double & cos2piy)
{
    double cosh2pix = cosh(M_PI * 2.0 * dx);
    return dx * (cosh2pix * cos2piy - 1.0) / ((cosh2pix - cos2piy) * (cosh2pix - cos2piy));
}

template<int images>
double g(const double & dx, const double & cos2piy)
{
    return g<images-1>(dx,cos2piy)+f(dx-double(images), cos2piy)+f(dx+double(images), cos2piy);
}

template<>
double g<0>(const double & dx, const double & cos2piy)
{
    return f(dx, cos2piy);
}

double f_dx(const double & dx, const double & cos2piy)
{
    double cosh2pix = cosh(M_PI * 2.0 * dx);
    double sinh2pix = sinh(M_PI * 2.0 * dx);

    return (cosh2pix * cos2piy - 1.0) / pow(cosh2pix-cos2piy, 2.0) +
            dx * (sinh2pix * cos2piy * 2.0 * M_PI / pow(cosh2pix-cos2piy, 2.0) -
                  (cosh2pix*cos2piy-1.0)/pow(cosh2pix-cos2piy, 3.0)*4.0*M_PI*sinh2pix);
}

template<int images>
double g_dx(const double & dx, const double & cos2piy)
{
    return g_dx<images-1>(dx, cos2piy) + f_dx(dx-double(images), cos2piy) + f_dx(dx+double(images), cos2piy);
}

template<>
double g_dx<0>(const double & dx, const double & cos2piy)
{
    return f_dx(dx, cos2piy);
}


AnalyticField::AnalyticField() :
    Field()
{
    std::ofstream out_file;
    out_file.open("field.dat", std::ios::app);

    fout = std::move(out_file);
}

AnalyticField::~AnalyticField()
{
    // Nothing to do
}

void AnalyticField::outPutStress(){

    float resolution = 0.005;

    for(float i = -0.5; i < 0.5; i += resolution){
        for(float j = -0.5; j < 0.5; j += resolution){
            fout << xy(i, j) << ";";
        }
        fout << "\n";
    }

}

double AnalyticField::xy(double dx, double dy)
{
    double cos2piy = cos(M_PI * 2.0 * dy);
    if (dx < 0)
    {
        return - dx * f(dx + double(ANALYTIC_FIELD_N+1), cos2piy) +
                (1.0 + dx) * f(dx - double(ANALYTIC_FIELD_N), cos2piy) +
                f(dx + double(ANALYTIC_FIELD_N), cos2piy) +
                g<ANALYTIC_FIELD_N-1>(dx, cos2piy);
    }

    return dx * f(dx - (double(ANALYTIC_FIELD_N+1)), cos2piy) +
            (1.0 - dx) * f(dx + double(ANALYTIC_FIELD_N), cos2piy) +
            f(dx - double(ANALYTIC_FIELD_N), cos2piy) +
            g<ANALYTIC_FIELD_N-1>(dx, cos2piy);
}

double AnalyticField::xy_diff_x(double dx, double dy)
{
    double cos2piy = cos(M_PI * 2.0 * dy);
    if (dx < 0)
    {
        return -f(dx+double(ANALYTIC_FIELD_N)+1.0, cos2piy)-
                dx*f_dx(dx+double(ANALYTIC_FIELD_N)+1.0, cos2piy)+
                f_dx(dx-double(ANALYTIC_FIELD_N), cos2piy)*(1.0+dx) +
                f(dx-double(ANALYTIC_FIELD_N), cos2piy) +
                f_dx(dx + double(ANALYTIC_FIELD_N), cos2piy) +
                g_dx<ANALYTIC_FIELD_N-1>(dx, cos2piy);
    }

    return  f(dx-double(ANALYTIC_FIELD_N)-1.0, cos2piy)+
            dx*f_dx(dx-double(ANALYTIC_FIELD_N)-1.0, cos2piy)+
            f_dx(dx+double(ANALYTIC_FIELD_N), cos2piy)*(1.0-dx) -
            f(dx+double(ANALYTIC_FIELD_N), cos2piy) +
            f_dx(dx - double(ANALYTIC_FIELD_N), cos2piy) +
            g_dx<ANALYTIC_FIELD_N-1>(dx, cos2piy);
}
