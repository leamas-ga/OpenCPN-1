/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  PlugIn Manager Object
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
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

#include "config.h"

#ifdef __MINGW32__
#undef IPV6STRICT  // mingw FTBS fix:  missing struct ip_mreq
#include <windows.h>
#endif

#include <string>

#include <wx/bitmap.h>
#include <wx/font.h>
#include <wx/menuitem.h>
#include <wx/scrolwin.h>
#include <wx/string.h>

#include "ocpn_plugin.h"



//----------------------------------------------------------------------------------------------------------
//    The PlugIn CallBack API Implementation, linkable shim
//    The definitions of this API are found in ocpn_plugin.h,
//    the real stuff is in plugin_api.cpp
//----------------------------------------------------------------------------------------------------------

int InsertPlugInTool(wxString label, wxBitmap* bitmap, wxBitmap* bmpRollover,
                     wxItemKind kind, wxString shortHelp, wxString longHelp,
                     wxObject* clientData, int position, int tool_sel,
                     opencpn_plugin* pplugin) { return 0; }

void RemovePlugInTool(int tool_id) {}

void SetToolbarToolViz(int item, bool viz) {}

void SetToolbarItemState(int item, bool toggle) {}

void SetToolbarToolBitmaps(int item, wxBitmap* bitmap, wxBitmap* bmpRollover)
{}

int InsertPlugInToolSVG(wxString label, wxString SVGfile,
                        wxString SVGfileRollover, wxString SVGfileToggled,
                        wxItemKind kind, wxString shortHelp, wxString longHelp,
                        wxObject* clientData, int position, int tool_sel,
                        opencpn_plugin* pplugin) { return 0; }

void SetToolbarToolBitmapsSVG(int item, wxString SVGfile,
                              wxString SVGfileRollover,
                              wxString SVGfileToggled) {}

int AddCanvasMenuItem(wxMenuItem* pitem, opencpn_plugin* pplugin,
                      const char* name) { return 0; }

void SetCanvasMenuItemViz(int item, bool viz, const char* name) { }

void SetCanvasMenuItemGrey(int item, bool grey, const char* name) {}

void RemoveCanvasMenuItem(int item, const char* name) {}

int AddCanvasContextMenuItem(wxMenuItem* pitem, opencpn_plugin* pplugin) {
  return 0;
}

void SetCanvasContextMenuItemViz(int item, bool viz) {}

void SetCanvasContextMenuItemGrey(int item, bool grey) {}

void RemoveCanvasContextMenuItem(int item) {}

wxFileConfig* GetOCPNConfigObject(void) { return 0; }

wxWindow* GetOCPNCanvasWindow() { return 0; }

void RequestRefresh(wxWindow* win) {}

void GetCanvasPixLL(PlugIn_ViewPort* vp, wxPoint* pp, double lat, double lon) {}

void GetDoubleCanvasPixLL(PlugIn_ViewPort* vp, wxPoint2DDouble* pp, double lat,
                          double lon) {}

void GetCanvasLLPix(PlugIn_ViewPort* vp, wxPoint p, double* plat,
                    double* plon) {}

bool GetGlobalColor(wxString colorName, wxColour* pcolour) { return true; }

wxFont* OCPNGetFont(wxString TextElement, int default_size) { return 0; }

wxFont* GetOCPNScaledFont_PlugIn(wxString TextElement, int default_size) {
  return 0;
}

double GetOCPNGUIToolScaleFactor_PlugIn(int GUIScaleFactor) { return 0; }

double GetOCPNGUIToolScaleFactor_PlugIn() { return 0; }

float GetOCPNChartScaleFactor_Plugin() { return 0.0; }

wxFont GetOCPNGUIScaledFont_PlugIn(wxString item) {
   wxFont f;
   return f;
}

bool AddPersistentFontKey(wxString TextElement) { return true; }

wxString GetActiveStyleName() { return ""; }

wxBitmap GetBitmapFromSVGFile(wxString filename, unsigned int width,
                              unsigned int height) {
  return wxBitmap(1, 1);
}

bool IsTouchInterface_PlugIn(void) { return true; }

wxColour GetFontColour_PlugIn(wxString TextElement) {
  wxColour c;
  return c;
}

wxString* GetpSharedDataLocation(void) { return 0; }

wxString* GetpPrivateApplicationDataLocation(void) { return 0; }

ArrayOfPlugIn_AIS_Targets* GetAISTargetArray(void) { return 0; }

wxAuiManager* GetFrameAuiManager(void) { return 0; }

bool AddLocaleCatalog(wxString catalog) { return true; }

void PushNMEABuffer(wxString buf) {}

wxXmlDocument GetChartDatabaseEntryXML(int dbIndex, bool b_getGeom) {
  wxXmlDocument doc;
  return doc;
}

bool UpdateChartDBInplace(wxArrayString dir_array, bool b_force_update,
                          bool b_ProgressDialog) {
  return true;
}

wxArrayString GetChartDBDirArrayString() {
  wxArrayString was;
  return was;
}

int AddChartToDBInPlace(wxString &full_path, bool b_RefreshCanvas) {
  return 0;
}

int RemoveChartFromDBInPlace(wxString &full_path) { return 0; }

wxString GetLocaleCanonicalName() { return ""; }

void SendPluginMessage(wxString message_id, wxString message_body) {}

void DimeWindow(wxWindow* win) {}

void JumpToPosition(double lat, double lon, double scale) {}

/* API 1.9 */
wxScrolledWindow* AddOptionsPage(OptionsParentPI parent, wxString title) {
  return 0;
}
bool DeleteOptionsPage(wxScrolledWindow* page) { return true; }
bool DecodeSingleVDOMessage(const wxString &str, PlugIn_Position_Fix_Ex* pos,
                            wxString* accumulator) {
  return true;
}

int GetChartbarHeight(void) { return 0; }

class RoutePoint;
bool GetRoutepointGPX(RoutePoint* pRoutePoint, char* buffer,
                      unsigned int buffer_length) {
  return true;
}

bool GetActiveRoutepointGPX(char* buffer, unsigned int buffer_length) {
  return true;
}
void PositionBearingDistanceMercator_Plugin(double lat, double lon, double brg,
                                            double dist, double* dlat,
                                            double* dlon) {
}

void DistanceBearingMercator_Plugin(double lat0, double lon0, double lat1,
                                    double lon1, double* brg, double* dist) {
}

double DistGreatCircle_Plugin(double slat, double slon, double dlat,
                              double dlon) {
  return 0;
}

void toTM_Plugin(float lat, float lon, float lat0, float lon0, double* x,
                 double* y) {
}

void fromTM_Plugin(double x, double y, double lat0, double lon0, double* lat,
                   double* lon) {
}

void toSM_Plugin(double lat, double lon, double lat0, double lon0, double* x,
                 double* y) {
}

void fromSM_Plugin(double x, double y, double lat0, double lon0, double* lat,
                   double* lon) {
}

void toSM_ECC_Plugin(double lat, double lon, double lat0, double lon0,
                     double* x, double* y) {
}

void fromSM_ECC_Plugin(double x, double y, double lat0, double lon0,
                       double* lat, double* lon) {
}

double toUsrDistance_Plugin(double nm_distance, int unit) {
  return 0;
}

double fromUsrDistance_Plugin(double usr_distance, int unit) {
  return 0;
}

double toUsrSpeed_Plugin(double kts_speed, int unit) {
  return 0;
}

double fromUsrSpeed_Plugin(double usr_speed, int unit) {
  return 0;
}

double toUsrTemp_Plugin(double cel_temp, int unit) {
  return 0;
}

double fromUsrTemp_Plugin(double usr_temp, int unit) {
  return 0;
}

wxString getUsrDistanceUnit_Plugin(int unit) { return ""; }

wxString getUsrSpeedUnit_Plugin(int unit) { return ""; }

wxString getUsrTempUnit_Plugin(int unit) { return ""; }

bool PlugIn_GSHHS_CrossesLand(double lat1, double lon1, double lat2,
                              double lon2) {
  return true;
}

void PlugInPlaySound(wxString &sound_file) {}

wxString GetNewGUID(void) { return ""; }

bool AddCustomWaypointIcon(wxBitmap* pimage, wxString key,
                           wxString description) {
  return true;
}

static void cloneHyperlinkList(RoutePoint* dst, const PlugIn_Waypoint* src) {}

bool AddSingleWaypoint(PlugIn_Waypoint* pwaypoint, bool b_permanent) {
  return true;
}

bool DeleteSingleWaypoint(wxString &GUID) { return true; }

class PlugIn_Waypoint;
bool UpdateSingleWaypoint(PlugIn_Waypoint* pwaypoint) { return true; }

bool GetSingleWaypoint(wxString GUID, PlugIn_Waypoint* pwaypoint) {
  return true;
}

wxArrayString GetWaypointGUIDArray(void) {
  wxArrayString result;
  return result;
}

wxArrayString GetRouteGUIDArray(void) {
  wxArrayString result;

  return result;
}

wxArrayString GetTrackGUIDArray(void) {
  wxArrayString result;
  return result;
}

wxArrayString GetIconNameArray(void) {
  wxArrayString result;
  return result;
}

bool AddPlugInRoute(PlugIn_Route* proute, bool b_permanent) { return true; }

bool DeletePlugInRoute(wxString &GUID) { return true; }

bool UpdatePlugInRoute(PlugIn_Route* proute) { return true; }

bool AddPlugInTrack(PlugIn_Track* ptrack, bool b_permanent) { return true; }

bool DeletePlugInTrack(wxString &GUID) { return true; }

class PlugIn_Track;
bool UpdatePlugInTrack(PlugIn_Track* ptrack) { return true; }

class PlugIn_ViewPort;
bool PlugInHasNormalizedViewPort(PlugIn_ViewPort* vp) { return true; }

void PlugInMultMatrixViewport(PlugIn_ViewPort* vp, float lat, float lon) {}
void PlugInNormalizeViewport(PlugIn_ViewPort* vp, float lat, float lon) {}
