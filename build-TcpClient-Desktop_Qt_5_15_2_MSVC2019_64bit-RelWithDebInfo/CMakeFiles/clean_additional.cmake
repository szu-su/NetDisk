# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "RelWithDebInfo")
  file(REMOVE_RECURSE
  "CMakeFiles\\TcpClient_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\TcpClient_autogen.dir\\ParseCache.txt"
  "TcpClient_autogen"
  )
endif()
