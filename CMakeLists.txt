cmake_minimum_required(VERSION 3.15)

project("Alex the Allegator 4" VERSION 1.2 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

option(WANT_ASAN "build with adress sanitizer" OFF)
if(WANT_ASAN)
	add_compile_options(-fno-omit-frame-pointer -fsanitize=address)
	add_link_options(-fno-omit-frame-pointer -fsanitize=address)
endif()

# miniz

add_library(miniz STATIC
	ext/miniz/miniz.h
	ext/miniz/miniz.c
)

target_compile_definitions(miniz
	PRIVATE
		MINIZ_NO_ZLIB_COMPATIBLE_NAMES
		MINIZ_NO_ZLIB_APIS
		MINIZ_NO_ARCHIVE_WRITING_APIS
		MINIZ_NO_TIME
		_LARGEFILE64_SOURCE
)

target_include_directories(miniz
	PUBLIC ext/miniz
)

# iniparser

add_library(iniparser STATIC
	ext/iniparser/dictionary.h
	ext/iniparser/dictionary.c
	ext/iniparser/iniparser.h
	ext/iniparser/iniparser.c
)

target_include_directories(iniparser
	PUBLIC ext/iniparser
)

# alex4

add_executable(alex4
	src/alex4.h
	src/main.c
	src/main.h
	src/actor.c
	src/actor.h
	src/bullet.c
	src/bullet.h
	src/control.c
	src/control.h
	src/fixed_math.c
	src/fixed_math.h
	src/data.c
	src/data.h
	src/hisc.c
	src/hisc.h
	src/map.c
	src/map.h
	src/options.c
	src/options.h
	src/particle.c
	src/particle.h
	src/player.c
	src/player.h
	src/script.c
	src/script.h
	src/scroller.c
	src/scroller.h
	src/timer.c
	src/timer.h
	src/token.c
	src/token.h
	src/unix.c
	src/unix.h
	src/misc.c
	src/misc.h
	# new ports need to implement these
	src/sound.h
	src/port.h
	# currently, only sdl port available
	src/sdl/port.c
	src/sdl/port.h
	src/sdl/sound.c
)

target_include_directories(alex4
	PUBLIC src
)

# optional

option(ENABLE_EDITOR "enable map editor (currently broken)" OFF)
if(ENABLE_EDITOR)
	target_sources(alex4 PRIVATE src/edit.c src/edit.h)
	target_compile_definitions(alex4 PRIVATE ENABLE_EDITOR)
endif()

option(ENABLE_SHOOTER "enable minigame (currently broken)" OFF)
if(ENABLE_SHOOTER)
	target_sources(alex4 PRIVATE src/shooter.c src/shooter.h)
	target_compile_definitions(alex4 PRIVATE ENABLE_SHOOTER)
endif()

# paths

if(UNIX)
	include(GNUInstallDirs)

	set(BINDIR ${CMAKE_INSTALL_BINDIR})
	set(datadir ${CMAKE_INSTALL_FULL_DATADIR}/alex4)
elseif(WIN32)
	# put everything in a folder
	set(BINDIR "dist")
	set(datadir ".")
endif()

# game data

set(DATADIR ${datadir} CACHE PATH "path where game data is searched")
target_compile_definitions(alex4 PRIVATE DATADIR="${DATADIR}")

function(create_zip output_file input_files working_dir)
	add_custom_command(
		COMMAND ${CMAKE_COMMAND} -E tar "cf" "${output_file}" --format=zip -- ${input_files}
		WORKING_DIRECTORY "${working_dir}"
		OUTPUT "${output_file}"
		DEPENDS ${input_files}
		COMMENT "Zipping to ${output_file}."
	)
endfunction()

file(GLOB DATAFILES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/data/font/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/images_mono/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/images_color/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/maps/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/music/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/scripts/*
	${CMAKE_CURRENT_SOURCE_DIR}/data/sfx_22/*
)
create_zip(${CMAKE_CURRENT_BINARY_DIR}/data.zip "${DATAFILES}"
	${CMAKE_CURRENT_SOURCE_DIR}/data
)
set(zip_files ${CMAKE_CURRENT_BINARY_DIR}/data.zip)

if(ENABLE_SHOOTER)
	file(GLOB_RECURSE SHOOTERFILES
		LIST_DIRECTORIES false
		CONFIGURE_DEPENDS
		${CMAKE_CURRENT_SOURCE_DIR}/shooter/*
	)
	create_zip(${CMAKE_CURRENT_BINARY_DIR}/shooter.zip "${SHOOTERFILES}"
		${CMAKE_CURRENT_SOURCE_DIR}/shooter
	)
	list(APPEND zip_files ${CMAKE_CURRENT_BINARY_DIR}/shooter.zip)
endif()

add_custom_target(alex4-data ALL DEPENDS ${zip_files})

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_FOUND)
	add_custom_command(
		COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/data/make_header.py
			${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_CURRENT_BINARY_DIR}/data_files.h
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/data_files.h
		DEPENDS ${zip_files} data/make_header.py
		COMMENT "(Re-)generating datafile header."
	)
	target_include_directories(alex4 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	target_sources(alex4 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/data_files.h)
else()
	message(STATUS "Using pre-generated datafile header.")
	target_include_directories(alex4 PRIVATE data)
	target_sources(alex4 PRIVATE data/data_files.h)
endif()

# libs

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
if(SDL2_FOUND AND SDL2_mixer_FOUND)
	message(STATUS "Using SDL2 ${SDL2_VERSION} and SDL2_mixer ${SDL2_mixer_VERSION}.")
endif()

target_link_libraries(alex4 miniz iniparser SDL2::SDL2 SDL2_mixer::SDL2_mixer)
if(UNIX)
	target_link_libraries(alex4 -lm)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(alex4 PRIVATE "-fno-math-errno")
	endif()
endif()

# installation

install(TARGETS alex4 DESTINATION ${BINDIR})
if(UNIX)
	install(FILES unix/alex4_32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps RENAME alex4.png)
	install(FILES unix/alex4_64.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps RENAME alex4.png)
	install(FILES unix/alex4.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()
install(FILES ${zip_files} DESTINATION ${DATADIR})
