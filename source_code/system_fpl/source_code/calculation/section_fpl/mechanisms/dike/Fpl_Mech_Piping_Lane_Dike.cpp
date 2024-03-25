#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Piping_Lane_Dike.h"

//Default constructor
Fpl_Mech_Piping_Lane_Dike::Fpl_Mech_Piping_Lane_Dike(void){


	this->buff_stress=0.0;
	this->buff_res=0.0;
	this->failure=0;

	this->resistance.set_unit(label::m);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Piping_Lane_Dike)-sizeof(Fpl_Random_Variables)*2,_sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Mech_Piping_Lane_Dike::~Fpl_Mech_Piping_Lane_Dike(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Piping_Lane_Dike)-sizeof(Fpl_Random_Variables)*2,_sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Piping_Lane_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_piping_lane, true, ptr_database, fpl_label::mech_piping_lane, fpl_label::sec_dike, 0, false);
		this->resistance.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Piping_Lane_Dike::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->lane_factor.set_input(id, section_id,fpl_label::lane_factor, false, ptr_database, fpl_label::mech_piping_lane, fpl_label::sec_dike, 0, false);
		this->lane_factor.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Piping_Lane_Dike::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "PIP_LANE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{
		this->lane_factor.set_input(ifile, line_counter,fpl_label::lane_factor,false, fpl_label::end_mech_pip_lane);
		this->lane_factor.check_given_boundaries(15.0,0.5);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		//result
		this->resistance.set_input(ifile, line_counter,fpl_label::res_piping_lane,true, fpl_label::end_mech_pip_lane);
		this->resistance.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	cout << "Input of variable members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
///Transfer the piping-mechanism (lane) data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Piping_Lane_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "PIP_LANE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->lane_factor.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_lane, fpl_label::sec_dike);
	
	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_piping_lane, fpl_label::sec_dike);


	Sys_Common_Output::output_fpl->rewind_userprefix();

}
//Output members of the mechanism to display/console
void Fpl_Mech_Piping_Lane_Dike::output_members(void){
	ostringstream prefix;
	prefix << "PIP_LANE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Lane..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->lane_factor.output_member();
	cout << "Result members mechanism piping after Lane..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Piping_Lane_Dike::check_statistic(void){
	this->lane_factor.check_statistic();
	this->resistance.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Piping_Lane_Dike::reset_statistics(void){
	this->lane_factor.reset_statistics();
	this->resistance.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Piping_Lane_Dike::output_statistic(void){
	ostringstream prefix;
	prefix << "PIP_LANE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Lane..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->lane_factor.output_statistic();
	cout << "Result members mechanism piping after Lane..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Piping_Lane_Dike::output_determ_results(void){
	ostringstream prefix;
	prefix << "PIP_LANE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism piping after Lane..." << endl;
	cout << " Lane factor               : "<< this->lane_factor.get_result().end_result << label::no_unit << endl;
	cout << "Result members mechanism piping after Lane..." << endl;
	cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::no_unit<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
int Fpl_Mech_Piping_Lane_Dike::calculate_mechanism(const bool random_calculation, const double waterlevel, const double leak_length){
	this->failure=0;
	//reset the random flags
	this->reset_random_flag();

	this->buff_stress=0.0;
	this->buff_res=0.0;

	if(waterlevel<=0.0){
		this->failure=0;
		return this->failure;
	}

	//stress
	this->buff_stress=waterlevel;
	//resistance
	this->buff_res=0.33333*leak_length/this->lane_factor.calculate_variables(random_calculation).end_result;

	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);

	if(this->resistance.get_result().end_result<=0.0){
		this->failure=1;
		return this->failure;
	}

	//check for failure
	if(this->buff_stress>=this->resistance.get_result().end_result){
		//failure
		this->failure=1;
	}
	else{
		this->failure=0;
	}


	return this->failure;

}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Piping_Lane_Dike::reset_random_flag(void){
	this->lane_factor.reset_random_generated();
	this->resistance.reset_random_generated();
}