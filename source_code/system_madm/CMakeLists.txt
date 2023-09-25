##
## List of files to work with (We can use glob, but better to keep it safe)
##
set(UI_FILES
	source_code/gui/MadmGui_Weighting_Set_Dia.ui
	source_code/gui/MadmGui_Rank_Wid.ui
	source_code/gui/MadmGui_Weight_Sets_Wid.ui
	source_code/gui/MadmGui_Total_Rank_Wid.ui 
)

set(H_FILES
	source_code/Madm_System.h
	source_code/Madm_Headers_Precompiled.h
	source_code/Madm_Analysis.h
	source_code/Madm_Solver_Saw.h
	source_code/Madm_Solver_Topsis.h
	source_code/Madm_Solver_Electre.h
	source_code/Madm_Decision_Matrix.h
	source_code/_Madm_Solver.h
	source_code/gui/MadmGui_Weighting_Set_Dia.h
	source_code/gui/MadmGui_Rank_Wid.h
	source_code/gui/MadmGui_Total_Rank_Wid.h
	source_code/gui/MadmGui_Weight_Sets_Wid.h
	source_code/gui/MadmGui_Matrix_Summary_Wid.h 
)

set(CPP_FILES 				
	source_code/Madm_System.cpp
	source_code/Madm_Analysis.cpp
	source_code/Madm_Solver_Saw.cpp
	source_code/Madm_Solver_Topsis.cpp
	source_code/Madm_Solver_Electre.cpp
	source_code/Madm_Decision_Matrix.cpp
	source_code/_Madm_Solver.cpp
	source_code/gui/MadmGui_Weighting_Set_Dia.cpp
	source_code/gui/MadmGui_Weight_Sets_Wid.cpp
	source_code/gui/MadmGui_Rank_Wid.cpp
	source_code/gui/MadmGui_Total_Rank_Wid.cpp
	source_code/gui/MadmGui_Matrix_Summary_Wid.cpp
)

set(QRC_FILES
)

set(PrecompiledHeaders
	source_code/Madm_Headers_Precompiled.h
)

##
## Create the Library by passing its files
##
add_library(system_madm
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${QRC_FILES}
	${PrecompiledHeaders}
)

## Needs headers from Cost Module
add_dependencies(system_madm system_cost_generate_ui_headers)

## Adding Precompiled Headers
target_precompile_headers(system_madm PRIVATE ${PrecompiledHeaders})

## Adding Include Directories (parent libraries will also inherit)
## MADM needs headers from ALT and COST
## MADM needs headers (moc) from ALT and COST, thus the need for Binary Include
target_include_directories(system_madm
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    	.
	    source_code
	    source_code/gui
		${CMAKE_SOURCE_DIR}/source_code/system_cost/
		${CMAKE_SOURCE_DIR}/source_code/system_cost/source_code
		${CMAKE_SOURCE_DIR}/source_code/system_cost/source_code/gui

		${CMAKE_SOURCE_DIR}/source_code/system_alt/
		${CMAKE_SOURCE_DIR}/source_code/system_alt/source_code/
		${CMAKE_SOURCE_DIR}/source_code/system_alt/source_code/gui
		
		${CMAKE_BINARY_DIR}/source_code/system_alt/
		${CMAKE_BINARY_DIR}/source_code/system_cost/
		${CMAKE_BINARY_DIR}/source_code/system_risk
)

## Adding Required Libraries
target_link_libraries(system_madm system_hydraulic system_dam system_fpl system_risk system_sys)



###
### Yes, AUTOUIC is on, but for some reason we still need to do this
###
qt5_wrap_ui(UI_FILES ${UI_FILES})