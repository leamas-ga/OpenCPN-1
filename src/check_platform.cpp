/******************************************************************************
 *
 * Project:  OpenCPN
 *
 * Purpose: Warning message for platforms without plugins
 *
 ***************************************************************************
 *   Copyright (C) 2021 Alec Leamas                                        *
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
 ***************************************************************************
 */

#include "config.h"

#include <string>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/stattext.h>

#include "check_platform.h"

extern bool g_hide_no_plugins_dlg;

static const char* const MESSAGE = _(R"""(
Unfortunately, there are no plugins available for this OpenCPN
package. However, by installing OpenCPN using Flatpak instead
plugins are available. See the User Manual Getting Started
chapter.
)""");

static const char* const HIDE_DIALOG_LABEL =
    _("Do not show this dialog next time");

/** The "Dont show this message next time" checkbox. */
struct HideCheckbox : public wxCheckBox {
  HideCheckbox(wxWindow* parent, const char* label)
      : wxCheckBox(parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize,
                   wxALIGN_RIGHT) {
    SetValue(g_hide_no_plugins_dlg);
    Bind(wxEVT_CHECKBOX,
         [&](wxCommandEvent& ev) { g_hide_no_plugins_dlg = ev.IsChecked(); });
  }
};

/**  Line with  "Don't show this message..." checkbox  */
struct HidePanel : wxPanel {
  HidePanel(wxWindow* parent, const char* label)
      : wxPanel(parent) {
    auto hbox = new wxBoxSizer(wxHORIZONTAL);
    hbox->Add(1, 1, 100, wxEXPAND);  // Expanding spacer
    hbox->Add(new HideCheckbox(this, label), wxSizerFlags().Expand());
    SetSizer(hbox);
    Fit();
    Show();
  }
};

/** The OK button. */
struct Buttons : public wxStdDialogButtonSizer  {
  Buttons(wxWindow* parent): wxStdDialogButtonSizer() {
    auto ok = new wxButton(parent, wxID_OK, _("OK"));
    SetAffirmativeButton(ok);
    Realize();
  }
};

/** Main top-level dialog. */
class NoPluginsDialog : public wxDialog {
public:
  NoPluginsDialog(wxWindow* parent)
      : wxDialog(parent, wxID_ANY, _("No plugins available")) {
    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto flags = wxSizerFlags().Expand().Border();
    sizer->Add(new wxStaticText(this, wxID_ANY, MESSAGE), flags);
    sizer->Add(new wxStaticLine(this), flags);
    sizer->Add(new HidePanel(this, HIDE_DIALOG_LABEL),
               flags.Right());
    sizer->Add(new wxStaticLine(this), wxSizerFlags().Expand());
    sizer->Add(new Buttons(this), wxSizerFlags().Border());
    SetSizer(sizer);
    Layout();
    Show();
  }
};

#ifdef FLATPAK
// On Flatpak plugins are always available.
void check_platform(wxWindow* parent) {}

#else
void check_platform(wxWindow* parent) {
  if (g_hide_no_plugins_dlg) return;
  std::string target(PKG_TARGET);
  if (target.find("aarch64") == std::string::npos) return;
  auto dlg = new NoPluginsDialog(parent);
  dlg->ShowModal();
}
#endif
