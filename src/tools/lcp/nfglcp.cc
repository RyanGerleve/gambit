//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/lcp/nfglcp.cc
// Compute Nash equilibria via Lemke-Howson algorithm
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//

#include <cstdio>
#include <unistd.h>
#include <iostream>

#include "libgambit/libgambit.h"
#include "nfglcp.h"
#include "lhtab.h"


using namespace Gambit;

extern int g_numDecimals, g_stopAfter, g_maxDepth;
extern bool g_printDetail;

namespace {
//
// Pseudo-exception raised when maximum number of equilibria to compute
// has been reached.  A convenience for unraveling a potentially
// deep recursion.
//
// FIXME: There is an identical twin of this in efglcp.cc.  This should be
// refactored into a more generally-useful and generally-visible location.
//
class EquilibriumLimitReachedNfg : public Exception {
public:
  virtual ~EquilibriumLimitReachedNfg() throw() { }
  const char *what(void) const throw() { return "Reached target number of equilibria"; }
};

} // end anonymous namespace


template <class T> Matrix<T> Make_A1(const StrategySupport &); 
template <class T> Vector<T> Make_b1(const StrategySupport &);
template <class T> Matrix<T> Make_A2(const StrategySupport &);
template <class T> Vector<T> Make_b2(const StrategySupport &);


//
// Function called when a CBFS is encountered.
// If it is not already in the list p_list, it is added.
// The corresponding equilibrium is computed and output.
// Returns 'true' if the CBFS is new; 'false' if it already appears in the
// list.
//
template <class T> bool
NashLcpStrategySolver<T>::OnBFS(const StrategySupport &p_support,
				List<BFS<T> > &p_list, LHTableau<T> &p_tableau) const
{
  BFS<T> cbfs(p_tableau.GetBFS());
  if (p_list.Contains(cbfs)) {
    return false;
  }

  p_list.Append(cbfs);

  MixedStrategyProfile<T> profile(p_support.NewMixedStrategyProfile<T>());
  int n1 = p_support.NumStrategies(1);
  int n2 = p_support.NumStrategies(2);
  T sum = (T) 0;

  for (int j = 1; j <= n1; j++) {
    if (cbfs.count(j))   sum += cbfs[j];
  }

  if (sum == (T) 0)  {
    // This is the trivial CBFS.
    return false;
  }

  for (int j = 1; j <= n1; j++) {
    if (cbfs.count(j)) {
      profile[p_support.GetStrategy(1, j)] = cbfs[j] / sum;
    }
    else {
      profile[p_support.GetStrategy(1, j)] = (T) 0;
    }
  }

  sum = (T) 0;

  for (int j = 1; j <= n2; j++) {
    if (cbfs.count(n1 + j))  sum += cbfs[n1 + j];
  }

  for (int j = 1; j <= n2; j++) {
    if (cbfs.count(n1 + j)) {
      profile[p_support.GetStrategy(2, j)] = cbfs[n1 + j] / sum;
    }
    else {
      profile[p_support.GetStrategy(2, j)] = (T) 0;
    }
  }
  
  this->m_onEquilibrium->Render(profile);

  if (g_stopAfter > 0 && p_list.Length() >= g_stopAfter) {
    throw EquilibriumLimitReachedNfg();
  }

  return true;
}

//
// AllLemke finds all accessible Nash equilibria by recursively 
// calling itself.  p_list maintains the list of basic variables 
// for the equilibria that have already been found.  
// From each new accessible equilibrium, it follows
// all possible paths, adding any new equilibria to the List.  
//
template <class T> void 
NashLcpStrategySolver<T>::AllLemke(const StrategySupport &p_support,
				   int j, LHTableau<T> &B,
				   List<BFS<T> > &p_list,
				   int depth) const
{
  if (g_maxDepth != 0 && depth > g_maxDepth) {
    return;
  }

  // On the initial depth=0 call, the CBFS we are at is the extraneous
  // solution.
  if (depth > 0 && !OnBFS(p_support, p_list, B)) {
    return;
  }
  
  for (int i = B.MinCol(); i <= B.MaxCol(); i++) {
    if (i != j)  {
      LHTableau<T> Bcopy(B);
      Bcopy.LemkePath(i);
      AllLemke(p_support, i, Bcopy, p_list, depth+1);
    }
  }
}

template <class T> List<MixedStrategyProfile<T> > 
NashLcpStrategySolver<T>::Solve(const StrategySupport &p_support) const
{
  List<BFS<T> > bfsList;

  try {
    Matrix<T> A1 = Make_A1<T>(p_support);
    Vector<T> b1 = Make_b1<T>(p_support);
    Matrix<T> A2 = Make_A2<T>(p_support);
    Vector<T> b2 = Make_b2<T>(p_support);
    LHTableau<T> B(A1, A2, b1, b2);

    if (g_stopAfter != 1) {
      try {
	AllLemke(p_support, 0, B, bfsList, 0);
      }
      catch (EquilibriumLimitReachedNfg &) {
	// This pseudo-exception requires no additional action;
	// bfsList will contain the list of equilibria found
      }
    }
    else  {
      B.LemkePath(1);
      OnBFS(p_support, bfsList, B);
    }
  }
  catch (...) {
    // for now, we won't give *any* solutions -- but we should list
    // any solutions found!
    throw;
  }

  return List<MixedStrategyProfile<T> >();
}

template class NashLcpStrategySolver<double>;
template class NashLcpStrategySolver<Rational>;



