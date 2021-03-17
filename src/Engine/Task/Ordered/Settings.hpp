/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2021 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
 */

#ifndef XCSOAR_ORDERED_TASK_SETTINGS_HPP
#define XCSOAR_ORDERED_TASK_SETTINGS_HPP

#include "StartConstraints.hpp"
#include "FinishConstraints.hpp"
#include "Task/Shapes/FAITriangleSettings.hpp"

/**
 * Settings for ordered tasks; most of these are set by
 * the #AbstractTaskFactory but can be overriden
 */
struct OrderedTaskSettings {
  /** Desired AAT minimum task time (s) */
  double aat_min_time;

  StartConstraints start_constraints;
  FinishConstraints finish_constraints;

  FAITriangleSettings fai_triangle;

  void SetDefaults();
};

#endif
