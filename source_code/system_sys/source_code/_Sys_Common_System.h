#pragma once
/**\class _Sys_Common_System
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _SYS_COMMON_SYSTEM_H
#define _SYS_COMMON_SYSTEM_H

//libs

//qt class
#include <QtSql>
#include <QApplication>
#include <QWidget>
#include <QBoxLayout>

//system sys class
#include "Warning.h"
#include "Error.h"
#include "Sys_Memory_Count.h"
#include "Tables.h"
#include "Data_Base.h"
#include "Common_Const.h"
#include "Sys_Database_Browser.h"


///Container class for three important members for objects, e.g. rivers, dikes etc., of the total system, the catchment area, \ingroup sys
/**

*/
class _Sys_Common_System 
{
public:
	///Default constructor
	_Sys_Common_System(void);
	///Default destructor
	 virtual ~_Sys_Common_System(void);

	//method
	///Set the system-id (_sys_system_id) of the object
	virtual void set_systemid(const _sys_system_id actuel_id);
	///Get the system id (_sys_system_id);
	_sys_system_id get_sys_system_id(void);
	///Get a pointer to the system id (_sys_system_id);
	_sys_system_id* get_ptr_sys_system_id(void);
	

protected:	
	//members
	///Identifier of the object, namely id-areastate and id-measure number
	_sys_system_id system_id;


};

#endif