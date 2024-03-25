# - Set allows giving a value to a variable similar to: UI_FILES = "file1.ui file2.ui file3.ui"
# - A list of .ui Files is made to make it easier to work with them
# - Glob can also be used but this manual way is safer and also the recommened approach for faster in between builds
# - The UI files need to be wrapped to generate the ui-xxxx.h files this is done using qt_wrap_ui
set(UI_FILES				        
	source_code/gui/DamGui_Result_Wid.ui
	source_code/gui/DamGui_Number_Raster_Dia.ui
	source_code/gui/DamGui_Ecn_Func_Wid.ui
	source_code/gui/DamGui_Eco_Btype_Func_Wid.ui
	source_code/gui/DamGui_Eco_Btype_Wid.ui
	source_code/gui/DamGui_Eco_Soil_Cost_Wid.ui
	source_code/gui/DamGui_Eco_Soil_Erosion_Wid.ui
	source_code/gui/DamGui_Pop_Function_Wid.ui
	source_code/gui/DamGui_Pys_Categories_Wid.ui
	source_code/gui/DamGui_Raster_Wid.ui
	source_code/gui/DamGui_Raster_Dia.ui
	source_code/gui/DamGui_Sc_Point_Wid.ui
	source_code/gui/DamGui_Ci_Point_Wid.ui
	source_code/gui/DamGui_Ci_Polygon_Wid.ui
)
	
# - Set allows giving a value to a variable similar to: H_FILES = "file1.h file2.h file3.h"
# - A list of .h Files is made to make it easier to work with them
# - All the h files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files are parts of it
set(H_FILES
	source_code/Dam_Damage_System.h
	source_code/Dam_Headers_Precompiled.h
	source_code/general/Dam_Hyd_Result_Raster.h
	source_code/general/Dam_Hydimport_Element.h
	source_code/economic/Dam_Ecn_Damage_Function.h
	source_code/economic/Dam_Ecn_System.h
	source_code/economic/Dam_Ecn_Element.h
	source_code/economic/Dam_Ecn_Raster.h
	source_code/general/Dam_Impact_Value_Floodplain.h 
	source_code/general/Dam_Impact_Values.h 
	source_code/general/Dam_Raster.h
	source_code/general/_Dam_Element.h
	source_code/general/_Dam_2value_Function.h
	source_code/general/_Dam_System.h
	source_code/general/Dam_Polygon.h
	source_code/ecologic/Dam_Eco_System.h
	source_code/ecologic/Dam_Eco_Btype_Damage_Function.h
	source_code/ecologic/Dam_Eco_Soil_Cost_Function.h
	source_code/ecologic/Dam_Eco_Soil_Erosion_Function.h
	source_code/ecologic/Dam_Eco_Btype_Element.h
	source_code/ecologic/Dam_Eco_Soil_Element.h
	source_code/ecologic/Dam_Eco_Btype_Raster.h
	source_code/ecologic/Dam_Eco_Soil_Raster.h
	source_code/ecologic/Dam_Eco_Btype.h
	source_code/people/Dam_People_System.h
	source_code/people/Dam_People_Raster.h
	source_code/people/Dam_People_Element.h
	source_code/people/Dam_People_Damage_Function.h
	source_code/simple_count/Dam_Sc_System.h
	source_code/simple_count/Dam_Sc_Point_Manager.h
	source_code/simple_count/Dam_Sc_Subcategory.h
	source_code/simple_count/Dam_Sc_Point.h
	source_code/CI/_Dam_CI_Element.h
	source_code/CI/Dam_CI_Point.h
	source_code/CI/Dam_CI_Polygon.h
	source_code/CI/Dam_CI_System.h
	source_code/CI/Dam_CI_Element_List.h
	source_code/psycho_social/Dam_Pys_System.h
	source_code/psycho_social/Dam_Pys_Raster.h
	source_code/psycho_social/Dam_Pys_Function.h
	source_code/psycho_social/Dam_Pys_Element.h
	source_code/psycho_social/Dam_Pys_Density_Function.h
	source_code/psycho_social/Dam_Pys_Category.h
	source_code/gui/DamGui_Result_Summary_Wid.h
	source_code/gui/DamGui_Result_Wid.h
	source_code/gui/DamGui_Number_Raster_Dia.h
	source_code/gui/DamGui_Ecn_Func_Wid.h
	source_code/gui/DamGui_Eco_Btype_Func_Wid.h
	source_code/gui/DamGui_Eco_Btype_Wid.h
	source_code/gui/DamGui_Eco_Soil_Cost_Wid.h
	source_code/gui/DamGui_Eco_Soil_Erosion_Wid.h
	source_code/gui/DamGui_Pop_Function_Wid.h
	source_code/gui/DamGui_Pys_Categories_Wid.h
	source_code/gui/DamGui_Raster_Wid.h
	source_code/gui/DamGui_Raster_Dia.h
	source_code/gui/DamGui_Sc_Point_Wid.h
	source_code/gui/DamGui_Sc_Subcategory_Wid.h
	source_code/gui/DamGui_Ci_Point_Wid.h
	source_code/gui/DamGui_Ci_Polygon_Wid.h
)

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of h Files is made to make it easier to work with them
# - All the .cpp files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files to compile
set(CPP_FILES            
	source_code/Dam_Damage_System.cpp
	source_code/general/Dam_Hyd_Result_Raster.cpp
	source_code/general/Dam_Hydimport_Element.cpp
	source_code/economic/Dam_Ecn_Damage_Function.cpp
	source_code/economic/Dam_Ecn_System.cpp
	source_code/economic/Dam_Ecn_Element.cpp
	source_code/economic/Dam_Ecn_Raster.cpp
	source_code/general/Dam_Impact_Value_Floodplain.cpp 
	source_code/general/Dam_Impact_Values.cpp 
	source_code/general/Dam_Raster.cpp
	source_code/general/_Dam_Element.cpp
	source_code/general/_Dam_2value_Function.cpp
	source_code/general/_Dam_System.cpp
	source_code/general/Dam_Polygon.cpp
	source_code/ecologic/Dam_Eco_System.cpp
	source_code/ecologic/Dam_Eco_Btype_Damage_Function.cpp
	source_code/ecologic/Dam_Eco_Soil_Cost_Function.cpp
	source_code/ecologic/Dam_Eco_Soil_Erosion_Function.cpp
	source_code/ecologic/Dam_Eco_Btype_Element.cpp
	source_code/ecologic/Dam_Eco_Soil_Element.cpp
	source_code/ecologic/Dam_Eco_Btype_Raster.cpp
	source_code/ecologic/Dam_Eco_Soil_Raster.cpp
	source_code/ecologic/Dam_Eco_Btype.cpp
	source_code/people/Dam_People_System.cpp
	source_code/people/Dam_People_Raster.cpp
	source_code/people/Dam_People_Element.cpp
	source_code/people/Dam_People_Damage_Function.cpp
	source_code/simple_count/Dam_Sc_System.cpp
	source_code/simple_count/Dam_Sc_Point_Manager.cpp
	source_code/simple_count/Dam_Sc_Subcategory.cpp
	source_code/simple_count/Dam_Sc_Point.cpp
	source_code/CI/_Dam_CI_Element.cpp
	source_code/CI/Dam_CI_Point.cpp
	source_code/CI/Dam_CI_Polygon.cpp
	source_code/CI/Dam_CI_System.cpp
	source_code/CI/Dam_CI_Element_List.cpp
	source_code/psycho_social/Dam_Pys_System.cpp
	source_code/psycho_social/Dam_Pys_Raster.cpp
	source_code/psycho_social/Dam_Pys_Function.cpp
	source_code/psycho_social/Dam_Pys_Element.cpp
	source_code/psycho_social/Dam_Pys_Density_Function.cpp
	source_code/psycho_social/Dam_Pys_Category.cpp
	source_code/gui/DamGui_Result_Summary_Wid.cpp 
	source_code/gui/DamGui_Result_Wid.cpp
	source_code/gui/DamGui_Number_Raster_Dia.cpp
	source_code/gui/DamGui_Ecn_Func_Wid.cpp
	source_code/gui/DamGui_Eco_Btype_Func_Wid.cpp
	source_code/gui/DamGui_Eco_Btype_Wid.cpp
	source_code/gui/DamGui_Eco_Soil_Cost_Wid.cpp
	source_code/gui/DamGui_Eco_Soil_Erosion_Wid.cpp
	source_code/gui/DamGui_Pop_Function_Wid.cpp
	source_code/gui/DamGui_Pys_Categories_Wid.cpp
	source_code/gui/DamGui_Raster_Wid.cpp
	source_code/gui/DamGui_Raster_Dia.cpp
	source_code/gui/DamGui_Sc_Point_Wid.cpp
	source_code/gui/DamGui_Ci_Point_Wid.cpp
	source_code/gui/DamGui_Ci_Polygon_Wid.cpp
	source_code/gui/DamGui_Sc_Subcategory_Wid.cpp
)

# - Set allows giving a value to a variable similar to: PrecompiledHeaders = "PrecompiledHeaders.h"
# - Set is used here for consistency of the code
# - The precompiledHeader will be targeted by the library so that it precomiples this header file for faster build times
set(PrecompiledHeaders
	source_code/Dam_Headers_Precompiled.h
)

# - add_library create the library
# - The add_library is the engine here and is defined as add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] [<source>...])
# - system_dam is the name given to the library
# - What follows are the source codes
add_library(system_dam
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${PrecompiledHeaders}
)

# - target_precompile_headers sets the precompiled headers of the target (library)
# - The precompiled headers is set to PUBLIC so that other libraries in the solution can also use this precompiled header
target_precompile_headers(system_dam PUBLIC ${PrecompiledHeaders})

# - target_include_directories sets the include directories for a specfic target
# - It is set to PUBLIC meaning the parent targerts also inherit these directories
# - For example: the HYD Library is a parent of solver and has access to things like: include "cvode.h"
# - ${CMAKE_CURRENT_BINARY_DIR} because it is the location of the ui_xxxx.h files that are generated by qt_wrap_ui
target_include_directories(system_dam
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
		source_code                             
		source_code/economic
		source_code/CI	  
		source_code/general                     
		source_code/ecologic                    
		source_code/psycho_social               
		source_code/people                      
		source_code/simple_count                
		source_code/gui 
)

# - target_link_libraries links libraries to a specfic target
# - The target_include_directories tells the program where the headers are. But not what the classes do
# - The dlls or libs are what include the functions themselves.
# - system_dam only depends on system_hydraulic, the rest are for clarity
target_link_libraries(system_dam system_hydraulic system_sys)

# - qt_wrap_ui (or qt5_wrap_ui) wraps ui files, in other words generating .h files from them
# - AUTOMOC can also be used which does this automatically. This, however, is not recommened, it brings more troubles than solutions  
qt_wrap_ui(UI_FILES ${UI_FILES})