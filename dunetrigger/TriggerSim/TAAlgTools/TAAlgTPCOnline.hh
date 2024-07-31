#ifndef dunetrigger_TriggerSim_TAAlgTools_TAAlgTPCOnline_hh
#define dunetrigger_TriggerSim_TAAlgTools_TAAlgTPCOnline_hh

#include "fhiclcpp/ParameterSet.h"

#include "dunetrigger/TriggerSim/TAAlgTools/TAAlgTPCTool.hh"

#include "dunetriggeralgs/TriggerActivityMaker.hpp"
#include "dunetriggeralgs/TriggerActivityFactory.hpp"


#include <string>
#include <nlohmann/json.hpp>

namespace duneana {
    class TAAlgTPCOnline : public TAAlgTPCTool {
        public:
            explicit TAAlgTPCOnline(fhicl::ParameterSet const& ps)
            : algname(ps.get<std::string>("algname")),
            algconfig(nlohmann::json::parse(ps.get<std::string>("algconfig")))
            {
                initialize();
            }

        private:
            //std::unique_ptr<triggeralgs::TriggerActivityMaker> alg;
            std::unique_ptr<triggeralgs::TriggerActivityMaker> alg;

            std::string algname;
            nlohmann::json algconfig;

            void initialize() override;
            void process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp, std::vector<TriggerActivity>& tas_out) override;
    };

    void TAAlgTPCOnline::initialize(){
        using triggeralgs::TriggerActivityFactory;

        auto tf = TriggerActivityFactory::get_instance();
        alg = tf->build_maker(algname);
        // parse the configuration json obtained from the parameter set as a
        // string and pass it to the algorithm
        alg->configure(algconfig);
    }

    void TAAlgTPCOnline::process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp, std::vector<TriggerActivity>& tas_out) {
        using std::vector;
        // This might break things
        // in theory these are the same data structure but with different fully
        // qualified names, so we can reinterpret_cast between them
        vector<triggeralgs::TriggerActivity>& tas = reinterpret_cast<vector<triggeralgs::TriggerActivity>&>(tas_out);
        // now call the operator to process the tp
        alg->operator()(*tp, tas);
    };
}

#endif