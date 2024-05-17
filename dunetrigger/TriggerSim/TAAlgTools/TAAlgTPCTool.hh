#ifndef DUNEANA_TRIGGERSIM_TAALGTPCTOOL_hh
#define DUNEANA_TRIGGERSIM_TAALGTPCTOOL_hh

#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/trigger/TriggerActivityData.hpp"

#include "canvas/Persistency/Common/PtrVector.h"

#include <vector>

namespace duneana {

  class TAAlgTPCTool {

  public:

    typedef std::pair< dunedaq::trgdataformats::TriggerActivityData,
                       art::PtrVector<dunedaq::trgdataformats::TriggerPrimitive> > TriggerActivity;

    virtual ~TAAlgTPCTool() noexcept = default; 

    virtual void initialize() {};
    virtual void process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp,
			                std::vector<TriggerActivity> & tas_out) = 0;
  };
}

#endif
