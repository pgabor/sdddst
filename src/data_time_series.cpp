#include "data_time_series.h"
#include "constants.h"

#include <fstream>
#include <cmath>

DataTimeSeries::DataTimeSeries(std::string dconfList, std::string fconf):
    fconfPath(fconf)
{
    std::ifstream dcs(dconfList);
    while (!dcs.eof()) {
        std::string tmp;
        dcs >> tmp;
        if (tmp == "") {
            break;
        }
        std::string path;
        dcs >> path;
        dconfs.push_back(std::make_pair(std::stod(tmp), path));
    }
}

bool DataTimeSeries::next(std::shared_ptr<sdddstCore::SimulationData> sD)
{
    if (dconfs.size() == 0) {
        return false;
    }

    auto pair = dconfs.front();

    sD->readDislocationDataFromFile(pair.second);
    sD->readPointDefectDataFromFile(fconfPath);

    sD->A = DEFAULT_A * 1./sqrt(sD->dc);
    sD->KASQR = DEFAULT_KASQR * double(sD->dc);

    sD->simTime = pair.first;

    dconfs.pop_front();

    return true;
}
