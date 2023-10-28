cmake_minimum_required(VERSION 3.22)
project(RType)

set(CMAKE_CXX_STANDARD 17)

# Add an option for ASan build
option(USE_ASAN "Use AddressSanitizer" OFF)
option(RENDER_HITBOX "Enable hitbox rendering" OFF)
option(USE_GPROF "Use gprof for profiling" OFF)
option(CLIENT_MODE "Build the client version" OFF)
option(SERVER_MODE "Build the server version" OFF)

include_directories(include)


# Find out which platform we're building on
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    # Windows specific code
    set(SFML_DIR "${PROJECT_SOURCE_DIR}/_myDeps/SFML/lib/cmake/SFML")
    link_directories("${PROJECT_SOURCE_DIR}/_myDeps/SFML/lib")
    # FFTW on Windows
    include_directories("${PROJECT_SOURCE_DIR}/_myDeps/fftw-3.3.5-dll64")
    link_directories("${PROJECT_SOURCE_DIR}/_myDeps/fftw-3.3.5-dll64")

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    # Linux specific code
    set(SFML_DIR "${PROJECT_SOURCE_DIR}/_myDeps/SFML/lib/cmake/SFML")

    # Try to find SFML, and if not found, install it.
    find_package(SFML 2.5 COMPONENTS graphics audio window system)
    if(NOT SFML_FOUND)
        message("SFML not found, installing...")
        execute_process(COMMAND sudo apt-get update)
        execute_process(COMMAND sudo apt-get install -y libsfml-dev)
        find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)
    endif()
    include_directories("/usr/include")
    link_directories("/usr/lib/x86_64-linux-gnu")
    # FFTW on Linux
    find_package(PkgConfig REQUIRED)
    pkg_search_module(FFTW fftw3 IMPORTED_TARGET)

    if(NOT FFTW_FOUND)
        message("FFTW not found, installing...")
        execute_process(COMMAND sudo apt-get update)
        execute_process(COMMAND sudo apt-get install -y libfftw3-dev)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
    endif()
    if(FFTW_FOUND)
        message("FFTW found at ${PkgConfig_FFTW_INCLUDE_DIRS}")
    endif()

    pkg_search_module(FFTWF fftw3f IMPORTED_TARGET)
    if(NOT FFTWF_FOUND)
        message("FFTWF not found, installing...")
        execute_process(COMMAND sudo apt-get update)
        execute_process(COMMAND sudo apt-get install -y libfftw3-dev)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(FFTWF REQUIRED fftw3f IMPORTED_TARGET)
    endif()
    if(FFTWF_FOUND)
        message("FFTW found at ${PkgConfig_FFTWF_INCLUDE_DIRS}")
    endif()
    pkg_search_module(GoogleTest REQUIRED gtest IMPORTED_TARGET)
    if(NOT GoogleTest_FOUND)
        message("GoogleTest not found, installing...")
        execute_process(COMMAND sudo apt-get update)
        execute_process(COMMAND sudo apt-get install -y libgtest-dev)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(GoogleTest REQUIRED gtest IMPORTED_TARGET)
    endif ()

    if (GoogleTest_FOUND)
        message("GoogleTest found at ${PkgConfig_GoogleTest_INCLUDE_DIRS}")
    endif ()
    FIND_PATH(CONFIG++_INCLUDE_DIR libconfig.h++ /usr/include /usr/local/include)

    FIND_LIBRARY(CONFIG++_LIBRARY NAMES config++ PATH /usr/lib /usr/local/lib)
    IF (CONFIG++_INCLUDE_DIR AND CONFIG++_LIBRARY)
        SET(CONFIG++_FOUND TRUE)
        MESSAGE(STATUS "Found DIR AND LIBRARY")
    ENDIF ( CONFIG++_INCLUDE_DIR AND CONFIG++_LIBRARY)
    IF (CONFIG++_FOUND)
            MESSAGE(STATUS "Found Config++: ${CONFIG++_LIBRARY}")
    ELSEIF(NOT CONFIG++_FOUND)
            MESSAGE( "Could not find Config++")
            execute_process(COMMAND sudo apt-get update)
            execute_process(COMMAND sudo apt-get install -y libconfig++-dev)
    ENDIF (CONFIG++_FOUND)


endif()

# Conditionally set ASan flags
if(USE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# Conditionally set the RENDER_HITBOX flag
if(RENDER_HITBOX)
    add_compile_definitions(RENDER_HITBOX)
endif()

# Conditionally set USE_GPROF flag
if(USE_GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()



set (COMMON_SRC_FILES
        include/SpritePool.hpp
        src/ThreadPool.cpp
        src/SpritePool.cpp
        src/Component.cpp
        src/Behaviour.cpp
        src/TextureManager.cpp
        src/Entity.cpp
        src/Registry.cpp
        src/main.cpp
        src/Spawner.cpp
        include/Batch.hpp
        src/Batch.cpp
        include/ISystem.hpp
        include/System.hpp
        src/QuadTree.cpp
        include/Quadtree.hpp
        include/ThreadPool.hpp
        include/Database/Database.hpp
        src/Database/Database.cpp
        src/Beatmap/BeatmapConfig.cpp
        src/GraphicElement/BeatmapPanel.cpp
        src/GraphicElement/Button.cpp
        src/GraphicElement/ButtonConfig.cpp
        src/GraphicElement/LoadingScreen.cpp
        src/GraphicElement/RectangleButton.cpp
        src/GUI/GUI.cpp
        src/GUI/GUIConfig.cpp
        src/Menu/Menu.cpp
        src/Config/Config.cpp
        src/ClientMain.cpp
        src/ServerMain.cpp
        src/taskLoader.cpp
        include/ClientMain.hpp
        src/ConfigServer.cpp
        src/Event.cpp
        src/global.cpp
        src/SceneParsing.cpp
        src/MenuSystem.cpp
)


# Main executable for Server
add_executable(ServerRTYPE ${COMMON_SRC_FILES})

# Main executable for Client
add_executable(ClientRTYPE ${COMMON_SRC_FILES})

# Conditionally set CLIENT_MODE flag
# Conditionally set CLIENT_MODE flag for Client
if(CLIENT_MODE)
    target_compile_definitions(ClientRTYPE PRIVATE CLIENT_MODE)
endif()

# Conditionally set SERVER_MODE flag for Server
if(SERVER_MODE)
    target_compile_definitions(ServerRTYPE PRIVATE SERVER_MODE)
endif()
# Link libraries for Server
target_link_libraries(ServerRTYPE
        PRIVATE
        PkgConfig::FFTW
        PkgConfig::FFTWF
        PkgConfig::GoogleTest
        sfml-graphics sfml-audio sfml-window sfml-system
        config++
)

# Link libraries for Client
target_link_libraries(ClientRTYPE
        PRIVATE
        PkgConfig::FFTW
        PkgConfig::FFTWF
        PkgConfig::GoogleTest
        sfml-graphics sfml-audio sfml-window sfml-system
        config++
)