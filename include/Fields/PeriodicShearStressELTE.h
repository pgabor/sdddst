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

/*
 * NOTE: This class is based on a library which was maintained by the Department of Materials physics
 * at Eötvös Loránd University, Budapest
 */

#ifndef SDDDST_CORE_PERIODIC_SHEAR_STRESS_H
#define SDDDST_CORE_PERIODIC_SHEAR_STRESS_H

#include <vector>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>

#include <gsl/gsl_sf_bessel.h>

#include "Fields/Field.h"

/*#define STRESSMSIZE	1024*/
#define STRESSMDT	double
//#define STRESSMFNXX	"/home/ispanovity/work/progs/dislocation_dynamics/periodic_sigma/periodic_stress_xx_1024x1024_bin.dat"
//#define STRESSMFNXX	"periodic_stress_xx_1024x1024_bin.dat"
//#define STRESSMFNYY	"/home/ispanovity/work/progs/dislocation_dynamics/periodic_sigma/periodic_stress_yy_1024x1024_bin.dat"
//#define STRESSMFNYY	"periodic_stress_yy_1024x1024_bin.dat"
//#define STRESSMFNXY	"/home/ispanovity/work/progs/dislocation_dynamics/periodic_sigma/periodic_stress_xy_1024x1024_bin.dat"
//#define STRESSMFNXY	"periodic_stress_xy_1024x1024_bin.dat"
//#define STRESSMFNXYCUT	"/home/ispanovity/work/progs/dislocation_dynamics/periodic_sigma/periodic_stress_xy_cut_1024x1024_bin.dat"
//#define STRESSMFNXY2ND	"/home/ispanovity/work/progs/dislocation_dynamics/periodic_sigma/periodic_stress_xy_2nd_order_1024x1024_bin.dat"
/*#define STRESSMRADIN2	(1000.0/STRESSMSIZE/STRESSMSIZE)*/
/*#define STRESSMRADOUT2	(1.3*STRESSMRADIN2)*/
#define STRESSMFACT	(4.0*M_PI)

#define MAX_H_FOR_DIFF (1.0/(4*stress_matrix_size))
#define MAX_DELTA_FOR_DIFF 1e-10
#define MIN_H_FOR_DIFF (MAX_H_FOR_DIFF/1000000)

#define ERRNO_INPUT	1

#define ANHARM_R0 0.001

namespace sdddstCoreELTE {

class PeriodicShearStressELTE : public sdddstCore::Field
{
public:	
    PeriodicShearStressELTE();
    virtual ~PeriodicShearStressELTE();
	
    void loadStress(std::string path, const char* str, int n, double R0 = 0.0);
    
    virtual void outPutStress();
    virtual double xy(double x, double y);
    virtual double xy_diff_x(double x, double y);

protected:
    std::ofstream fout;
    double** stressm_xy;
    double** stressm_xy_diff_x;

    double r0;
    unsigned int stress_matrix_size;
    double stress_interp_radius_in_sq;
    double stress_interp_radius_out_sq;
};

}
#endif //PERIODIC_SHEAR_STRESS_H
