#include <string>

#include <gtest/gtest.h>

#include <wx/app.h>
#include <wx/config.h>
#include <wx/string.h>

#include "BasePlatform.h"

#ifndef DECL_EXP
#if defined(__WXMSW__) || defined(__CYGWIN__)
#define DECL_EXP __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#define DECL_EXP __attribute__((visibility("default")))
#elif defined __WXOSX__
#define DECL_EXP __attribute__((visibility("default")))
#else
#define DECL_EXP
#endif
#endif

extern DECL_EXP wxString GetPluginDataDir(const char *plugin_name);

wxLog* g_logger;
wxString g_winPluginDir;
bool g_bportable;
wxConfig* pBaseConfig;
BasePlatform* g_BasePlatform;
wxString g_compatOS;
wxString g_compatOsVersion;

wxString g_catalog_channel;
wxString g_catalog_custom_url;

#ifdef  __linux__
static const char* const PRIVATE_DATA_DIR = "~/.opencpn";
#elif defined(__MSVC__)
static const char* const PRIVATE_DATA_DIR = "/ProgramData/opencpn";
#else
static const char* const PRIVATE_DATA_DIR = 0;
#endif

#ifdef __linux__
static const char* const
    PLUGIN_DATA_DIR = "~/.local/share/opencpn/plugins/@plugin@";
#else
static const char* const PLUGIN_DATA_DIR = 0;
#endif

namespace safe_mode { bool get_mode() { return true; } }

class PlatformTest: public wxAppConsole,  public ::testing::Test {
protected:

  PlatformTest(): wxAppConsole() {
    SetAppName("opencpn");
    g_BasePlatform = new BasePlatform();
  }

};

TEST_F(PlatformTest, PrivateDataDir) {
    auto path = g_BasePlatform->GetPrivateDataDir().ToStdString();
    std::string home(getenv("HOME"));
    size_t pos = path.find(home);
    if (pos != std::string::npos) path.replace(pos, home.length(), "~");
    if (PRIVATE_DATA_DIR) {
      EXPECT_TRUE(path == PRIVATE_DATA_DIR);
    }
}

TEST_F(PlatformTest, PluginDataDir) {
    auto path = GetPluginDataDir("foobar").ToStdString();
    std::string home(getenv("HOME"));
    size_t pos = path.find(home);
    if (pos != std::string::npos) path.replace(pos, home.length(), "~");
    if (PLUGIN_DATA_DIR) {
      EXPECT_TRUE(path ==  "");
    }
}
