#pragma once
/**\class Error
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef ERROR_H
#define ERROR_H

//system sys class
#include "_Sys_Exception.h"
#include "Sys_Exception_Sender.h"

///Data structure with gathers the error number of the modules \ingroup sys
struct _sys_error_module{
	///Number of errors in the sys-modul
	int sys_error;
	///Number of errors in the fpl-modul
	int fpl_error;
	///Number of errors in the hyd-modul
	int hyd_error;
	///Number of errors in the dam-modul
	int dam_error;
	///Number of errors in the risk-modul
	int risk_error;
	///Number of errors in the madm-modul
	int madm_error;
	///Number of errors in the alt-modul
	int alt_error;
	///Number of errors in the cost-modul
	int cost_error;
	///Total number of errors
	int total_error;
	//..introduce further modules
};

///Exception-class for errors \ingroup sys
/**
An error is throwed during application (throw...catch) after the information of the 
error are gathered. The error is throwed to this point in the programm, where 
the programm can continue without any detraction of the error. There are no reaction to
an error possible. The error can be also fatal, then a user reaction is immediately required. The exception stored with all information in the exception file. For each module 
output a short information to the user will be displayed.
\see Warning
*/
class Error : public _Sys_Exception
{
///Required to assign the error information to a warning, if it is switch to a warning
friend class Warning;

public:
	///Default constructor
	Error(void);
	///Default destructor
	~Error(void);

	//members

	//methods
	///Set the error message
	void set_msg(const string err_place, const string err_reason, const string err_help, const int err_type, const bool err_fatal);
	///Set the error message; here it is a user aborted message
	void set_msg( _sys_system_modules module_type_user_abort);

	///Output the error message
	void output_msg(const int error_occur);
	///Output the total number of errors
	static void output_total(void);
	///Reset the error counter
	static void reset_counter(void);
	///Get the fatal error flag
	bool give_fatal_flag(void);
	///Set the fatal error flag
	void set_fatal_flag(const bool flag);

	///Get the flag that is a user aborted exception; it is handled and throwed like an error
	bool get_user_aborted_exception(void);

	///Get the number of hydraulic errors
	static int get_number_hyd_errors(void);
	//Get the number of system errors
	static int get_number_sys_errors(void);
	///Get the number of damage errors
	static int get_number_dam_errors(void);
	///Get the number of fpl errors 
	static int get_number_fpl_errors(void);
	///Get the number of madm errors
	static int get_number_madm_errors(void);
	///Get the number of risk errors
	static int get_number_risk_errors(void);
	///Get the number of cost errors
	static int get_number_cost_errors(void);
	///Get the number of alt errors
	static int get_number_alt_errors(void);
	//..introduce further modules

	///Set the error number of the moduls to the given structure
	static void get_number_errors_moduls(_sys_error_module *errors);
	///Set the difference of the error number of the moduls to the given structure
	static void get_diff_number_errors_moduls(_sys_error_module *errors);
	
private:
	//members
	///Flag if it is a fatal error; the user must encounter it directly
	bool fatal_flag;
	///Flag if the exception occurs due to a user abortion of the application
	bool user_aborted_exception;

	///Count the total number of errors occured in the sys-module
	static int error_counter_sys;
	///Count the total number of errors occured in the fpl-module
	static int error_counter_fpl;
	///Count the total number of errors occured in the hyd-module
	static int error_counter_hyd;
	///Count the total number of errors occured in the madm-module
	static int error_counter_madm;
	///Count the total number of errors occured in the dam-module
	static int error_counter_dam;
	///Count the total number of errors occured in the risk-module
	static int error_counter_risk;
	///Count the total number of errors occured in the alt-module
	static int error_counter_alt;
	///Count the total number of errors occured in the cost-module
	static int error_counter_cost;
	//..introduce further modules

	///String for the time of error occurence
	string time_err;

	///String for the tooltip of the status bar; division of the exceptions to the modules
	static string exception_tooltip;

	
	//methods
	///Decide which type of error occured and convert it to a string; add new error types if requiered
	string error_type_translation(void);
	///Output the rough error occurence via the prefixes of the text output
	void output_error_occurrence(const int error_occur);
	///Set the flag that is a user aborted exception; it is handled and throwed like an error
	void set_user_aborted_exception(void);
	///Sum up the errors of the modules to the total number of error
	void count_total_error(void);
	///Set the exception tooltip for the status bar
	static void set_exception_tooltip(void);

	
};
#endif
