cmake_minimum_required(VERSION 3.19)
project(AoRGhostManager CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
	${SOURCES}
	main.cpp
	Ui/AoRGhostManager.cpp
	)

set(HEADERS
	${HEADERS}
	Ui/AoRGhostManager.h
	)

set(RESOURCES
	${RESOURCES}
	Ui/AoRGhostManager.qrc
	)

set(UIS
	${UIS}
	Ui/AoRGhostManager.ui
	)

# Use if Qt6_DIR isn't set in PATH
#set(Qt6_DIR C:\\Qt\\6.0.0\\msvc2019_64\\lib\\cmake\\Qt6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/Ui/Icon.rc")

add_executable(AoRGhostManager WIN32 ${SOURCES} ${HEADERS} ${RESOURCES} ${UIS} ${APP_ICON_RESOURCE_WINDOWS})

# Qt
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

target_link_libraries(AoRGhostManager PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

# VS stuff
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AoRGhostManager)