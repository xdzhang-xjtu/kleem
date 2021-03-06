//===-- KQuery2Z3.cpp -------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This class used to convert the format of KQuery to Z3 for convenient solve in
// Encode.cpp. The conversion is not complete because not all the instructions
// are used in our processing.

#ifndef KQUERY2Z3_H_
#define KQUERY2Z3_H_

#include "klee/Expr/Expr.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/Support/DataTypes.h"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <z3++.h>
#include <z3.h>
#include <z3_api.h>

using namespace z3;
namespace klee {

typedef struct fraction {
  int num;
  int den;
} Fraction;

class KQuery2Z3 {
private:
  // preserve for some private variable and function
  std::vector<ref<Expr>> kqueryExpr;
  // the parameter convert means convert a none float point to a float point.
  z3::expr eachExprToZ3(ref<Expr> &ele);
  z3::context &z3_ctx;
  std::vector<z3::expr> vecZ3Expr;
  std::vector<z3::expr> vecZ3ExprTest;
  std::vector<ref<Expr>> kqueryExprTest;
  void getFraction(double, Fraction &);
  int validNum(std::string &str);

public:
  KQuery2Z3(std::vector<ref<Expr>> &_queryExpr, z3::context &_z3_ctx);
  KQuery2Z3(z3::context &_z3_ctx);
  ~KQuery2Z3();

  // only public function for call to complete convert kqueryExpr to z3Expr
  void getZ3Expr();
  z3::expr getZ3Expr(ref<Expr> &e);
  void printZ3AndKqueryExpr();
  void printKquery();
};

} // namespace klee

#endif /*end conditional inclusion*/
