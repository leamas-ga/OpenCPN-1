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

#include <wx/event.h>
#include <wx/window.h>

#include "load_errors_dlg.h"
#include "plugin_loader.h"
#include "plugin_handler.h"
#include "mvc_gui_control.h"

wxDEFINE_EVENT(EVT_LOAD_COMPLETE, wxCommandEvent);



MvcGuiControl::MvcGuiControl(wxWindow* parent) : m_parent(parent) {
  auto loader = PluginLoader::getInstance();

  load_complete_listener.Listen(loader->evt_plugin_loadall_finalize, this,
                                EVT_LOAD_COMPLETE);
  Bind(EVT_LOAD_COMPLETE, [&](wxCommandEvent& ev) {
    LoadErrorsDlg::Run(m_parent, PluginLoader::getInstance()->LoadErrors());
  });
}
