#ifndef DUNETRIGGER_TRIGGERSIM_TPALGTPCTOOL_hh
#define DUNETRIGGER_TRIGGERSIM_TPALGTPCTOOL_hh

#include "detdataformats/trigger/TriggerPrimitive.hpp"

#include <vector>

namespace duneana {

  class TPAlgTPCTool {

  public:

    static const unsigned int ADC_SAMPLING_RATE_IN_DTS = 32; //32 DTS time ticks betwen adc samples

    virtual ~TPAlgTPCTool() noexcept = default; 

    //take in a waveform, add trigger primitives to it
    virtual void process_waveform(std::vector<short> const& adcs,
				  dunedaq::trgdataformats::channel_t const channel,
				  dunedaq::trgdataformats::detid_t const detid,
				  dunedaq::trgdataformats::timestamp_t const start_time,
				  std::vector<dunedaq::trgdataformats::TriggerPrimitive> & tps_out) = 0;
  };
}

#endif
