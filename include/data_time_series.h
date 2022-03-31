#ifndef DATATIMESERIES_H
#define DATATIMESERIES_H

#include <list>
#include <string>
#include "simulation_data.h"

class DataTimeSeries
{
public:
    DataTimeSeries(std::string dconfList, std::string fconf = "");

    bool next(std::shared_ptr<sdddstCore::SimulationData> sD);

private:
    std::list<std::pair<double, std::string>> dconfs;
    std::string fconfPath;
};

#endif // DATATIMESERIES_H
