#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

#include "catalog_parser.h"
#include "pugixml.hpp"

class CatalogTest: public ::testing::Test {
protected:
  catalog_ctx ctx;

  void SetUp() {
    std::ifstream is(TESTDATA_DIR "/ocpn-plugins.xml");
    std::stringstream buff;
    buff << is.rdbuf();
    ParseCatalog(buff.str(), &ctx);
  }
};

TEST_F(CatalogTest, CheckSize) {
  EXPECT_EQ(ctx.plugins.size(), 493);
}

TEST_F(CatalogTest, CheckVDR) {
  std::vector<PluginMetadata> found;
  auto& src = ctx.plugins;
  std::copy_if(src.begin(), src.end(), std::back_inserter(found),
              [](PluginMetadata pm) {
                   return pm.name == "VDR" && pm.target == "darwin"; });
  EXPECT_EQ(1, found.size());
  EXPECT_EQ("VDR", found[0].name );
  EXPECT_EQ("1.3.8.0+1455.v1.3.8.0", found[0].version);
  EXPECT_EQ("f6be99b54cae222aed773ed12e183f1fb53a6ce39d4c2c81b4ef9f9829c7b410",
            found[0].checksum);
  const char* const url =
      "https://dl.cloudsmith.io/public/opencpn/vdr-prod/raw/names/"
      "VDR-v1.3.8.0-darwin-10.13.6-tarball/versions/1.3.8.0+1455.v1.3.8.0/"
      "VDR-v1.3.8.0_darwin-10.13.6-x86_64.tar.gz";
  EXPECT_EQ(url, found[0].tarball_url );
}
