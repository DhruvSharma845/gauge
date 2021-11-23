#include "test_client.h"

GaugeMethodStore* GaugeMethodStore::pInstance{nullptr};

GaugeMethodStore::GaugeMethodStore() {}

GaugeMethodStore* GaugeMethodStore::GetInstance() {
    if(pInstance == nullptr) {
        pInstance = new GaugeMethodStore();
    }
    return pInstance;
}

void GaugeMethodStore::addMethod(GaugeMethodType gaugeMethod) {
    methods.push_back(gaugeMethod);
}

std::vector<GaugeMethodMetaData> GaugeMethodStore::getGaugeMethods() {
    std::vector<GaugeMethodMetaData> gaugesMetaData;
    int methodIndex = 0;
    for(auto gaugeMethodPtr: methods) {
        GaugeMethodMetaData instance{gaugeMethodPtr, methodIndex, 1};
        gaugesMetaData.push_back(instance);
        ++methodIndex;
    }
    return gaugesMetaData;
}

/***************************************/

GaugeMethodMetaData::GaugeMethodMetaData(GaugeMethodType gaugeMethod_, int methodIndex_, int numOfThreads_):
    gaugeMethod(gaugeMethod_),
    methodIndex(methodIndex_),
    numOfThreadsToRun(numOfThreads_),
    numOfIterations(0) {

}

int GaugeMethodMetaData::getConfiguredNumOfThreads() {
    return numOfThreadsToRun;
}

GaugeMethodType GaugeMethodMetaData::getGaugeMethod() {
    return gaugeMethod;
}

void GaugeMethodMetaData::runGaugeMethod(State& state) {
    gaugeMethod(state);
}

int GaugeMethodMetaData::getNumOfIterations() {
    return numOfIterations;
}

/***************************************/

GaugeMethodRunner::GaugeMethodRunner(GaugeMethodMetaData gaugeMethodMetaData_, GaugeRunStats gaugeRunStats_):
   gaugeMethodMetaData(gaugeMethodMetaData_),
   gaugeRunStats(gaugeRunStats_),
   iterations((gaugeMethodMetaData.getNumOfIterations() != 0) ? gaugeMethodMetaData.getNumOfIterations() : 1) {

}

void GaugeMethodRunner::execute() {
    Timer timer{};
    State state{iterations};
    gaugeMethodMetaData.runGaugeMethod(state);
    timer.stopTimer();
    gaugeRunStats.realTimeUsed = timer.getRealTimeUsed();
    gaugeRunStats.numOfIterations = iterations;
}

GaugeRunResult GaugeMethodRunner::getResults() {
    return GaugeRunResult{gaugeRunStats.realTimeUsed, gaugeRunStats.numOfIterations};
}

/***********************************/

Timer::Timer():
    startRealTime(0.0),
    realTimeUsed(0.0) {
        startTimer();
}

void Timer::startTimer() {
    startRealTime = ChronoClockNow();
    //std::cout << "Start time -> " << startRealTime << std::endl;
}

void Timer::stopTimer() {
    realTimeUsed = ChronoClockNow() - startRealTime;
    //std::cout << "End time -> " << realTimeUsed << std::endl;
}

double Timer::getRealTimeUsed() {
    return realTimeUsed;
}

double Timer::ChronoClockNow() {
    using FpSeconds = std::chrono::duration<double, std::chrono::seconds::period>;
    return FpSeconds(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

/************************************/

GaugeRunResult::GaugeRunResult(double realTimeUsed_, int numOfIterations_):
    realTimeUsed(realTimeUsed_),
    numOfIterations(numOfIterations_) {

}

double GaugeRunResult::getAverageRealTime() const {
    return (realTimeUsed / numOfIterations);
}
    
/***********************************/

auto reportResult(const GaugeRunResult& runResult) -> void {
    std::cout << runResult.getAverageRealTime() << " seconds." << std::endl;
}

auto registerGaugeMethod(GaugeMethodType gaugeMethod) -> int {
    GaugeMethodStore::GetInstance()->addMethod(gaugeMethod);
    return 0;
}

auto runProvidedGaugeMethods() -> void {
    std::vector<GaugeMethodMetaData> gaugeMethods{GaugeMethodStore::GetInstance()->getGaugeMethods()};
    if(gaugeMethods.empty()) {
        std::cout << "Did not find any gauge method to run!!!" << std::endl;
        return;
    }
    std::vector<GaugeMethodRunner> runners;
    for(const auto& gaugeMethodMetaData: gaugeMethods) {
        runners.emplace_back(gaugeMethodMetaData, GaugeRunStats());
    }

    for(auto& runner: runners) {
        runner.execute();
        GaugeRunResult runResult = runner.getResults();
        reportResult(runResult);
    }
}

/***********************************/

/**************************/

void Gauge_VectorInit(State& state) {
    for(int i = 0; i < state.maxIterations(); i++) {
        std::vector<int> v(state.maxIterations(), 0);
    }
    std::cout << "Vector Init" << std::endl;
}

/**************************/

GAUGE_REGISTER(Gauge_VectorInit);

GAUGE_DEFINE_MAIN();
