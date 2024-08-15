#ifndef DUNETRIGGER_TRIGGERSIM_TRIGGERSIMCOMMON_HH
#define DUNETRIGGER_TRIGGERSIM_TRIGGERSIMCOMMON_HH

namespace duneana::TriggerSim {
    enum Verbosity {
        // declaring values here to be explicit
        kQuiet = 0,
        kInfo = 1,
        kDebug = 2,
        kVerbose = 3
    };
}

#endif