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

#include "Fields/PeriodicShearStressELTE.h"

using namespace sdddstCoreELTE;

PeriodicShearStressELTE::PeriodicShearStressELTE() :
    Field()
{
    std::ofstream out_file;
    out_file.open("field.dat", std::ios::app);

    fout = std::move(out_file);

    r0=0.0;

    stressm_xy = nullptr;
    stressm_xy_diff_x = nullptr;
}

PeriodicShearStressELTE::~PeriodicShearStressELTE()
{
    if (stressm_xy_diff_x)
    {
        for(unsigned int i=0; i<stress_matrix_size; ++i)
            delete[] stressm_xy_diff_x[i];
        delete[] stressm_xy_diff_x;
    }
    if (stressm_xy)
    {
        for(unsigned int i=0; i<stress_matrix_size; ++i)
            delete[] stressm_xy[i];
        delete[] stressm_xy;
    }

    fout.close();
}

void PeriodicShearStressELTE::loadStress(std::string path, const char *str, int n, double R0)
{
    if (path.size() > 0 && path[path.size()-1] != '/')
    {
        path += "/";
    }
    double*** stress_matrix;

    FILE* fd;

    char fname[256];

    stress_matrix_size = n;

    if(r0 != 0.0 && R0 != r0)
    {
        std::cerr << "Error: all loaded stress fields should have the same r0...\n";
        exit(1);
    }
    r0 = R0;

    stress_interp_radius_in_sq = 225.0 / stress_matrix_size / stress_matrix_size;
    //		stress_interp_radius_in_sq = 0.0;
    stress_interp_radius_out_sq = 1.3 * stress_interp_radius_in_sq;

    if(strcmp(str, "xy_diff_x") == 0)
    {
        if(stressm_xy == 0)
        {
            fprintf(stderr, "#Error! xy stress table should be loaded before its derivative can be computed ...\n");
            exit(1);
        }

        stressm_xy_diff_x = new double*[stress_matrix_size];
        for(unsigned int i=0; i<stress_matrix_size; ++i)
            stressm_xy_diff_x[i] = new double[stress_matrix_size];

        for(unsigned int i=0; i<stress_matrix_size; i++)
            for(unsigned int j=0; j<stress_matrix_size; j++)
                stressm_xy_diff_x[i][j] = ( stressm_xy[(i+1)%stress_matrix_size][j]
                        - stressm_xy[(i+stress_matrix_size-1)%stress_matrix_size][j] ) / 2.0 * stress_matrix_size;

        return;
    }

    else if(strcmp(str, "xy") == 0)
    {
        stress_matrix = &stressm_xy;
        //			strcpy(fname, STRESSMFNXY);
    }
    else
    {
        fprintf(stderr, "#Error! Wrong parameter at PeriodicShearStress::loadStress()!\n");
        exit(1);
    }

    if(r0 == 0.0)
        sprintf(fname, (path + std::string("periodic_stress_%s_%dx%d_bin.dat")).c_str(), str, stress_matrix_size, stress_matrix_size);
    else
        sprintf(fname, (path + std::string("periodic_stress_%s_%dx%d_r0_%.0e_bin.dat")).c_str(), str, stress_matrix_size, stress_matrix_size, r0);

    *stress_matrix = new double*[stress_matrix_size];
    for(unsigned int i=0; i<stress_matrix_size; ++i)
        (*stress_matrix)[i] = new double[stress_matrix_size];

    if((fd=fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "# Error: Unable to open %s!\n", fname);
        exit(ERRNO_INPUT);
    }

    for(unsigned int i=0; i<stress_matrix_size; i++)
        for(unsigned int j=0; j<stress_matrix_size; j++)
        {
            STRESSMDT stresstmp;
            if(fread(&stresstmp, sizeof(STRESSMDT), 1, fd)!=1)
            {
                fprintf(stderr, "\n#Error: File format or read error in %s!\n", fname);
                exit(ERRNO_INPUT);
            }

            if(strcmp(str, "xy") == 0)
                (*stress_matrix)[i][j] = (double) stresstmp;
        }
    fclose(fd);
}

void PeriodicShearStressELTE::outPutStress(){

    float resolution = 0.005;

    for(float i = -0.5; i < 0.5; i += resolution){
        for(float j = -0.5; j < 0.5; j += resolution){
            fout << xy(i, j) << ";";
        }
        fout << "\n";
    }

}

double PeriodicShearStressELTE::xy(double x, double y)
{


    //		if(x<-0.5 || x>0.5 || y<-0.5 || y>0.5)
    //			std::cout << "Big error!\n";
    double ret = 0.0;

    double x2=x*x;
    double y2=y*y;
    double r2=x2+y2;

    if(r2==0)
    {
        std::cerr << "ERROR! (Possible division by zero in func. calculate_tau_xy)\n";
        return 0.0;
    }

    if(r2 <= stress_interp_radius_out_sq)
    {
        ret = x*(x2 - y2)/(r2*r2);
        //			std::cout << "fagyi\n";

        if(r2 >= stress_interp_radius_in_sq)
        {
            double mul=(r2-stress_interp_radius_in_sq)/(stress_interp_radius_out_sq-stress_interp_radius_in_sq);
            const double stressmsized=stress_matrix_size;

            if(x<0.0) x+=1.0;
            if(y<0.0) y+=1.0;

            double xconv=x*stressmsized;
            double yconv=y*stressmsized;

            double xfloor=floor(xconv);
            double yfloor=floor(yconv);
            double xinterp=xconv-xfloor;
            double yinterp=yconv-yfloor;
            double xinterpconj=1.0-xinterp;
            double yinterpconj=1.0-yinterp;
            unsigned int xi=(unsigned int)xfloor;
            unsigned int yi=(unsigned int)yfloor;
            if(xi>=stress_matrix_size){xi=stress_matrix_size-1;} /*HACK!*/
            if(yi>=stress_matrix_size){yi=stress_matrix_size-1;} /*HACK!*/
            ret = (1.0-mul)*ret + mul * (
                        xinterpconj*yinterpconj*stressm_xy[xi][yi] +
                        xinterpconj*yinterp*stressm_xy[xi][(yi+1)%stress_matrix_size] +
                    xinterp*yinterpconj*stressm_xy[(xi+1)%stress_matrix_size][yi] +
                    xinterp*yinterp*stressm_xy[(xi+1)%stress_matrix_size][(yi+1)%stress_matrix_size] );

            //				std::cout << "1:\t" << ret << "\n";
        }
    }
    else
    {
        const double stressmsized=stress_matrix_size;
        //			double xconv=(x+0.5)*stressmsized;
        //			double yconv=(y+0.5)*stressmsized;

        if(x<0.0) x+=1.0;
        if(y<0.0) y+=1.0;

        double xconv=x*stressmsized;
        double yconv=y*stressmsized;

        double xfloor=floor(xconv);
        double yfloor=floor(yconv);
        double xinterp=xconv-xfloor;
        double yinterp=yconv-yfloor;
        double xinterpconj=1.0-xinterp;
        double yinterpconj=1.0-yinterp;
        unsigned int xi=(unsigned int)xfloor;
        unsigned int yi=(unsigned int)yfloor;
        if(xi>=stress_matrix_size){xi=stress_matrix_size-1;} /*HACK!*/
        if(yi>=stress_matrix_size){yi=stress_matrix_size-1;} /*HACK!*/
        //			if(xi < 0){xi += stress_matrix_size;}
        //			if(yi < 0){yi += stress_matrix_size;}
        ret = 	xinterpconj*yinterpconj*stressm_xy[xi][yi] +
                xinterpconj*yinterp*stressm_xy[xi][(yi+1)%stress_matrix_size] +
                xinterp*yinterpconj*stressm_xy[(xi+1)%stress_matrix_size][yi] +
                xinterp*yinterp*stressm_xy[(xi+1)%stress_matrix_size][(yi+1)%stress_matrix_size];

        //			std::cout << "2:\t" << ret << "\n";
    }

    return ret;
}




double PeriodicShearStressELTE::xy_diff_x(double x, double y)
{
    //		if(x<-0.5 || x>0.5 || y<-0.5 || y>0.5)
    //			std::cout << "Big error!\n";
    double ret = 0.0;

    double x2=x*x;
    double y2=y*y;
    double r2=x2+y2;

    if(r2==0)
    {
        std::cerr << "ERROR! (Possible division by zero in func. calculate_tau_xy)\n";
        return 0.0;
    }

    if(r2 <= stress_interp_radius_out_sq)
    {
        ret = -(x2*x2 + y2*y2 - 6.0*x2*y2)/(r2*r2*r2);
        //			std::cout << "fagyi\n";

        if(r2 >= stress_interp_radius_in_sq)
        {
            double mul=(r2-stress_interp_radius_in_sq)/(stress_interp_radius_out_sq-stress_interp_radius_in_sq);
            const double stressmsized=stress_matrix_size;

            if(x<0.0) x+=1.0;
            if(y<0.0) y+=1.0;

            double xconv=x*stressmsized;
            double yconv=y*stressmsized;

            double xfloor=floor(xconv);
            double yfloor=floor(yconv);
            double yinterp=yconv-yfloor;
            double yinterpconj=1.0-yinterp;
            unsigned int xi=(unsigned int)xfloor;
            unsigned int yi=(unsigned int)yfloor;
            if(xi>=stress_matrix_size){xi=stress_matrix_size-1;} /*HACK!*/
            if(yi>=stress_matrix_size){yi=stress_matrix_size-1;} /*HACK!*/
            ret = (1.0-mul)*ret + mul * (
                        -stressmsized*yinterpconj*stressm_xy[xi][yi] +
                        -stressmsized*yinterp*stressm_xy[xi][(yi+1)%stress_matrix_size] +
                    stressmsized*yinterpconj*stressm_xy[(xi+1)%stress_matrix_size][yi] +
                    stressmsized*yinterp*stressm_xy[(xi+1)%stress_matrix_size][(yi+1)%stress_matrix_size] );

            //				std::cout << "1:\t" << ret << "\n";
        }
    }
    else
    {
        const double stressmsized=stress_matrix_size;
        //			double xconv=(x+0.5)*stressmsized;
        //			double yconv=(y+0.5)*stressmsized;

        if(x<0.0) x+=1.0;
        if(y<0.0) y+=1.0;

        double xconv=x*stressmsized;
        double yconv=y*stressmsized;

        double xfloor=floor(xconv);
        double yfloor=floor(yconv);
        double yinterp=yconv-yfloor;
        double yinterpconj=1.0-yinterp;
        unsigned int xi=(unsigned int)xfloor;
        unsigned int yi=(unsigned int)yfloor;
        if(xi>=stress_matrix_size){xi=stress_matrix_size-1;} /*HACK!*/
        if(yi>=stress_matrix_size){yi=stress_matrix_size-1;} /*HACK!*/
        //			if(xi < 0){xi += stress_matrix_size;}
        //			if(yi < 0){yi += stress_matrix_size;}
        ret = 	-stressmsized*yinterpconj*stressm_xy[xi][yi] +
                -stressmsized*yinterp*stressm_xy[xi][(yi+1)%stress_matrix_size] +
                stressmsized*yinterpconj*stressm_xy[(xi+1)%stress_matrix_size][yi] +
                stressmsized*yinterp*stressm_xy[(xi+1)%stress_matrix_size][(yi+1)%stress_matrix_size];

        //			std::cout << "2:\t" << ret << "\n";
    }

    return ret;
}
