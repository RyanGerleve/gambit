//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/liap/liap.cc
// Compute Nash equilibria by minimizing Liapunov function
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
#include "tools/options.h"

void PrintBanner(std::ostream &p_stream)
{
  p_stream << "Compute Nash equilibria by minimizing the Lyapunov function\n";
  p_stream << "Gambit version " VERSION ", Copyright (C) 1994-2014, The Gambit Project\n";
  p_stream << "This is free software, distributed under the GNU GPL\n\n";
}

void PrintHelp(char *progname)
{
  PrintBanner(std::cerr);
  std::cerr << "Usage: " << progname << " [OPTIONS] [file]\n";
  std::cerr << "If file is not specified, attempts to read game from standard input.\n";
  std::cerr << "With no options, attempts to compute one equilibrium starting at centroid.\n";

  std::cerr << "Options:\n";
  std::cerr << "  -d DECIMALS      print probabilities with DECIMALS digits\n";
  std::cerr << "  -h, --help       print this help message\n";
  std::cerr << "  -n COUNT         number of starting points to generate\n";
  std::cerr << "  -s FILE          file containing starting points\n";
  std::cerr << "  -q               quiet mode (suppresses banner)\n";
  std::cerr << "  -V, --verbose    verbose mode (shows intermediate output)\n";
  std::cerr << "                   (default is to only show equilibria)\n";
  std::cerr << "  -v, --version    print version information\n";
  exit(1);
}

extern void SolveStrategic(const Gambit::Game &);
extern void SolveExtensive(const Gambit::Game &);

int m_stopAfter = 0;
int m_numTries = 10;
int m_maxits1 = 100;
int m_maxitsN = 20;
double m_tol1 = 2.0e-10;
double m_tolN = 1.0e-10;
std::string startFile = "";
bool useRandom = false;
int g_numDecimals = 6;
bool verbose = false;

int main(int argc, char *argv[])
{
  using namespace boost::program_options;

  bool useStrategic = false;

  ToolOptions options;
  options.GetDesc().add_options()
    ("num-decimals,d", value<int>(&g_numDecimals))
    ("num-tries,n", value<int>(&m_numTries))
    ("start-file,s", value<std::string>(&startFile))
    ("use-strategic,S", bool_switch(&useStrategic))
    ("verbose,V", bool_switch(&verbose))
  ;

  options.Parse(argc, argv);

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
    
    if (!game->IsTree() || useStrategic) {
      SolveStrategic(game);
    }
    else {
      SolveExtensive(game);
    }
    return 0;
  }
  catch (Gambit::InvalidFileException e) {
    std::cerr << "Error: Game not in a recognized format.\n";
    if (verbose) std::cerr<<e.what()<<endl;
    return 1;
  }
  catch (...) {
    std::cerr << "Error: An internal error occurred.\n";
    return 1;
  }
}
