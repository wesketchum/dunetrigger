#pragma once

#include <fstream>

#include <atomic>

#include "TriggerCandidateMaker.hpp"
/// A simple class that just dumps every candidate it receives in a txt file, and wait random times in between

class NaiveTriggerCandidateConsumer
{
protected:
  std::ofstream file_out;
  std::atomic<bool> consume_on;
  int n_tc;
  NaiveTriggerQueue& tq;

public:
  NaiveTriggerCandidateConsumer(NaiveTriggerQueue& tq_)
    : tq(tq_)
    , consume_on(false)
    , n_tc(0)
  {
    file_out.open("candidate_dump.csv");
    file_out << "tstart,tend,tpeak,channel_start,channel_end,channel_peak,adc_integral,adc_peak,detid\n";
  }

  void Worker()
  {
    while (true) {
      DuneTriggers::TriggerCandidate tc;
      if (tq.GetNextProcessed(tc))
        PrintToFile(tc);

      if (not consume_on.load())
        break;
    }
  }

  void Start()
  {
    std::cout << "\033[32mStarting Candidate consumer\033[0m\n";
    consume_on.store(true);
  }

  void Stop()
  {
    std::cout << "\033[32mStopping Candidate consumer\033[0m\n";
    consume_on.store(false);
    DuneTriggers::TriggerCandidate tc;
    if (tq.tcm.GetLastCluster(tc))
      PrintToFile(tc);

    std::cout << "Created " << n_tc << " TCs.\n";
  }

  void PrintToFile(DuneTriggers::TriggerCandidate& tc)
  {
    n_tc++;
    std::string str =
      (std::to_string(tc.time_start) + "," + std::to_string(tc.time_end) + "," + std::to_string(tc.time_peak) + "," +
       std::to_string(tc.channel_start) + "," + std::to_string(tc.channel_end) + "," + std::to_string(tc.channel_peak) +
       "," + std::to_string(tc.adc_integral) + "," + std::to_string(tc.adc_peak) + "," + std::to_string(tc.detid) +
       "\n");
    file_out << str;
  }
};
