add_library(libwdg libwdg.cpp)
add_library(wdg::libwdg ALIAS libwdg)

set_target_properties(
	libwdg
	PROPERTIES OUTPUT_NAME wdg
)

target_compile_features(libwdg PUBLIC cxx_std_17)

target_include_directories(
	libwdg
	PUBLIC
	    $<INSTALL_INTERFACE:include>
	    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
	PRIVATE
	    ${CMAKE_SOURCE_DIR}/include
)

include(GNUInstallDirs)
install(TARGETS libwdg
	EXPORT wdg-targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
	EXPORT wdg-targets
	FILE wdg-config.cmake
	NAMESPACE wdg::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/wdg
)

