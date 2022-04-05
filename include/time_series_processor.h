#ifndef TIME_SERIES_PROCESSOR_H
#define TIME_SERIES_PROCESSOR_H

#include "simulation_data.h"
#include "data_time_series.h"
#include "simulation.h"

namespace sdddstEV {

class TimeSeriesProcessor {
public:
    TimeSeriesProcessor(std::shared_ptr<sdddstCore::SimulationData> simData, std::shared_ptr<DataTimeSeries> dateTimeSeries, sdddstCore::StressProtocol* stressProtocol);
    void run();

    std::shared_ptr<sdddstCore::Simulation> sim;

private:
    std::shared_ptr<sdddstCore::SimulationData> sD;
    std::shared_ptr<DataTimeSeries> dTS;
};

}
#endif
