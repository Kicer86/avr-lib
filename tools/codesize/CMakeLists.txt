project(codesize)
cmake_minimum_required(VERSION 2.8)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories(${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} .)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DYYDEBUG=1)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

set(codesize_SRC 
  codesize.cpp
)

flex_target(codesizeFlex codesize.l ${CMAKE_CURRENT_BINARY_DIR}/codesize.yy.cc
    COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/codesize.yy.hh")
bison_target(codesizeBison codesize.y ${CMAKE_CURRENT_BINARY_DIR}/codesize.tab.cc)
add_flex_bison_dependency(codesizeFlex codesizeBison)

add_executable(codesize 
  ${codesize_SRC}
  ${FLEX_codesizeFlex_OUTPUTS}
  ${BISON_codesizeBison_OUTPUTS}
  )

#link codesize staticaly with flex
target_link_libraries(codesize ${FLEX_LIBRARIES})

#install files
# if (WIN32)  # for windows (dll = runtime)
#   install (TARGETS htmlParser RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})
# else (WIN32)
#   install (TARGETS htmlParser LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
# endif(WIN32)

# install (FILES ${HTMLPARSER_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
