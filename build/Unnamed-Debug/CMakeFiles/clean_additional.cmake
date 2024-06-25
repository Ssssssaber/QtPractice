# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/QtPractice_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/QtPractice_autogen.dir/ParseCache.txt"
  "QtPractice_autogen"
  )
endif()
