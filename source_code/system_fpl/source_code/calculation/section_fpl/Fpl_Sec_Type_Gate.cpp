#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Sec_Type_Gate.h"

//init static members
Tables *Fpl_Sec_Type_Gate::result_table=NULL;

//Default constructor
Fpl_Sec_Type_Gate::Fpl_Sec_Type_Gate(void){

	//common
	Sys_Memory_Count::self()->add_mem((sizeof(Fpl_Sec_Type_Gate)-0*sizeof(Fpl_Random_Variables)), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Sec_Type_Gate::~Fpl_Sec_Type_Gate(void){

	//common
	Sys_Memory_Count::self()->minus_mem((sizeof(Fpl_Sec_Type_Gate)-0*sizeof(Fpl_Random_Variables)), _sys_system_modules::FPL_SYS);//count the memory

}
//______________
//public
//Set the input of the dike section (geometrie, variables, faulttree) per database
void Fpl_Sec_Type_Gate::set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database){

}
//Read in the fpl-section type from file
void Fpl_Sec_Type_Gate::read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim){

}
//Read the mechanisms of the fault-tree per file
void Fpl_Sec_Type_Gate::read_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){

}
//Check which mechanisms of the fault-tree are required per file
void Fpl_Sec_Type_Gate::check_mechanism_faulttree_perfile(QFile *ifile, int *line_counter){

}
//Read the waterside geometry of the section per file
void Fpl_Sec_Type_Gate::read_waterside_geometry(QFile *ifile, int *line_counter){

}
//Read the landside geometry of the section per file
void Fpl_Sec_Type_Gate::read_landside_geometry(QFile *ifile, int *line_counter){

}
//Transfer the section type data to database, e.g. geometry, random variables etc.
void Fpl_Sec_Type_Gate::transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database){

}
//Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
int Fpl_Sec_Type_Gate::make_faulttree(const bool random_calculation){
	int buff_failure=0;
	if(random_calculation==true){
		//reset the random generated flag
		this->reset_random_flag();
	}

	//dike section type variables
	//calculate the waterlevel
	this->current_waterlevel=this->waterlevel.calculate_variables(random_calculation).end_result;


	return buff_failure;
}
//Check the statistics of the random variables used in the section type
void Fpl_Sec_Type_Gate::check_statistic(void){

}
//Output the statistics of the random variables used in the section type to display/console
void Fpl_Sec_Type_Gate::output_statistic(void){

}
//Reset the statistic important values
void Fpl_Sec_Type_Gate::reset_statistics(void){

}
//Output the section members to display/console
void Fpl_Sec_Type_Gate::output_member(void){

}
//Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
void Fpl_Sec_Type_Gate::output_reliability(string output_folder, const int sec_id, const string sec_name){

}
//Output the geometry to tecplot
void Fpl_Sec_Type_Gate::output_geometry2tecplot(ofstream *output){

}
//Output the geometry to paraview
void Fpl_Sec_Type_Gate::output_geometry2paraview(ofstream *output) {


}
//Output the geometry to excel
void Fpl_Sec_Type_Gate::output_geometry2excel(ofstream *output) {

}
//Output result members of the mechanisms to database table
void Fpl_Sec_Type_Gate::output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int counter_mc_sim){

}

//Output the deterministic results to tecplot
void Fpl_Sec_Type_Gate::output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the deterministic results to paraview
void Fpl_Sec_Type_Gate::output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the deterministic results to excel
void Fpl_Sec_Type_Gate::output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}

//Output the MC results to tecplot
void Fpl_Sec_Type_Gate::output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the MC results to paraview
void Fpl_Sec_Type_Gate::output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the MC results to excel
void Fpl_Sec_Type_Gate::output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}

//Output the FRC results to tecplot
void Fpl_Sec_Type_Gate::output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the FRC results to paraview
void Fpl_Sec_Type_Gate::output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the FRC results to excel
void Fpl_Sec_Type_Gate::output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Create the database table for the results of the mechanism of the dike section (static)
void create_result_table(QSqlDatabase *ptr_database){

}
//Delete all data in the database table for the results of the mechanism of the dike section  (static)
void Fpl_Sec_Type_Gate::delete_data_in_result_table(QSqlDatabase *ptr_database){

}
//Delete a data set in the database table for the results of the mechanism of the dike section for a specific system state (static)
void Fpl_Sec_Type_Gate::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id){

}
//Delete a data set in the database table for the results of the mechanism of the dike section (static)
void Fpl_Sec_Type_Gate::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type){

}
//Set the database table for the results of the mechanism of the dike section : it sets the table name and the name of the columns and allocate them (static)
void Fpl_Sec_Type_Gate::set_result_table(QSqlDatabase *ptr_database){

}
//Close and delete the database table for the results of the mechanism of the dike section  (static)
void Fpl_Sec_Type_Gate::close_result_table(void){

}
//Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state (static)
void Fpl_Sec_Type_Gate::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){

}
//Switch the applied-flag for the results of the mechanism of the dike section in the database table for a defined system state and id (static)
void Fpl_Sec_Type_Gate::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){

}
//Check and set the section geometry out of the read in geometry members
void Fpl_Sec_Type_Gate::check_set_geometry(void){

}
//_______________
//private
//Set the general parameters of the section to the mechanisms
void Fpl_Sec_Type_Gate::set_general(void){

}
//Set the random variables of this section and the relevant mechanisms from database table
void Fpl_Sec_Type_Gate::set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id){

}
//Set the geometry of the section and their surrounding via a database table
void Fpl_Sec_Type_Gate::set_geometrie(QSqlDatabase *ptr_database, const int section_id){

}
//Set control parameters of the mechanisms
void Fpl_Sec_Type_Gate::set_control_parameter_mechanism(QSqlDatabase *ptr_database, const int section_id, const bool frc_sim){

}
//Initialize the mechanisms
void Fpl_Sec_Type_Gate::init_mechanism(const int section_id){

}
//Set the fault tree for this section type via a database table
void Fpl_Sec_Type_Gate::init_fault_tree(QSqlDatabase *ptr_database){

}
//Decide and allocate the mechanism, which should by applied during calculation
void Fpl_Sec_Type_Gate::decide_mechanism(const string mechanism_name, const bool application_flag, const bool applied_in_tree){

}
//Check the initialized fault-tree
void Fpl_Sec_Type_Gate::check_fault_tree(void){

}
//Reset the flags for the random generation
void Fpl_Sec_Type_Gate::reset_random_flag(void){

}
//Set error(s)
Error Fpl_Sec_Type_Gate::set_error(const int err_type){
		string place="Fpl_Sec_Type_Gate::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="Can not read in the fault-tree from the database table";
			help="Check the table";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://bad_alloc
			place.append("allocate_piping_sell_mechanism(void)");
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
//Set warning(s)
Warning Fpl_Sec_Type_Gate::set_warning(const int warn_type){
		string place="Fpl_Sec_Type_Gate::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="No mechanism is set for the section type dike";
			help="Check the fault tree for the section type";
			reaction="No reliabilty calculation required";
			type=6;
			break;
		case 1://results datas can not submitted
			place.append("output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id)");
			reason="Can not submit the mechanism result data to the database";
			help="Check the database";
			type=2;
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