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

/**
 * Checks whether the given string str equals a xcsoar internal file's filename
 * @param str The string to check
 * @return True if string equals a xcsoar internal file's filename
 */
gcc_pure
static bool
IsInternalFile(const TCHAR* str)
{
  static const TCHAR *const ifiles[] = {
    _T("xcsoar-checklist.txt"),
    _T("xcsoar-flarm.txt"),
    _T("xcsoar-marks.txt"),
    _T("xcsoar-persist.log"),
    _T("xcsoar-startup.log"),
    _T("xcsoar.log"),
    _T("xcsoar-rasp.dat"),
    _T("user.cup"),
    nullptr
  };

  for (unsigned i = 0; ifiles[i] != nullptr; i++)
    if (StringIsEqual(str, ifiles[i]))
      return true;

  return false;
}

class NFileVisitor: public File::Visitor
{
private:
  NFileDataField &datafield;

public:
  NFileVisitor(NFileDataField &_datafield) : datafield(_datafield) {}

  void Visit(Path path, Path filename) override {
    if (!IsInternalFile(filename.c_str()))
      datafield.AddFile(path);
  }
};

inline void
NFileDataField::Item::Set(Path _path){

	path = _path;
	filename = path.GetBase();
	if (filename == nullptr)
		filename = path;
}

NFileDataField::NFileDataField(DataFieldListener *listener) :
	DataField(Type::NFILE, false, listener){ }

int NFileDataField::GetAsInteger() const {return 0;} 
void NFileDataField::SetAsInteger(int new_value) {}

void NFileDataField::SetAsInteger(std::vector<int> values){

	for (auto value : values){

		Set(value);
	}
}

void NFileDataField::AddFile(Path path){

	if (files.full())
		return;

	std::cout << path.c_str() << std::endl;

	Item &item = files.append();
	item.Set(path);

}


void NFileDataField::AddNull(){

	assert(!files.full());

	Item &item = files.append();
	item.filename = Path(_T(""));
	item.path = Path(_T(""));
}


unsigned NFileDataField::GetNumFiles() const{return 0;} // TODO

// Find the index of path in files
int NFileDataField::Find(Path path) const {

	for (unsigned int i = 0; i < files.size(); i++){
		if (files[i].path == path)
			return i;
	}
	
	return -1;
}


void NFileDataField::Lookup(Path text){

	auto i = Find(text);

	if (i >= 0)
		current_selection.insert(i);


}


std::vector<Path> NFileDataField::GetPathFiles() const{ 

	if (current_selection.empty())
		return std::vector<Path>{Path(_T(""))};

	std::vector<Path> paths;

	for (auto index : current_selection){

		paths.push_back(files[index].path);
		std::cout << files[index].path.c_str() << std::endl;
		//assert(paths[index] != nullptr);
	}
	
	return paths;
}


void NFileDataField::ScanMultiplePatterns(const TCHAR *patterns){

	size_t length;

	while ((length = _tcsclen(patterns)) > 0){

		ScanDirectoryTop(patterns);
		patterns += length + 1;
	}

}


void NFileDataField::ScanDirectoryTop(const TCHAR *filter){

	NFileVisitor fv(*this);
	VisitDataFiles(filter, fv);
}


void NFileDataField::Set(unsigned int new_value){}
void NFileDataField::Set(std::vector<unsigned> new_values){}

Path NFileDataField::GetItem(unsigned index) const {

	return files[index].path;
}


void NFileDataField::Inc() {}
void NFileDataField::Dec() {}


const TCHAR * NFileDataField::GetAsString() const { return "Wololo";}
const TCHAR * NFileDataField::GetAsDisplayString() const { return "WOLOLO";}

