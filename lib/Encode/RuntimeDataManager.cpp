//===-- RuntimeDataManager.cpp -----------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "klee/Encode/RuntimeDataManager.h"
#include "klee/Support/ErrorHandling.h"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <utility>

using namespace std;
using namespace llvm;

namespace klee {

RuntimeDataManager::RuntimeDataManager() : currentTrace(NULL) {
  traceList.reserve(20);

  allFormulaNum = 0;
  solvingTimes = 0;
  allGlobal = 0;
  brGlobal = 0;
  satBranch = 0;
  unSatBranchBySolve = 0;
  unSatBranchByPreSolve = 0;

  solvingCost = 0.0;
  runningCost = 0.0;
  satCost = 0.0;
  unSatCost = 0.0;

  DTAMCost = 0;
  DTAMSerialCost = 0;
  DTAMParallelCost = 0;
  DTAMhybridCost = 0;
  PTSCost = 0;

}

RuntimeDataManager::~RuntimeDataManager() {
  for (auto trace : traceList) {
    delete trace;
  }
  }

std::string RuntimeDataManager::getResultString() {
  stringstream ss;
  ss << "AllFormulaNum:" << allFormulaNum << "\n";
  ss << "SovingTimes:" << solvingTimes << "\n";
  ss << "TotalNewPath:" << testedTraceList.size() << "\n";
  ss << "TotalOldPath:" << traceList.size() - testedTraceList.size() << "\n";
  ss << "TotalPath:" << traceList.size() << "\n";
  if (testedTraceList.size()) {
    ss << "allGlobal:" << allGlobal * 1.0 / testedTraceList.size() << "\n";
    ss << "brGlobal:" << brGlobal * 1.0 / testedTraceList.size() << "\n";
  } else {
    ss << "allGlobal:0"
       << "\n";
    ss << "brGlobal:0"
       << "\n";
  }
  if (testedTraceList.size()) {
    ss << "AllBranch:" << (satBranch + unSatBranchBySolve) * 1.0 / testedTraceList.size() << "\n";
    ss << "satBranch:" << satBranch * 1.0 / testedTraceList.size() << "\n";
  } else {
    ss << "AllBranch:0"
       << "\n";
    ss << "satBranch:0"
       << "\n";
  }
  if (satBranch) {
    ss << "satCost:" << satCost / satBranch << "\n";
  } else {
    ss << "satCost:0"
       << "\n";
  }
  if (testedTraceList.size()) {
    ss << "unSatBranchBySolve:" << unSatBranchBySolve * 1.0 / testedTraceList.size() << "\n";
  } else {
    ss << "unSatBranchBySolve:0"
       << "\n";
  }
  if (unSatBranchBySolve) {
    ss << "unSatCost:" << unSatCost / unSatBranchBySolve << "\n";
  } else {
    ss << "unSatCost:0"
       << "\n";
  }
  if (testedTraceList.size()) {
    ss << "unSatBranchByPreSolve:" << unSatBranchByPreSolve * 1.0 / testedTraceList.size() << "\n";
  } else {
    ss << "unSatBranchByPreSolve:0"
       << "\n";
  }

  ss << "SolvingCost:" << solvingCost << "\n";
  ss << "RunningCost:" << runningCost << "\n";

  ss << "DTAMCost:" << DTAMCost << "\n";
  ss << "DTAMSerialCost:" << DTAMSerialCost << "\n";
  ss << "DTAMParallelCost:" << DTAMParallelCost << "\n";
  ss << "DTAMhybridCost:" << DTAMhybridCost << "\n";
  ss << "PTSCost:" << PTSCost << "\n";

  ss << "DTAMSerialMap:" << allDTAMSerialMap.size() << "\n";
  ss << "DTAMParallelMap:" << allDTAMParallelMap.size() << "\n";
  ss << "DTAMhybridMap:" << allDTAMhybridMap.size() << "\n";
  ss << "taintMap:" << allTaintMap.size() << "\n";

  for (unsigned i = 0; i < allDTAMCost.size(); i++) {

    ss << "\n";
    ss << "times:" << i << "\n";

    ss << "allDTAMCost:" << allDTAMCost[i] << "\n";
    ss << "allDTAMSerialCost:" << allDTAMSerialCost[i] << "\n";
    ss << "allDTAMParallelCost:" << allDTAMParallelCost[i] << "\n";
    ss << "allDTAMhybridCost:" << allDTAMhybridCost[i] << "\n";
    ss << "allPTSCost:" << allPTSCost[i] << "\n";

    ss << "allDTAMSerial:" << DTAMSerial[i] << "\n";
    ss << "allDTAMParallel:" << DTAMParallel[i] << "\n";
    ss << "allDTAMhybrid:" << DTAMhybrid[i] << "\n";

    ss << "allTaint:" << taint[i] << "\n";
    ss << "allTaintPTS:" << taintPTS[i] << "\n";
    ss << "allNoTaintPTS:" << noTaintPTS[i] << "\n";

    ss << "allDTAMSerialMap:" << DTAMSerialMap[i] << "\n";
    ss << "allDTAMParallelMap:" << DTAMParallelMap[i] << "\n";
    ss << "allDTAMhybridMap:" << DTAMhybridMap[i] << "\n";
    ss << "allTaintMap:" << TaintAndPTSMap[i] << "\n";

    // add by haoyu 2017.07.03
    ss << "Send_Data:" << Send_Data[i] << "\n";
    ss << "Send_Data_Serial:" << Send_Data_Serial[i] << "\n";
    ss << "Send_Data_Parallel:" << Send_Data_Parallel[i] << "\n";
    ss << "Send_Data_Hybrid:" << Send_Data_Hybrid[i] << "\n";
    ss << "Send_Data_PTS:" << Send_Data_PTS[i] << "\n";
  }
  return ss.str();
}

unsigned RuntimeDataManager::getTestedPathsNumber() {
  return testedTraceList.size();
}

Trace *RuntimeDataManager::createNewTrace(unsigned traceId) {
  currentTrace = new Trace();
  currentTrace->Id = traceId;
  traceList.push_back(currentTrace);
  return currentTrace;
}

Trace *RuntimeDataManager::getCurrentTrace() {
  return currentTrace;
}

void RuntimeDataManager::addToScheduleSet(Prefix *prefix) {
  scheduleSet.push_back(prefix);
}

Prefix *RuntimeDataManager::getNextPrefix() {
  if (scheduleSet.empty()) {
    return NULL;
  } else {
    Prefix *prefix = scheduleSet.front();
    scheduleSet.pop_front();
    return prefix;
  }
}

void RuntimeDataManager::clearAllPrefix() {
  scheduleSet.clear();
}

bool RuntimeDataManager::isCurrentTraceUntested() {
  bool result = true;
  for (set<Trace *>::iterator ti = testedTraceList.begin(), te = testedTraceList.end(); ti != te; ti++) {
    if (currentTrace->isEqual(*ti)) {
      result = false;
      break;
    }
  }
  currentTrace->isUntested = result;
  if (result) {
    testedTraceList.insert(currentTrace);
  }
  return result;
}

void RuntimeDataManager::printAllPrefix(ostream &out) {
  out << "num of prefix: " << scheduleSet.size() << endl;
  unsigned num = 1;
  for (list<Prefix *>::iterator pi = scheduleSet.begin(), pe = scheduleSet.end(); pi != pe; pi++) {
    out << "Prefix " << num << endl;
    (*pi)->print(out);
    num++;
  }
}

void RuntimeDataManager::printAllTrace(ostream &out) {
  out << "\nTrace Info:\n";
  out << "num of trace: " << traceList.size() << endl << endl;
  unsigned num = 1;
  for (vector<Trace *>::iterator ti = traceList.begin(), te = traceList.end(); ti != te; ti++) {
    Trace *trace = *ti;
    if (trace->isUntested) {
      out << "Trace " << num << endl;
      if (trace->abstract.empty()) {
        trace->createAbstract();
      }
      for (vector<string>::iterator ai = trace->abstract.begin(), ae = trace->abstract.end(); ai != ae; ai++) {
        out << *ai << endl;
      }
      out << endl;
      num++;
    }
  }
}

} // namespace klee
