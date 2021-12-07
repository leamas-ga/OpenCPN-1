#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/dynlib.h>
#include <wx/log.h>

#include <archive.h>
#include <archive_entry.h>

#include "BasePlatform.h"
#include "catalog_parser.h"
#include "ocpn_utils.h"
#include "PluginHandler.h"
#include "plugin_loader.h"

bool g_bportable;
wxString g_compatOS;
wxString g_compatOsVersion;
wxString g_catalog_custom_url;
wxString g_catalog_channel;

wxLog* g_logger;
wxString g_winPluginDir;
BasePlatform* g_BasePlatform;
wxConfigBase* pBaseConfig;

namespace safe_mode { bool get_mode() { return true; } }

static const char* const USAGE =
  "  load <library>\n"
  "  install <tarball>\n"
  "  uninstall <plugin>\n"
  "  list-installed";

class MetadataHandler: public PluginHandler {

  bool do_extract(struct archive* src, struct archive* dest,
                  const char* dest_path)
  {
    struct archive_entry* entry = 0;
    while (true) {
      int r = archive_read_next_header(src, &entry);
      if (r == ARCHIVE_EOF) return false; 
      if (!archive_check(r, "archive read header error", src)) return false;
      std::string path = archive_entry_pathname(entry);
      if (std::string::npos == path.find("metadata.xml")) continue;

      archive_entry_set_pathname(entry, dest_path);
      r = archive_write_header(dest, entry);
      archive_check(r, "archive write install header error", dest);
      if (r >= ARCHIVE_OK && archive_entry_size(entry) > 0) {
        r = copy_data(src, dest);
        if (!archive_check(r, "archive copy data error", dest)) return false; 
      }
      r = archive_write_finish_entry(dest);
      if (!archive_check(r, "archive finish write error", dest)) return false;
      return true;
    }
    return false;
  }

public:

  /** Extract and parse metadata in tarball at path into metadata. */
  bool extract_metadata(const std::string path, PluginMetadata& metadata) {
    struct archive* src = archive_read_new();
    archive_read_support_filter_gzip(src);
    archive_read_support_format_tar(src);
    int r = archive_read_open_filename(src, path.c_str(), 10240);
    if (r != ARCHIVE_OK) {
      std::ostringstream os;
      os << "Cannot read installation tarball: " << path;
      wxLogWarning(os.str().c_str());
      return false;
    }
    struct archive* dest = archive_write_disk_new();
    const char* tmp_path = std::tmpnam(0);
    bool ok = do_extract(src, dest, tmp_path);
    archive_read_free(src);
    archive_write_free(dest);
    if (!ok) return false;

    std::ifstream is(tmp_path);
    std::stringstream buff;
    buff << is.rdbuf();
    remove(tmp_path);  // best effort, if it fails it fails...
    return ParseMetadata(buff.str(), metadata);
  }
};

class CliApp: public wxAppConsole {

  bool test_linkage(const std::string path) {
    wxDynamicLibrary library;
    bool ok = library.Load(path.c_str());
    printf(ok ? "File loaded OK\n" : "Load error\n");
    return ok;
  }

  bool test_install(const std::string& path)
  {
    g_BasePlatform = new BasePlatform();
    MetadataHandler handler;
    if (!ocpn::exists(path)) {
      std::cerr << "Cannot open tarball file: " << path << "\n";
      exit(1);
    }
    PluginMetadata metadata;
    bool ok = handler.extract_metadata(path, metadata);
    if (!ok) return false;
    return PluginHandler::getInstance()->installPlugin(metadata, path);
  }

  void do_exit(int code) { ExitMainLoop();  exit(code); }

  void OnInitCmdLine(wxCmdLineParser& parser) override {
    parser.AddParam();
    parser.AddParam("", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    parser.AddSwitch("h", "help");
    parser.AddUsageText(USAGE);
    SetAppName("opencpn");
  }

  bool OnCmdLineParsed(wxCmdLineParser& parser) override {
    wxAppConsole::OnCmdLineParsed(parser);
    std::string cmd = parser.GetParam(0).ToStdString();
    g_BasePlatform = new BasePlatform();
    if (cmd == "list-installed") {
      PluginLoader::getInstance()->LoadAllPlugIns(false);
      auto plugins = PluginHandler::getInstance()->getInstalled();
      for (auto& p : plugins)
          std::cout << p.name << " " << p.version << "\n";
      do_exit(0);
    }
    if (parser.GetParamCount() != 2) {
      parser.Usage();
      do_exit(2);
    }
    if (cmd == "load") {
      std::string library(parser.GetParam(1).ToStdString());
      bool ok = test_linkage(library);
      do_exit(ok ? 0 : 1);
    }
    else if (cmd == "install") {
      std::string plugin(parser.GetParam(1).ToStdString());
      bool ok = test_install(plugin);
      do_exit(ok ? 0 : 2);
    }
    else if (cmd == "uninstall") {
      std::string plugin(parser.GetParam(1).ToStdString());
      bool ok = PluginHandler::getInstance()->uninstall(plugin);
      do_exit(ok ? 0 : 2);
    }
    else if (parser.Found("h")) { parser.Usage(); do_exit(0); }
    else  { parser.Usage(); do_exit(1); }
    return false; // NOTREACHED
  }
};

wxIMPLEMENT_APP(CliApp);
