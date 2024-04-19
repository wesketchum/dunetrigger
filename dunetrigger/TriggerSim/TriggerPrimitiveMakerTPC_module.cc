////////////////////////////////////////////////////////////////////////
// Class:       TriggerPrimitiveMakerTPC
// Plugin Type: producer (Unknown Unknown)
// File:        TriggerPrimitiveMakerTPC_module.cc
//
// Generated at Tue Nov 14 05:00:20 2023 by Wesley Ketchum using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////


#include "dunetrigger/TriggerSim/TPAlgTools/TPAlgTPCTool.hh"

#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/DetID.hpp"
#include "lardataobj/RawData/RawDigit.h"

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
#include <iostream>

namespace duneana {
  class TriggerPrimitiveMakerTPC;
}


class duneana::TriggerPrimitiveMakerTPC : public art::EDProducer {
public:
  explicit TriggerPrimitiveMakerTPC(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerPrimitiveMakerTPC(TriggerPrimitiveMakerTPC const&) = delete;
  TriggerPrimitiveMakerTPC(TriggerPrimitiveMakerTPC&&) = delete;
  TriggerPrimitiveMakerTPC& operator=(TriggerPrimitiveMakerTPC const&) = delete;
  TriggerPrimitiveMakerTPC& operator=(TriggerPrimitiveMakerTPC&&) = delete;

  // Required functions.
  void produce(art::Event& e) override;

private:

  // Declare member data here.
  art::InputTag rawdigit_tag_;
  std::unique_ptr<TPAlgTPCTool> tpalg_;
  int verbosity_;
};


duneana::TriggerPrimitiveMakerTPC::TriggerPrimitiveMakerTPC(fhicl::ParameterSet const& p)
  : EDProducer{p}  // ,
  , rawdigit_tag_(p.get<art::InputTag>("rawdigit_tag"))
  , tpalg_{art::make_tool<TPAlgTPCTool>(p.get<fhicl::ParameterSet>("tpalg"))}
  , verbosity_(p.get<int>("verbosity",0))
{
  // Call appropriate produces<>() functions here.
  // Call appropriate consumes<>() for any products to be retrieved by this module.
  produces<std::vector<dunedaq::trgdataformats::TriggerPrimitive>>();
  consumes<std::vector<raw::RawDigit>>(rawdigit_tag_);

  
}

void duneana::TriggerPrimitiveMakerTPC::produce(art::Event& e)
{
  // Implementation of required member function here.
  
  //make output collection for the TriggerPrimitive objects
  auto tp_col_ptr = std::make_unique< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >();

  //readout raw digits from event
  auto rawdigit_handle = e.getValidHandle< std::vector<raw::RawDigit> >(rawdigit_tag_);
  
  auto rawdigit_vec = *rawdigit_handle;

  if(verbosity_>0)
    std::cout << "Found " << rawdigit_vec.size() << " raw::RawDigits" << std::endl;

  for( auto const& digit : rawdigit_vec) {
    tpalg_->process_waveform(digit.ADCs(),
			     digit.Channel(),
			     (uint16_t)(dunedaq::detdataformats::DetID::Subdetector::kHD_TPC),
			     0,
			     *tp_col_ptr);
  }

  e.put(std::move(tp_col_ptr));

}

DEFINE_ART_MODULE(duneana::TriggerPrimitiveMakerTPC)
