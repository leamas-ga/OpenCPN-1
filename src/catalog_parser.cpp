/******************************************************************************
 *
 * Project:  OpenCPN
 *
 ***************************************************************************
 *   Copyright (C) 2019 Alec Leamas                                        *
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

#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>

#include <wx/log.h>

#if defined(__MINGW32__) && defined(Yield)
#undef Yield  // from win.h, conflicts with mingw headers
#endif

#include "catalog_parser.h"
#include "ocpn_utils.h"
#include "pugixml.hpp"

bool ParseMetadataNode(pugi::xml_node plugin, PluginMetadata& metadata) {

  pugi::xml_node node;
  for (node = plugin.first_child(); node; node = node.next_sibling())
  {
    if (strcmp(node.name(), "name") == 0) {
      metadata.name = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "version") == 0) {
      metadata.version = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "release") == 0) {
      metadata.release = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "summary") == 0) {
      metadata.summary = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "api_version") == 0) {
      metadata.api_version = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "author") == 0) {
      metadata.author = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "description") == 0) {
      metadata.description = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "git-commit") == 0) {
      metadata.git_commit = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "git-date") == 0) {
      metadata.git_date = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "source") == 0) {
      metadata.source = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "tarball-url") == 0) {
      metadata.tarball_url = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "info-url") == 0) {
      metadata.info_url = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "target") == 0) {
      metadata.target = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "target-version") == 0) {
      metadata.target_version = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "target-arch") == 0) {
      metadata.target_arch = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "tarball-checksum") == 0) {
      metadata.checksum = ocpn::trim(node.first_child().value());
    } else if (strcmp(node.name(), "open-source") == 0) {
      metadata.openSource = ocpn::trim(node.first_child().value()) == "yes";
    }
  }
  return true;
}

bool ParseMetadata(const std::string& xml, PluginMetadata& metadata) {
  pugi::xml_document doc;
  doc.load_string(xml.c_str());
  pugi::xml_node plugin = doc.child("plugin");
  return ParseMetadataNode(plugin, metadata);
}

bool ParseCatalog(const std::string xml, catalog_ctx* ctx) {
  bool ok = true;
  PluginMetadata* plugin = 0;

  pugi::xml_document doc;
  doc.load_string(xml.c_str());

  pugi::xml_node elements = doc.child("plugins");
  for (pugi::xml_node element = elements.first_child(); element;
       element = element.next_sibling()) {
    if (!strcmp(element.name(), "version") && ctx->version == "") {
      ctx->version = ocpn::trim(element.first_child().value());
    } else if (strcmp(element.name(), "date") == 0 && ctx->date == "") {
      ctx->date = ocpn::trim(element.first_child().value());
    } else if (strcmp(element.name(), "meta-url") == 0) {
      ctx->meta_url = ocpn::trim(element.first_child().value());
    } else if (!strcmp(element.name(), "plugin")) {
      if (ctx->meta_url != "") {
        ctx->meta_urls.push_back(ctx->meta_url);
        ctx->meta_url = "";
      } else {
        if (plugin) ctx->plugins.push_back(*plugin);
        plugin = new PluginMetadata;
      }

      for (pugi::xml_node plugin_element = element.first_child();
           plugin_element; plugin_element = plugin_element.next_sibling()) {
        if (strcmp(plugin_element.name(), "meta-url") == 0) {
          auto url = ocpn::trim(plugin_element.first_child().value());
          ctx->meta_url = url;
        } else if (strcmp(plugin_element.name(), "name") == 0) {
          plugin->name = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "version") == 0) {
          plugin->version = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "release") == 0) {
          plugin->release = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "summary") == 0) {
          plugin->summary = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "api_version") == 0) {
          plugin->api_version =
              ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "author") == 0) {
          plugin->author = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "description") == 0) {
          plugin->description =
              ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "git-commit") == 0) {
          plugin->git_commit = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "git-date") == 0) {
          plugin->git_date = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "source") == 0) {
          plugin->source = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "tarball-url") == 0) {
          plugin->tarball_url =
              ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "info-url") == 0) {
          plugin->info_url = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "target") == 0) {
          plugin->target = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "build-gtk") == 0) {
          plugin->build_gtk = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "target-version") == 0) {
          plugin->target_version =
              ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "target-arch") == 0) {
          plugin->target_arch =
              ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "tarball-checksum") == 0) {
          plugin->checksum = ocpn::trim(plugin_element.first_child().value());
        } else if (strcmp(plugin_element.name(), "open-source") == 0) {
          plugin->openSource =
              ocpn::trim(plugin_element.first_child().value()) == "yes";
        }
      }
    }
  }

  // capture last plugin
  if (plugin)
    ctx->plugins.push_back(*plugin);
  else {
    if (ctx->meta_url != "") {
      ctx->meta_urls.push_back(ctx->meta_url);
      ctx->meta_url = "";
    }
  }

  return true;
}
