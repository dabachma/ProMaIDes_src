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
	if (error == true) {
		try {
			_Hyd_River_Profile::set_erg_table(ptr_database);
		}
		catch (Error msg) {
			throw msg;
		}
		_Hyd_River_Profile::erg_table->create_spatial_index2column(ptr_database, _Hyd_River_Profile::erg_table->get_column_name(hyd_label::proferg_polygon));
		_Hyd_River_Profile::close_erg_table();

	}

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
	if (Hyd_Element_Floodplain::check_bound2elems_view_exists(ptr_database) == false) {
		try {
			Hyd_Element_Floodplain::create_bound2elems_view(ptr_database);
		}
		catch (Error msg) {
			error = true;
		}
	}

	//check it
	error = false;
	if (_Hyd_River_Profile::check_bound2profile_view_exists(ptr_database) == false) {
		try {
			_Hyd_River_Profile::create_bound2profile_view(ptr_database);
		}
		catch (Error msg) {
			error = true;
		}
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
//Check and update the CI tables for DAM-module (11.3.2021)
void Sys_Version_Update::check_update_dam_ci(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check it
	try {
		Dam_CI_Point::set_point_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Point::point_table->table_name.found_flag == false) {
			Dam_CI_Point::close_point_table();
			Dam_CI_Point::create_point_table(ptr_database);

		}
	}

	Dam_CI_Point::close_point_table();


	//check system table
	error = false;
	//check it
	try {
		Dam_Damage_System::set_Dam_system_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_Damage_System::system_table->table_name.found_flag == false) {
			Dam_Damage_System::close_Dam_system_table();
			Dam_Damage_System::create_Dam_system_table(ptr_database);

		}
	}

	Dam_Damage_System::close_Dam_system_table();


	//check polygon table
	error = false;
	//check it
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Polygon::polygon_table->table_name.found_flag == false) {
			Dam_CI_Polygon::close_polygon_table();
			Dam_CI_Polygon::create_polygon_table(ptr_database);

		}
	}

	Dam_CI_Polygon::close_polygon_table();

	//check connection table
	error = false;
	//check it
	try {
		Dam_CI_Element_List::set_connection_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Element_List::connection_table->table_name.found_flag == false) {
			Dam_CI_Element_List::close_connection_table();
			Dam_CI_Element_List::create_connection_table(ptr_database);

		}
	}

	Dam_CI_Element_List::close_connection_table();


	//check point erg table
	error = false;
	//check it
	try {
		Dam_CI_Point::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Point::point_erg_table->table_name.found_flag == false) {
			Dam_CI_Point::close_erg_table();
			Dam_CI_Point::create_erg_table(ptr_database);

		}
	}
	Dam_CI_Point::close_erg_table();

	//check point instat erg table
	error = false;
	//check it
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Point::point_instat_erg_table->table_name.found_flag == false) {
			Dam_CI_Point::close_instat_erg_table();
			Dam_CI_Point::create_instat_erg_table(ptr_database);

		}
	}
	Dam_CI_Point::close_instat_erg_table();

	//check polygon erg table
	error = false;
	//check it
	try {
		Dam_CI_Polygon::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Polygon::polygon_erg_table->table_name.found_flag == false) {
			Dam_CI_Polygon::close_erg_table();
			Dam_CI_Polygon::create_erg_table(ptr_database);

		}
	}
	Dam_CI_Polygon::close_erg_table();

	//check polygon instat_erg table
	error = false;
	//check it
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Polygon::polygon_instat_erg_table->table_name.found_flag == false) {
			Dam_CI_Polygon::close_instat_erg_table();
			Dam_CI_Polygon::create_instat_erg_table(ptr_database);

		}
	}
	Dam_CI_Polygon::close_instat_erg_table();

	//check connection erg table
	error = false;
	//check it
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Element_List::connection_erg_table->table_name.found_flag == false) {
			Dam_CI_Element_List::close_erg_table();
			Dam_CI_Element_List::create_erg_table(ptr_database);

		}
	}
	Dam_CI_Element_List::close_erg_table();


	//check connection instat_erg table
	error = false;
	//check it
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}
	//create it
	if (error == true) {
		if (Dam_CI_Element_List::connection_instat_erg_table->table_name.found_flag == false) {
			Dam_CI_Element_List::close_instat_erg_table();
			Dam_CI_Element_List::create_instat_erg_table(ptr_database);

		}
	}
	Dam_CI_Element_List::close_instat_erg_table();




	//check if columns exists for results per timestep table point
	try {
		Dam_CI_Point::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//add failure duration in instat_erg table
	bool exists;
	exists = false;
	for (int i = 0; i < Dam_CI_Point::point_instat_erg_table->get_number_col(); i++) {
		if ((Dam_CI_Point::point_instat_erg_table->get_ptr_col())[i].id == dam_label::failure_duration) {
			exists = (Dam_CI_Point::point_instat_erg_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_point_instat_erg, dam_label::failure_duration, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_point_instat_erg, dam_label::failure_duration);
	}
	Dam_CI_Point::close_instat_erg_table();


	//check if columns exists for results per timestep table polygon
	try {
		Dam_CI_Polygon::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//add failure duration in instat_erg table
	exists = false;
	for (int i = 0; i < Dam_CI_Polygon::polygon_instat_erg_table->get_number_col(); i++) {
		if ((Dam_CI_Polygon::polygon_instat_erg_table->get_ptr_col())[i].id == dam_label::failure_duration) {
			exists = (Dam_CI_Polygon::polygon_instat_erg_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_polygon_instat_erg, dam_label::failure_duration, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_polygon_instat_erg, dam_label::failure_duration);
	}
	Dam_CI_Polygon::close_instat_erg_table();

}
//Check and add columns to the CI result-tables of the CU-connectors for DAM-module (5.7.2021)
void Sys_Version_Update::check_update_connect_results_dam_ci(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check if columns exists for stationary results of CI-connectors
	try {
		Dam_CI_Element_List::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	bool exists = false;
	for (int i = 0; i < Dam_CI_Element_List::connection_erg_table->get_number_col(); i++) {
		if ((Dam_CI_Element_List::connection_erg_table->get_ptr_col())[i].id == dam_label::in_id) {
			exists = (Dam_CI_Element_List::connection_erg_table->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_connection_erg, dam_label::in_id, sys_label::tab_col_type_int, false, "-1", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_connection_erg, dam_label::in_id);

		Tables buffer1;
		buffer1.add_columns(ptr_database, dam_label::tab_ci_connection_erg, dam_label::in_point_flag, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer1.add_columns_file(project_file, dam_label::tab_ci_connection_erg, dam_label::in_point_flag);
		Tables buffer2;
		buffer2.add_columns(ptr_database, dam_label::tab_ci_connection_erg, dam_label::out_id, sys_label::tab_col_type_int, false, "-1", _sys_table_type::dam);
		buffer2.add_columns_file(project_file, dam_label::tab_ci_connection_erg, dam_label::out_id);
		Tables buffer3;
		buffer3.add_columns(ptr_database, dam_label::tab_ci_connection_erg, dam_label::out_point_flag, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer3.add_columns_file(project_file, dam_label::tab_ci_connection_erg, dam_label::out_point_flag);
	}

	Dam_CI_Element_List::close_erg_table();

	error = false;


	//check if columns exists for instationary results of CI-connectors
	try {
		Dam_CI_Element_List::set_instat_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	exists = false; 
	for (int i = 0; i < Dam_CI_Element_List::connection_instat_erg_table->get_number_col(); i++) {
		if ((Dam_CI_Element_List::connection_instat_erg_table->get_ptr_col())[i].id == dam_label::in_id) {
			exists = (Dam_CI_Element_List::connection_instat_erg_table->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_connection_instat_erg, dam_label::in_id, sys_label::tab_col_type_int, false, "-1", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_connection_instat_erg, dam_label::in_id);
		Tables buffer1;
		buffer1.add_columns(ptr_database, dam_label::tab_ci_connection_instat_erg, dam_label::in_point_flag, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer1.add_columns_file(project_file, dam_label::tab_ci_connection_instat_erg, dam_label::in_point_flag);
		Tables buffer2;
		buffer2.add_columns(ptr_database, dam_label::tab_ci_connection_instat_erg, dam_label::out_id, sys_label::tab_col_type_int, false, "-1", _sys_table_type::dam);
		buffer2.add_columns_file(project_file, dam_label::tab_ci_connection_instat_erg, dam_label::out_id);
		Tables buffer3;
		buffer3.add_columns(ptr_database, dam_label::tab_ci_connection_instat_erg, dam_label::out_point_flag, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer3.add_columns_file(project_file, dam_label::tab_ci_connection_instat_erg, dam_label::out_point_flag);
	}

	Dam_CI_Element_List::close_instat_erg_table();

}
///Check and add columns to the CI point/polygon-tables of the CI-elements for DAM-module (21.9.2021)
void Sys_Version_Update::check_update_dam_ci_elements(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;
	//check if columns exists for CI-points
	try {
		Dam_CI_Point::set_point_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	bool exists = false;
	for (int i = 0; i < Dam_CI_Point::point_table->get_number_col(); i++) {
		if ((Dam_CI_Point::point_table->get_ptr_col())[i].id == dam_label::aut_value) {
			exists = (Dam_CI_Point::point_table->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_point, dam_label::aut_value, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_point, dam_label::aut_value);
		Tables buffer1;
		buffer1.add_columns(ptr_database, dam_label::tab_ci_point, dam_label::hub_value, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer1.add_columns_file(project_file, dam_label::tab_ci_point, dam_label::hub_value);
		Tables buffer2;
		buffer2.add_columns(ptr_database, dam_label::tab_ci_point, dam_label::cv_value, sys_label::tab_col_type_double, false, "-1.0", _sys_table_type::dam);
		buffer2.add_columns_file(project_file, dam_label::tab_ci_point, dam_label::cv_value);
		Tables buffer3;
		buffer3.add_columns(ptr_database, dam_label::tab_ci_point, dam_label::cp_value, sys_label::tab_col_type_double, false, "-1.0", _sys_table_type::dam);
		buffer3.add_columns_file(project_file, dam_label::tab_ci_point, dam_label::cp_value);
	}

	Dam_CI_Point::close_point_table();

	error = false;


	//check if columns exists for CI-polygons
	try {
		Dam_CI_Polygon::set_polygon_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	exists = false;
	for (int i = 0; i < Dam_CI_Polygon::polygon_table->get_number_col(); i++) {
		if ((Dam_CI_Polygon::polygon_table->get_ptr_col())[i].id == dam_label::aut_value) {
			exists = (Dam_CI_Polygon::polygon_table->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_ci_polygon, dam_label::aut_value, sys_label::tab_col_type_int, false, "0", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_ci_polygon, dam_label::aut_value);
		Tables buffer1;
		buffer1.add_columns(ptr_database, dam_label::tab_ci_polygon, dam_label::cv_value, sys_label::tab_col_type_double, false, "-1.0", _sys_table_type::dam);
		buffer1.add_columns_file(project_file, dam_label::tab_ci_polygon, dam_label::cv_value);
	}

	Dam_CI_Polygon::close_polygon_table();

}
//Check and add columns to the Dam-result and risk-result table for CI-results for DAM/RISK-module (8.3.2023)
void Sys_Version_Update::check_update_ci_reults(QSqlDatabase *ptr_database, const string project_file) {
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl ||
		Sys_Project::get_project_type() == _sys_project_type::proj_hyd_file ||
		Sys_Project::get_project_type() == _sys_project_type::proj_fpl_file) {
		return;
	}
	bool error = false;


	//check if columns exists for DAM-results
	try {
		Dam_Damage_System::set_erg_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	bool exists = false;
	for (int i = 0; i < Dam_Damage_System::erg_table->get_number_col(); i++) {
		if ((Dam_Damage_System::erg_table->get_ptr_col())[i].id == dam_label::ci_elect_pt) {
			exists = (Dam_Damage_System::erg_table->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_elect_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_info_tec_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer1.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_water_sup_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer2.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_water_treat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer3.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_energy_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer4.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_health_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer5.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_social_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer6.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_mat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer7.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_mat_pt);

		Tables buffer8;
		//add new table column
		buffer8.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_elect_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer8.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_elect_p);
		Tables buffer9;
		buffer9.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_info_tec_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer9.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_info_tec_p);
		Tables buffer10;
		buffer10.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_water_sup_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer10.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_water_sup_p);
		Tables buffer11;
		buffer11.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_water_treat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer11.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_water_treat_p);
		Tables buffer12;
		buffer12.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_energy_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer12.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_energy_p);
		Tables buffer13;
		buffer13.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_health_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer13.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_health_p);
		Tables buffer14;
		buffer14.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_social_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer14.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_social_p);
		Tables buffer15;
		buffer15.add_columns(ptr_database, dam_label::tab_dam_erg, dam_label::ci_mat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::dam);
		buffer15.add_columns_file(project_file, dam_label::tab_dam_erg, dam_label::ci_mat_p);
	}

	Dam_Damage_System::close_erg_table();

	error = false;
	exists = false;

	if (Sys_Project::get_project_type() == _sys_project_type::proj_dam ||
		Sys_Project::get_project_type() == _sys_project_type::proj_dam_hyd) {
		return;
	}

	

	//update for Risk-table total
	try {
		Risk_System::set_resulttable(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	
	for (int i = 0; i < Risk_System::table_results->get_number_col(); i++) {
		if ((Risk_System::table_results->get_ptr_col())[i].id == dam_label::ci_elect_pt) {
			exists = (Risk_System::table_results->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_elect_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_info_tec_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer1.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_water_sup_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer2.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_water_treat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer3.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_energy_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer4.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_health_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer5.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_social_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer6.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_mat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer7.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_mat_pt);

		Tables buffer8;
		//add new table column
		buffer8.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_elect_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer8.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_elect_p);
		Tables buffer9;
		buffer9.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_info_tec_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer9.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_info_tec_p);
		Tables buffer10;
		buffer10.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_water_sup_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer10.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_water_sup_p);
		Tables buffer11;
		buffer11.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_water_treat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer11.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_water_treat_p);
		Tables buffer12;
		buffer12.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_energy_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer12.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_energy_p);
		Tables buffer13;
		buffer13.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_health_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer13.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_health_p);
		Tables buffer14;
		buffer14.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_social_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer14.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_social_p);
		Tables buffer15;
		buffer15.add_columns(ptr_database, risk_label::tab_risk_result, dam_label::ci_mat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer15.add_columns_file(project_file, risk_label::tab_risk_result, dam_label::ci_mat_p);
	}

	Risk_System::close_resulttable();

	error = false;
	exists = false;


	//update for Risk-table detail
	try {
		Risk_System::set_resulttable_detailed(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	
	for (int i = 0; i < Risk_System::table_detailed_results->get_number_col(); i++) {
		if ((Risk_System::table_detailed_results->get_ptr_col())[i].id == dam_label::ci_elect_pt) {
			exists = (Risk_System::table_detailed_results->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_elect_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_info_tec_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer1.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_water_sup_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer2.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_water_treat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer3.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_energy_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer4.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_health_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer5.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_social_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer6.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_mat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer7.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_mat_pt);

		Tables buffer8;
		//add new table column
		buffer8.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_elect_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer8.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_elect_p);
		Tables buffer9;
		buffer9.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_info_tec_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer9.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_info_tec_p);
		Tables buffer10;
		buffer10.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_water_sup_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer10.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_water_sup_p);
		Tables buffer11;
		buffer11.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_water_treat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer11.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_water_treat_p);
		Tables buffer12;
		buffer12.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_energy_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer12.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_energy_p);
		Tables buffer13;
		buffer13.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_health_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer13.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_health_p);
		Tables buffer14;
		buffer14.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_social_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer14.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_social_p);
		Tables buffer15;
		buffer15.add_columns(ptr_database, risk_label::tab_risk_detailed, dam_label::ci_mat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer15.add_columns_file(project_file, risk_label::tab_risk_detailed, dam_label::ci_mat_p);
	}

	Risk_System::close_resulttable_detailed();

	error = false;
	exists = false;

	//update for Risk-table cummulated
	try {
		Risk_System::set_resulttable_cumulated(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in
	
	for (int i = 0; i < Risk_System::table_cumulated_results->get_number_col(); i++) {
		if ((Risk_System::table_cumulated_results->get_ptr_col())[i].id == dam_label::ci_elect_pt) {
			exists = (Risk_System::table_cumulated_results->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_elect_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_info_tec_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer1.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_water_sup_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer2.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_water_treat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer3.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_energy_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer4.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_health_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer5.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_social_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer6.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_mat_pt, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer7.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_mat_pt);

		Tables buffer8;
		//add new table column
		buffer8.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_elect_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer8.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_elect_p);
		Tables buffer9;
		buffer9.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_info_tec_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer9.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_info_tec_p);
		Tables buffer10;
		buffer10.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_water_sup_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer10.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_water_sup_p);
		Tables buffer11;
		buffer11.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_water_treat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer11.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_water_treat_p);
		Tables buffer12;
		buffer12.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_energy_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer12.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_energy_p);
		Tables buffer13;
		buffer13.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_health_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer13.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_health_p);
		Tables buffer14;
		buffer14.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_social_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer14.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_social_p);
		Tables buffer15;
		buffer15.add_columns(ptr_database, risk_label::tab_risk_cumulated, dam_label::ci_mat_p, sys_label::tab_col_type_double, true, "0.0", _sys_table_type::risk);
		buffer15.add_columns_file(project_file, risk_label::tab_risk_cumulated, dam_label::ci_mat_p);
	}

	Risk_System::close_resulttable_cumulated ();

	error = false;
	exists = false;

	if (Sys_Project::get_project_type() == _sys_project_type::proj_risk) {
		return;
	}

	//update MADM table
	try {
		Madm_System::set_table_sets(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in

	for (int i = 0; i < Madm_System::table_sets->get_number_col(); i++) {
		if ((Madm_System::table_sets->get_ptr_col())[i].id == madm_label::crit_risk_ci_elect_pt) {
			exists = (Madm_System::table_sets->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_elect_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_info_tec_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer1.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_water_sup_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer2.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_water_treat_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer3.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_energy_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer4.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_health_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer5.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_social_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer6.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, madm_label::tab_set, madm_label::crit_risk_ci_mat_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer7.add_columns_file(project_file, madm_label::tab_set, madm_label::crit_risk_ci_mat_pt);


		Madm_System::delete_data_in_table_sets(ptr_database);
		Madm_System::set_default_weighting_set2datbase(ptr_database);




	}


	Madm_System::close_table_sets();

	error = false;
	exists = false;


	//update MADM decisionmatrix table
	try {
		Madm_Decision_Matrix::set_table_matrix(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//check just id_in

	for (int i = 0; i < Madm_Decision_Matrix::table_matrix->get_number_col(); i++) {
		if ((Madm_Decision_Matrix::table_matrix->get_ptr_col())[i].id == madm_label::crit_risk_ci_elect_pt) {
			exists = (Madm_Decision_Matrix::table_matrix->get_ptr_col())[i].found_flag;
		}
	}
	//add
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_elect_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_elect_pt);
		Tables buffer1;
		buffer1.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_info_tec_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer1.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_info_tec_pt);
		Tables buffer2;
		buffer2.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_water_sup_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer2.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_water_sup_pt);
		Tables buffer3;
		buffer3.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_water_treat_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer3.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_water_treat_pt);
		Tables buffer4;
		buffer4.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_energy_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer4.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_energy_pt);
		Tables buffer5;
		buffer5.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_health_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer5.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_health_pt);
		Tables buffer6;
		buffer6.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_social_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer6.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_social_pt);
		Tables buffer7;
		buffer7.add_columns(ptr_database, madm_label::tab_matrix, madm_label::crit_risk_ci_mat_pt, sys_label::tab_col_type_double, false, "0.0", _sys_table_type::madm);
		buffer7.add_columns_file(project_file, madm_label::tab_matrix, madm_label::crit_risk_ci_mat_pt);


	}



	Madm_Decision_Matrix::close_table_matrix();

	error = false;
	exists = false;

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

	error = false;

	//check if columns exists for results per timestep table
	try {
		Hyd_Element_Floodplain::set_erg_instat_table(ptr_database, true);
	}
	catch (Error msg) {
		error = true;
	}

	//fp id and element id in instat erg FP tables
	exists = false;
	for (int i = 0; i < Hyd_Element_Floodplain::erg_instat_table->get_number_col(); i++) {
		if ((Hyd_Element_Floodplain::erg_instat_table->get_ptr_col())[i].id == hyd_label::elemdata_fpno) {
			exists = (Hyd_Element_Floodplain::erg_instat_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_fpelem_erg_instat, hyd_label::elemdata_fpno, sys_label::tab_col_type_int,true, "9999", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_fpelem_erg_instat, hyd_label::elemdata_fpno);
	}

	exists = false;
	for (int i = 0; i < Hyd_Element_Floodplain::erg_instat_table->get_number_col(); i++) {
		if ((Hyd_Element_Floodplain::erg_instat_table->get_ptr_col())[i].id == hyd_label::elemdata_id) {
			exists = (Hyd_Element_Floodplain::erg_instat_table->get_ptr_col())[i].found_flag;
		}
	}
	if (exists == false) {
		Tables buffer;
		//add new table column
		buffer.add_columns(ptr_database, hyd_label::tab_fpelem_erg_instat, hyd_label::elemdata_id, sys_label::tab_col_type_int, true, "9999", _sys_table_type::hyd);
		buffer.add_columns_file(project_file, hyd_label::tab_fpelem_erg_instat, hyd_label::elemdata_id);
	}

	Hyd_Element_Floodplain::close_erg_instat_table();


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

