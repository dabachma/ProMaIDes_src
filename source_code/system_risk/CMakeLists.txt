# - Set allows giving a value to a variable similar to: UI_FILES = "file1.ui file2.ui file3.ui"
# - A list of .ui Files is made to make it easier to work with them
# - Glob can also be used but this manual way is safer and also the recommened approach for faster in between builds
# - The UI files need to be wrapped to generate the ui-xxxx.h files this is done using qt_wrap_ui
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

# - Set allows giving a value to a variable similar to: H_FILES = "file1.h file2.h file3.h"
# - A list of .h Files is made to make it easier to work with them
# - All the h files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files are parts of it
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

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of h Files is made to make it easier to work with them
# - All the .cpp files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files to compile
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

# - Set allows giving a value to a variable similar to: PrecompiledHeaders = "PrecompiledHeaders.h"
# - Set is used here for consistency of the code
# - The precompiledHeader will be targeted by the library so that it precomiples this header file for faster build times
set(PrecompiledHeaders
	source_code/Risk_Headers_Precompiled.h
)

# - add_library create the library
# - The add_library is the engine here and is defined as add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] [<source>...])
# - system_risk is the name given to the library
# - What follows are the source codes
add_library(system_risk
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${PrecompiledHeaders}
)

# - target_precompile_headers sets the precompiled headers of the target (library)
# - The precompiled headers is set to PUBLIC so that other libraries in the solution can also use this precompiled header
target_precompile_headers(system_risk PUBLIC ${PrecompiledHeaders})

# - target_include_directories sets the include directories for a specfic target
# - It is set to PUBLIC meaning the parent targerts also inherit these directories
# - For example: the HYD Library is a parent of solver and has access to things like: include "cvode.h"
# - ${CMAKE_CURRENT_BINARY_DIR} because it is the location of the ui_xxxx.h files that are generated by qt_wrap_ui
target_include_directories(system_risk
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
    	source_code
		source_code/gui
)

# - target_link_libraries links libraries to a specfic target
# - The target_include_directories tells the program where the headers are. But not what the classes do
# - The dlls or libs are what include the functions themselves.
# - system_risk only depends on system_dam system_fpl, the rest are for clarity
target_link_libraries(system_risk system_dam system_fpl system_hydraulic system_sys)

# - qt_wrap_ui (or qt5_wrap_ui) wraps ui files, in other words generating .h files from them
# - AUTOMOC can also be used which does this automatically. This, however, is not recommened, it brings more troubles than solutions  
qt5_wrap_ui(UI_FILES ${UI_FILES})