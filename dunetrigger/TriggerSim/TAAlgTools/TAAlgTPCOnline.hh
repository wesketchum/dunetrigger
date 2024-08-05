#ifndef dunetrigger_TriggerSim_TAAlgTools_TAAlgTPCOnline_hh
#define dunetrigger_TriggerSim_TAAlgTools_TAAlgTPCOnline_hh

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "fhiclcpp/ParameterSet.h"

#include "dunetrigger/TriggerSim/TAAlgTools/TAAlgTPCTool.hh"

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityMaker.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityFactory.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivity.hpp"

#include <string>
#include <nlohmann/json.hpp>

namespace duneana {
    class TAAlgTPCOnline : public TAAlgTPCTool {
        public:
            explicit TAAlgTPCOnline(fhicl::ParameterSet const& ps)
            : algname(ps.get<std::string>("algname")),
            algconfig(nlohmann::json::parse(ps.get<std::string>("algconfig")))
            {
                alg->configure(algconfig);
                initialize();

            }

        private:
            //std::unique_ptr<triggeralgs::TriggerActivityMaker> alg;

            std::string algname;
            nlohmann::json algconfig;
            std::vector<triggeralgs::TriggerActivity> tas_temp;


            std::shared_ptr<triggeralgs::AbstractFactory<triggeralgs::TriggerActivityMaker>> tf = triggeralgs::TriggerActivityFactory::get_instance();

            std::unique_ptr<triggeralgs::TriggerActivityMaker> alg = tf->build_maker(algname);

            void initialize() override;
            void process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp, std::vector<TriggerActivity>& tas_out) override;
    };

    void TAAlgTPCOnline::initialize(){
        tas_temp = {};
    }

    void TAAlgTPCOnline::process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp, std::vector<TriggerActivity>& tas_out) {
        using std::vector;
        using dunedaq::trgdataformats::TriggerActivityData;

        alg->operator()(*tp, tas_temp);
        // TODO figure out the best way to do this
        // there is some weird mismatch between the data types
        // and we need to figure out how to handle this with minimal overhead 
        for(triggeralgs::TriggerActivity t : tas_temp){
            TriggerActivityData td = static_cast<TriggerActivityData>(t);
            TriggerActivity c;
            c.first = td;
            c.second.push_back(tp);
            tas_out.push_back(c);
        }
    };
}

#endif