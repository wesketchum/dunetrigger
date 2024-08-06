#ifndef dunetrigger_TriggerSim_TCAlgTools_TCAlgTPCOnline_hh
#define dunetrigger_TriggerSim_TCAlgTools_TCAlgTPCOnline_hh

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerCandidateData.hpp"
#include "fhiclcpp/ParameterSet.h"

#include "dunetrigger/TriggerSim/TCAlgTools/TCAlgTPCTool.hh"

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateMaker.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidate.hpp"

#include <string>
#include <nlohmann/json.hpp>

namespace duneana {
    class TCAlgTPCOnline : public TCAlgTPCTool {
        public:
            explicit TCAlgTPCOnline(fhicl::ParameterSet const& ps)
            : algname(ps.get<std::string>("algname")),
            algconfig(nlohmann::json::parse(ps.get<std::string>("algconfig")))
            {
                alg->configure(algconfig);
                initialize();
            }

        private:
            std::string algname;
            nlohmann::json algconfig;
            std::vector<triggeralgs::TriggerCandidate> tcs_temp;


            std::shared_ptr<triggeralgs::AbstractFactory<triggeralgs::TriggerCandidateMaker>> tf = triggeralgs::TriggerCandidateFactory::get_instance();

            std::unique_ptr<triggeralgs::TriggerCandidateMaker> alg = tf->build_maker(algname);

            void initialize() override;
            void process_ta(dunedaq::trgdataformats::TriggerActivityData const& ta, std::vector<dunedaq::trgdataformats::TriggerCandidateData>& tcs_out) override;
    };

    void TCAlgTPCOnline::initialize(){
        // clear the temporary TC storage
        tcs_temp = {};
    }

    void TCAlgTPCOnline::process_ta(dunedaq::trgdataformats::TriggerActivityData const& ta, std::vector<dunedaq::trgdataformats::TriggerCandidateData>& tcs_out) {
        using std::vector;
        using triggeralgs::TriggerCandidate;
        using dunedaq::trgdataformats::TriggerCandidateData;

        // triggeralgs wants the derived type TriggerActivity, whereas we have
        // the base type TriggerActivityData, so we cast this
        // NOTE we REALLY should change which type we use to match the online
        // interface
        // TODO verify that the extra field inputs in TriggerActivity isn't used
        const triggeralgs::TriggerActivity& t = static_cast<const triggeralgs::TriggerActivity&>(ta);

        alg->operator()(t, tcs_temp);
        // todo verify that we should do this here and not on intialization
        // (likely by comparing runs)
        for(triggeralgs::TriggerCandidate t : tcs_temp){
            tcs_out.push_back(static_cast<TriggerCandidateData>(t));
        }
    };
}

#endif