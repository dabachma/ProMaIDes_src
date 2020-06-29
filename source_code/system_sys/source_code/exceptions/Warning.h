#pragma once
/**\class Warning
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef WARNING_H
#define WARNING_H

//system sys class
#include "_Sys_Exception.h"
#include "Error.h"


///Data structure with gathers the warning number of the modules \ingroup sys
struct _sys_warning_module{
	///Number of warnings in the sys-modul
	int sys_warning;
	///Number of warnings in the fpl-modul
	int fpl_warning;
	///Number of warnings in the hyd-modul
	int hyd_warning;
	///Number of warnings in the dam-modul
	int dam_warning;
	///Number of warnings in the risk-modul
	int risk_warning;
	///Number of warnings in the madm-modul
	int madm_warning;
	///Number of warnings in the cost-modul
	int cost_warning;
	///Number of warnings in the alt-modul
	int alt_warning;
	///Total number of warnings
	int total_warnings;
	//..introduce further modules
};

///Exception-class for warnings \ingroup sys
/**
A warning is in contrast to an error not throwed in the application (throw...catch). It is 
immediately displayed and in general there is a reaction to the warning. The application 
continues normaly. The exception stored with all information in the exception file. For each module 
output a short information to the user will be displayed.
\see Error
*/
class Warning : public _Sys_Exception
{
public:
	///Default constructor
	Warning(void);
	///Default destructor
	~Warning(void);

	//methods
	///Set the warning message
	void set_msg(const string warn_place, const string warn_reason, const string warn_help, const string warn_reaction, const int warn_type);//set the error_msg
	///Output the warning message
	void output_msg(const int warning_occur);
	///Output the total number of errors
	static void output_total(void);
	///Reset the warning counter
	static void reset_counter(void);

	///Change an error to a warning (e.g. during mc or frc analysis)
	void error_to_warning(class Error *my_error, const int warn_type);

	///Set the reaction string, how the warning is encountered
	void set_reaction(const string reaction);

	///Set the flag if a warning output is required (true:= output is requiered; false:= no output of the warning)
	void set_during_calculation_flag(const bool flag);

	///Get the number of hydraulic warnings
	static int get_number_hyd_warnings(void);
	///Get the number of system warnings
	static int get_number_sys_warnings(void);
	///Get the number of damage warnings
	static int get_number_dam_warnings(void);
	///Get the number of fpl warnings
	static int get_number_fpl_warnings(void);
	///Get the number of madm warnings
	static int get_number_madm_warnings(void);
	///Get the number of risk warnings
	static int get_number_risk_warnings(void);
	///Get the number of cost warnings
	static int get_number_cost_warnings(void);
	///Get the number of alt warnings
	static int get_number_alt_warnings(void);
	//..introduce further modules

	///Set the warning number of the moduls to the given structure
	static void get_number_warning_moduls(_sys_warning_module *warning);
	///Set the difference of the warning number of the moduls to the given structure
	static void get_diff_number_warning_moduls(_sys_warning_module *warning);

private:
	//members
	///Flag if a warning output is required (true:= output is requiered; false:= no output of the warning)
	bool during_calculation_flag;

	///Count the total number of warnings occured in the sys-module
	static int warning_counter_sys;
	///Count the total number of warnings occured in the fpl-module
	static int warning_counter_fpl;
	//Count the total number of warnings occured in the hyd-module
	static int warning_counter_hyd;
	///Count the total number of warnings occured for the madm-modul
	static int warning_counter_madm;
	///Count the total number of warnings occured in the dam-module
	static int warning_counter_dam;
	///Count the total number of warnings occured in the risk-module
	static int warning_counter_risk;
	///Count the total number of warnings occured in the alt-module
	static int warning_counter_alt;
	///Count the total number of warnings occured in the cost-module
	static int warning_counter_cost;
	//..introduce further modules

	///Store the reason of the last warning message (for checking of equivalent, multiple warnings)
	static string old_reason;
	///Store the place of the last warning message (for checking of equivalent, multiple warnings)
	static string old_place;
	///Store the secondary information of the last warning message (for checking of equivalent, multiple warnings)
	static string old_info;
	///Counter how often an equivalent warning is occured; if it is occurred more than 2 times consecutively, it won't be output any more
	static int multiple_counter;

	///Information string, how the warning is encountered
	string reaction;
	///String for the time of warning occurence
	string time_warn;

	///String for the tooltip of the status bar; division of the exceptions to the modules
	static string exception_tooltip;

	//method
	///Decide which type of warning occured and convert it to a string; add new warning types if requiered
	string warning_type_translation(void);
	///Output the rough warning occurence via the prefixes of the text output
	void output_warning_occurrence(const int warning_occur);
	///Check if the last occured warning is from the place, the reason and the secondary info equivalent
	bool check_multi_warning(void);
	///Sum up the warnings of the modules to the total number of error
	void count_total_warnings(void);
	///Set the exception tooltip for the status bar
	static void set_exception_tooltip(void);
	
};
#endif