////////////////////////////////////////////////////////////////////////
// Class:       TriggerActivityTxtDump
// Plugin Type: analyzer (Unknown Unknown)
// File:        TriggerActivityTxtDump_module.cc
//
// Generated at Thu Aug 15 14:22:55 2024 by ddrobner using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "detdataformats/trigger/TriggerActivityData.hpp"

#include <fstream>
#include <iostream>

#include <TCanvas.h>
#include <THStack.h>
#include <TLegend.h>
#include <TH1D.h>
#include <art/Framework/Services/Registry/ServiceHandle.h>
#include <fcntl.h>
#include <larcore/Geometry/Geometry.h>

class TriggerActivityTxtDump;


class TriggerActivityTxtDump : public art::EDAnalyzer {
public:
  explicit TriggerActivityTxtDump(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerActivityTxtDump(TriggerActivityTxtDump const&) = delete;
  TriggerActivityTxtDump(TriggerActivityTxtDump&&) = delete;
  TriggerActivityTxtDump& operator=(TriggerActivityTxtDump const&) = delete;
  TriggerActivityTxtDump& operator=(TriggerActivityTxtDump&&) = delete;

  //void endJob() override;
  void beginJob() override;
  void analyze(art::Event const& e) override;

private:

  art::InputTag sim_ta_tag;

  std::ofstream sim_out;


};


TriggerActivityTxtDump::TriggerActivityTxtDump(fhicl::ParameterSet const& p)
  : EDAnalyzer{p},
    sim_ta_tag(p.get<art::InputTag>("sim_ta_tag"))
    // ,
  // More initializers here.
{
  using dunedaq::trgdataformats::TriggerActivityData;
  // Call appropriate consumes<>() for any products to be retrieved by this
  // module.
  consumes<std::vector<TriggerActivityData>>(sim_ta_tag);
}

void TriggerActivityTxtDump::beginJob() {
  sim_out.open("sim_tas.txt");
}

void TriggerActivityTxtDump::analyze(art::Event const& e)
{
  using dunedaq::trgdataformats::TriggerActivityData;
  // Implementation of required member function here.
  std::vector<TriggerActivityData> sim_tas = *(e.getValidHandle<std::vector<TriggerActivityData>>(sim_ta_tag));

  std::cout << "Sim Length: " << sim_tas.size() << std::endl;

  

  // order is time_start, time_peak, time_activity, adc_peak, channel_peak,
  // channel_start, adc_int, event, time_end

  for(auto s : sim_tas){
    sim_out << "(" 
        << s.adc_integral <<  "," 
        << s.adc_peak << ","
        << (uint16_t)s.algorithm << ","
        << s.channel_end << ","
        << s.channel_peak << ","
        << s.channel_start << ","
        << (uint16_t)s.detid << ","
        << s.time_activity << "," 
        << s.time_end << "," 
        << s.time_peak << "," 
        << s.time_start << "," 
        << (uint16_t)s.type
        << ")" << std::endl;
  }

}

DEFINE_ART_MODULE(TriggerActivityTxtDump)


