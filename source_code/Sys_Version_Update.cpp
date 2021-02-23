#include "Main_Headers_Precompiled.h"
//#include "Sys_Version_Update.h"

//Default constructor
Sys_Version_Update::Sys_Version_Update(void){
	Sys_Memory_Count::self()->add_mem((sizeof(Sys_Version_Update)), _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Version_Update::~Sys_Version_Update(void){
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Version_Update)), _sys_system_modules::SYS_SYS);
}
//____________
//public
//Check for update hydraulic table for observation points (20.12.2011)
void Sys_Version_Update::check_update_hyd_table_obs_point(QSqlDatabase *ptr_database){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}
	bool error=false;
	//check it
	try{
		Hyd_Observation_Point_Manager::set_table(ptr_database, true);
	}
	catch(Error msg){
		error=true;
	}
	//create it
	if(error==true){
		if(Hyd_Observation_Point_Manager::obs_point_table->table_name.found_flag==false){
			Hyd_Observation_Point_Manager::close_table();
			Hyd_Observation_Point_Manager::create_table(ptr_database);
		}
	}

	Hyd_Observation_Point_Manager::close_table();
}
//Check and update the text of the hydraulic table of the polysegment (20.12.2011)
void Sys_Version_Update::check_update_hyd_table_polysegment_name(const string project_file){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}

	ifstream ifile;
	string myline;
	string buffer;

	//open file for reading
	ifile.open(project_file.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Filename: " << project_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	string table_info;
	bool table_begin=false;
	int pos=0;
	bool found=false;
	//read in file
	do{
		getline(ifile, myline,'\n');
		if(table_begin==false){
			pos=myline.find(sys_label::table_begin);
			if(pos>=0){
				table_begin=true;
			}
		}
		pos=-1;
		if(table_begin==true){
			buffer=myline;
			_Hyd_Parse_IO::erase_comment(&buffer);
			pos=buffer.find("Name    name");
			if(pos>=0 && found==false){
				found=true;
				buffer.replace(pos,pos+hyd_label::polyseg_name.length()-2, hyd_label::polyseg_name);
				myline=buffer;
			}
		}
		table_info.append(myline);
		table_info.append("\n");
	}
	while(ifile.eof()==false);
	ifile.close();

	if(table_info.find_last_of("\n")==table_info.length()-1){
		table_info.erase(table_info.length()-1);
	}

	ofstream ofile;
	//open for writing
	ofile.open(project_file.c_str(), ios_base::out);
	if(ofile.is_open()==false){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Filename: " << project_file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	ofile << table_info;
	ofile.flush();
	//close file
	ofile.close();
}
//Check and update the text of the hydraulic table of the hydraulic system members (21.02.2012)
void Sys_Version_Update::check_update_hyd_table_system_member_sync(QSqlDatabase *ptr_database, const string project_file){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}

	bool error=false;

	//check if columns exists
	try{
		Hyd_Param_Global::set_table(ptr_database, true);
	}
	catch(Error msg){
		error=true;
	}
	//max change h fp
	bool exists=false;
	for(int i=0; i< Hyd_Param_Global::global_param_table->get_number_col(); i++){
		if((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id==hyd_label::syn_maxchange_h_fp){
			exists=(Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::syn_maxchange_h_fp, sys_label::tab_col_type_double, true, "0.01", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::syn_maxchange_h_fp);
	}
	//max change h rv
	exists=false;
	for(int i=0; i< Hyd_Param_Global::global_param_table->get_number_col(); i++){
		if((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id==hyd_label::syn_maxchange_h_rv){
			exists=(Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::syn_maxchange_h_rv, sys_label::tab_col_type_double, true, "0.01", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::syn_maxchange_h_rv);
	}

	//min step internal
	exists=false;
	for(int i=0; i< Hyd_Param_Global::global_param_table->get_number_col(); i++){
		if((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id==hyd_label::syn_min_int_tstep){
			exists=(Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::syn_min_int_tstep, sys_label::tab_col_type_double, true, "10.0", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::syn_min_int_tstep);
	}

	//max change v head rv
	exists=false;
	for(int i=0; i< Hyd_Param_Global::global_param_table->get_number_col(); i++){
		if((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id==hyd_label::syn_maxchange_v_rv){
			exists=(Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::syn_maxchange_v_rv, sys_label::tab_col_type_double, true, "10.0", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::syn_maxchange_v_rv);
	}

	Hyd_Param_Global::close_table();
}
//Check and update the result members table of the fpl section type dike (21.05.2012)
void Sys_Version_Update::check_update_fpl_table_result_sec_type_dike(QSqlDatabase *ptr_database, const string project_file){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam || Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}

	bool error=false;

	//check if columns exists
	try{
		Fpl_Sec_Type_Dike::set_result_table(ptr_database, true);
	}
	catch(Error msg){
		error=true;
	}
	//critical discharge
	bool exists=false;
	for(int i=0; i< Fpl_Sec_Type_Dike::result_table->get_number_col(); i++){
		if((Fpl_Sec_Type_Dike::result_table->get_ptr_col())[i].id==fpl_label::crit_landside_discharge){
			exists=(Fpl_Sec_Type_Dike::result_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, fpl_label::tab_fpl_dike_result, fpl_label::crit_landside_discharge, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::fpl);
		buffer.add_columns_file(project_file, fpl_label::tab_fpl_dike_result, fpl_label::crit_landside_discharge);
	}

	Fpl_Sec_Type_Dike::close_result_table();
}
//Check for update fpl table for results of section type dune (5.5.2012)
void Sys_Version_Update::check_update_fpl_table_dune_results(QSqlDatabase *ptr_database){
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam || Sys_Project::get_project_type()==_sys_project_type::proj_dam_hyd ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}
	bool error=false;
	//check it
	try{
		Fpl_Sec_Type_Dune::set_result_table(ptr_database, true);
	}
	catch(Error msg){
		error=true;
	}
	//create it
	if(error==true){
		if(Fpl_Sec_Type_Dune::result_table->table_name.found_flag==false){
			Fpl_Sec_Type_Dune::close_result_table();
			Fpl_Sec_Type_Dune::create_result_table(ptr_database);
		}
	}

	Fpl_Sec_Type_Dune::close_result_table();
}
//Check and update the text of the hydraulic table of the hydraulic element result members; smax is introduced (6.07.2012)
void Sys_Version_Update::check_update_hyd_table_elem_result_smax(QSqlDatabase *ptr_database, const string project_file){
	if(	Sys_Project::get_project_type()==_sys_project_type::proj_fpl ||
		Sys_Project::get_project_type()==_sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file){
		return;
	}

	bool error=false;

	//check if columns exists
	try{
		Hyd_Element_Floodplain::set_erg_table(ptr_database, true);
	}
	catch(Error msg){
		error=true;
	}
	//max s value
	bool exists=false;
	for(int i=0; i< Hyd_Element_Floodplain::erg_table->get_number_col(); i++){
		if((Hyd_Element_Floodplain::erg_table->get_ptr_col())[i].id==hyd_label::elemerg_s_max){
			exists=(Hyd_Element_Floodplain::erg_table->get_ptr_col())[i].found_flag;
		}
	}
	if(exists==false){
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_fpelem_erg_max, hyd_label::elemerg_s_max, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_fpelem_erg_max, hyd_label::elemerg_s_max);
	}

	Hyd_Element_Floodplain::close_erg_table();
}
//Check and update the hydraulic global parameter table with output settings (3.9.2020)
void Sys_Version_Update::check_update_hyd_table_global_param(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}

	bool error = false;

	//check if columns exists
	try {
		Hyd_Param_Global::set_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//output tecplot 1d
	bool exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_tecplot_1d) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_tecplot_1d, sys_label::tab_col_type_bool, false, "false", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_tecplot_1d);
	}

	//output tecplot 2d
	exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_tecplot_2d) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_tecplot_2d, sys_label::tab_col_type_bool, false, "false", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_tecplot_2d);
	}

	//output paraview 1d
	exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_paraview_1d) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_paraview_1d, sys_label::tab_col_type_bool, false, "true", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_paraview_1d);
	}

	//output paraview 2d
	exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_paraview_2d) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_paraview_2d, sys_label::tab_col_type_bool, false, "true", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_paraview_2d);
	}

	//output bluekenue 2d
	exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_bluekenue_2d) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_bluekenue_2d, sys_label::tab_col_type_bool, false, "false", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_bluekenue_2d);
	}

	//output bluekenue 2d
	exists = false;
	for (int i = 0; i < Hyd_Param_Global::global_param_table->get_number_col(); i++) {
		if ((Hyd_Param_Global::global_param_table->get_ptr_col())[i].id == hyd_label::output_instat_db) {
			exists = (Hyd_Param_Global::global_param_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_sys_param, hyd_label::output_instat_db, sys_label::tab_col_type_bool, false, "true", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_sys_param, hyd_label::output_instat_db);
	}




	Hyd_Param_Global::close_table();

}
//Check and update the hydraulic table for instationary floodplain results (7.9.2020)
void Sys_Version_Update::check_update_hyd_table_instat_results(QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check it
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Hyd_Element_Floodplain::erg_instat_table->table_name.found_flag == false) {
			Hyd_Element_Floodplain::close_erg_instat_table();
			Hyd_Element_Floodplain::create_erg_instat_table(ptr_database);

			//add an index to the max erg table
			try {
				Hyd_Element_Floodplain::set_erg_table(ptr_database);
			}
			catch (Error msg) {
				throw msg;
			}
			Hyd_Element_Floodplain::erg_table->create_spatial_index2column(ptr_database, Hyd_Element_Floodplain::erg_table->get_column_name(hyd_label::elemdata_polygon));
			Hyd_Element_Floodplain::close_erg_table();



		}
	}

	

	Hyd_Element_Floodplain::close_erg_instat_table();

}
///Check and update the hydraulic table for instationary river results (16.9.2020)
void Sys_Version_Update::check_update_hyd_table_instat_results_rv(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check it
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (_Hyd_River_Profile::erg_instat_table->table_name.found_flag == false) {
			_Hyd_River_Profile::close_erg_instat_table();
			_Hyd_River_Profile::create_erg_instat_table(ptr_database);

		}
	}



	_Hyd_River_Profile::close_erg_instat_table();
	this->check_update_hyd_table_river_result_width(ptr_database, project_file);

	//make indizes for max results table
	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}


	_Hyd_River_Profile::erg_table->create_spatial_index2column(ptr_database, _Hyd_River_Profile::erg_table->get_column_name(hyd_label::elemdata_polygon));


	_Hyd_River_Profile::close_erg_table();

}
//Check and update the hydraulic view for boundary conditions to floodplain elements / river profile (3.2.2021)
void Sys_Version_Update::check_update_hyd_view_bound2elements_profile(QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check it
	try {
		Hyd_Element_Floodplain::create_bound2elems_view(ptr_database);
	}
	catch (Error msg) {
		error = true;
	}

	//check it
	error = false;
	try {
		_Hyd_River_Profile::create_bound2profile_view(ptr_database);
	}
	catch (Error msg) {
		error = true;
	}
}
//Check and update the output control parameter for FPL-module (22.2.2021)
void Sys_Version_Update::check_update_fpl_output_control(QSqlDatabase *ptr_database) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam || Sys_Project::get_project_type() == _sys_project_type::proj_dam_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}

	try {
		Fpl_Mc_Sim::set_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}

	//check if available
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter << "Select ";
	test_filter << " * ";
	test_filter << " from " << Fpl_Mc_Sim::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << "= " << "'" << fpl_control_param::output_tecplot << "'";


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if (results.lastError().isValid()) {
		Error msg;
		msg.set_msg("Sys_Version_Update::check_update_fpl_output_control(QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if (results.rowCount() > 0) {
		Fpl_Mc_Sim::close_table();
		return;
	}


	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob = Fpl_Mc_Sim::table->maximum_int_of_column(Fpl_Mc_Sim::table->get_column_name(fpl_label::glob_id), ptr_database) + 1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Mc_Sim::table->get_table_name();
	query_string_fix << " ( ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::glob_id) << " , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " , ";
	query_string_fix << Fpl_Mc_Sim::table->get_column_name(label::description) << " ) ";

	//Keystring for maximal number of a monte-carlo simulations 
	ostringstream query_string;
	//Keystring for the output setting to tecplot   
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_tecplot << "'" << " , ";
	query_string << fpl_control_param::output_tecplot_def << " , ";
	query_string << "'Tecplot output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the output setting to paraview   
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_paraview << "'" << " , ";
	query_string << fpl_control_param::output_paraview_def << " , ";
	query_string << "'ParaView output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	//Keystring for the output setting to excel  
	query_string << " VALUES ( ";
	query_string << id_glob << " , ";
	query_string << "'" << fpl_control_param::output_excel << "'" << " , ";
	query_string << fpl_control_param::output_excel_def << " , ";
	query_string << "'Excel output required 1 := true ; 0 := false'" << " ) ";
	total << query_string_fix.str() << query_string.str();
	Data_Base::database_request(&model, total.str(), ptr_database);
	total.str("");
	query_string.str("");
	id_glob++;

	Fpl_Mc_Sim::close_table();

}
//____________
//private
//Check and update the text of the hydraulic table of the hydraulic river profile result members; width_max is introduced (18.02.2021)
void Sys_Version_Update::check_update_hyd_table_river_result_width(QSqlDatabase *ptr_database, const string project_file) {

	if (Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}

	bool error = false;

	//check if columns exists for max results table
	try {
		_Hyd_River_Profile::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//width
	bool exists = false;
	for (int i = 0; i < _Hyd_River_Profile::erg_table->get_number_col(); i++) {
		if ((_Hyd_River_Profile::erg_table->get_ptr_col())[i].id == hyd_label::proferg_width_max) {
			exists = (_Hyd_River_Profile::erg_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_rvprof_erg_max, hyd_label::proferg_width_max, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_rvprof_erg_max, hyd_label::proferg_width_max);
	}

	_Hyd_River_Profile::close_erg_table();

	error = false;

	//check if columns exists for results per timestep table
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//width
	exists = false;
	for (int i = 0; i < _Hyd_River_Profile::erg_instat_table->get_number_col(); i++) {
		if ((_Hyd_River_Profile::erg_instat_table->get_ptr_col())[i].id == hyd_label::proferg_width_max) {
			exists = (_Hyd_River_Profile::erg_instat_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_rvprof_erg_instat, hyd_label::proferg_width_max, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_rvprof_erg_instat, hyd_label::proferg_width_max);
	}

	_Hyd_River_Profile::close_erg_instat_table();



}
//Set error(s)
Error Sys_Version_Update::set_error(const int err_type){
	string place="Sys_Version_Update::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://can not open project
			place.append("check_update_hyd_table_polysegment_name(const string project_file)");
			reason="Can not open the project file";
			help="Check the file";
			type=5;
			break;
		case 1://can not open project
			place.append("check_update_hyd_table_system_member_sync(const string project_file)");
			reason="Can not open the project file";
			help="Check the file";
			type=5;
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

