//===-- MutexManager.h ------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MUTEXMANAGER_H_
#define MUTEXMANAGER_H_

#include <iostream>
#include <map>
#include <string>

#include "klee/Thread/Mutex.h"

namespace klee {

class MutexManager {
private:
  std::map<std::string, Mutex *> mutexPool;
  std::map<unsigned, Mutex *> blockedThreadPool;
  unsigned nextMutexId;

public:
  MutexManager();
  virtual ~MutexManager();
  bool lock(std::string mutexName, unsigned threadId, bool &isBlocked, std::string &errorMsg);
  bool lock(Mutex *mutex, unsigned threadId, bool &isBlocked, std::string &errorMsg);
  bool unlock(std::string mutexName, std::string &errorMsg);
  bool unlock(Mutex *mutex, std::string &errorMsg);
  bool addMutex(std::string mutexName, std::string &errorMsg);
  Mutex *getMutex(std::string mutexName);
  void clear();
  void print(std::ostream &out);
  unsigned getNextMutexId();
  void addBlockedThread(unsigned threadId, std::string mutexName);
  bool tryToLockForBlockedThread(unsigned threadId, bool &isBlocked, std::string &errorMsg);
};

} // namespace klee

#endif /* MUTEXMANAGER_H_ */
