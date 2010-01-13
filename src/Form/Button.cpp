/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

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

#include "Form/Button.hpp"
#include "Form/Container.hpp"
#include "Screen/Animation.hpp"
#include "Interface.hpp"

WndButton::WndButton(ContainerControl *Parent, const TCHAR *Name,
    const TCHAR *Caption, int X, int Y, int Width, int Height, void
    (*Function)(WindowControl *Sender)) :
  WindowControl(Parent, NULL, Name, X, Y, Width, Height),
  mDown(false),
  mDefault(false),
  mLastDrawTextHeight(-1),
  mOnClickNotify(Function)
{
  // a button can receive focus
  SetCanFocus(true);

  // fore- and background color should be derived from the parent control
  SetForeColor(GetOwner()->GetForeColor());
  SetBackColor(GetOwner()->GetBackColor());

  // copy the buttons caption to the mCaption field
  _tcscpy(mCaption, Caption);
}

bool
WndButton::on_mouse_up(int x, int y)
{
  // If button does not have capture -> call parent function
  if (!has_capture())
    return WindowControl::on_mouse_up(x, y);

  release_capture();

  // If button hasn't been pressed, mouse is only released over it -> return
  if (!mDown)
    return true;

  // Button is not pressed anymore
  mDown = false;

  // Repainting needed
  invalidate();

  if (mOnClickNotify != NULL) {
    // Save the button coordinates for possible animation
    RECT mRc = get_screen_position();
    SetSourceRectangle(mRc);

    // Call the OnClick function
    (mOnClickNotify)(this);
  }

  return true;
}

bool
WndButton::on_mouse_down(int x, int y)
{
  (void)x;
  (void)y;

  // Button is now pressed
  mDown = true;

  if (!GetFocused())
    // If button not yet focused -> give focus to the button
    set_focus();
  else
    // If button has focus -> repaint
    // QUESTION TB: why not invalidate() if focus is set!?
    invalidate();

  set_capture();

  return true;
}

bool
WndButton::on_mouse_move(int x, int y, unsigned keys)
{
  // If button does not have capture -> call parent function
  if (!has_capture())
    return WindowControl::on_mouse_move(x, y, keys);

  bool in = in_client_rect(x, y);

  // If button is currently pressed and mouse cursor is moving on top of it
  if (in != mDown) {
    mDown = in;
    invalidate();
  }

  return true;
}

bool
WndButton::on_mouse_double(int x, int y)
{
  (void)x;
  (void)y;

  // Button is now pressed
  mDown = true;
  // ... will be repainted
  invalidate();
  // ... and gets captured
  set_capture();

  return true;
}

bool
WndButton::on_key_down(unsigned key_code)
{
#ifdef VENTA_DEBUG_EVENT
  TCHAR ventabuffer[80];
  wsprintf(ventabuffer,TEXT("ONKEYDOWN key_code=%d"), key_code); // VENTA-
  DoStatusMessage(ventabuffer);
#endif

  switch (key_code) {
#ifdef GNAV
  // JMW added this to make data entry easier
  case VK_F4:
#endif
  case VK_RETURN:
  case VK_SPACE:
    // "Press" the button via keys and repaint it
    if (!mDown) {
      mDown = true;
      invalidate();
    }
    return true;
  }

  // If key_down hasn't been handled yet -> call parent function
  return WindowControl::on_key_down(key_code);
}

bool
WndButton::on_key_up(unsigned key_code)
{
  switch (key_code) {
#ifdef GNAV
  // JMW added this to make data entry easier
  case VK_F4:
#endif
  case VK_RETURN:
  case VK_SPACE:
    // Return if button was not pressed long enough
    if (!XCSoarInterface::Debounce())
      return true; // prevent false trigger

    // If button was pressed before
    if (mDown) {
      // "Release" button via keys
      mDown = false;
      // Repaint the button
      invalidate();

      if (mOnClickNotify != NULL) {
        // Save the button coordinates for possible animation
        RECT mRc = get_screen_position();
        SetSourceRectangle(mRc);

        // Call the OnClick function
        (mOnClickNotify)(this);
      }
    }

    return true;
  }

  // If key_down hasn't been handled yet -> call parent function
  return WindowControl::on_key_up(key_code);
}

void
WndButton::on_paint(Canvas &canvas)
{
  // Call parent on_paint function (selector/focus)
  WindowControl::on_paint(canvas);

  // Get button RECT and shrink it to make room for the selector/focus
  RECT rc = get_client_rect();
  InflateRect(&rc, -2, -2); // todo border width

  // JMW todo: add icons?

  // Draw button to the background
  canvas.draw_button(rc, mDown);

  // If button has text on it
  if (mCaption != NULL && mCaption[0] != '\0') {
    // Set drawing colors
    canvas.set_text_color(GetForeColor());
    canvas.set_background_color(GetBackColor());
    canvas.background_transparent();

    // Set drawing font
    canvas.select(*GetFont());

    if (mLastDrawTextHeight < 0) {
      // Calculate the text height and save it for the future
      RECT rc_t = rc;
      canvas.formatted_text(&rc_t, mCaption,
          DT_CALCRECT | DT_EXPANDTABS | DT_CENTER | DT_NOCLIP | DT_WORDBREAK);

      mLastDrawTextHeight = rc_t.bottom - rc_t.top;
    }

    // If button is pressed, offset the text for 3D effect
    if (mDown)
      OffsetRect(&rc, 2, 2);

    // Vertical middle alignment
    rc.top += (canvas.get_height() - 4 - mLastDrawTextHeight) / 2;

    canvas.formatted_text(&rc, mCaption,
        DT_EXPANDTABS | DT_CENTER | DT_NOCLIP | DT_WORDBREAK);
  }
}
