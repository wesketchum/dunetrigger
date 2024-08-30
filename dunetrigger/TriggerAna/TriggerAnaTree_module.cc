////////////////////////////////////////////////////////////////////////
// Class:       TriggerAnaTree
// Plugin Type: analyzer (Unknown Unknown)
// File:        TriggerAnaTree_module.cc
//
// Generated at Fri Aug 30 14:50:19 2024 by jierans using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileDirectory.h"
#include "art_root_io/TFileService.h"


#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerCandidateData.hpp"
#include "detdataformats/trigger/TriggerPrimitive.hpp"

#include <TDirectory.h>
#include <TFile.h>
#include <TTree.h>

using dunedaq::trgdataformats::TriggerPrimitive;
using dunedaq::trgdataformats::TriggerActivityData;
using dunedaq::trgdataformats::TriggerCandidateData;

class TriggerAnaTree;
class TriggerAnaTree : public art::EDAnalyzer {
public:
  explicit TriggerAnaTree(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerAnaTree(TriggerAnaTree const&) = delete;
  TriggerAnaTree(TriggerAnaTree&&) = delete;
  TriggerAnaTree& operator=(TriggerAnaTree const&) = delete;
  TriggerAnaTree& operator=(TriggerAnaTree&&) = delete;

  // Required functions.
  void beginJob() override;
  void analyze(art::Event const& e) override;
  // void endJob() override;

private:
  art::ServiceHandle<art::TFileService> tfs;
  std::map<std::string, TTree*> tree_map;
  // buffers for writing to ROOT Trees
  std::map<std::string, dunedaq::trgdataformats::TriggerPrimitive> tp_bufs;
  std::map<std::string, dunedaq::trgdataformats::TriggerActivityData> ta_bufs;
  std::map<std::string, dunedaq::trgdataformats::TriggerCandidateData> tc_bufs;

  void make_tp_tree_if_needed(std::string tag);
  void make_ta_tree_if_needed(std::string map_tag);
  void make_tc_tree_if_needed(std::string map_tag);
};

TriggerAnaTree::TriggerAnaTree(fhicl::ParameterSet const& p)
  : EDAnalyzer{p}  // ,
  // More initializers here.
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void TriggerAnaTree::beginJob() {
}

void TriggerAnaTree::analyze(art::Event const& e)
{

  std::vector<art::Handle<std::vector<TriggerPrimitive>>> tpHandles = e.getMany<std::vector<TriggerPrimitive>>();
  std::vector<art::Handle<std::vector<TriggerActivityData>>> taHandles = e.getMany<std::vector<TriggerActivityData>>();
  std::vector<art::Handle<std::vector<TriggerCandidateData>>> tcHandles = e.getMany<std::vector<TriggerCandidateData>>();

  for (auto const &tpHandle : tpHandles){
    std::string tag = tpHandle.provenance()->inputTag().encode();
    std::string map_tag = "tp/" + tag;
    make_tp_tree_if_needed(tag);
    for (const TriggerPrimitive &tp : *tpHandle) {
      tp_bufs[map_tag] = tp;
      tree_map[map_tag]->Fill();
    }
  }
  for (auto const &taHandle : taHandles){
    std::string tag = taHandle.provenance()->inputTag().encode();
    std::string map_tag = "ta/" + tag;
    make_ta_tree_if_needed(tag);
    for (const TriggerActivityData &ta : *taHandle) {
      ta_bufs[map_tag] = ta;
      tree_map[map_tag]->Fill();
    }
  }
  for (auto const &tcHandle : tcHandles){
    std::string tag = tcHandle.provenance()->inputTag().encode();
    std::string map_tag = "tc/" + tag;
    make_tc_tree_if_needed(tag);
    for (const TriggerCandidateData &tc : *tcHandle) {
      tc_bufs[map_tag] = tc;
      tree_map[map_tag]->Fill();
    }
  }
}

void TriggerAnaTree::make_tp_tree_if_needed(std::string tag) {
  std::string map_tag = "tp/" + tag;
  if (!tree_map.count(map_tag)) {
    art::TFileDirectory tp_dir =
        tfs->mkdir("TriggerPrimitives", "Trigger Primitive Trees");
    std::cout << "Creating new TTree for " << tag << std::endl;

    // Replace ":" with "_" in TTree names so that they can be used in ROOT's intepreter
    std::string tree_name = tag;
    std::replace(tree_name.begin(), tree_name.end(), ':', '_');
    TTree* tree = tp_dir.make<TTree>(tree_name.c_str(), tree_name.c_str());
    tree_map[map_tag] = tree;
    TriggerPrimitive &tp = tp_bufs[map_tag];
    tree->Branch("version", &tp.version);
    tree->Branch("time_start", &tp.time_start);
    tree->Branch("time_peak", &tp.time_peak);
    tree->Branch("time_over_threshold", &tp.time_over_threshold);
    tree->Branch("channel", &tp.channel);
    tree->Branch("adc_integral", &tp.adc_integral);
    tree->Branch("adc_peak", &tp.adc_peak);
    tree->Branch("detid", &tp.detid);
    tree->Branch("type", &tp.type, "type/I");
    tree->Branch("algorithm", &tp.algorithm, "algorithm/I");
  }
}

void TriggerAnaTree::make_ta_tree_if_needed(std::string tag) {
  std::string map_tag = "ta/" + tag;
  if (!tree_map.count(map_tag)) {
    art::TFileDirectory ta_dir =
        tfs->mkdir("TriggerActivities", "Trigger Activity Trees");
    std::cout << "Creating new TTree for " << tag << std::endl;
    // Replace ":" with "_" in TTree names so that they can be used in ROOT's intepreter
    std::string tree_name = tag;
    std::replace(tree_name.begin(), tree_name.end(), ':', '_');
    TTree *tree = ta_dir.make<TTree>(tree_name.c_str(), tree_name.c_str());
    tree_map[map_tag] = tree;
    TriggerActivityData &ta = ta_bufs[map_tag];
    tree->Branch("version", &ta.version);
    tree->Branch("time_start", &ta.time_start);
    tree->Branch("time_end", &ta.time_end);
    tree->Branch("time_peak", &ta.time_peak);
    tree->Branch("time_activity", &ta.time_activity);
    tree->Branch("channel_start", &ta.channel_start);
    tree->Branch("channel_end", &ta.channel_end);
    tree->Branch("channel_peak", &ta.channel_peak);
    tree->Branch("adc_integral", &ta.adc_integral);
    tree->Branch("adc_peak", &ta.adc_peak);
    tree->Branch("detid", &ta.detid);
    // HACK: assuming enums are ints here
    tree->Branch("type", &ta.type, "type/I");
    tree->Branch("algorithm", &ta.algorithm, "algorithm/I");
  }
}

void TriggerAnaTree::make_tc_tree_if_needed(std::string tag) {
  std::string map_tag = "tc/" + tag;
  if (!tree_map.count(map_tag)) {
    art::TFileDirectory tc_dir =
        tfs->mkdir("TriggerCandidates", "Trigger Candidate Trees");
    std::cout << "Creating new TTree for " << tag << std::endl;
    // Replace ":" with "_" in TTree names so that they can be used in ROOT's intepreter
    std::string tree_name = tag;
    std::replace(tree_name.begin(), tree_name.end(), ':', '_');
    TTree *tree = tc_dir.make<TTree>(tree_name.c_str(), tree_name.c_str());
    tree_map[map_tag] = tree;
    TriggerCandidateData &tc = tc_bufs[map_tag];
    tree->Branch("version", &tc.version);
    tree->Branch("time_start", &tc.time_start);
    tree->Branch("time_end", &tc.time_end);
    tree->Branch("time_candidate", &tc.time_candidate);
    tree->Branch("detid", &tc.detid);
    tree->Branch("type", &tc.type, "type/I");
    tree->Branch("algorithm", &tc.algorithm, "type/I");
  }
}

DEFINE_ART_MODULE(TriggerAnaTree)

