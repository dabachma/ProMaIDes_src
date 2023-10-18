#include "Cost_Headers_Precompiled.h"
//#include "Cost_System.h"

//init static members
bool Cost_System::abort_thread_flag=false;


//Default destructor
Cost_System::Cost_System(void){

	this->thread_type=_cost_thread_type::cost_thread_unknown;
	this->number_error=0;
	this->number_warning_begin=0;
	this->number_warning=0;

	this->thread_is_started=false;


	Sys_Memory_Count::self()->add_mem(sizeof(Cost_System),_sys_system_modules::COST_SYS);//count the memory
}
//Default destructor
Cost_System::~Cost_System(void){

	//close database
	if(this->qsqldatabase.isOpen()){
		this->qsqldatabase.close();
	}

	Sys_Memory_Count::self()->minus_mem(sizeof(Cost_System),_sys_system_modules::COST_SYS);//count the memory
}
//______
//public
//Set the thread type for specifiying the task of the thread
void Cost_System::set_thread_type(_cost_thread_type type){
	this->thread_type=type;
}
//Set a pointer to the database; the copy of the database is made in this method
void Cost_System::set_ptr2database(QSqlDatabase *ptr_database){
	this->qsqldatabase=QSqlDatabase::cloneDatabase(*ptr_database, sys_label::str_cost.c_str());
	this->qsqldatabase.open();
}
//Get the thread type
_cost_thread_type Cost_System::get_thread_type(void){
	return this->thread_type;
}
//Create a whole set of cost database tables
void Cost_System::create_cost_database_tables(void){
	ostringstream cout;
	cout << "Create COST system tables..." << endl ;
	Sys_Common_Output::output_cost->output_txt(&cout);
	this->set_start_warnings_number();
	try{
		Cost_Ecn_Cost_System::create_table_cost(&this->qsqldatabase);
	}
	catch(Error msg){
		this->number_error++;
		msg.output_msg(7);
		
	}
	this->set_warning_number();
	cout << "Creation of the COST database tables is finished" << endl ;
	Sys_Common_Output::output_cost->output_txt(&cout);
	this->output_error_statistic();
}
//Check all madm database tables, for their existence in the database and their declaration in the database table-file
void Cost_System::check_cost_database_tables(void){
	ostringstream cout;
	cout << "Check COST system tables..." << endl ;
	Sys_Common_Output::output_cost->output_txt(&cout,false, false);
	this->set_start_warnings_number();
	try{
		cout << "Check database table of economical cost..." << endl ;
		Sys_Common_Output::output_cost->output_txt(&cout,false, false);
		Cost_System::check_stop_thread_flag();
		Cost_Ecn_Cost_System::set_table_cost(&this->qsqldatabase);

	}
	catch(Error msg){
		this->number_error++;
		ostringstream info;
		info << "Check if the COST tables are created!"<< endl;
		msg.make_second_info(info.str());
		msg.set_fatal_flag(false);
		msg.output_msg(7);
		this->output_error_statistic();
	}
	this->set_warning_number();
	cout << "Check of the COST database tables is finished" << endl ;
	Sys_Common_Output::output_cost->output_txt(&cout,false, false);
	
}
//Delete the data of all madm database tables
void Cost_System::delete_data_cost_database_tables(void){
	try{
		Cost_Ecn_Cost_System::delete_data_in_table_cost(&this->qsqldatabase);
	}
	catch(Error msg){
		msg.output_msg(7);
	}
}
//Close all madm database tables (static)
void Cost_System::close_cost_database_tables(void){
	Cost_Ecn_Cost_System::close_table_cost();
}
//Set the stop thread flag (static)
void Cost_System::set_stop_thread_flag(const bool flag){
	Cost_System::abort_thread_flag=flag;
}
//Check the stop thread flag (static)
void Cost_System::check_stop_thread_flag(void){
	if(Cost_System::abort_thread_flag==true){
		Error msg;
		msg.set_msg(_sys_system_modules::COST_SYS);
		throw msg;
	}
}
//Get the stop thread flag (static)
bool Cost_System::get_stop_thread_flag(void){
	return Cost_System::abort_thread_flag;
}
//Run the thread; the type of thread, which will be performed, has to be set before with set_thread_type( _cost_thread_type type)
void Cost_System::run(void){
/** \see set_thread_type(_cost_thread_type type), _cost_thread_type
*/

	this->thread_is_started=true;
	try{
		switch(this->thread_type){
			case(_cost_thread_type::cost_create_tab):
				this->create_cost_database_tables();
				break;
			case(_cost_thread_type::cost_check_tab):
				this->check_cost_database_tables();
				break;			
			default:
				Warning msg=this->set_warning(0);
				msg.output_msg(3);
		}
	}
	catch(Error msg){
		this->thread_is_started=false;
		msg.output_msg(7);
	}
	this->thread_is_started=false;
}
//Ask economical cost data per dialog (user-defined)
bool Cost_System::ask_ecn_cost_per_dia(QWidget *parent){
	
	return this->ecn_system.set_cost_per_dialog(parent, &this->qsqldatabase, this->system_id);
}
//Get the number of errors
int Cost_System::get_number_error(void){
	return this->number_error;
}
//Get the flag if the thread has started
bool Cost_System::get_thread_has_started(void){
	return this->thread_is_started;
}
//_______
//private
//Set the number of warnings before an action
void Cost_System::set_start_warnings_number(void){
	this->number_warning_begin=Warning::get_number_cost_warnings();
}
//Set the warning number, which occured during an action
void Cost_System::set_warning_number(void){
	this->number_warning=Warning::get_number_cost_warnings()-this->number_warning_begin;
}
//Get the warning number, which occurred during an action
int Cost_System::get_occured_warning(void){
	return this->number_warning;
}
//Output the error statistic of the cost system
void Cost_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "STA"<<"> ";
	Sys_Common_Output::output_cost->set_userprefix(prefix.str());
	cout << "Error statistics of the COST-system..." << endl ;
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)             :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)           :" << W(3) << this->number_warning <<endl;
	if(Cost_System::abort_thread_flag==true){
		cout << " User has aborted the action " << endl;
	}
	Sys_Common_Output::output_cost->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_cost->rewind_userprefix();
}
//Set warning(s)
Warning Cost_System::set_warning(const int warn_type){
	string place="Cost_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the type of thread is not specified 
			place.append("run(void)") ;
			reason="The thread type is unknown";
			reaction="No thread is launched";
			help= "Check the source code";
			type=5;	
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
}
//Set error(s)
Error Cost_System::set_error(const int ){
	string place="Cost_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	//switch (err_type){
	//	//case 0://bad alloc
	//	//	place.append("allocate_file_names(void)");
	//	//	reason="Can not allocate the memory";
	//	//	help="Check the memory";
	//	//	type=10;
	//	//	break;
	//	default:
	//		place.append("set_error(const int err_type)");
	//		reason ="Unknown flag!";
	//		help="Check the flags";
	//		type=6;
	//}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
