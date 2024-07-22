#ifndef DUNETRIGGER_TRIGGERSIM_TAAlgTPCADCSimpleWindow_hh
#define DUNETRIGGER_TRIGGERSIM_TAAlgTPCADCSimpleWindow_hh

#include "detdataformats/trigger/TriggerActivityData.hpp"
#include "detdataformats/trigger/TriggerPrimitive.hpp"
#include "fhiclcpp/ParameterSet.h" 

#include "detdataformats/DetID.hpp"
#include "dunetrigger/TriggerSim/TAAlgTools/TAAlgTPCTool.hh"
#include "triggeralgs/ADCSimpleWindow/TriggerActivityMakerADCSimpleWindow.hpp"

namespace duneana {

  class TAAlgTPCADCSimpleWindow : public TAAlgTPCTool {

  public:
    explicit TAAlgTPCADCSimpleWindow(fhicl::ParameterSet const& ps)
      : multiplicity_(ps.get<size_t>("multiplicity"))
      , verbosity_(ps.get<int>("verbosity",0))
    {
      initialize();
    }
  
  private:
    typedef triggeralgs::TriggerActivityMaker TAMaker;
    typedef triggeralgs::TriggerActivityMakerADCSimpleWindow TAMakerADCSimpleWindow; 

     TAMaker* alg;
    
    void initialize() override
    {
      //triggeralgs::TriggerActivityMakerADCSimpleWindow alg = new triggeralgs; 
      ta_current_ = TriggerActivity();
      alg = new TAMakerADCSimpleWindow();
    }

    //process a single tp
    //if we have met some condition for making a TA, then add it to the output vector
    void process_tp(art::Ptr<dunedaq::trgdataformats::TriggerPrimitive> tp,
		            std::vector<TriggerActivity> & tas_out)
    {
      //TAMakerADCSimpleWindow::operator(alg)()
      std::vector<triggeralgs::TriggerActivity> tas;
      alg->operator()(*tp, tas);
      if(tas.size() != 0){
        std::cout << "Found " << tas.size() << " TAs" << std::endl;
      }
      for (auto t : tas){
        dunedaq::trgdataformats::TriggerActivityData td = static_cast<dunedaq::trgdataformats::TriggerActivityData>(t);
        TriggerActivity c;
        c.first = td;
        c.second.push_back(tp);
        tas_out.push_back(c);
      }
    }

    
  private:
    const size_t multiplicity_;
    const int verbosity_;

    TriggerActivity ta_current_;
    
  };
  
}

#endif
