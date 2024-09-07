#ifndef DUNETRIGGER_TRIGGERSIM_VERSBOSITY_HH
#define DUNETRIGGER_TRIGGERSIM_VERSBOSITY_HH

namespace duneana{
    enum Verbosity{
        // these would all implicitly have these values
        // but it's best to explicitly define things
        // for anyone in the future looking
        kQuiet = 0,
        kInfo = 1,
        kDebug = 2,
        kVerbose = 3
    };
}

#endif