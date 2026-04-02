# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appMusicApp_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appMusicApp_autogen.dir\\ParseCache.txt"
  "appMusicApp_autogen"
  )
endif()
