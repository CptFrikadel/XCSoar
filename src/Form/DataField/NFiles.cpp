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

class FileVisitor: public File::Visitor
{
private:
  NFileDataField &datafield;

public:
  FileVisitor(NFileDataField &_datafield) : datafield(_datafield) {}

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
	DataField(Type::FILE, true, listener){
	   	std::cout << "WOLOLO" << std::endl;
	}

int NFileDataField::GetAsInteger() const {return 0;} 
void NFileDataField::SetAsInteger(int new_value) {}

void NFileDataField::SetAsInteger(std::vector<int> values){

	for (auto value : values){

		Set(value);
	}
}

void NFileDataField::AddFile(Path path){}
void NFileDataField::AddNull(){}
unsigned NFileDataField::GetNumFiles() const{return 0;}
int NFileDataField::Find(Path path) const {return 0;}
void NFileDataField::Lookup(Path text){}
std::vector<Path> NFileDataField::GetPathFiles() const{ return std::vector<Path>{};}
void NFileDataField::ScanMultiplePatterns(const TCHAR *reference){}
void NFileDataField::Set(unsigned int new_value){}
void NFileDataField::Set(std::vector<unsigned> new_values){}

Path NFileDataField::GetItem(unsigned index) const {

	return files[index].path;
}


void NFileDataField::Inc() {}
void NFileDataField::Dec() {}


const TCHAR * NFileDataField::GetAsString() const { return nullptr;}
const TCHAR * NFileDataField::GetAsDisplayString() const { return nullptr;}

ComboList NFileDataField::CreateComboList(const TCHAR *reference) const {

	ComboList combo_list;

	return combo_list;
}
