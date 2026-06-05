cmake_minimum_required(VERSION 3.14)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else()
	execute_process(
		COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
		COMMAND head -n 1
		COMMAND cut -c9-
		OUTPUT_VARIABLE SDK
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()

if (NOT EXISTS ${SDK})
	message(FATAL_ERROR "SDK Path not found; set ENV value PLAYDATE_SDK_PATH")
	return()
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# ゲーム名
set(PLAYDATE_GAME_NAME myfirstgame)
set(PLAYDATE_GAME_DEVICE myfirstgame_DEVICE)

project(${PLAYDATE_GAME_NAME} C ASM)

# src 以下の .c を全部集める。
# 注意：ファイルを新規追加したら一度 cmake を再実行する（GLOB のため）。
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.c")

if (TOOLCHAIN STREQUAL "armgcc")
	add_executable(${PLAYDATE_GAME_DEVICE} ${SDK}/C_API/buildsupport/setup.c ${SOURCES})
	target_include_directories(${PLAYDATE_GAME_DEVICE} PRIVATE src)
else()
	add_library(${PLAYDATE_GAME_NAME} SHARED ${SOURCES})
	target_include_directories(${PLAYDATE_GAME_NAME} PRIVATE src)
endif()

include(${SDK}/C_API/buildsupport/playdate_game.cmake)
