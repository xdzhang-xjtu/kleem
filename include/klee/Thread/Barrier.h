//===-- Barrier.h -----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef BARRIER_H_
#define BARRIER_H_

#include <string>
#include <vector>

#define MAXCOUNT 0x7FFFFFFF
namespace klee {

class Barrier {
public:
  std::string name;

private:
  std::vector<unsigned> blockedList;
  unsigned count;
  unsigned current;

public:
  static const unsigned DEFAULTCOUNT = 0x7fffffff;
  Barrier(std::string name, unsigned count);
  virtual ~Barrier();
  void wait(unsigned threadId);
  void reset();
  bool isFull();
  void setCount(unsigned count);
  std::vector<unsigned> &getBlockedList();
};

} /* namespace klee */

#endif /* BARRIER_H_ */
