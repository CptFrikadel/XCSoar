// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "Map.hpp"
#include "LocalPath.hpp"
#include "system/Path.hpp"
#include "Compatibility/path.h"
#include "util/StringAPI.hxx"
#include "util/StringCompare.hxx"
#include "util/StringPointer.hxx"
#include "util/Macros.hpp"
#include "util/StringView.hxx"
#include "util/IterableSplitString.hxx"
#include "Language/Language.hpp"
#include <string_view>

#ifdef _UNICODE
#include "util/AllocatedString.hxx"
#endif

#include <windef.h> /* for MAX_PATH */

AllocatedPath
ProfileMap::GetPath(std::string_view key) const noexcept
{
  TCHAR buffer[MAX_PATH];
  if (!Get(key, std::span{buffer}))
      return nullptr;

  if (StringIsEmpty(buffer))
    return nullptr;

  return ExpandLocalPath(Path(buffer));
}

std::vector<AllocatedPath>
ProfileMap::GetMultiplePaths(std::string_view key) const noexcept
{
  std::vector<AllocatedPath> paths;

  auto paths_string = Get(key);

  if(paths_string == nullptr)
    return paths;


  for (auto i : TIterableSplitString(paths_string, _('|'))){

    if (i.empty())
      continue;

#ifdef _UNICODE
    std::wstring file_string(i);
#else
    std::string file_string(i);
#endif

    AllocatedPath path(Path(file_string.c_str()));

    if (!path.MatchesExtension(_("txt")) && !path.MatchesExtension(_("air")))
      continue;

    paths.push_back(ExpandLocalPath(path));

  }


  return paths;
}

bool
ProfileMap::GetPathIsEqual(std::string_view key, Path value) const noexcept
{
  const auto saved_value = GetPath(key);
  if (saved_value == nullptr)
    return false;

  return saved_value == value;
}

[[gnu::pure]]
static Path
BackslashBaseName(const TCHAR *p) noexcept
{
  if (DIR_SEPARATOR != '\\') {
    const auto *backslash = StringFindLast(p, _T('\\'));
    if (backslash != NULL)
      p = backslash + 1;
  }

  return Path(p).GetBase();
}

#ifdef _UNICODE

BasicAllocatedString<TCHAR>
ProfileMap::GetPathBase(std::string_view key) const noexcept
{
  TCHAR buffer[MAX_PATH];
  if (!Get(key, std::span{buffer}))
      return nullptr;

  const TCHAR *base = BackslashBaseName(buffer).c_str();
  if (base == nullptr)
    return nullptr;

  return BasicAllocatedString<TCHAR>(base);
}

#else

StringPointer<TCHAR>
ProfileMap::GetPathBase(std::string_view key) const noexcept
{
  const auto *path = Get(key);
  if (path != nullptr)
    path = BackslashBaseName(path).c_str();

  return path;
}

#endif

void
ProfileMap::SetPath(std::string_view key, Path value) noexcept
{
  if (value == nullptr || StringIsEmpty(value.c_str()))
    Set(key, _T(""));
  else {
    const auto contracted = ContractLocalPath(value);
    if (contracted != nullptr)
      value = contracted;

    Set(key, value.c_str());
  }
}
