add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)

add_executable(dcv_tests 
  "tests/utils.test.cpp"
  "tests/models.test.cpp"
  "tests/influxdb.test.cpp"
  "tests/bus.test.cpp"
  "tests/manager.test.cpp"
)

target_link_libraries(dcv_tests dcv pthread gtest_main gmock)
set(TESTS_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/dcv/tests")
install(TARGETS dcv_tests RUNTIME DESTINATION ${TESTS_INSTALL_DIR})
