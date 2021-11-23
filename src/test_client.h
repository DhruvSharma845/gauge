#include <iostream>
#include <vector>
#include <iterator>
#include <chrono>

#include "gauge.h"

class State {
public:
    struct StateIterator {
        struct Value {};
        using iterator_category = std::forward_iterator_tag;
        using value_type = Value;
        using pointer = Value;  // or also value_type*
        using reference = Value;  // or also value_type&

        StateIterator(): iterationCount(0) {}

        StateIterator(int iterationCount_): iterationCount(iterationCount_) {}

        Value operator*() {
            return Value();
        }

        StateIterator& operator++() {
            --iterationCount;
            return *this;
        }

        bool operator!=(StateIterator const&) const {
            if(iterationCount != 0) {
                return true;
            }
            return false;
        }

        int iterationCount;
    };

    StateIterator begin();
    StateIterator end();

    int maxIterations() { return iterations; }

    State(int iterations_): iterations(iterations_) {}
private:
    int iterations;
};


class GaugeMethodMetaData {
public:
    GaugeMethodMetaData(GaugeMethodType gaugeMethod_, int methodIndex_, int numOfThreads_);
    int getConfiguredNumOfThreads();
    GaugeMethodType getGaugeMethod();
    int getNumOfIterations();
    void runGaugeMethod(State& state);
private:
    GaugeMethodType gaugeMethod;
    int methodIndex;

    int numOfThreadsToRun;
    int numOfIterations;
};

class GaugeMethodStore {
public:
    void addMethod(GaugeMethodType gaugeMethod);
    std::vector<GaugeMethodMetaData> getGaugeMethods();
    static GaugeMethodStore* GetInstance();
protected:
     GaugeMethodStore();
private:
    GaugeMethodStore(GaugeMethodStore& other) =delete;
    void operator=(const GaugeMethodStore& other) =delete;
private:
    std::vector<GaugeMethodType> methods;
    static GaugeMethodStore* pInstance;
};

class GaugeRunStats {
public:
    double realTimeUsed;
    int numOfIterations;
};

class GaugeRunResult {
public:
    GaugeRunResult(double realTimeUsed_, int numOfIterations_);
    double getAverageRealTime() const;
private:
    double realTimeUsed;
    int numOfIterations;
};

class GaugeMethodRunner {
public:
    GaugeMethodRunner(GaugeMethodMetaData gaugeMethodMetaData_, GaugeRunStats gaugeRunStats_);
    void execute();
    GaugeRunResult getResults();
private:
    GaugeMethodMetaData gaugeMethodMetaData;
    GaugeRunStats gaugeRunStats;
    int iterations;
};

class Timer {
public:
    Timer();
    void startTimer();
    void stopTimer();
    double getRealTimeUsed();

public:
    double ChronoClockNow();
    int startRealTime;
    int realTimeUsed;
};
