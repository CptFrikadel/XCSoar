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

#include "Nfiles.hpp"
#include "ComboList.hpp"
#include "LocalPath.hpp"
#include "Util/StringAPI.hxx"
#include "OS/FileUtil.hpp"

#include <algorithm>

#include <windef.h> /* for MAX_PATH */
#include <cassert>
#include <stdlib.h>

#include <iostream>



NFileDataField::NFileDataField(DataFieldListener *listener) :
	DataField(Type::NFILE, false, listener){ }

int NFileDataField::GetAsInteger() const {return 0;} 
void NFileDataField::SetAsInteger(int new_value) {}

void NFileDataField::SetAsInteger(std::vector<int> values){

	for (auto value : values){

		Set(value);
	}
}


unsigned NFileDataField::GetNumFiles() const{return 0;} // TODO

// Find the index of path in files
int NFileDataField::Find(Path path) const {

	return file_datafield.Find(path);

}


void NFileDataField::Lookup(Path text){

	file_datafield.EnsureLoaded();
	auto i = Find(text);

	if (i >= 0){
		current_selection.insert(i);
		original_selection.insert(i);
	}

	UpdateDisplayString();

}

void NFileDataField::Restore(){

	current_selection.clear();
	current_selection = original_selection;

}

std::vector<Path> NFileDataField::GetPathFiles() const{ 

	if (current_selection.empty())
		return std::vector<Path>{Path(_T(""))};

	std::vector<Path> paths;

	for (auto index : current_selection){

		paths.push_back(file_datafield.files[index].path);
	}
	
	return paths;
}


void NFileDataField::ScanMultiplePatterns(const TCHAR *patterns){

	file_datafield.ScanMultiplePatterns(patterns);

}


void NFileDataField::Set(unsigned int new_value){

	if (current_selection.find(new_value) != current_selection.end()){
		current_selection.erase(new_value);
	} else {
		current_selection.insert(new_value);
	}

	UpdateDisplayString();

}

void NFileDataField::UnSet(Path path){

	auto i = Find(path);
	
	if (i >= 0){

		current_selection.erase(i);
	}

	UpdateDisplayString();
}


void NFileDataField::Set(std::vector<unsigned> new_values){}

Path NFileDataField::GetItem(unsigned index) const {

	return file_datafield.files[index].path;
}

ComboList NFileDataField::CreateComboList(const TCHAR *reference) const {

	return file_datafield.CreateComboList(reference);

}

void NFileDataField::SetFromCombo(int datafield_index, const TCHAR *string_value){

	current_selection.insert(datafield_index);
	
}

void NFileDataField::ForceModify(Path path){

	file_datafield.ForceModify(path);

	current_selection.insert(file_datafield.files.size() -1);
}


void NFileDataField::Inc() { std::cout << "DIKKE INC YO!" << std::endl;}
void NFileDataField::Dec() { std::cout << "DIKKE DEC YO!" << std::endl;}


const TCHAR * NFileDataField::GetAsString() const { return "Wololo";}

void NFileDataField::UpdateDisplayString() {

	UnsafeCopyString(display_string, "");

	for (auto i : current_selection){

		_tcscat(display_string, file_datafield.files[i].filename.GetBase().c_str());
		_tcscat(display_string, " ");

	}

}

const TCHAR * NFileDataField::GetAsDisplayString() const { 
	
	return display_string;
}

