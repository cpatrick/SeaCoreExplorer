#-----------------------------------------------------------------------------
# Get and build VTK
#-----------------------------------------------------------------------------

ExternalProject_Add(VTK
  GIT_REPOSITORY "${git_protocol}://vtk.org/VTK.git"
  GIT_TAG "v6.1.0"
  SOURCE_DIR VTK
  BINARY_DIR VTK-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${ep_common_args}
    -DBUILD_SHARED_LIBS:BOOL=TRUE
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=""
  INSTALL_COMMAND ""
)

set(VTK_DIR ${CMAKE_BINARY_DIR}/VTK-build)
