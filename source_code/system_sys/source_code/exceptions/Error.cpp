//#include "Error.h"
#include "Sys_Headers_Precompiled.h"

//init static members
int Error::error_counter_sys=0;
int Error::error_counter_fpl=0;
int Error::error_counter_hyd=0;
int Error::error_counter_madm=0;
int Error::error_counter_dam=0;
int Error::error_counter_risk=0;
int Error::error_counter_alt=0;
int Error::error_counter_cost=0;
string Error::exception_tooltip;

//constructor
Error::Error(void){
	this->fatal_flag=false;
	this->user_aborted_exception=false;
}
//destructor
Error::~Error(void){	
}
//_________________________________
//public
//set the error_msg
void Error::set_msg(const string err_place, const string err_reason, const string err_help, const int err_type, const bool err_fatal){
	this->user_aborted_exception=false;
	this->place=err_place;
	this->reason=err_reason;
	this->help=err_help;
	this->type=err_type;
	this->fatal_flag=err_fatal;
	this->time_err=set_time();
}
//Set the error message; here it is a user aborted message
void Error::set_msg( _sys_system_modules module_type_user_abort){
	this->time_err=set_time();
	this->set_user_aborted_exception();
	this->module_type=module_type_user_abort;
	switch (this->module_type){
		case _sys_system_modules::SYS_SYS:
			this->reason = "The thread of Modul SYS is aborted by the user";
			this->module_occurrence="SYS";
			break;
		case _sys_system_modules::FPL_SYS:
			this->reason = "The thread of Modul FPL is aborted by the user";
			this->module_occurrence="FPL";
			break;
		case _sys_system_modules::HYD_SYS:
			this->reason = "The thread of Modul HYD is aborted by the user";
			this->module_occurrence="HYD";
			break;
		case _sys_system_modules::MADM_SYS:
			this->reason = "The thread of Modul MADM is aborted by the user";
			this->module_occurrence="MADM";
			break;
		case _sys_system_modules::DAM_SYS:
			this->reason = "The thread of Modul DAM is aborted by the user";
			this->module_occurrence="DAM";
			break;
		case _sys_system_modules::RISK_SYS:
			this->reason = "The thread of Modul RISK is aborted by the user";
			this->module_occurrence="RISK";
			break;
		default:
			this->reason = "The thread of Modul UNKNOWN is aborted by the user";
			this->module_occurrence="UNKNOWN";
	}

}
//output the msg 
void Error::output_msg(const int error_occur){

	if(this->user_aborted_exception==false){
		//output to the file/gui where the error occur
		this->output_error_occurrence(error_occur);

		//output to the exception outputs
		ostringstream cout;
		cout << "################################################" << endl;
		cout << "ERROR"<<endl;
		//time of error
		cout << "DATE/TIME              " << this->time_err;
		//number of error
		cout << "NUMBER                 " << this->counter_output << endl;
		//module occurrence
		cout << "MODULE                 " << this->module_occurrence << endl;
		//number erro module
		cout << "ERROR IN MODULE        " << this->current_exception_number_module <<endl;
		//error type
		cout << "TYPE                   " << this->error_type_translation();
		//further information
		cout << "REASON                 ";
		_Sys_Exception::insert_tabs_after_endl(&this->reason);
		cout << this->reason << endl;
		cout << "CODE PLACE             "<< this->place << endl;
		cout << "APPLICATION PLACE      "<< this->occurence << endl;
		cout << "HELP                   ";
		_Sys_Exception::insert_tabs_after_endl(&this->help);
		cout << this->help << endl;
		cout << "SECOND INFOS           ";
		if(secundary_info.empty()){
			cout << "No secondary information available " << endl;
		}
		else{
			_Sys_Exception::insert_tabs_after_endl(&this->secundary_info);
			cout << this->secundary_info;
		}
		
		//fatal error or non fatal error
		if(this->fatal_flag){
				cout << "A fatal error is occured!" << endl;
				cout << "################################################" << endl;
				Sys_Exception_Sender::self()->set_fatal_errortxt(cout.str().c_str());
		}
		else{
				cout << "A non fatal error!" << endl;
				cout << "################################################" << endl;
		}
		//final output to excep output
		Sys_Common_Output::output_excep->output_txt(cout.str(),false);
		Sys_Common_Output::output_excep->reset_prefix_was_outputed();
		Error::set_exception_tooltip();
		//emit the error number
		Sys_Exception_Sender::self()->set_error_number(this->counter_output, Error::exception_tooltip.c_str());
	}
	else{

		//output to the exception outputs
		ostringstream cout;
		cout << "___________________________________" << endl;
		cout << "USERSTOP"<<endl;
		//time of error
		cout << "DATE/TIME              " << this->time_err;
		//module occurrence
		cout << "MODULE                 " << this->module_occurrence << endl;
		//further information
		cout << "REASON                 " << this->reason<<endl;
		cout << "___________________________________" << endl;
		this->output_msg2moduldisplay(cout.str());
	}
}
//output total number of errors
void Error::output_total(void){
	ostringstream buff;
	int total= Error::error_counter_sys+Error::error_counter_fpl+Error::error_counter_hyd+
		Error::error_counter_dam+Error::error_counter_risk+Error::error_counter_alt+Error::error_counter_cost+
		Error::error_counter_madm;
	buff<<"________________________"<<endl;
	buff<<"Total numbers of errors:  " << total << endl;
	if(Error::error_counter_sys!=0){
		buff << " " <<sys_label::str_sys << "  " <<Error::error_counter_sys<<endl;
	}
	if(Error::error_counter_fpl!=0){
		buff << " " <<sys_label::str_fpl << "  " <<Error::error_counter_fpl<<endl;
	}
	if(Error::error_counter_hyd!=0){
		buff << " " <<sys_label::str_hyd << "  " <<Error::error_counter_hyd<<endl;
	}
	if(Error::error_counter_dam!=0){
		buff << " " <<sys_label::str_dam << "  " <<Error::error_counter_dam<<endl;
	}
	if(Error::error_counter_risk!=0){
		buff << " " <<sys_label::str_risk << " " <<Error::error_counter_risk<<endl;
	}
	if(Error::error_counter_alt!=0){
		buff << " " <<sys_label::str_alt << "  " <<Error::error_counter_alt<< endl;
	}
	if(Error::error_counter_cost!=0){
		buff << " " <<sys_label::str_cost << " " <<Error::error_counter_cost<< endl;
	}
	if(Error::error_counter_madm!=0){
		buff << " " <<sys_label::str_madm << " " <<Error::error_counter_madm<< endl;
	}

	Sys_Common_Output::output_excep->output_txt(&buff,false);
}
//reset counter (static)
void Error::reset_counter(void){
	Error::error_counter_fpl=0;
	Error::error_counter_sys=0;
	Error::error_counter_hyd=0;
	Error::error_counter_madm=0;
	Error::error_counter_dam=0;
	Error::error_counter_cost=0;
	Error::error_counter_alt=0;
	Error::error_counter_risk=0;


	Error::set_exception_tooltip();

	//emit the error number
	Sys_Exception_Sender::self()->set_error_number(0, Error::exception_tooltip.c_str());
}
//give the fatal error flag
bool Error::give_fatal_flag(void){
	return this->fatal_flag;
}
//Set the fatal error flag
void Error::set_fatal_flag(const bool flag){
	this->fatal_flag=flag;
}
//Get the flag that is a user aborted exception; it is handled and throwed like an error
bool Error::get_user_aborted_exception(void){
	return this->user_aborted_exception;
}
//Get the number of hydraulic errors (static)
int Error::get_number_hyd_errors(void){
	return error_counter_hyd;
}
//Get the number of system errors (static)
int Error::get_number_sys_errors(void){
	return error_counter_sys;
}
//Get the number of damage errors (static)
int Error::get_number_dam_errors(void){
	return error_counter_dam;
}
//Get the number of fpl errors (static)
int Error::get_number_fpl_errors(void){
	return error_counter_fpl;
}
//Get the number of madm errors (static)
int Error::get_number_madm_errors(void){
	return error_counter_madm;
}
//Get the number of risk errors (static)
int Error::get_number_risk_errors(void){
	return error_counter_risk;
}
//Get the number of cost errors (static)
int Error::get_number_cost_errors(void){
	return error_counter_cost;
}
//Get the number of alt errors (static)
int Error::get_number_alt_errors(void){
	return error_counter_alt;
}
//Set the error number of the moduls to the given structure (static)
void Error::get_number_errors_moduls(_sys_error_module *errors){
	errors->dam_error=Error::error_counter_dam;
	errors->fpl_error=Error::error_counter_fpl;
	errors->hyd_error=Error::error_counter_hyd;
	errors->risk_error=Error::error_counter_risk;
	errors->sys_error=Error::error_counter_sys;
	errors->madm_error=Error::error_counter_madm;
	errors->cost_error=Error::error_counter_cost;
	errors->alt_error=Error::error_counter_alt;
	errors->total_error=0;
}
//Set the difference of the error number of the moduls to the given structure (static)
void Error::get_diff_number_errors_moduls(_sys_error_module *errors){
	errors->dam_error=Error::error_counter_dam-errors->dam_error;
	errors->fpl_error=Error::error_counter_fpl-errors->fpl_error;
	errors->hyd_error=Error::error_counter_hyd-errors->hyd_error;
	errors->risk_error=Error::error_counter_risk-errors->risk_error;
	errors->sys_error=Error::error_counter_sys-errors->sys_error;
	errors->madm_error=Error::error_counter_madm-errors->madm_error;
	errors->cost_error=Error::error_counter_cost-errors->cost_error;
	errors->alt_error=Error::error_counter_alt-errors->alt_error;
	errors->total_error=errors->dam_error+errors->fpl_error+errors->hyd_error+
		errors->risk_error+errors->sys_error+errors->madm_error+errors->cost_error+errors->alt_error;
}
//_______________________________________
//privat
//decide which type of error
string Error::error_type_translation(void){
	ostringstream cout;
	switch (type){
		case 1:
			cout << "File Input Error " << endl;
			break;
		case 2:
			cout << "Database Input Error " << endl;
			break;
		case 3:
			cout << "Variable Error " << endl;
			break;
		case 4:
			cout << "Database Error " << endl;
			break;
		case 5:
			cout << "File Error " << endl;
			break;
		case 6:
			cout << "Code Error " << endl;
			break;
		case 7:
			cout << "Random Generator Error " << endl;
			break;
		case 8:
			cout << "Table File Error " << endl;
			break;
		case 9:
			cout << "Table/Query Error " << endl;
			break;
		case 10:
			cout << "Dynamic Memory Error " << endl;
			break;
		case 11:
			cout << "Geometrie Error" << endl;
			break;
		case 12:
			cout << "Fault-tree Error" << endl;
			break;
		case 13:
			cout << "River Tables Error" << endl;
			break;
		case 14:
			cout << "River Profile Error" << endl;
			break;
		case 15:
			cout << "River Model Error" << endl;
			break;
		case 16:
			cout << "Coupling Error" << endl;
			break;
		case 17:
			cout << "Solver Error" << endl;
			break;
		case 18:
			cout << "Floodplain Model Error" << endl;
			break;
		case 19:
			cout << "Implementation Error" << endl;
			break;
		case 20:
			cout << "Hydraulic System Error" << endl;
			break;
		case 21:
			cout << "Economic Damage Error" << endl;
			break;
		case 22:
			cout << "People2risk Damage Error" << endl;
			break;
		case 23:
			cout << "Ecological Damage Error" << endl;
			break;
		case 24:
			cout << "Damage Calculation Error" << endl;
			break;
		case 25:
			cout << "Psycho-social Damage Error" << endl;
			break;
		case 26:
			cout << "Project Error" << endl;
			break;
		case 27:
			cout << "FPL-Section Error" << endl;
			break;
		case 28:
			cout << "FPL2HYD connection Error" << endl;
			break;
		case 29:
			cout << "RISK scenario Error" << endl;
			break;
		case 30:
			cout << "MADM analysis Error" << endl;
			break;
		case 31:
			cout << "ALT implementation Error" << endl;
			break;
		case 32:
			cout << "Slope slice table error" << endl;
			break;
		case 33:
			cout << "Resources error" << endl;
			break;
		case 34:
			cout << "CI-system error" << endl;
			break;
		case 35:
			cout << "GPU Solver error" << endl;
			break;
		default:
			cout << "Unknown Error " << endl;

	}
	return cout.str();
}
//output where the error occurrs
void Error::output_error_occurrence(const int error_occur){
	//output to the outputs where the error occurs
	ostringstream cout1;
	cout1 << "################################################" << endl;
	
	switch (error_occur){
			case 0://system error
				this->error_counter_sys++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_system->get_totalprefix();
				Sys_Common_Output::output_system->output_txt(cout1.str(),false);
				Sys_Common_Output::output_system->reset_userprefix();
				this->module_occurrence="SYS: Check SYS-logfile, SYS-display";
				this->current_exception_number_module=this->error_counter_sys;
				break;
			case 1://fpl_calc error
				this->error_counter_fpl++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_fpl->get_totalprefix();
				Sys_Common_Output::output_fpl->output_txt(cout1.str(),false);
				Sys_Common_Output::output_fpl->reset_userprefix();
				this->module_occurrence="FPL: Check FPL-logfile, FPL-display";
				this->current_exception_number_module=this->error_counter_fpl;
				break;
			case 2://system_hyd
				this->error_counter_hyd++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_hyd->get_totalprefix();
				Sys_Common_Output::output_hyd->output_txt(cout1.str(),false);
				Sys_Common_Output::output_hyd->reset_userprefix();
				this->module_occurrence="HYD: Check HYD-logfile, HYD-display";
				this->current_exception_number_module=this->error_counter_hyd;
				break;
			case 3://system_madm
				this->error_counter_madm++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_madm->get_totalprefix();
				Sys_Common_Output::output_madm->output_txt(cout1.str(),false);
				Sys_Common_Output::output_madm->reset_userprefix();
				this->module_occurrence="MADM: Check MADM-logfile, MADM-display";
				this->current_exception_number_module=this->error_counter_madm;
				break;
			case 4://system_dam
				this->error_counter_dam++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_dam->get_totalprefix();
				Sys_Common_Output::output_dam->output_txt(cout1.str(),false);
				Sys_Common_Output::output_dam->reset_userprefix();
				this->module_occurrence="DAM: Check DAM-logfile, DAM-display";
				this->current_exception_number_module=this->error_counter_dam;
				break;
			case 5://system_risk
				this->error_counter_risk++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_risk->get_totalprefix();
				Sys_Common_Output::output_risk->output_txt(cout1.str(),false);
				Sys_Common_Output::output_risk->reset_userprefix();
				this->module_occurrence="RISK: Check RISK-logfile, RISK-display";
				this->current_exception_number_module=this->error_counter_risk;
				break;
			case 6://system_alt
				this->error_counter_alt++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_alt->get_totalprefix();
				Sys_Common_Output::output_alt->output_txt(cout1.str(),false);
				Sys_Common_Output::output_alt->reset_userprefix();
				this->module_occurrence="ALT: Check ALT-logfile, ALT-display";
				this->current_exception_number_module=this->error_counter_alt;
				break;
			case 7://system_cost
				this->error_counter_cost++;
				this->count_total_error();
				//fatal error or non fatal error
				if(this->fatal_flag){
						cout1 << "A Fatal error is occured!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;	
				}
				else{
						cout1 << "A non Fatal error!" << endl;
						cout1 << "Check the exception output for Errornumber " << this->counter_output << endl;
						cout1 << "################################################" << endl;
				}
				this->occurence=Sys_Common_Output::output_cost->get_totalprefix();
				Sys_Common_Output::output_cost->output_txt(cout1.str(),false);
				Sys_Common_Output::output_cost->reset_userprefix();
				this->module_occurrence="COST: Check COST-logfile, COST-display";
				this->current_exception_number_module=this->error_counter_cost;
				break;
			default:
				break;
	}
	
}
//Set the flag that is a user aborted exception; it is handled and throwed like an error
void Error::set_user_aborted_exception(void){
	this->user_aborted_exception=true;
}
//Sum up the errors of the modules to the total number of error
void Error::count_total_error(void){
	this->counter_output=Error::error_counter_sys+Error::error_counter_fpl
		+Error::error_counter_hyd+Error::error_counter_madm
		+Error::error_counter_dam+Error::error_counter_risk+ 
		+Error::error_counter_cost+Error::error_counter_alt;
}
//Set the exception tooltip for the status bar (static)
void Error::set_exception_tooltip(void){
	string buffer;
	ostringstream buff;
	if(Error::error_counter_sys!=0){
		buff << sys_label::str_sys << "  " <<Error::error_counter_sys<<endl;
	}
	if(Error::error_counter_fpl!=0){
		buff << sys_label::str_fpl << "  " <<Error::error_counter_fpl<<endl;
	}
	if(Error::error_counter_hyd!=0){
		buff << sys_label::str_hyd << "  " <<Error::error_counter_hyd<<endl;
	}
	if(Error::error_counter_dam!=0){
		buff << sys_label::str_dam << "  " <<Error::error_counter_dam<<endl;
	}
	if(Error::error_counter_risk!=0){
		buff << sys_label::str_risk << " " <<Error::error_counter_risk<<endl;
	}
	if(Error::error_counter_alt!=0){
		buff << sys_label::str_alt << "  " <<Error::error_counter_alt<< endl;
	}
	if(Error::error_counter_cost!=0){
		buff << sys_label::str_cost << " " <<Error::error_counter_cost<< endl;
	}
	if(Error::error_counter_madm!=0){
		buff << sys_label::str_madm << " " <<Error::error_counter_madm<< endl;
	}


	buffer=buff.str();
	int pos=0;
	pos=buffer.rfind("\n");

	if(pos>0){
		buffer=buffer.erase(pos);
	}

	Error::exception_tooltip = buffer;
}