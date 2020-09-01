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

#ifndef XCSOAR_NFILE_DATA_FIELD_HPP
#define XCSOAR_NFILE_DATA_FIELD_HPP

#include "Base.hpp"
#include "Repository/FileType.hpp"
#include "OS/Path.hpp"
#include "Util/StaticArray.hxx"

#include <set>
#include <vector>

/**
 * #DataField specialisation that supplies options as a list of
 * files matching a suffix. Allows for selection of more than one file.
 *
 */
class NFileDataField final : public FileDataField {

public:

	// FileList Item
	struct Item { //TODO: this is the same as in FileDataField
		/** Filename */
		Path filename;
		/** Path including Filename */
		AllocatedPath path;

		Item():filename(nullptr), path(nullptr) {}

		Item(Item &&src):filename(src.filename), path(std::move(src.path)) {
		  src.filename = nullptr;
		  src.path = nullptr;
		}

		Item(const Item &) = delete;

		Item &operator=(Item &&src) {
		  std::swap(filename, src.filename);
		  std::swap(path, src.path);
		  return *this;
		}

		void Set(Path _path);
	};

private:
	static constexpr unsigned MAX_FILES = 512;

	// Currently selected files
	std::set<unsigned int> current_selection;

	// FileList item array
	StaticArray<Item, MAX_FILES> files;

	FileType file_type;

public:

	NFileDataField(DataFieldListener *listener=nullptr);

	FileType GetFileType() const {
		return file_type;
	}

	void SetFileType(FileType _file_type){
		file_type = _file_type;
	}

	// Add a filename/path to the filelist
	void AddFile(Path path);

	// Add an empty row to filelist
	void AddNull();

	// Returns the number of files in the list
	gcc_pure 
	unsigned GetNumFiles() const;

	gcc_pure
	int Find(Path path) const;

	/**
	 * Iterate through the list and find the item where path matches
	 * text. If found, the item is set as selected.
	 */
	void Lookup(Path text);

	// Return the currently selected items
	gcc_pure
	std::vector<Path> GetPathFiles() const;

	//Set selection to the given index(es)
	void Set(unsigned new_value);
	void Set(std::vector<unsigned> new_values);
	void ForceModify(Path path);

	/**
	 * Scan multiple shell patterns.  Each pattern is terminated by a
	 * null byte, and the list ends with an empty pattern.
	 */
	void ScanMultiplePatterns(const TCHAR *patterns);
	void ScanDirectoryTop(const TCHAR *filter);

	gcc_pure
	Path GetItem(unsigned index) const;


	// Virtual methods from DataField Class
	void Inc() override;
	void Dec() override;
	int GetAsInteger() const override;
	const TCHAR *GetAsString() const override;
	const TCHAR *GetAsDisplayString() const override;
	void SetAsInteger(int value) override;

	ComboList CreateComboList(const TCHAR *reference) const override;
	void SetFromCombo(int datafield_index, const TCHAR *string_value) override; 

	// "Overloaded" functions to deal with multiple files
	std::vector<int> GetAsIntegers() const;
	void SetAsInteger(std::vector<int> values);


};

#endif
