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

# Use if CMAKE_PREFIX_PATH isn't set in env variables
#set(Qt6_DIR C:\\Qt\\6.0.0\\msvc2019_64\\lib\\cmake\\Qt6)

#set(CMAKE_AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/Ui/Bundle" "${PROJECT_SOURCE_DIR}/Ui/Bundle/Resources")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(AoRGhostManager WIN32 ${SOURCES} ${HEADERS} ${RESOURCES} ${UIS})

# Qt
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# libbinaryio
#add_subdirectory(3rdparty/libbinaryio "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libbinaryio" EXCLUDE_FROM_ALL)
#target_include_directories(AoRGhostManager PRIVATE ${ROOT} "${CMAKE_CURRENT_BINARY_DIR}/3rdparty/libbinaryio" EXCLUDE_FROM_ALL)

target_link_libraries(AoRGhostManager PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets) #libbinaryio