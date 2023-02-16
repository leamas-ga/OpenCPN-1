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

#ifndef BAD_PLUG_DLG_H___
#define BAD_PLUG_DLG_H___

#include <string>

#include <wx/string.h>
#include <wx/window.h>

#include "plugin_handler.h"

#include "gui_lib.h"

static const char* const kBadPLuginMsg =
_( R"""(The plugin %s is incompatible with this version of
OpnCPN and will be uninstalled. You might want to re-install it
after updating the catalog.)""");

/** Message box for incompatible plugin with known name but no version. */
class BadPluginDlg: public OCPNMessageDialog {
private:

  std::string FormatMsg(const std::string& plugin) {
    return wxString::Format(kBadPLuginMsg, plugin).ToStdString();
  }

public:

  static void Run(wxWindow* parent, const std::string& plugin) {
    int sts = BadPluginDlg(parent, plugin).ShowModal();
    if (sts == wxID_YES || sts == wxID_OK)
      PluginHandler::getInstance()->uninstall(plugin);
  }

  BadPluginDlg(wxWindow* parent, const std::string& plugin)
    : OCPNMessageDialog(parent, FormatMsg(plugin)) {}
};


#endif   // BAD_PLUG_DLG_H___
