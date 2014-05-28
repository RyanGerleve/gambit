//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/lcp/efglcp.cc
// Implementation of algorithm to solve extensive forms using linear
// complementarity program from sequence form
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
#include "efglcp.h"

using namespace Gambit;

#include "lhtab.h"
#include "lemketab.h"

template <class T> class Solution {
public:
  int ns1, ns2, ni1, ni2;
  Rational maxpay;
  T eps;
  List<BFS<T> > m_list;
  List<GameInfoset> isets1, isets2;
};



//
// Sets the action probabilities at unreached information sets
// which are left undefined by the sequence form method to
// the centroid.  This helps IsNash and LiapValue work correctly.
//
template <class T>
void UndefinedToCentroid(MixedBehavProfile<T> &p_profile)
{
  Game efg = p_profile.GetGame();

  for (int pl = 1; pl <= efg->NumPlayers(); pl++) {
    GamePlayer player = efg->GetPlayer(pl);
    for (int iset = 1; iset <= player->NumInfosets(); iset++) {
      GameInfoset infoset = player->GetInfoset(iset);
      
      if (p_profile.GetRealizProb(infoset) > (T) 0) {
	continue;
      }
	  
      T total = (T) 0;
      for (int act = 1; act <= infoset->NumActions(); act++) {
	total += p_profile.GetActionProb(infoset->GetAction(act));
      }

      if (total == (T) 0) {
	for (int act = 1; act <= infoset->NumActions(); act++) {
	  p_profile(pl, iset, act) = (T) 1.0 / (T) infoset->NumActions();
	}
      }
    }
  }
}


//
// Lemke implements the Lemke's algorithm (as refined by Eaves 
// for degenerate problems) for  Linear Complementarity
// problems, starting from the primary ray.  
//

template <class T> List<MixedBehavProfile<T> > 
NashLcpBehavSolver<T>::Solve(const BehavSupport &p_support) const
{
  BFS<T> cbfs;
  int i, j;
  Solution<T> solution;

  solution.isets1 = p_support.ReachableInfosets(p_support.GetGame()->GetPlayer(1));
  solution.isets2 = p_support.ReachableInfosets(p_support.GetGame()->GetPlayer(2));

  int ntot;
  solution.ns1 = p_support.NumSequences(1);
  solution.ns2 = p_support.NumSequences(2);
  solution.ni1 = p_support.GetGame()->GetPlayer(1)->NumInfosets()+1;
  solution.ni2 = p_support.GetGame()->GetPlayer(2)->NumInfosets()+1;

  ntot = solution.ns1+solution.ns2+solution.ni1+solution.ni2;

  Matrix<T> A(1,ntot,0,ntot);
  Vector<T> b(1,ntot);

  solution.maxpay = p_support.GetGame()->GetMaxPayoff() + Rational(1);

  T prob = (T)1;
  for (i = A.MinRow(); i <= A.MaxRow(); i++) {
    b[i] = (T) 0;
    for (j = A.MinCol(); j <= A.MaxCol(); j++) {
      A(i,j) = (T) 0; 
    }
  }

  FillTableau(p_support, A, p_support.GetGame()->GetRoot(), prob, 1, 1, 0, 0,
	      solution);
  for (i = A.MinRow(); i <= A.MaxRow(); i++) { 
    A(i,0) = -(T) 1;
  }
  A(1,solution.ns1+solution.ns2+1) = (T) 1;
  A(solution.ns1+solution.ns2+1,1) = -(T) 1;
  A(solution.ns1+1,solution.ns1+solution.ns2+solution.ni1+1) = (T) 1;
  A(solution.ns1+solution.ns2+solution.ni1+1,solution.ns1+1) = -(T) 1;
  b[solution.ns1+solution.ns2+1] = -(T)1;
  b[solution.ns1+solution.ns2+solution.ni1+1] = -(T)1;

  LTableau<T> tab(A,b);
  solution.eps = tab.Epsilon();
  
  MixedBehavProfile<T> profile(p_support);
  Vector<T> sol(tab.MinRow(),tab.MaxRow());
  List<MixedBehavProfile<T> > solutions;
  
  try {
    if (m_stopAfter != 1) {
      try {
	AllLemke(p_support, solution.ns1+solution.ns2+1, 
		 tab, 0, A, solution, solutions);
      }
      catch (NashEquilibriumLimitReached &) {
	// Just handle this silently; equilibria are already printed
	// as they are found.
      }
    }
    else {
      tab.Pivot(solution.ns1+solution.ns2+1,0);
      tab.SF_LCPPath(solution.ns1+solution.ns2+1);
      
      AddBFS(tab, solution);
      tab.BasisVector(sol);
      GetProfile(p_support, tab, 
		 profile,sol,p_support.GetGame()->GetRoot(), 1, 1,
		 solution);
      UndefinedToCentroid(profile);
      this->m_onEquilibrium->Render(profile);
    }
  }
  catch (...) {
    // catch exception; return solutions computed (if any)
  }

  return solutions;
}

template <class T> int 
NashLcpBehavSolver<T>::AddBFS(const LTableau<T> &tableau,
			      Solution<T> &p_solution) const
{
  BFS<T> cbfs;
  Vector<T> v(tableau.MinRow(), tableau.MaxRow());
  tableau.BasisVector(v);

  for (int i = tableau.MinCol(); i <= tableau.MaxCol(); i++) {
    if (tableau.Member(i)) {
      cbfs.insert(i, v[tableau.Find(i)]);
    }
  }

  if (p_solution.m_list.Contains(cbfs))  return 0;
  p_solution.m_list.Append(cbfs);
  return 1;
}

//
// All_Lemke finds all accessible Nash equilibria by recursively 
// calling itself.  List maintains the list of basic variables 
// for the equilibria that have already been found.  
// From each new accessible equilibrium, it follows
// all possible paths, adding any new equilibria to the List.  
//
template <class T> int 
NashLcpBehavSolver<T>::AllLemke(const BehavSupport &p_support,
				int j, LTableau<T> &B, int depth,
				Matrix<T> &A,
				Solution<T> &p_solution,
				List<MixedBehavProfile<T> > &p_solutions) const
{
  if (m_maxDepth != 0 && depth > m_maxDepth) {
    return 1;
  }

  int i,newsol,missing;
  T small_num = (T)1/(T)1000;

  Vector<T> sol(B.MinRow(),B.MaxRow());
  MixedBehavProfile<T> profile(p_support);

  newsol =0;
  for (i = B.MinRow(); i <= B.MaxRow() && newsol == 0; i++) {
    if (i != j)  {
      LTableau<T> BCopy(B);
      A(i,0) = -small_num;
      BCopy.Refactor();

      if (depth == 0) {
	BCopy.Pivot(j, 0);
	missing = -j;
      }
      else {
	missing = BCopy.SF_PivotIn(0);
      }

      newsol = 0;

      if (BCopy.SF_LCPPath(-missing) == 1) {
	newsol = AddBFS(BCopy, p_solution);
	BCopy.BasisVector(sol);
	GetProfile(p_support, BCopy, profile, sol,
		   p_support.GetGame()->GetRoot(), 1, 1,
		   p_solution);
	UndefinedToCentroid(profile);
	if (newsol) {
	  this->m_onEquilibrium->Render(profile);
	  p_solutions.Append(profile);
	  if (m_stopAfter > 0 && p_solutions.Length() >= m_stopAfter) {
	    throw NashEquilibriumLimitReached();
	  }
	}
      }
      else {
	// gout << ": Dead End";
      }
      
      A(i,0) = (T) -1;
      if (newsol) {
	BCopy.Refactor();
	AllLemke(p_support, i, BCopy, depth+1, A, p_solution, p_solutions);
      }
    }
  }
  
  return 1;
}

template <class T>
void NashLcpBehavSolver<T>::FillTableau(const BehavSupport &p_support, 
					Matrix<T> &A,
					const GameNode &n, T prob,
					int s1, int s2, int i1, int i2,
					Solution<T> &p_solution) const
{
  int snew;
  int ns1 = p_solution.ns1;
  int ns2 = p_solution.ns2;
  int ni1 = p_solution.ni1;
  int ni2 = p_solution.ni2;

  GameOutcome outcome = n->GetOutcome();
  if (outcome) {
    A(s1,ns1+s2) = Rational(A(s1,ns1+s2)) +
      Rational(prob) * (outcome->GetPayoff<Rational>(1) - p_solution.maxpay);
    A(ns1+s2,s1) = Rational(A(ns1+s2,s1)) +
      Rational(prob) * (outcome->GetPayoff<Rational>(2) - p_solution.maxpay);
  }
  if (n->GetInfoset()) {
    if (n->GetPlayer()->IsChance()) {
      GameInfoset infoset = n->GetInfoset();
      for (int i = 1; i <= n->NumChildren(); i++) {
	FillTableau(p_support, A, n->GetChild(i),
		    Rational(prob) * infoset->GetActionProb(i, Rational(0)),
		    s1, s2, i1, i2, p_solution);
      }
    }
    int pl = n->GetPlayer()->GetNumber();
    if (pl==1) {
      i1=p_solution.isets1.Find(n->GetInfoset());
      snew=1;
      for (int i = 1; i < i1; i++) {
	snew+=p_support.NumActions(p_solution.isets1[i]->GetPlayer()->GetNumber(),
				   p_solution.isets1[i]->GetNumber());
      }
      A(s1,ns1+ns2+i1+1) = -(T)1;
      A(ns1+ns2+i1+1,s1) = (T)1;
      for (int i = 1; i <= p_support.NumActions(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber()); i++) {
	A(snew+i,ns1+ns2+i1+1) = (T)1;
	A(ns1+ns2+i1+1,snew+i) = -(T)1;
	FillTableau(p_support, A, n->GetChild(p_support.GetAction(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber(), i)->GetNumber()),prob,snew+i,s2,i1,i2, p_solution);
      }
    }
    if(pl==2) {
      i2=p_solution.isets2.Find(n->GetInfoset());
      snew=1;
      for (int i = 1; i < i2; i++) {
	snew+=p_support.NumActions(p_solution.isets2[i]->GetPlayer()->GetNumber(),
				   p_solution.isets2[i]->GetNumber());
      }
      A(ns1+s2,ns1+ns2+ni1+i2+1) = -(T)1;
      A(ns1+ns2+ni1+i2+1,ns1+s2) = (T)1;
      for (int i = 1; i <= p_support.NumActions(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber()); i++) {
	A(ns1+snew+i,ns1+ns2+ni1+i2+1) = (T)1;
	A(ns1+ns2+ni1+i2+1,ns1+snew+i) = -(T)1;
	FillTableau(p_support, A, n->GetChild(p_support.GetAction(n->GetInfoset()->GetPlayer()->GetNumber(), n->GetInfoset()->GetNumber(), i)->GetNumber()),prob,s1,snew+i,i1,i2, p_solution);
      }
    }
    
  }
}


template <class T>
void NashLcpBehavSolver<T>::GetProfile(const BehavSupport &p_support,
				       const LTableau<T> &tab, 
				       MixedBehavProfile<T> &v, 
				       const Vector<T> &sol,
				       const GameNode &n, int s1, int s2,
				       Solution<T> &p_solution) const
{
  int ns1 = p_solution.ns1;
  int ns2 = p_solution.ns2;

  if (n->GetInfoset()) {
    int pl = n->GetPlayer()->GetNumber();
    int iset = n->GetInfoset()->GetNumber();

    if (n->GetPlayer()->IsChance()) {
      for (int i = 1; i <= n->NumChildren(); i++) {
	GetProfile(p_support, tab, v, sol, n->GetChild(i), s1, s2,
		   p_solution);
      }
    }
    else if (pl == 1) {
      int inf = p_solution.isets1.Find(n->GetInfoset());
      int snew = 1;
      for (int i = 1; i < inf; i++) {
	snew += p_support.NumActions(1, p_solution.isets1[i]->GetNumber()); 
      }
      
      for (int i = 1; i <= p_support.NumActions(pl, iset); i++) {
	v(pl,inf,i) = (T) 0;
	if (tab.Member(s1)) {
	  int ind = tab.Find(s1);
	  if (sol[ind] > p_solution.eps) {
	    if (tab.Member(snew+i)) {
	      int ind2 = tab.Find(snew+i);
	      if (sol[ind2] > p_solution.eps) {
		v(pl,inf,i) = sol[ind2] / sol[ind];
	      }
	    }
	  } 
	} 
	GetProfile(p_support, tab, v, sol,
		   n->GetChild(p_support.GetAction(pl, iset, i)->GetNumber()),
		   snew+i, s2, p_solution);
      }
    }
    else if (pl == 2) { 
      int inf = p_solution.isets2.Find(n->GetInfoset());
      int snew = 1;
      for (int i = 1; i < inf; i++) {
	snew += p_support.NumActions(2, p_solution.isets2[i]->GetNumber()); 
      }

      for (int i = 1; i<= p_support.NumActions(pl, iset); i++) {
	v(pl,inf,i) = (T) 0;
	if (tab.Member(ns1+s2)) {
	  int ind = tab.Find(ns1+s2);
	  if (sol[ind] > p_solution.eps) {
	    if (tab.Member(ns1+snew+i)) {
	      int ind2 = tab.Find(ns1+snew+i);
	      if (sol[ind2] > p_solution.eps) {
		v(pl,inf,i) = sol[ind2] / sol[ind];
	      }
	    }
	  } 
	} 
	GetProfile(p_support, tab, v, sol,
		   n->GetChild(p_support.GetAction(pl, iset, i)->GetNumber()),
		   s1, snew+i, p_solution);
      }
    }
  }
}

template class NashLcpBehavSolver<double>;
template class NashLcpBehavSolver<Rational>;


