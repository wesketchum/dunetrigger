/**
 * @file test_factory.cxx
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2023.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

// NOLINTNEXTLINE(build/define_used)
#define BOOST_TEST_MODULE boost_test_macro_overview

#include "triggeralgs/TriggerActivityFactory.hpp"
#include "triggeralgs/TriggerActivityMaker.hpp"

#include <boost/test/included/unit_test.hpp>

#include <iostream>
#include <vector>

using namespace dunedaq;

namespace triggeralgs {

BOOST_AUTO_TEST_CASE(test_macro_overview)
{
  std::unique_ptr<TriggerActivityMaker> prescale_maker = TriggerActivityFactory::makeTAMaker("TriggerActivityMakerPrescalePlugin");

  std::vector<TriggerActivity> prescale_ta;
  TriggerPrimitive some_tp;
  for (int idx = 0; idx < 10; idx++) {
    some_tp.type = TriggerPrimitive::Type::kTPC;
    some_tp.algorithm = TriggerPrimitive::Algorithm::kSimpleThreshold;
    some_tp.time_start = idx;
    some_tp.time_peak = 1+idx;
    some_tp.time_over_threshold = 2;
    some_tp.adc_integral = 1000+idx;
    some_tp.adc_peak = 1000+idx;
    some_tp.channel = 0+idx;
    some_tp.detid = 0;
    (*prescale_maker)(some_tp, prescale_ta);
  }

  bool same_alg = (prescale_ta[0].algorithm == TriggerActivity::Algorithm::kPrescale);

  BOOST_TEST(same_alg);
}

} /* namespace triggeralgs */
