#ifndef __GAUGE_H__
#define __GAUGE_H__

class State;

using GaugeMethodType = void(*)(State& state);

int registerGaugeMethod(GaugeMethodType gaugeMethod);

#define GAUGE_REGISTER(...) \
    static int ANONYMOUS_VARIABLE = (registerGaugeMethod(&__VA_ARGS__))

void runProvidedGaugeMethods();

#define GAUGE_DEFINE_MAIN() \
    auto main(int argc, char* argv[]) -> int { \
        runProvidedGaugeMethods();  \
        return 0;   \
    }   \
    int main(int, char**)


#endif // __GAUGE_H__