cmake_minimum_required(VERSION 3.15)
project(capybara_shootemup C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Options for static linking
option(BUILD_STATIC "Build with static linking" OFF)
option(BUILD_GAME "Build the main game" ON)
option(BUILD_TOOLS "Build tool executables" ON)
option(BUILD_DEMOS "Build demo/showcase executables" ON)

# Compiler flags
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Static linking flags
if(BUILD_STATIC)
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
    endif()
endif()

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find required packages
find_package(raylib 4.0 QUIET)
if(NOT raylib_FOUND)
    # If raylib is not found, try pkg-config
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(RAYLIB REQUIRED raylib)
    include_directories(${RAYLIB_INCLUDE_DIRS})
    link_directories(${RAYLIB_LIBRARY_DIRS})
endif()

# SQLite3
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(SQLITE3 sqlite3)
    if(SQLITE3_FOUND)
        set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARIES})
        include_directories(${SQLITE3_INCLUDE_DIRS})
        link_directories(${SQLITE3_LIBRARY_DIRS})
    endif()
endif()

if(NOT SQLITE3_FOUND)
    if(BUILD_STATIC AND UNIX AND NOT APPLE)
        # For static builds on Linux
        find_library(SQLITE3_LIBRARY NAMES libsqlite3.a sqlite3)
    else()
        # For dynamic builds or macOS
        find_library(SQLITE3_LIBRARY NAMES sqlite3)
    endif()
    
    if(SQLITE3_LIBRARY)
        set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
    else()
        message(FATAL_ERROR "SQLite3 not found. Install with: brew install sqlite3 (macOS) or apt install libsqlite3-dev (Linux)")
    endif()
endif()

# Platform-specific libraries
set(PLATFORM_LIBS "")
if(UNIX AND NOT APPLE)
    # Find OpenGL on Linux
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS ${OPENGL_LIBRARIES} pthread dl rt X11 m)
elseif(APPLE)
    find_library(OPENGL_FRAMEWORK OpenGL)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREVIDEO_FRAMEWORK CoreVideo)
    set(PLATFORM_LIBS 
        ${OPENGL_FRAMEWORK}
        ${COCOA_FRAMEWORK}
        ${IOKIT_FRAMEWORK}
        ${COREVIDEO_FRAMEWORK}
        m
    )
elseif(WIN32)
    set(PLATFORM_LIBS winmm gdi32 opengl32)
endif()

# Source file groups
set(CORE_SRCS
    src/core/main.c
    src/core/game.c
)

set(ENTITY_SRCS
    src/entities/player_ship.c
    src/entities/enemy_types.c
)

set(SYSTEM_SRCS
    src/systems/weapon.c
    src/systems/collision.c
    src/systems/renderer.c
    src/systems/wave_system.c
    src/systems/level_system.c
    src/systems/level1_waves.c
    src/systems/level2_waves.c
    src/systems/projectile_types.c
    src/systems/explosion.c
    src/systems/combat_system.c
    src/systems/projectile_manager.c
    src/systems/powerup.c
    src/systems/menu.c
)

set(UTIL_SRCS
    src/utils/logger.c
    src/utils/database.c
)

set(AUDIO_ANALYSIS_SRCS
    src/utils/audio_analysis.c
)

# Main game executable
if(BUILD_GAME)
    add_executable(shootemup
        ${CORE_SRCS}
        ${ENTITY_SRCS}
        ${SYSTEM_SRCS}
        ${UTIL_SRCS}
    )

    if(raylib_FOUND)
        target_link_libraries(shootemup raylib ${SQLITE3_LIBRARIES} ${PLATFORM_LIBS})
    else()
        target_link_libraries(shootemup ${RAYLIB_LIBRARIES} ${SQLITE3_LIBRARIES} ${PLATFORM_LIBS})
    endif()

    # Install target
    install(TARGETS shootemup DESTINATION bin)
endif()

# Enemy showcase
if(BUILD_DEMOS)
    add_executable(enemy_showcase
        src/demo/enemy_showcase.c
        src/entities/enemy_types.c
        src/entities/player_ship.c
        src/systems/projectile_types.c
        src/systems/weapon.c
        src/systems/wave_system.c
        src/systems/level_system.c
        src/systems/level1_waves.c
        src/systems/level2_waves.c
        src/systems/combat_system.c
        src/systems/projectile_manager.c
        src/systems/collision.c
        src/systems/explosion.c
        src/systems/powerup.c
        src/utils/logger.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(enemy_showcase raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(enemy_showcase ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Sprite showcase
if(BUILD_DEMOS)
    add_executable(enemy_showcase_sprites
        src/demo/enemy_showcase_sprites.c
        src/entities/enemy_types.c
        src/systems/projectile_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(enemy_showcase_sprites raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(enemy_showcase_sprites ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Projectile showcase
if(BUILD_DEMOS)
    add_executable(projectile_showcase
        src/demo/projectile_showcase.c
        src/systems/projectile_types.c
        src/entities/enemy_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(projectile_showcase raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(projectile_showcase ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Player showcase
if(BUILD_DEMOS)
    add_executable(player_showcase
        src/demo/player_ship_showcase.c
        src/entities/player_ship.c
        src/systems/projectile_types.c
        src/entities/enemy_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(player_showcase raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(player_showcase ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Powerup showcase
if(BUILD_DEMOS)
    add_executable(powerup_showcase
        src/demo/powerup_showcase.c
        src/systems/powerup.c
        src/entities/player_ship.c
        src/entities/enemy_types.c
        src/systems/projectile_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(powerup_showcase raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(powerup_showcase ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Audio analysis GUI
if(BUILD_DEMOS)
    add_executable(audio_analysis_gui
        src/demo/audio_analysis_gui.c
        ${AUDIO_ANALYSIS_SRCS}
    )
    
    if(raylib_FOUND)
        target_link_libraries(audio_analysis_gui raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(audio_analysis_gui ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Audio analysis CLI
if(BUILD_DEMOS)
    add_executable(audio_analysis_cli
        src/demo/audio_analysis_cli.c
        ${AUDIO_ANALYSIS_SRCS}
    )
    
    if(raylib_FOUND)
        target_link_libraries(audio_analysis_cli raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(audio_analysis_cli ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# Tools - Sprite generators
if(BUILD_TOOLS)
    add_executable(generate_enemy_sprites
        src/tools/generate_enemy_sprites.c
        src/entities/enemy_types.c
        src/systems/projectile_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(generate_enemy_sprites raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(generate_enemy_sprites ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()

    add_executable(generate_spaceship_sprites
        src/tools/generate_spaceship_sprites.c
        src/entities/enemy_types.c
        src/systems/projectile_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(generate_spaceship_sprites raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(generate_spaceship_sprites ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()

    add_executable(generate_projectile_sprites
        src/tools/generate_projectile_sprites.c
        src/systems/projectile_types.c
        src/entities/enemy_types.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(generate_projectile_sprites raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(generate_projectile_sprites ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()

    add_executable(generate_player_sprite
        src/tools/generate_player_sprite.c
    )
    
    if(raylib_FOUND)
        target_link_libraries(generate_player_sprite raylib ${PLATFORM_LIBS})
    else()
        target_link_libraries(generate_player_sprite ${RAYLIB_LIBRARIES} ${PLATFORM_LIBS})
    endif()
endif()

# High score populator
if(BUILD_TOOLS)
    add_executable(populate_highscores
        src/tools/populate_highscores.c
        src/utils/database.c
    )
    
    target_link_libraries(populate_highscores ${SQLITE3_LIBRARIES})
endif()

# Install assets
install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION .)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/docs DESTINATION .)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "CapybaraShootEmUp")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "Capybara Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A side-scrolling shoot'em up game")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

