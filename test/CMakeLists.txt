cmake_minimum_required(VERSION 3.1)

if (TARGET ocpn::gtest)
    return ()
endif ()

set(SRC
    src/catalog_test.cpp
    ${PROJECT_SOURCE_DIR}/src/catalog_parser.cpp
    ${PROJECT_SOURCE_DIR}/src/ocpn_utils.cpp
    ${PROJECT_SOURCE_DIR}/src/pugixml.cpp
)
add_executable(o_tests ${SRC})
target_include_directories(o_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(o_tests PRIVATE ${wxWidgets_LIBRARIES})

find_package(GTest REQUIRED)
target_link_libraries(o_tests PUBLIC GTest::gtest)
target_link_libraries(o_tests PUBLIC GTest::gtest_main)
target_compile_definitions(o_tests PRIVATE
  TESTDATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
)

include(GoogleTest)
enable_testing()
gtest_add_tests(TARGET o_tests TEST_LIST tests)
