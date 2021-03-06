//===-- Prefix.cpp ----------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iterator>

#include "klee/Encode/Prefix.h"
#include "klee/Module/InstructionInfoTable.h"
#include <llvm/IR/Instruction.h>

using namespace ::std;
using namespace ::llvm;

namespace klee {

Prefix::Prefix(vector<Event *> &eventList, std::map<Event *, uint64_t> &threadIdMap, std::string name)
    : eventList(eventList), threadIdMap(threadIdMap), name(name) {
  position = this->eventList.begin();
}

void Prefix::reuse() {
  position = this->eventList.begin();
}

Prefix::~Prefix() {}

vector<Event *> *Prefix::getEventList() {
  return &eventList;
}

void Prefix::increasePosition() {
  if (!isFinished()) {
    position++;
  }
}

bool Prefix::isFinished() {
  return position == eventList.end();
}

Prefix::EventIterator Prefix::begin() {
  return eventList.begin();
}

Prefix::EventIterator Prefix::end() {
  return eventList.end();
}

Prefix::EventIterator Prefix::current() {
  return Prefix::EventIterator(position);
}

uint64_t Prefix::getNextThreadId() {
  assert(!isFinished());
  Event *event = *position;
  map<Event *, uint64_t>::iterator ti = threadIdMap.find(event);
  return ti->second;
}

unsigned Prefix::getCurrentEventThreadId() {
  assert(!isFinished());
  Event *event = *position;
  return event->threadId;
}

void Prefix::print(ostream &out) {
  for (vector<Event *>::iterator ei = eventList.begin(), ee = eventList.end(); ei != ee; ei++) {
    Event *event = *ei;
    out << "thread" << event->threadId << " " << event->inst->info->file << " " << event->inst->info->line << ": "
        << event->inst->inst->getOpcodeName();
    map<Event *, uint64_t>::iterator ti = threadIdMap.find(event);
    if (ti != threadIdMap.end()) {
      out << "\n child threadId = " << ti->second;
    }
    out << endl;
  }
}

void Prefix::print(raw_ostream &out) {
  for (vector<Event *>::iterator ei = eventList.begin(), ee = eventList.end(); ei != ee; ei++) {
    Event *event = *ei;
    out << "thread" << event->threadId << " " << event->inst->info->file << " " << event->inst->info->line << ": ";
    event->inst->inst->print(out);
    map<Event *, uint64_t>::iterator ti = threadIdMap.find(event);
    if (ti != threadIdMap.end()) {
      out << "\n child threadId = " << ti->second;
    }
    out << '\n';
  }
}

KInstruction *Prefix::getCurrentInst() {
  assert(!isFinished());
  Event *event = *position;
  return event->inst;
}
std::string Prefix::getName() {
  return name;
}

} /* namespace klee */
