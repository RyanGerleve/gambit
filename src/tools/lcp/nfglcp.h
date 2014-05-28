//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/lcp/nfglcp.h
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

#ifndef LCP_NFGLCP_H
#define LCP_NFGLCP_H

#include "libgambit/nash.h"

using namespace Gambit;

template <class T> class LHTableau;
template <class T> class BFS;

template <class T> class NashLcpStrategySolver : public NashStrategySolver<T> {
public:
  NashLcpStrategySolver(shared_ptr<StrategyProfileRenderer<T> > p_onEquilibrium = 0)
    : NashStrategySolver<T>(p_onEquilibrium) { }
  virtual ~NashLcpStrategySolver()  { }

  virtual List<MixedStrategyProfile<T> > Solve(const StrategySupport &) const;

private:
  bool OnBFS(const StrategySupport &,
	     List<BFS<T> > &, LHTableau<T> &) const;
  void AllLemke(const StrategySupport &, int j, LHTableau<T> &,
		List<BFS<T> > &, int) const;
};



#endif  // LCP_NFGLCP_H
