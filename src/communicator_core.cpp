#include "communicator_core.h"

#include <mutex>
#include <unordered_map>

class MutexMap {
  std::mutex mtx_;
  std::unordered_map<int, std::mutex> mp_;
 public:
  MutexMap() {}
  MutexMap(const MutexMap&) = delete;
  MutexMap(MutexMap&&) = delete;
  MutexMap& operator=(const MutexMap&) = delete;
  MutexMap& operator=(MutexMap&&) = delete;

  void Lock(int id) {
    mtx_.lock();
    std::mutex& now_mtx = mp_[id];
    mtx_.unlock();
    now_mtx.lock();
  }
  void Unlock(int id) {
    mtx_.lock();
    auto iter = mp_.find(id);
    if (iter != mp_.end()) {
      iter->second.unlock();
    }
    mtx_.unlock();
  }
};

MutexMap prob_lock, sub_lock;

void LockProblem(int id) {
  prob_lock.Lock(id);
}
void LockSubmission(int id) {
  sub_lock.Lock(id);
}
void UnlockProblem(int id) {
  prob_lock.Unlock(id);
}
void UnlockSubmission(int id) {
  sub_lock.Unlock(id);
}
