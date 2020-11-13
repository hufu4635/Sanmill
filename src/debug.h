﻿/*
  Sanmill, a mill game playing engine derived from NineChess 1.5
  Copyright (C) 2015-2018 liuweilhy (NineChess author)
  Copyright (C) 2019-2020 Calcitem <calcitem@outlook.com>

  Sanmill is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Sanmill is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"
#include <cstdio>

#ifdef QT_GUI_LIB
#include <QDebug>
#endif

//#define QT_NO_DEBUG_OUTPUT

#define CSTYLE_DEBUG_OUTPUT

#ifdef CSTYLE_DEBUG_OUTPUT
#define loggerDebug printf
#else
#ifdef QT_GUI_LIB
#define loggerDebug qDebug
#endif
#endif /* CSTYLE_DEBUG_OUTPUT */

#endif /* DEBUG_H */
