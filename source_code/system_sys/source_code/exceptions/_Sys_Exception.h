#pragma once
/**\class _Sys_Exception
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _EXCEPTION_H
#define _EXCEPTION_H
//no unsafe warning
#define _CRT_SECURE_NO_WARNINGS 

//libs
#include <cstdlib>//for ranmar;exit()
#include <iostream>//readin /read out
#include <fstream>//function for output/input to a file
#include <string>//string-functions
#include <sstream>//stream function for ostreams like int to string
#include <ctime>//time-funtcions
//qtlibs 
#include <QMessageBox>

//system sys class
#include "Sys_Common_Output.h"

//namespaces
using namespace std;

///Base class for exceptions; place, reason, help etc. are gathered here and are outputed here \ingroup sys
/**
fjkffhdh
*/
class _Sys_Exception 
{

public:
	///Default constructor
	_Sys_Exception(void);
	///Default destructor
	virtual ~_Sys_Exception(void);

	//members


	//methods
	///Append to string secondary informations
	void make_second_info(const string info);

	///Insert into a string with line-breaks tabulators after the line-breaks
	static void insert_tabs_after_endl(string *txt);

protected:
	//members:
	///Place of the exception in the code
	string place;
	///Reason for the exception
	string reason;
	///Help to encounter the exception
	string help;
	///Prefix for the output of the exceptions
	string userprefix;
	///Text for the exception in which module the error/warning occurs (0 sys, 1 fpl, 2 hyd, 3 madm, 4 dam)
	string module_occurrence;
	///Number of error/warning for the current module for the output
	int current_exception_number_module;

	///Type of the exception (further specified in the child classes)
	int type;
	///Secondary information to the exception
	string secundary_info;
	//Counter, how often the same exeption occurred
	int counter_output;
	///Place, where the exception occurred in the application
	string occurence; 

	///Type of the module, where the exception occurs
	_sys_system_modules module_type;


	//methods
	///Output the exception
	virtual void output_msg(const int error_occur)=0;
	///Set the exceptiontime
	string set_time(void); 
	///Output the message to the corresponding module output display
	void output_msg2moduldisplay(string txt);

	
};
#endif