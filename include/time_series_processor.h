#ifndef TIME_SERIES_PROCESSOR_H
#define TIME_SERIES_PROCESSOR_H

#include "simulation_data.h"
#include "data_time_series.h"
#include "simulation.h"
#include "dislocation.h"

namespace sdddstEV {

class TimeSeriesProcessor {
public:
    TimeSeriesProcessor(std::shared_ptr<sdddstCore::SimulationData> simData, std::shared_ptr<DataTimeSeries> dateTimeSeries, sdddstCore::StressProtocol* stressProtocol);
    void run();

    double calculateHessianDerivative(int i, int j, int k, std::vector<sdddstCore::Dislocation> & dislocations);

    std::shared_ptr<sdddstCore::Simulation> sim;

private:
    std::shared_ptr<sdddstCore::SimulationData> sD;
    std::shared_ptr<DataTimeSeries> dTS;
};

}
#endif
