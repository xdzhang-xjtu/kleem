//===-- Barrier.cpp ---------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Thread/Barrier.h"

using namespace ::std;

namespace klee {

Barrier::Barrier(string name, unsigned count) : name(name), count(count), current(0) {}

Barrier::~Barrier() {}

void Barrier::wait(unsigned threadId) {
  this->current++;
  blockedList.push_back(threadId);
}

void Barrier::reset() {
  this->current = 0;
  blockedList.clear();
}

bool Barrier::isFull() {
  if (this->current == this->count) {
    return true;
  } else {
    return false;
  }
}

void Barrier::setCount(unsigned count) {
  this->count = count;
}

vector<unsigned> &Barrier::getBlockedList() {
  return blockedList;
}

} /* namespace klee */
