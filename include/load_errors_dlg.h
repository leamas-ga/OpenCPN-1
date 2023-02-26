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

#ifndef LOAD_ERROR_DLG_H___
#define LOAD_ERROR_DLG_H___

#include <iostream>                       // Debug  junk
#include <sstream>
#include <string>
#include <memory>

#include <wx/window.h>

#include "plugin_loader.h"
#include "plugin_handler.h"
#include "gui_lib.h"

static const char* const kBadPluginsIntro = _(R"""(
The following  plugins have encountered errors during startup:

)""");

static const char* const kBadPluginIntro = _(R"""(
The following plugin has encountered errors during startup:

)""");


static const char* const kBadLibsIntro = _(R"""(
The following libraries  have encountered errors during startup:

)""");

static const char* const kBadLibIntro = _(R"""(
The following library has encountered errors during startup:

)""");
static const char* const kBadPluginsFooter = _(R"""(

These plugins will be uninstalled. You might want to reinstall
them after updating  the catalog.
)""");

static const char* const kBadPluginFooter = _(R"""(

This plugin will be uninstalled. You might want to reinstall
it after updating  the catalog.
)""");


static const char* const kBadLibsFooter = _(R"""(

These libraries  will be removed. You might want to reinstall them
after updating  the catalog. However, I don't know which actual 
plugins to install in this case.
)""");

static const char* const kBadLibFooter = _(R"""(

This library  will be removed. You might want to reinstall it
after updating  the catalog. However, I don't know which actual 
plugin to install in this case.
)""");

class FormatCtx {
public:
  std::vector<std::string> plugins;
  std::vector<std::string> libs;
  FormatCtx(const std::vector<LoadError> errors) {
    auto handler =  PluginHandler::getInstance();
    for (const auto& e :  errors) {	      
      auto plugin = handler->getPluginByLibrary(e.lib_path); 
      if (plugin != "")
        plugins.push_back(plugin);
      else
        libs.push_back(e.lib_path);
    }
  }
};


/** Message box for incompatible plugin with known name and version. */
class LoadErrorsDlg : public OCPNMessageDialog {
private:
  std::string FormatMsg(const FormatCtx ctx) {
    auto handler =  PluginHandler::getInstance();
    std::stringstream ss;
    if (ctx.plugins.size() > 0) {
       ss << (ctx.plugins.size() == 1 ?  kBadPluginIntro : kBadPluginsIntro);
       for (const auto& p : ctx.plugins) {
          ss << "    " << p << "\n";
       }
       ss << (ctx.plugins.size() == 1 ? kBadPluginFooter: kBadPluginsFooter);
    }
    if (ctx.libs.size() > 0 ) {
       ss << (ctx.libs.size() == 1 ?  kBadLibIntro : kBadLibsIntro);
       for (const auto& lib : ctx.libs) {
          ss << "    " << lib << "\n";
       }
       ss << (ctx.libs.size() == 1 ?  kBadLibFooter : kBadLibsFooter);
    }	
    return ss.str();
  }

  LoadErrorsDlg(wxWindow* parent, const FormatCtx format_ctx) 
      : OCPNMessageDialog(parent, FormatMsg(format_ctx)) {}

public:
  static void Run(wxWindow* parent, const std::vector<LoadError> errors) {
    if (errors.size() == 0) return;
    FormatCtx format_ctx(errors);
    auto dlg = new LoadErrorsDlg(parent, format_ctx);
    int sts = dlg->ShowModal();

    if (sts == wxID_YES || sts == wxID_OK) {
      for (const auto& plugin : format_ctx.plugins) {
        PluginHandler::getInstance()->uninstall(plugin);
      }
      for (const auto& lib : format_ctx.libs)  remove(lib.c_str());
    } 
    dlg->Destroy();
  }
};

#endif  // LOAD_ERROR_DLG_H___
