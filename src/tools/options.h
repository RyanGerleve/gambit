//
// This file is part of Gambit
// Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
//
// FILE: src/tools/options.h
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

#ifndef GAMBIT_TOOL_OPTIONS_H
#define GAMBIT_TOOL_OPTIONS_H

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <string>

class ToolOptions {
public:
  ToolOptions();

  boost::program_options::options_description& GetDesc() { return m_desc; }
  boost::program_options::variables_map& GetMap() { return m_map; }

  void Parse(int argc, char* argv[]);

  bool Help() const { return m_map.count("help") != 0; }
  bool Version() const { return m_map.count("version") != 0; }
  bool Quiet() const { return m_map.count("quiet") != 0; }
  boost::optional<std::string> Filename() const;

private:
  boost::program_options::options_description m_desc;
  boost::program_options::positional_options_description m_positional;
  boost::program_options::variables_map m_map;
};

#endif
