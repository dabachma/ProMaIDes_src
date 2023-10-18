##
## List of files to work with (We can use glob, but better to keep it safe)
##
set(UI_FILES
	source_code/gui/CostGui_Alternative_Ecn_Cost_Dia.ui
)

set(H_FILES
	source_code/Cost_System.h
	source_code/Cost_Headers_Precompiled.h
	source_code/gui/CostGui_Alternative_Ecn_Cost_Dia.h
	source_code/Cost_Ecn_Cost_System.h
)

set(CPP_FILES
	source_code/Cost_System.cpp
	source_code/gui/CostGui_Alternative_Ecn_Cost_Dia.cpp
	source_code/Cost_Ecn_Cost_System.cpp
)

set(QRC_FILES
)

set(PrecompiledHeaders
	source_code/Cost_Headers_Precompiled.h
)

##
## Create the Library by passing its files
##
add_library(system_cost
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${QRC_FILES}
	${PrecompiledHeaders}
)

## Creating a custom target for libraries that depend on headers from this
qt5_wrap_ui(UI_FILES ${UI_FILES})
add_custom_target(system_cost_generate_ui_headers DEPENDS ${UI_FILES})
set_target_properties(system_cost_generate_ui_headers PROPERTIES FOLDER CMakePredefinedTargets)

## Needs headers from ALT Module
add_dependencies(system_cost system_alt_generate_ui_headers)

## Adding Precompiled Headers
target_precompile_headers(system_cost PRIVATE ${PrecompiledHeaders})

## Adding Include Directories (parent libraries will also inherit)
target_include_directories(system_cost
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    	.
		source_code
		source_code/gui
		${CMAKE_SOURCE_DIR}/source_code/system_alt
		${CMAKE_SOURCE_DIR}/source_code/system_alt/source_code
		${CMAKE_SOURCE_DIR}/source_code/system_alt/source_code/gui
		${CMAKE_BINARY_DIR}/source_code/system_alt
)

## Adding Required Libraries
target_link_libraries(system_cost system_hydraulic system_dam system_fpl system_risk system_sys)

###
### Yes, AUTOUIC is on, but for some reason we still need to do this
###
### We already wraped them up
