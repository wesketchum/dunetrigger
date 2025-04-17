/* @file: AbstractFactory.hpp
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2023.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#ifndef TRIGGERALGS_ABSTRACT_FACTORY_HPP_
#define TRIGGERALGS_ABSTRACT_FACTORY_HPP_

// #include "logging/Logging.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace triggeralgs {

template <typename T>
class AbstractFactory
{
  using maker_creator = std::function<std::unique_ptr<T>()>;
  using creation_map = std::unordered_map<std::string, maker_creator>;

  public:
    AbstractFactory() {}
    AbstractFactory(const AbstractFactory&) = delete;
    AbstractFactory& operator=(const AbstractFactory&) = delete;
    virtual ~AbstractFactory() {}

    std::unique_ptr<T> build_maker(const std::string& alg_name);

    static void register_creator(const std::string alg_name, maker_creator creator);

    static std::shared_ptr<AbstractFactory<T>> get_instance();

  protected:
    static std::shared_ptr<AbstractFactory<T>> s_single_factory;

  private:
    static creation_map& get_makers();
};

} /* namespace triggeralgs */

#include "dunetrigger/triggeralgs/include/triggeralgs/AbstractFactory.hxx"

#endif // TRIGGERALGS_ABSTRACT_FACTORY_HPP_
