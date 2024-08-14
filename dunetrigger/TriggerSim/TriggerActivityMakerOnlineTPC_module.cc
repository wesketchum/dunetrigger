////////////////////////////////////////////////////////////////////////
// Class:       TriggerActivityMakerOnlineTPC
// Plugin Type: producer (Unknown Unknown)
// File:        TriggerActivityMakerOnlineTPC_module.cc
//
// Generated at Tue Aug 13 11:03:40 2024 by ddrobner using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "larcore/Geometry/Geometry.h"
#include "larcoreobj/SimpleTypesAndConstants/readout_types.h"

#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "detdataformats/trigger/TriggerActivityData.hpp"

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivity.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <cassert>

namespace duneana {
  class TriggerActivityMakerOnlineTPC;
}


class duneana::TriggerActivityMakerOnlineTPC : public art::EDProducer {
public:
  explicit TriggerActivityMakerOnlineTPC(fhicl::ParameterSet const& p);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  TriggerActivityMakerOnlineTPC(TriggerActivityMakerOnlineTPC const&) = delete;
  TriggerActivityMakerOnlineTPC(TriggerActivityMakerOnlineTPC&&) = delete;
  TriggerActivityMakerOnlineTPC& operator=(TriggerActivityMakerOnlineTPC const&) = delete;
  TriggerActivityMakerOnlineTPC& operator=(TriggerActivityMakerOnlineTPC&&) = delete;

  void beginJob() override;
  void produce(art::Event& e) override;

private:

  // fields for job configuration
  std::string algname;
  fhicl::ParameterSet algconfig;
  art::InputTag tp_tag;

  int verbosity;

  // defining this here so it isn't ugly to write every time
  // need it to store the index of the triggerprimitive to later make an art::Assn
  typedef std::pair<size_t, dunedaq::trgdataformats::TriggerPrimitive> TriggerPrimitiveIdx ;

  // by making the factory a class field it's theoretically possible to make a
  // new algorithm at any point
  // since we were discussing different algorithms for different planes, I think
  // this makes that relatively easy
  std::shared_ptr<triggeralgs::AbstractFactory<triggeralgs::TriggerActivityMaker>> tf = triggeralgs::TriggerActivityFactory::get_instance();
  // this part however, needs to be changed around if we do that
  std::unique_ptr<triggeralgs::TriggerActivityMaker> alg;

  // small function to compare tps by time
  static bool compareTriggerPrimitive(const TriggerPrimitiveIdx& tp1, const TriggerPrimitiveIdx& tp2)
    {return (tp1.second.time_start < tp2.second.time_start);}
  
  // little function to check equality of TPs
  // we can possible do with fewer conditions, but the most common ones are
  // first so it (theoretically) should immediately return when the first or
  // second is false
  static bool isTPEqual(const dunedaq::trgdataformats::TriggerPrimitive& tp1, const dunedaq::trgdataformats::TriggerPrimitive& tp2)
    {return (tp1.detid == tp2.detid && tp1.channel == tp2.channel && tp1.time_peak == tp2.time_peak && tp1.adc_integral == tp2.adc_integral);}

};


duneana::TriggerActivityMakerOnlineTPC::TriggerActivityMakerOnlineTPC(fhicl::ParameterSet const& p)
  : EDProducer{p},
  algname(p.get<std::string>("algorithm")),
  algconfig(p.get<fhicl::ParameterSet>("algconfig")),
  tp_tag(p.get<art::InputTag>("tp_tag")),
  verbosity(p.get<int>("verbosity", 1))
    // ,
  // More initializers here.
{
  // for compactness of the producer and consumer declarations
  using dunedaq::trgdataformats::TriggerPrimitive;
  using dunedaq::trgdataformats::TriggerActivityData;

  produces<std::vector<TriggerActivityData>>();
  produces<art::Assns<TriggerActivityData, TriggerPrimitive>>();
  consumes<std::vector<TriggerPrimitive>>(tp_tag);
}

void duneana::TriggerActivityMakerOnlineTPC::beginJob(){

  nlohmann::json algconfig_json;
  for(auto k : algconfig.get_all_keys()){
    // TODO handle possible different types
    // from what I've seen it's only uint64's and bools
    // but we can just use 0 and 1 for those
    algconfig_json[k] = algconfig.get<uint64_t>(k);
  }

  // build the TAMaker algorithm using the factory
  alg = tf->build_maker(algname);

  // TODO find out about LArSoft's actual error handling system
  // check if the algorithm is not a nullptr
  assert(alg != nullptr);

  // call the configure method on the trigger algorithm using the json object
  // previously parsed from the sub-parameterset in the job config
  alg->configure(algconfig_json);
}

void duneana::TriggerActivityMakerOnlineTPC::produce(art::Event& e)
{
  // these things end up getting written about 5000 times, so let's do this here 
  using dunedaq::trgdataformats::TriggerPrimitive;
  using dunedaq::trgdataformats::TriggerActivityData;

  // get a service handle for geometry
  art::ServiceHandle<geo::Geometry> geom;

  // unique ptrs to vectors for associations
  auto ta_vec_ptr = std::make_unique<std::vector<TriggerActivityData> >();
  auto tp_vec_ptr = std::make_unique<std::vector<TriggerPrimitive>>();
  auto tp_in_tas_assn_ptr = std::make_unique<art::Assns<TriggerActivityData, TriggerPrimitive>>();

  // ptrmaker to make an art::ptr for the assocs
  art::PtrMaker<dunedaq::trgdataformats::TriggerActivityData> taPtrMaker{e};

  // get a valid handle and dereference it to get TPs
  auto tpHandle = e.getValidHandle<std::vector<TriggerPrimitive>>(tp_tag);
  std::vector<TriggerPrimitive> tp_vec = *tpHandle;

  // in essence what we want to do here, is group the TPs by ROP and then sort
  // by time, but we need to keep the index in the original TP vector (and can't
  // make an art::ptr now so we lose that if we hand it off to the online algo)
  std::map<readout::ROPID, std::vector<TriggerPrimitiveIdx>> tp_by_rop;
  for(size_t i = 0; i < tp_vec.size(); ++i){
    readout::ROPID rop = geom->ChannelToROP(tp_vec.at(i).channel);
    tp_by_rop[rop].push_back(std::pair<size_t, TriggerPrimitive>(i, tp_vec.at(i)));
  }

  // now we process each ROP
  std::vector<triggeralgs::TriggerActivity> created_tas;
  for(auto& tps : tp_by_rop){
    //first we need to sort by time
    std::sort(tps.second.begin(), tps.second.end(), compareTriggerPrimitive);

    // and now loop through the TPs and create TAs
    for (auto& tp : tps.second){
      alg->operator()(tp.second, created_tas);
    }

    // now the fun part
    // to make the associations we have to use the idx we created earlier
    // BUT the input TA only has a list of the base objects in it
    // so we need to search through it and then get out our object with the
    // index intact to make the association
    for(auto const& out_ta : created_tas){
      // now we find the TPs which are in the output TA and create the
      // associations
      auto const taPtr = taPtrMaker(ta_vec_ptr->size());

      // add output ta to the ta dataproduct vector and create a PtrVector for
      // the TPs in the TA
      ta_vec_ptr->emplace_back(out_ta);
      art::PtrVector<TriggerPrimitive> tp_in_ta_ptrs;

      // now loop through each TP in the TA to handle associations
      for(auto& in_tp : out_ta.inputs){
        // get an iterator to matching TPs with find_if
        std::vector<TriggerPrimitiveIdx>::iterator tp_it 
        = std::find_if(tps.second.begin(), tps.second.end(),
        [&](TriggerPrimitiveIdx& t){return isTPEqual(t.second, in_tp);});

        // find_if will return tps.second.end() if none are found
        while(tp_it != tps.second.end()){
            // push back an art::Ptr pointing to the proper index in the vector
            // of input TPs
            tp_in_ta_ptrs.push_back(art::Ptr<TriggerPrimitive>(tpHandle, tp_it->first));
            // get an iterator to the next (if any) matching TP
            tp_it = std::find_if(++tp_it, tps.second.end(), 
              [&](TriggerPrimitiveIdx& t){return isTPEqual(t.second, in_tp);});
        }
      }
      // add the associations to the tp_in_ta assoc
      tp_in_tas_assn_ptr->addMany(taPtr, tp_in_ta_ptrs);
    }
  }
  // Move the TAs and Associations onto the event
  if(verbosity >= 1){
    std::cout << "Created " << created_tas.size() << " TAs" << std::endl;
    std::cout << "TA PtrVec Size " << ta_vec_ptr->size() << std::endl;
  }
  e.put(std::move(ta_vec_ptr));
  e.put(std::move(tp_in_tas_assn_ptr));
}

DEFINE_ART_MODULE(duneana::TriggerActivityMakerOnlineTPC)
