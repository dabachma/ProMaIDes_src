#pragma once
#ifndef  MAIN_HEADERS_PRECOMPILED_H
#define MAIN_HEADERS_PRECOMPILED_H

//libs
//#include <iostream>//readin /read out
//#include <string>//string-functions
//#include <sstream>//stream function for strings like int to string
//
////qt class
//#include <QMessageBox>
//#include <QScrollBar>
//#include <QTableView>
//#include <QMetaType>

//system sys
#include "Data_Base.h"
#include "Common_Const.h"
#include "Sys_Common_Output.h"
#include "Sys_Exception_Sender.h"
#include "Sys_Status_Bar_Wid.h"

//system fpl
#include "Fpl_Calculation.h"
//system dam
#include "Dam_Damage_System.h"
//system hyd
#include "Hyd_Multiple_Hydraulic_Systems.h"
//system hyd
#include "Alt_System.h"

//ui forms
#include "ui_Main_Wid.h"

//system mainwid
#include "Main_Wid.h"
#include "Sys_Data_Tree_Wid.h"
#include "Sys_Output_Text_Wid.h"
#include "Sys_Data_Widget_Loader.h"

#include "Sys_Dock_Widget.h"

//class for the version update
#include "Sys_Version_Update.h"

#endif
