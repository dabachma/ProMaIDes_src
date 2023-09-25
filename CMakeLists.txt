cmake_minimum_required(VERSION 3.23)

##Create configs
set(CMAKE_CONFIGURATION_TYPES Debug;Release)

##Create Solution
##This will be the name of the .sln file
project(pro_Solution)

##Folder used to hide All_BUILD & other targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

##Using C++ 11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

## Output variables from python
message("Qt Include set to: " ${QT_DIR})
message("Qwt Include set to: " ${QWT_INCLUDE_DIR})
message("Qwt Library Release set to: " ${QWT_LIBRARY_RELEASE})
message("Qwt Library Debug set to: " ${QWT_LIBRARY_DEBUG})
message("Dlls Directory set to: " ${DLL_DIR})
message("")


#Add QT Path
list(APPEND CMAKE_PREFIX_PATH ${QT_DIR})




###########################################################################
## List of files to work with
###########################################################################
set(UI_FILES
    source_code/Main_Wid.ui
    source_code/Sys_Data_Tree_Wid.ui
)

set(H_FILES
    source_code/Main_Wid.h
    source_code/Main_Headers_Precompiled.h
    source_code/Sys_Data_Tree_Wid.h
    source_code/Sys_Output_Text_Wid.h
    source_code/Sys_Dock_Widget.h
    source_code/Sys_Version_Update.h
)

set(CPP_FILES
    source_code/Main_Wid.cpp
    source_code/Sys_Data_Tree_Wid.cpp
    source_code/Sys_Output_Text_Wid.cpp
    source_code/Sys_Dock_Widget.cpp
    source_code/Sys_Version_Update.cpp
    source_code/main.cpp
)

set(QRC_FILES
    ${CMAKE_SOURCE_DIR}/source_code/system_sys/source_code/gui/resources/system_sys_resource.qrc
    ${CMAKE_SOURCE_DIR}/source_code/system_sys/source_code/gui/resources/hydroxygen.qrc
)

set(RCFiles
    promaides.rc
)

set(PrecompiledHeaders
    source_code/Main_Headers_Precompiled.h
)

#Add QWT_DLL definition to entire solution
add_definitions(-DQWT_DLL)

##
## Turn on automatic processing of files by QT
##
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

##
## Add the QT required libraries (Added later in system_sys)
##
find_package(Qt5 COMPONENTS Core Sql Svg PrintSupport Widgets Gui Xml REQUIRED)

## Create the ProMaIDeS Executable  
add_executable(ProMaIDeS
    ${UI_FILES}
    ${H_FILES}
    ${CPP_FILES}
    ${QRC_FILES}
    ${RCFiles}
    ${PrecompiledHeaders}
)
##Change executable name
set_target_properties(ProMaIDeS PROPERTIES OUTPUT_NAME ProMaIDeS_vc)

## Adding Precompiled Headers
target_precompile_headers(ProMaIDeS PRIVATE ${PrecompiledHeaders})

## Adding projects
add_subdirectory(source_code/system_alt/)
add_subdirectory(source_code/system_cost/)
add_subdirectory(source_code/system_dam/)
add_subdirectory(source_code/system_fpl/)
add_subdirectory(source_code/system_hydraulic/)
add_subdirectory(source_code/system_hydraulic/source_code/solver/)
add_subdirectory(source_code/system_madm/)
add_subdirectory(source_code/system_risk/)
add_subdirectory(source_code/system_sys/)

## Adding Include Directories (The rest of includes is inherited from children)
target_include_directories(ProMaIDeS
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        .
        source_code
        source_code/gui
)

## Adding Required Libraries
## QGIS 
target_link_libraries(ProMaIDeS
    system_alt
    system_cost
    system_dam
    system_fpl
    system_hydraulic
    solver
    system_madm
    system_risk
    system_sys
)

###
### Yes, AUTOUIC is on, but for some reason we still need to do this
###
qt5_wrap_ui(UI_FILES ${UI_FILES})


######## POST BUILD STUFF ########
## Copying dlls
add_custom_command(TARGET ProMaIDeS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/source_code/Vendors/RequiredDLLs/$<CONFIGURATION>"
        $<TARGET_FILE_DIR:ProMaIDeS>)