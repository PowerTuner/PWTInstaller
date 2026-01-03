cmake_minimum_required(VERSION 3.23)
project(PWTInstaller VERSION 1.0 DESCRIPTION "PowerTuner installer" LANGUAGES CXX)

set(PROJECT_AUTHOR "kylon")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.10 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_resources(QT_RESOURCE
	src/Resources/resources.qrc
)
qt_add_big_resources(QT_BIG_RESOURCE
	src/Resources/installFiles.qrc
)

configure_file(src/Resources/Windows/win.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/win.rc)
configure_file(src/Resources/version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)
set_property(SOURCE src/Resources/installFiles.qrc PROPERTY SKIP_AUTORCC ON)
add_subdirectory(src/external/libPWTWin32)

qt_add_executable(${PROJECT_NAME}
	${QT_RESOURCE}
	${QT_BIG_RESOURCE}

	src/Pages/Include/InstallWorker.h
	src/Pages/Include/InstallWorker.cpp
	src/Pages/Include/DownloadWorker.h
	src/Pages/Include/DownloadWorker.cpp
	src/Pages/Include/InstallServiceThread.h
	src/Pages/Include/InstallServiceThread.cpp

	src/Pages/Page.h
	src/Pages/Page.cpp
	src/Pages/WelcomePage.h
	src/Pages/WelcomePage.cpp
	src/Pages/LicensePage.h
	src/Pages/LicensePage.cpp
	src/Pages/WinringPage.h
	src/Pages/WinringPage.cpp
	src/Pages/FolderPage.h
	src/Pages/FolderPage.cpp
	src/Pages/CustomizePage.h
	src/Pages/CustomizePage.cpp
	src/Pages/SummaryPage.h
	src/Pages/SummaryPage.cpp
	src/Pages/InstallPage.h
	src/Pages/InstallPage.cpp
	src/Pages/FinishPage.h
	src/Pages/FinishPage.cpp

	src/InstallData.h
	src/main.cpp
	src/mainwindow.cpp
	src/mainwindow.h

	src/mainwindow.ui
	win.rc
)

include(CheckIPOSupported)
check_ipo_supported(RESULT has_ipo OUTPUT ipo_error)
if (has_ipo)
	message(STATUS "${PROJECT_NAME}: IPO/LTO enabled")
	set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:DEBUG>:/W3 /wo4061>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Widgets Qt::Network PWT::WIN32 Ktmw32)

set_target_properties(${PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE TRUE
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND mt -manifest ${CMAKE_CURRENT_SOURCE_DIR}/src/Resources/Windows/${PROJECT_NAME}.exe.manifest -outputresource:${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.exe
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "add manifest"
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
	NO_TRANSLATIONS
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
