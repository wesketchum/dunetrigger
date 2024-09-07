/**
 * @file TriggerTPCInfoComparator_module.cc
 *
 * @brief This module compares offline and online trigger information for TPC.
 *
 * The TriggerTPCInfoComparator module compares the trigger information obtained from LArSoft (offline trigger) and the DAQ (online trigger) for the TPC. It analyzes the TPs (Trigger Primitives), TAs (Trigger Activities), and TCs (Trigger Candidates) and compares them to check for any discrepancies or differences between the offline and online triggers.
 *
 * The module takes the following input tags:
 * - tp_tag: Input tag for the TriggerPrimitive data from LArSoft
 * - ta_tag: Input tag for the TriggerActivity data from LArSoft
 * - tc_tag: Input tag for the TriggerCandidate data from LArSoft
 * - daq_tag: Input tag for the trigger data from the DAQ
 *
 * The module performs the following tasks:
 * - Compares the offline and online TPs and sets the comparison result
 * - Compares the offline and online TAs and sets the comparison result
 * - Compares the offline and online TCs and sets the comparison result
 *
 * The module also provides general event information such as run number, subrun number, and event ID.
 *
 * This module is generated using cetskelgen.
 *
 * @date Mon May 13 2024
 * @author Hamza Amar Es-sghir
 */
////////////////////////////////////////////////////////////////////////
// Class:       TriggerTPCInfoComparator
// Plugin Type: analyzer (Unknown Unknown)
// File:        TriggerTPCInfoComparator_module.cc
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
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "dunetrigger/TriggerSim/Verbosity.hh"

// Additional framework includes
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"

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
  class TriggerTPCInfoComparator;
}


class duneana::TriggerTPCInfoComparator : public art::EDAnalyzer {
public:
  explicit TriggerTPCInfoComparator(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerTPCInfoComparator(TriggerTPCInfoComparator const&) = delete;
  TriggerTPCInfoComparator(TriggerTPCInfoComparator&&) = delete;
  TriggerTPCInfoComparator& operator=(TriggerTPCInfoComparator const&) = delete;
  TriggerTPCInfoComparator& operator=(TriggerTPCInfoComparator&&) = delete;

  // Required functions.
  void analyze(art::Event const& e) override;

  // Selected optional functions.
  void beginJob() override;

private:

  // General event information
  int fRun;
  int fSubRun;
  unsigned int fEventID;

  // Comparison TTrees
  TTree *fTPComparisonTree;
  TTree *fTAComparisonTree;
  TTree *fTCComparisonTree;
  
  // Variables for comparison
  long unsigned int fTPSizeOffline, fTPSizeOnline;
  long unsigned int fTASizeOffline, fTASizeOnline;
  long unsigned int fTCSizeOffline, fTCSizeOnline;
  std::vector<bool> fTPComparison, fTAComparison, fTCComparison;

  art::InputTag tp_tag_, ta_tag_, tc_tag_;
  art::InputTag daq_tag;
  int verbosity_;

  // Trigger information from LArSoft (offline trigger) and DAQ (online trigger)
  std::vector<dunedaq::trgdataformats::TriggerPrimitive> fTriggerPrimitive, fTriggerPrimitiveDAQ;
  std::vector<dunedaq::trgdataformats::TriggerActivityData> fTriggerActivity, fTriggerActivityDAQ;
  std::vector<dunedaq::trgdataformats::TriggerCandidateData> fTriggerCandidate, fTriggerCandidateDAQ;
};


duneana::TriggerTPCInfoComparator::TriggerTPCInfoComparator(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  // ,
  , tp_tag_(p.get<art::InputTag>("tp_tag"))
  , ta_tag_(p.get<art::InputTag>("ta_tag"))
  , tc_tag_(p.get<art::InputTag>("tc_tag"))
  , daq_tag(p.get<art::InputTag>("daq_tag"))
  , verbosity_(p.get<int>("verbosity",0))
{
  // Consumes offline trigger information
  consumes<std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(tp_tag_);
  consumes<std::vector<dunedaq::trgdataformats::TriggerActivityData> >(ta_tag_);
  consumes<std::vector<dunedaq::trgdataformats::TriggerCandidateData> >(tc_tag_);

  // Consumes DAQ trigger information
  consumes<std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(daq_tag);
  consumes<std::vector<dunedaq::trgdataformats::TriggerActivityData> >(daq_tag);
  consumes<std::vector<dunedaq::trgdataformats::TriggerCandidateData> >(daq_tag);
}

void duneana::TriggerTPCInfoComparator::analyze(art::Event const& e)
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

  // Take TCs from event
  auto tc_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerCandidateData> >(tc_tag_);
  fTriggerCandidate = *tc_handle;

  // Take DAQ TPs from event
  auto daq_tp_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerPrimitive> >(daq_tag);
  fTriggerPrimitiveDAQ = *daq_tp_handle;

  // Take DAQ TAs from event
  auto daq_ta_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerActivityData> >(daq_tag);
  fTriggerActivityDAQ = *daq_ta_handle;

  // Take DAQ TCs from event
  auto daq_tc_handle = e.getValidHandle< std::vector<dunedaq::trgdataformats::TriggerCandidateData> >(daq_tag);
  fTriggerCandidateDAQ = *daq_tc_handle;

  // Load the geometry service
  art::ServiceHandle<geo::Geometry> geom;

  if(verbosity_ >= Verbosity::kInfo)
  {
    std::cout << "Offline summary" << std::endl; 
    std::cout << "Found " << fTriggerPrimitive.size() << " TPs" << std::endl;
    std::cout << "Found " << fTriggerActivity.size() << " TAs" << std::endl;
    std::cout << "Found " << fTriggerCandidate.size() << " TCs" << std::endl << std::endl;

    std::cout << "DAQ summary" << std::endl;
    std::cout << "Found " << fTriggerPrimitiveDAQ.size() << " TPs" << std::endl;
    std::cout << "Found " << fTriggerActivityDAQ.size() << " TAs" << std::endl;
    std::cout << "Found " << fTriggerCandidateDAQ.size() << " TCs" << std::endl;
  }

  // Compare offline and online TPs
  fTPSizeOffline = fTriggerPrimitive.size();
  fTPSizeOnline = fTriggerPrimitiveDAQ.size();
  long unsigned int minTPSize = std::min(fTPSizeOffline, fTPSizeOnline);
  fTPComparison.resize(minTPSize);
  for (long unsigned int i = 0; i < minTPSize; i++) {
    bool foundMatch = false; //  to check if a match is found
    for (long unsigned int j = 0; j < fTriggerPrimitiveDAQ.size(); j++) {
      // Compare TPs and set comparison result
      if (fTriggerPrimitive[i].channel == fTriggerPrimitiveDAQ[j].channel &&
	  fTriggerPrimitive[i].time_start == fTriggerPrimitiveDAQ[j].time_start &&
	  fTriggerPrimitive[i].time_over_threshold == fTriggerPrimitiveDAQ[j].time_over_threshold &&
	  fTriggerPrimitive[i].time_peak == fTriggerPrimitiveDAQ[j].time_peak &&
	  fTriggerPrimitive[i].adc_integral == fTriggerPrimitiveDAQ[j].adc_integral &&
	  fTriggerPrimitive[i].adc_peak == fTriggerPrimitiveDAQ[j].adc_peak &&
	  fTriggerPrimitive[i].detid == fTriggerPrimitiveDAQ[j].detid &&
	  fTriggerPrimitive[i].type == fTriggerPrimitiveDAQ[j].type &&
	  fTriggerPrimitive[i].algorithm == fTriggerPrimitiveDAQ[j].algorithm) {
	foundMatch = true;
	break; // Break the loop if a match is found
      }
    }
    fTPComparison[i] = foundMatch; // Set comparison result
  }

  // Compare offline and online TAs
  fTASizeOffline = fTriggerActivity.size();
  fTASizeOnline = fTriggerActivityDAQ.size();
  long unsigned int minTASize = std::min(fTASizeOffline, fTASizeOnline);
  fTAComparison.resize(minTASize);
  for (long unsigned int i = 0; i < minTASize; i++) {
    bool foundMatch = false; //  to check if a match is found
    for(long unsigned int j = 0; j < fTriggerActivityDAQ.size(); j++) {
      // Compare TAs and set comparison result
      if (fTriggerActivity[i].channel_start == fTriggerActivityDAQ[j].channel_start &&
          fTriggerActivity[i].channel_end == fTriggerActivityDAQ[j].channel_end &&
          fTriggerActivity[i].channel_peak == fTriggerActivityDAQ[j].channel_peak &&
          fTriggerActivity[i].time_start == fTriggerActivityDAQ[j].time_start &&
          fTriggerActivity[i].time_end == fTriggerActivityDAQ[j].time_end &&
          fTriggerActivity[i].time_peak == fTriggerActivityDAQ[j].time_peak &&
          fTriggerActivity[i].time_activity == fTriggerActivityDAQ[j].time_activity &&
          fTriggerActivity[i].adc_integral == fTriggerActivityDAQ[j].adc_integral &&
          fTriggerActivity[i].adc_peak == fTriggerActivityDAQ[j].adc_peak &&
          fTriggerActivity[i].detid == fTriggerActivityDAQ[j].detid &&
          fTriggerActivity[i].type == fTriggerActivityDAQ[j].type &&
          fTriggerActivity[i].algorithm == fTriggerActivityDAQ[j].algorithm) {
        foundMatch = true;
        break; // Break the loop if a match is found
      }
    }
    fTAComparison[i] = foundMatch; // Set comparison result
  }

  // Compare offline and online TCs 
  fTCSizeOffline = fTriggerCandidate.size();
  fTCSizeOnline = fTriggerCandidateDAQ.size();
  long unsigned int minTCSize = std::min(fTCSizeOffline, fTCSizeOnline);
  fTCComparison.resize(minTCSize);
  for (long unsigned int i = 0; i < minTCSize; i++) {
    bool foundMatch = false; //  to check if a match is found
    for (long unsigned int j = 0; j < fTriggerCandidateDAQ.size(); j++) {
      // Compare TCs and set comparison result
      if (fTriggerCandidate[i].time_start == fTriggerCandidateDAQ[j].time_start &&
          fTriggerCandidate[i].time_end == fTriggerCandidateDAQ[j].time_end &&
          fTriggerCandidate[i].time_candidate == fTriggerCandidateDAQ[j].time_candidate &&
          fTriggerCandidate[i].version == fTriggerCandidateDAQ[j].version &&
          fTriggerCandidate[i].detid == fTriggerCandidateDAQ[j].detid &&
          fTriggerCandidate[i].type == fTriggerCandidateDAQ[j].type &&
          fTriggerCandidate[i].algorithm == fTriggerCandidateDAQ[j].algorithm) {
        foundMatch = true;
        break; // Break the loop if a match is found
      }
    }
    fTCComparison[i] = foundMatch; // Set comparison result
  }

  // Fill the comparison TTrees
  fTPComparisonTree->Fill();
  fTAComparisonTree->Fill();
  fTCComparisonTree->Fill();
}

void duneana::TriggerTPCInfoComparator::beginJob()
{
  // Make our handle to the TFileService
  art::ServiceHandle<art::TFileService> tfs;

  // Create offline vs online comparison TTrees
  fTPComparisonTree = tfs->make<TTree>("TPComparisonTree", "Offline vs Online TP Comparison");
  fTAComparisonTree = tfs->make<TTree>("TAComparisonTree", "Offline vs Online TA Comparison");
  fTCComparisonTree = tfs->make<TTree>("TCComparisonTree", "Offline vs Online TC Comparison");

  // Set branch addresses for comparison TTrees
  fTPComparisonTree->Branch("Event" , &fEventID, "Event/I");
  fTPComparisonTree->Branch("Run"   , &fRun    , "Run/I");
  fTPComparisonTree->Branch("SubRun", &fSubRun , "SubRun/I");

  fTPComparisonTree->Branch("SizeOffline", &fTPSizeOffline, "SizeOffline/I");
  fTPComparisonTree->Branch("SizeOnline", &fTPSizeOnline, "SizeOnline/I");
  fTPComparisonTree->Branch("Comparison", &fTPComparison);

  fTAComparisonTree->Branch("Event" , &fEventID, "Event/I");
  fTAComparisonTree->Branch("Run"   , &fRun    , "Run/I");
  fTAComparisonTree->Branch("SubRun", &fSubRun , "SubRun/I");

  fTAComparisonTree->Branch("SizeOffline", &fTASizeOffline, "SizeOffline/I");
  fTAComparisonTree->Branch("SizeOnline", &fTASizeOnline, "SizeOnline/I");
  fTAComparisonTree->Branch("Comparison", &fTAComparison);

  fTCComparisonTree->Branch("Event" , &fEventID, "Event/I");
  fTCComparisonTree->Branch("Run"   , &fRun    , "Run/I");
  fTCComparisonTree->Branch("SubRun", &fSubRun , "SubRun/I");

  fTCComparisonTree->Branch("SizeOffline", &fTCSizeOffline, "SizeOffline/I");
  fTCComparisonTree->Branch("SizeOnline", &fTCSizeOnline, "SizeOnline/I");
  fTCComparisonTree->Branch("Comparison", &fTCComparison);

}

DEFINE_ART_MODULE(duneana::TriggerTPCInfoComparator)
