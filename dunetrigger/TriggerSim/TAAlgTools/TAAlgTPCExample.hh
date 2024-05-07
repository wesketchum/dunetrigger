#ifndef DUNETRIGGER_TRIGGERSIM_TAALGTPCEXAMPLE_hh
#define DUNETRIGGER_TRIGGERSIM_TAALGTPCEXAMPLE_hh

#include "fhiclcpp/ParameterSet.h" 

#include "detdataformats/DetID.hpp"
#include "dunetrigger/TriggerSim/TAAlgTools/TAAlgTPCTool.hh"

namespace duneana {

  class TAAlgTPCExample : public TAAlgTPCTool {

  public:
    explicit TAAlgTPCExample(fhicl::ParameterSet const& ps)
      : multiplicity_(ps.get<size_t>("multiplicity"))
      , verbosity_(ps.get<int>("verbosity",0))
    {
      initialize();
    }
    
    void initialize() override
    {
      ta_current_ = TriggerActivity();
      ta_current_.first.channel_end = 0;
      ta_current_.first.time_end = 0;
      ta_current_.first.detid = (uint16_t)(dunedaq::detdataformats::DetID::Subdetector::kHD_TPC);
      ta_current_.first.type = dunedaq::trgdataformats::TriggerActivityData::Type::kTPC;
      ta_current_.first.algorithm = dunedaq::trgdataformats::TriggerActivityData::Algorithm::kUnknown;
      
    }

    //process a single tp
    //if we have met some condition for making a TA, then add it to the output vector
    void process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp,
		            std::vector<TriggerActivity> & tas_out)
    {

      ta_current_.second.push_back(tp);

      if(ta_current_.first.time_start > tp->time_start)
	    ta_current_.first.time_start = tp->time_start;
      if(ta_current_.first.time_end < (tp->time_start + tp->time_over_threshold))
	    ta_current_.first.time_end = tp->time_start+tp->time_over_threshold;
      if(ta_current_.first.channel_start > tp->channel)
	    ta_current_.first.channel_start = tp->channel;
      if(ta_current_.first.channel_end < tp->channel)
	    ta_current_.first.channel_end = tp->channel;

      ta_current_.first.adc_integral += tp->adc_integral;

      if(ta_current_.second.size() >= multiplicity_){
	    tas_out.push_back(ta_current_);
	    initialize();
      }
	

    }

    
  private:
    const size_t multiplicity_;
    const int verbosity_;

    TriggerActivity ta_current_;
    
  };
  
}

#endif
