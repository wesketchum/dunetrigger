////////////////////////////////////////////////////////////////////////
// Class:       TriggerCandidateMakerTPC
// Plugin Type: producer (Unknown Unknown)
// File:        TriggerCandidateMakerTPC_module.cc
//
// Generated at Wed May 1 05:00:20 2024 by Michal Rigan using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////


#include "dunetrigger/TriggerSim/TCAlgTools/TCAlgTPCTool.hh"

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerCandidateData.hpp"
#include "detdataformats/DetID.hpp"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/make_tool.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "dunetrigger/TriggerSim/Verbosity.hh"

#include <memory>
#include <algorithm>
#include <iostream>

namespace duneana {
  class TriggerCandidateMakerTPC;
}


class duneana::TriggerCandidateMakerTPC : public art::EDProducer {
public:
  explicit TriggerCandidateMakerTPC(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerCandidateMakerTPC(TriggerCandidateMakerTPC const&) = delete;
  TriggerCandidateMakerTPC(TriggerCandidateMakerTPC&&) = delete;
  TriggerCandidateMakerTPC& operator=(TriggerCandidateMakerTPC const&) = delete;
  TriggerCandidateMakerTPC& operator=(TriggerCandidateMakerTPC&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;

private:

  // Declare member data here.
  art::InputTag ta_tag_;
  std::unique_ptr<TCAlgTPCTool> tcalg_;
  int verbosity_;

  static bool compareTriggerActivity(dunedaq::trgdataformats::TriggerActivityData ta1,
				     dunedaq::trgdataformats::TriggerActivityData ta2);
};


duneana::TriggerCandidateMakerTPC::TriggerCandidateMakerTPC(fhicl::ParameterSet const& p)
  : EDProducer{p}  // ,
  , ta_tag_(p.get<art::InputTag>("ta_tag"))
  , tcalg_{art::make_tool<TCAlgTPCTool>(p.get<fhicl::ParameterSet>("tcalg"))}
  , verbosity_(p.get<int>("verbosity",0))
{
  // Call appropriate produces<>() functions here.
  // Call appropriate consumes<>() for any products to be retrieved by this module.
  produces<std::vector<dunedaq::trgdataformats::TriggerCandidateData>>();
  consumes<std::vector<dunedaq::trgdataformats::TriggerActivityData>>(ta_tag_);
 
}

bool duneana::TriggerCandidateMakerTPC::compareTriggerActivity(dunedaq::trgdataformats::TriggerActivityData ta1,
							       dunedaq::trgdataformats::TriggerActivityData ta2)
{
  return ta1.time_start < ta2.time_start;
}

void duneana::TriggerCandidateMakerTPC::produce(art::Event& e)
{
  //make output collection for the TriggerCandidateData objects
  auto tc_vec_ptr = std::make_unique< std::vector<dunedaq::trgdataformats::TriggerCandidateData> >();

  //grab tas from event
  auto ta_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerActivityData> >(ta_tag_);  
  auto ta_vec = *ta_handle;

  if(verbosity_ >= Verbosity::kInfo)
    std::cout << "Found " << ta_vec.size() << " TAs" << std::endl;

  //we need to sort the tas by time
  //and then, can run the TC algorithm.
  std::sort(ta_vec.begin(),ta_vec.end(),compareTriggerActivity);

  if(verbosity_ >= Verbosity::kInfo){
    std::cout << "\t " << ta_vec.size() << " TAs between [" 
              << ta_vec.front().time_start << ", " << ta_vec.back().time_end
              << "]" << std::endl;
  }

  //initialize our tcalg
  tcalg_->initialize();

  //loop through the TAs and process
  for( auto const& ta : ta_vec)
    tcalg_->process_ta(ta,*tc_vec_ptr);

  e.put(std::move(tc_vec_ptr));

}

DEFINE_ART_MODULE(duneana::TriggerCandidateMakerTPC)
