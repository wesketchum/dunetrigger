/**
 * @file TriggerTPCInfoDisplay_module.cc
 *
 * @brief This module is an analyzer that displays information about trigger primitives, trigger activities, and trigger candidates.
 *
 * It reads trigger primitive, trigger activity, and trigger candidate data from the input event and fills three separate TTree objects with the data.
 * The module also provides general event information such as run number, subrun number, and event ID.
 *
 * The module takes three input tags to specify the collections of trigger primitive, trigger activity, and trigger candidate data.
 * It also supports an optional verbosity level to control the amount of output printed to the console.
 *
 * The filled TTree objects can be used for further analysis or visualization of the trigger data.
 */
////////////////////////////////////////////////////////////////////////
// Class:       TriggerTPCInfoDisplay
// Plugin Type: analyzer (Unknown Unknown)
// File:        TriggerTPCInfoDisplay_module.cc
//
// Generated at Mon Apr 29 11:24:28 2024 by Hamza Amar Es-sghir using cetskelgen
// from  version .
////////////////////////////////////////////////////////////////////////

#include "detdataformats/trigger/TriggerCandidateData.hpp"
#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/DetID.hpp"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RawData/RDTimeStamp.h"

#include "larcore/Geometry/Geometry.h"

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/make_tool.h"
#include "canvas/Utilities/InputTag.h"
#include "canvas/Persistency/Common/Assns.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// Additional framework includes
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"
#include "dunetrigger/TriggerSim/Verbosity.hh"

// ROOT includes
#include <TH1I.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TRandom.h>
#include <TVector3.h>
#include <fcntl.h>

#include <memory>
#include <algorithm>
#include <iostream>

namespace duneana {
  class TriggerTPCInfoDisplay;
}


class duneana::TriggerTPCInfoDisplay : public art::EDAnalyzer {
public:
  explicit TriggerTPCInfoDisplay(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerTPCInfoDisplay(TriggerTPCInfoDisplay const&) = delete;
  TriggerTPCInfoDisplay(TriggerTPCInfoDisplay&&) = delete;
  TriggerTPCInfoDisplay& operator=(TriggerTPCInfoDisplay const&) = delete;
  TriggerTPCInfoDisplay& operator=(TriggerTPCInfoDisplay&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  // Selected optional functions.
  void beginJob() override;

private:

  // Create output Tree
  TTree *fTPTree;
  TTree *fTPfromTPTATree;
  TTree *fTATree;
  TTree *fTCTree;

  // General event information
  int fRun;
  int fSubRun;
  unsigned int fEventID;

  ////////////////////////
  // fTPTree variables //
  ///////////////////////
  using timestamp_t = uint64_t;
  using channel_t = uint32_t;
  using version_t = uint16_t;
  using detid_t = uint16_t;

  channel_t fChannelID;
  timestamp_t fStart_time;
  timestamp_t fTime_over_threshold;
  timestamp_t fTime_peak;
  uint32_t fADC_integral;
  uint16_t fADC_peak;
  int fROP_ID;
  detid_t fDetID;
  int fType;
  int fAlgorithm;

  ///////////////////////////
  // fTPfromTPTATree variables //
  ///////////////////////////
  int fTAnumber;
  channel_t fChannelID_TPinTA;
  timestamp_t fStart_time_TPinTA;
  timestamp_t fTime_over_threshold_TPinTA;
  timestamp_t fTime_peak_TPinTA;
  uint32_t fADC_integral_TPinTA;
  uint16_t fADC_peak_TPinTA;

  ////////////////////////
  // fTATree variables //
  ///////////////////////
  channel_t fChannel_start_TA;
  channel_t fChannel_end_TA;
  channel_t fChannel_peak_TA;
  timestamp_t fTime_start_TA;
  timestamp_t fTime_end_TA;
  timestamp_t fTime_peak_TA;
  timestamp_t fTime_activity;
  uint32_t fADC_integral_TA;
  uint16_t fADC_peak_TA;
  int fAlgorithm_TA;

  ////////////////////////
  // fTCTree variables //
  ///////////////////////
  timestamp_t fTime_start_TC;
  timestamp_t fTime_end_TC;
  timestamp_t fTime_candidate;
  version_t version;
  unsigned int fType_TC;
  unsigned int fAlgorithm_TC;

  art::InputTag tp_tag_;
  art::InputTag ta_tag_;
  art::InputTag tc_tag_;
  int verbosity_;

  // Trigger information
  std::vector<dunedaq::trgdataformats::TriggerActivityData> fTriggerActivity;
  std::vector<dunedaq::trgdataformats::TriggerPrimitive> fTriggerPrimitive;
  std::vector<dunedaq::trgdataformats::TriggerCandidateData> fTriggerCandidate;
  
};


duneana::TriggerTPCInfoDisplay::TriggerTPCInfoDisplay(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  // ,
  , tp_tag_(p.get<art::InputTag>("tp_tag"))
  , ta_tag_(p.get<art::InputTag>("ta_tag"))
  , tc_tag_(p.get<art::InputTag>("tc_tag"))
  , verbosity_(p.get<int>("verbosity",0))
{
  consumes<std::vector<dunedaq::trgdataformats::TriggerPrimitive>>(tp_tag_);
  consumes<std::vector<dunedaq::trgdataformats::TriggerActivityData>>(ta_tag_);
  consumes<art::Assns<dunedaq::trgdataformats::TriggerActivityData, dunedaq::trgdataformats::TriggerPrimitive> >(ta_tag_);
  consumes<art::Assns<dunedaq::trgdataformats::TriggerPrimitive, dunedaq::trgdataformats::TriggerActivityData> >(ta_tag_);
  consumes<std::vector<dunedaq::trgdataformats::TriggerCandidateData>>(tc_tag_);
}

void duneana::TriggerTPCInfoDisplay::analyze(art::Event const& e)
{
  // Set all general event information
  fRun    = e.run();
  fSubRun = e.subRun();
  fEventID = e.id().event();

  // Take TPs from event
  auto tp_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(tp_tag_);  
  fTriggerPrimitive = *tp_handle;

  // Take TAs from event
  auto ta_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerActivityData> >(ta_tag_);
  fTriggerActivity = *ta_handle;

  // Take TPs from TP-TA association from event
  auto tpfromtpta_handle = e.getValidHandle< art::Assns<dunedaq::trgdataformats::TriggerPrimitive, dunedaq::trgdataformats::TriggerActivityData> >(ta_tag_);
  auto fTPfromTPTA = *tpfromtpta_handle;

  // Take TAs from TP-TA association from event
  auto tafromtpta_handle = e.getValidHandle< art::Assns<dunedaq::trgdataformats::TriggerActivityData, dunedaq::trgdataformats::TriggerPrimitive> >(ta_tag_);
  auto fTAfromTPTA = *tafromtpta_handle;

  // Take TCs from event
  auto tc_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerCandidateData> >(tc_tag_);
  fTriggerCandidate = *tc_handle;

  // Load the geometry service
  art::ServiceHandle<geo::Geometry> geom;

  if(verbosity_ >= Verbosity::kInfo)
  {
    //std::cout << "Found " << rawdigit_vec.size() << " raw::RawDigits" << std::endl;
    std::cout << "Found " << fTriggerPrimitive.size() << " TPs" << std::endl;
    std::cout << "Found " << fTriggerActivity.size() << " TAs" << std::endl;
    std::cout << "Found " << fTriggerCandidate.size() << " TCs" << std::endl;
  }

  // Fill TP tree
  for(long unsigned int i=0; i < fTriggerPrimitive.size(); i++)
  {
    fChannelID = fTriggerPrimitive[i].channel;
    fStart_time = fTriggerPrimitive[i].time_start;
    fTime_over_threshold = fTriggerPrimitive[i].time_over_threshold;
    fTime_peak = fTriggerPrimitive[i].time_peak;
    fADC_integral = fTriggerPrimitive[i].adc_integral;
    fADC_peak = fTriggerPrimitive[i].adc_peak;
    fDetID = fTriggerPrimitive[i].detid;
    fType = static_cast<int>(fTriggerPrimitive[i].type);
    fAlgorithm = static_cast<int>(fTriggerPrimitive[i].algorithm);
    
    // Get ROP ID (ReadOut Plane ID)
    auto rop = geom->ChannelToROP(fChannelID);
    fROP_ID = rop.ROP;
     
    // Fill tree
    fTPTree -> Fill();
  }

  // Fill TPinTA tree
  int ta_channel_start = 0;
  uint32_t ta_adc_int = 0;
  int ta_count = 0;
  for(long unsigned int i=0; i < fTPfromTPTA.size(); i++)
  {
    if (fTAfromTPTA[i].first->channel_start != ta_channel_start || fTAfromTPTA[i].first->adc_integral != ta_adc_int) {
      ta_channel_start = fTAfromTPTA[i].first->channel_start;
      ta_adc_int = fTAfromTPTA[i].first->adc_integral;
      ta_count++;
      if(verbosity_ >= Verbosity::kInfo)
	std::cout<<"For TA number "<<ta_count<<", the channel_start, start_time and adc_integral are  "<<ta_channel_start<<", "<<fTAfromTPTA[i].first->time_start<<" and "<<ta_adc_int<<"\n";
    }
    
    fTAnumber = ta_count;
    fChannelID_TPinTA = fTPfromTPTA[i].first->channel;
    fStart_time_TPinTA = fTPfromTPTA[i].first->time_start;
    fTime_over_threshold_TPinTA = fTPfromTPTA[i].first->time_over_threshold;
    fTime_peak_TPinTA = fTPfromTPTA[i].first->time_peak;
    fADC_integral_TPinTA = fTPfromTPTA[i].first->adc_integral;
    fADC_peak_TPinTA = fTPfromTPTA[i].first->adc_peak;
    
    // Fill tree
    fTPfromTPTATree -> Fill();
  }
  
  // Fill TA tree
  for(long unsigned int i=0; i < fTriggerActivity.size(); i++)
  {
    fChannel_start_TA = fTriggerActivity[i].channel_start;
    fChannel_end_TA = fTriggerActivity[i].channel_end;
    fChannel_peak_TA = fTriggerActivity[i].channel_peak;
    fTime_start_TA = fTriggerActivity[i].time_start;
    fTime_end_TA = fTriggerActivity[i].time_end;
    fTime_peak_TA = fTriggerActivity[i].time_peak;
    fTime_activity = fTriggerActivity[i].time_activity;
    fADC_integral_TA = fTriggerActivity[i].adc_integral;
    fADC_peak_TA = fTriggerActivity[i].adc_peak;
    fAlgorithm_TA = static_cast<int>(fTriggerActivity[i].algorithm);

    // Fill tree
    fTATree -> Fill();
  }

  // Fill TC tree
  for(long unsigned int i=0; i < fTriggerCandidate.size(); i++)
  {
    fTime_start_TC = fTriggerCandidate[i].time_start;
    fTime_end_TC = fTriggerCandidate[i].time_end;
    fTime_candidate = fTriggerCandidate[i].time_candidate;
    version = fTriggerCandidate[i].version;
    fType_TC = static_cast<int>(fTriggerCandidate[i].type);
    fAlgorithm_TC = static_cast<int>(fTriggerCandidate[i].algorithm);

    // Fill tree
    fTCTree -> Fill();
  }
}

void duneana::TriggerTPCInfoDisplay::beginJob()
{
  // Make our handle to the TFileService
  art::ServiceHandle<art::TFileService> tfs;
  // The TTrees
  fTPTree = tfs->make<TTree>("TPTree", "DAQ trigger primitive maker tree");
  fTPfromTPTATree = tfs->make<TTree>("TPinTATree", "DAQ trigger primitive in trigger activity tree");
  fTATree = tfs->make<TTree>("TATree", "DAQ trigger activity maker tree");
  fTCTree = tfs->make<TTree>("TCTree", "DAQ trigger candidate maker tree");
  
  ////////////////////////////////////////
  // fTriggerPrimitive tree information //
  ////////////////////////////////////////
  // General event information
  fTPTree -> Branch( "Event" , &fEventID, "Event/I"  );
  fTPTree -> Branch( "Run"   , &fRun    , "Run/I"    );
  fTPTree -> Branch( "SubRun", &fSubRun , "SubRun/I" );
  // Trigger primitive information
  fTPTree -> Branch( "ChannelID" , &fChannelID);
  fTPTree -> Branch( "ROP_ID" , &fROP_ID);
  fTPTree -> Branch( "Start_time" , &fStart_time);
  fTPTree -> Branch( "Time_over_threshold" , &fTime_over_threshold);
  fTPTree -> Branch( "Time_peak" , &fTime_peak);
  fTPTree -> Branch( "ADC_integral" , &fADC_integral);
  fTPTree -> Branch( "ADC_peak" , &fADC_peak);
  fTPTree -> Branch( "DetID" , &fDetID);
  fTPTree -> Branch( "Type" , &fType);
  fTPTree -> Branch( "Algorithm" , &fAlgorithm);

  //////////////////////////////
  // fTPfromTPTA tree information //
  //////////////////////////////
  fTPfromTPTATree -> Branch( "Event" , &fEventID, "Event/I"  );
  fTPfromTPTATree -> Branch( "Run"   , &fRun    , "Run/I"    );
  fTPfromTPTATree -> Branch( "SubRun", &fSubRun , "SubRun/I" );
  fTPfromTPTATree -> Branch( "TAnumber", &fTAnumber , "TAnumber/I" );
  fTPfromTPTATree -> Branch( "ChannelID" , &fChannelID_TPinTA);
  fTPfromTPTATree -> Branch( "Start_time" , &fStart_time_TPinTA);
  fTPfromTPTATree -> Branch( "Time_over_threshold" , &fTime_over_threshold_TPinTA);
  fTPfromTPTATree -> Branch( "Time_peak" , &fTime_peak_TPinTA);
  fTPfromTPTATree -> Branch( "ADC_integral" , &fADC_integral_TPinTA);
  fTPfromTPTATree -> Branch( "ADC_peak" , &fADC_peak_TPinTA);

  ////////////////////////////////////////
  // fTriggerActivity tree information //
  ///////////////////////////////////////
  // General event information
  fTATree -> Branch( "Event" , &fEventID, "Event/I"  );
  fTATree -> Branch( "Run"   , &fRun    , "Run/I"    );
  fTATree -> Branch( "SubRun", &fSubRun , "SubRun/I" );
  // Trigger activity information
  fTATree -> Branch( "Channel_start" , &fChannel_start_TA);
  fTATree -> Branch( "Channel_end" , &fChannel_end_TA);
  fTATree -> Branch( "Channel_peak"  , &fChannel_peak_TA);
  fTATree -> Branch( "Time_start" , &fTime_start_TA);
  fTATree -> Branch( "Time_end" , &fTime_end_TA);
  fTATree -> Branch( "Time_peak" , &fTime_peak_TA);
  fTATree -> Branch( "Time_activity" , &fTime_activity);
  fTATree -> Branch( "ADC_integral" , &fADC_integral_TA);
  fTATree -> Branch( "ADC_peak" , &fADC_peak_TA);
  fTATree -> Branch( "DetID" , &fDetID);
  fTATree -> Branch( "Type" , &fType);
  fTATree -> Branch( "Algorithm" , &fAlgorithm_TA);

  ////////////////////////////////////////
  // fTriggerCandidate tree information //
  ///////////////////////////////////////
  // General event information
  fTCTree -> Branch( "Event" , &fEventID, "Event/I"  );
  fTCTree -> Branch( "Run"   , &fRun    , "Run/I"    );
  fTCTree -> Branch( "SubRun", &fSubRun , "SubRun/I" );
  // Trigger candidate information
  fTCTree -> Branch( "Time_start" , &fTime_start_TC);
  fTCTree -> Branch( "Time_end" , &fTime_end_TC);
  fTCTree -> Branch( "Time_candidate" , &fTime_candidate);
  fTCTree -> Branch( "Version" , &version);
  fTCTree -> Branch( "DetID" , &fDetID);
  fTCTree -> Branch( "Type" , &fType_TC);
  fTCTree -> Branch( "Algorithm" , &fAlgorithm_TC);
}

DEFINE_ART_MODULE(duneana::TriggerTPCInfoDisplay)
