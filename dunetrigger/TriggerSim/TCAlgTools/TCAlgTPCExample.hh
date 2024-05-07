#ifndef DUNETRIGGER_TRIGGERSIM_TCALGTPCEXAMPLE_hh
#define DUNETRIGGER_TRIGGERSIM_TCALGTPCEXAMPLE_hh

#include "fhiclcpp/ParameterSet.h" 

#include "detdataformats/DetID.hpp"
#include "dunetrigger/TriggerSim/TCAlgTools/TCAlgTPCTool.hh"

namespace duneana {

  class TCAlgTPCExample : public TCAlgTPCTool {

  public:
    explicit TCAlgTPCExample(fhicl::ParameterSet const& ps)
      : multiplicity_(ps.get<size_t>("multiplicity"))
      , verbosity_(ps.get<int>("verbosity",0))
    {
      initialize();
    }
    
    void initialize() override
    {
      count_ = 0;
      tc_current_ = dunedaq::trgdataformats::TriggerCandidateData();
      tc_current_.time_start = 0;
      tc_current_.time_end = 0;
      tc_current_.time_candidate = 0;
      tc_current_.detid = (uint16_t)(dunedaq::detdataformats::DetID::Subdetector::kHD_TPC);
      tc_current_.type = dunedaq::trgdataformats::TriggerCandidateData::Type::kTPCLowE;
      tc_current_.algorithm = dunedaq::trgdataformats::TriggerCandidateData::Algorithm::kUnknown;
      
    }

    //process a single TA
    //for now promote every TA to TC, then add it to the output vector
    void process_ta(dunedaq::trgdataformats::TriggerActivityData const& ta,
		    std::vector<dunedaq::trgdataformats::TriggerCandidateData> & tcs_out)
    {
      ++count_;

      tc_current_.time_start = ta.time_start;
      tc_current_.time_end = ta.time_end;
      tc_current_.time_candidate = (ta.time_end - ta.time_start) + ta.time_start;

      if(count_ >= multiplicity_){
	tcs_out.push_back(tc_current_);
	initialize();
      }
	

    }

    
  private:
    const size_t multiplicity_;
    const int verbosity_;

    size_t count_;
    dunedaq::trgdataformats::TriggerCandidateData tc_current_;
    
  };
  
}

#endif
