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

#include <stdint.h>
#include <cstdio>
#include <cstdio>
#include <string>

#include <wx/tokenzr.h>

#ifdef ocpnUSE_SVG
#include <wxSVG/svg.h>
#endif  // ocpnUSE_SVG

#include "AIS_Decoder.h"
#include "ais.h"
#include "canvasMenu.h"
#include "catalog_handler.h"
#include "chartbase.h"  // for ChartPlugInWrapper
#include "chartdb.h"
#include "chcanv.h"
#include "config_var.h"
#include "dychart.h"
#include "FontMgr.h"
#include "logger.h"
#include "multiplexer.h"
#include "NavObjectCollection.h"
#include "navutil.h"
#include "OCPN_AUIManager.h"
#include "OCPN_DataStreamEvent.h"
#include "ocpndc.h"
#include "OCPNPlatform.h"
#include "ocpn_utils.h"
#include "options.h"
#include "piano.h"
#include "plugin_cache.h"
#include "PluginHandler.h"
#include "pluginmanager.h"
#include "Route.h"
#include "routemanagerdialog.h"
#include "routeman.h"
#include "s52plib.h"
#include "styles.h"
#include "toolbar.h"
#include "Track.h"

#ifdef __OCPN__ANDROID__
#include "androidUTIL.h"
#endif

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

extern wxImage LoadSVGIcon(wxString filename, int width, int height);

extern MyConfig *pConfig;
extern AIS_Decoder *g_pAIS;
extern OCPN_AUIManager *g_pauimgr;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
extern wxLocale *plocale_def_lang;
#endif

extern OCPNPlatform *g_Platform;
extern ChartDB *ChartData;
extern MyFrame *gFrame;
extern ocpnStyle::StyleManager *g_StyleManager;
extern options *g_pOptions;
extern Multiplexer *g_pMUX;
extern bool g_bShowChartBar;
extern Routeman *g_pRouteMan;
extern WayPointman *pWayPointMan;
extern Select *pSelect;
extern RouteManagerDialog *pRouteManagerDialog;
extern RouteList *pRouteList;
extern TrackList *pTrackList;
extern PlugInManager *g_pi_manager;
extern s52plib *ps52plib;
extern wxString ChartListFileName;
extern bool g_boptionsactive;
extern options *g_options;
extern ColorScheme global_color_scheme;
extern wxArrayString g_locale_catalog_array;
extern int g_GUIScaleFactor;
extern int g_ChartScaleFactor;
extern wxString g_locale;
extern bool g_btouch;
extern ocpnFloatingToolbarDialog *g_MainToolbar;

extern int g_chart_zoom_modifier;
extern int g_chart_zoom_modifier_vector;
extern double g_display_size_mm;
extern bool g_bopengl;

extern ChartGroupArray *g_pGroupArray;
extern unsigned int g_canvasConfig;

extern wxString g_CmdSoundString;

extern int g_iSDMMFormat;

extern unsigned int gs_plib_flags;
extern wxString g_lastPluginMessage;
extern ChartCanvas *g_focusCanvas;
extern ChartCanvas *g_overlayCanvas;
extern bool g_bquiting;
extern wxString g_catalog_channel;
extern wxString g_catalog_custom_url;

extern PlugInManager *s_ppim;

WX_DEFINE_ARRAY_PTR(ChartCanvas *, arrayofCanvasPtr);
extern arrayofCanvasPtr g_canvasArray;

//----------------------------------------------------------------------------------------------------------
//    The PlugIn CallBack API Implementation
//    The definitions of this API are found in ocpn_plugin.h
//----------------------------------------------------------------------------------------------------------

int InsertPlugInTool(wxString label, wxBitmap *bitmap, wxBitmap *bmpRollover,
                     wxItemKind kind, wxString shortHelp, wxString longHelp,
                     wxObject *clientData, int position, int tool_sel,
                     opencpn_plugin *pplugin) {
  if (s_ppim)
    return s_ppim->AddToolbarTool(label, bitmap, bmpRollover, kind, shortHelp,
                                  longHelp, clientData, position, tool_sel,
                                  pplugin);
  else
    return -1;
}

void RemovePlugInTool(int tool_id) {
  if (s_ppim) s_ppim->RemoveToolbarTool(tool_id);
}

void SetToolbarToolViz(int item, bool viz) {
  if (s_ppim) s_ppim->SetToolbarToolViz(item, viz);
}

void SetToolbarItemState(int item, bool toggle) {
  if (s_ppim) s_ppim->SetToolbarItemState(item, toggle);
}

void SetToolbarToolBitmaps(int item, wxBitmap *bitmap, wxBitmap *bmpRollover) {
  if (s_ppim) s_ppim->SetToolbarItemBitmaps(item, bitmap, bmpRollover);
}

int InsertPlugInToolSVG(wxString label, wxString SVGfile,
                        wxString SVGfileRollover, wxString SVGfileToggled,
                        wxItemKind kind, wxString shortHelp, wxString longHelp,
                        wxObject *clientData, int position, int tool_sel,
                        opencpn_plugin *pplugin) {
  if (s_ppim)
    return s_ppim->AddToolbarTool(label, SVGfile, SVGfileRollover,
                                  SVGfileToggled, kind, shortHelp, longHelp,
                                  clientData, position, tool_sel, pplugin);
  else
    return -1;
}

void SetToolbarToolBitmapsSVG(int item, wxString SVGfile,
                              wxString SVGfileRollover,
                              wxString SVGfileToggled) {
  if (s_ppim)
    s_ppim->SetToolbarItemBitmaps(item, SVGfile, SVGfileRollover,
                                  SVGfileToggled);
}

int AddCanvasMenuItem(wxMenuItem *pitem, opencpn_plugin *pplugin,
                      const char *name) {
  if (s_ppim)
    return s_ppim->AddCanvasContextMenuItem(pitem, pplugin, name);
  else
    return -1;
}

void SetCanvasMenuItemViz(int item, bool viz, const char *name) {
  if (s_ppim) s_ppim->SetCanvasContextMenuItemViz(item, viz, name);
}

void SetCanvasMenuItemGrey(int item, bool grey, const char *name) {
  if (s_ppim) s_ppim->SetCanvasContextMenuItemGrey(item, grey, name);
}

void RemoveCanvasMenuItem(int item, const char *name) {
  if (s_ppim) s_ppim->RemoveCanvasContextMenuItem(item, name);
}

int AddCanvasContextMenuItem(wxMenuItem *pitem, opencpn_plugin *pplugin) {
  /* main context popup menu */
  return AddCanvasMenuItem(pitem, pplugin, "");
}

void SetCanvasContextMenuItemViz(int item, bool viz) {
  SetCanvasMenuItemViz(item, viz);
}

void SetCanvasContextMenuItemGrey(int item, bool grey) {
  SetCanvasMenuItemGrey(item, grey);
}

void RemoveCanvasContextMenuItem(int item) { RemoveCanvasMenuItem(item); }

wxFileConfig *GetOCPNConfigObject(void) {
  if (s_ppim)
    return pConfig;  // return the global application config object
  else
    return NULL;
}

wxWindow *GetOCPNCanvasWindow() {
  wxWindow *pret = NULL;
  if (s_ppim) {
    MyFrame *pFrame = s_ppim->GetParentFrame();
    pret = (wxWindow *)pFrame->GetPrimaryCanvas();
  }
  return pret;
}

void RequestRefresh(wxWindow *win) {
  if (win) win->Refresh();
}

void GetCanvasPixLL(PlugIn_ViewPort *vp, wxPoint *pp, double lat, double lon) {
  //    Make enough of an application viewport to run its method....
  ViewPort ocpn_vp;
  ocpn_vp.clat = vp->clat;
  ocpn_vp.clon = vp->clon;
  ocpn_vp.m_projection_type = vp->m_projection_type;
  ocpn_vp.view_scale_ppm = vp->view_scale_ppm;
  ocpn_vp.skew = vp->skew;
  ocpn_vp.rotation = vp->rotation;
  ocpn_vp.pix_width = vp->pix_width;
  ocpn_vp.pix_height = vp->pix_height;

  wxPoint ret = ocpn_vp.GetPixFromLL(lat, lon);
  pp->x = ret.x;
  pp->y = ret.y;
}

void GetDoubleCanvasPixLL(PlugIn_ViewPort *vp, wxPoint2DDouble *pp, double lat,
                          double lon) {
  //    Make enough of an application viewport to run its method....
  ViewPort ocpn_vp;
  ocpn_vp.clat = vp->clat;
  ocpn_vp.clon = vp->clon;
  ocpn_vp.m_projection_type = vp->m_projection_type;
  ocpn_vp.view_scale_ppm = vp->view_scale_ppm;
  ocpn_vp.skew = vp->skew;
  ocpn_vp.rotation = vp->rotation;
  ocpn_vp.pix_width = vp->pix_width;
  ocpn_vp.pix_height = vp->pix_height;

  *pp = ocpn_vp.GetDoublePixFromLL(lat, lon);
}

void GetCanvasLLPix(PlugIn_ViewPort *vp, wxPoint p, double *plat,
                    double *plon) {
  //    Make enough of an application viewport to run its method....
  ViewPort ocpn_vp;
  ocpn_vp.clat = vp->clat;
  ocpn_vp.clon = vp->clon;
  ocpn_vp.m_projection_type = vp->m_projection_type;
  ocpn_vp.view_scale_ppm = vp->view_scale_ppm;
  ocpn_vp.skew = vp->skew;
  ocpn_vp.rotation = vp->rotation;
  ocpn_vp.pix_width = vp->pix_width;
  ocpn_vp.pix_height = vp->pix_height;

  return ocpn_vp.GetLLFromPix(p, plat, plon);
}

bool GetGlobalColor(wxString colorName, wxColour *pcolour) {
  wxColour c = GetGlobalColor(colorName);
  *pcolour = c;

  return true;
}

wxFont *OCPNGetFont(wxString TextElement, int default_size) {
  return FontMgr::Get().GetFont(TextElement, default_size);
}

wxFont *GetOCPNScaledFont_PlugIn(wxString TextElement, int default_size) {
  return GetOCPNScaledFont(TextElement, default_size);
}

double GetOCPNGUIToolScaleFactor_PlugIn(int GUIScaleFactor) {
  return g_Platform->GetToolbarScaleFactor(GUIScaleFactor);
}

double GetOCPNGUIToolScaleFactor_PlugIn() {
  return g_Platform->GetToolbarScaleFactor(g_GUIScaleFactor);
}

float GetOCPNChartScaleFactor_Plugin() {
  return g_Platform->getChartScaleFactorExp(g_ChartScaleFactor);
}

wxFont GetOCPNGUIScaledFont_PlugIn(wxString item) {
  return GetOCPNGUIScaledFont(item);
}

bool AddPersistentFontKey(wxString TextElement) {
  return FontMgr::Get().AddAuxKey(TextElement);
}

wxString GetActiveStyleName() {
  if (g_StyleManager)
    return g_StyleManager->GetCurrentStyle()->name;
  else
    return _T("");
}

wxBitmap GetBitmapFromSVGFile(wxString filename, unsigned int width,
                              unsigned int height) {
#ifdef ocpnUSE_SVG
#ifndef __OCPN__ANDROID__
  wxSVGDocument svgDoc;
  if ((width > 0) && (height > 0) && svgDoc.Load(filename))
    return wxBitmap(svgDoc.Render(width, height, NULL, false, true));
  else
    return wxBitmap(1, 1);
#else
  return loadAndroidSVG(filename, width, height);
#endif
#else
  return wxBitmap(width, height);
#endif  // ocpnUSE_SVG
}

bool IsTouchInterface_PlugIn(void) { return g_btouch; }

wxColour GetFontColour_PlugIn(wxString TextElement) {
  return FontMgr::Get().GetFontColor(TextElement);
}

wxString *GetpSharedDataLocation(void) {
  return g_Platform->GetSharedDataDirPtr();
}

wxString *GetpPrivateApplicationDataLocation(void) {
  return g_Platform->GetPrivateDataDirPtr();
}

ArrayOfPlugIn_AIS_Targets *GetAISTargetArray(void) {
  if (!g_pAIS) return NULL;

  ArrayOfPlugIn_AIS_Targets *pret = new ArrayOfPlugIn_AIS_Targets;

  //      Iterate over the AIS Target Hashmap
  AIS_Target_Hash::iterator it;

  AIS_Target_Hash *current_targets = g_pAIS->GetTargetList();

  for (it = (*current_targets).begin(); it != (*current_targets).end(); ++it) {
    AIS_Target_Data *td = it->second;
    PlugIn_AIS_Target *ptarget = Create_PI_AIS_Target(td);
    pret->Add(ptarget);
  }

//  Test one alarm target
#if 0
    AIS_Target_Data td;
    td.n_alarm_state = AIS_ALARM_SET;
    PlugIn_AIS_Target *ptarget = Create_PI_AIS_Target(&td);
    pret->Add(ptarget);
#endif
  return pret;
}

wxAuiManager *GetFrameAuiManager(void) { return g_pauimgr; }

bool AddLocaleCatalog(wxString catalog) {
#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)

  if (plocale_def_lang) {
    // Add this catalog to the persistent catalog array
    g_locale_catalog_array.Add(catalog);

    return plocale_def_lang->AddCatalog(catalog);
  } else
#endif
    return false;
}

void PushNMEABuffer(wxString buf) {
  OCPN_DataStreamEvent event(wxEVT_OCPN_DATASTREAM, 0);
  std::string s = std::string(buf.mb_str());
  event.SetNMEAString(s);
  event.SetStream(NULL);

  g_pMUX->AddPendingEvent(event);
}

wxXmlDocument GetChartDatabaseEntryXML(int dbIndex, bool b_getGeom) {
  wxXmlDocument doc = ChartData->GetXMLDescription(dbIndex, b_getGeom);

  return doc;
}

bool UpdateChartDBInplace(wxArrayString dir_array, bool b_force_update,
                          bool b_ProgressDialog) {
  //    Make an array of CDI
  ArrayOfCDI ChartDirArray;
  for (unsigned int i = 0; i < dir_array.GetCount(); i++) {
    wxString dirname = dir_array[i];
    ChartDirInfo cdi;
    cdi.fullpath = dirname;
    cdi.magic_number = _T("");
    ChartDirArray.Add(cdi);
  }

  bool b_ret = gFrame->UpdateChartDatabaseInplace(ChartDirArray, b_force_update,
                                                  b_ProgressDialog,
                                                  ChartData->GetDBFileName());

  gFrame->ChartsRefresh();

  return b_ret;
}

wxArrayString GetChartDBDirArrayString() {
  return ChartData->GetChartDirArrayString();
}

int AddChartToDBInPlace(wxString &full_path, bool b_RefreshCanvas) {
  // extract the path from the chart name
  wxFileName fn(full_path);
  wxString fdir = fn.GetPath();

  bool bret = false;
  if (ChartData) {
    bret = ChartData->AddSingleChart(full_path);

    if (bret) {
      // Save to disk
      pConfig->UpdateChartDirs(ChartData->GetChartDirArray());
      ChartData->SaveBinary(ChartListFileName);

      //  Completely reload the chart database, for a fresh start
      ArrayOfCDI XnewChartDirArray;
      pConfig->LoadChartDirArray(XnewChartDirArray);
      delete ChartData;
      ChartData = new ChartDB();
      ChartData->LoadBinary(ChartListFileName, XnewChartDirArray);

      // Update group contents
      if (g_pGroupArray) ChartData->ApplyGroupArray(g_pGroupArray);

      if (g_boptionsactive) {
        g_options->UpdateDisplayedChartDirList(ChartData->GetChartDirArray());
      }

      if (b_RefreshCanvas || !gFrame->GetPrimaryCanvas()->GetQuiltMode()) {
        gFrame->ChartsRefresh();
      }
    }
  }
  return bret;
}

int RemoveChartFromDBInPlace(wxString &full_path) {
  bool bret = false;
  if (ChartData) {
    bret = ChartData->RemoveSingleChart(full_path);

    // Save to disk
    pConfig->UpdateChartDirs(ChartData->GetChartDirArray());
    ChartData->SaveBinary(ChartListFileName);

    //  Completely reload the chart database, for a fresh start
    ArrayOfCDI XnewChartDirArray;
    pConfig->LoadChartDirArray(XnewChartDirArray);
    delete ChartData;
    ChartData = new ChartDB();
    ChartData->LoadBinary(ChartListFileName, XnewChartDirArray);

    // Update group contents
    if (g_pGroupArray) ChartData->ApplyGroupArray(g_pGroupArray);

    if (g_boptionsactive) {
      g_options->UpdateDisplayedChartDirList(ChartData->GetChartDirArray());
    }

    gFrame->ChartsRefresh();
  }

  return bret;
}

wxString GetLocaleCanonicalName() { return g_locale; }

void SendPluginMessage(wxString message_id, wxString message_body) {
  s_ppim->SendMessageToAllPlugins(message_id, message_body);

  //  We will send an event to the main application frame (gFrame)
  //  for informational purposes.
  //  Of course, gFrame is encouraged to use any or all the
  //  data flying by if judged useful and dependable....

  OCPN_MsgEvent Nevent(wxEVT_OCPN_MSG, 0);
  Nevent.SetID(message_id);
  Nevent.SetJSONText(message_body);
  gFrame->GetEventHandler()->AddPendingEvent(Nevent);
}

void DimeWindow(wxWindow *win) { DimeControl(win); }

void JumpToPosition(double lat, double lon, double scale) {
  gFrame->JumpToPosition(gFrame->GetFocusCanvas(), lat, lon, scale);
}

/* API 1.9 */
wxScrolledWindow *AddOptionsPage(OptionsParentPI parent, wxString title) {
  if (!g_pOptions) return NULL;

  size_t parentid;
  switch (parent) {
    case PI_OPTIONS_PARENT_DISPLAY:
      parentid = g_pOptions->m_pageDisplay;
      break;
    case PI_OPTIONS_PARENT_CONNECTIONS:
      parentid = g_pOptions->m_pageConnections;
      break;
    case PI_OPTIONS_PARENT_CHARTS:
      parentid = g_pOptions->m_pageCharts;
      break;
    case PI_OPTIONS_PARENT_SHIPS:
      parentid = g_pOptions->m_pageShips;
      break;
    case PI_OPTIONS_PARENT_UI:
      parentid = g_pOptions->m_pageUI;
      break;
    case PI_OPTIONS_PARENT_PLUGINS:
      parentid = g_pOptions->m_pagePlugins;
      break;
    default:
      wxLogMessage(
          _T("Error in PluginManager::AddOptionsPage: Unknown parent"));
      return NULL;
      break;
  }

  return g_pOptions->AddPage(parentid, title);
}

bool DeleteOptionsPage(wxScrolledWindow *page) {
  if (!g_pOptions) return false;
  return g_pOptions->DeletePluginPage(page);
}

bool DecodeSingleVDOMessage(const wxString &str, PlugIn_Position_Fix_Ex *pos,
                            wxString *accumulator) {
  if (!pos) return false;

  GenericPosDatEx gpd;
  AIS_Error nerr = AIS_GENERIC_ERROR;
  if (g_pAIS) nerr = g_pAIS->DecodeSingleVDO(str, &gpd, accumulator);
  if (nerr == AIS_NoError) {
    pos->Lat = gpd.kLat;
    pos->Lon = gpd.kLon;
    pos->Cog = gpd.kCog;
    pos->Sog = gpd.kSog;
    pos->Hdt = gpd.kHdt;

    //  Fill in the dummy values
    pos->FixTime = 0;
    pos->Hdm = 1000;
    pos->Var = 1000;
    pos->nSats = 0;

    return true;
  }

  return false;
}

int GetChartbarHeight(void) {
  int val = 0;
  if (g_bShowChartBar) {
    ChartCanvas *cc = gFrame->GetPrimaryCanvas();
    if (cc && cc->GetPiano()) {
      val = cc->GetPiano()->GetHeight();
    }
  }
  return val;
}

bool GetRoutepointGPX(RoutePoint *pRoutePoint, char *buffer,
                      unsigned int buffer_length) {
  bool ret = false;

  NavObjectCollection1 *pgpx = new NavObjectCollection1;
  pgpx->AddGPXWaypoint(pRoutePoint);
  wxString gpxfilename = wxFileName::CreateTempFileName(wxT("gpx"));
  pgpx->SaveFile(gpxfilename);
  delete pgpx;

  wxFFile gpxfile(gpxfilename);
  wxString s;
  if (gpxfile.ReadAll(&s)) {
    if (s.Length() < buffer_length) {
      strncpy(buffer, (const char *)s.mb_str(wxConvUTF8), buffer_length - 1);
      ret = true;
    }
  }

  gpxfile.Close();
  ::wxRemoveFile(gpxfilename);

  return ret;
}

bool GetActiveRoutepointGPX(char *buffer, unsigned int buffer_length) {
  if (g_pRouteMan->IsAnyRouteActive())
    return GetRoutepointGPX(g_pRouteMan->GetpActivePoint(), buffer,
                            buffer_length);
  else
    return false;
}

void PositionBearingDistanceMercator_Plugin(double lat, double lon, double brg,
                                            double dist, double *dlat,
                                            double *dlon) {
  PositionBearingDistanceMercator(lat, lon, brg, dist, dlat, dlon);
}

void DistanceBearingMercator_Plugin(double lat0, double lon0, double lat1,
                                    double lon1, double *brg, double *dist) {
  DistanceBearingMercator(lat0, lon0, lat1, lon1, brg, dist);
}

double DistGreatCircle_Plugin(double slat, double slon, double dlat,
                              double dlon) {
  return DistGreatCircle(slat, slon, dlat, dlon);
}

void toTM_Plugin(float lat, float lon, float lat0, float lon0, double *x,
                 double *y) {
  toTM(lat, lon, lat0, lon0, x, y);
}

void fromTM_Plugin(double x, double y, double lat0, double lon0, double *lat,
                   double *lon) {
  fromTM(x, y, lat0, lon0, lat, lon);
}

void toSM_Plugin(double lat, double lon, double lat0, double lon0, double *x,
                 double *y) {
  toSM(lat, lon, lat0, lon0, x, y);
}

void fromSM_Plugin(double x, double y, double lat0, double lon0, double *lat,
                   double *lon) {
  fromSM(x, y, lat0, lon0, lat, lon);
}

void toSM_ECC_Plugin(double lat, double lon, double lat0, double lon0,
                     double *x, double *y) {
  toSM_ECC(lat, lon, lat0, lon0, x, y);
}

void fromSM_ECC_Plugin(double x, double y, double lat0, double lon0,
                       double *lat, double *lon) {
  fromSM_ECC(x, y, lat0, lon0, lat, lon);
}

double toUsrDistance_Plugin(double nm_distance, int unit) {
  return toUsrDistance(nm_distance, unit);
}

double fromUsrDistance_Plugin(double usr_distance, int unit) {
  return fromUsrDistance(usr_distance, unit);
}

double toUsrSpeed_Plugin(double kts_speed, int unit) {
  return toUsrSpeed(kts_speed, unit);
}

double fromUsrSpeed_Plugin(double usr_speed, int unit) {
  return fromUsrSpeed(usr_speed, unit);
}

double toUsrTemp_Plugin(double cel_temp, int unit) {
  return toUsrTemp(cel_temp, unit);
}

double fromUsrTemp_Plugin(double usr_temp, int unit) {
  return fromUsrTemp(usr_temp, unit);
}

wxString getUsrDistanceUnit_Plugin(int unit) {
  return getUsrDistanceUnit(unit);
}

wxString getUsrSpeedUnit_Plugin(int unit) { return getUsrSpeedUnit(unit); }

wxString getUsrTempUnit_Plugin(int unit) { return getUsrTempUnit(unit); }

bool PlugIn_GSHHS_CrossesLand(double lat1, double lon1, double lat2,
                              double lon2) {
  static bool loaded = false;
  if (!loaded) {
    gshhsCrossesLandInit();
    loaded = true;
  }

  return gshhsCrossesLand(lat1, lon1, lat2, lon2);
}

void PlugInPlaySound(wxString &sound_file) {
  PlugInPlaySoundEx(sound_file, -1);
}

// API 1.10 Route and Waypoint Support
// wxBitmap *FindSystemWaypointIcon( wxString& icon_name );

//      PlugInWaypoint implementation
PlugIn_Waypoint::PlugIn_Waypoint() { m_HyperlinkList = NULL; }

PlugIn_Waypoint::PlugIn_Waypoint(double lat, double lon,
                                 const wxString &icon_ident,
                                 const wxString &wp_name,
                                 const wxString &GUID) {
  wxDateTime now = wxDateTime::Now();
  m_CreateTime = now.ToUTC();
  m_HyperlinkList = NULL;

  m_lat = lat;
  m_lon = lon;
  m_IconName = icon_ident;
  m_MarkName = wp_name;
  m_GUID = GUID;
}

PlugIn_Waypoint::~PlugIn_Waypoint() {}

//      PlugInRoute implementation
PlugIn_Route::PlugIn_Route(void) { pWaypointList = new Plugin_WaypointList; }

PlugIn_Route::~PlugIn_Route(void) {
  pWaypointList->DeleteContents(false);  // do not delete Waypoints
  pWaypointList->Clear();

  delete pWaypointList;
}

//      PlugInTrack implementation
PlugIn_Track::PlugIn_Track(void) { pWaypointList = new Plugin_WaypointList; }

PlugIn_Track::~PlugIn_Track(void) {
  pWaypointList->DeleteContents(false);  // do not delete Waypoints
  pWaypointList->Clear();

  delete pWaypointList;
}

wxString GetNewGUID(void) { return GpxDocument::GetUUID(); }

bool AddCustomWaypointIcon(wxBitmap *pimage, wxString key,
                           wxString description) {
  pWayPointMan->ProcessIcon(*pimage, key, description);
  return true;
}

static void cloneHyperlinkList(RoutePoint *dst, const PlugIn_Waypoint *src) {
  //  Transcribe (clone) the html HyperLink List, if present
  if (src->m_HyperlinkList == nullptr) return;

  if (src->m_HyperlinkList->GetCount() > 0) {
    wxPlugin_HyperlinkListNode *linknode = src->m_HyperlinkList->GetFirst();
    while (linknode) {
      Plugin_Hyperlink *link = linknode->GetData();

      Hyperlink *h = new Hyperlink();
      h->DescrText = link->DescrText;
      h->Link = link->Link;
      h->LType = link->Type;

      dst->m_HyperlinkList->Append(h);

      linknode = linknode->GetNext();
    }
  }
}

bool AddSingleWaypoint(PlugIn_Waypoint *pwaypoint, bool b_permanent) {
  //  Validate the waypoint parameters a little bit

  //  GUID
  //  Make sure that this GUID is indeed unique in the Routepoint list
  bool b_unique = true;
  wxRoutePointListNode *prpnode = pWayPointMan->GetWaypointList()->GetFirst();
  while (prpnode) {
    RoutePoint *prp = prpnode->GetData();

    if (prp->m_GUID == pwaypoint->m_GUID) {
      b_unique = false;
      break;
    }
    prpnode = prpnode->GetNext();  // RoutePoint
  }

  if (!b_unique) return false;

  RoutePoint *pWP =
      new RoutePoint(pwaypoint->m_lat, pwaypoint->m_lon, pwaypoint->m_IconName,
                     pwaypoint->m_MarkName, pwaypoint->m_GUID);

  pWP->m_bIsolatedMark = true;  // This is an isolated mark

  cloneHyperlinkList(pWP, pwaypoint);

  pWP->m_MarkDescription = pwaypoint->m_MarkDescription;

  if (pwaypoint->m_CreateTime.IsValid())
    pWP->SetCreateTime(pwaypoint->m_CreateTime);
  else {
    wxDateTime dtnow(wxDateTime::Now());
    pWP->SetCreateTime(dtnow);
  }

  pWP->m_btemp = (b_permanent == false);

  pSelect->AddSelectableRoutePoint(pwaypoint->m_lat, pwaypoint->m_lon, pWP);
  if (b_permanent) pConfig->AddNewWayPoint(pWP, -1);

  if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
    pRouteManagerDialog->UpdateWptListCtrl();

  return true;
}

bool DeleteSingleWaypoint(wxString &GUID) {
  //  Find the RoutePoint
  bool b_found = false;
  RoutePoint *prp = pWayPointMan->FindRoutePointByGUID(GUID);

  if (prp) b_found = true;

  if (b_found) {
    pWayPointMan->DestroyWaypoint(prp);
    if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
      pRouteManagerDialog->UpdateWptListCtrl();
  }

  return b_found;
}

bool UpdateSingleWaypoint(PlugIn_Waypoint *pwaypoint) {
  //  Find the RoutePoint
  bool b_found = false;
  RoutePoint *prp = pWayPointMan->FindRoutePointByGUID(pwaypoint->m_GUID);

  if (prp) b_found = true;

  if (b_found) {
    double lat_save = prp->m_lat;
    double lon_save = prp->m_lon;

    prp->m_lat = pwaypoint->m_lat;
    prp->m_lon = pwaypoint->m_lon;
    prp->SetIconName(pwaypoint->m_IconName);
    prp->SetName(pwaypoint->m_MarkName);
    prp->m_MarkDescription = pwaypoint->m_MarkDescription;
    prp->SetVisible(pwaypoint->m_IsVisible);
    if (pwaypoint->m_CreateTime.IsValid())
      prp->SetCreateTime(pwaypoint->m_CreateTime);

    //  Transcribe (clone) the html HyperLink List, if present

    if (pwaypoint->m_HyperlinkList) {
      prp->m_HyperlinkList->Clear();
      if (pwaypoint->m_HyperlinkList->GetCount() > 0) {
        wxPlugin_HyperlinkListNode *linknode =
            pwaypoint->m_HyperlinkList->GetFirst();
        while (linknode) {
          Plugin_Hyperlink *link = linknode->GetData();

          Hyperlink *h = new Hyperlink();
          h->DescrText = link->DescrText;
          h->Link = link->Link;
          h->LType = link->Type;

          prp->m_HyperlinkList->Append(h);

          linknode = linknode->GetNext();
        }
      }
    }

    if (prp) prp->ReLoadIcon();

    SelectItem *pFind = pSelect->FindSelection(
        gFrame->GetPrimaryCanvas(), lat_save, lon_save, SELTYPE_ROUTEPOINT);
    if (pFind) {
      pFind->m_slat = pwaypoint->m_lat;  // update the SelectList entry
      pFind->m_slon = pwaypoint->m_lon;
    }

    if (!prp->m_btemp) pConfig->UpdateWayPoint(prp);

    if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
      pRouteManagerDialog->UpdateWptListCtrl();
  }

  return b_found;
}

bool GetSingleWaypoint(wxString GUID, PlugIn_Waypoint *pwaypoint) {
  //  Find the RoutePoint
  RoutePoint *prp = pWayPointMan->FindRoutePointByGUID(GUID);

  if (!prp) return false;

  PlugInFromRoutePoint(pwaypoint, prp);

  return true;
}

wxArrayString GetWaypointGUIDArray(void) {
  wxArrayString result;
  RoutePointList *list = pWayPointMan->GetWaypointList();

  wxRoutePointListNode *prpnode = list->GetFirst();
  while (prpnode) {
    RoutePoint *prp = prpnode->GetData();
    result.Add(prp->m_GUID);

    prpnode = prpnode->GetNext();  // RoutePoint
  }

  return result;
}

wxArrayString GetRouteGUIDArray(void) {
  wxArrayString result;
  RouteList *list = pRouteList;

  wxRouteListNode *prpnode = list->GetFirst();
  while (prpnode) {
    Route *proute = prpnode->GetData();
    result.Add(proute->m_GUID);

    prpnode = prpnode->GetNext();  // Route
  }

  return result;
}

wxArrayString GetTrackGUIDArray(void) {
  wxArrayString result;
  TrackList *list = pTrackList;

  wxTrackListNode *prpnode = list->GetFirst();
  while (prpnode) {
    Track *ptrack = prpnode->GetData();
    result.Add(ptrack->m_GUID);

    prpnode = prpnode->GetNext();  // Track
  }

  return result;
}

wxArrayString GetIconNameArray(void) {
  wxArrayString result;

  for (int i = 0; i < pWayPointMan->GetNumIcons(); i++) {
    wxString *ps = pWayPointMan->GetIconKey(i);
    result.Add(*ps);
  }
  return result;
}

bool AddPlugInRoute(PlugIn_Route *proute, bool b_permanent) {
  Route *route = new Route();

  PlugIn_Waypoint *pwp;
  RoutePoint *pWP_src;
  int ip = 0;
  wxDateTime plannedDeparture;

  wxPlugin_WaypointListNode *pwpnode = proute->pWaypointList->GetFirst();
  while (pwpnode) {
    pwp = pwpnode->GetData();

    RoutePoint *pWP = new RoutePoint(pwp->m_lat, pwp->m_lon, pwp->m_IconName,
                                     pwp->m_MarkName, pwp->m_GUID);

    //  Transcribe (clone) the html HyperLink List, if present
    cloneHyperlinkList(pWP, pwp);
    pWP->m_MarkDescription = pwp->m_MarkDescription;
    pWP->m_bShowName = false;
    pWP->SetCreateTime(pwp->m_CreateTime);

    route->AddPoint(pWP);

    pSelect->AddSelectableRoutePoint(pWP->m_lat, pWP->m_lon, pWP);

    if (ip > 0)
      pSelect->AddSelectableRouteSegment(pWP_src->m_lat, pWP_src->m_lon,
                                         pWP->m_lat, pWP->m_lon, pWP_src, pWP,
                                         route);
    else
      plannedDeparture = pwp->m_CreateTime;
    ip++;
    pWP_src = pWP;

    pwpnode = pwpnode->GetNext();  // PlugInWaypoint
  }

  route->m_PlannedDeparture = plannedDeparture;

  route->m_RouteNameString = proute->m_NameString;
  route->m_RouteStartString = proute->m_StartString;
  route->m_RouteEndString = proute->m_EndString;
  if (!proute->m_GUID.IsEmpty()) {
    route->m_GUID = proute->m_GUID;
  }
  route->m_btemp = (b_permanent == false);

  pRouteList->Append(route);

  if (b_permanent) pConfig->AddNewRoute(route);

  if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
    pRouteManagerDialog->UpdateRouteListCtrl();

  return true;
}

bool DeletePlugInRoute(wxString &GUID) {
  bool b_found = false;

  //  Find the Route
  Route *pRoute = g_pRouteMan->FindRouteByGUID(GUID);
  if (pRoute) {
    g_pRouteMan->DeleteRoute(pRoute);
    b_found = true;
  }
  return b_found;
}

bool UpdatePlugInRoute(PlugIn_Route *proute) {
  bool b_found = false;

  //  Find the Route
  Route *pRoute = g_pRouteMan->FindRouteByGUID(proute->m_GUID);
  if (pRoute) b_found = true;

  if (b_found) {
    bool b_permanent = (pRoute->m_btemp == false);
    g_pRouteMan->DeleteRoute(pRoute);

    b_found = AddPlugInRoute(proute, b_permanent);
  }

  return b_found;
}

bool AddPlugInTrack(PlugIn_Track *ptrack, bool b_permanent) {
  Track *track = new Track();

  PlugIn_Waypoint *pwp = 0;
  TrackPoint *pWP_src = 0;
  int ip = 0;

  wxPlugin_WaypointListNode *pwpnode = ptrack->pWaypointList->GetFirst();
  while (pwpnode) {
    pwp = pwpnode->GetData();

    TrackPoint *pWP = new TrackPoint(pwp->m_lat, pwp->m_lon);
    pWP->SetCreateTime(pwp->m_CreateTime);

    track->AddPoint(pWP);

    if (ip > 0)
      pSelect->AddSelectableTrackSegment(pWP_src->m_lat, pWP_src->m_lon,
                                         pWP->m_lat, pWP->m_lon, pWP_src, pWP,
                                         track);
    ip++;
    pWP_src = pWP;

    pwpnode = pwpnode->GetNext();  // PlugInWaypoint
  }

  track->SetName(ptrack->m_NameString);
  track->m_TrackStartString = ptrack->m_StartString;
  track->m_TrackEndString = ptrack->m_EndString;
  track->m_GUID = ptrack->m_GUID;
  track->m_btemp = (b_permanent == false);

  pTrackList->Append(track);

  if (b_permanent) pConfig->AddNewTrack(track);

  if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
    pRouteManagerDialog->UpdateTrkListCtrl();

  return true;
}

bool DeletePlugInTrack(wxString &GUID) {
  bool b_found = false;

  //  Find the Route
  Track *pTrack = g_pRouteMan->FindTrackByGUID(GUID);
  if (pTrack) {
    g_pRouteMan->DeleteTrack(pTrack);
    b_found = true;
  }

  if (pRouteManagerDialog && pRouteManagerDialog->IsShown())
    pRouteManagerDialog->UpdateTrkListCtrl();

  return b_found;
}

bool UpdatePlugInTrack(PlugIn_Track *ptrack) {
  bool b_found = false;

  //  Find the Track
  Track *pTrack = g_pRouteMan->FindTrackByGUID(ptrack->m_GUID);
  if (pTrack) b_found = true;

  if (b_found) {
    bool b_permanent = (pTrack->m_btemp == false);
    g_pRouteMan->DeleteTrack(pTrack);

    b_found = AddPlugInTrack(ptrack, b_permanent);
  }

  return b_found;
}

bool PlugInHasNormalizedViewPort(PlugIn_ViewPort *vp) {
#ifdef ocpnUSE_GL
  ViewPort ocpn_vp;
  ocpn_vp.m_projection_type = vp->m_projection_type;

  return glChartCanvas::HasNormalizedViewPort(ocpn_vp);
#else
  return false;
#endif
}

void PlugInMultMatrixViewport(PlugIn_ViewPort *vp, float lat, float lon) {
#ifdef ocpnUSE_GL
  ViewPort ocpn_vp;
  ocpn_vp.clat = vp->clat;
  ocpn_vp.clon = vp->clon;
  ocpn_vp.m_projection_type = vp->m_projection_type;
  ocpn_vp.view_scale_ppm = vp->view_scale_ppm;
  ocpn_vp.skew = vp->skew;
  ocpn_vp.rotation = vp->rotation;
  ocpn_vp.pix_width = vp->pix_width;
  ocpn_vp.pix_height = vp->pix_height;

// TODO fix for multicanvas    glChartCanvas::MultMatrixViewPort(ocpn_vp, lat,
// lon);
#endif
}

void PlugInNormalizeViewport(PlugIn_ViewPort *vp, float lat, float lon) {
#ifdef ocpnUSE_GL
  ViewPort ocpn_vp;
  glChartCanvas::NormalizedViewPort(ocpn_vp, lat, lon);

  vp->clat = ocpn_vp.clat;
  vp->clon = ocpn_vp.clon;
  vp->view_scale_ppm = ocpn_vp.view_scale_ppm;
  vp->rotation = ocpn_vp.rotation;
  vp->skew = ocpn_vp.skew;
#endif
}

wxString GetPluginDataDir(const char *plugin_name) {
  static const wxString sep = wxFileName::GetPathSeparator();

  wxString datadirs = g_Platform->GetPluginDataPath();
  wxLogMessage(_T("PlugInManager: Using data dirs from: ") + datadirs);
  wxStringTokenizer dirs(datadirs, ";");
  while (dirs.HasMoreTokens()) {
    wxString dir = dirs.GetNextToken();
    wxFileName tryDirName(dir);
    wxDir tryDir;
    if (!tryDir.Open(tryDirName.GetFullPath())) continue;
    wxString next;
    bool more = tryDir.GetFirst(&next);
    while (more) {
      if (next == plugin_name) {
        next = next.Prepend(tryDirName.GetFullPath() + sep);
        wxLogMessage(_T("PlugInManager: using data dir: %s"), next);
        return next;
      }
      more = tryDir.GetNext(&next);
    }
    tryDir.Close();
  }
  wxLogMessage(_T("Warnińg: no data directory found, using \"\""));
  return "";
}


