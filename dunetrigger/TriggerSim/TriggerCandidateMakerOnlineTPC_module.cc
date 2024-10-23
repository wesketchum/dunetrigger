////////////////////////////////////////////////////////////////////////
// Class:       TriggerCandidateMakerOnlineTPC
// Plugin Type: producer (Unknown Unknown)
// File:        TriggerCandidateMakerOnlineTPC_module.cc
//
// Generated at Tue Aug 13 16:43:32 2024 by ddrobner using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "canvas/Persistency/Common/types.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "larcore/Geometry/Geometry.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidate.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerCandidateData.hpp"

#include "dunetrigger/TriggerSim/Verbosity.hh"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

namespace triggeralgs {
  struct ExtTriggerActivity : public TriggerActivity
  {
    ExtTriggerActivity(const dunedaq::trgdataformats::TriggerActivityData& data) {
      this->time_start = data.time_start;
      this->time_end = data.time_end;
      this->time_peak = data.time_peak;
      this->time_activity = data.time_activity;
      this->channel_start = data.channel_start;
      this->channel_end = data.channel_end;
      this->channel_peak = data.channel_peak;
      this->adc_integral = data.adc_integral;
      this->adc_peak = data.adc_peak;
      this->detid = data.detid;
      this->type = data.type;
      this->algorithm = data.algorithm;
      this->version = data.version;
    }
  };
}

namespace duneana {
class TriggerCandidateMakerOnlineTPC;
}

class duneana::TriggerCandidateMakerOnlineTPC : public art::EDProducer {
public:
  explicit TriggerCandidateMakerOnlineTPC(fhicl::ParameterSet const &p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerCandidateMakerOnlineTPC(TriggerCandidateMakerOnlineTPC const &) =
      delete;
  TriggerCandidateMakerOnlineTPC(TriggerCandidateMakerOnlineTPC &&) = delete;
  TriggerCandidateMakerOnlineTPC &
  operator=(TriggerCandidateMakerOnlineTPC const &) = delete;
  TriggerCandidateMakerOnlineTPC &
  operator=(TriggerCandidateMakerOnlineTPC &&) = delete;

  void beginJob() override;
  void produce(art::Event &e) override;

private:
  art::InputTag ta_tag;
  std::string algname;
  fhicl::ParameterSet algconfig;

  std::shared_ptr<
      triggeralgs::AbstractFactory<triggeralgs::TriggerCandidateMaker>>
      alg_factory = triggeralgs::TriggerCandidateFactory::get_instance();
  std::unique_ptr<triggeralgs::TriggerCandidateMaker> alg;

  int verbosity;


  typedef std::pair<size_t, dunedaq::trgdataformats::TriggerActivityData>
      TriggerActivityIdx;

  static bool compareTriggerActivity(
      const dunedaq::trgdataformats::TriggerActivityData &ta1,
      const dunedaq::trgdataformats::TriggerActivityData &ta2) {
    return (ta1.time_start < ta2.time_start);
  }

  static bool
  isTAEqual(const dunedaq::trgdataformats::TriggerActivityData &ta1,
            const dunedaq::trgdataformats::TriggerActivityData &ta2) {
    return (ta1.detid == ta2.detid && ta1.channel_peak == ta2.channel_peak &&
            ta1.time_peak == ta2.time_peak &&
            ta1.adc_integral == ta2.adc_integral);
  }
};

duneana::TriggerCandidateMakerOnlineTPC::TriggerCandidateMakerOnlineTPC(
    fhicl::ParameterSet const &p)
    : EDProducer{p}, ta_tag(p.get<art::InputTag>("ta_tag")),
      algname(p.get<std::string>("algorithm")),
      algconfig(p.get<fhicl::ParameterSet>("algconfig")),
      verbosity(p.get<int>("verbosity", 0)) {
  using dunedaq::trgdataformats::TriggerActivityData;
  using dunedaq::trgdataformats::TriggerCandidateData;

  consumes<std::vector<TriggerActivityData>>(ta_tag);
  produces<std::vector<TriggerCandidateData>>();
  produces<art::Assns<TriggerCandidateData, TriggerActivityData>>();
}

void duneana::TriggerCandidateMakerOnlineTPC::beginJob() {
  // build alg using the factory
  alg = alg_factory->build_maker(algname);

  // parse the parameterset as json
  nlohmann::json alg_json;
  for (auto &k : algconfig.get_all_keys()) {
    try {
      alg_json[k] = algconfig.get<uint64_t>(k);
    }
    catch (const fhicl::exception& e) {
      try {
	// If false, try retrieving the parameter as a boolean
	alg_json[k] = algconfig.get<bool>(k);
      }
      catch (const fhicl::exception& e) {
	std::cerr << "Error: FHiCL parameter is neither an int nor a bool in the FHiCL file. \n";
      }
    }
  }
  
  // and pass that on to the trigger algorithm
  alg->configure(alg_json);
}

void duneana::TriggerCandidateMakerOnlineTPC::produce(art::Event &e) {
  using dunedaq::trgdataformats::TriggerActivityData;
  using dunedaq::trgdataformats::TriggerCandidateData;
  // get a handle on the art geometry service
  art::ServiceHandle<geo::Geometry> geo;

  // get a handle on the TAs and dereference it
  auto ta_handle = e.getValidHandle<std::vector<TriggerActivityData>>(ta_tag);
  std::vector<TriggerActivityData> ta_vec = *ta_handle;

  // some unique pointers to new vectors for the data products
  auto tc_vec_ptr = std::make_unique<std::vector<TriggerCandidateData>>();
  auto ta_in_tc_assn_ptr =
      std::make_unique<art::Assns<TriggerCandidateData, TriggerActivityData>>();

  // PtrMaker to make pointers to the new TCs
  art::PtrMaker<TriggerCandidateData> tc_ptr_maker{e};

  // create a vector of inputs with the 'file' index of the TA
  std::vector<TriggerActivityIdx> input_tas;//(ta_vec.size());
  for (size_t i = 0; i < ta_vec.size(); ++i) {
    input_tas.push_back(TriggerActivityIdx(i, ta_vec.at(i)));
  }

  // now we sort the TAs by time using a lambda as the comparison function which
  // pulls out the second element of the pair. Alternatively, we can rewrite the
  // comparison function to use the pairs (although that makes it less versatile)
  std::sort(input_tas.begin(), input_tas.end(),
            [](TriggerActivityIdx &ta1, TriggerActivityIdx &ta2) {
              return compareTriggerActivity(ta1.second, ta2.second);
            });

  // I don't really think there's a better way to do this
  // because my understanding is that the art::Ptrs work
  // by effectively pointing to an index in the file object
  // meaning we have to preserve that index somehow

  // if there's a better way to do this, I'd love to know it though

  // create a vector of online TCs for the online algorithm to store it's
  // outputs in
  std::vector<triggeralgs::TriggerCandidate> produced_tcs = {};
  
  // process the input TAs
  for (const auto &ta : input_tas) {
    dunedaq::trgdataformats::TriggerActivityData ta_data = ta.second;
    triggeralgs::ExtTriggerActivity ta_ext(ta_data);
    (*alg)(ta_ext, produced_tcs);
  }

  // now we need to handle the associations
  for (auto out_tc : produced_tcs) {
    // make an art pointer to the tc vector
    auto const tcPtr = tc_ptr_maker(tc_vec_ptr->size());

    // emplace the new tc onto the back of it
    tc_vec_ptr->emplace_back(out_tc);

    // now make a PtrVector for the associations
    art::PtrVector<TriggerActivityData> tas_in_tc_ptr;

    // and now get the TAs out of the processed TC :)
    for (const TriggerActivityData &in_ta : out_tc.inputs) {

      // get an iterator to the first matching TA
      // here we need to use a lambda to pull the second element out of the
      // <size_t, TriggerActivityData> pair
      std::vector<TriggerActivityIdx>::iterator ta_it = std::find_if(
          input_tas.begin(), input_tas.end(),
          [&](TriggerActivityIdx &t) { return isTAEqual(in_ta, t.second); });
      // stop when there are no more matches
      while (ta_it != input_tas.end()) {
        // push back the matching ptr
        tas_in_tc_ptr.push_back(
            art::Ptr<TriggerActivityData>(ta_handle, ta_it->first));
        // and get an iterator pointing to the next match
        ta_it =
            std::find_if(++ta_it, input_tas.end(), [&](TriggerActivityIdx &t) {
              return isTAEqual(in_ta, t.second);
            });
      }

      // pritn a debug message if we can't find and associated TAs
      if(tas_in_tc_ptr.empty() && verbosity >= Verbosity::kDebug ){
        std::cout << "No associated TAs found for TC!" << std::endl;
      }
    }
    // add the associations
    ta_in_tc_assn_ptr->addMany(tcPtr, tas_in_tc_ptr);
  }

  if (verbosity >= Verbosity::kInfo) {
    std::cout << "Created " << produced_tcs.size() << " TCs" << std::endl;
  }

  // move the produced things onto the event 
  e.put(std::move(tc_vec_ptr));
  e.put(std::move(ta_in_tc_assn_ptr));
}

DEFINE_ART_MODULE(duneana::TriggerCandidateMakerOnlineTPC)
