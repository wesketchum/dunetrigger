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
      count_ = 0;
      ta_current_ = dunedaq::trgdataformats::TriggerActivityData();
      ta_current_.channel_end = 0;
      ta_current_.time_end = 0;
      ta_current_.detid = (uint16_t)(dunedaq::detdataformats::DetID::Subdetector::kHD_TPC);
      ta_current_.type = dunedaq::trgdataformats::TriggerActivityData::Type::kTPC;
      ta_current_.algorithm = dunedaq::trgdataformats::TriggerActivityData::Algorithm::kUnknown;
      
    }

    //process a single tp
    //if we have met some condition for making a TA, then add it to the output vector
    void process_tp(dunedaq::trgdataformats::TriggerPrimitive const& tp,
		    std::vector<dunedaq::trgdataformats::TriggerActivityData> & tas_out)
    {
      ++count_;

      if(ta_current_.time_start > tp.time_start)
	ta_current_.time_start = tp.time_start;
      if(ta_current_.time_end < (tp.time_start + tp.time_over_threshold))
	ta_current_.time_end = tp.time_start+tp.time_over_threshold;
      if(ta_current_.channel_start > tp.channel)
	ta_current_.channel_start = tp.channel;
      if(ta_current_.channel_end < tp.channel)
	ta_current_.channel_end = tp.channel;

      ta_current_.adc_integral += tp.adc_integral;

      if(count_ >= multiplicity_){
	tas_out.push_back(ta_current_);
	initialize();
      }
	

    }

    
  private:
    const size_t multiplicity_;
    const int verbosity_;

    size_t count_;
    dunedaq::trgdataformats::TriggerActivityData ta_current_;
    
  };
  
}

#endif
