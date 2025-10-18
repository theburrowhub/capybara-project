cmake_minimum_required(VERSION 3.15)

# Project definition
project(capybara-shootemup 
    VERSION 1.0.0
    DESCRIPTION "Capybara Shoot'em Up - A retro-style space shooter"
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options
option(BUILD_STATIC "Build static binaries" OFF)
option(BUILD_GAME "Build main game" ON)
option(BUILD_TOOLS "Build tools (sprite generators, etc.)" ON)
option(BUILD_DEMOS "Build demo executables" ON)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
endif()

# Find required libraries
find_package(raylib 5.0 QUIET)
if(NOT raylib_FOUND)
    # Try pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)
    
    if(NOT RAYLIB_FOUND)
        message(FATAL_ERROR "raylib not found. Please install raylib 5.0 or later.")
    endif()
endif()

# SQLite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

# Platform-specific libraries
set(PLATFORM_LIBS m)

if(UNIX AND NOT APPLE)
    # Linux
    set(PLATFORM_LIBS ${PLATFORM_LIBS} GL pthread dl rt X11)
elseif(APPLE)
    # macOS
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    set(PLATFORM_LIBS ${PLATFORM_LIBS} 
        ${OPENGL_LIBRARY} 
        ${COCOA_LIBRARY} 
        ${IOKIT_LIBRARY} 
        ${COREVIDEO_LIBRARY}
    )
elseif(WIN32)
    # Windows
    set(PLATFORM_LIBS ${PLATFORM_LIBS} winmm gdi32 opengl32)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
if(RAYLIB_INCLUDE_DIRS)
    include_directories(${RAYLIB_INCLUDE_DIRS})
endif()
if(SQLITE3_INCLUDE_DIRS)
    include_directories(${SQLITE3_INCLUDE_DIRS})
endif()

# Link directories
if(RAYLIB_LIBRARY_DIRS)
    link_directories(${RAYLIB_LIBRARY_DIRS})
endif()
if(SQLITE3_LIBRARY_DIRS)
    link_directories(${SQLITE3_LIBRARY_DIRS})
endif()

# Source files organization
set(CORE_SRCS
    src/core/main.c
    src/core/game.c
)

set(ENTITY_SRCS
    src/entities/player_ship.c
    src/entities/enemy_types.c
)

set(INPUT_SRCS
    src/input/input_config.c
    src/input/input_manager.c
)

set(GAMEPLAY_SRCS
    src/gameplay/weapon.c
    src/gameplay/wave_system.c
    src/gameplay/level_system.c
    src/gameplay/level1_waves.c
    src/gameplay/level2_waves.c
    src/gameplay/powerup.c
)

set(RENDERING_SRCS
    src/rendering/renderer.c
)

set(PHYSICS_SRCS
    src/physics/collision.c
    src/physics/combat_system.c
)

set(EFFECTS_SRCS
    src/effects/projectile_types.c
    src/effects/explosion.c
    src/effects/projectile_manager.c
)

set(UI_SRCS
    src/ui/menu.c
)

set(UTIL_SRCS
    src/utils/logger.c
    src/utils/database.c
)

set(AUDIO_ANALYSIS_SRCS
    src/utils/audio_analysis.c
)

# Function to link common libraries
function(link_game_libraries target)
    if(raylib_FOUND)
        target_link_libraries(${target} raylib)
    else()
        target_link_libraries(${target} ${RAYLIB_LIBRARIES})
    endif()
    target_link_libraries(${target} ${PLATFORM_LIBS})
    
    # Static linking options
    if(BUILD_STATIC)
        if(WIN32)
            target_link_options(${target} PRIVATE -static)
        elseif(UNIX AND NOT APPLE)
            target_link_options(${target} PRIVATE -static-libgcc)
        endif()
    endif()
endfunction()

# Main game executable
if(BUILD_GAME)
    add_executable(shootemup
        ${CORE_SRCS}
        ${ENTITY_SRCS}
        ${INPUT_SRCS}
        ${GAMEPLAY_SRCS}
        ${RENDERING_SRCS}
        ${PHYSICS_SRCS}
        ${EFFECTS_SRCS}
        ${UI_SRCS}
        ${UTIL_SRCS}
    )
    
    link_game_libraries(shootemup)
    target_link_libraries(shootemup ${SQLITE3_LIBRARIES})
    
    # Strip in release mode
    if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT APPLE)
        add_custom_command(TARGET shootemup POST_BUILD
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:shootemup>
            COMMENT "Stripping shootemup binary"
        )
    endif()
endif()

# Tools
if(BUILD_TOOLS)
    # Enemy sprite generator
    add_executable(generate_enemy_sprites
        src/tools/generate_enemy_sprites.c
        src/entities/enemy_types.c
        src/effects/projectile_types.c
    )
    link_game_libraries(generate_enemy_sprites)
    
    # Spaceship sprite generator
    add_executable(generate_spaceship_sprites
        src/tools/generate_spaceship_sprites.c
        src/entities/enemy_types.c
        src/effects/projectile_types.c
    )
    link_game_libraries(generate_spaceship_sprites)
    
    # Projectile sprite generator
    add_executable(generate_projectile_sprites
        src/tools/generate_projectile_sprites.c
        src/effects/projectile_types.c
        src/entities/enemy_types.c
    )
    link_game_libraries(generate_projectile_sprites)
    
    # Player sprite generator
    add_executable(generate_player_sprite
        src/tools/generate_player_sprite.c
    )
    link_game_libraries(generate_player_sprite)
    
    # High score populator
    add_executable(populate_highscores
        src/tools/populate_highscores.c
        src/utils/database.c
    )
    target_link_libraries(populate_highscores ${SQLITE3_LIBRARIES})
endif()

# Demo common source
set(DEMO_COMMON_SRCS
    src/demo/demo_common.c
)

# Demo executables
if(BUILD_DEMOS)
    # Enemy showcase
    add_executable(enemy_showcase
        src/demo/enemy_showcase.c
        ${DEMO_COMMON_SRCS}
        src/entities/enemy_types.c
        src/entities/player_ship.c
        src/effects/projectile_types.c
        src/gameplay/weapon.c
        src/gameplay/wave_system.c
        src/gameplay/level_system.c
        src/gameplay/level1_waves.c
        src/gameplay/level2_waves.c
        src/physics/combat_system.c
        src/effects/projectile_manager.c
        src/physics/collision.c
        src/effects/explosion.c
        src/gameplay/powerup.c
        src/input/input_config.c
        src/input/input_manager.c
        src/utils/logger.c
    )
    link_game_libraries(enemy_showcase)
    
    # Enemy showcase sprites
    add_executable(enemy_showcase_sprites
        src/demo/enemy_showcase_sprites.c
        ${DEMO_COMMON_SRCS}
        src/entities/enemy_types.c
        src/effects/projectile_types.c
        src/effects/explosion.c
        src/effects/projectile_manager.c
        src/input/input_config.c
        src/input/input_manager.c
    )
    link_game_libraries(enemy_showcase_sprites)
    
    # Projectile showcase
    add_executable(projectile_showcase
        src/demo/projectile_showcase.c
        ${DEMO_COMMON_SRCS}
        src/effects/projectile_types.c
        src/entities/enemy_types.c
        src/effects/explosion.c
        src/effects/projectile_manager.c
        src/input/input_config.c
        src/input/input_manager.c
    )
    link_game_libraries(projectile_showcase)
    
    # Player ship showcase
    add_executable(player_showcase
        src/demo/player_ship_showcase.c
        ${DEMO_COMMON_SRCS}
        src/entities/player_ship.c
        src/effects/projectile_types.c
        src/entities/enemy_types.c
        src/effects/explosion.c
        src/effects/projectile_manager.c
        src/input/input_config.c
        src/input/input_manager.c
    )
    link_game_libraries(player_showcase)
    
    # Powerup showcase
    add_executable(powerup_showcase
        src/demo/powerup_showcase.c
        ${DEMO_COMMON_SRCS}
        src/gameplay/powerup.c
        src/entities/player_ship.c
        src/entities/enemy_types.c
        src/effects/projectile_types.c
        src/effects/explosion.c
        src/effects/projectile_manager.c
        src/input/input_config.c
        src/input/input_manager.c
    )
    link_game_libraries(powerup_showcase)
    
    # Audio analysis GUI
    add_executable(audio_analysis_gui
        src/demo/audio_analysis_gui.c
        ${AUDIO_ANALYSIS_SRCS}
    )
    link_game_libraries(audio_analysis_gui)
    
    # Audio analysis CLI
    add_executable(audio_analysis_cli
        src/demo/audio_analysis_cli.c
        ${AUDIO_ANALYSIS_SRCS}
    )
    link_game_libraries(audio_analysis_cli)
endif()

# Installation rules
if(BUILD_GAME)
    install(TARGETS shootemup
        RUNTIME DESTINATION .
    )
    
    # Install assets
    install(DIRECTORY assets/
        DESTINATION assets
    )
    
    # Install documentation
    install(FILES README.md
        DESTINATION .
    )
    
    if(EXISTS ${CMAKE_SOURCE_DIR}/docs/USER_MANUAL.pdf)
        install(FILES docs/USER_MANUAL.pdf
            DESTINATION docs
        )
    endif()
    
    if(EXISTS ${CMAKE_SOURCE_DIR}/docs/GAMEPLAY.md)
        install(FILES docs/GAMEPLAY.md
            DESTINATION docs
        )
    endif()
    
    # Platform-specific run scripts
    if(WIN32)
        # Create run.bat
        file(WRITE ${CMAKE_BINARY_DIR}/run.bat "@echo off\nshootemup.exe\n")
        install(FILES ${CMAKE_BINARY_DIR}/run.bat
            DESTINATION .
        )
    else()
        # Create run.sh
        file(WRITE ${CMAKE_BINARY_DIR}/run.sh "#!/bin/bash\n./shootemup\n")
        install(FILES ${CMAKE_BINARY_DIR}/run.sh
            DESTINATION .
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )
    endif()
endif()

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "capybara-shootemup")
set(CPACK_PACKAGE_VENDOR "Capybara Games")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Capybara Shoot'em Up - A retro-style space shooter")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# Use version from environment variable if available (for CI/CD)
if(DEFINED ENV{PROJECT_VERSION})
    set(CPACK_PACKAGE_VERSION $ENV{PROJECT_VERSION})
endif()

# Platform-specific package names
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "capybara-shootemup-windows-x86_64-${CPACK_PACKAGE_VERSION}")
elseif(APPLE)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "capybara-shootemup-macos-universal-${CPACK_PACKAGE_VERSION}")
elseif(UNIX)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_PACKAGE_FILE_NAME "capybara-shootemup-linux-x86_64-${CPACK_PACKAGE_VERSION}")
endif()

include(CPack)

# Print configuration summary
message(STATUS "==============================================")
message(STATUS "Capybara Shoot'em Up - Build Configuration")
message(STATUS "==============================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "Static linking:    ${BUILD_STATIC}")
message(STATUS "Build game:        ${BUILD_GAME}")
message(STATUS "Build tools:       ${BUILD_TOOLS}")
message(STATUS "Build demos:       ${BUILD_DEMOS}")
message(STATUS "C Compiler:        ${CMAKE_C_COMPILER}")
message(STATUS "Platform:          ${CMAKE_SYSTEM_NAME}")
message(STATUS "==============================================")
