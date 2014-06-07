//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/lp/lp.cc
// Implementation of algorithm to compute mixed strategy equilibria
// of constant sum normal form games via linear programming
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

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstdlib>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include "libgambit/libgambit.h"
#include "efglp.h"
#include "nfglp.h"
#include "tools/options.h"

using namespace Gambit;


void PrintBanner(std::ostream &p_stream)
{
  p_stream << "Compute Nash equilibria by solving a linear program\n";
  p_stream << "Gambit version " VERSION ", Copyright (C) 1994-2014, The Gambit Project\n";
  p_stream << "This is free software, distributed under the GNU GPL\n\n";
}

void PrintHelp(char *progname)
{
  PrintBanner(std::cerr);
  std::cerr << "Usage: " << progname << " [OPTIONS] [file]\n";
  std::cerr << "If file is not specified, attempts to read game from standard input.\n";
  std::cerr << "With no options, reports all Nash equilibria found.\n\n";

  std::cerr << "Options:\n";
  std::cerr << "  -d DECIMALS      compute using floating-point arithmetic;\n";
  std::cerr << "                   display results with DECIMALS digits\n";
  std::cerr << "  -S               use strategic game\n";
  std::cerr << "  -P               find only subgame-perfect equilibria\n";
  std::cerr << "  -h, --help       print this help message\n";
  std::cerr << "  -q               quiet mode (suppresses banner)\n";
  std::cerr << "  -v, --version    print version information\n";
  exit(1);
}

int main(int argc, char *argv[])
{
  using namespace boost::program_options;

  int numDecimals = 6;
  bool useFloat = false, useStrategic = false, printDetail = false;
  bool bySubgames = false;

  ToolOptions options;
  options.GetDesc().add_options()
    ("num-decimals,d", value<int>(&numDecimals))
    ("use-strategic,S", bool_switch(&useStrategic))
    ("subgame-perfect,P", bool_switch(&bySubgames))
    ("print-detail,D", bool_switch(&printDetail))
  ;

  options.Parse(argc, argv);

  if (options.GetMap().count("num-decimals")) {
    useFloat = true;
  }

  if (options.Version()) {
    PrintBanner(std::cerr); exit(1);
  }

  if (options.Help()) {
    PrintHelp(argv[0]);
  }

  if (!options.Quiet()) {
    PrintBanner(std::cerr);
  }

  std::istream* input_stream = &std::cin;
  std::ifstream file_stream;
  boost::optional<std::string> filename = options.Filename();
  if (filename) {
    file_stream.open(*filename);
    if (!file_stream.is_open()) {
      std::ostringstream error_message;
      error_message << argv[0] << ": " << *filename;
      perror(error_message.str().c_str());
      exit(1);
    }
    input_stream = &file_stream;
  }

  try {
    Gambit::Game game = Gambit::ReadGame(*input_stream);

    if (game->NumPlayers() != 2) {
      std::cerr << "Error: Game does not have two players.\n";
      return 1;
    }

    if (!game->IsConstSum()) {
      std::cerr << "Error: Game is not constant-sum.\n";
      return 1;
    }

    if (!game->IsTree() || useStrategic) {
      if (useFloat) {
	shared_ptr<StrategyProfileRenderer<double> > renderer;
	if (printDetail)  {
	  renderer = new MixedStrategyDetailRenderer<double>(std::cout,
							     numDecimals);
	}
	else {
	  renderer = new MixedStrategyCSVRenderer<double>(std::cout, numDecimals);
	}
	NashLpStrategySolver<double> algorithm(renderer);
	algorithm.Solve(game);
      }
      else {
	shared_ptr<StrategyProfileRenderer<Rational> > renderer;
	if (printDetail) {
	  renderer = new MixedStrategyDetailRenderer<Rational>(std::cout);
	}
	else {
	  renderer = new MixedStrategyCSVRenderer<Rational>(std::cout);
	}
	NashLpStrategySolver<Rational> algorithm(renderer);
	algorithm.Solve(game);
      }
    }
    else {
      if (!bySubgames) {
	if (useFloat) {
	  shared_ptr<StrategyProfileRenderer<double> > renderer;
	  if (printDetail)  {
	    renderer = new BehavStrategyDetailRenderer<double>(std::cout,
							       numDecimals);
	  }
	  else {
	    renderer = new BehavStrategyCSVRenderer<double>(std::cout, 
							    numDecimals);
	  }
	  NashLpBehavSolver<double> algorithm(renderer);
	  algorithm.Solve(game);
	}
	else {
	  shared_ptr<StrategyProfileRenderer<Rational> > renderer;
	  if (printDetail) {
	    renderer = new BehavStrategyDetailRenderer<Rational>(std::cout);
	  }
	  else {
	    renderer = new BehavStrategyCSVRenderer<Rational>(std::cout);
	  }
	  NashLpBehavSolver<Rational> algorithm(renderer);
	  algorithm.Solve(game);
	}
      }
      else {
	if (useFloat) {
	  shared_ptr<NashBehavSolver<double> > stage = 
	    new NashLpBehavSolver<double>();
	  shared_ptr<StrategyProfileRenderer<double> > renderer;
	  if (printDetail)  {
	    renderer = new BehavStrategyDetailRenderer<double>(std::cout,
							       numDecimals);
	  }
	  else {
	    renderer = new BehavStrategyCSVRenderer<double>(std::cout, 
							    numDecimals);
	  }
	  SubgameNashBehavSolver<double> algorithm(stage, renderer);
	  algorithm.Solve(game);
	}
	else {
	  shared_ptr<NashBehavSolver<Rational> > stage = 
	    new NashLpBehavSolver<Rational>();
	  shared_ptr<StrategyProfileRenderer<Rational> > renderer;
	  if (printDetail)  {
	    renderer = new BehavStrategyDetailRenderer<Rational>(std::cout,
								 numDecimals);
	  }
	  else {
	    renderer = new BehavStrategyCSVRenderer<Rational>(std::cout, 
							      numDecimals);
	  }
	  SubgameNashBehavSolver<Rational> algorithm(stage, renderer);
	  algorithm.Solve(game);
	}
      }
    }
    return 0;
  }
  catch (InvalidFileException) {
    std::cerr << "Error: Game not in a recognized format.\n";
    return 1;
  }
  catch (std::runtime_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}


