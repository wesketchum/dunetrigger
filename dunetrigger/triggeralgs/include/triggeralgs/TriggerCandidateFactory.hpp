/* @file: TriggerCandidateFactory.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2023.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_TRIGGER_CANDIDATE_FACTORY_HPP_
#define TRIGGERALGS_TRIGGER_CANDIDATE_FACTORY_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerCandidateMaker.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/AbstractFactory.hpp"

#define REGISTER_TRIGGER_CANDIDATE_MAKER(tcm_name, tcm_class)                                                                                      \
  static struct tcm_class##Registrar {                                                                                                            \
    tcm_class##Registrar() {                                                                                                                      \
      TriggerCandidateFactory::register_creator(tcm_name, []() -> std::unique_ptr<TriggerCandidateMaker> {return std::make_unique<tcm_class>();});   \
    }                                                                                                                                             \
  } tcm_class##_registrar;

namespace triggeralgs {

class TriggerCandidateFactory : public AbstractFactory<TriggerCandidateMaker> {};

} /* namespace triggeralgs */

#endif // TRIGGERALGS_TRIGGER_CANDIDATE_FACTORY_HPP_
