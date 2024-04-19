#ifndef DUNEANA_TRIGGERSIM_TAALGTPCTOOL_hh
#define DUNEANA_TRIGGERSIM_TAALGTPCTOOL_hh

#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/trigger/TriggerActivityData.hpp"

#include <vector>

namespace duneana {

  class TAAlgTPCTool {

  public:

    virtual ~TAAlgTPCTool() noexcept = default; 

    virtual void initialize() {};
    virtual void process_tp(dunedaq::trgdataformats::TriggerPrimitive const& tp,
			    std::vector<dunedaq::trgdataformats::TriggerActivityData> & tas_out) = 0;
  };
}

#endif
