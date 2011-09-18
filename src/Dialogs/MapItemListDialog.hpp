/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
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

#ifndef XCSOAR_AIRSPACE_AT_POINT_DIALOG_HPP
#define XCSOAR_AIRSPACE_AT_POINT_DIALOG_HPP

#include "Math/fixed.hpp"

class SingleWindow;
struct GeoPoint;
class AirspaceRenderer;
struct AirspaceComputerSettings;
struct AirspaceRendererSettings;
class Waypoints;
struct WaypointLook;
struct WaypointRendererSettings;
struct MoreData;
struct DerivedInfo;
struct AirspaceLook;

bool ShowMapItemListDialog(SingleWindow &parent, const GeoPoint &location,
                               const AirspaceRenderer &renderer,
                               const AirspaceComputerSettings &computer_settings,
                               const AirspaceRendererSettings &renderer_settings,
                               const Waypoints *waypoints,
                               const WaypointLook &_waypoint_look,
                               const WaypointRendererSettings &waypoint_settings,
                               const MoreData &basic,
                               const DerivedInfo &calculated, fixed range);

#endif
