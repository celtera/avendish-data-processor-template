cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(MyData CXX)

# Max/MSP externals link the static CRT (/MT) on Windows, as the official
# max-sdk-base requires. Set it for the whole addon -- before Avendish and the
# object library are created -- so every target shares one runtime; mixing /MT
# and /MD trips MSVC with LNK2038. (CMP0091 is NEW via cmake_minimum_required.)
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Use the Avendish the host already provides (e.g. ossia score); otherwise fetch it.
# The same CMakeLists builds this as an ossia/score add-on or as a standalone object.
find_package(Avendish QUIET)
if(NOT Avendish_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Avendish
    GIT_REPOSITORY "https://github.com/celtera/avendish"
    GIT_TAG 00827f5cc3b2323cfa3a554e799397b4fab14701)
  FetchContent_Populate(Avendish)
  list(APPEND CMAKE_PREFIX_PATH "${avendish_SOURCE_DIR}")
  find_package(Avendish REQUIRED)
endif()

avnd_addon_init(NAME MyData)

# CATEGORY object: the data / message back-ends (no audio plug-in formats) -- ossia,
# Max/MSP, Pure Data, Python, TouchDesigner (CHOP message), Godot. In a score build only
# the ossia process is produced. Back-ends whose SDK is absent are silently skipped.
avnd_addon_object(
  BASE MyData
  C_NAME my_data_processor
  CLASS MyDataProcessor
  # Explicit back-end list (the object-category set) including dump — the
  # plain-C++ introspection target the portability CI builds.
  BACKENDS dump ossia python pd max standalone wasm example_host
           touchdesigner:CHOP_MESSAGE godot:NODE
  MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/Processor.hpp"
  SOURCES
    src/Model.cpp
    src/Model.hpp
    src/Processor.hpp)

avnd_addon_finalize(NAME MyData UUID d589bb28-5055-4758-9236-00747b31a41d VERSION 1)
