/*
  Solvitaire: a solver for perfect information solitaire games
  Copyright (C) 2018 Charles Blake <thecharlesblake@live.co.uk> and 
  Ian Gent <Ian.Gent@st-andrews.ac.uk>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program (see LICENSE file); if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
//
// Created by thecharlesblake on 10/16/17.
//

#ifndef SOLVITAIRE_LOG_HELPER_H
#define SOLVITAIRE_LOG_HELPER_H

#include <iostream>

#if defined NDEBUG
    #define LOG_DEBUG(msg)
#else
    #define LOG_DEBUG(msg) clog << "[debug]\n" << msg << "\n"
#endif
#define LOG_INFO(msg)  clog << "[info] " << msg << "\n"
#define LOG_WARNING(msg)  clog << "[warning] " << msg << "\n"
#define LOG_ERROR(msg)  cerr << "[error] " << msg << "\n"

#endif //SOLVITAIRE_LOG_HELPER_H
