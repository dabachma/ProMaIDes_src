# - Set allows giving a value to a variable similar to: UI_FILES = "file1.ui file2.ui file3.ui"
# - A list of .ui Files is made to make it easier to work with them
# - Glob can also be used but this manual way is safer and also the recommened approach for faster in between builds
# - The UI files need to be wrapped to generate the ui-xxxx.h files this is done using qt_wrap_ui
set(UI_FILES
	source_code/gui/Sys_Database_Dia.ui 
	source_code/gui/Sys_Database_File_Dia.ui 
	source_code/gui/Sys_Output_Flag_Dia.ui 
	source_code/gui/Sys_Filechooser_Dia.ui 
	source_code/gui/Sys_Multipathsettings_Dia.ui
	source_code/gui/Sys_Status_Bar_Wid.ui
	source_code/gui/Sys_Output_Logfile_Dia.ui
	source_code/gui/Sys_Welcome_Wid.ui
	source_code/database/Sys_Database_Projectbrowser_Wid.ui 
	source_code/gui/Sys_One_Filechooser_Wid.ui 
	source_code/gui/Sys_Multi_Filechooser_Dia.ui 
	source_code/gui/Sys_New_Project_Dia.ui 
	source_code/gui/Sys_Member_Bool_BaseWid.ui 
	source_code/gui/Sys_Member_Dbl_BaseWid.ui 
	source_code/gui/Sys_Member_Enum_BaseWid.ui 
	source_code/gui/Sys_Member_Int_BaseWid.ui 
	source_code/gui/Sys_Member_String_BaseWid.ui 
	source_code/gui/Sys_Member_ScientificDbl_BaseWid.ui 
	source_code/gui/Sys_Member_ScientificInt_BaseWid.ui 
	source_code/gui/Sys_Member_Dbl_Prec_BaseWid.ui 
	source_code/gui/Sys_Member_Dbl_Prec_BaseDia.ui 
	source_code/gui/Sys_Export_Dia.ui 
	source_code/gui/Sys_Text_Searcher_Wid.ui 
	source_code/gui/Sys_Text_Browser_Wid.ui 
	source_code/gui/Sys_File_Tree_Wid.ui 
	source_code/gui/Sys_Delete_Logfile_Dia.ui 
	source_code/gui/Sys_Result_Summary_Wid.ui 
	source_code/gui/_Sys_Abstract_Base_Wid.ui 
	source_code/gui/Sys_Base_Edit_Dia.ui 
	source_code/gui/Sys_Base_Expand_Wid.ui 
	source_code/gui/Sys_FallTree_BaseWid.ui 
    source_code/gui/Sys_Postgres_Config_Dia.ui
	source_code/gui/Sys_Plot_Frame.ui
	source_code/gui/Sys_PlotSettings_Dia.ui
	source_code/gui/Sys_CurveSettings_Wid.ui
	source_code/gui/Sys_Sql_Help_Row_Wid.ui
	source_code/gui/Sys_Sql_Help_Condition_Wid.ui
)

# - Set allows giving a value to a variable similar to: H_FILES = "file1.h file2.h file3.h"
# - A list of .h Files is made to make it easier to work with them
# - All the h files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files are parts of it
set(H_FILES
	source_code/_Sys_Common_System.h
	source_code/Common_Const.h
	source_code/Sys_Common_Output.h
	source_code/Sys_Memory_Count.h
	source_code/Sys_Output_Division.h
	source_code/Sys_Headers_Precompiled.h
	source_code/Tables.h
	source_code/Sys_Project.h
	source_code/Sys_Project_Copier.h
	source_code/Sys_Thread_Closing_Manager.h
	source_code/Profiler.h
	source_code/database/Data_Base.h
	source_code/database/Sys_Database_Browser.h
	source_code/database/Sys_Database_Projectbrowser_Wid.h
	source_code/database/Sys_Database_Project_Tree_Wid.h
	source_code/database/Sys_Sqlmodel_Loader.h
	source_code/database/Sys_AES_Algo.h
	source_code/exceptions/_Sys_Exception.h
	source_code/exceptions/Error.h
	source_code/exceptions/Sys_Exception_Sender.h
	source_code/exceptions/Warning.h
	source_code/exceptions/Sys_Reconnect_Db_Dia.h
	source_code/geometrie_general/_Geo_Geometrie.h
	source_code/geometrie_general/Geo_Circle.h
	source_code/geometrie_general/Geo_Point.h
	source_code/geometrie_general/_Geo_Polygon.h
	source_code/geometrie_general/Geo_Simple_Polygon.h
	source_code/geometrie_general/Geo_Raster_Polygon.h
	source_code/geometrie_general/Geo_Raster_Segment.h
	source_code/geometrie_general/Geo_Raster_Straight_Line.h
	source_code/geometrie_general/Geo_Polysegment.h
	source_code/geometrie_general/Geo_Segment.h
	source_code/geometrie_general/Geo_Straight_Line.h
	source_code/geometrie_general/Geo_Ray.h
	source_code/geometrie_general/Geo_Interception_Point_List.h
	source_code/geometrie_general/Geo_Point_List.h
	source_code/geometrie_general/Geo_Raster.h
	source_code/geometrie_general/Geo_Convex_Polygon.h
	source_code/geometrie_general/Geo_Complex_Polygon.h
	source_code/geometrie_general/Geo_Polygon_Interception_Point.h
	source_code/geometrie_general/Geo_Polygon_Interception_Point_List.h
	source_code/gui/Sys_Database_Dia.h
	source_code/gui/Sys_Database_File_Dia.h
	source_code/gui/Sys_Output_Flag_Dia.h
	source_code/gui/Sys_Status_Bar_Wid.h
	source_code/gui/Sys_Filechooser_Dia.h
	source_code/gui/Sys_Multipathsettings_Dia.h
	source_code/gui/Sys_Output_Logfile_Dia.h
	source_code/gui/Sys_Diverse_Text_Dia.h
	source_code/gui/Sys_Welcome_Wid.h
	source_code/gui/Sys_One_Filechooser_Wid.h
	source_code/gui/Sys_Multi_CheckBox_Dia.h
	source_code/gui/Sys_Multi_Filechooser_Dia.h
	source_code/gui/Sys_New_Project_Dia.h
	source_code/gui/Sys_Member_Bool_BaseWid.h
	source_code/gui/Sys_Member_Dbl_BaseWid.h
	source_code/gui/Sys_Member_Enum_BaseWid.h
	source_code/gui/Sys_Member_Int_BaseWid.h
	source_code/gui/Sys_Member_String_BaseWid.h
	source_code/gui/Sys_Member_ScientificDbl_BaseWid.h
	source_code/gui/Sys_Member_ScientificInt_BaseWid.h
	source_code/gui/Sys_Member_Dbl_Prec_BaseWid.h
    source_code/gui/Sys_Postgres_Config_Dia.h
	source_code/gui/Sys_Plot_Wid.h
	source_code/gui/Sys_Plot_Frame.h
	source_code/gui/Sys_PlotSettings_Dia.h
	source_code/gui/Sys_Sql_Table_Wid.h
	source_code/gui/Sys_Export_Dia.h
	source_code/gui/Sys_Text_Searcher_Wid.h
	source_code/gui/Sys_Text_Browser_Wid.h
	source_code/gui/Sys_File_Tree_Wid.h
	source_code/gui/Sys_Delete_Logfile_Dia.h
	source_code/gui/_Sys_Result_Summary_Wid.h
	source_code/gui/_Sys_Data_Wid.h
	source_code/gui/_Sys_Abstract_Base_Wid.h
	source_code/gui/Sys_SpinBox_Delegate.h
	source_code/gui/Sys_Base_Edit_Dia.h
	source_code/gui/Sys_Base_Expand_Wid.h
	source_code/gui/Sys_ComboBox_Delegate.h
	source_code/gui/Sys_DoubleSpinBox_Delegate.h
	source_code/gui/Sys_CurveSettings_Wid.h
	source_code/gui/Sys_Sql_Help_Row_Wid.h
	source_code/gui/Sys_Sql_Help_Condition_Wid.h
	source_code/gui/Sys_Check_Combobox_Model.h
	source_code/gui/Sys_Check_Combobox_View.h
	source_code/gui/Sys_Letter_Button.h
	source_code/gui/Sys_Custom_Plot_Picker.h	       
)

# - Set allows giving a value to a variable similar to: CPP_FILES = "file1.cpp file2.cpp file3.cpp"
# - A list of h Files is made to make it easier to work with them
# - All the .cpp files in this library need to be added here MANUALLY.
# - This variable is passed to the library when creating it so that it knows what files to compile
set(CPP_FILES
	source_code/_Sys_Common_System.cpp
	source_code/Sys_Common_Output.cpp
	source_code/Common_Function.cpp
	source_code/Sys_Memory_Count.cpp
	source_code/Sys_Output_Division.cpp
	source_code/Tables.cpp
	source_code/Sys_Project_Copier.cpp
	source_code/Sys_Thread_Closing_Manager.cpp
	source_code/Sys_Project.cpp
	source_code/Profiler.cpp
	source_code/database/Data_Base.cpp
	source_code/database/Sys_Database_Browser.cpp
	source_code/database/Sys_Database_Projectbrowser_Wid.cpp
	source_code/database/Sys_Database_Project_Tree_Wid.cpp
	source_code/database/Sys_Sqlmodel_Loader.cpp
	source_code/database/Sys_AES_Algo.cpp
	source_code/exceptions/_Sys_Exception.cpp
	source_code/exceptions/Error.cpp
	source_code/exceptions/Sys_Exception_Sender.cpp
	source_code/exceptions/Warning.cpp
	source_code/exceptions/Sys_Reconnect_Db_Dia.cpp
	source_code/geometrie_general/_Geo_Geometrie.cpp
	source_code/geometrie_general/Geo_Circle.cpp
	source_code/geometrie_general/Geo_Point.cpp
	source_code/geometrie_general/_Geo_Polygon.cpp
	source_code/geometrie_general/Geo_Simple_Polygon.cpp
	source_code/geometrie_general/Geo_Raster_Polygon.cpp
	source_code/geometrie_general/Geo_Raster_Segment.cpp
	source_code/geometrie_general/Geo_Raster_Straight_Line.cpp
	source_code/geometrie_general/Geo_Polysegment.cpp
	source_code/geometrie_general/Geo_Segment.cpp
	source_code/geometrie_general/Geo_Straight_Line.cpp
	source_code/geometrie_general/Geo_Ray.cpp
	source_code/geometrie_general/Geo_Interception_Point_List.cpp
	source_code/geometrie_general/Geo_Point_List.cpp
	source_code/geometrie_general/Geo_Raster.cpp
	source_code/geometrie_general/Geo_Convex_Polygon.cpp
	source_code/geometrie_general/Geo_Complex_Polygon.cpp
	source_code/geometrie_general/Geo_Polygon_Interception_Point.cpp
	source_code/geometrie_general/Geo_Polygon_Interception_Point_List.cpp
	source_code/gui/Sys_Database_Dia.cpp
	source_code/gui/Sys_Database_File_Dia.cpp
	source_code/gui/Sys_Output_Flag_Dia.cpp
	source_code/gui/Sys_Status_Bar_Wid.cpp
	source_code/gui/Sys_Filechooser_Dia.cpp
	source_code/gui/Sys_Multipathsettings_Dia.cpp
	source_code/gui/Sys_Output_Logfile_Dia.cpp
	source_code/gui/Sys_Diverse_Text_Dia.cpp
	source_code/gui/Sys_Welcome_Wid.cpp
	source_code/gui/Sys_One_Filechooser_Wid.cpp
	source_code/gui/Sys_Multi_CheckBox_Dia.cpp
	source_code/gui/Sys_Multi_Filechooser_Dia.cpp
	source_code/gui/Sys_New_Project_Dia.cpp
	source_code/gui/Sys_Member_Bool_BaseWid.cpp
	source_code/gui/Sys_Member_Dbl_BaseWid.cpp
	source_code/gui/Sys_Member_Enum_BaseWid.cpp
	source_code/gui/Sys_Member_Int_BaseWid.cpp
	source_code/gui/Sys_Member_String_BaseWid.cpp
	source_code/gui/Sys_Member_ScientificDbl_BaseWid.cpp
	source_code/gui/Sys_Member_ScientificInt_BaseWid.cpp
	source_code/gui/Sys_Member_Dbl_Prec_BaseWid.cpp
    source_code/gui/Sys_Postgres_Config_Dia.cpp
	source_code/gui/Sys_Plot_Wid.cpp
	source_code/gui/Sys_Plot_Frame.cpp
	source_code/gui/Sys_PlotSettings_Dia.cpp
	source_code/gui/Sys_Sql_Table_Wid.cpp
	source_code/gui/Sys_Export_Dia.cpp
	source_code/gui/Sys_Text_Searcher_Wid.cpp
	source_code/gui/Sys_Text_Browser_Wid.cpp
	source_code/gui/Sys_File_Tree_Wid.cpp
	source_code/gui/Sys_Delete_Logfile_Dia.cpp
	source_code/gui/_Sys_Result_Summary_Wid.cpp
	source_code/gui/_Sys_Data_Wid.cpp
	source_code/gui/_Sys_Abstract_Base_Wid.cpp
	source_code/gui/Sys_SpinBox_Delegate.cpp
	source_code/gui/Sys_Base_Edit_Dia.cpp
	source_code/gui/Sys_Base_Expand_Wid.cpp
	source_code/gui/Sys_ComboBox_Delegate.cpp
	source_code/gui/Sys_DoubleSpinBox_Delegate.cpp
	source_code/gui/Sys_CurveSettings_Wid.cpp
	source_code/gui/Sys_Sql_Help_Row_Wid.cpp
	source_code/gui/Sys_Sql_Help_Condition_Wid.cpp
	source_code/gui/Sys_Check_Combobox_Model.cpp
	source_code/gui/Sys_Check_Combobox_View.cpp
	source_code/gui/Sys_Letter_Button.cpp
	source_code/gui/Sys_Custom_Plot_Picker.cpp	       
)

# - Set allows giving a value to a variable similar to: QRC_FILES = "Resource1.qrc Resource2.qrc Resource3.qrc"
# - This set is not used.
# - It is left here because the following message are important:
# - THIS NEEDS TO BE INCULDED IN THE EXECUTABLE
# - THIS MEANS CLCode.rc NEEDS TO BE ADDED TO "add_executable(ProMaIDeS)"
# - add_executable(ProMaIDeS) IS FOUND IN THE TOP CMakeLists.txt
# - AFAIK you can't add resources to a shared library
set(QRC_FILES
	source_code/gui/resources/system_sys_resource.qrc
	source_code/gui/resources/hydroxygen.qrc
)

# - Set allows giving a value to a variable similar to: PrecompiledHeaders = "PrecompiledHeaders.h"
# - Set is used here for consistency of the code
# - The precompiledHeader will be targeted by the library so that it precomiples this header file for faster build times
set(PrecompiledHeaders
	source_code/Sys_Headers_Precompiled.h
)

# - add_library create the library
# - The add_library is the engine here and is defined as add_library(<name> [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL] [<source>...])
# - system_sys is the name given to the library
# - What follows are the source codes
add_library(system_sys
	${UI_FILES}
	${H_FILES}
	${CPP_FILES}
	${PrecompiledHeaders}
)

# - target_precompile_headers sets the precompiled headers of the target (library)
# - system_hydraulic is the target
# - The precompiled headers is set to PUBLIC so that other libraries in the solution can also use this precompiled header
target_precompile_headers(system_sys PUBLIC ${PrecompiledHeaders})

# - target_include_directories sets the include directories for a specfic target
# - It is set to PUBLIC meaning the parent targerts also inherit these directories
# - For example: the HYD Library is a parent of solver and has access to things like: include "cvode.h"
# - Qwt Headers are required and thus added to the include directory.
# - ${CMAKE_CURRENT_BINARY_DIR} because it is the location of the ui_xxxx.h files that are generated by qt_wrap_ui
# - ${QWT_INCLUDE_DIR} is defined from the main CMakeLists.txt file. And passed on to this library.
# - ${Qt5_INCLUDE_DIRS} is not required as it is handled automatically by find_package(Qt5 REQUIRED)
target_include_directories(system_sys
    PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}
      source_code
      source_code/database
      source_code/exceptions
      source_code/geometrie_general
      source_code/gui
      source_code/gui/resources
      ${QWT_INCLUDE_DIR}
)

# - target_link_libraries links libraries to a specfic target
# - The target_include_directories tells the program where the headers are. But not what the classes do
# - The dlls or libs are what include the functions themselves.
# - Qt required libraries as well as QWT are listed here
target_link_libraries(system_sys
	PUBLIC
    	Qt::Core
    	Qt::Sql
    	Qt::Svg
    	Qt::PrintSupport
    	Qt::Widgets
    	Qt::Gui
    	Qt::Xml
    	#Qt::OpenGL
		$<$<PLATFORM_ID:Windows>:Qt::WinMain>
		$<$<PLATFORM_ID:Linux>:Qt::X11Extras>
    	#$<$<PLATFORM_ID:Linux>:${PostgreSQL_LIBRARIES}>
		$<$<CONFIG:Release>:${QWT_LIBRARY_RELEASE}>
		$<$<CONFIG:Debug>:${QWT_LIBRARY_DEBUG}>
)

# - qt_wrap_ui (or qt5_wrap_ui) wraps ui files, in other words generating .h files from them
# - AUTOMOC can also be used which does this automatically. This, however, is not recommened, it brings more troubles than solutions  
qt_wrap_ui(UI_FILES ${UI_FILES})