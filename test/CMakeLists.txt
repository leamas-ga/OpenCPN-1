cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::gtest)
    return ()
endif ()

set(SRC
    src/catalog_test.cpp
    src/platform_test.cpp
    ${PROJECT_SOURCE_DIR}/src/BasePlatform.cpp
    ${PROJECT_SOURCE_DIR}/src/catalog_handler.cpp
    ${PROJECT_SOURCE_DIR}/src/catalog_parser.cpp
    ${PROJECT_SOURCE_DIR}/src/config_var.cpp
    ${PROJECT_SOURCE_DIR}/src/Downloader.cpp
    ${PROJECT_SOURCE_DIR}/src/logger.cpp
    ${PROJECT_SOURCE_DIR}/src/ocpn_utils.cpp
    ${PROJECT_SOURCE_DIR}/src/ocpn_plugin.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin_api_shim.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin_cache.cpp
    ${PROJECT_SOURCE_DIR}/src/plugin_loader.cpp
    ${PROJECT_SOURCE_DIR}/src/PluginHandler.cpp
    ${PROJECT_SOURCE_DIR}/src/PluginPaths.cpp
    ${PROJECT_SOURCE_DIR}/src/pugixml.cpp
    ${PROJECT_SOURCE_DIR}/src/semantic_vers.cpp
)
add_executable(o_tests ${SRC})
target_include_directories(o_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(o_tests PRIVATE ocpn::wxjson)
target_link_libraries(o_tests PRIVATE ocpn::wxcurl)
target_link_libraries(o_tests PRIVATE ocpn::wxsvg)
target_link_libraries(o_tests PRIVATE ${wxWidgets_LIBRARIES})
target_link_libraries(o_tests PRIVATE ${LibArchive_LIBRARIES})
target_link_libraries(o_tests PRIVATE "${LIBELF_LIBRARY}")

find_package(GTest REQUIRED)
target_link_libraries(o_tests PUBLIC GTest::gtest)
target_link_libraries(o_tests PUBLIC GTest::gtest_main)
target_compile_definitions(o_tests PRIVATE
  TESTDATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
)

include(GoogleTest)
enable_testing()
gtest_add_tests(TARGET o_tests TEST_LIST tests)
