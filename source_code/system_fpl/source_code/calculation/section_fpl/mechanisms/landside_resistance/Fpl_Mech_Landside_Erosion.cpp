#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Mech_Landside_Erosion.h"

//Default constructor
Fpl_Mech_Landside_Erosion::Fpl_Mech_Landside_Erosion(void){

	this->failure=false;
	this->buff_res=0.0;
	this->buff_stress=0.0;

	this->resistance.set_unit(label::qm_per_secm);
	this->stress.set_unit(label::qm_per_secm);
	this->critical_discharge.set_unit(label::qm_per_secm);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Landside_Erosion)-sizeof(Fpl_Random_Variables)*3,_sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Mech_Landside_Erosion::~Fpl_Mech_Landside_Erosion(void){


	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Landside_Erosion)-sizeof(Fpl_Random_Variables)*3,_sys_system_modules::FPL_SYS);//count the memory

}
//_________
//public
//Set the random variables of the result members
void Fpl_Mech_Landside_Erosion::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->resistance.set_input(id, section_id,fpl_label::res_landside_discharge, true, ptr_database, fpl_label::mech_erosion_landside, fpl_label::sec_all, 0, false);
		this->resistance.set_distribution_types(ptr_database);

		this->stress.set_input(id, section_id,fpl_label::stress_landside_discharge, true, ptr_database, fpl_label::mech_erosion_landside, fpl_label::sec_all, 0, false);
		this->stress.set_distribution_types(ptr_database);

	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Landside_Erosion::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->critical_discharge.set_input(id, section_id,fpl_label::crit_landside_discharge, false, ptr_database, fpl_label::mech_erosion_landside, fpl_label::sec_all, 0, false);
		this->critical_discharge.set_distribution_types(ptr_database);
		
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Landside_Erosion::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "LANDSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{

		this->critical_discharge.set_input(ifile, line_counter,fpl_label::crit_landside_discharge, false, fpl_label::end_mech_landside_ero);
		this->critical_discharge.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		//results
		this->resistance.set_input(ifile, line_counter,fpl_label::res_landside_discharge, true, fpl_label::end_mech_landside_ero);
		this->resistance.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		this->stress.set_input(ifile, line_counter,fpl_label::stress_landside_discharge, true, fpl_label::end_mech_landside_ero);
		this->stress.check_positiv_zero_variable();
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
//Transfer the bursting-mechanism (DIN 19712) data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Landside_Erosion::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "LANDSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->critical_discharge.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_landside, fpl_label::sec_all);

	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->resistance.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_landside, fpl_label::sec_all);
	this->stress.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_erosion_landside, fpl_label::sec_all);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members of the mechanism to display/console
void Fpl_Mech_Landside_Erosion::output_members(void){
	ostringstream prefix;
	prefix << "LANDSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism erosion of the landside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->critical_discharge.output_member();
	
	cout << "Result members mechanism erosion of the landside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_member();
	this->stress.output_member();
	

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Check the statistic of the random variables of the mechanism
void Fpl_Mech_Landside_Erosion::check_statistic(void){
	this->critical_discharge.check_statistic();
	this->resistance.check_statistic();
	this->stress.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Landside_Erosion::reset_statistics(void){
	this->critical_discharge.reset_statistics();
	this->resistance.reset_statistics();
	this->stress.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables of the mechanism to display/console
void Fpl_Mech_Landside_Erosion::output_statistic(void){
	ostringstream prefix;
	prefix << "LANDSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism erosion of the landside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->critical_discharge.output_statistic();
	

	cout << "Result members mechanism erosion of the landside slope..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->resistance.output_statistic();
	this->stress.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the results of a deterministic calculation to display/console
void Fpl_Mech_Landside_Erosion::output_determ_results(void){
	ostringstream prefix;
	prefix << "LANDSIDE_ERO> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results mechanism erosion of the landside slope..."<< endl;
	cout << " Critical discharge        : "<< this->critical_discharge.get_result().end_result<<label::qm_per_secm << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	cout << "Result members mechanism erosion of the landside slope..." << endl;
	cout << " Resistance                : "<< this->resistance.get_result().end_result<<label::qm_per_secm<<endl;
	cout << " Stress                    : "<< this->stress.get_result().end_result<<label::qm_per_secm<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is discharge over the landside slope
bool Fpl_Mech_Landside_Erosion::calculate_mechanism(const bool random_calculation, const double q_current){
	this->failure=false;
	//reset the random flags
	this->reset_random_flag();



	//resistance
	this->buff_res=this->critical_discharge.calculate_variables(random_calculation).end_result;

	
	//stress
	

	//set the results variables
	this->resistance.set_new_middle(this->buff_res);
	this->resistance.calculate_variables(random_calculation);
	this->stress.set_new_middle(q_current);
	this->stress.calculate_variables(random_calculation);

	//check for failure
	if(this->stress.get_result().end_result>=this->resistance.get_result().end_result){
		//failure
		this->failure=true;
	}
	else{
		this->failure=false;
	}

	return this->failure;
}
//__________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Landside_Erosion::reset_random_flag(void){
	this->critical_discharge.reset_random_generated();
	this->resistance.reset_random_generated();
	this->stress.reset_random_generated();
}
//Set warning(s)
Warning Fpl_Mech_Landside_Erosion::set_warning(const int warn_type){
		string place="Fpl_Mech_Landside_Erosion::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id, Fpl_Dike_Var_Materialzone *zone, const int number)");
			reason="No material variable zone is found for the density of the impermeable layer";
			help="Check the random variable and the material variable zone";
			reaction="No connection can be established between the material variable zone and the random variable; the results can be wrong";
			type=20;
			break;
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
Error Fpl_Mech_Landside_Erosion::set_error(const int err_type){
	string place="Fpl_Mech_Landside_Erosion::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_bursting_slices(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;

}