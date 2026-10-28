add_executable(wdg wdg.cpp)
target_link_libraries(wdg PRIVATE wdg::libwdg PkgConfig::readline)

install(
	TARGETS wdg
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
