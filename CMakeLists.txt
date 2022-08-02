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

# alex4

add_executable(alex4
	src/main.c
	src/main.h
	src/actor.c
	src/actor.h
	src/bullet.c
	src/bullet.h
	src/control.c
	src/control.h
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
	src/sdl_wrapper.c
	src/sdl_wrapper.h
	src/sound.c
	src/sound.h
	src/misc.c
	src/misc.h
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

set(_data ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(_maps ${CMAKE_CURRENT_SOURCE_DIR}/maps)
set(_sfx ${CMAKE_CURRENT_SOURCE_DIR}/sfx_22)
file(GLOB DATAFILES CONFIGURE_DEPENDS ${_data}/*)
file(GLOB MAPFILES CONFIGURE_DEPENDS ${_maps}/*)
file(GLOB SFXFILES CONFIGURE_DEPENDS ${_sfx}/*)
create_zip(${CMAKE_CURRENT_BINARY_DIR}/data.zip "${DATAFILES}" ${_data})
create_zip(${CMAKE_CURRENT_BINARY_DIR}/maps.zip "${MAPFILES}" ${_maps})
create_zip(${CMAKE_CURRENT_BINARY_DIR}/sfx_22.zip "${SFXFILES}" ${_sfx})
unset(_data)
unset(_maps)
unset(_sfx)

set(zip_files
	${CMAKE_CURRENT_BINARY_DIR}/data.zip
	${CMAKE_CURRENT_BINARY_DIR}/maps.zip
	${CMAKE_CURRENT_BINARY_DIR}/sfx_22.zip
)

if(ENABLE_SHOOTER)
	set(_shooter ${CMAKE_CURRENT_SOURCE_DIR}/shooter)
	file(GLOB SHOOTERFILES CONFIGURE_DEPENDS ${_shooter}/*)
	create_zip(${CMAKE_CURRENT_BINARY_DIR}/shooter.zip "${SHOOTERFILES}" ${_shooter})
	unset(_shooter)
	list(APPEND zip_files ${CMAKE_CURRENT_BINARY_DIR}/shooter.zip)
endif()

add_custom_target(alex4-data ALL DEPENDS ${zip_files})

# libs

find_package(SDL2 REQUIRED VERBOSE)
find_package(SDL2_mixer REQUIRED VERBOSE)

target_link_libraries(alex4 miniz SDL2::SDL2 SDL2_mixer::SDL2_mixer)

# installation

install(TARGETS alex4 DESTINATION ${BINDIR})
if(UNIX)
	install(FILES unix/alex4.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
	install(FILES unix/alex4.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()
install(FILES ${zip_files} DESTINATION ${DATADIR})
