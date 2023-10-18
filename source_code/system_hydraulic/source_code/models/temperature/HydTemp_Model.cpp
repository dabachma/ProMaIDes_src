//#include "HydTemp_Model.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *HydTemp_Model::general_temp_param_table=NULL;

//Constructor 
HydTemp_Model::HydTemp_Model(void){

	
	this->model_is_applied = false;

	this->ptr_river_model = NULL;
	this->profiles = NULL;



	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydTemp_Model)-sizeof(Hyd_Floodplain_Polysegment)*3-sizeof(Hyd_Floodplain_Polygon)-
		sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//destructor
HydTemp_Model::~HydTemp_Model(void){
	
	
	if(this->profiles!=NULL){
		delete []this->profiles;
		this->profiles=NULL;
	}
	//this->close_tecplot_output();


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydTemp_Model)-sizeof(Hyd_Floodplain_Polysegment)*3-sizeof(Hyd_Floodplain_Polygon)-
		sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//_________________
//public
//input the river model per file
void HydTemp_Model::input_members(const string global_file, const int index, const string global_path){
		

	try{
		//read in global model parameters
		this->Param_Temp.parameter_per_parser(global_file, index, global_path);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());


	if (this->Param_Temp.temp_profile_File == label::not_set) {

		ostringstream cout;
		cout << "No temperature model is applied for this river" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->model_is_applied = false;
		return;
	}

	try{
		this->Param_Temp.check_members();
		//allocate the river profile with number of profiles
		this->allocate_temp_river_profiles();
		//read in the profile members
		cout <<"Read in temperature data of " << this->Param_Temp.Param_RV->get_number_profiles() << " profiles of the Rivermodel from file "<< this->Param_Temp.temp_profile_File << " ..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->input_river_profiles_temp_data_perfile();
		this->connect_profiles2instat_boundarycurves();
		
	
	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_Temp.Param_RV->get_river_number() << endl;
		info << "Rivername  : " << this->Param_Temp.Param_RV->get_river_name() << endl;
	
		msg.make_second_info(info.str());
		throw msg;
	}
	this->model_is_applied = true;
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Input the river model with the index per file with parser; the general settings as well as the profile data
void HydTemp_Model::input_members(const HydTemp_Param param_temp){
	try{
		//read in global model parameters
		this->Param_Temp= param_temp;
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->Param_Temp.check_members();
		//allocate the river profile with number of profiles
		this->allocate_temp_river_profiles();
		//read in the profile members
		cout << "Read in temperature data of " << this->Param_Temp.Param_RV->get_number_profiles() << " profiles of the Rivermodel from file " << this->Param_Temp.temp_profile_File << " ..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->input_river_profiles_temp_data_perfile();
		

	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_Temp.Param_RV->get_river_number()<< endl;
		info << "Rivername  : " << this->Param_Temp.Param_RV->get_river_name() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Transfer the temperature river model data to a database; the general settings as well as the profile data
void HydTemp_Model::transfer_input_members2database(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() != _sys_project_type::proj_hyd_temp) {
		return;
	}

	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Model::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//give the table name to the query model
	model.setTable((HydTemp_Model::general_temp_param_table->get_table_name()).c_str());
					
	//insert row
	model.insertRows(0, 1);
	//insert data
	model.setData(model.index(0,model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::genmod_id)).c_str())), this->Param_Temp.Param_RV->get_river_number());
	model.setData(model.index(0,model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(label::applied_flag)).c_str())),true);
	model.setData(model.index(0,model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::genmod_name)).c_str())), this->Param_Temp.Param_RV->get_river_name().c_str());

	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::gw_temp)).c_str())), this->Param_Temp.gw_temp);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::brunt_coef)).c_str())), this->Param_Temp.brunt_coef);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::view2sky)).c_str())), this->Param_Temp.view2sky_coef);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::cond_bed)).c_str())), this->Param_Temp.con_bed_coef);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::bed_temp)).c_str())), this->Param_Temp.bed_temp);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::bed_warm_coef)).c_str())), this->Param_Temp.bed_warming_coeff);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::diff_solar_rad)).c_str())), this->Param_Temp.diffusiv_sol_rad_coeff);
	model.setData(model.index(0, model.record().indexOf((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::temp_model_applied)).c_str())), this->model_is_applied);


	//submit it to the datbase
	Data_Base::database_submit(&model);
	if(model.lastError().isValid()){
		Warning msg=set_warning(2);
		ostringstream info;
		info << "Table Name                : " << HydTemp_Model::general_temp_param_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Model river number        : " << this->Param_Temp.Param_RV->get_river_number() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

	if (this->model_is_applied == true) {
		//transfer profile datas
		this->transfer_profile_members2database(ptr_database);
	}
	else {
		ostringstream cout;
		cout << "No temperature model is applied for this river; nothing to transfer!" << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Transfer a hydraulic boundary szenario from file to a database
void HydTemp_Model::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database){
/**
This transfer includes just the boundary conditions of the profiles (_Hyd_River_Profile) 
and the instationary boundary curves (Hyd_Instationary_Boundary). The id of the hyraulic boundary
scenario has to be set before.
*/

	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"Transfer temperature boundary scenario of river model " << this->Param_Temp.Param_RV->get_river_number()<<" ..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	try{
		
		for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			//this->river_profiles[i].transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_RV.get_river_number());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();	
		}
		

	}
	catch(Error msg){
		ostringstream info;
		info << "River model number        : " << this->Param_Temp.Param_RV->get_river_number() << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Delete all data in the database table for the river model parameters as well as the river profile data (static) 
void HydTemp_Model::delete_data_in_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() != _sys_project_type::proj_hyd_temp) {
		return;
	}
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Model::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//delete the table
	HydTemp_Model::general_temp_param_table->delete_data_in_table(ptr_database);

	//delete append table
	HydTemp_Profile::delete_data_in_table(ptr_database);
}
//Input the river model with the index from a database selection of the relevant river models; the general settings as well as the profile data
void HydTemp_Model::input_members(const int index, const QSqlTableModel *query_result,QSqlDatabase *ptr_database, const bool just_profiles, const bool relevant, const bool output_flag){
	if (Sys_Project::get_project_type() != _sys_project_type::proj_hyd_temp) {
		return;
	}

	if(output_flag==true){
		ostringstream cout;
		cout <<"Read in the temperature data of river model "<< this->Param_Temp.Param_RV->get_river_number() <<" from database..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix for output
		ostringstream prefix;
		prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() <<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	}
	this->model_is_applied = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::temp_model_applied)).c_str()).toBool();
	if (this->model_is_applied==false) {

		if (output_flag == true) {
			ostringstream cout;
			cout << "No temperature model is applied for river " << this->Param_Temp.Param_RV->get_river_number() << " in database. Temperature model is not applied!" << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}

		return;

	}

	//read out the results
	this->Param_Temp.gw_temp =query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::gw_temp)).c_str()).toDouble();

	this->Param_Temp.brunt_coef = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::brunt_coef)).c_str()).toDouble();
	this->Param_Temp.view2sky_coef = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::view2sky)).c_str()).toDouble();
	this->Param_Temp.con_bed_coef = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::cond_bed)).c_str()).toDouble();
	this->Param_Temp.bed_temp = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::bed_temp)).c_str()).toDouble();
	this->Param_Temp.bed_warming_coeff = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::bed_warm_coef)).c_str()).toDouble();
	this->Param_Temp.diffusiv_sol_rad_coeff = query_result->record(index).value((HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::diff_solar_rad)).c_str()).toDouble();
	
	
	//read in the profiles
	try{

		QSqlQueryModel prof_query_result;
		int found_profiles = 0;
		//check the general river parameter
		this->Param_Temp.check_members();
		//get the number of profiles
		if(relevant==true){
			found_profiles =HydTemp_Profile::select_relevant_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_Temp.Param_RV->RVNumber, output_flag);
		}
		else{
			found_profiles =HydTemp_Profile::select_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_Temp.Param_RV->RVNumber);
		}
		if (found_profiles == 0) {
			this->model_is_applied = false;
			if (output_flag == true) {
				ostringstream cout;
				cout << "No temperature profiles are found for river " << this->Param_Temp.Param_RV->get_river_number() << " in database. Temperature model is not applied!" << endl;
				this->model_is_applied = false;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				//rewind the prefix
				Sys_Common_Output::output_hyd->rewind_userprefix();
			}
			return;
		}

		//allocate the river profile with number of profiles
		this->allocate_temp_river_profiles();
		

		//read in the profile members
		this->input_profiles_perdatabase(&prof_query_result, ptr_database, output_flag);
		this->model_is_applied = true;
		this->connect_profiles2instat_boundarycurves();
		
		
		

		
	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_Temp.Param_RV->get_river_number() << endl;
		info << "Rivername  : " << this->Param_Temp.Param_RV->get_river_name() << endl;
		msg.make_second_info(info.str());
		if(output_flag==true){
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	if(output_flag==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Create the database table for the general parameters of the river model (static)
void HydTemp_Model::create_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Model::general_temp_param_table == NULL) {
			ostringstream cout;
			cout << "Create general temperature river model data database table..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name = hyd_label::tab_temprv_gen;
			
			const int num_col = 14;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for (int i = 0; i < num_col; i++) {
				tab_col[i].key_flag = false;
				tab_col[i].unsigned_flag = false;
				tab_col[i].primary_key_flag = false;
			}

			tab_col[0].name = hyd_label::genmod_id;
			tab_col[0].type = sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag = true;
			tab_col[0].primary_key_flag = true;

			tab_col[1].name = hyd_label::genmod_name;
			tab_col[1].type = sys_label::tab_col_type_string;

			tab_col[2].name = label::areastate_id;
			tab_col[2].type = sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag = true;
			tab_col[2].key_flag = true;

			tab_col[3].name = label::measure_id;
			tab_col[3].type = sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag = true;
			tab_col[3].key_flag = true;

			tab_col[4].name = label::applied_flag;
			tab_col[4].type = sys_label::tab_col_type_bool;
			tab_col[4].default_value = "true";
			tab_col[4].key_flag = true;

			tab_col[5].name = hyd_label::gw_temp;
			tab_col[5].type = sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag = true;
			tab_col[5].default_value = "283.0";

			tab_col[6].name = hyd_label::brunt_coef;
			tab_col[6].type = sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag = true;
			tab_col[6].default_value = "0.5";

			tab_col[7].name = hyd_label::view2sky;
			tab_col[7].type = sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag = true;
			tab_col[7].default_value = "0.5";

			tab_col[8].name = hyd_label::cond_bed;
			tab_col[8].type = sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag = true;
			tab_col[8].default_value = "15";

			tab_col[9].name = hyd_label::bed_temp;
			tab_col[9].type = sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag = true;
			tab_col[9].default_value = "283";

			tab_col[10].name = hyd_label::bed_warm_coef;
			tab_col[10].type = sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag = true;
			tab_col[10].default_value = "0.5";

			tab_col[11].name = hyd_label::diff_solar_rad;
			tab_col[11].type = sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag = true;
			tab_col[11].default_value = "0.5";

			tab_col[12].name = hyd_label::temp_model_applied;
			tab_col[12].type = sys_label::tab_col_type_bool;
			tab_col[12].default_value = "false";



			tab_col[13].name = label::description;
			tab_col[13].type = sys_label::tab_col_type_string;



			try {
				HydTemp_Model::general_temp_param_table = new Tables();
				if (HydTemp_Model::general_temp_param_table->create_non_existing_tables(tab_name, tab_col, num_col, ptr_database, _sys_table_type::hyd) == false) {
					cout << " Table exists" << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Model::create_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Model::close_table();;
				throw msg;
			}

			HydTemp_Model::close_table();
		}
	}

}
//Set the database table for the general parameters of the temerature river model: it sets the table name and the name of the columns and allocate them (static)
void HydTemp_Model::set_table(QSqlDatabase *ptr_database){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Model::general_temp_param_table == NULL) {
			//make specific input for this class
			const string tab_id_name = hyd_label::tab_temprv_gen;
			string tab_id_col[14];

			tab_id_col[0] = hyd_label::genmod_id;
			tab_id_col[1] = label::areastate_id;
			tab_id_col[2] = label::measure_id;
			tab_id_col[3] = label::applied_flag;
			tab_id_col[4] = label::description;
			tab_id_col[5] = hyd_label::gw_temp;
			tab_id_col[6] = hyd_label::brunt_coef;
			tab_id_col[7] = hyd_label::view2sky;
			tab_id_col[8] = hyd_label::cond_bed;
			tab_id_col[9] = hyd_label::bed_temp;
			tab_id_col[10] = hyd_label::bed_warm_coef;
			tab_id_col[11] = hyd_label::diff_solar_rad;
			tab_id_col[12] = hyd_label::genmod_name;
			tab_id_col[13] = hyd_label::temp_model_applied;

			try {
				HydTemp_Model::general_temp_param_table = new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col) / sizeof(tab_id_col[0]));
				HydTemp_Model::general_temp_param_table->set_name(ptr_database, _sys_table_type::hyd);
			}
			catch (bad_alloc& t) {
				Error msg;
				msg.set_msg("HydTemp_Model::set_table(QSqlDatabase *ptr_database)", "Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info << "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch (Error msg) {
				HydTemp_Model::close_table();
				throw msg;
			}
		}
	}
}
//Close and delete the database table for the general parameters of the temerature river model (static)
void HydTemp_Model::close_table(void){
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (HydTemp_Model::general_temp_param_table != NULL) {
			delete HydTemp_Model::general_temp_param_table;
			HydTemp_Model::general_temp_param_table = NULL;
		}
		//close append tables
		HydTemp_Profile::close_table();
	}
}
//Select and count the number of relevant temerature river models in a database table (static)
int HydTemp_Model::select_relevant_model_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output){
	int number=0;
	try{
		HydTemp_Model::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream prefix;
			prefix << "TEMPRV> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

			ostringstream cout;
			cout << "Search for relevant temperature river models in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);

	}

	//give the table name to the query model
	results->setTable((HydTemp_Model::general_temp_param_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << HydTemp_Model::general_temp_param_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << HydTemp_Model::general_temp_param_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << HydTemp_Model::general_temp_param_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << HydTemp_Model::general_temp_param_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) " ;
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("HydTemp_Model::select_relevant_model_database(QSqlTableModel *results,_sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Model::general_temp_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::genmod_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant temperature river model(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}


	return number;
}
//Select and count the number of relevant temerature river models in a database table with a given river number (static)
int HydTemp_Model::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no){
	try{
		HydTemp_Model::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int number=0;

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << HydTemp_Model::general_temp_param_table->get_table_name();
	test_filter << " where " ;
	test_filter << HydTemp_Model::general_temp_param_table->get_column_name(hyd_label::genmod_id) << " = " << rv_no;
	
	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("HydTemp_Model::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Model::general_temp_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results->rowCount();
//
//
	return number;
}
//Check the number of relevant temerature river models in a database table (static)
int HydTemp_Model::check_relevant_model_database(QSqlDatabase *ptr_database){
	QSqlQueryModel results;
	try{
		HydTemp_Model::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int number=0;

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << HydTemp_Model::general_temp_param_table->get_table_name();
		
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Warning msg;
		msg.set_msg("HydTemp_Model::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << HydTemp_Model::general_temp_param_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results.rowCount();
//
//
	return number;
}
//Get a pointer to the temperature profile by a given index
HydTemp_Profile* HydTemp_Model::get_ptr_profile(const int index) {
	if (index < 0 || index >= this->Param_Temp.Param_RV->get_number_profiles()) {
		return NULL;
	}
	else {
	
			return &this->profiles[index];
		

	}

}
//Clone the members of the temperature model
void HydTemp_Model::clone_model(HydTemp_Model *river){
	ostringstream cout;
	this->system_id=river->get_sys_system_id();
	this->hyd_sz=river->hyd_sz;
	this->Param_Temp=river->Param_Temp;
	_Hyd_Model::clone_model(this);



	//allocate the profile with number of profiles
	this->allocate_temp_river_profiles();

	cout <<"Clone temperature profiles..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		this->profiles[i].clone_profile(&river->profiles[i]);	
	}


	this->connect_profiles2instat_boundarycurves();
	
	cout <<"Cloning of the temperature model is finished"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
//
}
//initialize the temperature profiles
void HydTemp_Model::init_temp_model(QSqlDatabase *ptr_database, const int number_timestep, const double delta_t){
	if (this->model_is_applied == false) {

		return;
	}
	int number_t_step = 0;
	QSqlQueryModel query;
	number_t_step = _Hyd_River_Profile::select_data_in_instat_erg_table(&query, ptr_database, this->system_id, this->hyd_sz.get_id(),  "CA", this->Param_Temp.Param_RV->get_river_number());
	number_t_step = number_t_step / this->Param_Temp.Param_RV->get_number_profiles();

	if (number_t_step != number_timestep) {
		Error msg = this->set_error(3);
		ostringstream info;
		info << "Number timesteps required: " << number_timestep << endl;
		info << "Number timesteps found :   " << number_t_step << endl;
msg.make_second_info(info.str());
throw msg;

	}

	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		this->profiles[i].bound_water_depth.clear_boundary_curve();
		this->profiles[i].bound_water_depth.set_systemid(this->system_id);
		this->profiles[i].bound_discharge.clear_boundary_curve();
		this->profiles[i].bound_discharge.set_systemid(this->system_id);
		this->profiles[i].bound_velocity.clear_boundary_curve();
		this->profiles[i].bound_velocity.set_systemid(this->system_id);
	}
	QList<double> time;
	QList<double> value;

	for (int j = 0; j < this->Param_Temp.Param_RV->get_number_profiles(); j++) {
		time.append(0.0);
		value.append(query.record(j).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max)).c_str()).toDouble());

		for (int i = 0; i < number_t_step; i++) {
			time.append(((i + 1) * delta_t) / 3600.0);
			value.append(query.record(j + (i*this->Param_Temp.Param_RV->get_number_profiles())).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_h_max)).c_str()).toDouble());

		}
		time.append(time.last() + 100000);
		value.append(value.last());
		this->profiles[j].bound_water_depth.set_curve_dircetly(time, value, _hyd_bound_type::waterlevel_type);
		time.clear();
		value.clear();

	}

	time.clear();
	value.clear();

	for (int j = 0; j < this->Param_Temp.Param_RV->get_number_profiles(); j++) {
		time.append(0.0);
		value.append(query.record(j).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max)).c_str()).toDouble());

		for (int i = 0; i < number_t_step; i++) {
			time.append(((i + 1) * delta_t) / 3600.0);
			value.append(query.record(j + (i*this->Param_Temp.Param_RV->get_number_profiles())).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_v_max)).c_str()).toDouble());

		}
		time.append(time.last() + 100000);
		value.append(value.last());
		this->profiles[j].bound_velocity.set_curve_dircetly(time, value, _hyd_bound_type::waterlevel_type);
		time.clear();
		value.clear();

	}

	time.clear();
	value.clear();

	for (int j = 0; j < this->Param_Temp.Param_RV->get_number_profiles(); j++) {
		time.append(0.0);
		value.append(query.record(j).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max)).c_str()).toDouble());

		for (int i = 0; i < number_t_step; i++) {
			time.append(((i + 1) * delta_t) / 3600.0);
			value.append(query.record(j + (i*this->Param_Temp.Param_RV->get_number_profiles())).value((_Hyd_River_Profile::erg_instat_table->get_column_name(hyd_label::proferg_q_max)).c_str()).toDouble());

		}
		time.append(time.last() + 100000);
		value.append(value.last());
		this->profiles[j].bound_discharge.set_curve_dircetly(time, value, _hyd_bound_type::waterlevel_type);
		time.clear();
		value.clear();

	}

	//set distance
	for (int j = 0; j < this->Param_Temp.Param_RV->get_number_profiles(); j++) {
		if (j == 0) {
			this->profiles[j].set_distance2up(0.0);
		}
		else {

			this->profiles[j].set_distance2up(this->ptr_river_model->get_ptr_river_profile(j)->get_distance2upstream());
		}

	}



}
//check the temperature  model
void HydTemp_Model::check_temp_model(const bool output) {
	if (this->model_is_applied == false) {

		return;
	}


	//chek for inlet in first and last!!!
	if (this->profiles[0].get_inlet_temperature_applied() == true) {
		//Error
		Error msg = this->set_error(9);
		ostringstream info;
		info << "First profile"<< endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if (this->profiles[this->Param_Temp.Param_RV->get_number_profiles()-1].get_inlet_temperature_applied() == true) {
		//Error
		Error msg = this->set_error(9);
		ostringstream info;
		info << "Last profile" << endl;
		msg.make_second_info(info.str());
		throw msg;

	}


	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		if (i == 0) {
			this->profiles[i].check_profiles(NULL);

		}
		else if (i==this->Param_Temp.Param_RV->get_number_profiles()-1) {
			this->profiles[i].check_profiles(NULL);
		}
		else {
			this->profiles[i].check_profiles(&this->ptr_river_model->river_profiles[i-1]);
		}


	}

}
//Compare the equality of two temperature models in terms of number of profiles
void HydTemp_Model::compare_models(HydTemp_Model *compare_model){
///**This comparison is primarily important for adding new hydraulic boundary scenarios. It ensures, that 
//the new boundary conditions are set properly to the river model and his profiles. 
//*/
//
//	if(this->number_inbetween_profiles!=compare_model->number_inbetween_profiles){
//		Error msg=this->set_error(18);
//		ostringstream info;
//		info << "Number river profiles in model " << this->Param_RV.RVNumber << ": "<<this->number_inbetween_profiles+2<< endl;
//		info << "Number river profiles in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->number_inbetween_profiles+2<< endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//
//	//compare the river station
//	if(abs(this->inflow_river_profile.get_river_station()-compare_model->inflow_river_profile.get_river_station())>constant::meter_epsilon){
//		Error msg=this->set_error(18);
//		ostringstream info;
//		info << "Inflow riverstation in model " << FORMAT_FIXED_REAL << P(5) << this->Param_RV.RVNumber << ": "<<this->inflow_river_profile.get_river_station()<< label::m<< endl;
//		info << "Inflow riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->inflow_river_profile.get_river_station()<<label::m<< endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//	for(int i=0; i<this->number_inbetween_profiles; i++){
//		if(abs(this->river_profiles[i].get_river_station()-compare_model->river_profiles[i].get_river_station())>constant::meter_epsilon){
//			Error msg=this->set_error(18);
//			ostringstream info;
//			info << "Profile " << i+1<< " riverstation in model " << FORMAT_FIXED_REAL << P(5)<< this->Param_RV.RVNumber << ": "<<this->river_profiles[i].get_river_station()<< label::m<< endl;
//			info << "Profile " << i+1<< " riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->river_profiles[i].get_river_station()<<label::m<< endl;
//			msg.make_second_info(info.str());
//			throw msg;
//		}
//	}
//	if(abs(this->outflow_river_profile.get_river_station()-compare_model->outflow_river_profile.get_river_station())>constant::meter_epsilon){
//		Error msg=this->set_error(18);
//		ostringstream info;
//		info << "Outflow riverstation in model " << FORMAT_FIXED_REAL << P(5) << this->Param_RV.RVNumber << ": "<<this->outflow_river_profile.get_river_station()<< label::m<< endl;
//		info << "Outflow riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->outflow_river_profile.get_river_station()<<label::m<< endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//
}
//Set a new hydraulic boundary scenario id to the temperature models
void HydTemp_Model::set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz){
	this->hyd_sz=new_sz;




}
///Set a pointer to the river model
void HydTemp_Model::set_ptr2RV(Hyd_Model_River *RV_model) {
	this->Param_Temp.Param_RV = &RV_model->Param_RV;
	this->ptr_river_model = RV_model;

}
//output the members
void HydTemp_Model::output_members(void){

	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());


	ostringstream cout;
	if (this->model_is_applied == true) {
		cout << "Output the general parameters..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//output the parameter of the river model
		this->Param_Temp.output_members();

		//output
		cout << "Output the temperature data per river profiles (" << this->Param_Temp.Param_RV->get_number_profiles() << ")..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);


		for(int i=0; i<this->Param_Temp.Param_RV->RVNofProf; i++){
			this->profiles[i].output_members();
		}

	}
	else {
		cout << "No temperature model is applied for river no. " << this->Param_Temp.Param_RV->get_river_number() << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//output the setted and calculated members
void HydTemp_Model::output_setted_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_" << this->Param_Temp.Param_RV->get_river_number() << "> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());


	ostringstream cout;
	if (this->model_is_applied == true) {
		cout << "PROFILE INFOS" << endl;

		Sys_Common_Output::output_hyd->output_txt(&cout);


		//output the river profile
		for (int i = 0; i < this->Param_Temp.Param_RV->RVNofProf; i++) {
			this->profiles[i].output_setted_members();
		}
;
	}
	else {



		cout << "No temperature model is applied for river no. " << this->Param_Temp.Param_RV->get_river_number() << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
///Output the geometrie to paraview as 2d
void HydTemp_Model::output_geometrie2paraview_2d(void) {
	if (this->model_is_applied == false) {
		return;
	}
//
//	//get the file name
//	string filename = this->Param_RV.get_filename_geometrie2file_2d(hyd_label::paraview);
//	//Add file type
//	filename += hyd_label::vtk;
//
//	if (filename == label::not_set) {
//		return;
//	}
//	//open the file
//	this->tecplot_output.open(filename.c_str());
//	if (this->tecplot_output.is_open() == false) {
//		Error msg = this->set_error(7);
//		ostringstream info;
//		info << "Filename " << filename << endl;
//		msg.make_second_info(info.str());
//		throw msg;
//	}
//
//
//
//	//fileheader
//	//output the file header
//	this->tecplot_output << "# vtk DataFile Version 3.0" << endl;
//	this->tecplot_output << "Geometry of river " << this->Param_RV.RVNumber << " with river name " << this->Param_RV.RVName << " (Data of polygon from downstream profile)"<< endl;
//	//this->tecplot_output << "# Element-Type expression: 0 STANDARD_ELEM; 1 COAST_ELEM; 2 NOFLOW_ELEM; 3 NOINFO_ELEM; 4 RIVER_ELEM; 5 OTHER_FP_ELEM; 6 DIKELINE_ELEM" << endl << endl;
//	this->tecplot_output << "ASCII" << endl;
//	this->tecplot_output << "DATASET POLYDATA" << endl;
//	this->tecplot_output << "POINTS " << (this->number_inbetween_profiles +2)*2<< " double" << endl;
//
//	//output points
//	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//	this->tecplot_output  << "  0 "<<endl;
//
//	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//	this->tecplot_output << "  0 " << endl;
//
//
//	for (int i = 0; i < this->number_inbetween_profiles; i++) {
//		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//		this->tecplot_output << "  0 " << endl;
//
//		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//		this->tecplot_output << "  0 " << endl;
//
//	}
//	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//	this->tecplot_output << "  0 " << endl;
//	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
//	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
//	this->tecplot_output << "  0 " << endl;
//	this->tecplot_output << endl;
//
//	this->tecplot_output << "POLYGONS " << this->number_inbetween_profiles+1 << " "<< (this->number_inbetween_profiles + 1)*5 << endl;
//	for (int i = 0; i < (this->number_inbetween_profiles + 1); i++) {
//		this->tecplot_output << "4 " ;
//		if (i == 0) {
//			this->tecplot_output << " 0 2 3 1" << endl;
//
//		}
//		else {
//			int index = i * 2;
//			this->tecplot_output << " "<< index <<" "<< index+2 << " "<< index+3 << " "<< index+1<< endl;
//		}
//
//
//	}
//
//	//output celldata
//	this->tecplot_output << "CELL_DATA " << this->number_inbetween_profiles + 1 << endl;
//	string buff_unit;
//
//	buff_unit = " z_";
//	buff_unit += label::m;
//	buff_unit = functions::clean_white_space(&buff_unit);
//	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
//	this->tecplot_output << "LOOKUP_TABLE default" << endl;
//	for (int i = 0; i < this->number_inbetween_profiles; i++) {
//
//		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_global_z_min() << "  ";
//	}
//
//	//outflow profile
//	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_global_z_min() << "  ";
//	this->tecplot_output << endl ;
//
//	buff_unit = " type_"; 
//	buff_unit += label::no_unit;
//	buff_unit = functions::clean_white_space(&buff_unit);
//	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
//	this->tecplot_output << "LOOKUP_TABLE default" << endl;
//	for (int i = 0; i < this->number_inbetween_profiles; i++) {
//
//		this->tecplot_output << this->river_profiles[i].get_profile_type() << "  ";
//	}
//
//	//outflow profile
//	this->tecplot_output << this->outflow_river_profile.get_profile_type() << "  ";
//	this->tecplot_output << endl;
//
//	buff_unit = " init_h_";
//	buff_unit += label::m;
//	buff_unit = functions::clean_white_space(&buff_unit);
//	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
//	this->tecplot_output << "LOOKUP_TABLE default" << endl;
//	for (int i = 0; i < this->number_inbetween_profiles; i++) {
//
//		this->tecplot_output << this->river_profiles[i].get_init_condition_value() << "  ";
//	}
//
//	//outflow profile
//	this->tecplot_output << this->outflow_river_profile.get_init_condition_value() << "  ";
//	this->tecplot_output << endl;
//
//	//close the file
//	this->tecplot_output.close();
//	this->tecplot_output.clear();
}
//Output the result members for each temperature profile at every timestep
void HydTemp_Model::output_result_members_per_timestep(void){
	if (this->model_is_applied == false) {
		return;
	}
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	HydTemp_Profile::output_result_members_per_timestep_header();
	ostringstream cout;
	for(int i=0; i< this->Param_Temp.Param_RV->RVNofProf; i++){
		this->profiles[i].output_result_members_per_timestep(&cout, this->ptr_river_model->get_ptr_river_profile(i)->get_river_station());
		cout << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
		cout.clear();
	}

	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the result members per timestep to csv as 1d
void HydTemp_Model::output_result2csv_1d(const double timepoint, const int timestep_number) {
	if (this->model_is_applied == false) {
		return;
	}
//
	// for 1d - output
	//results per timesteps
	ofstream output_csv;
	output_csv.clear();
	if (this->Param_Temp.Param_RV->tecplot_outfile_name_1d != label::not_set) {
		//open the file
		string buffer = this->Param_Temp.get_filename_result2file_1d(hyd_label::paraview);
		//Add time and file type
		buffer += "_";
		int time = (int)timepoint;
		buffer += std::to_string(time);
		buffer += hyd_label::csv;
		output_csv.open(buffer.c_str());
		//check if it is open
		if (output_csv.is_open() == false) {
			Error msg = this->set_error(6);
			ostringstream info;
			info << "File name " << this->Param_Temp.get_filename_result2file_1d(hyd_label::paraview) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else {
			//output the file header
			output_csv << " Riverstation " << label::m << "," << " z_min " << label::m << "," << " Water_Temp " << label::kelvin << ",";
			//output_csv << "Air_Temp" << label::kelvin << "," << "Solar_radiation" << label::watt_per_square_m << ",";
			//output_csv << "Humidity" << label::no_unit << ",";
			//output_csv << "Wind_speed" << label::m_per_sec << ",";
			//output_csv << "Cloudness" << label::no_unit << ",";
			//output_csv << "Shadow" << label::no_unit << ",";
			//output_csv << "Inlet_Temp" << label::kelvin <<"," ;
			output_csv << "Phi_total" << label::watt_per_square_m << ",";
			output_csv << "Phi_bed" << label::watt_per_square_m << ",";
			output_csv << "Phi_cond" << label::watt_per_square_m << ",";
			output_csv << "Phi_eva" << label::watt_per_square_m << ",";
			output_csv << "Phi_lw" << label::watt_per_square_m << ",";
			output_csv << "Phi_solar" << label::watt_per_square_m<<",";
			//output_csv << "Q" << label::qm_per_sec << ",";
			//output_csv << "v" << label::m_per_sec << ",";
			//output_csv << "s" << label::m << ",";
			output_csv << "h" << label::m <<endl;
			



			output_csv <<  endl;
		}
		
	}

	//output per profile
	if (output_csv.is_open() == true) {

		//output intflow profile

		for (int i = 0; i < this->Param_Temp.Param_RV->RVNofProf; i++) {
			this->profiles[i].output_result_members_per_timestep2csv(&output_csv, this->ptr_river_model->get_ptr_river_profile(i)->get_river_station(), this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_global_z_min());
		}

		output_csv.flush();
		output_csv.close();
	}
}
//Output the result members per timestep to paraview as 2d
void HydTemp_Model::output_result2paraview_2d(const double timepoint, const int timestep_number) {
	if (this->model_is_applied == false) {
		return;
	}
//
	//get the file name
	string filename = this->Param_Temp.get_filename_result2file_2d(hyd_label::paraview);
	//Add file type
	int time = (int)timepoint;
	filename += "_";
	filename += std::to_string(time);
	filename += hyd_label::vtk;

	if (filename == label::not_set) {
		return;
	}
	ofstream output;
	//open the file
	output.open(filename.c_str());
	if (output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	output << "# vtk DataFile Version 3.0" << endl;
	output << "Values of river " << this->Param_Temp.Param_RV->RVNumber << " with river name " << this->Param_Temp.Param_RV->RVName << " time "<< timepoint <<" (Data of polygon from downstream profile)" << endl;
	output << "ASCII" << endl;
	output << "DATASET POLYDATA" << endl;
	output << "POINTS " << (this->Param_Temp.Param_RV->get_number_profiles()) * 2 << " double" << endl;

	//output points



	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		output << "  0 " << endl;

		output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		output << "  0 " << endl;

	}

	output << endl;

	output << "POLYGONS " << this->Param_Temp.Param_RV->get_number_profiles() - 1 << " " << (this->Param_Temp.Param_RV->get_number_profiles() - 1) * 5 << endl;
	for (int i = 0; i < (this->Param_Temp.Param_RV->get_number_profiles() - 1); i++) {
		output << "4 ";
		if (i == 0) {
			output << " 0 2 3 1" << endl;

		}
		else {
			int index = i * 2;
			output << " " << index << " " << index + 2 << " " << index + 3 << " " << index + 1 << endl;
		}


	}

	//output celldata
	output << "CELL_DATA " << this->Param_Temp.Param_RV->get_number_profiles() - 1 << endl;
	string buff_unit;

	buff_unit = " Water_temp_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].temp_current << "  ";
	}

	output << endl;





	buff_unit = " Air_Temp_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].air_temp.current_value << "  ";
	}

	output << endl;

	buff_unit = " Solar_radiation_";
	buff_unit += label::watt_per_square_m;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].solar_rad.current_value << "  ";
	}

	output << endl;




	buff_unit = " Humidity_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].humid.current_value << "  ";
	}

	output << endl;

	buff_unit = " Wind_speed_";
	buff_unit += label::m_per_sec;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].wind.current_value<< "  ";
	}

	output << endl;

	buff_unit = " Cloudness_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].wind.current_value << "  ";
	}

	output << endl;

	buff_unit = " Shadow_";
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].wind.current_value << "  ";
	}

	output << endl;

	buff_unit = " Inlet_Temp_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		output << this->profiles[i].wind.current_value << "  ";
	}

	output << endl;


	output.clear();
	output.close();

}
//Output the result members per timestep to databse as 2d
void HydTemp_Model::output_result2database_2d(QSqlDatabase *ptr_database, const string break_sz, const double timepoint, const int timestep_number, const string time) {
	if (this->model_is_applied == false) {
		return;
	}
//
//
	//the table is set (the name and the column names) and allocated
	try {
		HydTemp_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	int glob_id = HydTemp_Profile::erg_instat_table->maximum_int_of_column(HydTemp_Profile::erg_instat_table->get_column_name(label::glob_id), ptr_database) + 1;

	//polygon string
	string poly_buffer;


	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {
		this->ptr_river_model->set_river_section_polygon(i);
		poly_buffer = this->ptr_river_model->river_section_polygon.get_polygon2sql_string();
		this->profiles[i].output_instat_results(ptr_database, this->Param_Temp.Param_RV->RVNumber, poly_buffer, &glob_id, break_sz, time);
	}

//
}
//output solver errors
void HydTemp_Model::output_solver_errors(const double time_point, const int step_counter, const string timestring, const string realtime, const double diff_time, const int total_internal, const int internal_steps){
	if (this->model_is_applied == false) {
		return;
	}
//
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << W(2) << step_counter ;
	cout << W(15) << P(1)<< FORMAT_FIXED_REAL << time_point;
	cout << W(15) << timestring;
	cout << W(15) << realtime;
	cout << P(1)<< FORMAT_FIXED_REAL<<"  (" << diff_time<<")  ";
	cout << W(15) << this->get_number_solversteps() ;
	cout << "  ("<<this->get_number_solversteps()-this->diff_solver_steps<<")   ";
	cout << W(12) << P(1)<< FORMAT_SCIENTIFIC_REAL<<  this->norm_estim_error;
	cout << W(12) << P(1)<< FORMAT_SCIENTIFIC_REAL<< this->max_estim_error;
	cout << W(12) << total_internal;
	cout << "  ("<<internal_steps <<")";
	cout << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->diff_solver_steps=this->get_number_solversteps();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//output final statistics of the temperature model
void HydTemp_Model::output_final(void){	

//
	//set prefix for output
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number() <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	if (this->model_is_applied == true) {
		//solver
		this->output_solver_statistics();
		//hydrological balance
		//this->calculate_total_hydrological_balance();
		//is there a balance possible ??? TODO UDO???
		//this->output_hydrological_balance();
		//output the maximum values to console/display
		this->output_maximum_results();
	}
	else {
		cout << "No temperature model is applied for river no. " << this->Param_Temp.Param_RV->get_river_number() << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the maximum result members to csv
void HydTemp_Model::output_result_max2csv(void) {
	if (this->model_is_applied == false) {
		return;
	}
	//maximum results
	if (this->Param_Temp.Param_RV->tecplot_outfile_name_1d != label::not_set) {
		//close the result file per time step
		this->tecplot_output.close();
		this->tecplot_output.clear();

		//reopen the file
		string buffer = this->Param_Temp.get_filename_result2file_1d_maxvalues(hyd_label::paraview);
		//Add file type
		buffer += hyd_label::csv;
		this->tecplot_output.open(buffer.c_str());
		//check if it is open
		if (this->tecplot_output.is_open() == false) {
			Error msg = this->set_error(11);
			ostringstream info;
			info << "File name " << this->Param_Temp.get_filename_result2file_1d_maxvalues(hyd_label::paraview) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//output the file header
		this->tecplot_output << " Riverstation " << label::m;
		this->tecplot_output << " z_min  " << label::m << W(15);
		this->tecplot_output <<  ", Water_Temp_max" << label::kelvin << W(15);
		this->tecplot_output << ", t(Temp_max)" << label::sec << W(15);
		this->tecplot_output << ", Water_Temp_min" << label::kelvin << W(15);
		this->tecplot_output << ", t(Temp_min)" << label::sec << W(15);
		this->tecplot_output << ", Water_Temp_average" << label::kelvin << W(15);

		this->tecplot_output << endl;



		//inbetween profiles
		for (int i = 0; i < this->Param_Temp.Param_RV->RVNofProf; i++) {
			this->profiles[i].output_max_results2csvfile(&this->tecplot_output, this->ptr_river_model->get_ptr_river_profile(i)->get_river_station(), this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_global_z_min());

		}
		//close it
		this->tecplot_output.close();

	}

}
//Output the maximum result members to paraview as 2d
void HydTemp_Model::output_result_max2paraview2d(void) {
	if (this->model_is_applied == false) {
		return;
	}
	//get the file name
	string filename = this->Param_Temp.get_filename_result2file_2d_maxvalues(hyd_label::paraview);
	//Add file type
	filename += hyd_label::vtk;

	if (filename == label::not_set) {
		return;
	}
	//open the file
	this->tecplot_output.open(filename.c_str());
	if (this->tecplot_output.is_open() == false) {
		Error msg = this->set_error(7);
		ostringstream info;
		info << "Filename " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//fileheader
	//output the file header
	this->tecplot_output << "# vtk DataFile Version 3.0" << endl;
	this->tecplot_output << "Maximum values of river " << this->Param_Temp.Param_RV->get_river_number() << " with river name " << this->Param_Temp.Param_RV->get_river_name() << " (Data of polygon from downstream profile)" << endl;
	this->tecplot_output << "ASCII" << endl;
	this->tecplot_output << "DATASET POLYDATA" << endl;
	this->tecplot_output << "POINTS " << (this->Param_Temp.Param_RV->get_number_profiles()) * 2 << " double" << endl;

	//output points



	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		this->tecplot_output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

		this->tecplot_output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->ptr_river_model->get_ptr_river_profile(i)->typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

	}

	this->tecplot_output << endl;

	this->tecplot_output << "POLYGONS " << this->Param_Temp.Param_RV->get_number_profiles() - 1 << " " << (this->Param_Temp.Param_RV->get_number_profiles() - 1) * 5 << endl;
	for (int i = 0; i < (this->Param_Temp.Param_RV->get_number_profiles() - 1); i++) {
		this->tecplot_output << "4 ";
		if (i == 0) {
			this->tecplot_output << " 0 2 3 1" << endl;

		}
		else {
			int index = i * 2;
			this->tecplot_output << " " << index << " " << index + 2 << " " << index + 3 << " " << index + 1 << endl;
		}


	}

	//output celldata
	this->tecplot_output << FORMAT_FIXED_REAL << P(2) << "CELL_DATA " << this->Param_Temp.Param_RV->get_number_profiles() - 1 << endl;
	string buff_unit;

	buff_unit = " Temp_max_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		this->tecplot_output << this->profiles[i].temp_value_max.maximum<< "  ";
	}
	
	this->tecplot_output << endl;

	buff_unit = " Temp_min_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		this->tecplot_output << this->profiles[i].temp_value_min.maximum << "  ";
	}
	this->tecplot_output << endl;

	buff_unit = " Temp_av_";
	buff_unit += label::kelvin;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles()-1; i++) {

		this->tecplot_output << this->profiles[i].temp_average << "  ";
	}
	this->tecplot_output << endl;
	

	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();



}
//Output the maximum result members to a database table
void HydTemp_Model::output_result_max2database(QSqlDatabase *ptr_database, const string break_sz){
	if (this->model_is_applied == false) {
		return;
	}
//
	//the table is set (the name and the column names) and allocated
	try{
		HydTemp_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id= HydTemp_Profile::erg_table->maximum_int_of_column(HydTemp_Profile::erg_table->get_column_name(label::glob_id), ptr_database)+1;

	//polygon string
	string poly_buffer;


	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles()-1;i++){
		this->ptr_river_model->set_river_section_polygon(i);
		poly_buffer= this->ptr_river_model->river_section_polygon.get_polygon2sql_string();
		this->profiles[i].output_max_results(ptr_database, this->Param_Temp.Param_RV->RVNumber, poly_buffer, &glob_id, break_sz); 
	}

}
//Reset all coupling values of the model; use it before syncronization
void HydTemp_Model::reset_coupling_values(void){

	

	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		//this->profiles[i].reset_coupling_discharge();
	}
	
}
//Reset the model to the state before the calculation
void HydTemp_Model::reset_model(Hyd_Param_Global *global_params){
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Reset river model and reinit solver..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	this->reset_coupling_values();
	
	this->reset_total_hydrological_balance();

	//inbetween profiles
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		this->profiles[i].reset_hydrobalance_maxvalues();
	}


	this->warn_counter=0;
	this->tol_reduce_counter=0;
	this->close_tecplot_output();
	this->change_abs_tol=this->setted_abs_tol;
	this->change_rel_tol=this->setted_rel_tol;


	this->reinit_solver(global_params);



	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Clear the boundary condition, which are set; boundary conditions due to interception are not cleared
void HydTemp_Model::clear_boundary_condition(const bool with_output){
	if (this->model_is_applied == false) {
		return;
	}
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if(with_output==true){
		cout << "Clear temperature model boundary condition..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

 
	this->Param_Temp.Param_RV->number_instat_boundary=0;



	//inbetween profiles
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		this->profiles[i].reset_boundary_data();
	}


	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
void HydTemp_Model::set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database){
	if (this->model_is_applied == false) {
		return;
	}
	ostringstream prefix;
	prefix << "TEMPRV_"<< this->Param_Temp.Param_RV->get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{	
		//instationary boundary
		this->ptr_river_model->read_instat_boundary_curves(output_flag, ptr_database);

		QSqlTableModel prof_query_result(0, *ptr_database);
		//get the number of profiles
		int number=HydTemp_Profile::select_relevant_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_Temp.Param_RV->RVNumber, output_flag);

		if(this->Param_Temp.Param_RV->RVNofProf!=number){
			Error msg=this->set_error(19);
			ostringstream info;
			info << "Table name               : " << HydTemp_Model::general_temp_param_table->get_table_name() << endl;
			info << "Number found profiles    : " << number << endl;
			info << "Number required profiles : " << this->Param_Temp.Param_RV->RVNofProf<< endl;
			msg.make_second_info(info.str());
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}



		//boundary data
		int glob_id=-1;
		//read out the results of the request for this index
		//glob_id=prof_query_result.record(0).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

		for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
			glob_id=prof_query_result.record(i).value((HydTemp_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
			this->profiles[i].hyd_sz.set_members(&this->hyd_sz);
			this->profiles[i].input_boundary_data_per_database(i, ptr_database, glob_id);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		


		//connect it 
		this->connect_profiles2instat_boundarycurves();
		//check boundary
		this->check_instationary_boundary_found();
	}
	catch(Error msg){

		ostringstream info;
		info << "Rivernumber: " << this->Param_Temp.Param_RV->RVNumber << endl;
		info << "Rivername  : " << this->Param_Temp.Param_RV->RVName << endl;
		msg.make_second_info(info.str());
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//make the syncronisation between the models and the boundaries
void HydTemp_Model::make_syncronisation(const double time_point){
	if (this->model_is_applied == false) {
		return;
	}

	//for the inbetween profiles
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		
		if (i == 0) {
			this->profiles[i].make_syncronisation(i, time_point, &this->Param_Temp, NULL, &this->profiles[i + 1], NULL);
		}
		else if (i== this->Param_Temp.Param_RV->get_number_profiles()-1) {
			this->profiles[i].make_syncronisation(i, time_point, &this->Param_Temp, &this->profiles[i - 1], NULL, NULL);

		}
		else {
			this->profiles[i].make_syncronisation(i, time_point, &this->Param_Temp, &this->profiles[i - 1], &this->profiles[i + 1], &this->ptr_river_model->river_profiles[i-1]);
		}
		
	}

}
//Calculate the balance and the maximum values of the model
void HydTemp_Model::make_hyd_balance_max(const double time_point) {
		if (this->model_is_applied == false) {
			return;
		}

		//for the inbetween profiles
		for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {

			//max values!!
			this->profiles[i].calculate_temp_balance(time_point);

		}
}
//Reset the solver of the model
void HydTemp_Model::reset_solver(void){
//	if(this->inflow_river_profile.typ_of_profile!=NULL){
//		this->set_solver_statistics();
//		CVodeReInit(this->cvode_mem,this->inflow_river_profile.typ_of_profile->get_old_time_point(), this->results);
//		this->no_reset++;
//	}
}
//solve_model
void HydTemp_Model::solve_model(const double next_time_point, const string system_id){
	
	if (this->model_is_applied == false) {
		return;
	}
	try{
		//run the solver
		this->run_solver(next_time_point, system_id);

		for(int i=0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++){
		
			this->profiles[i].set_actuel_temp_from_solver((this->results_real[i]));
		}
		
	}
	catch(Error msg){
		ostringstream info;
		info << "Time point  :" << next_time_point << endl;
		info << "Rivernumber :" << this->Param_Temp.Param_RV->RVNumber << endl;
		string buffer = this->Param_Temp.get_filename_result2file_1d(hyd_label::paraview);
		//Add time and file type
		buffer += "_";
		int time = (int)next_time_point;
		buffer += std::to_string(time);
		buffer += hyd_label::csv;
		info << "Exception output 1d-TEMPRV results are written in " << buffer << endl;
		info << "Please check it for unplausible results; check the 1d-TEMPRV model " <<  endl;
		this->output_result2csv_1d(next_time_point, -999);
		msg.make_second_info(info.str());
		throw msg;
	}	
}
//Close the output files for a tecplot output
void HydTemp_Model::close_tecplot_output(void){
//
	if(this->tecplot_output_2d.is_open()==true){
		this->tecplot_output_2d.close();
	}
	if(this->tecplot_output_2d_geo.is_open()==true){
		this->tecplot_output_2d_geo.close();
	}
}
//Get the pointer to the profile by a given index
HydTemp_Profile* HydTemp_Model::get_ptr_river_profile(const int index) {
	if (index < 0 || index >= this->Param_Temp.Param_RV->get_number_profiles()) {
		return NULL;
	}
	else {	
		return &this->profiles[index - 1];
	}
}
//Get the pointer to the profile by a given number
HydTemp_Profile* HydTemp_Model::get_ptr_river_profile_number(const int number) {

	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		if (this->profiles[i].get_profile_number() == number) {
			return &this->profiles[i];
		}
	}

	return NULL;
}
//Set the folder for the file-output in case of an database based calculation
void HydTemp_Model::set_output_folder(const string folder) {
	//nothing to do
}
//_____________________________
//private
//Allocate the river profile for temperature data 
void HydTemp_Model::allocate_temp_river_profiles(void) {
		//number of section between the profiles
	this->NEQ = this->Param_Temp.Param_RV->get_number_profiles();
    this->NEQ_real=this->NEQ;
	

	//1-dimensional=> half bandwidth=1
	this->half_bandwidth=1;

	try{
		this->profiles =new HydTemp_Profile[this->Param_Temp.Param_RV->get_number_profiles()];
	}
	catch(bad_alloc &t){
		ostringstream info;
		Error msg=this->set_error(0);
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for (int i = 0; i < this->Param_Temp.Param_RV->get_number_profiles(); i++) {
		this->profiles[i].hyd_sz.set_members(&this->hyd_sz);

	}

}
//Input the river profile temperature data per file
void HydTemp_Model::input_river_profiles_temp_data_perfile(void){
	//a profile is found
	bool found_flag=false;
	//file
	QFile profile_file;
	//count the lines in file
	int line_counter=0;
	//profile counter
	int profile_counter=0;

	//open file
	profile_file.setFileName(this->Param_Temp.temp_profile_File.c_str());
	QIODevice::OpenMode my_flag;
	my_flag=my_flag|(QIODevice::ReadOnly);
	my_flag=my_flag|(QIODevice::Text);
	profile_file.open(my_flag);


	if(profile_file.isOpen()==false){
		Error msg=this->set_error(1);
		throw msg;
	}
	

	try{
		//read in each profile
		for(int i=0; i<this->Param_Temp.Param_RV->get_number_profiles(); i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			//search for the begin of the zone
			string myline;
			QString qmyline;
			stringstream my_stream;
			string key;
			do{
				//getline(profile_file, myline,'\n');
				qmyline=profile_file.readLine();
				myline=qmyline.toStdString();
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				my_stream << myline;
				my_stream >> key;
				my_stream.clear();
				my_stream.str("");
				if(key=="ZONE"){
					found_flag=true;
					profile_counter++;
					break;	
				}
			}
			while(profile_file.atEnd()!=true);
			//nothing is found in the file
			if(found_flag==false){
				Error msg=this->set_error(2);
				throw msg;
			}
			//read in the profile specific data 
			this->profiles[i].input_members(&profile_file, i, myline, &line_counter);

		}//end loop over profiles


		if(profile_counter< this->Param_Temp.Param_RV->get_number_profiles()){
			Error msg=this->set_error(8);
			ostringstream info;
			info <<"Founded numbers of profile :" << profile_counter+1 << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}catch(Error msg){
		profile_file.close();
		ostringstream info;
		info << "Filename   : " << this->Param_Temp.temp_profile_File.c_str() <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	profile_file.close();
}
///Transfer the temperature profile data to a database
void HydTemp_Model::transfer_profile_members2database(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream cout;
	if(with_output==true){
		cout <<"Transfer temperature data of " << this->NEQ+1 << " profiles of the river model to database ..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	int id_glob = 0;
	//HydTemp_Profile::get_max_glob_id_prof_table(ptr_database) + 1;
	QSqlQuery model(*ptr_database);

	//inbetween
	for(int i=0; i<this->Param_Temp.Param_RV->get_number_profiles(); i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		
		if(i%100==0 && i>0){
			if(with_output==true){
				cout << "Transfer temperature data of river profile "<< i+1 <<" to " << i+100 <<" to database..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}
		}
		id_glob = this->ptr_river_model->get_ptr_river_profile(i)->get_prof_glob_id();

		this->profiles[i].transfer_profile_members2database(id_glob, &model, ptr_database, this->Param_Temp.Param_RV->get_river_number());
		//id_glob++;
	}
	
}
//Input the temperature profile data per database
void HydTemp_Model::input_profiles_perdatabase(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag){
	if(output_flag==true){
		ostringstream cout;
		cout <<"Read in temperature data of " << this->Param_Temp.Param_RV->get_number_profiles() << " profiles of the river model from database ..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->profiles[i].input_members_per_database(i,query_result, ptr_database);
	}

}
//set the init condition to the result vector
void HydTemp_Model::set_initcond2resultvector(void){
//
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles();i++){
		this->profiles[i].set_init_condition();
	}
 
	for(int i=0; i< this->Param_Temp.Param_RV->get_number_profiles(); i++){
		this->results_real[i]=this->profiles[i].get_init_condition_value();
	}
}
//connet the profiles to the instat boundary curves
void HydTemp_Model::connect_profiles2instat_boundarycurves(void){	
	ostringstream cout;
	cout <<"Connect the temperature profiles with the instationary boundary curves..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	

	//connect all profiles in between (point wise)
	for(int j=0; j< this->Param_Temp.Param_RV->get_number_profiles(); j++){
		try{
			this->profiles[j].connect_instat_boundarycurve(this->ptr_river_model->instat_boundary_curves, this->Param_Temp.Param_RV->number_instat_boundary);
		}
		catch(Error msg){
			ostringstream info;
			info <<" Profilenumber "<< j << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//check instationary boundary found
void HydTemp_Model::check_instationary_boundary_found(void){

	//check if all are found (starts by one because the first profile can not have any lateral boundary condition)
	for(int j=0; j< this->Param_Temp.Param_RV->get_number_profiles(); j++){//loop over all elems
		if(this->profiles[j].get_instat_boundarycurve_found()!=true){
			Error msg=this->set_error(4);
			ostringstream info;
			info <<"Profilenumber "<< j+1 << endl;
			info <<"Profilename   "<< this->profiles[j].get_profile_name() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}


}
//set function to solve to the solver
void HydTemp_Model::set_function2solver(void){
	int flag=-1;
	//set the function where the diff equation is specified
	//The function CVodeMalloc provides required problem and solution speci¯cations, allocates internal memory, and initializes cvode.
	flag = CVodeInit(this->cvode_mem, ftemp_equation2solve, 0.0, this->results);

		
	this->setted_rel_tol= this->Param_Temp.Param_RV->get_relative_solver_tolerance();
	this->setted_abs_tol=*(this->Param_Temp.Param_RV->get_absolute_solver_tolerance());
	this->change_abs_tol=this->setted_abs_tol;
	this->change_rel_tol=this->setted_rel_tol;
	
	if(flag<0){	
		Error msg=this->set_error(15);
		ostringstream info;
		info <<"Solver function: CVodeMalloc(this->cvode_mem, f1D_equation2solve, global_params->get_startime(), this->results, CV_SS, this->Param_RV.get_relative_solver_tolerance(), this->Param_RV.get_absolute_solver_tolerance())"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//The function CVodeSetFdata specifies the user data block f data, for use by the user right-hand side function f, and attaches it to the main cvode memory block
    //flag = CVodeSetFdata(this->cvode_mem, this);
	flag = CVodeSetUserData(this->cvode_mem, this);
	if(flag<0){
		Error msg=this->set_error(15);
		ostringstream info;
		info <<"Solver function: CVodeSetFdata(this->cvode_mem, this)"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	flag=CVodeSStolerances(this->cvode_mem, this->Param_Temp.Param_RV->get_relative_solver_tolerance(),*this->Param_Temp.Param_RV->get_absolute_solver_tolerance());
	if(flag<0){
		Error msg=this->set_error(15);
		ostringstream info;
		info <<"Solver function: VodeSStolerances(this->cvode_mem, this->Param_RV.get_relative_solver_tolerance(),*this->Param_RV.get_absolute_solver_tolerance())"<< endl;
		info <<"CVode: " << this->cvode_init_flags2string(flag) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//output final statistics of the solver
void HydTemp_Model::output_solver_statistics(void){
	//set prefix for output
	ostringstream cout;
	cout << "SOLVER" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	_Hyd_Model::output_solver_statistics(&cout);

}
//Output the maximum results
void HydTemp_Model::output_maximum_results(void){
//
	HydTemp_Profile::output_header_max_results();

	for (int i = 0; i < this->Param_Temp.Param_RV->RVNofProf; i++) {
		double river_station;
		river_station = this->ptr_river_model->get_ptr_river_profile(i)->get_river_station();
		this->profiles[i].output_max_results(river_station);
	}

}
//Output the hydrological balance of the floodplain model
void HydTemp_Model::output_hydrological_balance(void){
	//todo UDO
//	ostringstream cout;
//
//	this->calculate_total_hydrological_balance();
//
//	cout << "HYDROLOGICAL BALANCE " << label::cubicmeter << endl;
//	Sys_Common_Output::output_hyd->output_txt(&cout);
//	cout << " Total watervolume init-cond.             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_watervolume_init<< endl;
//	cout << " Total watervolume left                   :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_watervolume<< endl;
//	if(abs(this->total_volume_error_zero_outflow)>constant::zero_epsilon){
//		cout << "  Total error dry outflow                 :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_volume_error_zero_outflow<< endl;
//	}
//	cout << " Total volume inflow profile              :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->inflow_river_profile.get_inflow_volume()<< endl;
//	if(abs(this->inflow_river_profile.get_watervolume_difference_direct_coupling())>constant::zero_epsilon){
//		cout << "  Error volume inflow (h_bound)           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->inflow_river_profile.get_watervolume_difference_direct_coupling()<< endl;
//	}
//	cout << " Total volume outflow profile             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->outflow_river_profile.get_outflow_volume()<< endl;
//	if(abs(this->outflow_river_profile.get_watervolume_difference_direct_coupling())>constant::zero_epsilon){
//		cout << "  Error volume outflow (h_bound)          :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->outflow_river_profile.get_watervolume_difference_direct_coupling()<< endl;
//	}	
//	Sys_Common_Output::output_hyd->output_txt(&cout,true);
//	cout << " Sum of volumes                           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL << this->sum_volumes << endl;
//	cout << " Error [%]                                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL << this->error_hydrological_balance << endl;
//	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Calculate the hydrological balance of the floodplain model
void HydTemp_Model::calculate_total_hydrological_balance(void){

	//todo UDO
//	//reset the values
//	this->reset_total_hydrological_balance();
//
//	//calculate the init watervolume and the water volume at this time point
//	this->total_watervolume_init=(this->inflow_river_profile.get_watervolume_init());
//	this->total_watervolume=(this->inflow_river_profile.get_watervolume_element());
//	for(int i=0; i< this->number_inbetween_profiles; i++){
//		this->total_watervolume_init=this->total_watervolume_init+this->river_profiles[i].get_watervolume_init();
//		this->total_watervolume=this->total_watervolume+this->river_profiles[i].get_watervolume_element();
//	}
//

}
//Reset the total hydraological balance values
void HydTemp_Model::reset_total_hydrological_balance(void){
	//todo UDO
//	//reset the values
//	this->total_watervolume=0.0;
//	this->sum_volumes=0.0;
//	this->total_lateral_boundary_volume.volume_in=0.0;
//	this->total_lateral_boundary_volume.volume_out=0.0;

}
//Get the model description
string HydTemp_Model::get_model_description(void){
	ostringstream txt;
	txt << "Rivermodel " << this->Param_Temp.Param_RV->get_river_number() <<", name : " << this->Param_Temp.Param_RV->get_river_name() <<endl;
	return txt.str();
}
//Set warning(s)
Warning HydTemp_Model::set_warning(const int warn_type){
		string place="HydTemp_Model::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://the distances given and geometrical do not correspond; the given one is greater 1.5 times 
			place.append("check_river_model(void)");
			reason="The given distance between the profiles is two time greater (tolerance) than the geometrial distances";
			help="Check the geometry of the river model";
			//info << "River model: " << this->Param_RV.RVName << "  No. " << this->Param_RV.RVNumber << endl;
			reaction="No reaction";
			type=12;	
			break;
		case 1://the distances given and geometrical do not correspond; the given one is smaller 
			place.append("check_river_model(void)");
			reason="The given distance between the profiles is half times smaller (tolerance) than the geometrial distances";
			help="Check the geometry of the river model";
			//info << "River model: " << this->Param_RV.RVName<<"  No. "<< this->Param_RV.RVNumber << endl;
			reaction="No reaction";
			type=12;
			break;
		case 2://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general parameters of the river model to the database";
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
};
//set the error
Error HydTemp_Model::set_error(const int err_type){
		string place="HydTemp_Model::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_temp_river_profiles(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://geometry file is not open
			place.append("input_river_profiles_perfile(void)");
			reason="Can not open the file of river profiles";
			help="Check the file";
			//info << "Filename is: " << this->Param_RV.RVGeomtryFile << endl;
			type=5;
			break;
		case 2://no zone found
			place.append("input_river_profiles_perfile(void)");
			reason="No Profile zone found";
			help="Check the file";
			//info << "Filename is: " << this->Param_RV.RVGeomtryFile << endl;
			type=1;
			break;
		case 3://not all curves wehere found by the connection
			place.append("connect_profiles2instat_boundarycurves(void)");
			reason = "Not all boundary curves are found";
			help = "Check the number of boundary curves in boundary file and in the main file";
			type = 1;
			break;
		case 4://not all curves wehere found by the connection
			place.append("init_temp_model(QSqlDatabase *ptr_database, const int number_timestep)");
			reason="Not enough instationary timesteps are found in database";
			help="Please check the time settings";
			type=2;
			break;
		case 6://could not open the tecplot file
			place.append("output_result2csv_1d(const double timepoint, const int timestep_number)");
			reason = "Could not open the file for the paraview output of the temperature river model";
			help = "Check the file";
			type = 5;
			break;
		case 7://could not open the  file
			place.append("output_geometrie2paraview_2d(void)");
			reason = "Could not open the file for the paraview output of the temperature river model";
			help = "Check the file";
			type = 5;
			break;
		case 8://not all profiles are found
			place.append("input_river_profiles_temp_data_perfile(void)");
			reason="Not all profiles are found";
			help="Check the profile file and the setted number of profiles in the global file for this river number";
			type=15;
			break;
		case 9://inlet boundary at first/last profiel
			place.append("check_temp_model(const bool output)");
			reason = "There is an inlet temperature boundary found at the first/last profile";
			help = "At the first/last profile no temperature inlet boundary is applied; a inlet temperature boudnary is requierd!";
			type = 15;
			break;
		case 15://problem with the setting of the function to the solver
			place.append("set_function2solver(Hyd_Param_Global *global_params)");
			reason = "Can not set the function to solve to the solver";
			help = "Check the solver settings";
			type = 17;
			break;
		case 18://the two river models are not equal
			place.append("compare_models(HydTemp_Model *compare_model)");
			reason = "The checked river models are not equal";
			help = "Check the river models and ensure their equality";
			type = 15;
			break;
		case 19://the number of profile do not match to the founded number in database
			place.append("set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database)");
			reason = "The number of profile found in database do not equal to the given number";
			help = "Check the table";
			type = 2;;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason = "Unknown flag!";
			help = "Check the flags";
			type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//___________________________________________
//static Main function for temperature modelling
#ifdef _WIN32
int __cdecl ftemp_equation2solve(realtype time, N_Vector results, N_Vector da_dt, void* river_data) {
#elif defined(__unix__) || defined(__unix)
int __attribute__((cdecl)) ftemp_equation2solve(realtype time, N_Vector results, N_Vector da_dt, void* river_data) {
#endif
	//
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	//cast the floodplain_data
	HydTemp_Model *rv_data = (class HydTemp_Model*)river_data;

	//connect the N_Vectors
	realtype *result_data = NULL;
	result_data = NV_DATA_S(results);

	realtype *dh_da_data = NULL;
	dh_da_data = NV_DATA_S(da_dt);


	//set the flow specific values for the profiles 
	for (int i = 0; i < rv_data->Param_Temp.Param_RV->get_number_profiles(); i++) {
		try {
			rv_data->profiles[i].set_actuel_temp_from_solver((result_data[i]));

		}
		catch (Error msg) {
			ostringstream info;
			info << "Profilenumer    :" << i << endl;
			info << "Riverstation    :" << rv_data->profiles[i].get_profile_name() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	try {
		//set the da_dt values for each profile
		//first profile
		dh_da_data[0] = 0.0;
		//inbetween
		for (int i = 1; i < rv_data->Param_Temp.Param_RV->get_number_profiles()-1; i++) {
			if (rv_data->profiles[i].get_water_temperature_applied() == false && rv_data->profiles[i].get_inlet_temperature_applied()==false) {
				dh_da_data[i] = ((rv_data->profiles[i - 1].get_actual_temperature() - rv_data->profiles[i].get_actual_temperature()) / rv_data->profiles[i].get_distance2up())*0.5*(rv_data->profiles[i - 1].get_flow_velocity_current() + rv_data->profiles[i].get_flow_velocity_current()) -
					((rv_data->profiles[i].get_actual_temperature() - rv_data->profiles[i + 1].get_actual_temperature()) / rv_data->profiles[i + 1].get_distance2up())*0.5*(rv_data->profiles[i].get_flow_velocity_current() + rv_data->profiles[i + 1].get_flow_velocity_current());
				dh_da_data[i] = dh_da_data[i] + rv_data->profiles[i].get_delta_temp();
			}
			else {
				dh_da_data[i] = 0.0;

			}



		}
		//last profile
		int i = rv_data->Param_Temp.Param_RV->get_number_profiles() - 1;
		if (rv_data->profiles[i].get_water_temperature_applied() == false && rv_data->profiles[i].get_inlet_temperature_applied() == false) {
			dh_da_data[i] = ((rv_data->profiles[i - 2].get_actual_temperature() - rv_data->profiles[i - 1].get_actual_temperature()) / rv_data->profiles[i - 1].get_distance2up())*0.5*(rv_data->profiles[i - 2].get_flow_velocity_current() + rv_data->profiles[i - 1].get_flow_velocity_current()) -
				((rv_data->profiles[i].get_actual_temperature() - rv_data->profiles[i].get_actual_temperature()) / rv_data->profiles[i].get_distance2up())*0.5*(rv_data->profiles[i - 1].get_flow_velocity_current() + rv_data->profiles[i].get_flow_velocity_current());
			dh_da_data[i] = dh_da_data[i] + rv_data->profiles[i].get_delta_temp();
		}
		else {
			dh_da_data[i] = 0.0;

		}

		//for testing
		//ostringstream out;
		//for (int i = 0; i < rv_data->Param_Temp.Param_RV->get_number_profiles(); i++) {
			//out << " No " <<i<<  " dh_data " << dh_da_data[i]<< " result " << result_data[i] << " delta T " << rv_data->profiles[i].get_delta_temp() <<  "  velo " << rv_data->profiles[i].get_flow_velocity_current()<< " distance " << rv_data->profiles[i].get_distance2up() << endl;
			//Sys_Common_Output::output_hyd->output_txt(&out);
		//}

	}
	catch (Error msg){
		throw msg;
	}

	

	return 0;
}
