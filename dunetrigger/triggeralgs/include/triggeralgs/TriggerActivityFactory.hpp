/* @file: TriggerActivityFactory.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2023.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_TRIGGER_ACTIVITY_FACTORY_HPP_
#define TRIGGERALGS_TRIGGER_ACTIVITY_FACTORY_HPP_

#include "dunetrigger/triggeralgs/include/triggeralgs/TriggerActivityMaker.hpp"
#include "dunetrigger/triggeralgs/include/triggeralgs/AbstractFactory.hpp"

#define REGISTER_TRIGGER_ACTIVITY_MAKER(tam_name, tam_class)                                                                                      \
  static struct tam_class##Registrar {                                                                                                            \
    tam_class##Registrar() {                                                                                                                      \
      TriggerActivityFactory::register_creator(tam_name, []() -> std::unique_ptr<TriggerActivityMaker> {return std::make_unique<tam_class>();});   \
    }                                                                                                                                             \
  } tam_class##_registrar;

namespace triggeralgs {

class TriggerActivityFactory : public AbstractFactory<TriggerActivityMaker> {};

} /* namespace triggeralgs */

#endif // TRIGGERALGS_TRIGGER_ACTIVITY_FACTORY_HPP_
