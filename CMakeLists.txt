cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(MyData CXX)

include(dependencies.cmake)

add_library(MyDataProcessor STATIC
  src/Model.cpp
  src/Model.hpp
  src/Processor.hpp
)

# avnd_make_object instantiates the data / message back-ends (no audio plug-in
# formats): ossia, Max/MSP, Pure Data, Python, TouchDesigner (CHOP message), Godot.
# Back-ends whose SDK is not found are silently skipped.
avnd_make_object(
  TARGET MyDataProcessor
  MAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/Processor.hpp"
  MAIN_CLASS MyDataProcessor
  C_NAME my_data_processor
)

# Workaround: when godot-cpp is pulled in via FetchContent, its only targets are
# `godot-cpp` (+ alias `godot::cpp`) — there is no `godot-cpp::godot-cpp`. Avendish's
# avnd_make_godot doesn't always resolve the target, so the generated headers under
# <godot-cpp-build>/gen/include never make it onto the GDExtension's include path and
# compilation fails with "godot_cpp/classes/node.hpp not found". Re-linking the target
# here (at a scope where it is visible) propagates godot-cpp's PUBLIC include dirs.
if(TARGET MyDataProcessor_NODE_godot AND TARGET godot-cpp)
  target_link_libraries(MyDataProcessor_NODE_godot PRIVATE godot-cpp)
endif()
