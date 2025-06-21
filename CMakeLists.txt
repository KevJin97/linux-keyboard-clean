cmake_minimum_required(VERSION 3.16)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)	# required for editor

# C++ Compiler Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ADD_EVENT_PRIVILEGES "Be able to run without sudo" ON)

# Set Module Path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)


find_package(PkgConfig REQUIRED)
pkg_search_module(LIBEVDEV REQUIRED libevdev)

file(GLOB KEYBOARD_LOCK_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# Compilation Instructions
project(keyboard-lock)
add_executable(keyboard-lock ${KEYBOARD_LOCK_SOURCES})
target_include_directories(keyboard-lock
	PRIVATE
		${LIBEVDEV_INCLUDE_DIRS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(keyboard-lock PUBLIC ${LIBEVDEV_LIBRARIES})

# Add CMake Module
if (ADD_EVENT_PRIVILEGES)
	message(STATUS "Adding input event privileges to executable")
	include(CHANGE_GROUP_ID)
	target_compile_definitions(keyboard-lock PRIVATE GROUP_PERMISSIONS_ENABLED)
endif()
