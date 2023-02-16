/**************************************************************************
 *   Copyright (C) 2022 Alec Leamas                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/


#ifndef BAD_SYMBOL_DLG_H___
#define BAD_SYMBOL_DLG_H___

#include <string>

#include <wx/string.h>
#include <wx/window.h>

#include "plugin_handler.h"

#include "gui_lib.h"
static const char* const kBadSymbolMsg = _( R"""(
plugin library %s is missing the required linker symbol
%s. Most likely, this is not a plugin. In any
case it cannot be used

The library will be removed.)""");


/** Message box for plugin where dlopen() etc. fails. */
class BadSymbolDlg: public OCPNMessageDialog {
private:

  std::string FormatMsg(const std::string& path, const std::string& symbol) {
    std::string basename = wxFileName(path).GetFullName().ToStdString();
    return wxString::Format(kBadSymbolMsg, basename, symbol).ToStdString();
  }

public:

  BadSymbolDlg(wxWindow* parent, const std::string& library,
               const std::string& symbol)
   : OCPNMessageDialog(parent, FormatMsg(library, symbol)) {}

  static void Run(wxWindow* parent, const std::string& library,
                  const std::string& symbol) {
    int sts = BadSymbolDlg(parent, library, symbol).ShowModal();
    if (sts == wxID_YES || sts == wxID_OK) remove(library.c_str());
  }
};


#endif  // BAD_SYMBOL_DLG_H___
