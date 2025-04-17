#pragma once
#include <boost/lockfree/queue.hpp>

#include "dune-triggers/Trivial/TriggerCandidateMaker_Trivial.hpp"

class NaiveTriggerQueue
{
protected:
  boost::lockfree::queue<DuneTriggers::TriggerPrimitive> queue_in;
  boost::lockfree::queue<DuneTriggers::TriggerCandidate> queue_out;

  std::atomic<bool> process_on;

  bool bounded_push_with_warning(const DuneTriggers::TriggerPrimitive& dat)
  {
    if (not queue_in.bounded_push(dat)) {
      std::cout << "\033[31mWarning, queue_in is full, not filling!\033[0m\n";
      return false;
    }
    return true;
  }

public:
  DuneTriggers::TriggerCandidateMaker& tcm;
  NaiveTriggerQueue(DuneTriggers::TriggerCandidateMaker& tcm_, size_t depth_in = 100, size_t depth_out = 100)
    : queue_in(depth_in)
    , queue_out(depth_out)
    , process_on(false)
    , tcm(tcm_)
  {
    assert(queue_in.is_lock_free());
    assert(queue_out.is_lock_free());
  }

  void AddToQueue(const DuneTriggers::TriggerPrimitive& dat) { bounded_push_with_warning(dat); }

  void AddToQueue(const std::vector<DuneTriggers::TriggerPrimitive>& dats)
  {
    std::for_each(dats.begin(), dats.end(), [this](const DuneTriggers::TriggerPrimitive dat) -> void {
      this->bounded_push_with_warning(dat);
    });
  }

  void Worker()
  {
    while (true) {

      DuneTriggers::TriggerPrimitive tp;
      std::vector<DuneTriggers::TriggerCandidate> tcs;

      if (queue_in.pop(tp))
        tcm(tp, tcs);

      for (auto const& tc : tcs) {
        queue_out.push(tc);
      }

      if (not process_on.load())
        break;
    }
  }

  void Start() { process_on.store(true); }

  void Stop() { process_on.store(false); }

  void SoftStop()
  {

    // // Nothing will be sending data anymore
    // mtx_queue_in.lock();

    // if (not queue_in.empty()) {
    //   std::cout << "TPs queue not entirely consumed, doing that now.\n";
    //   DuneTriggers::TriggerPrimitive tp;

    //   while (get_next_in_queue_no_mutex(tp)) {

    //     std::vector<DuneTriggers::TriggerCandidate> tcs;
    //     tcm(tp,tcs);
    //     mtx_queue_out.lock();

    //     for (auto const& tc: tcs) {
    //       queue_out.push_back(tc);
    //     }
    //     mtx_queue_out.unlock();
    //   }
    //   std::cout << "TPs consumed\n";
    // }

    // mtx_queue_in.unlock(); // Doesn't matter anyway

    int n_tries = 10, i_try = 0;

    while (true) {

      bool should_try_again = (not queue_out.empty() and i_try < n_tries);

      if (!should_try_again)
        break;

      if (not i_try) {
        std::cout << "TC queue isn't empty, waiting for consumer";
      } else {
        std::cout << ".";
      }

      std::chrono::milliseconds millisec_wait(50);
      std::this_thread::sleep_for(millisec_wait);
      i_try++;
    }
    std::cout << "\n";
    if (not queue_out.empty()) {
      std::cout << "The consumer of TC hasn't caught up\n";
    }

    process_on.store(false);
  }

  const bool GetNextProcessed(DuneTriggers::TriggerCandidate& dat) { return queue_out.pop(dat); }
};
