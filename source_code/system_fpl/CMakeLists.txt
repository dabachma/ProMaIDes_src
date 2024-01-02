# - Set allows giving a value to a variable similar to: UI_FILES = "file1.ui file2.ui file3.ui"
# - A list of .ui Files is made to make it easier to work with them
# - Glob can also be used but this manual way is safer and also the recommened approach for faster in between builds
# - The UI files need to be wrapped to generate the ui-xxxx.h files this is done using qt_wrap_ui
set(UI_FILES
    source_code/gui/FplGui_Section_Dia.ui
    source_code/gui/FplGui_Random_Check_Dia.ui
    source_code/gui/FplGui_Check_Disttype_Triangle_Dia.ui
    source_code/gui/FplGui_Check_Disttype_Mean_Dia.ui
    source_code/gui/FplGui_Prob_Result_Wid.ui
    source_code/gui/FplGui_Detailed_Result_Wid.ui
    source_code/gui/FplGui_Frc_Values_Dia.ui
    source_code/gui/FplGui_System_Member_Wid.ui
    source_code/gui/FplGui_Faulttree_Dike_Wid.ui
    source_code/gui/FplGui_Faulttree_Dune_Wid.ui
    source_code/gui/FplGui_Faulttree_Gate_Wid.ui 	
)

# - Set allows giving a value to a variable similar to: H_FILES = "file1.h file2.h file3.h"
# - A list of .h Files is made to make it easier to work with them
# - All the h files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files are parts of it
set(H_FILES
    source_code/Fpl_Calculation.h
    source_code/Fpl_Headers_Precompiled.h
    source_code/calculation/performance/Fpl_Frc_Sim.h
    source_code/calculation/performance/Fpl_Mc_Sim.h
    source_code/calculation/random/_Fpl_Distribution_Class.h
    source_code/calculation/random/Fpl_Distribution_Class_Discret.h
    source_code/calculation/random/Fpl_Distribution_Class_Discret_Mean.h
    source_code/calculation/random/Fpl_Distribution_Class_Mean.h
    source_code/calculation/random/Fpl_Distribution_Class_Mean_Mean.h
    source_code/calculation/random/Fpl_Distribution_Class_Triangle.h
    source_code/calculation/random/Fpl_Random_Variables.h
    source_code/calculation/random/_Fpl_Random_Generator.h
    source_code/calculation/random/Fpl_Random_Generator_Ranmar.h
    source_code/calculation/random/Fpl_Random_Generator_Zikurat.h
    source_code/calculation/random/Fpl_Random_Generator_MofAll.h
    source_code/calculation/random/Fpl_Random_Generator_Sfmt.h
    source_code/calculation/random/Fpl_Check_Distribution.h
    source_code/calculation/section_fpl/_Fpl_Section_Type.h
    source_code/calculation/section_fpl/Fpl_Sec_Type_Dike.h
    source_code/calculation/section_fpl/Fpl_Sec_Type_Highground.h
    source_code/calculation/section_fpl/Fpl_Sec_Type_Dune.h
    source_code/calculation/section_fpl/Fpl_Sec_Type_Gate.h
    source_code/calculation/section_fpl/Fpl_Section.h
    source_code/calculation/section_fpl/Fpl_Section_Points.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Waterside.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Waterside.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Crest.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Landside.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Landside.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone_Connector.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table_Point.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Var_Materialzone.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Hinterland.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Foreland.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Segment.h
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Waterside_Materialzone.h
    source_code/calculation/section_fpl/mechanisms/_Fpl_Failure_Mechanism.h
    source_code/calculation/section_fpl/mechanisms/_Fpl_Mechanism.h
    source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point_List.h
    source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Overflow_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waverunup_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Bursting_Din_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Lane_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Sell_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Schmertmann_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Bursting_Slice.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Seepage_Calculator_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Wave_Impact_Waterside_Dike.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion.h
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion_Start.h
    source_code/calculation/section_fpl/mechanisms/highground/Fpl_Fail_Overflow_HiGround.h
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Local_Wind_Fetch.h
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Mech_Wave_Generation.h
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Waveparam_Calculation.h
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Wind_Fetch.h
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Mech_Slope_Stability_Dike.h
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slip_Circle.h
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_Slices.h
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_External_Force.h
    source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Erosion_Dune_van_Gent.h
    source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Waverunup_Dune.h
    source_code/calculation/section_fpl/mechanisms/landside_resistance/Fpl_Mech_Landside_Erosion.h
    source_code/calculation/section_fpl/Fpl_Frc_Curve.h
    source_code/gui/FplGui_Section_Dia.h
    source_code/gui/FplGui_Random_Check_Dia.h
    source_code/gui/FplGui_Check_Disttype_Triangle_Dia.h
    source_code/gui/FplGui_Check_Disttype_Mean_Dia.h
    source_code/gui/FplGui_Prob_Result_Wid.h
    source_code/gui/FplGui_Detailed_Result_Wid.h
    source_code/gui/FplGui_Frc_Values_Dia.h
    source_code/gui/FplGui_System_Member_Wid.h
    source_code/gui/FplGui_Faulttree_Dike_Wid.h
    source_code/gui/FplGui_Faulttree_Dune_Wid.h
    source_code/gui/FplGui_Faulttree_Gate_Wid.h
)

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of h Files is made to make it easier to work with them
# - All the .cpp files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files to compile
set(CPP_FILES			
    source_code/Fpl_Calculation.cpp
    source_code/calculation/performance/Fpl_Frc_Sim.cpp
    source_code/calculation/performance/Fpl_Mc_Sim.cpp
    source_code/calculation/random/_Fpl_Distribution_Class.cpp
    source_code/calculation/random/Fpl_Distribution_Class_Discret.cpp
    source_code/calculation/random/Fpl_Distribution_Class_Discret_Mean.cpp
    source_code/calculation/random/Fpl_Distribution_Class_Mean.cpp
    source_code/calculation/random/Fpl_Distribution_Class_Mean_Mean.cpp
    source_code/calculation/random/Fpl_Distribution_Class_Triangle.cpp
    source_code/calculation/random/Fpl_Random_Variables.cpp
    source_code/calculation/random/_Fpl_Random_Generator.cpp
    source_code/calculation/random/Fpl_Random_Generator_Ranmar.cpp
    source_code/calculation/random/Fpl_Random_Generator_Zikurat.cpp
    source_code/calculation/random/Fpl_Random_Generator_MofAll.cpp
    source_code/calculation/random/Fpl_Random_Generator_Sfmt.cpp
    source_code/calculation/random/Fpl_Check_Distribution.cpp
    source_code/calculation/section_fpl/_Fpl_Section_Type.cpp
    source_code/calculation/section_fpl/Fpl_Sec_Type_Dike.cpp
    source_code/calculation/section_fpl/Fpl_Sec_Type_Highground.cpp
    source_code/calculation/section_fpl/Fpl_Sec_Type_Dune.cpp
    source_code/calculation/section_fpl/Fpl_Sec_Type_Gate.cpp
    source_code/calculation/section_fpl/Fpl_Section.cpp
    source_code/calculation/section_fpl/Fpl_Section_Points.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Waterside.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Waterside.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Crest.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Landside.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Landside.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone_Connector.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table_Point.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Var_Materialzone.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Hinterland.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Foreland.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Segment.cpp
    source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Waterside_Materialzone.cpp
    source_code/calculation/section_fpl/mechanisms/_Fpl_Failure_Mechanism.cpp
    source_code/calculation/section_fpl/mechanisms/_Fpl_Mechanism.cpp
    source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point_List.cpp
    source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Overflow_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waverunup_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Bursting_Din_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Lane_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Sell_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Schmertmann_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Bursting_Slice.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Seepage_Calculator_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Wave_Impact_Waterside_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion.cpp
    source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion_Start.cpp
    source_code/calculation/section_fpl/mechanisms/highground/Fpl_Fail_Overflow_HiGround.cpp
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Local_Wind_Fetch.cpp
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Mech_Wave_Generation.cpp
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Waveparam_Calculation.cpp
    source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Wind_Fetch.cpp
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Mech_Slope_Stability_Dike.cpp
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slip_Circle.cpp
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_Slices.cpp
    source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_External_Force.cpp
    source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Erosion_Dune_van_Gent.cpp
    source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Waverunup_Dune.cpp
    source_code/calculation/section_fpl/mechanisms/landside_resistance/Fpl_Mech_Landside_Erosion.cpp
    source_code/calculation/section_fpl/Fpl_Frc_Curve.cpp
    source_code/gui/FplGui_Section_Dia.cpp
    source_code/gui/FplGui_Random_Check_Dia.cpp
    source_code/gui/FplGui_Check_Disttype_Triangle_Dia.cpp
    source_code/gui/FplGui_Check_Disttype_Mean_Dia.cpp
    source_code/gui/FplGui_Detailed_Result_Wid.cpp
    source_code/gui/FplGui_Prob_Result_Wid.cpp
    source_code/gui/FplGui_Frc_Values_Dia.cpp
    source_code/gui/FplGui_System_Member_Wid.cpp
    source_code/gui/FplGui_Faulttree_Dike_Wid.cpp
    source_code/gui/FplGui_Faulttree_Dune_Wid.cpp
    source_code/gui/FplGui_Faulttree_Gate_Wid.cpp
)

# - Set allows giving a value to a variable similar to: PrecompiledHeaders = "PrecompiledHeaders.h"
# - Set is used here for consistency of the code
# - The precompiledHeader will be targeted by the library so that it precomiples this header file for faster build times
set(PrecompiledHeaders
    source_code/Fpl_Headers_Precompiled.h
)

# - add_library create the library
# - The add_library is the engine here and is defined as add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] [<source>...])
# - system_fpl is the name given to the library
# - What follows are the source codes
add_library(system_fpl
    ${UI_FILES}
    ${H_FILES}
    ${CPP_FILES}
    ${QRC_FILES}
    ${PrecompiledHeaders}
)

# - target_precompile_headers sets the precompiled headers of the target (library)
# - The precompiled headers is set to PUBLIC so that other libraries in the solution can also use this precompiled header
target_precompile_headers(system_fpl PUBLIC ${PrecompiledHeaders})

# - target_include_directories sets the include directories for a specfic target
# - It is set to PUBLIC meaning the parent targerts also inherit these directories
# - For example: the HYD Library is a parent of solver and has access to things like: include "cvode.h"
# - ${CMAKE_CURRENT_BINARY_DIR} because it is the location of the ui_xxxx.h files that are generated by qt_wrap_ui
target_include_directories(system_fpl
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        source_code 
        source_code/calculation 
        source_code/gui 
        source_code/calculation/performance 
        source_code/calculation/random 
        source_code/calculation/section_fpl 
        source_code/calculation/section_fpl/geometrie_dike 
        source_code/calculation/section_fpl/mechanisms 
        source_code/calculation/section_fpl/mechanisms/dike 
        source_code/calculation/section_fpl/mechanisms/highground 
        source_code/calculation/section_fpl/mechanisms/wave_generation 
        source_code/calculation/section_fpl/mechanisms/slope_stability 
        source_code/calculation/section_fpl/mechanisms/landside_resistance 
        source_code/calculation/section_fpl/mechanisms/dune
        source_code/calculation/section_fpl/mechanisms/gate
)

# - target_link_libraries links libraries to a specfic target
# - The target_include_directories tells the program where the headers are. But not what the classes do
# - The dlls or libs are what include the functions themselves.
# - system_fpl only depends on system_risk, the rest are for clarity
target_link_libraries(system_fpl system_hydraulic system_sys)

# - qt_wrap_ui (or qt5_wrap_ui) wraps ui files, in other words generating .h files from them
# - AUTOMOC can also be used which does this automatically. This, however, is not recommened, it brings more troubles than solutions  
qt_wrap_ui(UI_FILES ${UI_FILES})

