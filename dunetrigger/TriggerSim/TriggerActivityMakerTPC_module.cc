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

#include "art/Persistency/Common/PtrMaker.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/make_tool.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/PtrVector.h"
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

  static bool compareTriggerPrimitive(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp1,
				                      art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp2);
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
  produces<art::Assns<dunedaq::trgdataformats::TriggerActivityData,dunedaq::trgdataformats::TriggerPrimitive>>();
  consumes<std::vector<dunedaq::trgdataformats::TriggerPrimitive>>(tp_tag_);
 
}

bool duneana::TriggerActivityMakerTPC::compareTriggerPrimitive(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp1,
							                                   art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp2)
{
  return tp1->time_start < tp2->time_start;
}

void duneana::TriggerActivityMakerTPC::produce(art::Event& e)
{
  
  //grab the geometry service
  art::ServiceHandle<geo::Geometry> geom;
  
  //make output collections for the TriggerActivityData objects,
  //the TriggerPrimitives contained in them, and
  //the association between those
  auto ta_vec_ptr = std::make_unique< std::vector<dunedaq::trgdataformats::TriggerActivityData> >();
  auto tp_vec_ptr = std::make_unique< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >();
  auto tp_in_tas_assn_ptr = std::make_unique< art::Assns<dunedaq::trgdataformats::TriggerActivityData,dunedaq::trgdataformats::TriggerPrimitive> >();

  //make PtrMake objects for creating the assns
  art::PtrMaker<dunedaq::trgdataformats::TriggerActivityData> taPtrMaker{e};

    //grab tps from event
  auto tp_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(tp_tag_);
  auto tp_vec = *tp_handle;

  if(verbosity_>0)
    std::cout << "Found " << tp_vec.size() << " TPs" << std::endl;

  //need to sort TPs per plane per detector module (APA, CRP...)
  //note: use art::PtrVector here since we are going to need to store the assn between TA and TPs
  std::map< int, art::PtrVector<dunedaq::trgdataformats::TriggerPrimitive> > tps_per_rop_map;
  for( size_t i_tp=0; i_tp < tp_vec.size(); ++i_tp) {

    auto rop = geom->ChannelToROP(tp_vec[i_tp].channel);

    //TPs in the colleciton plane arrive in two sets, eg for APA1, they have rops (0, 0, 2) and (0, 0, 3)
    //merging the two TP sets (fixme: it is only working for data when only collection plane was active, eg, PD-2 run028508)
    readout::ROPID c0_r0_t2 = {0, 0, 2}, c0_r1_t2 = {0, 1, 2}, c0_r2_t2 = {0, 2, 2}, c0_r3_t2 = {0, 3, 2}; 
    readout::ROPID c0_r0_t3 = {0, 0, 3}, c0_r1_t3 = {0, 1, 3}, c0_r2_t3 = {0, 2, 3}, c0_r3_t3 = {0, 3, 3};
    int det_module = 0;
    if (rop == c0_r0_t2 || rop == c0_r0_t3 ) det_module = 0;
    else if (rop == c0_r1_t2 || rop == c0_r1_t3) det_module = 1;
    else if (rop == c0_r2_t2 || rop == c0_r2_t3) det_module = 2;
    else if (rop == c0_r3_t2 || rop == c0_r3_t3) det_module = 3;
    tps_per_rop_map[det_module].push_back( art::Ptr<dunedaq::trgdataformats::TriggerPrimitive>(tp_handle,i_tp) );
  }

  //now, per map, we need to sort the tps by time
  // and then, can run the TA algorithm.
  for (auto & tps : tps_per_rop_map) {
    std::sort(tps.second.begin(),tps.second.end(),compareTriggerPrimitive);

    if(verbosity_>0){
      std::cout << "\t Detector module number: " << tps.first << std::endl;
      std::cout << "\t\t " << tps.second.size() << " TPs between [" 
		<< tps.second.front()->time_start << ", " << tps.second.back()->time_start
		<< "]" << std::endl;
    }

    //create an output vector and initialize our taalg
    std::vector< TAAlgTPCTool::TriggerActivity> tas_out;
    // taalg_->initialize();

    //loop through the TPs and process
    for( auto const& tp : tps.second)
      taalg_->process_tp(tp,tas_out);

    //loop over the output TAs
    for( auto const& ta : tas_out){

        //create the art Ptrs needed for the associations
        auto const taPtr = taPtrMaker(ta_vec_ptr->size());

        //fill our final output collections
        ta_vec_ptr->emplace_back(ta.first);
        tp_in_tas_assn_ptr->addMany(taPtr,ta.second);
    }

  }

  //place onto the event
  e.put(std::move(ta_vec_ptr));
  e.put(std::move(tp_in_tas_assn_ptr));

}

DEFINE_ART_MODULE(duneana::TriggerActivityMakerTPC)
