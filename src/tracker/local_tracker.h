/**
 * Copyright (c) 2015 by Contributors
 */
#ifndef DIFACTO_TRACKER_LOCAL_TRACKER_H_
#define DIFACTO_TRACKER_LOCAL_TRACKER_H_
#include <vector>
#include "difacto/tracker.h"
#include "./async_local_tracker.h"
namespace difacto {

/**
 * \brief an implementation of the tracker which only runs within a local
 * process
 */
class LocalTracker : public Tracker {
 public:
  LocalTracker() { }
  virtual ~LocalTracker() { delete tracker_; }

  KWArgs Init(const KWArgs& kwargs) override { return kwargs; }

  typedef std::pair<int, std::string> Job;

  void Issue(const std::vector<Job>& jobs) override {
    if (!tracker_) tracker_ = new AsyncLocalTracker<Job, Job>();
    tracker_->Issue(jobs);
  }

  int NumRemains() override {
    return CHECK_NOTNULL(tracker_)->NumRemains();
  }

  void Clear() override {
    CHECK_NOTNULL(tracker_)->Clear();
  }

  void Stop() override {
    if (tracker_) {
      delete tracker_;
      tracker_ = nullptr;
    }
  }

  void Wait() override {
    CHECK_NOTNULL(tracker_)->Wait();
  }

  void SetMonitor(const Monitor& monitor) override {
    CHECK_NOTNULL(tracker_)->SetMonitor(
        [monitor](const Job& rets) {
          monitor(rets.first, rets.second);
        });
  }

  void SetConsumer(const Consumer& consumer) override {
    CHECK_NOTNULL(tracker_)->SetConsumer(
        [consumer](const Job& args,
                   const Tracker<Job>::Callback& on_complete,
                   Job* rets) {
          rets->first = args.first;
          consumer(job.args, &(rets->second));
          on_complete();
        });
  }

 private:
  AsyncLocalTracker<std::string>* tracker_ = nullptr;
};

}  // namespace difacto
#endif  // DIFACTO_TRACKER_LOCAL_TRACKER_H_
