##
## List of files to work with (We can use glob, but better to keep it safe)
##
set(UI_FILES
	source_code/gui/RiskGui_Result_Cumulated_Wid.ui
	source_code/gui/RiskGui_Risk_Scenario_Dia.ui
	source_code/gui/RiskGui_Break_Height_Dia.ui
	source_code/gui/RiskGui_Catchment_Scenario_Dia.ui
	source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.ui
	source_code/gui/RiskGui_Result_Wid.ui
	source_code/gui/RiskGui_Detailed_Result_Wid.ui
	source_code/gui/RiskGui_Calculate_All_Dia.ui
	source_code/gui/RiskGui_Optimisation_Prediction_Dia.ui
	source_code/gui/RiskGui_System_Member_Wid.ui
)

set(H_FILES
	source_code/Risk_System.h
	source_code/Risk_Headers_Precompiled.h
	source_code/Risk_Break_Scenario_Management.h
	source_code/Risk_Break_Scenario.h
	source_code/Risk_Hydraulic_Break_Calculation.h
	source_code/Risk_Fpl_Stress.h
	source_code/Risk_Dam_Results.h
	source_code/Risk_Damage_Predictor.h
	source_code/Risk_Catchment_Scenario_Tree.h
	source_code/Risk_Catchment_Break_Scenario_List.h
	source_code/gui/RiskGui_Result_Reliability_Dia.h
	source_code/gui/RiskGui_Result_Cumulated_Wid.h
	source_code/gui/RiskGui_Risk_Scenario_Dia.h
	source_code/gui/RiskGui_Break_Height_Dia.h
	source_code/gui/RiskGui_Catchment_Scenario_Dia.h
	source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.h
	source_code/gui/RiskGui_Result_Wid.h
	source_code/gui/RiskGui_Catchment_Scenario_Statistic_Wid.h
	source_code/gui/RiskGui_Detailed_Result_Wid.h
	source_code/gui/RiskGui_Detailed_Risk_Summary_Wid.h
	source_code/gui/RiskGui_Result_Scenario_Wid.h
	source_code/gui/RiskGui_Calculate_All_Dia.h
	source_code/gui/RiskGui_Probabilty_Fpl_Section_Wid.h
	source_code/gui/RiskGui_Optimisation_Prediction_Dia.h
	source_code/gui/RiskGui_System_Member_Wid.h
)

set(CPP_FILES
	source_code/Risk_System.cpp
	source_code/Risk_Break_Scenario_Management.cpp
	source_code/Risk_Break_Scenario.cpp
	source_code/Risk_Hydraulic_Break_Calculation.cpp
	source_code/Risk_Damage_Predictor.cpp             
	source_code/Risk_Catchment_Scenario_Tree.cpp     
	source_code/Risk_Catchment_Break_Scenario_List.cpp 
	source_code/Risk_Fpl_Stress.cpp
	source_code/Risk_Dam_Results.cpp
	source_code/gui/RiskGui_Result_Reliability_Dia.cpp
	source_code/gui/RiskGui_Result_Cumulated_Wid.cpp
	source_code/gui/RiskGui_Risk_Scenario_Dia.cpp
	source_code/gui/RiskGui_Break_Height_Dia.cpp
	source_code/gui/RiskGui_Catchment_Scenario_Dia.cpp
	source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.cpp 
	source_code/gui/RiskGui_Result_Wid.cpp
	source_code/gui/RiskGui_Catchment_Scenario_Statistic_Wid.cpp
	source_code/gui/RiskGui_Detailed_Result_Wid.cpp
	source_code/gui/RiskGui_Detailed_Risk_Summary_Wid.cpp
	source_code/gui/RiskGui_Result_Scenario_Wid.cpp
	source_code/gui/RiskGui_Calculate_All_Dia.cpp
	source_code/gui/RiskGui_Probabilty_Fpl_Section_Wid.cpp
	source_code/gui/RiskGui_Optimisation_Prediction_Dia.cpp
	source_code/gui/RiskGui_System_Member_Wid.cpp
)

set(QRC_FILES
)

set(PrecompiledHeaders
	source_code/Risk_Headers_Precompiled.h
)

##
## Create the Library by passing its files
##
add_library(system_risk
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${QRC_FILES}
	${PrecompiledHeaders}
)

## Adding Precompiled Headers
target_precompile_headers(system_risk PRIVATE ${PrecompiledHeaders})

## Adding Include Directories (parent libraries will also inherit)
target_include_directories(system_risk
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    	.
    	source_code
		source_code/gui
)


## Adding Required Libraries
target_link_libraries(system_risk system_hydraulic system_dam system_fpl system_sys)

###
### Yes, AUTOUIC is on, but for some reason we still need to do this
###
qt5_wrap_ui(UI_FILES ${UI_FILES})