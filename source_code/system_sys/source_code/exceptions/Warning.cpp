//#include "Warning.h"
//#include "Error.h"

#include "Sys_Headers_Precompiled.h"

//initialize the static variables
int Warning::warning_counter_sys=0;
int Warning::warning_counter_fpl=0;
int Warning::warning_counter_hyd=0;
int Warning::warning_counter_madm=0;
int Warning::warning_counter_dam=0;
int Warning::warning_counter_risk=0;
int Warning::warning_counter_alt=0;
int Warning::warning_counter_cost=0;

string Warning::old_place;
string Warning::old_info;
string Warning::old_reason;
string Warning::exception_tooltip;
int Warning::multiple_counter=0;

//constructor
Warning::Warning(void){
	this->during_calculation_flag=true;	
}

//destructor
Warning::~Warning(void){
	
}
//_________________________________
//public
//set the warning_msg
void Warning::set_msg(const string warn_place, const string warn_reason, const string warn_help, const string warn_reaction, const int warn_type){
	place=warn_place;
	reason=warn_reason;
	help=warn_help;
	type=warn_type;
	reaction=warn_reaction;
	time_warn=set_time();
}
//output the msg 
void Warning::output_msg(const int warning_occur){
	if(this->check_multi_warning()==true && Warning::multiple_counter>=2){
		return;
	}
	//store the old warnings
	Warning::old_reason=this->reason;
	Warning::old_place=this->place;
	Warning::old_info=this->secundary_info;


	this->output_warning_occurrence(warning_occur);
	//output to the exception outputs
	//put the output togehter
	ostringstream cout1;
	cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout1 << "WARNING" <<endl;
	//time of error
	cout1 << "DATE/TIME              " << this->time_warn;
	//number of error
	cout1 << "NUMBER                 " << this->counter_output << endl;
	//module occurrence
	cout1 << "MODULE                 " << this->module_occurrence << endl;
	//number erro module
	cout1 << "WARNING IN MODULE      " << this->current_exception_number_module <<endl;
	cout1 << "TYPE                   " << this->warning_type_translation();
	//further information
	cout1 << "REASON                 ";
	_Sys_Exception::insert_tabs_after_endl(&this->reason);
	cout1 << this->reason << endl;
	cout1 << "CODE PLACE             "<< this->place << endl;
	cout1 << "APPLICATION PLACE      " << this->occurence << endl;
	_Sys_Exception::insert_tabs_after_endl(&help);
	cout1 << "HELP                   " << this->help << endl;
	if(this->reaction.empty()==true){
		this->reaction="No reaction ";
	}
	cout1 << "REACTION               " << this->reaction << endl;
	cout1 << "SECOND INFOS           ";
	if(secundary_info.empty()){
		cout1 << "No secondary information available " << endl;
	}
	else{
		_Sys_Exception::insert_tabs_after_endl(&this->secundary_info);
		cout1 << this->secundary_info ;
	}
	if(Warning::multiple_counter==1){
		cout1 << "This warning occurs several times back-to-back!" << endl;
	}
	cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	//final output to excep output
	Sys_Common_Output::output_excep->output_txt(cout1.str(),false);
	Sys_Common_Output::output_excep->reset_prefix_was_outputed();
	Warning::set_exception_tooltip();
	//emit the error number
	Sys_Exception_Sender::self()->set_warning_number(this->counter_output, Warning::exception_tooltip.c_str());	


}
//output total number of warnings
void Warning::output_total(void){
	ostringstream buff;
	int total= Warning::warning_counter_sys+Warning::warning_counter_fpl+Warning::warning_counter_hyd+
		Warning::warning_counter_dam+Warning::warning_counter_risk+Warning::warning_counter_alt+Warning::warning_counter_cost+
		Warning::warning_counter_madm;
	buff<<"________________________"<<endl;
	buff<<"Total numbers of warnings:  " << total << endl;
	if(Warning::warning_counter_sys!=0){
		buff << " " <<sys_label::str_sys << "  " <<Warning::warning_counter_sys<<endl;
	}
	if(Warning::warning_counter_fpl!=0){
		buff << " " <<sys_label::str_fpl << "  " <<Warning::warning_counter_fpl<<endl;
	}
	if(Warning::warning_counter_hyd!=0){
		buff << " " <<sys_label::str_hyd << "  " <<Warning::warning_counter_hyd<<endl;
	}
	if(Warning::warning_counter_dam!=0){
		buff << " " <<sys_label::str_dam << "  " <<Warning::warning_counter_dam<<endl;
	}
	if(Warning::warning_counter_risk!=0){
		buff << " " <<sys_label::str_risk << " " <<Warning::warning_counter_risk<<endl;
	}
	if(Warning::warning_counter_alt!=0){
		buff << " " <<sys_label::str_alt << "  " <<Warning::warning_counter_alt<<endl;
	}
	if(Warning::warning_counter_cost!=0){
		buff << " " <<sys_label::str_cost << " " <<Warning::warning_counter_cost<<endl;
	}
	if(Warning::warning_counter_madm!=0){
		buff << " " <<sys_label::str_madm << " " <<Warning::warning_counter_madm<< endl;
	}

	Sys_Common_Output::output_excep->output_txt(&buff,false);
}
//reset the counter
void Warning::reset_counter(void){
	Warning::warning_counter_sys=0;
	Warning::warning_counter_fpl=0;
	Warning::warning_counter_hyd=0;
	Warning::warning_counter_madm=0;
	Warning::warning_counter_dam=0;
	Warning::warning_counter_risk=0;
	Warning::warning_counter_cost=0;
	Warning::warning_counter_alt=0;

	Warning::old_place="";
	Warning::old_info="";
	Warning::old_reason="";

	Warning::multiple_counter=0;

	Warning::set_exception_tooltip();

	//emit the error number
	Sys_Exception_Sender::self()->set_warning_number(0, Warning::exception_tooltip.c_str());
}
//change an error to a warning (during mc or frc analysis)
void Warning::error_to_warning(class Error *my_error, const int warn_type){
	this->place=my_error->place;
	this->reason=my_error->reason;
	this->help=my_error->help;
	this->type=warn_type;
	this->time_warn=my_error->time_err;
	this->secundary_info=my_error->secundary_info;

};
//Set the reaction string, how the warning is encountered
void Warning::set_reaction(const string reaction){
	this->reaction=reaction;
}
//Set the flag if a warning output is required (true:= output is requiered; false:= no output of the warning)
void Warning::set_during_calculation_flag(const bool flag){
	this->during_calculation_flag=flag;
}
//Get the number of hydraulic warnings (static)
int Warning::get_number_hyd_warnings(void){
	return warning_counter_hyd;
}
//Get the number of system warnings (static)
int Warning::get_number_sys_warnings(void){
	return warning_counter_sys;
}
//Get the number of damage warnings (static)
int Warning::get_number_dam_warnings(void){
	return warning_counter_dam;
}
//Get the number of fpl warnings (static)
int Warning::get_number_fpl_warnings(void){
	return Warning::warning_counter_fpl;
}
//Get the number of madm warnings (static)
int Warning::get_number_madm_warnings(void){
	return Warning::warning_counter_madm;
}
//Get the number of risk warnings (static)
int Warning::get_number_risk_warnings(void){
	return Warning::warning_counter_risk;
}
//Get the number of cost warnings (static)
int Warning::get_number_cost_warnings(void){
	return Warning::warning_counter_cost;
}
//Get the number of alt warnings (static)
int Warning::get_number_alt_warnings(void){
	return Warning::warning_counter_alt;
}
//Set the warning number of the moduls to the given structure (static)
void Warning::get_number_warning_moduls(_sys_warning_module *warning){
	warning->dam_warning=Warning::warning_counter_dam;
	warning->fpl_warning=Warning::warning_counter_fpl;
	warning->hyd_warning=Warning::warning_counter_hyd;
	warning->risk_warning=Warning::warning_counter_risk;
	warning->sys_warning=Warning::warning_counter_sys;
	warning->madm_warning=Warning::warning_counter_madm;
	warning->cost_warning=Warning::warning_counter_cost;
	warning->alt_warning=Warning::warning_counter_alt;
	warning->total_warnings=0;
}
//Set the difference of the warning number of the moduls to the given structure (static)
void Warning::get_diff_number_warning_moduls(_sys_warning_module *warning){
	warning->dam_warning=Warning::warning_counter_dam-warning->dam_warning;
	warning->fpl_warning=Warning::warning_counter_fpl-warning->fpl_warning;
	warning->hyd_warning=Warning::warning_counter_hyd-warning->hyd_warning;
	warning->risk_warning=Warning::warning_counter_risk-warning->risk_warning;
	warning->sys_warning=Warning::warning_counter_sys-warning->sys_warning;
	warning->madm_warning=Warning::warning_counter_madm-warning->madm_warning;

	warning->cost_warning=Warning::warning_counter_cost-warning->cost_warning;
	warning->alt_warning=Warning::warning_counter_alt-warning->alt_warning;

	warning->total_warnings=warning->dam_warning+warning->fpl_warning+warning->hyd_warning+
		warning->risk_warning+warning->sys_warning+warning->madm_warning+warning->alt_warning
		+warning->cost_warning;
}
//_________________________________
//privat
//decide which type of warning
string Warning::warning_type_translation(void){
	ostringstream cout;
	//warningtype
		switch (type){
			case 1:
				cout << "File Input Warning " << endl;
				break;
			case 2:
				cout << "Database Input Warning " << endl;
				break;
			case 3:
				cout << "Variable Warning  " << endl;
				break;
			case 4:
				cout << "Output Warning  " << endl;
				break;
			case 5:
				cout << "Code Warning "<< endl;
				break;
			case 6:
				cout << "MC Warning " << endl;
				break;
			case 7:
				cout << "FRC Warning " << endl;
				break;
			case 8:
				cout << "FPL-Section Warning " << endl;
				break;
			case 9:
				cout << "Geometrie Warning " << endl;
				break;
			case 10:
				cout << "Coupling Warning" << endl;
				break;
			case 11:
				cout << "Solver Warning" << endl;
				break;
			case 12:
				cout << "River Model Warning" << endl;
				break;
			case 13:
				cout << "Memory Warning" << endl;
				break;
			case 14:
				cout << "Multi-thread Warning" << endl;
				break;
			case 15:
				cout << "Economic Damage Warning" << endl;
				break;
			case 16:
				cout << "People2risk Damage Warning" << endl;
				break;
			case 17:
				cout << "Ecological Damage Warning" << endl;
				break;
			case 18:
				cout << "Damage Calculation Warning" << endl;
				break;
			case 19:
				cout << "Psycho-social Damage Warning" << endl;
				break;
			case 20:
				cout << "Fault-tree Warning" << endl;
				break;
			case 21:
				cout << "Break risk Warning" << endl;
				break;
			case 22:
				cout << "Nobreak risk Warning" << endl;
				break;
			case 23:
				cout << "FPL2HYD connection Warning" << endl;
				break;
			case 24:
				cout << "MADM-analysis Warning" << endl;
				break;
			case 25:
				cout << "ALT Warning" << endl;
				break;
			case 26:
				cout << "FPL slope Warning " << endl;
				break;
			case 27:
				cout << "CI-system Waring" << endl;
				break;
			default:
				cout << "Unknown Warning " << endl;
		}		
	return cout.str();
}
//output where the warning occurrs
void Warning::output_warning_occurrence(const int warning_occur){

	
	//output to the outputs where the warning occurs
	ostringstream cout1;
	cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	cout1 << "A Warning is occured!" << endl;
	
	switch (warning_occur){
		//system sys
		case 0:
			this->warning_counter_sys++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_system->get_totalprefix();
			this->module_occurrence="SYS: Check SYS-logfile, SYS-display";
			this->current_exception_number_module=this->warning_counter_sys;
			break;
		//system fpl
		case 1:
			this->warning_counter_fpl++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_fpl->get_totalprefix();
			this->module_occurrence="FPL: Check FPL-logfile, FPL-display";
			this->current_exception_number_module=this->warning_counter_fpl;
			break;
		//system hyd
		case 2:
			this->warning_counter_hyd++;
			this->count_total_warnings();	
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_hyd->get_totalprefix();
			this->module_occurrence="HYD: Check HYD-logfile, HYD-display";
			this->current_exception_number_module=this->warning_counter_hyd;
			break;
		//system madm
		case 3:
			this->warning_counter_madm++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_madm->get_totalprefix();
			this->module_occurrence="MADM: Check MADM-logfile, MADM-display";
			this->current_exception_number_module=this->warning_counter_madm;
			break;
		//system dam
		case 4:
			this->warning_counter_dam++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_dam->get_totalprefix();
			this->module_occurrence="DAM: Check DAM-logfile, DAM-display";
			this->current_exception_number_module=this->warning_counter_dam;
			break;
		//system risk
		case 5:
			this->warning_counter_risk++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_risk->get_totalprefix();
			this->module_occurrence="RISK: Check RISK-logfile, RISK-display";
			this->current_exception_number_module=this->warning_counter_risk;
			break;
		//system alt
		case 6:
			this->warning_counter_alt++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_alt->get_totalprefix();
			this->module_occurrence="ALT: Check ALT-logfile, ALT-display";
			this->current_exception_number_module=this->warning_counter_alt;
			break;
		//system cost
		case 7:
			this->warning_counter_cost++;
			this->count_total_warnings();
			cout1 << "Check the exception output for Warningnumber " << this->counter_output << endl;
			cout1 << "++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			this->occurence=Sys_Common_Output::output_cost->get_totalprefix();
			this->module_occurrence="COST: Check COST-logfile, ALT-display";
			this->current_exception_number_module=this->warning_counter_cost;
			break;
		default:
			break;
	}

	if(this->during_calculation_flag==true){
		Sys_Common_Output::output_system->output_txt(cout1.str(),false);
	}
}
//check if the last warning is from place and reason the same
bool Warning::check_multi_warning(void){
	if(Warning::old_reason==this->reason && Warning::old_place==this->place && Warning::old_info==this->secundary_info){
		Warning::multiple_counter++;
		return true;
	}
	else{
		Warning::multiple_counter=0;
		return false;
	}
}
//Sum up the warnings of the modules to the total number of error
void Warning::count_total_warnings(void){
	this->counter_output=Warning::warning_counter_fpl+Warning::warning_counter_sys+
		Warning::warning_counter_hyd+Warning::warning_counter_madm+Warning::warning_counter_dam+
		Warning::warning_counter_risk+Warning::warning_counter_alt+Warning::warning_counter_cost;

}
//Set the exception tooltip for the status bar (static)
void Warning::set_exception_tooltip(void){
	ostringstream buff;
	string buffer;
	if(Warning::warning_counter_sys!=0){
		buff << sys_label::str_sys << "  " <<Warning::warning_counter_sys<<endl;
	}
	if(Warning::warning_counter_fpl!=0){
		buff << sys_label::str_fpl << "  " <<Warning::warning_counter_fpl<<endl;
	}
	if(Warning::warning_counter_hyd!=0){
		buff << sys_label::str_hyd << "  " <<Warning::warning_counter_hyd<<endl;
	}
	if(Warning::warning_counter_dam!=0){
		buff << sys_label::str_dam << "  " <<Warning::warning_counter_dam<<endl;
	}
	if(Warning::warning_counter_risk!=0){
		buff << sys_label::str_risk << " " <<Warning::warning_counter_risk<<endl;
	}
	if(Warning::warning_counter_alt!=0){
		buff << sys_label::str_alt << "  " <<Warning::warning_counter_alt<<endl;
	}
	if(Warning::warning_counter_cost!=0){
		buff << sys_label::str_cost << " " <<Warning::warning_counter_cost<<endl;
	}
	if(Warning::warning_counter_madm!=0){
		buff << sys_label::str_madm << " " <<Warning::warning_counter_madm<< endl;
	}

	buffer=buff.str();
	int pos=0;
	pos=buffer.rfind("\n");

	if(pos>0){
		buffer=buffer.erase(pos);
	}

	Warning::exception_tooltip = buffer;
}