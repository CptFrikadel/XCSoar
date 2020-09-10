/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
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

#ifndef XCSOAR_DIALOGS_LIST_PICKER_HPP
#define XCSOAR_DIALOGS_LIST_PICKER_HPP

#include "HelpDialog.hpp"
#include "WidgetDialog.hpp"
#include "Widget/ListWidget.hpp"
#include "Widget/TextWidget.hpp"
#include "Widget/TwoWidgets.hpp"
#include "UIGlobals.hpp"
#include "Language/Language.hpp"
#include "Event/Timer.hpp"
#include "Event/PeriodicTimer.hpp"

#include <tchar.h>

class ListItemRenderer;

/** returns string of item's help text **/
typedef const TCHAR* (*ItemHelpCallback_t)(unsigned item);


class ListPickerWidget : public ListWidget, public ActionListener {
  unsigned num_items;
  unsigned initial_value;
  unsigned row_height;

  bool visible;

  ListItemRenderer &item_renderer;
  ActionListener &action_listener;

  /**
   * This timer is used to postpone the initial UpdateHelp() call.
   * This is necessary because the TwoWidgets instance is not fully
   * initialised yet in Show(), and recursively calling into Widget
   * methods is dangerous anyway.
   */
  Timer postpone_update_help{[this]{
    UpdateHelp(GetList().GetCursorIndex());
  }};

  const TCHAR *const caption, *const help_text;
  ItemHelpCallback_t item_help_callback;
  TextWidget *help_widget;
  TwoWidgets *two_widgets;

public:
  ListPickerWidget(unsigned _num_items, unsigned _initial_value,
                   unsigned _row_height,
                   ListItemRenderer &_item_renderer,
                   ActionListener &_action_listener,
                   const TCHAR *_caption, const TCHAR *_help_text)
    :num_items(_num_items), initial_value(_initial_value),
     row_height(_row_height),
     visible(false),
     item_renderer(_item_renderer),
     action_listener(_action_listener),
     caption(_caption), help_text(_help_text),
     item_help_callback(nullptr) {}

  using ListWidget::GetList;

  void EnableItemHelp(ItemHelpCallback_t _item_help_callback,
                      TextWidget *_help_widget,
                      TwoWidgets *_two_widgets) {
    item_help_callback = _item_help_callback;
    help_widget = _help_widget;
    two_widgets = _two_widgets;
  }

  void UpdateHelp(unsigned index) {
    if (!visible || item_help_callback == nullptr)
      return;

    help_widget->SetText(item_help_callback(index));
    two_widgets->UpdateLayout();
  }

  /* virtual methods from class Widget */

  virtual void Prepare(ContainerWindow &parent,
                       const PixelRect &rc) override {
    ListControl &list = CreateList(parent, UIGlobals::GetDialogLook(), rc,
                                   row_height);
    list.SetLength(num_items);
    list.SetCursorIndex(initial_value);
  }

  virtual void Unprepare() override {
    DeleteWindow();
  }

  virtual void Show(const PixelRect &rc) override {
    ListWidget::Show(rc);

    visible = true;
    postpone_update_help.Schedule({});
  }

  virtual void Hide() override {
    visible = false;
    postpone_update_help.Cancel();
    ListWidget::Hide();
  }

  /* virtual methods from class ListControl::Handler */

  void OnPaintItem(Canvas &canvas, const PixelRect rc,
                   unsigned idx) noexcept override {
    item_renderer.OnPaintItem(canvas, rc, idx);
  }

  void OnCursorMoved(unsigned index) noexcept override {
    UpdateHelp(index);
  }

  bool CanActivateItem(unsigned index) const noexcept override {
    return true;
  }

  void OnActivateItem(unsigned index) noexcept override {
    action_listener.OnAction(mrOK);
  }

  /* virtual methods from class ActionListener */

  void OnAction(int id) noexcept override {
    HelpDialog(caption, help_text);
  }
};



/**
 * Shows a list dialog and lets the user pick an item.
 * @param caption
 * @param num_items
 * @param initial_value
 * @param item_height
 * @param item_renderer Paint a single item
 * @param update Update per timer
 * @param help_text enable the "Help" button and show this text on click
 * @param itemhelp_callback Callback to return string for current item help
 * @param extra_caption caption of another button that closes the
 * dialog (nullptr disables it)
 * @return the list index, -1 if the user cancelled the dialog, -2 if
 * the user clicked the "extra" button
 */
int
ListPicker(const TCHAR *caption,
           unsigned num_items, unsigned initial_value,
           unsigned item_height,
           ListItemRenderer &item_renderer, bool update = false,
           const TCHAR *help_text = nullptr,
           ItemHelpCallback_t itemhelp_callback = nullptr,
           const TCHAR *extra_caption=nullptr);

#endif
