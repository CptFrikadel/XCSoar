/*
Copyright_License {

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

#ifndef XCSOAR_TOPOGRAPHY_THREAD_HPP
#define XCSOAR_TOPOGRAPHY_THREAD_HPP

#include "thread/StandbyThread.hpp"
#include "Projection/WindowProjection.hpp"
#include "Geo/GeoBounds.hpp"

#include <functional>

class TopographyStore;

/**
 * A thread that loads topography files asynchronously.
 */
class TopographyThread final : private StandbyThread {
  TopographyStore &store;

  const std::function<void()> callback;

  WindowProjection next_projection;

  GeoBounds last_bounds;
  double scale_threshold;

public:
  TopographyThread(TopographyStore &_store, std::function<void()> &&_callback);
  ~TopographyThread();

  using StandbyThread::LockStop;

  void Trigger(const WindowProjection &_projection);

private:
  /* virtual methods from class StandbyThread*/
  void Tick() noexcept override;
};

#endif
