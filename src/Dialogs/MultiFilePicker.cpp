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

#include "MultiFilePicker.hpp"
#include "Dialogs/FilePicker.hpp"
#include "Widget/ListWidget.hpp"
#include "HelpDialog.hpp"
#include "Form/ActionListener.hpp"
#include "Widget/TextWidget.hpp"
#include "Widget/TwoWidgets.hpp"
#include "UIGlobals.hpp"
#include "WidgetDialog.hpp"
#include "Form/DataField/Nfiles.hpp"
#include "Renderer/TextRowRenderer.hpp"
#include "Look/DialogLook.hpp"
#include "Form/Form.hpp"
#include "Language/Language.hpp"

#include <iostream>

class MultiFilePickerWidget : public ListWidget, public ActionListener {

	unsigned num_items;
	unsigned initial_value;
	unsigned row_height;

	bool visible;

	ListItemRenderer &item_renderer;
	ActionListener &action_listener;

	const TCHAR *const caption, *const help_text;

	TextWidget *help_widget;
	TwoWidgets *two_widgets;

public:
	
	MultiFilePickerWidget(unsigned _num_items, unsigned _initial_value,
						  unsigned _row_height, 
						  ListItemRenderer &_item_renderer,
						  ActionListener &_action_listener,
						  const TCHAR *_caption, const TCHAR *_help_text) :
		num_items(_num_items), initial_value(_initial_value),
		row_height(_row_height), 
		item_renderer(_item_renderer),
		action_listener(_action_listener),
		caption(_caption), help_text(_help_text) { }

	void UpdateHelp(unsigned index){
		if (!visible)
			return;

		help_widget->SetText("Some help text");
		two_widgets->UpdateLayout();
	}

	// Get the list of picked files
	void GetResult(){
		// TODO
	}

	void AddFile(){

	}

	// Virtual methods from the Widget class
	
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
	}

	virtual void Hide() override {
		visible = false;
		ListWidget::Hide();
	}

	void OnPaintItem(Canvas &canvas, const PixelRect rc,
					 unsigned idx) noexcept override {
		item_renderer.OnPaintItem(canvas, rc, idx);
	}

	void OnCursorMoved(unsigned index) noexcept override {
		//UpdateHelp(index); // TODO
	}

	bool CanActivateItem(unsigned index) const noexcept override {
		return true;
	}

	void OnActivateItem(unsigned index) noexcept override {
		action_listener.OnAction(mrOK);
	}	

	void OnAction(int id) noexcept override {

		switch (id){
		case 100:
			HelpDialog(caption, help_text);
			break;
		case 1:
			std::cout << "ADD" << std::endl;
			break;
		case 2:
			std::cout << "REMOVE" << std::endl;
			break;
		}

	}


};

class MultiFilePickerSupport : public ListItemRenderer, public ActionListener {

	TextRowRenderer row_renderer;
	//TODO add list that has to be rendered
	std::vector<Path>& active_files;

public:

	MultiFilePickerSupport(std::vector<Path> &_active_files) :
		active_files(_active_files) {}

	unsigned CalculateLayout(const DialogLook &look) {
		return row_renderer.CalculateLayout(*look.list.font);
	}

	virtual void OnPaintItem(Canvas &canvas, const PixelRect rc,
							 unsigned i) noexcept override {

		row_renderer.DrawTextRow(canvas, rc, active_files[i].c_str());
	}

	virtual void OnAction(int id) noexcept override{

		switch (id){
		case 1:
			std::cout << "Pressed ADD!" << std::endl;
			break;
		case 2:
			std::cout << "Pressed REMOVE!" << std::endl;
			break;
		}
	}


};
	

/**
 * Create the MultiPicker widget thingy
 * Do the picking of files somehow
 *
 * Write the result back to the NFileDataField
 *
 * @return whether or not something was picked
 *
 */
bool MultiFilePicker(const TCHAR *caption, NFileDataField &df, 
					 const TCHAR *help_text)
{

	std::cout << "WAAAAJOOOOW" << std::endl;

	WidgetDialog dialog(WidgetDialog::Full{}, UIGlobals::GetMainWindow(), 
					    UIGlobals::GetDialogLook(), caption);
	
	std::vector<Path> active_files = df.GetPathFiles();

	MultiFilePickerSupport support(active_files);

	//unsigned num_items = df.active_files.size();
	

	MultiFilePickerWidget * file_widget = new MultiFilePickerWidget(active_files.size(), 0, 
			support.CalculateLayout(UIGlobals::GetDialogLook()), 
						  support, dialog, caption, help_text);
	
	Widget * widget = file_widget;

	dialog.AddButton(_("Help"), *file_widget, 100);

	dialog.AddButton(_("Add"), *file_widget, 1);

	dialog.AddButton(_("Remove"), *file_widget, 2);

	dialog.AddButton(_("Cancel"), mrCancel);

	dialog.EnableCursorSelection();

	dialog.FinishPreliminary(widget);

	dialog.ShowModal();

	return false;
}
