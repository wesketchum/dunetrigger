#ifndef DUNEANA_TRIGGERSIM_TCALGTPCTOOL_hh
#define DUNEANA_TRIGGERSIM_TCALGTPCTOOL_hh

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerCandidateData.hpp"

#include <vector>

namespace duneana {

  class TCAlgTPCTool {

  public:

    virtual ~TCAlgTPCTool() noexcept = default; 

    virtual void initialize() {};
    virtual void process_ta(dunedaq::trgdataformats::TriggerActivityData const& ta,
			    std::vector<dunedaq::trgdataformats::TriggerCandidateData> & tcs_out) = 0;
  };
}

#endif
