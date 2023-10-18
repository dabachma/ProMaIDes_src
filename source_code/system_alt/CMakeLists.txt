##
## List of files to work with (We can use glob, but better to keep it safe)
##
set(UI_FILES
	source_code/gui/AltGui_Alternative_Info_Dia.ui
	source_code/gui/AltGui_Alternative_Type_Dia.ui
	source_code/gui/AltGui_Measure_Replacing_File_Dia.ui
	source_code/gui/AltGui_File_Replacing_Wid.ui
	source_code/gui/AltGui_Alternative_Base_On_Dia.ui
	source_code/gui/AltGui_Dam_Resettlement_Dia.ui
	source_code/gui/AltGui_Dam_Change_Function_Dia.ui
	source_code/gui/AltGui_Id_Replacing_Wid.ui
	source_code/gui/AltGui_Measure_Dia.ui
	source_code/gui/AltGui_Alternative_Info_Wid.ui
)

set(H_FILES
	source_code/Alt_System.h
	source_code/Alt_Headers_Precompiled.h
	source_code/gui/AltGui_Alternative_Info_Dia.h
	source_code/gui/AltGui_Alternative_Type_Dia.h
	source_code/gui/AltGui_Measure_Replacing_File_Dia.h
	source_code/gui/AltGui_File_Replacing_Wid.h
	source_code/gui/AltGui_Alternative_Base_On_Dia.h
	source_code/gui/AltGui_Dam_Change_Function_Dia.h
	source_code/gui/AltGui_Dam_Resettlement_Dia.h
	source_code/gui/AltGui_Id_Replacing_Wid.h
	source_code/gui/AltGui_Measure_Dia.h
	source_code/gui/AltGui_Alternative_Info_Wid.h
)

set(CPP_FILES
	source_code/Alt_System.cpp
	source_code/gui/AltGui_Alternative_Info_Dia.cpp
	source_code/gui/AltGui_Alternative_Type_Dia.cpp
	source_code/gui/AltGui_Measure_Replacing_File_Dia.cpp
	source_code/gui/AltGui_File_Replacing_Wid.cpp
	source_code/gui/AltGui_Alternative_Base_On_Dia.cpp
	source_code/gui/AltGui_Dam_Change_Function_Dia.cpp
	source_code/gui/AltGui_Dam_Resettlement_Dia.cpp
	source_code/gui/AltGui_Id_Replacing_Wid.cpp
	source_code/gui/AltGui_Measure_Dia.cpp
	source_code/gui/AltGui_Alternative_Info_Wid.cpp
)

set(QRC_FILES
)

set(PrecompiledHeaders
	source_code/Alt_Headers_Precompiled.h
)

##
## Create the Library by passing its files
##
add_library(system_alt
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${QRC_FILES}
	${PrecompiledHeaders}
)

## Creating a custom target for libraries that depend on headers from this
qt5_wrap_ui(UI_FILES ${UI_FILES})
add_custom_target(system_alt_generate_ui_headers DEPENDS ${UI_FILES})
set_target_properties(system_alt_generate_ui_headers PROPERTIES FOLDER CMakePredefinedTargets)

## Needs headers from COST Module
add_dependencies(system_alt system_cost_generate_ui_headers)

## Adding Precompiled Headers
target_precompile_headers(system_alt PRIVATE ${PrecompiledHeaders})

## Adding Include Directories (parent libraries will also inherit)
target_include_directories(system_alt
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    	.
		source_code
		source_code/gui
        ${CMAKE_BINARY_DIR}/source_code/system_fpl
        ${CMAKE_BINARY_DIR}/source_code/system_dam
        ${CMAKE_BINARY_DIR}/source_code/system_risk
        ${CMAKE_BINARY_DIR}/source_code/system_madm
        ${CMAKE_BINARY_DIR}/source_code/system_cost
)

## Adding Required Libraries
target_link_libraries(system_alt system_hydraulic system_fpl system_dam system_risk system_cost system_madm  system_sys)

###
### Yes, AUTOUIC is on, but for some reason we still need to do this
###
### We already wraped them up