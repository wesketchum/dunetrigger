////////////////////////////////////////////////////////////////////////
// Class:       TriggerActivityMakerTPC
// Plugin Type: producer (Unknown Unknown)
// File:        TriggerActivityMakerTPC_module.cc
//
// Generated at Tue Nov 14 05:00:20 2023 by Wesley Ketchum using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////


#include "dunetrigger/TriggerSim/TAAlgTools/TAAlgTPCTool.hh"

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/DetID.hpp"

#include "larcore/Geometry/Geometry.h"

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

#include <memory>
#include <algorithm>
#include <iostream>

namespace duneana {
  class TriggerActivityMakerTPC;
}


class duneana::TriggerActivityMakerTPC : public art::EDProducer {
public:
  explicit TriggerActivityMakerTPC(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerActivityMakerTPC(TriggerActivityMakerTPC const&) = delete;
  TriggerActivityMakerTPC(TriggerActivityMakerTPC&&) = delete;
  TriggerActivityMakerTPC& operator=(TriggerActivityMakerTPC const&) = delete;
  TriggerActivityMakerTPC& operator=(TriggerActivityMakerTPC&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;

private:

  // Declare member data here.
  art::InputTag tp_tag_;
  std::unique_ptr<TAAlgTPCTool> taalg_;
  int verbosity_;

  static bool compareTriggerPrimitive(dunedaq::trgdataformats::TriggerPrimitive tp1,
				      dunedaq::trgdataformats::TriggerPrimitive tp2);
};


duneana::TriggerActivityMakerTPC::TriggerActivityMakerTPC(fhicl::ParameterSet const& p)
  : EDProducer{p}  // ,
  , tp_tag_(p.get<art::InputTag>("tp_tag"))
  , taalg_{art::make_tool<TAAlgTPCTool>(p.get<fhicl::ParameterSet>("taalg"))}
  , verbosity_(p.get<int>("verbosity",0))
{
  // Call appropriate produces<>() functions here.
  // Call appropriate consumes<>() for any products to be retrieved by this module.
  produces<std::vector<dunedaq::trgdataformats::TriggerActivityData>>();
  consumes<std::vector<dunedaq::trgdataformats::TriggerPrimitive>>(tp_tag_);
 
}

bool duneana::TriggerActivityMakerTPC::compareTriggerPrimitive(dunedaq::trgdataformats::TriggerPrimitive tp1,
							       dunedaq::trgdataformats::TriggerPrimitive tp2)
{
  return tp1.time_start < tp2.time_start;
}

void duneana::TriggerActivityMakerTPC::produce(art::Event& e)
{
  
  //grab the geometry service
  art::ServiceHandle<geo::Geometry> geom;
  
  //make output collection for the TriggerActivityData objects
  auto ta_vec_ptr = std::make_unique< std::vector<dunedaq::trgdataformats::TriggerActivityData> >();

  //grab tps from event
  auto tp_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(tp_tag_);  
  auto tp_vec = *tp_handle;

  if(verbosity_>0)
    std::cout << "Found " << tp_vec.size() << " TPs" << std::endl;

  //need to sort TPs by ROP (APA, CRP...)
  std::map< readout::ROPID,std::vector<dunedaq::trgdataformats::TriggerPrimitive> > tps_per_rop_map;
  for( auto const& tp : tp_vec) {
    auto rop = geom->ChannelToROP(tp.channel);
    tps_per_rop_map[rop].push_back(tp);
  }

  //now, per map, we need to sort the tps by time
  // and then, can run the TA algorithm.
  for (auto & tps : tps_per_rop_map) {
    std::sort(tps.second.begin(),tps.second.end(),compareTriggerPrimitive);

    if(verbosity_>0){
      std::cout << "\t ROP: " << tps.first << std::endl;
      std::cout << "\t\t " << tps.second.size() << " TPs between [" 
		<< tps.second.front().time_start << ", " << tps.second.back().time_start
		<< "]" << std::endl;
    }

    //initialize our taalg
    taalg_->initialize();

    //loop through the TPs and process
    for( auto const& tp : tps.second)
      taalg_->process_tp(tp,*ta_vec_ptr);

  }

  e.put(std::move(ta_vec_ptr));

}

DEFINE_ART_MODULE(duneana::TriggerActivityMakerTPC)
