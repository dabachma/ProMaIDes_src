//#include "Fpl_Sec_Type_Highground.h"
#include "Fpl_Headers_Precompiled.h"
//init static members
Tables *Fpl_Sec_Type_Highground::result_table=NULL;

//Default constructor
Fpl_Sec_Type_Highground::Fpl_Sec_Type_Highground(void){
	this->overflow_event=NULL;
	this->waterlevel.set_unit(label::m);
	this->height.set_unit(label::m);
	//common
	Sys_Memory_Count::self()->add_mem((sizeof(Fpl_Sec_Type_Highground)-2*sizeof(Fpl_Random_Variables)), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Sec_Type_Highground::~Fpl_Sec_Type_Highground(void){
	this->delete_overflow();
	Sys_Memory_Count::self()->minus_mem((sizeof(Fpl_Sec_Type_Highground)-2*sizeof(Fpl_Random_Variables)), _sys_system_modules::FPL_SYS);//count the memory
}
//______________
//public
//Set the input of the dike section (geometrie, variables, faulttree)
void Fpl_Sec_Type_Highground::set_input(const int section_id,  const bool frc_sim, QSqlDatabase *ptr_database){
	//set the fault-tree per table and allocate the needed classes
	this->init_fault_tree(ptr_database);
	//read in the geometrie
	this->set_geometrie(ptr_database, section_id);
	//set the input for the varaibles which belongs to the section_type class
	this->set_variables(frc_sim, ptr_database, section_id);

}
//Transfer the section type data to database, e.g. geometry, random variables etc.
void Fpl_Sec_Type_Highground::transfer_sectiontype2database(const int section_id, QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Transfer the section type data of a (highground) to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	cout << "Transfer general random variables to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->waterlevel.transfer_rand2database(ptr_database, this->system_id, section_id,fpl_label::mech_all,fpl_label::sec_highground);
	this->height.transfer_rand2database(ptr_database, this->system_id, section_id,fpl_label::mech_overflow,fpl_label::sec_highground);

}
//Read in the fpl-section type from file
void Fpl_Sec_Type_Highground::read_section_type_per_file(QFile *ifile, int *line_counter, const bool frc_sim){
	ostringstream cout;
	cout << "Input the general data of the highground section..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	qint64 pos_file=0;
	int store_line=0;
	try{
		pos_file=this->find_start_block_file(ifile, line_counter, fpl_label::begin_sec_general, fpl_label::end_sec_general, true);
		store_line=*line_counter;
	}
	catch(Error msg){
		throw msg;
	}
	//read in the random variables general block
	try{
		if(frc_sim==false){
			this->waterlevel.set_input(ifile, line_counter, fpl_label::waterlevel, false, fpl_label::end_sec_general);
		}
		else{
			this->waterlevel.set_input(ifile, line_counter, fpl_label::waterlevel_frc, true, fpl_label::end_sec_general);	
		}
		this->waterlevel.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=store_line;
		
		this->height.set_input(ifile, line_counter, fpl_label::height, true , fpl_label::end_sec_general);
		this->height.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=store_line;
		//base point water is always 0,0...all is related to them!
		this->base_water.set_point_coordinate(0.0,0.0);
		//calculate the mean value of height
		this->height.set_new_middle(this->base_water.vertical_distance(this->crest_water));
	}
	catch(Error msg){
		throw msg;
	}

	cout << "Input of the general data of the highground section is finished"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
}
//Calculate the fault tree with random distributed variables (true) or with deterministic variables (false)
int Fpl_Sec_Type_Highground::make_faulttree(const bool random_calculation){
	if(random_calculation==true){
		//reset the random flag
		this->reset_random_flag();
		this->height.reset_random_generated();
	}
	double waterlevel_buff=waterlevel.calculate_variables(random_calculation).end_result;
	if(this->overflow_event!=NULL){
		if(overflow_event->get_2calc_flag()==true){
			return overflow_event->calc_mechanism(height.calculate_variables(random_calculation).end_result, waterlevel_buff);
		}
	}

	return 0;
}
//Check the statistics of the random variables used in the section type
void Fpl_Sec_Type_Highground::check_statistic(void){
	waterlevel.check_statistic();
	height.check_statistic();
}
//Output the statistics of the random variables used in the section type to display/console
void Fpl_Sec_Type_Highground::output_statistic(void){
	ostringstream cout;
	cout << "Statistics of Variables (highground)..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->waterlevel.output_statistic();
	this->height.output_statistic();
}
//Reset the statistic important values
void Fpl_Sec_Type_Highground::reset_statistics(void){
	this->waterlevel.reset_statistics();
	this->height.reset_statistics();
}
//Output the section members to display/console
void Fpl_Sec_Type_Highground::output_member(void){
	ostringstream cout;
	cout << "Geometrie members highground..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->base_water.output_members(&cout);
	this->crest_water->output_members(&cout);
	cout << "Variable members highground..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->waterlevel.output_member();
	this->height.output_member();
}
//Output the reliability of the fault tree mechanisms for a deterministic calculation to display/console
void Fpl_Sec_Type_Highground::output_reliability(string , const int , const string ){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RES"<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << "Deterministic results of the sectiontype highground..." << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),true);
	//general
	this->waterlevel.output_deterministic();
	this->height.output_deterministic();

	if(this->overflow_event!=NULL){
		if(this->overflow_event->get_2calc_flag()==true){
			this->overflow_event->output_reliabilty();
		}
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the geometry to tecplot
void Fpl_Sec_Type_Highground::output_geometry2tecplot(ofstream *output){
	//output the zone header
	*output << endl << "ZONE T= "<< "\" Cubature \" "<< " I = " << 4 << endl;


	*output << W(5) << P(4) << FORMAT_FIXED_REAL << -10.0 << W(12)<< 0.0 <<  endl;
	*output << W(12) << 0.0 << W(12)<< 0.0 <<  endl;
	*output << W(12) << 0.0 << W(12)<< this->height.get_deterministic_mid() <<  endl;
	*output << W(12) << 10.0 << W(12)<< this->height.get_deterministic_mid() <<  endl;


	output->flush();

}
//Output result members to database table
void Fpl_Sec_Type_Highground::output_result2table(QSqlDatabase *ptr_database, _fpl_simulation_type simulation_type, _sys_system_id id, const int section_id, const int ){

	try{
		//delete existing data and set table
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//evaluate the global identifier
	int id_glob=Fpl_Sec_Type_Highground::result_table->maximum_int_of_column(Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
			
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Sec_Type_Highground::result_table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::simulation_type) <<" , ";
	//data
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::waterlevel) <<" , ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::height) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << section_id << " , " ;
	query_string << id.area_state << " , " ;
	query_string << id.measure_nr<< " , " ;
	query_string << "'"<<Fpl_Mc_Sim::convert_sim_type2text(simulation_type)<<"'"<< " , " ;
	//data
	query_string << this->waterlevel.get_calculated_mean()<< " , " ;	
	query_string << this->height.get_calculated_mean()<< " ) " ;
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Fpl_Sec_Type_Highground::result_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Fpl section id            : " << section_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Output the deterministic results to tecplot
void Fpl_Sec_Type_Highground::output_determ_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the deterministic results to paraview
void Fpl_Sec_Type_Highground::output_determ_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the deterministic results to excel
void Fpl_Sec_Type_Highground::output_determ_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}

//Output the MC results to tecplot
void Fpl_Sec_Type_Highground::output_mc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the MC results to paraview
void Fpl_Sec_Type_Highground::output_mc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the MC results to excel
void Fpl_Sec_Type_Highground::output_mc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}

//Output the FRC results to tecplot
void Fpl_Sec_Type_Highground::output_frc_res2tecplot(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the FRC results to paraview
void Fpl_Sec_Type_Highground::output_frc_res2paraview(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Output the FRC results to excel
void Fpl_Sec_Type_Highground::output_frc_res2excel(ofstream *output, QSqlDatabase *ptr_database, _sys_system_id id, const int section_id) {

}
//Create the database table for the results of the mechanism of the highground section (static)
void Fpl_Sec_Type_Highground::create_result_table(QSqlDatabase *ptr_database){
		if(Fpl_Sec_Type_Highground::result_table==NULL){
			ostringstream cout;
			cout << "Create highground result database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_fpl_highground_result;
			const int num_col=8;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::areastate_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=label::measure_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=label::applied_flag;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";
			tab_col[4].key_flag=true;

			tab_col[5].name=fpl_label::simulation_type;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::waterlevel;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=fpl_label::height;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;
			

			try{
				Fpl_Sec_Type_Highground::result_table= new Tables();
				if(Fpl_Sec_Type_Highground::result_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Sec_Type_Highground::create_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Sec_Type_Highground::close_result_table();
				throw msg;
			}

		Fpl_Sec_Type_Highground::close_result_table();
	}
}
//Delete all data in the database table for the results of the mechanism of the highground section (static)
void Fpl_Sec_Type_Highground::delete_data_in_result_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Fpl_Sec_Type_Highground::result_table->delete_data_in_table(ptr_database);
}
//Delete a data set in the database table for the results of the mechanism of the highground section for a specific system state (static)
void Fpl_Sec_Type_Highground::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Highground::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Highground::delete_data_in_result_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Highground::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete a data set in the database table for the results of the mechanism of the highground section (static)
void Fpl_Sec_Type_Highground::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type){

	try{
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//the table is set (the name and the column names) and allocated
	if(sim_type==_fpl_simulation_type::sim_determ){
		//do not delete determnistic results
		return;
	}

	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Fpl_Sec_Type_Highground::result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::section_id) << " = " << section_id;
	query_string << " AND ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::simulation_type) << " = '" << Fpl_Mc_Sim::convert_sim_type2text(sim_type)<<"'";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Highground::delete_data_in_result_table(QSqlDatabase *ptr_database, const int section_id, const _sys_system_id id, const _fpl_simulation_type sim_type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Highground::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the results of the mechanism of the highground section : it sets the table name and the name of the columns and allocate them (static)
void Fpl_Sec_Type_Highground::set_result_table(QSqlDatabase *ptr_database){
	if(Fpl_Sec_Type_Highground::result_table==NULL){

		//make specific input for this class
		const string tab_id_name=fpl_label::tab_fpl_highground_result;
		string tab_col[8];

		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=fpl_label::simulation_type;
		tab_col[5]=fpl_label::waterlevel;
		tab_col[6]=fpl_label::height;
		tab_col[7]=label::applied_flag;



		try{
			Fpl_Sec_Type_Highground::result_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Sec_Type_Highground::result_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Sec_Type_Highground::set_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Sec_Type_Highground::close_result_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the results of the mechanism of the highground section (static)
void Fpl_Sec_Type_Highground::close_result_table(void){
	if(Fpl_Sec_Type_Highground::result_table!=NULL){
		delete Fpl_Sec_Type_Highground::result_table;
		Fpl_Sec_Type_Highground::result_table=NULL;
	}
}
//Switch the applied-flag for the results of the mechanism of the highground section in the database table for a defined system state
void Fpl_Sec_Type_Highground::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Highground::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Highground::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Highground::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Highground::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Highground::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the results of the mechanism of the highground section in the database table for a defined system state and id
void Fpl_Sec_Type_Highground::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Sec_Type_Highground::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Sec_Type_Highground::result_table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Sec_Type_Highground::result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Sec_Type_Highground::result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Highground::result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Sec_Type_Highground::result_table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Sec_Type_Highground::switch_applied_flag_result_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Sec_Type_Highground::result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//_________
//private
//Allocate the the overflow mechanism event
void Fpl_Sec_Type_Highground::allocate_overflow(void){
		try{
			this->overflow_event=new Fpl_Fail_Overflow_HiGround();
		}
		catch(bad_alloc&t){
			Error msg=this->set_error(2);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
}
//Delete the overflow mechanism event
void Fpl_Sec_Type_Highground::delete_overflow(void){
	if(this->overflow_event!=NULL){
		delete this->overflow_event;
		this->overflow_event=NULL;
	}
}
//Set the variable via a database table
void Fpl_Sec_Type_Highground::set_variables(const bool frc_sim, QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set general variables of the highground section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		//waterlevel is special (frc calculation or mc calc)
		if(frc_sim==false){
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel,false, ptr_database, fpl_label::mech_all,fpl_label::sec_highground,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}
		else{
			this->waterlevel.set_input(this->system_id, section_id, fpl_label::waterlevel_frc,true, ptr_database, fpl_label::mech_all,fpl_label::sec_highground,0);
			this->waterlevel.set_distribution_types(ptr_database);
		}

		//other members
		this->height.set_input(this->system_id, section_id, fpl_label::height,true, ptr_database, fpl_label::mech_overflow,fpl_label::sec_highground,0);
		this->height.set_distribution_types(ptr_database);

		//calculate the mean value of height
		this->height.set_new_middle(this->base_water.vertical_distance(this->crest_water));
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the geometry via a database table
void Fpl_Sec_Type_Highground::set_geometrie(QSqlDatabase *ptr_database, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the geometry of the highground section " <<section_id<<" ... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);	
	try{
		//read the cubature
		this->read_cubature(ptr_database, section_id);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the fault tree for this section type via a database table
void Fpl_Sec_Type_Highground::read_cubature(QSqlDatabase *, const int ){
	//base point water is always 0,0...all is related to them!
	this->base_water.set_point_coordinate(0.0,0.0);
}
//set the fault-tree from table
void Fpl_Sec_Type_Highground::init_fault_tree(QSqlDatabase *ptr_database){
	
	//output
	ostringstream cout;
	cout <<"Initialize the fault tree for a highground-section... " << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);

	//set the mechanism table
	try{
		_Fpl_Section_Type::set_table_mechanism(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//set the infos of the faulttree
	//string for the query for one section
	ostringstream filter;
	filter << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = " << "'"<<fpl_label::sec_highground<<"'";
	filter << " and " <<  _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = true";
	model.setTable((_Fpl_Section_Type::table_mechanism->get_table_name()).c_str());
	model.setFilter(filter.str().c_str());
    //set the query 
	Data_Base::database_request(&model);
	if(model.lastError().isValid()){//table is not found
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name              : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info        : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw (msg);
	}
	//read out the results
	//section type is set (string)
	string mechanism_buffer;
	bool mechanism_flag=false;
	int number_result=model.rowCount();
	for(int i=0;i<number_result;i++){
		mechanism_flag=false;
		mechanism_buffer = model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name)).c_str()).toString().toStdString();
		mechanism_flag = model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied)).c_str()).toBool();
		this->decide_mechanism(mechanism_buffer, mechanism_flag, true);
	}
	//no mechanism is set
	if(this->number_mech==0){
		Warning msg=this->set_warning(0);
		msg.output_msg(1);
	}
};
//Decide and allocate the mechanism, which should by applied during calculation
void Fpl_Sec_Type_Highground::decide_mechanism(const string mechanism_name, const bool application_flag, const bool){
	//application desired
	if(application_flag==true){
		if(mechanism_name==fpl_label::mech_overflow){
			this->allocate_overflow();
			this->overflow_event->set_2calc_flag(true);
			this->number_mech++;
			ostringstream cout;
			cout <<"Mechanism overflow is initialized..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
		}
	}
}
//Set error(s)
Error Fpl_Sec_Type_Highground::set_error(const int err_type){
		string place="Fpl_Sec_Type_Highground::";
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
		case 2://bad_alloc
			place.append("allocate_overflow(void)");
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
Warning Fpl_Sec_Type_Highground::set_warning(const int warn_type){
	string place="Fpl_Sec_Type_Highground::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no mechanism is set
			place.append("init_fault_tree(QSqlDatabase *ptr_database)");
			reason="No mechanism is set for the section type highground";
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