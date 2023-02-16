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

#ifndef INCOMPAT_PLUG_DLG_H___
#define INCOMPAT_PLUG_DLG_H___

#include <sstream>
#include <string>
#include <memory>

#include <wx/window.h>

#include "plugin_handler.h"
#include "gui_lib.h"

static const char* const kIncompatFooter = _(R"""(
The plugin is not compatible with this version of OpnCPN and
will be uninstalled. You might want to re-install it after
updating the catalog.)""");

/** Message box for incompatible plugin with known name and version. */
class IncompatPluginDlg: public OCPNMessageDialog {
private:

  std::string FormatMsg(std::shared_ptr<const PluginInfo> info) {
    std::stringstream ss;
    ss << _( "Incompatible plugin detected\n\n");
    ss << _("    Name:        ") << info->name << "\n";
    ss << _("    Version:     ");
    ss << info->version_major << "." << info->version_minor << "\n";
    if (info->api_major != 0) {
      ss << _("    API version: ");
      ss << info->api_major << "." << info->api_minor << "\n";
    }
    ss << kIncompatFooter;
    return ss.str();
  }

public:

  static void Run(wxWindow* parent, std::shared_ptr<const PluginInfo> info) {
    int sts = IncompatPluginDlg(parent, info).ShowModal();
    if (sts == wxID_YES || sts == wxID_OK)
      PluginHandler::getInstance()->uninstall(info->name);
  }

  IncompatPluginDlg(wxWindow* parent, std::shared_ptr<const PluginInfo> info)
    : OCPNMessageDialog(parent, FormatMsg(info)) {}
};


#endif   // INCOMPAT_PLUG_DLG_H___
