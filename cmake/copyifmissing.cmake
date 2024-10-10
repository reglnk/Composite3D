cmake_path(GET dest PARENT_PATH dirde)
if(NOT EXISTS "${dest}")
  file(COPY "${src}" DESTINATION "${dirde}")
endif()