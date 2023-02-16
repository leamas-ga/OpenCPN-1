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

#include "mvc_gui_control.h"
#include "plugin_loader.h"
#include "bad_plugin_dlg.h"
#include "bad_library_dlg.h"
#include "bad_plugvers_dlg.h"
#include "bad_symbol_dlg.h"

wxDEFINE_EVENT(EVT_BAD_LIBRARY, wxCommandEvent);
wxDEFINE_EVENT(EVT_BAD_PLUGIN, wxCommandEvent);
wxDEFINE_EVENT(EVT_INCOMPAT_PLUGIN, ObservedEvt);
wxDEFINE_EVENT(EVT_NO_CREATE_PLUGIN, ObservedEvt);
wxDEFINE_EVENT(EVT_NO_DESTROY_PLUGIN, ObservedEvt);

/* Experimental, probably to be nuked. FIXME (leamas)
template<typename T>
void MvcGuiControl::ListenSetup(ObservableListener& listener,
                                T ev_type,
                                EventVar& event_var,
                                std::function<void(ObservedEvt&)> lambda) {
  listener.Listen(event_var, this, ev_type);
  Bind(ev_type, lambda);
}
*/



MvcGuiControl::MvcGuiControl(wxWindow* parent) : m_parent(parent) {
  auto loader = PluginLoader::getInstance();

  bad_library_listener.Listen(loader->evt_unloadable_lib, this,
                              EVT_BAD_LIBRARY);
  unreadable_lib_listener.Listen(loader->evt_unreadable_plugin, this,
                                 EVT_BAD_LIBRARY);
  Bind(EVT_BAD_LIBRARY, [&](wxCommandEvent& ev) {
       BadLibraryDlg::Run(m_parent, ev.GetString().ToStdString()); });

  bad_plugin_listener.Listen(loader->evt_unloadable_plugin, this,
                             EVT_BAD_PLUGIN);
  Bind(EVT_BAD_PLUGIN, [&](wxCommandEvent& ev) {
       BadPluginDlg::Run(m_parent, ev.GetString().ToStdString()); });

  incompat_lib_listener.Listen(loader->evt_incompat_plugin, this,
                               EVT_INCOMPAT_PLUGIN);
  Bind(EVT_INCOMPAT_PLUGIN, [&](ObservedEvt& ev) {
       auto ptr = UnpackEvtPointer<PluginInfo>(ev);
       IncompatPluginDlg::Run(m_parent, ptr); });

  no_createpi_listener.Listen(loader->evt_no_create_plugin, this,
                              EVT_NO_CREATE_PLUGIN);
  Bind(EVT_NO_CREATE_PLUGIN, [&](wxCommandEvent& ev) {
       BadSymbolDlg::Run(m_parent, ev.GetString().ToStdString(), "create_pi"); });

  no_destroy_listener.Listen(loader->evt_no_destroy_plugin, this,
                             EVT_NO_DESTROY_PLUGIN);
  Bind(EVT_NO_DESTROY_PLUGIN, [&](wxCommandEvent& ev) {
       BadSymbolDlg::Run(m_parent, ev.GetString().ToStdString(), "destroy_pi"); });
}
