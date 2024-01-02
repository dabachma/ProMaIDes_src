#pragma once
#ifndef SYS_HEADERS_PRECOMPILED_H
#define SYS_HEADERS_PRECOMPILED_H


//MS specific (Overloading of not deprecated Functions)
#ifdef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#endif

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif 

#define _CRT_SECURE_NO_DEPRECATE 

#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
// End of MS specific

//project managing class
#include "Sys_Project.h"

//geometrie classes
#include "_Geo_Geometrie.h"
#include "Geo_Point.h"
#include "Geo_Straight_Line.h"
#include "Geo_Ray.h"
#include "Geo_Segment.h"
#include "Geo_Polysegment.h"
#include "_Geo_Polygon.h"
#include "Geo_Simple_Polygon.h"
#include "Geo_Raster_Polygon.h"
#include "Geo_Interception_Point_List.h"
#include "Geo_Point_List.h"
#include "Geo_Raster.h"
#include "Geo_Circle.h"
#include "Geo_Convex_Polygon.h"
#include "Geo_Complex_Polygon.h"
#include "Geo_Polygon_Interception_Point_List.h"
#include "Geo_Polygon_Interception_Point.h"
#include "Geo_Raster_Segment.h"
#include "Geo_Raster_Straight_Line.h"

//exception classes
//base class of the exceptions
#include "_Sys_Exception.h"
//error class as exception
#include "Error.h"
//warning class as exception
#include "Warning.h"
//sender class for the exceptions
#include "Sys_Exception_Sender.h"

//memory counter
#include "Sys_Memory_Count.h"

//division class for the output
#include "Sys_Output_Division.h"
//common output class
#include "Sys_Common_Output.h"

//the database class
#include "Data_Base.h"
//class for the database tables
#include "Tables.h"
//class for aes-encoding
#include "Sys_AES_Algo.h"

#include "_Sys_Common_System.h"
//definition of constants and labels
#include "Common_Const.h"

//Gui class for the statusbar widget
#include "Sys_Status_Bar_Wid.h"

//mysql browser for the database
#include "Sys_Database_Browser.h"

//dialog for postgres configuration in system tray
#include "Sys_Postgres_Config_Dia.h"
//dialog for the database file
#include "Sys_Database_File_Dia.h"
//dialog for the database connection
#include "Sys_Database_Dia.h"
//dialog to set the output detailed flag
#include "Sys_Output_Flag_Dia.h"

//dialog for file choosing
#include "Sys_Filechooser_Dia.h"
//dialog for multipath settings without a textlabel for each browser 
#include "Sys_Multipathsettings_Dia.h"

//widget for a file browser-line
#include "Sys_One_Filechooser_Wid.h"
//dialog for multiple browser-lines
#include "Sys_Multi_Filechooser_Dia.h"

//Dialog-class for asking the deleting of logfiles in the logfile-archiv
#include "Sys_Delete_Logfile_Dia.h"

//dialog to change the logfile of the modules
#include "Sys_Output_Logfile_Dia.h"
//Dialog-class for reasking the user
#include "Sys_Diverse_Text_Dia.h"
//dialog class if the database connection fails during calculation
#include "Sys_Reconnect_Db_Dia.h"
//Widget-class for the welcome widget
#include "Sys_Welcome_Wid.h"
//dialog for an export to file
#include "Sys_Export_Dia.h"

//frame class with a plot included
#include "Sys_Plot_Frame.h"
//Widget-class for a plot
#include "Sys_Plot_Wid.h"
//Dialog class for plot settings
#include "Sys_PlotSettings_Dia.h"
#include "Sys_CurveSettings_Wid.h"
#include "Sys_Custom_Plot_Picker.h"
#include "Sys_Check_Combobox_View.h"
#include "Sys_Check_Combobox_Model.h"


//Button class for sending the button text
#include "Sys_Letter_Button.h"


//Dialog for creating new projects
#include "Sys_New_Project_Dia.h"

//Widget class as tree-widget for displaying the existing project tables in the database
#include "Sys_Database_Project_Tree_Wid.h"
//Widget class of a simple sql database browser for browsing the project tables in the database
#include "Sys_Database_Projectbrowser_Wid.h"
//Thread class for loading sql table models
#include "Sys_Sqlmodel_Loader.h"
//Tableview widget for displaying the sql-tables
#include "Sys_Sql_Table_Wid.h"


//Base widget for displaying data
#include "Sys_Member_Bool_BaseWid.h"
#include "Sys_Member_Dbl_BaseWid.h"
#include "Sys_Member_Dbl_Prec_BaseWid.h"
#include "Sys_Member_Enum_BaseWid.h"
#include "Sys_Member_Int_BaseWid.h"
#include "Sys_Member_ScientificDbl_BaseWid.h"
#include "Sys_Member_ScientificInt_BaseWid.h"
#include "Sys_Member_String_BaseWid.h"


//Delegates for model item in Qt
#include "Sys_ComboBox_Delegate.h"
#include "Sys_SpinBox_Delegate.h"
#include "Sys_DoubleSpinBox_Delegate.h"

//Thread class for mannaging the clsoing of the different threads
#include "Sys_Thread_Closing_Manager.h"
//text searcher for the output displays
#include "Sys_Text_Searcher_Wid.h"
//Text browser widget
#include "Sys_Text_Browser_Wid.h"
//file tree widget
#include "Sys_File_Tree_Wid.h"

//base widgets classes
#include "_Sys_Data_Wid.h"
#include "_Sys_Result_Summary_Wid.h"

//base widget for the data display widgets
#include "_Sys_Abstract_Base_Wid.h"

#include "Sys_Base_Expand_Wid.h"
#include "Sys_Base_Edit_Dia.h"

#include "Sys_FallTree_BaseWid.h"


#include "Sys_Multi_CheckBox_Dia.h"

//helper widegt for an sql query
#include "Sys_Sql_Help_Row_Wid.h"
//helper widegt for an sql query
#include "Sys_Sql_Help_Condition_Wid.h"

//thread class to copy projects
#include "Sys_Project_Copier.h"


//namespace
using namespace std;

#endif