# *********************************************************************
# LP3_CORE
#       Handles very rudimentary, basic stuff all the other libs need.
# *********************************************************************
project(Lp3_Core CXX)
cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)
include(CMakePackageConfigHelpers)

enable_testing()

include("${CMAKE_BINARY_DIR}/conan_paths.cmake"
        OPTIONAL
        RESULT_VARIABLE using_conan)

if(using_conan)
    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
endif()
# TODO: if I keep onn supporting cget, simply set Catch2, gsl_microsoft to
#       empty libraries if using_conan is false.
find_package(Boost 1.71.0 REQUIRED)
find_package(Catch2 REQUIRED)
find_package(gsl_microsoft REQUIRED)

if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    find_package(sdl2 2.0.9 MODULE REQUIRED)
    find_package(sdl2_image 2.0.5 REQUIRED)
endif()

add_library(lp3_core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/le.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/assert.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/casts.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/log.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/main.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/Exception.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/exit.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/utils.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/assert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ErrOut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ErrOut.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sdl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WCharToCharConverter.cpp
)
set_target_properties(lp3_core PROPERTIES OUTPUT_NAME "Lp3_Core")

# Mandate the use of at least C++17 by everything that uses this
target_compile_features(lp3_core
    PUBLIC
    cxx_std_17
)

target_include_directories(lp3_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src)

# This is built as a "shared library" in sarcastic air quotes. It's only
# made that way to make linking faster, and relies on consumers using the same
# version of the runtime it was built with. IIRC this used to not be that big
# of a problem with VS, but now it is, thus the disabled warnings.
if(BUILD_SHARED_LIBS)
    target_compile_definitions(lp3_core
        PUBLIC  LP3_CORE_API_DYNAMIC
        PRIVATE LP3_CORE_API_CREATE
    )
    if (MSVC)
        target_compile_options(lp3_core PRIVATE /wd4251 /wd4275)
    endif()
endif()

target_link_libraries(lp3_core
    PUBLIC Boost::boost
           gsl_microsoft::gsl_microsoft
)

if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_link_libraries(lp3_core
        PUBLIC sdl2::sdl2
               sdl2_image::sdl2_image
    )
else()
    target_compile_options(lp3_core PUBLIC
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_IMAGE=2"
        "SHELL:-s SDL2_IMAGE_FORMATS=[\"png\",\"bmp\"]"
    )
    target_link_options(lp3_core PUBLIC
        "SHELL:-s USE_SDL=2"
        "SHELL:-s USE_SDL_IMAGE=2"
        "SHELL:-s SDL2_IMAGE_FORMATS=[\"png\",\"bmp\"]"
    )
endif()


if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_sources(lp3_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/js/platform.cpp)
else()
    target_sources(lp3_core PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/pc/platform.cpp)
endif()

# Unfortunately, CMake's precompiled headers aren't working in Emscripten yet.
if (NOT "${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
    target_precompile_headers(lp3_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/le.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/assert.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/casts.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/log.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/sdl.hpp

        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/config.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/Exception.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/exit.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lp3/core/utils.hpp
    )
endif()


# *********************************************************************
# Package / Install Stuff
# *********************************************************************

install(DIRECTORY include/ DESTINATION include)

install(TARGETS lp3_core EXPORT Lp3_Core-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

add_library(lp3::core ALIAS lp3_core)
install(EXPORT Lp3_Core-targets
    FILE Lp3_Core-targets.cmake
    NAMESPACE lp3::
    DESTINATION lib/cmake/Lp3_Core
)

file(WRITE "${PROJECT_BINARY_DIR}/Lp3_Core-config.cmake" "
include(CMakeFindDependencyMacro)
find_dependency(Boost)
find_dependency(sdl2)
include(\"\${CMAKE_CURRENT_LIST_DIR}/Lp3_Core-targets.cmake\")
")

write_basic_package_version_file("${PROJECT_BINARY_DIR}/Lp3_Core-config-version.cmake"
    VERSION 1.0.1
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${PROJECT_BINARY_DIR}/Lp3_Core-config.cmake"
    "${PROJECT_BINARY_DIR}/Lp3_Core-config-version.cmake"
    DESTINATION lib/cmake/Lp3_Core
)

# *********************************************************************
# Tests and Drivers / Demos
# *********************************************************************

get_directory_property(is_child PARENT_DIRECTORY)
if (NOT is_child)
    set(LP3_CORE_FULL True)
endif()

if (LP3_CORE_FULL)
    add_executable(ExitDemo ${CMAKE_CURRENT_SOURCE_DIR}/demos/ExitDemo.cpp)
    target_link_libraries(ExitDemo lp3_core)

    add_executable(LogDemo ${CMAKE_CURRENT_SOURCE_DIR}/demos/LogDemo.cpp)
    target_link_libraries(LogDemo lp3_core)

    add_executable(MainDemo ${CMAKE_CURRENT_SOURCE_DIR}/demos/MainDemo.cpp)
    target_link_libraries(MainDemo lp3_core)

    add_executable(assert_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/assert_tests.cpp)
    target_link_libraries(assert_tests lp3_core Catch2::Catch2)
    add_test(assert_tests assert_tests)

    add_executable(media_tests ${CMAKE_CURRENT_SOURCE_DIR}/tests/media_tests.cpp)
    target_link_libraries(media_tests lp3_core Catch2::Catch2)
    add_test(media_tests media_tests)
    if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
        # Only bother embedding the one file.
        target_link_options(media_tests PRIVATE
            "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/ci/media/core/story.txt@/core/story.txt"
        )
    endif()

    add_executable(Sdl2Simple WIN32
                   ${CMAKE_CURRENT_SOURCE_DIR}/demos/Sdl2Simple.cpp)
    target_link_libraries(Sdl2Simple lp3_core)
    if ("${CMAKE_SYSTEM_NAME}" MATCHES "Emscripten")
        set_target_properties(Sdl2Simple PROPERTIES
            SUFFIX ".html"
            LINK_FLAGS "--emrun"
        )
        target_link_options(Sdl2Simple PRIVATE
            "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/ci/media@/"
        )
    endif()

    add_executable(MultipleWindows WIN32
                   ${CMAKE_CURRENT_SOURCE_DIR}/demos/MultipleWindows.cpp)
    target_link_libraries(MultipleWindows lp3_core)

    if (LP3_SETUP_FOLDERS)
        set_target_properties (lp3_core PROPERTIES FOLDER core)
        set_target_properties (ExitDemo PROPERTIES FOLDER core)
        set_target_properties (LogDemo PROPERTIES FOLDER core)
        set_target_properties (MainDemo PROPERTIES FOLDER core)
        set_target_properties (assert_tests PROPERTIES FOLDER core)
        set_target_properties (media_tests PROPERTIES FOLDER core)
        set_target_properties (Sdl2Simple PROPERTIES FOLDER core)
        set_target_properties (MultipleWindows PROPERTIES FOLDER core)
    endif()
    if(BUILD_SHARED_LIBS)
        if (MSVC)
            target_compile_options(assert_tests PRIVATE /wd4251 /wd4275)
            target_compile_options(ExitDemo PRIVATE /wd4251 /wd4275)
            target_compile_options(LogDemo PRIVATE /wd4251 /wd4275)
            target_compile_options(MainDemo PRIVATE /wd4251 /wd4275)
            target_compile_options(assert_tests PRIVATE /wd4251 /wd4275)
            target_compile_options(media_tests PRIVATE /wd4251 /wd4275)
            target_compile_options(Sdl2Simple PRIVATE /wd4251 /wd4275)
            target_compile_options(MultipleWindows PRIVATE /wd4251 /wd4275)
        endif()
    endif()
endif()

