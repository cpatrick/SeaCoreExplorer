find_package(Git)
if(NOT GIT_FOUND)
  message(ERROR "Cannot find git. git is required for Superbuild")
endif()

option( USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

set(git_protocol "git")

include( ExternalProject )

# Compute -G arg for configuring external projects with the same CMake
# generator:
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}" )
endif()

set( SeaCoreExplorer_DEPENDENCIES )

set(ep_common_args
    "-DCMAKE_BUILD_TYPE:STRING=Release"
)

include( ${CMAKE_SOURCE_DIR}/cmake/External-VTK.cmake )
list( APPEND SeaCoreExplorer_DEPENDENCIES VTK )

ExternalProject_Add( SeaCoreExplorer-Inner
  DEPENDS ${SeaCoreExplorer_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_SOURCE_DIR}
  BINARY_DIR SeaCoreExplorer-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DUSE_SUPERBUILD:BOOL=OFF
    -DVTK_DIR:PATH=${VTK_DIR}
  INSTALL_COMMAND ""
)
