//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/options.cc
// Command line option parsing common to all tools
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

#include <cstdlib>
#include <iostream>

#include "options.h"

using namespace boost::program_options;

ToolOptions::ToolOptions() :
  m_desc("Allowed options")
{
  m_desc.add_options()
    ("help,h", "produce help message")
    ("version,v", "produce version message")
    ("quiet,q", "quiet mode (supresses banner)")
    ("input-file", value<std::string>())   
  ;

  m_positional.add("input-file", 1);
}

void ToolOptions::Parse(const int argc, char* argv[])
{
  command_line_parser parser(argc, argv);
  parser.options(m_desc);
  parser.positional(m_positional);
  try {
    store(parser.run(), m_map);
  }
  catch (error& ex) {
    std::cerr << ex.what() << std::endl;
    std::exit(1);
  }
  notify(m_map);
}

boost::optional<std::string> ToolOptions::Filename() const
{
  if (m_map.count("input-file")) {
    return m_map["input-file"].as<std::string>();
  }

  return boost::optional<std::string>();
}
