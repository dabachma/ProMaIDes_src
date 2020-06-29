#pragma once
#ifndef FPL_DATA_H
#define FPL_DATA_H

//libs

//class
#include "_Common_System.h"
#include "Fpl_Data_Section.h"
#include "Fpl_Mc_Sim.h"



class Fpl_Data : public _Common_System
{
public:

	//constructor
	Fpl_Data(void);
	//destructor
	~Fpl_Data(void);
	//decide per console which action is desired
	void action(void);
	//show the performance table
	QTableView* show_performance_table(void);
	
private:
	//members
	Database_Viewer *my_browser;
	

	//build the error msg
	Error set_error(const int err_type);

};
#endif