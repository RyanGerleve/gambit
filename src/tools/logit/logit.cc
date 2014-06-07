//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/logit/logit.cc
// Command-line driver program for quantal response equilibrium tracing and
// maximum likelihood estimation.
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
#include <cstdlib>
#include <cerrno>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include "libgambit/libgambit.h"
#include "efglogit.h"
#include "nfglogit.h"
#include "tools/options.h"


void PrintBanner(std::ostream &p_stream)
{
  p_stream << "Compute a branch of the logit equilibrium correspondence\n";
  p_stream << "Gambit version " VERSION ", ";
  p_stream << "Copyright (C) 1994-2014, The Gambit Project\n";
  p_stream << "This is free software, distributed under the GNU GPL\n\n";
}

void PrintHelp(char *progname)
{
  PrintBanner(std::cerr);
  std::cerr << "Usage: " << progname << " [OPTIONS] [file]\n";
  std::cerr << "If file is not specified, attempts to read game from standard input.\n";

  std::cerr << "Options:\n";
  std::cerr << "  -d DECIMALS      show equilibria as floating point with DECIMALS digits\n";
  std::cerr << "  -s STEP          initial stepsize (default is .03)\n";
  std::cerr << "  -a ACCEL         maximum acceleration (default is 1.1)\n";
  std::cerr << "  -m MAXLAMBDA     stop when reaching MAXLAMBDA (default is 1000000)\n";
  std::cerr << "  -l LAMBDA        compute QRE at `lambda` accurately\n";
  std::cerr << "  -L FILE          compute maximum likelihood estimates;\n";
  std::cerr << "                   read strategy frequencies from FILE\n";
  std::cerr << "  -h, --help       print this help message\n";
  std::cerr << "  -q               quiet mode (suppresses banner)\n";
  std::cerr << "  -e               print only the terminal equilibrium\n";
  std::cerr << "                   (default is to print the entire branch)\n";
  std::cerr << "  -v, --version    print version information\n";
  exit(1);
}

//
// Read in a comma-separated values list of observed data values
//
bool ReadProfile(std::istream &p_stream, Gambit::Array<double> &p_profile)
{
  for (int i = 1; i <= p_profile.Length(); i++) {
    if (p_stream.eof() || p_stream.bad()) {
      return false;
    }

    p_stream >> p_profile[i];
    if (i < p_profile.Length()) {
      char comma;
      p_stream >> comma;
    }
  }
  // Read in the rest of the line and discard
  std::string foo;
  std::getline(p_stream, foo);
  return true;
}


int main(int argc, char *argv[])
{
  using namespace boost::program_options;

  bool useStrategic = false;
  double maxLambda = 1000000.0;
  std::string mleFile = "", startFile = "";
  double maxDecel = 1.1;
  double hStart = 0.03;
  double targetLambda = -1.0;
  bool fullGraph = true;
  int decimals = 6;

  ToolOptions options;
  options.GetDesc().add_options()
    ("num-decimals,d", value<int>(&decimals))
    ("start-size,s", value<double>(&hStart))
    ("max-decel,a", value<double>(&maxDecel))
    ("max-lambda,m", value<double>(&maxLambda))
    ("only-terminal,e", "print only the terminal equilibrium")
    ("use-strategic,S", bool_switch(&useStrategic))
    ("mle-file,L", value<std::string>(&mleFile))
    ("start-file,p", value<std::string>(&startFile))
    ("target-lambda,l", value<double>(&targetLambda))
  ;

  options.Parse(argc, argv);

  if (options.GetMap().count("only-terminal")) {
    fullGraph = false;
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
    Gambit::Array<double> frequencies;
    Gambit::Game game = Gambit::ReadGame(*input_stream);

    if (mleFile != "" && (!game->IsTree() || useStrategic)) {
      frequencies = Gambit::Array<double>(game->MixedProfileLength());
      std::ifstream mleData(mleFile.c_str());
      ReadProfile(mleData, frequencies);
    }
  

    if (!game->IsTree() || useStrategic) {
      if (startFile == "") {
	Gambit::MixedStrategyProfile<double> start(game->NewMixedStrategyProfile(0.0));
	StrategicQREPathTracer tracer(start);
	tracer.SetMaxDecel(maxDecel);
	tracer.SetStepsize(hStart);
	tracer.SetFullGraph(fullGraph);
	tracer.SetTargetParam(targetLambda);
	tracer.SetDecimals(decimals);
	tracer.SetMLEFrequencies(frequencies);
	tracer.TraceStrategicPath(start, 0.0, maxLambda, 1.0);
      }
      else {
	Gambit::Array<double> profile(game->MixedProfileLength() + 1);
	std::ifstream startData(startFile.c_str());
	ReadProfile(startData, profile);
	Gambit::MixedStrategyProfile<double> start(game->NewMixedStrategyProfile(0.0));
	for (int i = 1; i <= start.MixedProfileLength(); i++) {
	  start[i] = profile[i+1];
	}
	StrategicQREPathTracer tracer1(start);
	tracer1.SetMaxDecel(maxDecel);
	tracer1.SetStepsize(hStart);
	tracer1.SetFullGraph(fullGraph);
	tracer1.SetTargetParam(targetLambda);
	tracer1.SetDecimals(decimals);
	tracer1.TraceStrategicPath(start, profile[1], maxLambda, 1.0);
	std::cout << std::endl;
	StrategicQREPathTracer tracer2(start);
	tracer2.SetMaxDecel(maxDecel);
	tracer2.SetStepsize(hStart);
	tracer2.SetFullGraph(fullGraph);
	tracer2.SetTargetParam(targetLambda);
	tracer2.SetDecimals(decimals);
	tracer2.TraceStrategicPath(start, profile[1], maxLambda, -1.0);
      }

    }
    else {
      MixedBehavProfile<double> start(game);
      AgentQREPathTracer tracer(start);
      tracer.SetMaxDecel(maxDecel);
      tracer.SetStepsize(hStart);
      tracer.SetFullGraph(fullGraph);
      tracer.SetTargetParam(targetLambda);
      tracer.SetDecimals(decimals);
      tracer.TraceAgentPath(start, 0.0, maxLambda, 1.0);
    }
    return 0;
  }
  catch (Gambit::InvalidFileException e) {
    std::cerr << "Error: Game not in a recognized format.\n";
    if(!options.Quiet()) std::cerr<<e.what()<<endl;
    return 1;
  }
  catch (...) {
    std::cerr << "Error: An internal error occurred.\n";
    return 1;
  }
}
