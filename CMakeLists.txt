###############################################################################
 # Copyright 2014 Matthew Adams, Elizabeth Reischmann, Patrick Reynolds
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #  http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
###############################################################################
cmake_minimum_required(VERSION 2.8)

project(SeaCoreExplorer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

option(USE_SUPERBUILD "Use superbuild to download and compile VTK." ON)

if(USE_SUPERBUILD)
    include(cmake/Superbuild.cmake)
else()

    # Let's use C++11
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()

    find_package(VTK REQUIRED)
    include(${VTK_USE_FILE})

    find_package(Qt4 REQUIRED)
    include(${QT_USE_FILE})

    find_package(Boost 1.46)

    set(UI_FILES
        ui/MainWindow.ui
    )

    set(RESOURCE_FILES
        ui/SeaCoreExplorer.qrc
    )

    set(QT_WRAP
        include/QSeaCoreTableModel.h
        include/MainWindow.h
    )

    set(Core_CXX_FILES
        src/SeaCore.cxx
    )

    set(SeaCoreExplorer_CXX_FILES
        src/QSeaCoreTableModel.cxx
        src/MainWindow.cxx
        src/main.cxx
    )

    QT4_WRAP_UI(UISrcs ${UI_FILES})
    QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
    QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCE_FILES})

    include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${Boost_INCLUDE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )

    set(SeaCoreComparer_CXX_FILES
        src/SeaCoreComparer.cxx
    )

    add_executable(SeaCoreExplorer
        ${Core_CXX_FILES}
        ${SeaCoreExplorer_CXX_FILES}
        ${UISrcs}
        ${MOCSrcs}
        ${RESOURCES_RCC}
    )

    target_link_libraries(SeaCoreExplorer ${VTK_LIBRARIES} ${QT_LIBRARIES})

    add_executable(SeaCoreComparer
        ${Core_CXX_FILES}
        ${SeaCoreComparer_CXX_FILES}
    )

    target_link_libraries(SeaCoreComparer ${VTK_LIBRARIES})

endif()