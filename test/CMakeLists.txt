add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE wdg::libwdg Catch2::Catch2WithMain)
