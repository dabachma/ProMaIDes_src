//#include "Hyd_Hydraulic_System.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
bool Hyd_Hydraulic_System::qt_thread_applied=false;

//static file for development
//ofstream Hyd_Hydraulic_System::test;

//constructor
Hyd_Hydraulic_System::Hyd_Hydraulic_System(void){
	this->my_rvmodels=NULL;
	this->my_fpmodels=NULL;
	this->my_comodel=NULL;
	this->my_temp_model = NULL;
	this->output_time=0.0;
	this->internal_time=0.0;
	this->next_internal_time=0.0;
	this->timestep_counter=0;
	this->timestep_internal_counter=0;
	this->total_internal_timestep=0;
	this->diff_real_time=0.0;

	this->internal_timestep_current=0.0;
	this->internal_timestep_base=0.0;
	this->internal_timestep_min=10.0;

	this->error_number=0;
	this->warning_number=0;
	this->warning_number_start=0;

	this->thread_number=0;
	this->calc_number=0;
	this->identifier_str=label::not_set;

	this->file_output_required=true;

	this->break_sz="CA";
	this->database_is_set=false;
	this->preproc_success_flag=false;

	this->file_output_folder=label::not_set;

	//omp_set_num_threads(2);
//	omp_set_dynamic(3);

	this->output_is_running=false;
	this->output_is_allowed=false;
	this->output_is_required=false;
	this->temp_calc = false;

	//development static output file
	//Hyd_Hydraulic_System::test.open("./ERG/test.dat");
	//Hyd_Hydraulic_System::test << "TITLE = "<< "\" test \"" << endl;
	//Hyd_Hydraulic_System::test << "VARIABLES = ";
	//Hyd_Hydraulic_System::test << "\" t \" ";
	//Hyd_Hydraulic_System::test << "\" q \" ";
	//Hyd_Hydraulic_System::test << "\" h1 \" ";
	//Hyd_Hydraulic_System::test << "\" h2 \" ";
	//Hyd_Hydraulic_System::test << "\" qbuff \" ";
	//Hyd_Hydraulic_System::test << "\" flag \" "<<endl;
	//Hyd_Hydraulic_System::test << P(4) << FORMAT_FIXED_REAL;


	this->coupling_managment.set_ptr2outputflags(&this->global_parameters.output_flags);
	this->check_opencl_available();

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Hydraulic_System)-sizeof(Hyd_Param_Global)-sizeof(Hyd_Param_Material)-sizeof(Hyd_Coupling_Management), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Hydraulic_System::~Hyd_Hydraulic_System(void){
	if(this->my_fpmodels!=NULL){
		delete []this->my_fpmodels;
		this->my_fpmodels=NULL;
	}
	if(this->my_rvmodels!=NULL){
		delete []this->my_rvmodels;
		this->my_rvmodels=NULL;
	}
	if(this->my_comodel!=NULL){
		delete this->my_comodel;
		this->my_comodel=NULL;
	}

	if (this->my_temp_model != NULL) {
		delete[]this->my_temp_model;
		this->my_temp_model = NULL;
	}

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Hydraulic_System)-sizeof(Hyd_Param_Global)-sizeof(Hyd_Param_Material)-sizeof(Hyd_Coupling_Management), _sys_system_modules::HYD_SYS);

	//development static output file
	//test.close();
}
//______________
//public
//(static) ask the global file name
string Hyd_Hydraulic_System::ask_global_file_name(void){
	string global_file;
	//ask for the name
	cout << "Give the global input file: ";
	cin >> global_file;

	return global_file;
}
//set the hydraulic system per file, also the subsystems
void Hyd_Hydraulic_System::set_system_per_file(const string global_file){
	//set prefix for output
	ostringstream prefix;
	prefix << "INP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->set_start_warning_number();
		// Get the Global information from parser
		this->global_parameters.globals_per_parser(global_file);
		this->global_parameters.check_members();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//input the material parameters
		this->material_params.set_filename(this->global_parameters.material_file);
		this->material_params.read_values_per_file();

		//input the observation points
		this->obs_point_managment.input_obs_point(this->global_parameters.obs_point_file);

		//allocate it
		this->allocate_river_models();
		//input the river models with parser
		this->input_rivers_models(global_file);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate diversion channel couplings
		this->coupling_managment.set_rv2rv_diversion(this->global_parameters.number_div_channel);
		//read them in
		this->coupling_managment.read_rv2rv_diversion_per_file(global_file);

		//set and allocate couplings of a river and flooplain-model via hydraulic structures
		this->coupling_managment.set_rv2fp_structure(this->global_parameters.number_struc_coupling);
		//read them in
		this->coupling_managment.read_rv2fp_structure_per_file(global_file);

		//set and allocate couplings of a river and flooplain-model via dikebreak
		this->coupling_managment.set_rv2fp_dikebreak(this->global_parameters.number_dikebreak_rv2fp_coupling);
		//read them in
		this->coupling_managment.read_rv2fp_dikebreak_per_file(global_file);

		//set and allocate couplings of a coast and flooplain-model via dikebreak
		this->coupling_managment.set_fp2co_dikebreak(this->global_parameters.number_dikebreak_fp2co_coupling);
		//read them in
		this->coupling_managment.read_fp2co_dikebreak_per_file(global_file);

		//allocate it
		this->allocate_floodplain_models();
		//init and set river model models with parser
		this->input_floodplains_models(global_file);
		//check the approximate memory requirement for the fp-models
		this->calculate_approx_memory_fpmodels();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//allocate it
		this->allocate_coast_model();
		//init and set river model models with parser
		this->input_coast_model(global_file);

		//allocate it
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			this->allocate_temp_model();
			//init and set temperaturmodel models with parser
			this->input_temp_model(global_file);
		}
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->set_final_warning_number();
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->set_final_warning_number();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Read in one river model per file
void Hyd_Hydraulic_System::set_river_model_per_file(const Hyd_Param_RV param_rv){
	ostringstream prefix;
	prefix << "INP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	try{
		this->global_parameters.GlobNofRV=1;
		//allocate it
		this->allocate_river_models();
		//input the river models with parser
		this->input_rivers_models(param_rv);
	}catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Read in the hydraulic system- and modelparameters from a database
void Hyd_Hydraulic_System::set_system_per_database(const bool modul_extern_startet){
	//set prefix for output
	ostringstream prefix;
	prefix << "INP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->set_start_warning_number();
		// Get the Global information from database
		this->global_parameters.globals_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//input the material parameters
		this->material_params.matparams_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//input observation points
		this->obs_point_managment.input_obs_point(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//count and populate the database table model
		QSqlTableModel results(0, this->database);
		QSqlQueryModel query;

		//river models
		this->global_parameters.GlobNofRV=Hyd_Model_River::select_relevant_model_database(&results, this->system_id);
		//allocate rv-models
		this->allocate_river_models();
		//input the river models per database
		this->input_rivers_models(&results, &this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//temp model
		if ((Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp)) {
			int no_temp=HydTemp_Model::select_relevant_model_database(&results, this->system_id);
			if (no_temp > 0) {
				this->global_parameters.tempmodel_applied = true;
				//HydTemp_Model::select_relevant_model_database(&results, this->system_id);
				this->allocate_temp_model();
				this->input_temp_model(&results, &this->database);
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

			}
		}

		//set and allocate diversion channel couplings
		this->global_parameters.number_div_channel=Hyd_Coupling_RV2RV_Diversion::select_relevant_dv_channels_database(&results, this->system_id);
		this->coupling_managment.set_rv2rv_diversion(this->global_parameters.number_div_channel);
		//read them in from the selection
		this->coupling_managment.read_rv2rv_diversion_per_database(&results, this->system_id);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate couplings of a river and flooplain-model via hydraulic structures
		this->global_parameters.number_struc_coupling=Hyd_Coupling_RV2FP_Structure::select_relevant_structures_database(&this->database, &query, this->system_id);
		this->coupling_managment.set_rv2fp_structure(this->global_parameters.number_struc_coupling);
		//read them in from the selection
		this->coupling_managment.read_rv2fp_structure_per_database(&query, this->system_id);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate couplings of a river and flooplain-model via dikebreak
		this->global_parameters.number_dikebreak_rv2fp_coupling=_Hyd_Coupling_Dikebreak::select_relevant_break_data_database(&this->database, &query, this->system_id);
		this->coupling_managment.set_rv2fp_dikebreak(this->global_parameters.number_dikebreak_rv2fp_coupling);
		//read them in from the selection
		this->coupling_managment.read_rv2fp_dikebreak_per_database(&query, this->system_id);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate couplings of a river and flooplain-model via dikebreak
		this->global_parameters.number_dikebreak_fp2co_coupling=_Hyd_Coupling_Dikebreak::select_relevant_break_data_coast_database(&this->database, &query, this->system_id);
		this->coupling_managment.set_fp2co_dikebreak(this->global_parameters.number_dikebreak_fp2co_coupling);
		//read them in from the selection
		this->coupling_managment.read_fp2co_dikebreak_per_database(&query, this->system_id);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//floodplain models
		this->global_parameters.GlobNofFP=Hyd_Model_Floodplain::select_relevant_model_database(&results);
		//allocate fp-models
		this->allocate_floodplain_models();
		//input the river models per database
		this->input_floodplains_models(&results, &this->database, false);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//coast model
		int number=0;
		QSqlQueryModel result_coast;
		number=Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(&result_coast, &this->database, this->system_id, 0, true, true);
		if(number==1){
			this->global_parameters.coastmodel_applied=true;
			this->allocate_coast_model();
			this->input_coast_model(&result_coast, &this->database);
		}
		else{
			this->global_parameters.coastmodel_applied=false;
		}

		//check global parameters
		this->global_parameters.check_members();

		//check the approximate memory requirement for the fp-models
		this->calculate_approx_memory_fpmodels();
		this->set_final_warning_number();

		this->global_parameters.calculate_total_numbers();


	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->set_final_warning_number();
		if(modul_extern_startet==true && Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag()==false){
			this->error_number++;
		}
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Read in the hydraulic temperature system- and modelparameters from a database
void Hyd_Hydraulic_System::set_temp_system_per_database(const bool modul_extern_startet) {
	//set prefix for output
	ostringstream prefix;
	prefix << "INP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try {
		this->set_start_warning_number();
		// Get the Global information from database
		this->global_parameters.globals_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//input the material parameters
		this->material_params.matparams_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//input observation points
		this->obs_point_managment.input_obs_point(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//count and populate the database table model
		QSqlTableModel results(0, this->database);
		QSqlQueryModel query;

		//river models
		this->global_parameters.GlobNofRV = Hyd_Model_River::select_relevant_model_database(&results, this->system_id);
		//allocate rv-models
		this->allocate_river_models();
		//input the river models per database
		this->input_rivers_models(&results, &this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//temp model
		if ((Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp)) {
			int no_temp = HydTemp_Model::select_relevant_model_database(&results, this->system_id);
			if (no_temp > 0) {
				this->global_parameters.tempmodel_applied = true;
				//HydTemp_Model::select_relevant_model_database(&results, this->system_id);
				this->allocate_temp_model();
				this->input_temp_model(&results, &this->database);
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

			}
		}

	
		this->set_final_warning_number();

	}
	catch (Error msg) {
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->set_final_warning_number();
		if (modul_extern_startet == true && Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag() == false) {
			this->error_number++;
		}
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();


}
//Read in the hydraulic system- and modelparameters from a database just for the floodplain models
void Hyd_Hydraulic_System::set_2Dsystem_per_database(const bool modul_extern_startet){
	//set prefix for output
	ostringstream prefix;
	prefix << "INP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->set_start_warning_number();
		// Get the Global information from database
		this->global_parameters.globals_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//input the material parameters
		this->material_params.matparams_per_database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//count and populate the database table model
		QSqlTableModel results(0, this->database);
		QSqlQueryModel query;

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//floodplain models
		this->global_parameters.GlobNofFP=Hyd_Model_Floodplain::select_relevant_model_database(&results);
		//allocate fp-models
		this->allocate_floodplain_models();
		//input the river models per database
		this->input_floodplains_models(&results, &this->database, true);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//check global parameters
		this->global_parameters.check_members();
		this->set_final_warning_number();
		this->global_parameters.calculate_total_numbers();
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->set_final_warning_number();
		if(modul_extern_startet==true && Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag()==false){
			this->error_number++;
		}
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Read in the the river-model(s) with their profile etc. and the coast model from database
void Hyd_Hydraulic_System::set_river_coast_model_per_database(const bool with_output){
	//set prefix for output
	ostringstream prefix;
	if(with_output==true){
		prefix << "INP> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	}
	//count and populate the database table model
	QSqlTableModel results(0, this->database);

	try{
		//river models
		this->global_parameters.GlobNofRV=Hyd_Model_River::select_relevant_model_database(&results, this->system_id, with_output);
		//allocate rv-models
		this->allocate_river_models();
		//input the river models per database
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->input_rivers_models(&results, &this->database, with_output);

		//coast model
		int number=0;
		QSqlQueryModel result_coast;
		number=Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(&result_coast, &this->database, this->system_id, 0, true, with_output);
		if(number==1){
			this->global_parameters.coastmodel_applied=true;
			this->allocate_coast_model();
			this->input_coast_model(&result_coast, &this->database, with_output);
		}
		else{
			this->global_parameters.coastmodel_applied=false;
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	if(with_output==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
///Read in the base hydraulic system per file and set this data to a database; all existing hydraulic data in database will be deleted
void Hyd_Hydraulic_System::import_basesystem_file2db(const string global_file){
	try{
		//delete the data in the tables
		this->delete_data_hyd_database_tables();

		//input global parameters
		this->set_start_warning_number();
		this->global_parameters.transfer_globals_file2database_table(&this->database, global_file);
		//input the material parameters
		this->material_params.transfer_materialparams_file2database_table(&this->database,this->global_parameters.material_file);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//observation points
		this->obs_point_managment.transfer_obs_points2database(&this->database, this->global_parameters.obs_point_file);

		//input the river models
		//allocate it
		this->allocate_river_models();
		//input the river models with parser
		this->input_rivers_models(global_file);
		//transfer it to the database
		this->transfer_rivermodel_data2database(&this->database);


		//temp model
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			if (this->global_parameters.tempmodel_applied == true) {
				this->allocate_temp_model();
				//init and set temperaturmodel models with parser
				this->input_temp_model(global_file);
				//transfer temp model to database
				this->transfer_tempmodel_data2database(&this->database);
			}

		}
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//set and allocate diversion channel couplings
		this->coupling_managment.set_rv2rv_diversion(this->global_parameters.number_div_channel);
		//read them in
		this->coupling_managment.read_rv2rv_diversion_per_file(global_file);
		//transfer it to database
		this->coupling_managment.transfer_rv2rv_diversion2database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate couplings of a river and flooplain-model via hydraulic structures
		this->coupling_managment.set_rv2fp_structure(this->global_parameters.number_struc_coupling);
		//read them in
		this->coupling_managment.read_rv2fp_structure_per_file(global_file);
		//transfer it to database
		this->coupling_managment.transfer_rv2fp_structure2database(&this->database);

		//set and allocate couplings of a river and flooplain-model via dikebreak
		this->coupling_managment.set_rv2fp_dikebreak(this->global_parameters.number_dikebreak_rv2fp_coupling);
		//read them in
		this->coupling_managment.read_rv2fp_dikebreak_per_file(global_file);
		//transfer it to database
		this->coupling_managment.transfer_rv2fp_dikebreak2database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//set and allocate couplings of a river and flooplain-model via dikebreak
		this->coupling_managment.set_fp2co_dikebreak(this->global_parameters.number_dikebreak_fp2co_coupling);
		//read them in
		this->coupling_managment.read_fp2co_dikebreak_per_file(global_file);
		//transfer it to database
		this->coupling_managment.transfer_fp2co_dikebreak2database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//allocate it
		this->allocate_floodplain_models();
		//init and set river model models with parser
		this->input_floodplains_models(global_file);
		//transfer it to the database
		this->transfer_floodplainmodel_data2database(&this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//allocate it
		this->allocate_coast_model();
		//init and set river model models with parser
		this->input_coast_model(global_file);
		if(this->global_parameters.coastmodel_applied==true){
			this->my_comodel->transfer_input_members2database(&this->database);
		}
	}
	catch(Error msg){
		this->set_final_warning_number();
		throw msg;
	}
	this->set_final_warning_number();
}
//Add new hydraulic boundary scenarios to the base hydraulic system into the database; the whole hydraulic system must be set before, e.g. read in per file
void Hyd_Hydraulic_System::import_new_hydraulic_boundary_sz2database(void){
	try{
		//transfer it to the database
		this->set_start_warning_number();
		//rivermodels
		for(int i=0; i< this->global_parameters.GlobNofRV;i++){
			this->my_rvmodels[i].transfer_hydraulic_boundary_sz2database(&this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		//floodplain models
		for(int i=0; i< this->global_parameters.GlobNofFP;i++){
			this->my_fpmodels[i].transfer_hydraulic_boundary_sz2database(&this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		//coast model
		if(this->global_parameters.coastmodel_applied==true){
			this->my_comodel->transfer_hydraulic_boundary_sz2database(&this->database);
		}
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp && this->global_parameters.tempmodel_applied == true) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				this->my_temp_model[i].transfer_hydraulic_boundary_sz2database(&this->database);
				Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			}

		}
	}
	catch(Error msg){
		this->set_final_warning_number();
		throw msg;
	}
	this->set_final_warning_number();
}
//Create a whole set of hydraulic database tables
void Hyd_Hydraulic_System::create_hyd_database_tables(void){
	if(this->database_is_set==false){
		return;
	}
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		//global
		Hyd_Boundary_Szenario_Management::create_table(&this->database);
		Hyd_Param_Global::create_table(&this->database);
		Hyd_Param_Material::create_table(&this->database);
		//floodplan model
		Hyd_Model_Floodplain::create_table(&this->database);
		//floodplain elements
		Hyd_Element_Floodplain::create_table(&this->database);
		//floodplain
		Hyd_Element_Floodplain::create_erg_table(&this->database);
	}
	else{
		//global
		Hyd_Boundary_Szenario_Management::create_table(&this->database);
		Hyd_Param_Global::create_table(&this->database);
		Hyd_Param_Material::create_table(&this->database);
		Hyd_Observation_Point_Manager::create_table(&this->database);

		//river model
		Hyd_Model_River::create_table(&this->database);
		//river profiles
		_Hyd_River_Profile::create_profile_table(&this->database);
		_Hyd_River_Profile::create_profile_boundary_table(&this->database);
		_Hyd_River_Profile::create_profile_bridge_table(&this->database);
		_Hyd_River_Profile::create_profile_dikebreak_table(&this->database);
		_Hyd_River_Profile::create_profile_wallbreak_table(&this->database);
		_Hyd_River_Profile_Type::create_profile_point_table(&this->database);
		_Hyd_River_Profile::create_bound2profile_view(&this->database);

		//floodplan model
		Hyd_Model_Floodplain::create_table(&this->database);
		//floodplain elements
		Hyd_Element_Floodplain::create_table(&this->database);
		Hyd_Element_Floodplain::create_element_boundary_table(&this->database);
		Hyd_Element_Floodplain::create_bound2elems_view(&this->database);

		//geometrical structures
		Hyd_Floodplain_Polysegment::create_table(&this->database);
		Hyd_Floodplain_Polygon::create_table(&this->database);

		//instationary boundary conditions
		Hyd_Instationary_Boundary::create_table(&this->database);

		//user defined couplings
		Hyd_Coupling_RV2RV_Diversion::create_table(&this->database);
		Hyd_Coupling_RV2FP_Structure::create_table(&this->database);
		_Hyd_Coupling_Dikebreak::create_table(&this->database);
		_Hyd_Coupling_Dikebreak::create_result_table(&this->database);

		//result tables
		//floodplain
		Hyd_Element_Floodplain::create_erg_table(&this->database);
		Hyd_Element_Floodplain::create_erg_instat_table(&this->database);
		//river
		_Hyd_River_Profile::create_erg_table(&this->database);
		_Hyd_River_Profile::create_erg_instat_table(&this->database);


		//coupling
		Hyd_Coupling_RV2FP_Merged::create_max_h_table(&this->database);



		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			HydTemp_Model::create_table(&this->database);
			HydTemp_Profile::create_erg_instat_table(&this->database);
			HydTemp_Profile::create_erg_table(&this->database);
			HydTemp_Profile::create_profile_boundary_table(&this->database);
			HydTemp_Profile::create_profile_table(&this->database);
			HydTemp_Profile::create_bound2profile_view(&this->database);


		}

	}
}
//Check all hydraulic database tables, for their existence in the database and their declaration in the database table-file
void Hyd_Hydraulic_System::check_hyd_database_tables(void){
	if(this->database_is_set==false){
		return;
	}
	try{
		if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
			//global
			Hyd_Boundary_Szenario_Management::set_table(&this->database);
			Hyd_Param_Global::set_table(&this->database);
			Hyd_Param_Material::set_table(&this->database);

			//floodplan model
			Hyd_Model_Floodplain::set_table(&this->database);
			//floodplain elements
			Hyd_Element_Floodplain::set_table(&this->database);
			//floodplain
			Hyd_Element_Floodplain::set_table(&this->database);
		}
		else{
			//input global parameters
			this->set_start_warning_number();
			//global
			ostringstream cout;
			cout << "Check global hydraulic boundary scenario database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Boundary_Szenario_Management::set_table(&this->database);
			cout << "Check global hydraulic data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);;
			Hyd_Param_Global::set_table(&this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			cout << "Check hydraulic material data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Param_Material::set_table(&this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

			cout << "Check observation point data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Observation_Point_Manager::set_table(&this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

			//river model
			cout << "Check general river model data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Model_River::set_table(&this->database);
			//river profiles
			cout << "Check river profile database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_profile_table(&this->database);
			cout << "Check river profile boundary database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_profile_boundary_table(&this->database);
			cout << "Check river profile bridge database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_profile_bridge_table(&this->database);
			cout << "Check river profile dikebreak database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_profile_dikebreak_table(&this->database);
			cout << "Check river profile wallbreak database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_profile_wallbreak_table(&this->database);
			cout << "Check river profile type point database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile_Type::set_profile_point_table(&this->database);

			//floodplan model
			cout << "Check general floodplain model data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Model_Floodplain::set_table(&this->database);
			//floodplain elements
			cout << "Check floodplain element boundary database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Element_Floodplain::set_table(&this->database);
			cout << "Check floodplain element boundary database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Element_Floodplain::set_element_boundary_table(&this->database);

			//geometrical structures
			cout << "Check polysegment (dike-, coastline) data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Floodplain_Polysegment::set_table(&this->database);
			cout << "Check floodplain polygon data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Floodplain_Polygon::set_table(&this->database);

			//instationary boundary conditions
			cout << "Check global instationary boundary data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Instationary_Boundary::set_table(&this->database);

			//instationary boundary conditions
			cout << "Check global instationary boundary point data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Instationary_Boundary::set_point_table(&this->database);

			//user defined couplings
			cout << "Check diversion channel data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Coupling_RV2RV_Diversion::set_table(&this->database);
			cout << "Check hydraulic coupling structure data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Coupling_RV2FP_Structure::set_table(&this->database);
			cout << "Check dike-/wallbreak coupling data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_Coupling_Dikebreak::set_table(&this->database);
			cout << "Check dike-/wallbreak coupling result data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_Coupling_Dikebreak::set_result_table(&this->database);

			//result tables
			cout << "Check max-result database table (floodplain)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Element_Floodplain::set_erg_table(&this->database);

			cout << "Check instationary result database table (floodplain)..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Element_Floodplain::set_erg_instat_table(&this->database);

			//result tables
			cout << "Check max-result database table (river)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_erg_table(&this->database);

			cout << "Check instationary result database table (river)..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			_Hyd_River_Profile::set_erg_instat_table(&this->database);

			cout << "Check max-break waterlevel database table (coupling2fpl)..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout,false, false);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			Hyd_Coupling_RV2FP_Merged::set_max_h_table(&this->database);

			if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
				cout << "Check the temperature table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				cout << "Check the temperature result table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				HydTemp_Model::set_table(&this->database);
				cout << "Check the temperature instationary result table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				HydTemp_Profile::set_erg_instat_table(&this->database);
				cout << "Check the temperature general parameters table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				HydTemp_Profile::set_erg_table(&this->database);
				cout << "Check the temperature profile table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				HydTemp_Profile::set_profile_table(&this->database);
				cout << "Check the temperature profile boundary data table..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout, false, false);
				HydTemp_Profile::set_profile_boundary_table(&this->database);
			


			}

		}
	}
	catch(Error msg){
		this->set_final_warning_number();
		ostringstream info;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->set_final_warning_number();
}
//Delete the data of all hydraulic database tables (static)
void Hyd_Hydraulic_System::delete_data_hyd_database_tables(void){
	if(this->database_is_set==false){
		return;
	}
	ostringstream cout;
	cout << "Delete all data in the hydraulic database tables..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		//global
		Hyd_Param_Global::delete_data_in_table(&this->database);
		Hyd_Param_Material::delete_data_in_table(&this->database);

		//delete data of the floodplain tables, also appending tables
		Hyd_Model_Floodplain::delete_data_in_table(&this->database);
	}
	else{
		//global
		Hyd_Param_Global::delete_data_in_table(&this->database);
		Hyd_Param_Material::delete_data_in_table(&this->database);
		Hyd_Observation_Point_Manager::delete_data_in_table(&this->database);

		//delete data of the river tables, also appending tables
		Hyd_Model_River::delete_data_in_table(&this->database);
		//delete data of the floodplain tables, also appending tables
		Hyd_Model_Floodplain::delete_data_in_table(&this->database);

		//polygons
		Hyd_Floodplain_Polygon::delete_data_in_table(&this->database);
		//polysegments
		Hyd_Floodplain_Polysegment::delete_data_in_table(&this->database);
		//instationary boundary curves
		Hyd_Instationary_Boundary::delete_data_in_table(&this->database);
		//user defined couplings
		Hyd_Coupling_RV2RV_Diversion::delete_data_in_table(&this->database);
		Hyd_Coupling_RV2FP_Structure::delete_data_in_table(&this->database);
		_Hyd_Coupling_Dikebreak::delete_data_in_table(&this->database);
		_Hyd_Coupling_Dikebreak::delete_data_in_result_table(&this->database);

		Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(&this->database);

		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {

			HydTemp_Model::delete_data_in_table(&this->database);
			HydTemp_Profile::delete_data_in_table(&this->database);


		}
	}
}
//Close all hydraulic database tables (static)
void Hyd_Hydraulic_System::close_hyd_database_tables(void){
	//ostringstream cout;
	//cout << "Close all tables of the hydraulic database..." << endl ;
	//Sys_Common_Output::output_hyd->output_txt(&cout);
	if(Sys_Project::get_project_type()==_sys_project_type::proj_dam){
		Hyd_Boundary_Szenario_Management::close_table();
		Hyd_Param_Global::close_table();
		Hyd_Param_Material::close_table();
		//floodplain models; here also appending tables are closed
		Hyd_Model_Floodplain::close_table();
	}
	else{
		//global
		Hyd_Boundary_Szenario_Management::close_table();
		Hyd_Param_Global::close_table();
		Hyd_Param_Material::close_table();
		//river models; here also appending tables are closed
		Hyd_Model_River::close_table();
		//floodplain models; here also appending tables are closed
		Hyd_Model_Floodplain::close_table();
		Hyd_Observation_Point_Manager::close_table();

		//polygons
		Hyd_Floodplain_Polygon::close_table();
		//polysegments
		Hyd_Floodplain_Polysegment::close_table();
		//instationary boundary curves
		Hyd_Instationary_Boundary::close_table();
		//user defined couplings
		Hyd_Coupling_RV2RV_Diversion::close_table();
		Hyd_Coupling_RV2FP_Structure::close_table();
		_Hyd_Coupling_Dikebreak::close_table();
		_Hyd_Coupling_Dikebreak::close_result_table();
		Hyd_Coupling_RV2FP_Merged::close_max_h_table();
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			HydTemp_Model::close_table();
			HydTemp_Profile::close_table();
		}
	}
}
//Switch applied flag of the hydraulic results for a given system id (static)
void Hyd_Hydraulic_System::switch_applied_flags_results(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	_Hyd_River_Profile::switch_applied_flag_erg_table(ptr_database, id, flag);
	Hyd_Element_Floodplain::switch_applied_flag_erg_table(ptr_database, id, flag);
	_Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(ptr_database, id, flag);
	Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(ptr_database, id, flag);
}
//Switch applied flag of the hydraulic results for a given system id and given hydraulic boundary scenario id (static)
void Hyd_Hydraulic_System::switch_applied_flags_results(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int hyd_sc){
	_Hyd_River_Profile::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Hyd_Element_Floodplain::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	_Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(ptr_database, id, hyd_sc, flag);
	Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(ptr_database, id, hyd_sc, flag);
}
//Copy the results of a given system id to another one (static)
void Hyd_Hydraulic_System::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	_Hyd_River_Profile::copy_results(ptr_database, src, dest);
	_Hyd_River_Profile::copy_instat_results(ptr_database, src, dest);
	Hyd_Element_Floodplain::copy_results(ptr_database, src, dest);
	Hyd_Element_Floodplain::copy_instat_results(ptr_database, src, dest);
	_Hyd_Coupling_Dikebreak::copy_results(ptr_database, src, dest);
	Hyd_Coupling_RV2FP_Merged::copy_results(ptr_database, src, dest);
}
//Check if hydraulic results are calculated (static)
bool Hyd_Hydraulic_System::check_hyd_results_calculated(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag){
	QSqlQueryModel model;

	if(_Hyd_River_Profile::select_data_in_erg_table(&model, ptr_database, id, bound_sz, break_sz ,like_flag)>0){
		return true;
	}

    //count relevant elemts
    if(Hyd_Element_Floodplain::check_calc_in_erg_table(&model, ptr_database, id, bound_sz, break_sz ,like_flag)==true){
		return true;
	}

	return false;
}
//Check if instationary hydraulic results are calculated (static)
bool Hyd_Hydraulic_System::check_hyd_instat_results_calculated(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz, const bool like_flag) {
	QSqlQueryModel model;

	if (_Hyd_River_Profile::select_data_in_instat_erg_table(&model, ptr_database, id, bound_sz, break_sz, like_flag) > 0) {
		return true;
	}

	//count relevant elemts
	if (Hyd_Element_Floodplain::check_calc_in_instat_erg_table(&model, ptr_database, id, bound_sz, break_sz, like_flag) == true) {
		return true;
	}

	return false;
}
//Set a new hydraulic boundary scenario id to the hydraulic system and all appendent models
void Hyd_Hydraulic_System::set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz){
	this->hyd_sz.set_members(&new_sz);
	//rivermodels
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
	}
	//floodplain models
	for(int i=0; i< this->global_parameters.GlobNofFP; i++){
		this->my_fpmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
	}
	//coast model
	if(this->global_parameters.coastmodel_applied==true){
		this->my_comodel->set_hydraulic_bound_sz(this->hyd_sz);
	}
	//temp model
	if ((Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp)){
		if (this->global_parameters.tempmodel_applied == true) {
			this->my_temp_model->set_new_hyd_bound_sz_id(this->hyd_sz);
		}

	}
}
//Get a pointer to the currently used hydraulic boundary scenario
Hyd_Boundary_Szenario* Hyd_Hydraulic_System::get_ptr_hyd_bound_scenario(void){
	return &this->hyd_sz;
}
//output the models/global etc.
void Hyd_Hydraulic_System::output_glob_models(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "OUT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;

	//output the globalparameters
	this->global_parameters.output_members();

	//output the material parameter
	this->material_params.output_members();

	//river models
	if(this->global_parameters.GlobNofRV>0){
		cout << "Rivermodel information..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			this->my_rvmodels[j].output_members();
		}
	}

	//temp model
	if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
		if (this->global_parameters.GlobNofRV > 0) {
			cout << "Temperature model information of river..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
				this->my_temp_model[j].output_members();
			}
		}

	}

	//floodplain models
	if(this->global_parameters.GlobNofFP>0){
		cout << "Floodplainmodel information ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			this->my_fpmodels[j].output_members();
		}
	}
	//coast model
	if(this->global_parameters.coastmodel_applied==true){
		cout << "Coastmodel information ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->my_comodel->output_members();
	}
	//couplings
	this->coupling_managment.output_members();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
void Hyd_Hydraulic_System::compare_hydraulic_system(Hyd_Hydraulic_System *compare_system){
/**
The equality of the system is concerning the number of river-, floodplain- and coast model(s).
The equality of the models are compared in terms of:
- river models (Hyd_Model_River): the number of profiles and their riverstations
- floodpain models (Hyd_Model_Floodplain): the geometrical values of the model, e.g. angle, width of the elements in x-/y-direction etc. , and
their number of elements

This comparison is primarily important for adding new hydraulic boundary scenarios. It ensures, that
the new boundary conditions are set properly to the hydraulic system. The complete input of each model
have to be done before this check.
*/

	if(this->global_parameters.GlobNofRV!=compare_system->global_parameters.GlobNofRV){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number river model(s) system "<< this->calc_number<<": "<<this->global_parameters.GlobNofRV<< endl;
		info << "Number river model(s) system "<< compare_system->calc_number<<": "<<compare_system->global_parameters.GlobNofRV<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->global_parameters.GlobNofFP!=compare_system->global_parameters.GlobNofFP){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number floodplain model(s) system "<< this->calc_number<<": "<<this->global_parameters.GlobNofFP<< endl;
		info << "Number floodplain model(s) system "<< compare_system->calc_number<<": "<<compare_system->global_parameters.GlobNofFP<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->global_parameters.coastmodel_applied!=compare_system->global_parameters.coastmodel_applied){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Coast model system "<< this->calc_number<<": "<<functions::convert_boolean2string(this->global_parameters.coastmodel_applied)<< endl;
		info << "Coast model system "<< compare_system->calc_number<<": "<<functions::convert_boolean2string(compare_system->global_parameters.coastmodel_applied)<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->global_parameters.number_dikebreak_fp2co_coupling!=compare_system->global_parameters.number_dikebreak_fp2co_coupling){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number user defined break coast to floodplain "<< this->calc_number<<": "<<this->global_parameters.number_dikebreak_fp2co_coupling<< endl;
		info << "Number user defined break coast to floodplain "<< compare_system->calc_number<<": "<<compare_system->global_parameters.number_dikebreak_fp2co_coupling<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(this->global_parameters.number_dikebreak_rv2fp_coupling!=compare_system->global_parameters.number_dikebreak_rv2fp_coupling){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number user defined break river to floodplain "<< this->calc_number<<": "<<this->global_parameters.number_dikebreak_rv2fp_coupling<< endl;
		info << "Number user defined break river to floodplain "<< compare_system->calc_number<<": "<<compare_system->global_parameters.number_dikebreak_rv2fp_coupling<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(this->global_parameters.number_div_channel!=compare_system->global_parameters.number_div_channel){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number user defined diversion channel "<< this->calc_number<<": "<<this->global_parameters.number_div_channel<< endl;
		info << "Number user defined diversion channel "<< compare_system->calc_number<<": "<<compare_system->global_parameters.number_div_channel<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(this->global_parameters.number_struc_coupling!=compare_system->global_parameters.number_struc_coupling){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Number user defined structure(s) "<< this->calc_number<<": "<<this->global_parameters.number_struc_coupling<< endl;
		info << "Number user defined structure(s)"<< compare_system->calc_number<<": "<<compare_system->global_parameters.number_struc_coupling<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		for(int i=0; i< this->global_parameters.GlobNofRV; i++){
			this->my_rvmodels[i].compare_models(&compare_system->my_rvmodels[i]);
		}
		for(int i=0; i< this->global_parameters.GlobNofFP; i++){
			this->my_fpmodels[i].compare_models(&compare_system->my_fpmodels[i]);
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"First hydraulic system  : " << this->calc_number << endl;
		info<<"Second hydraulic system : " << compare_system->calc_number << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Compare the equality of two hydraulic systems in terms of model numbers; further the models are compared
void Hyd_Hydraulic_System::transfer_glob_elem_id(Hyd_Hydraulic_System *to_system){
	for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
		this->my_fpmodels[i].transfer_glob_elem_id_fp(&to_system->my_fpmodels[i]);
	}

}
//Clone the hydraulic system after initialisation TODO
void Hyd_Hydraulic_System::clone_system(Hyd_Hydraulic_System *system){
	ostringstream cout;
	ostringstream prefix;
	cout <<"Clone hydraulic base system (thread 0) to hydraulic system of thread number "<< this->thread_number<<"..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	prefix << "CLONE_"<<this->thread_number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(&prefix);

	try{
		this->set_start_warning_number();
		this->global_parameters=system->global_parameters;
		this->material_params=system->material_params;
		this->hyd_sz=system->hyd_sz;
		this->system_id=system->system_id;
		//allocate rv-models
		this->allocate_river_models();
		//copy river
		cout <<"Clone river models..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		prefix << "RV> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		for(int i=0; i< this->global_parameters.get_number_river_model();i++){
			cout <<"Clone river model " << i << "..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[i].clone_model(&system->my_rvmodels[i]);
			this->my_rvmodels[i].init_solver(&this->global_parameters);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		Sys_Common_Output::output_hyd->rewind_userprefix();

		//clone coast model
		if(this->global_parameters.coastmodel_applied==true){
			cout <<"Clone coast model..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->allocate_coast_model();
			this->my_comodel->clone_model(system->my_comodel);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}

		//allocate fp-models
		this->allocate_floodplain_models();
		//copy floodplain
		cout <<"Clone floodplain models..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		prefix << "FP> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		for(int i=0; i< this->global_parameters.get_number_floodplain_model(); i++){
			cout <<"Clone floodplain model " << i << "..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_fpmodels[i].clone_model(&system->my_fpmodels[i], &this->material_params, this->my_comodel);
			this->my_fpmodels[i].init_solver(&this->global_parameters);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		Sys_Common_Output::output_hyd->rewind_userprefix();

		//couplings
		prefix << "COUP> ";
		Sys_Common_Output::output_hyd->set_userprefix(&prefix);
		this->coupling_managment.clone_couplings(&system->coupling_managment, this);
		Sys_Common_Output::output_hyd->rewind_userprefix();

		//obs points
		this->obs_point_managment.clone_obs_points(&(system->obs_point_managment),  this->my_rvmodels,  this->my_fpmodels);
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		this->set_final_warning_number();
		throw msg;
	}

	this->set_final_warning_number();
	cout <<"Hydraulic system cloning is finished"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	Sys_Common_Output::output_hyd->rewind_userprefix();

	//cout <<"START1"<<endl;
	//Sys_Common_Output::output_hyd->output_txt(&cout);
	/*this->output_glob_models();
	this->output_setted_members();*/

	//cout <<"StART2"<<endl;
	//Sys_Common_Output::output_hyd->output_txt(&cout);
	//system->output_glob_models();
	//system->output_setted_members();
}
//initialize the models
void Hyd_Hydraulic_System::init_models(const bool modul_extern_startet){
	//set prefix for output
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->set_start_warning_number();

		//connect river
		this->connect_rivers(&this->material_params);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//make the coupling for the diversion channel
		this->coupling_managment.set_pointer_rv2rv_diversion(this->my_rvmodels, this->global_parameters.GlobNofRV);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//make th geometrical interception
		this->make_geometrical_interception();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//connect floodplains
		this->connect_floodplains();

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//automatic coupling
		this->init_automatic_coupling();

		//check the models
		this->check_models();

		//init the observation point
		this->obs_point_managment.init_obs_points(this->global_parameters.GlobNofRV, this->my_rvmodels, this->global_parameters.GlobNofFP, this->my_fpmodels, this->global_parameters.GlobTNof, this->global_parameters.GlobNofITS+1);

		this->set_final_warning_number();
	}
	catch(Error msg){
		this->set_final_warning_number();
		if(modul_extern_startet==true && Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag()==false){
			this->error_number++;
		}
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
///Initialize the temperature models
void Hyd_Hydraulic_System::init_temp_models(const bool modul_extern_startet) {
	//set prefix for output
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try {
		this->set_start_warning_number();

		//connect river
		this->connect_rivers(&this->material_params);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->connect_temperature_model();

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();



		//check the models
		this->check_models();

		//init the observation point
		this->obs_point_managment.init_temp_obs_points(this->global_parameters.GlobNofRV, this->my_temp_model, this->global_parameters.GlobTNof, this->global_parameters.GlobNofITS + 1);

		this->set_final_warning_number();
	}
	catch (Error msg) {
		this->set_final_warning_number();
		if (modul_extern_startet == true && Hyd_Multiple_Hydraulic_Systems::get_stop_thread_flag() == false) {
			this->error_number++;
		}
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//output setted and calculated members
void Hyd_Hydraulic_System::output_setted_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "OUT_SET> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	//river models (console)
	if(this->global_parameters.GlobNofRV>0){
		cout << "Setted members of the Rivermodel(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			this->my_rvmodels[j].output_setted_members();
		}
	}
	//river models fileoutput

	if(this->file_output_required==true){
		if(this->global_parameters.GlobNofRV>0){
			cout << "Setted members of the Rivermodel(s) to file..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
				cout << "for Tecplot..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
					this->my_rvmodels[j].output_geometrie2tecplot_2d();
				}
			}
			if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
				cout << "for ParaView..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
					this->my_rvmodels[j].output_geometrie2paraview_2d();
				}
			}
			if (this->global_parameters.get_output_flags().tecplot_2d_required == false && this->global_parameters.get_output_flags().paraview_2d_required == false) {
				cout << "No output required..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
			}
		}
	}
	

	//floodplain models (console)
	if(this->global_parameters.GlobNofFP>0){
		cout << "Setted members of the Floodplainmodel(s)..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			this->my_fpmodels[j].output_setted_members();
		}
	}
	//floodplain models file output
	if(this->file_output_required==true){
		if(this->global_parameters.GlobNofFP>0){
			cout << "Setted members of the Floodplainmodel(s) to file..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);	
			if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
				cout << "for Tecplot..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
					this->my_fpmodels[j].output_geometrie2tecplot();
				}
			}
			if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
				cout << "for ParaView..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
					this->my_fpmodels[j].output_geometrie2paraview();
				}
			}
			if (this->global_parameters.get_output_flags().bluekenue_2d_required==true) {
				cout << "for BlueKenue..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
					this->my_fpmodels[j].output_geometrie2bluekenue();
				}
			}
			if (this->global_parameters.get_output_flags().tecplot_2d_required == false && this->global_parameters.get_output_flags().paraview_2d_required == false && this->global_parameters.get_output_flags().bluekenue_2d_required == false) {
				cout << "No output required..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}
			
		}
	}
	//coast models tecplot output
	if(this->file_output_required==true){
		if(this->global_parameters.coastmodel_applied==true){
			cout << "Setted members of the Coastmodel to file..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
				cout << "for Tecplot..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				this->my_comodel->output_geometrie2tecplot();
			}
			if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
				cout << "for ParaView..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				this->my_comodel->output_geometrie2paraview();
			}
			if (this->global_parameters.get_output_flags().tecplot_2d_required == false && this->global_parameters.get_output_flags().paraview_2d_required == false) {
				cout << "No output required..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}
		}
	}
	//couplings
	if(this->database_is_set==true){
		this->coupling_managment.output_setted_members(&this->database);
	}
	else{
		this->coupling_managment.output_setted_members(NULL);
	}

	this->obs_point_managment.output_setted_members();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output setted and calculated temperature members
void Hyd_Hydraulic_System::output_setted_temp_members(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "OUT_SET> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	//river models (console)
	if (this->global_parameters.GlobNofRV > 0) {
		cout << "Setted members of the Temperature model(s)..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			this->my_temp_model[j].output_setted_members();
		}
	}

	this->obs_point_managment.output_setted_temp_members();
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();


}
//Initialize the solver for the models; all not needed data is here deleted
void Hyd_Hydraulic_System::init_solver(void){
	ostringstream cout;
	ostringstream prefix;
	//delete not needed data (geometry)
	this->clear_models();
	//river models
	try{
		this->set_start_warning_number();
		//set the parameters for Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			prefix << "RV_"<<j<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			//init the solver
			cout<<"Initialize the solver for river model ..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[j].init_solver(&this->global_parameters);
			Sys_Common_Output::output_hyd->rewind_userprefix();
			prefix.str("");
		}
		this->set_final_warning_number();
	}
	catch(Error msg){
		this->set_final_warning_number();
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//floodplain models
	//2DGPU
	try{
		this->set_start_warning_number();
		//set the parameters for Hyd_Model_Floodplain
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			prefix << "FP_"<<j<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			//init the solver
			cout<<"Initialize the solver for floodplain model ..."<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_fpmodels[j].init_solver(&this->global_parameters);
			Sys_Common_Output::output_hyd->rewind_userprefix();
			prefix.str("");
		}
		this->set_final_warning_number();
	}
	catch(Error msg){
		this->set_final_warning_number();
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
}
//Initialize the solver for the temperature models; all not needed data is here deleted
void Hyd_Hydraulic_System::init_temp_solver(void) {
	ostringstream cout;
	ostringstream prefix;
	//delete not needed data (geometry)
	this->clear_models();
	//river models
	try {
		this->set_start_warning_number();
		//set the parameters for Hyd_Model_River
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			prefix << "TEMPRV_" << j << "> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			//init the solver
			cout << "Initialize the solver for temperature model ..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_temp_model[j].init_solver(&this->global_parameters);
			Sys_Common_Output::output_hyd->rewind_userprefix();
			prefix.str("");
		}
		this->set_final_warning_number();
	}
	catch (Error msg) {
		this->set_final_warning_number();
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}


}
//make the calculation
void Hyd_Hydraulic_System::make_calculation(void){
	//set Profiler
	Profiler profiler = new Profiler(true);
	this->profiler = profiler;

	//set the first output time
	this->output_time=this->global_parameters.GlobTStep+this->global_parameters.get_startime();
	this->next_internal_time=this->global_parameters.get_startime();

	this->internal_timestep_min=this->global_parameters.min_internal_step;

	//loop over river model
	ostringstream cout;

	//real_time starting (begin time recording
	time(&this->start_time);
	this->actual_time=this->start_time;
	this->set_start_warning_number();
	//output header
	if(this->thread_number==0){
		//set prefix for output
		ostringstream prefix;
		prefix << "CALC"<<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		cout <<"Start calculation..."<<endl;
		cout <<W(17) << " Step " <<  W(15) << " Modeltime " <<  W(15)<< " Modeltime "  << W(20)<< " Real time (Diff)" ;
		cout << W(28) << " Solversteps (Diff)" << W(12) << " norm.Error " << W(12) << " max.Error " << W(12) << " Internal t_step (Diff)" << endl;
		cout << W(29) << label::sec << W(23) << label::time_unit_output << W(19) << label::time_unit_output << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	this->internal_timestep_current=0.0;
	this->internal_timestep_base=this->global_parameters.GlobTStep/this->global_parameters.GlobNofITS;
	this->internal_timestep_current=this->internal_timestep_base;
	if(this->internal_timestep_base<this->internal_timestep_min){
		this->internal_timestep_min=this->internal_timestep_base;
	}

	try{
		this->coupling_managment.init_output_files();
		//loop over the output timesteps
		for(int i=0; i < this->global_parameters.GlobTNof; i++){
			//make the internal loop over the internal timesteps
			this->make_calculation_internal();

			//count the next output time step
			this->output_time=this->output_time+this->global_parameters.GlobTStep;
			this->timestep_counter++;
		}
	}
	catch(Error msg){
		this->set_final_warning_number();
		throw msg;
	}
	this->profiler.logValues();
	this->set_final_warning_number();
	//reset the counter
	this->timestep_counter=0;
}
//Make the temperature calculation of the external loop (output time steps)
void Hyd_Hydraulic_System::make_temp_calculation(void) {
	//set the first output time
	this->output_time = this->global_parameters.GlobTStep + this->global_parameters.get_startime();
	this->next_internal_time = this->global_parameters.get_startime();

	this->internal_timestep_min = this->global_parameters.min_internal_step;

	//loop over river model
	ostringstream cout;

	//real_time starting (begin time recording
	time(&this->start_time);
	this->actual_time = this->start_time;
	this->set_start_warning_number();
	//output header
	if (this->thread_number == 0) {
		//set prefix for output
		ostringstream prefix;
		prefix << "CALC" << "> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		cout << "Start calculation..." << endl;
		cout << W(17) << " Step " << W(15) << " Modeltime " << W(15) << " Modeltime " << W(20) << " Real time (Diff)";
		cout << W(28) << " Solversteps (Diff)" << W(12) << " norm.Error " << W(12) << " max.Error " << W(12) << " Internal t_step (Diff)" << endl;
		cout << W(29) << label::sec << W(23) << label::time_unit_output << W(19) << label::time_unit_output << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	this->internal_timestep_current = 0.0;
	this->internal_timestep_base = this->global_parameters.GlobTStep / this->global_parameters.GlobNofITS;
	this->internal_timestep_current = this->internal_timestep_base;
	if (this->internal_timestep_base < this->internal_timestep_min) {
		this->internal_timestep_min = this->internal_timestep_base;
	}

	try {
		//loop over the output timesteps
		for (int i = 0; i < this->global_parameters.GlobTNof; i++) {
			//make the internal loop over the internal timesteps
			this->make_temp_calculation_internal();

			//count the next output time step
			this->output_time = this->output_time + this->global_parameters.GlobTStep;
			this->timestep_counter++;
		}
	}
	catch (Error msg) {
		this->set_final_warning_number();
		throw msg;
	}

	this->set_final_warning_number();
	//reset the counter
	this->timestep_counter = 0;


}
//Set the thread number; used for the output of the calculation class; use it just before the method run(void)
void Hyd_Hydraulic_System::set_thread_number(const int thread){
	this->thread_number=thread;
}
//Set the system number by multi system calculations
void Hyd_Hydraulic_System::set_system_number(const int calcnumber){
	this->calc_number=calcnumber;
}
//Get the system number
int Hyd_Hydraulic_System::get_system_number(void){
	return this->calc_number;
}
//Run the calculation; it is identically to the methods make_calculation(void); just use this for multithreading
void Hyd_Hydraulic_System::run(void){
	if(this->error_number!=0){
		//loop over river model
		ostringstream cout;
		//set prefix for output
		ostringstream prefix;
		prefix << this->get_identifier_prefix();
		prefix << "CALC> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		cout << "No calculation due to an error occurence before..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//rewind two times the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
	else if(this->temp_calc==false){
		try{
			this->make_calculation();
		}
		catch(Error msg){
			if(msg.get_user_aborted_exception()==false){
				ostringstream info;
				info <<"Hydraulic system : "<< this->get_identifier_prefix(false) << endl;
				msg.make_second_info(info.str());
				this->error_number++;
				msg.output_msg(2);
			}
			else{
				this->output_final_model_statistics(false);
				msg.output_msg(2);
			}
		}
	}
	else if (this->temp_calc == true) {
		try {
			this->make_temp_calculation();
		}
		catch (Error msg) {
			if (msg.get_user_aborted_exception() == false) {
				ostringstream info;
				info << "Hydraulic system : " << this->get_identifier_prefix(false) << endl;
				msg.make_second_info(info.str());
				this->error_number++;
				msg.output_msg(2);
			}
			else {
				this->output_final_model_statistics(false);
				msg.output_msg(2);
			}
		}
	}
}
//output final statistics of the models
void Hyd_Hydraulic_System::output_final_model_statistics(const bool all_output){
	//set prefix for output
	ostringstream prefix;
	prefix << this->get_identifier_prefix();
	prefix<< "STA> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	this->set_start_warning_number();

	if(this->error_number!=0){
		//loop over river model
		ostringstream cout;
		cout << "No final output due to an error occurence before..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//errors/warnings
		this->set_final_warning_number();
		this->output_final_error_warning_number();
	}
	else if (this->temp_calc == false) {
		ostringstream cout;
		this->output_final_system_statistics();
		//river models
		if (this->global_parameters.GlobNofRV > 0) {
			cout << "Statistics of the Rivermodel(s)..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
				this->my_rvmodels[j].output_final();
			}
			if (this->file_output_required == true && all_output == true) {
				cout << "Output maximum results of the Rivermodel(s) to file..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);

				if (this->global_parameters.get_output_flags().tecplot_1d_required == true) {
					cout << "for Tecplot..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
						this->my_rvmodels[j].output_result_max2tecplot();
					}
				}
				if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
					cout << "for ParaView / csv 1d..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
						this->my_rvmodels[j].output_result_max2csv();
					}
				}
				if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
					cout << "for ParaView 2d..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
						this->my_rvmodels[j].output_result_max2paraview2d();
					}
				}
				if (this->global_parameters.get_output_flags().tecplot_1d_required == false && this->global_parameters.get_output_flags().paraview_1d_required == false) {
					cout << "No output required..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				}

			}
			if (this->database_is_set == true && all_output == true) {
				cout << "Output maximum results of the Rivermodel(s) to database..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
					this->my_rvmodels[j].output_result_max2database(&this->database, this->break_sz);
				}
			}
		}

		//floodplain models 
		if (this->global_parameters.GlobNofFP > 0) {
			cout << "Statistics of the Floodplainmodel(s)..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
				this->my_fpmodels[j].output_final();
				if (this->my_fpmodels[j].pManager != nullptr) {
					this->my_fpmodels[j].pManager->profiler->logValues();
				}
			}
			if (this->file_output_required == true && all_output == true) {
				cout << "Output maximum results of the Floodplainmodel(s) to file..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
					cout << "for Tecplot..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
						this->my_fpmodels[j].output_result_max2tecplot();
					}
				}
				if (this->global_parameters.get_output_flags().bluekenue_2d_required == true) {
					cout << "for BlueKenue..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
						this->my_fpmodels[j].output_result_max2bluekenue();
					}
				}
				if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
					cout << "for ParaView..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
						this->my_fpmodels[j].output_result_max2paraview();
					}
				}
				if (this->global_parameters.get_output_flags().tecplot_2d_required == false && this->global_parameters.get_output_flags().paraview_2d_required == false) {
					cout << "No output required..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				}
			}
			if (this->database_is_set == true && all_output == true) {
				//delete the data
				Hyd_Element_Floodplain::delete_data_in_erg_table(&this->database, this->system_id, this->hyd_sz.get_id(), this->break_sz);
				cout << "Output maximum results of the Floodplainmodel(s) to database..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				bool was_output = false;
				bool must_output = false;

				for (int j = 0; j < this->global_parameters.GlobNofFP; j++) {
					if (j == this->global_parameters.GlobNofFP - 1 && was_output == false) {
						must_output = true;
					}
					this->my_fpmodels[j].output_result_max2database(&this->database, this->break_sz, &was_output, must_output);
				}
			}
		}
		//couplings
		this->coupling_managment.output_final_results();

		if (this->database_is_set == true && all_output == true) {
			this->coupling_managment.output_final_results(&this->database, this->hyd_sz.get_id(), this->break_sz);
			this->coupling_managment.output_final_results_max_waterlevel(&this->database, this->hyd_sz.get_id(), this->break_sz);
		}

		//observation points
		if (this->file_output_required == true) {
			string rv_buff = label::not_set;
			string fp_buff = label::not_set;
			//output to tecplot
			if (this->global_parameters.get_number_river_model() > 0) {
				rv_buff = this->my_rvmodels[0].Param_RV.get_filename_result2file_1d_obs_point(hyd_label::tecplot);
				rv_buff += hyd_label::dat;
			}
			if (this->global_parameters.get_number_floodplain_model() > 0) {
				fp_buff = this->my_fpmodels[0].Param_FP.get_filename_obs_point2file(hyd_label::tecplot);
				fp_buff += hyd_label::dat;
			}
			cout << "Observation point data to file..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			if (this->global_parameters.get_output_flags().tecplot_1d_required == true) {
				this->obs_point_managment.output_obs_points2tecplot_file(rv_buff, fp_buff);
			}
			//output to csv
			if (this->global_parameters.get_number_river_model() > 0) {
				rv_buff = this->my_rvmodels[0].Param_RV.get_filename_result2file_1d_obs_point(hyd_label::paraview);
			}
			if (this->global_parameters.get_number_floodplain_model() > 0) {
				fp_buff = this->my_fpmodels[0].Param_FP.get_filename_obs_point2file(hyd_label::paraview);
			}
			if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
				this->obs_point_managment.output_obs_points2paraview_file(rv_buff, fp_buff);
			}
			if (this->global_parameters.get_output_flags().tecplot_1d_required == false && this->global_parameters.get_output_flags().paraview_1d_required == false) {
				cout << "No observation point output required..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);

			}
		}
	}
	else if (this->temp_calc==true) {

			ostringstream cout;
			this->output_final_temp_system_statistics();
			//river models
			if (this->global_parameters.GlobNofRV > 0) {
				cout << "Statistics of the Temperature model(s)..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
					this->my_temp_model[j].output_final();
				}
				if (this->file_output_required == true && all_output == true) {
					cout << "Output maximum results of the Temperature model(s) to file..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);


					if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
						cout << "for ParaView / csv 1d..." << endl;
						Sys_Common_Output::output_hyd->output_txt(&cout);
						for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
							this->my_temp_model[j].output_result_max2csv();
						}
					}
					if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
						cout << "for ParaView 2d..." << endl;
						Sys_Common_Output::output_hyd->output_txt(&cout);
						for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
							this->my_temp_model[j].output_result_max2paraview2d();
						}
					}
					if (this->global_parameters.get_output_flags().tecplot_1d_required == false && this->global_parameters.get_output_flags().paraview_1d_required == false) {
						cout << "No output required..." << endl;
						Sys_Common_Output::output_hyd->output_txt(&cout);
					}

				}
				if (this->database_is_set == true && all_output == true) {
					cout << "Output maximum results of the Temperature model(s) to database..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);
					for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
						this->my_temp_model[j].output_result_max2database(&this->database, this->break_sz);
					}
				}
			}


			
			//observation points
			if (this->file_output_required == true) {
				string rv_buff = label::not_set;
				string fp_buff = label::not_set;
				cout << "Observation point data to file..." << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
				//output to csv
				if (this->global_parameters.get_number_river_model() > 0) {
					rv_buff = this->my_temp_model[0].Param_Temp.get_filename_result2file_1d_obs_point(hyd_label::paraview);
				}
				if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
					
					this->obs_point_managment.output_temp_obs_points2paraview_file(rv_buff);
				}
				if (this->global_parameters.get_output_flags().tecplot_1d_required == false) {
					cout << "No observation point output required..." << endl;
					Sys_Common_Output::output_hyd->output_txt(&cout);

				}
			}



	}

	//errors/warnings
	this->set_final_warning_number();
	this->output_final_error_warning_number();
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Get the number of errors
int Hyd_Hydraulic_System::get_number_errors(void){
	return this->error_number;
}
//Get the number of errors
int Hyd_Hydraulic_System::get_number_warnings(void){
	return this->warning_number;
}
//Increase the error number by 1
void Hyd_Hydraulic_System::increase_error_number(void){
	this->error_number++;
}
//Convert the model type (_hyd_model_type) into a text string (static)
string Hyd_Hydraulic_System::convert_model_type2txt(const _hyd_model_type type){
	string txt;

	switch(type){
		case(_hyd_model_type::RIVER_MODEL):
			txt=hyd_label::river_model;
			break;
		case(_hyd_model_type::FLOODPLAIN_MODEL):
			txt=hyd_label::floodplain_model;
			break;
		case(_hyd_model_type::COAST_MODEL):
			txt=hyd_label::coast_model;
			break;
		default:
			txt=label::unknown_type;
	}

	return txt;
}
//Convert a string into a model type (_hyd_model_type) (static)
_hyd_model_type Hyd_Hydraulic_System::convert_txt2model_type(const string text){
	_hyd_model_type type;
	if(text==hyd_label::river_model){
		type=_hyd_model_type::RIVER_MODEL;
	}
	else if(text==hyd_label::floodplain_model){
		type=_hyd_model_type::FLOODPLAIN_MODEL;
	}
	else if(text==hyd_label::coast_model){
		type=_hyd_model_type::COAST_MODEL;
	}
	else{
		Error msg;
		msg.set_msg("Hyd_Hydraulic_System::convert_model_type2txt(string text)", "Can not convert this model type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert model type: " << text << endl;
		info << "Possible types: "<< endl;
		info << " " << hyd_label::river_model << endl;
		info << " " << hyd_label::floodplain_model << endl;
		info << " " << hyd_label::coast_model << endl;

		msg.make_second_info(info.str());
		throw msg;
	}

	return type;
}
//Set the system-id (_sys_system_id) of the object
void Hyd_Hydraulic_System::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->coupling_managment.set_systemid(actuel_id);
}
//Reset the system to the state before the calculation after the preproccesing
void Hyd_Hydraulic_System::reset_system(void){
	ostringstream cout;
	cout << "Reset hydraulic system thread "<<this->thread_number<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	this->output_time=0.0;
	this->internal_time=0.0;
	this->next_internal_time=0.0;
	this->timestep_counter=0;
	this->timestep_internal_counter=0;
	this->total_internal_timestep=0;
	this->diff_real_time=0.0;
	this->internal_timestep_min=10.0;

	this->error_number=0;
	this->warning_number=0;
	this->warning_number_start=0;

	this->calc_number=0;
	this->output_is_running=false;
	this->output_is_allowed=false;
	this->output_is_required=false;

	this->break_sz="CA";
	this->identifier_str=label::not_set;
	this->preproc_success_flag=false;

	ostringstream prefix;
	prefix << "RESET> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	//river models
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].reset_model(&this->global_parameters);
	}
	//floodplain models
	for(int i=0; i< this->global_parameters.GlobNofFP; i++){
		this->my_fpmodels[i].reset_model(&this->global_parameters);
	}
	//couplings
	this->coupling_managment.reset_couplings();
	this->obs_point_managment.clear_obs_points();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Clear the boundary condition of the models representing the system
void Hyd_Hydraulic_System::clear_boundary_conditions(void){
	ostringstream cout;
	cout << "Clear hydraulic boundary condition of system thread "<<this->thread_number<<"..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	ostringstream prefix;
	prefix << "CLEAR> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	//river models
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].clear_boundary_condition();
	}
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		if (this->temp_calc == true) {
			this->my_temp_model[i].clear_boundary_condition();

		}
	}
	//floodplain models
	for(int i=0; i< this->global_parameters.GlobNofFP; i++){
		this->my_fpmodels[i].clear_boundary_condition();
	}
	//coast model
	if(this->global_parameters.coastmodel_applied==true){
		this->my_comodel->clear_boundary_condition();
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Set a new set of boundary conditions to the system's models; the new boundary scenario has to be specified with set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz) before
void Hyd_Hydraulic_System::set_new_boundary_condition(void){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//river models
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
		this->my_rvmodels[i].set_new_boundary_condition(true, &this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	}
	if (this->temp_calc == true) {
		for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
			this->my_temp_model[i].set_new_hyd_bound_sz_id(this->hyd_sz);
			this->my_temp_model[i].set_new_boundary_condition(true, &this->database);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}


	//floodplain models
	for(int i=0; i< this->global_parameters.GlobNofFP; i++){
		this->my_fpmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
		this->my_fpmodels[i].set_new_boundary_condition(true, &this->database, this->my_comodel);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	}
	//coast model
	if(this->global_parameters.coastmodel_applied==true){
		this->my_comodel->set_hydraulic_bound_sz(this->hyd_sz);
		this->my_comodel->set_new_boundary_condition(true, &this->database);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Set the flag if a file output is requiered
void Hyd_Hydraulic_System::set_file_output_flag(const bool flag){
	this->file_output_required=flag;
}
//Set identifier string for the break scenario
void Hyd_Hydraulic_System::set_string_break_sc(const string break_sc){
	this->break_sz=break_sc;
}
//Get identifier string for the break scenario
string Hyd_Hydraulic_System::get_string_break_sc(void){
	return this->break_sz;
}
//Set a pointer to the database and copy it
void Hyd_Hydraulic_System::set_ptr2database(QSqlDatabase *ptr_database, const int thread_number){
	ostringstream buff;
	buff<<sys_label::str_hyd<<"_"<<thread_number;
	this->database=QSqlDatabase::cloneDatabase(*ptr_database, buff.str().c_str());
	this->database.open();
	if(this->database.isOpen()==true){
		database_is_set=true;
	}
	this->thread_number=thread_number;
}
//Set a the database without making a copy of the database connection
void Hyd_Hydraulic_System::set_ptr2database(QSqlDatabase database, const int thread_number){
	this->database=database;
	if(this->database.isOpen()==true){
		database_is_set=true;
	}
	this->thread_number=thread_number;
}
//Total reset of the hydraulic system
void Hyd_Hydraulic_System::total_reset(void){
	ostringstream cout;
	cout << "Total reset hydraulic system..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	if(this->my_fpmodels!=NULL){
		delete []this->my_fpmodels;
		this->my_fpmodels=NULL;
	}
	if(this->my_rvmodels!=NULL){
		delete []this->my_rvmodels;
		this->my_rvmodels=NULL;
	}
	if(this->my_comodel!=NULL){
		delete this->my_comodel;
		this->my_comodel=NULL;
	}
	this->output_time=0.0;
	this->internal_time=0.0;
	this->next_internal_time=0.0;
	this->timestep_counter=0;
	this->timestep_internal_counter=0;
	this->total_internal_timestep=0;
	this->diff_real_time=0.0;

	this->internal_timestep_current=0.0;
	this->internal_timestep_base=0.0;
	this->internal_timestep_min=10.0;

	this->error_number=0;
	this->warning_number=0;
	this->warning_number_start=0;

	this->calc_number=0;
	this->output_is_running=false;
	this->output_is_allowed=false;
	this->output_is_required=false;

	this->file_output_required=true;

	this->break_sz="CA";
	this->preproc_success_flag=false;

	this->global_parameters.total_reset();
	this->material_params.total_reset();
	this->coupling_managment.total_reset();
	this->obs_point_managment.total_reset();
}
//Set flag that the preprocessing was successful/not successful
void Hyd_Hydraulic_System::set_preproc_flag(const bool flag){
	this->preproc_success_flag=flag;
}
//Get the flag if the preprocessing was successful/not successful
bool Hyd_Hydraulic_System::get_preproc_flag(void){
	return this->preproc_success_flag;
}
//Set output folder name in case of a calculation via database by a given scenario name
void Hyd_Hydraulic_System::set_folder_name(const string sc_name, const bool creation_flag){
	ostringstream buffer;
	if(creation_flag==false){
		buffer<<Sys_Project::get_main_path()<<sys_label::folder_outputfile<<"/";
		buffer<<functions::convert_system_module2txt(_sys_system_modules::HYD_SYS)<<"/";
	}
	else{
		if(this->file_output_required==true){
			QDir my_dir;
			buffer<<Sys_Project::get_main_path()<<sys_label::folder_outputfile<<"/";
			if(my_dir.exists(buffer.str().c_str())==false){
				my_dir.mkdir(buffer.str().c_str());
			}
			//add modul
			buffer<<functions::convert_system_module2txt(_sys_system_modules::HYD_SYS)<<"/";
			if(my_dir.exists(buffer.str().c_str())==false){
				my_dir.mkdir(buffer.str().c_str());
			}

			//add scenario name and create folder
			buffer<<"A"<<this->system_id.area_state<<"-M"<<this->system_id.measure_nr<<"."<<sc_name<<"/";
			if(my_dir.exists(buffer.str().c_str())==false){
				my_dir.mkdir(buffer.str().c_str());
			}
			else {
				if (this->temp_calc == true) {
					//delete existing files (old style)
					QStringList list;
					my_dir.cd(buffer.str().c_str());
					list=my_dir.entryList(QDir::Files);
					for(int i=0; i<list.count(); i++){
						if (list.at(i).contains("TEMP") == true) {
							my_dir.remove(list.at(i));
						}
					}

				}
				else {
					my_dir.cd(buffer.str().c_str());
					my_dir.removeRecursively();
					my_dir.mkdir(buffer.str().c_str());
				}


			}
			//add additional folders
			my_dir.cd(buffer.str().c_str());
			if (this->global_parameters.get_output_flags().tecplot_1d_required == true || this->global_parameters.get_output_flags().tecplot_2d_required == true) {
				my_dir.mkdir(hyd_label::tecplot.c_str());
			}
			if (this->global_parameters.get_output_flags().bluekenue_2d_required == true) {
				my_dir.mkdir(hyd_label::bluekenue.c_str());
			}
			if (this->global_parameters.get_output_flags().paraview_1d_required == true || this->global_parameters.get_output_flags().paraview_2d_required == true) {
				my_dir.mkdir(hyd_label::paraview.c_str());
			}



			//add prefix for the files
			//buffer<<"DBASE";
		}
		this->file_output_folder=buffer.str();
		//set if they are already allocated to the models
		if(this->my_rvmodels!=NULL){
			for(int i=0; i<this->global_parameters.GlobNofRV; i++){
				this->my_rvmodels[i].set_output_folder(this->file_output_folder);
			}
		}
		if(this->my_fpmodels!=NULL){
			for(int i=0; i<this->global_parameters.GlobNofFP; i++){
				this->my_fpmodels[i].set_output_folder(this->file_output_folder);
			}
		}
		if(this->my_comodel!=NULL){
			this->my_comodel->set_output_folder(this->file_output_folder);
		}
	}
}
//Set output folder name in case of file calculation output
void Hyd_Hydraulic_System::set_folder_name_file(void) {
	ostringstream buffer;
	if (this->file_output_required == true) {
		QDir my_dir;
		// create folder
		buffer << this->global_parameters.get_output_flags().output_folder<< "/output" << "/";
		if (my_dir.exists(buffer.str().c_str()) == false) {
			my_dir.mkdir(buffer.str().c_str());
		}
		else {
			my_dir.cd(buffer.str().c_str());
			my_dir.removeRecursively();
			my_dir.mkdir(buffer.str().c_str());
			//delete existing files (old style)
			//QStringList list;
			//list=my_dir.entryList(QDir::Files);
			//for(int i=0; i<list.count(); i++){
			//	my_dir.remove(list.at(i));
			//}
		}
		//add additional folders
		my_dir.cd(buffer.str().c_str());
		if (this->global_parameters.get_output_flags().tecplot_1d_required == true || this->global_parameters.get_output_flags().tecplot_2d_required == true) {
			my_dir.mkdir(hyd_label::tecplot.c_str());
		}
		if (this->global_parameters.get_output_flags().bluekenue_2d_required == true) {
			my_dir.mkdir(hyd_label::bluekenue.c_str());
		}
		if (this->global_parameters.get_output_flags().paraview_1d_required == true || this->global_parameters.get_output_flags().paraview_2d_required == true) {
			my_dir.mkdir(hyd_label::paraview.c_str());
		}
		this->file_output_folder = buffer.str();

		//set if they are already allocated to the models
		if (this->my_rvmodels != NULL) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				this->my_rvmodels[i].set_output_folder(this->file_output_folder);
			}
		}
		if (this->my_fpmodels != NULL) {
			for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
				this->my_fpmodels[i].set_output_folder(this->file_output_folder);
			}
		}
		if (this->my_comodel != NULL) {
			this->my_comodel->set_output_folder(this->file_output_folder);
		}

	}


}
//Set identifier string
void Hyd_Hydraulic_System::set_identifier_string(const string identifier){
	this->identifier_str=identifier;
}
//Get the identifier prefix for the output to display/console
string Hyd_Hydraulic_System::get_identifier_prefix(const bool as_prefix){
	ostringstream buff_stream;

	if(as_prefix==true){
		if(this->identifier_str==label::not_set){
			buff_stream<< "SYS_"<<this->calc_number<<"> ";
		}
		else{
			buff_stream<<this->identifier_str<<"> ";
		}
	}
	else{
		if(this->identifier_str==label::not_set){
			buff_stream<< "SYS "<<this->calc_number;
		}
		else{
			buff_stream<<this->identifier_str;
		}
	}
	return buff_stream.str();
}
//Get the pointer to a floodplain model by a given model number
Hyd_Model_Floodplain* Hyd_Hydraulic_System::get_ptr_floodplain_model(const int number){
	for(int i=0; i<this->global_parameters.get_number_floodplain_model(); i++){
		if(this->my_fpmodels[i].Param_FP.get_floodplain_number()==number){
			return &this->my_fpmodels[i];
		}
	}
	return NULL;
}
//Get the pointer to a floodplain model by a given index
Hyd_Model_Floodplain* Hyd_Hydraulic_System::get_ptr_floodplain_model_index(const int index){
	if(index<0 || index>this->global_parameters.GlobNofFP){
		return NULL;
	}
	else{
		return &this->my_fpmodels[index];
	}
}
//Get the pointer to a river model by a given model number
Hyd_Model_River* Hyd_Hydraulic_System::get_ptr_river_model(const int number){
	for(int i=0; i<this->global_parameters.get_number_river_model(); i++){
		if(this->my_rvmodels[i].Param_RV.get_river_number()==number){
			return &this->my_rvmodels[i];
		}
	}
	return NULL;
}
//Get the pointer to a river model by a given index
Hyd_Model_River* Hyd_Hydraulic_System::get_ptr_river_model_index(const int index){
	if(index<0 || index>this->global_parameters.GlobNofRV){
		return NULL;
	}
	else{
		return &this->my_rvmodels[index];
	}
}
//Output the number of error(s)/warning(s)
void Hyd_Hydraulic_System::output_final_error_warning_number(void){
	ostringstream cout;
	cout << "FINAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)   :" << W(3) << this->error_number <<endl;
	cout << " Warning(s) :" << W(3) << this->warning_number <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Set the total riverline to no-overflow
void Hyd_Hydraulic_System::set_total_river2no_overflow(void){
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].set_total_river2no_overflow();
	}
}
//Get if the output to display is running
bool Hyd_Hydraulic_System::get_output_is_running(void){
	return this->output_is_running;
}
//Get if the output to display is required
bool Hyd_Hydraulic_System::get_output_is_required(void){
	return this->output_is_required;
}
//Set that an output is allowed
void Hyd_Hydraulic_System::set_output_is_allowed(void){
	if(this->output_is_required==true){
		this->output_is_allowed=true;
		this->output_is_running=true;
	}
	else{
		this->output_is_allowed=false;
	}
}
//Input just the river models per database (used in the Alt_System)
void Hyd_Hydraulic_System::input_just_river_models(QSqlDatabase *ptr_database, const bool with_output){
	this->global_parameters.globals_per_database(ptr_database, with_output);
	//input the material parameters
	this->material_params.matparams_per_database(ptr_database, with_output);

	QSqlTableModel model(0,*ptr_database);
	this->global_parameters.GlobNofRV=Hyd_Model_River::select_relevant_model_database(&model, this->system_id, with_output);
	//allocate rv-models
	this->allocate_river_models();
	//input
	try{
		//set the parameters for Hyd_Model_River from database
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			this->my_rvmodels[j].set_systemid(this->system_id);
			this->my_rvmodels[j].input_members(j,&model, ptr_database, true, false, with_output);
		}
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			this->my_rvmodels[j].init_river_model_geometry();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Initialize and connect the data of the rivers
void Hyd_Hydraulic_System::connect_rivers(Hyd_Param_Material *mat_param){
	ostringstream cout;
	ostringstream prefix;
	try{
		//set the parameters for Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			prefix << "RV_"<<j<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			prefix.str("");
			//connect the elems
			cout<<"Initialize the data of river model " << j << "..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[j].init_river_model(mat_param);
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
}
//Initialize and connect the data of the temperature models
void Hyd_Hydraulic_System::connect_temperature_model(void) {
	ostringstream cout;
	ostringstream prefix;
	try {
		//set the parameters for Hyd_Model_River
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			prefix << "TEMPRV_" << j << "> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			prefix.str("");
			//connect the elems
			cout << "Initialize the data of temperature model " << j << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_temp_model[j].init_temp_model(&this->database, this->global_parameters.get_number_timesteps(), this->global_parameters.get_stepsize());
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
	}
	catch (Error msg) {
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

}
//Set river inflow boundary condition to true for all river models (used in Alt-System)
void Hyd_Hydraulic_System::set_rv_inflow_flag(void){
	for (int j =0; j < this->global_parameters.GlobNofRV; j++){
		this->my_rvmodels[j].inflow_river_profile.set_point_bound_flag(true);	
	}
}
//Set if the temperature calculation is applied
void Hyd_Hydraulic_System::set_temp_calc_apply(const bool flag) {
	this->temp_calc = flag;
}
//______________
//private
//Output final statistic of the system
void Hyd_Hydraulic_System::output_final_system_statistics(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "GLOB> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Statistics of the Hydraulic-System..." << endl ;
	cout << " TIME" << endl;
	cout << "  Simulation time          : " << functions::convert_seconds2string(this->actual_time-this->start_time) << label::time_unit_output<< endl;
	cout << "  Real time                : " << functions::convert_seconds2string(this->output_time-this->global_parameters.GlobTStep-this->global_parameters.get_startime());
	cout << label::time_unit_output << endl;
	cout << "  Relation (Sim t)/(Real t): " << P(3) << FORMAT_FIXED_REAL ;
	cout << (this->actual_time-this->start_time)/((this->output_time-this->global_parameters.GlobTStep-this->global_parameters.get_startime())/3600.0) <<label::sec_per_hour<< endl;
	cout << " SPACE" << endl;
	double buff=0.0;
	double buff2=0.0;
	if(this->global_parameters.GlobNofRV>0){
		for(int i=0; i<this->global_parameters.GlobNofRV; i++){
			buff=buff+this->my_rvmodels[i].river_midline.get_total_distance();
		}
		for(int i=0; i<this->global_parameters.GlobNofRV; i++){
			buff2=buff2+this->my_rvmodels[i].river_polygon.calculate_area();
		}
		cout << "  Number river models      : " << this->global_parameters.GlobNofRV << endl;
		cout << "  Total river length       : " <<  P(0) << FORMAT_FIXED_REAL <<  buff << label::m<< endl;
		cout << "  Total river area         : " <<  P(0) << FORMAT_FIXED_REAL <<  buff2 << label::squaremeter<< endl;
	}
	buff=0.0;
	if(this->global_parameters.GlobNofFP>0){
		for(int i=0; i<this->global_parameters.GlobNofFP; i++){
			buff=buff+this->my_fpmodels[i].raster.geometrical_bound.calculate_area();
		}
		cout << "  Number floodplain models : " << this->global_parameters.GlobNofFP<< endl;
		cout << "  Total floodplain area    : " << buff << label::squaremeter << endl;
		cout << "  Wetted area              : " << this->calculate_was_wetted_area_fp(&buff) << label::squaremeter << endl;
		cout << "  Wetted floodplain area   : " << buff << label::squaremeter << endl;
	}

	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output final statistics of the temperature system
void Hyd_Hydraulic_System::output_final_temp_system_statistics(void) {
	//set prefix for output
	ostringstream prefix;
	prefix << "GLOB> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Statistics of the Hydraulic-Temperature-System..." << endl;
	cout << " TIME" << endl;
	cout << "  Simulation time          : " << functions::convert_seconds2string(this->actual_time - this->start_time) << label::time_unit_output << endl;
	cout << "  Real time                : " << functions::convert_seconds2string(this->output_time - this->global_parameters.GlobTStep - this->global_parameters.get_startime());
	cout << label::time_unit_output << endl;
	cout << "  Relation (Sim t)/(Real t): " << P(3) << FORMAT_FIXED_REAL;
	cout << (this->actual_time - this->start_time) / ((this->output_time - this->global_parameters.GlobTStep - this->global_parameters.get_startime()) / 3600.0) << label::sec_per_hour << endl;
	
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Allocate the floodplain model
void Hyd_Hydraulic_System::allocate_floodplain_models(void){
	// alloc the fp-models
	try{
		this->my_fpmodels = new Hyd_Model_Floodplain [this->global_parameters.GlobNofFP];
	}
	catch(bad_alloc& ){
		Error msg=this->set_error(0);
		throw msg;
	}
	//set the szenario data
	for(int i=0; i< this->global_parameters.GlobNofFP; i++){
		this->my_fpmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
		this->my_fpmodels[i].set_systemid(this->system_id);
		if(this->file_output_required==true){
			this->my_fpmodels[i].set_output_folder(this->file_output_folder);
		}
	}
}
//Read in the floodplain models of the system with Hyd_Parse_FP
void Hyd_Hydraulic_System::input_floodplains_models(const string global_file){
	ostringstream cout;

	if(this->global_parameters.GlobNofFP>0){
		cout << "Read in  " << this->global_parameters.GlobNofFP << " Floodplainmodel(s) per parser..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//set the parameters for Hyd_Model_Floodplain
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			cout<<"Set Floodplainmodel " << j << " with file..." <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_fpmodels[j].input_members(global_file,j,this->global_parameters.get_global_path());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the data of the floodplain models to the database
void Hyd_Hydraulic_System::transfer_floodplainmodel_data2database(QSqlDatabase *ptr_database){
	ostringstream cout;

	if(this->global_parameters.GlobNofFP>1){
		cout << "Transfer data of " << this->global_parameters.GlobNofFP << " Floodplainmodel(s) to database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//transfer the data of Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			cout<<"Transfer data of Floodplainmodel " << j << "..." <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_fpmodels[j].transfer_input_members2database(ptr_database);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Read in the floodplain models of the system from a database
void Hyd_Hydraulic_System::input_floodplains_models(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool just_elems){
	ostringstream cout;
	if(this->global_parameters.GlobNofFP>0){
		cout << "Read in " << this->global_parameters.GlobNofFP << " Floodplainmodel(s) from database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//set the parameters for Hyd_Model_River from database
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			this->my_fpmodels[j].set_systemid(this->system_id);
			this->my_fpmodels[j].input_members(j,query_result, ptr_database, just_elems, false, true);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Initialize and connect the data of the floodplain models
void Hyd_Hydraulic_System::connect_floodplains(void){
	ostringstream cout;
	ostringstream prefix;
	try{
		//set the parameters for Hyd_Model_Floodplain
		for (int j =0; j < this->global_parameters.GlobNofFP; j++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			prefix << "FP_"<<j<<"> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			prefix.str("");
			//connect the elems
			cout<<"Initialize the data of floodplain model " << j << "..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_fpmodels[j].init_floodplain_model(&this->material_params, this->my_comodel);
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
}
//Allocate the river model
void Hyd_Hydraulic_System::allocate_river_models(void){
	// alloc the rv-models
	try{
		this->my_rvmodels = new Hyd_Model_River [this->global_parameters.GlobNofRV];
	}
	catch(bad_alloc& ){
		Error msg=this->set_error(0);
		throw msg;
	}
	//set the szenario data
	for(int i=0; i< this->global_parameters.GlobNofRV; i++){
		this->my_rvmodels[i].set_new_hyd_bound_sz_id(this->hyd_sz);
		this->my_rvmodels[i].set_systemid(this->system_id);
		if(this->file_output_required==true){
			this->my_rvmodels[i].set_output_folder(this->file_output_folder);
		}
	}
}
//Read in the river models of the system with Hyd_Parse_RV
void Hyd_Hydraulic_System::input_rivers_models(const string global_file){
	ostringstream cout;

	if(this->global_parameters.GlobNofRV>0){
		cout << "Read in  " << this->global_parameters.GlobNofRV << " Rivermodel(s) per parser..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//set the parameters for Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			cout<<"Set Rivermodel " << j << " with file..." <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[j].input_members(global_file,j,this->global_parameters.get_global_path());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Read in the river models of the system with given parameters
void Hyd_Hydraulic_System::input_rivers_models(const Hyd_Param_RV param_rv){
	ostringstream cout;

	if(this->global_parameters.GlobNofRV>0){
		cout << "Read in  " << this->global_parameters.GlobNofRV << " Rivermodel(s) per parser..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//set the parameters for Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			cout<<"Set Rivermodel " << j << " with file..." <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[j].input_members(param_rv);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the data of the river models to the database
void Hyd_Hydraulic_System::transfer_rivermodel_data2database(QSqlDatabase *ptr_database){
	ostringstream cout;

	if(this->global_parameters.GlobNofRV>1){
		cout << "Transfer data of " << this->global_parameters.GlobNofRV << " Rivermodel(s) to database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try{
		//transfer the data of Hyd_Model_River
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			cout<<"Transfer data of Rivermodel " << j << "..." <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_rvmodels[j].transfer_input_members2database(ptr_database);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Read in the river models of the system from a database
void Hyd_Hydraulic_System::input_rivers_models(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool with_output){
	ostringstream cout;
	if(with_output==true){
		if(this->global_parameters.GlobNofRV>0){
			cout << "Read in " << this->global_parameters.GlobNofRV << " Rivermodel(s) from database..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
	}

	try{
		//set the parameters for Hyd_Model_River from database
		for (int j =0; j < this->global_parameters.GlobNofRV; j++){
			this->my_rvmodels[j].set_systemid(this->system_id);
			this->my_rvmodels[j].input_members(j,query_result, ptr_database, false, true, with_output);
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Allocate the coast model
void Hyd_Hydraulic_System::allocate_coast_model(void){
	if(this->global_parameters.coastmodel_applied==true){
		try{
			this->my_comodel=new Hyd_Coast_Model;
		}catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
		//set the szenario data
		this->my_comodel->set_hydraulic_bound_sz(this->hyd_sz);

		this->my_comodel->set_systemid(this->system_id);
		if(this->file_output_required==true){
			this->my_comodel->set_output_folder(this->file_output_folder);
		}
	}
}
//Read in the coast model of the system with the parser class CO
void Hyd_Hydraulic_System::input_coast_model(const string global_file){
	if(this->global_parameters.coastmodel_applied==true){
		ostringstream cout;
		cout << "Read in the coastmodel per parser..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		try{
			this->my_comodel->input_members(global_file, this->global_parameters.get_global_path());
		}
		catch(Error msg){
			throw msg;
		}
	}
}
//Read in the coast model of the system from a selection of a database
void Hyd_Hydraulic_System::input_coast_model(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool with_output){
	if(this->global_parameters.coastmodel_applied==true){
		try{
			this->my_comodel->input_members(query_result, ptr_database, with_output);
		}
		catch(Error msg){
			throw msg;
		}
	}
}
//Allocate the temperature model
void Hyd_Hydraulic_System::allocate_temp_model(void) {
	if (this->global_parameters.tempmodel_applied == true && this->global_parameters.GlobNofRV>0) {
		try {
			this->my_temp_model = new HydTemp_Model[this->global_parameters.GlobNofRV];
		}
		catch (bad_alloc &) {
			Error msg = this->set_error(0);
			throw msg;
		}
		//set the szenario data
		for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
			this->my_temp_model[i].set_ptr2RV(&(this->my_rvmodels[i]));
			this->my_temp_model[i].set_systemid(this->system_id);
			this->my_temp_model[i].set_new_hyd_bound_sz_id(this->hyd_sz);
			
			
			if (this->file_output_required == true) {
				this->my_temp_model[i].set_output_folder(this->file_output_folder);
			}
		}
	}
	else {
		this->global_parameters.tempmodel_applied =false;
		Warning msg = this->set_warning(1);
		msg.output_msg(2);
	}

}
//Read in the temperature model of the system with HydTemp_Parse
void Hyd_Hydraulic_System::input_temp_model(const string global_file) {
	ostringstream cout;

	if (this->global_parameters.GlobNofRV > 0) {
		cout << "Read in temperature model of " << this->global_parameters.GlobNofRV << " Rivermodel(s) per parser..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try {
		//set the parameters for Hyd_Model_River
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			cout << "Set temperature model " << j << " with file..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_temp_model[j].input_members(global_file, j, this->global_parameters.get_global_path());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch (Error msg) {
		throw msg;
	}



}
//Transfer the data of the river models to the database
void Hyd_Hydraulic_System::transfer_tempmodel_data2database(QSqlDatabase *ptr_database) {
	ostringstream cout;

	if (this->global_parameters.GlobNofRV > 1) {
		cout << "Transfer data of " << this->global_parameters.GlobNofRV << " Temperature model(s) to database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try {
		//transfer the data of Hyd_Model_River
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			cout << "Transfer data of Temperature model " << j << "..." << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			this->my_temp_model[j].transfer_input_members2database(ptr_database);
		}
	}
	catch (Error msg) {
		throw msg;
	}

}
//Read in the temperature model of the system from selection of a database
void Hyd_Hydraulic_System::input_temp_model(const QSqlTableModel *query_result, QSqlDatabase *ptr_database, const bool with_output) {
	ostringstream cout;

	if (this->global_parameters.GlobNofRV > 0) {
		cout << "Read in temperature model of " << this->global_parameters.GlobNofRV << " Rivermodel(s) from database..." << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	try {
		//set the parameters for Hyd_Model_River
		for (int j = 0; j < this->global_parameters.GlobNofRV; j++) {
			this->my_temp_model[j].set_systemid(this->system_id);

			this->my_temp_model[j].input_members(j, query_result, ptr_database, false, true, with_output);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch (Error msg) {
		throw msg;
	}

}
//Make the geometrical interceptions between the models
void Hyd_Hydraulic_System::make_geometrical_interception(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "INTCEP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	try{
		//between the river models
		this->make_geometrical_interception_fp2rv();
		//between the floodplain models
		this->make_geometrical_interception_fp2fp();
		//between a coast model and the floodplain models
		this->make_geometrical_interception_co2fp();
		//between a coast and river models
		this->make_geometrical_interception_rv2co();
		//between the river- and river models
		this->make_geometrical_interception_rv2rv();
	}
	catch(Error msg){
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Make geometrical interceptions between coast model and floodplain models
void Hyd_Hydraulic_System::make_geometrical_interception_co2fp(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	if(this->global_parameters.GlobNofFP>0 && this->global_parameters.coastmodel_applied==true){
		ostringstream cout;
		cout << "Geometrical interception between coast-model and floodplain-models..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//set prefix for output
		ostringstream prefix;
		prefix << "CO_FP> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//Check if the first point of the coastline poly segment is outside the floodplains
		for(int i=0; i<this->global_parameters.GlobNofFP; i++){
			if(this->my_fpmodels[i].raster.geometrical_bound.check_point_inside_atboundary(this->my_comodel->coastline_polysegment.get_first_point())==true){
				Error msg=this->set_error(2);
				throw msg;
			}
		}

		//make the interception
		//list for counting an initializing the couplings
		Hyd_Coupling_Model_List list;
		_hyd_model_coupling coup_buff;
		for(int i=0; i<this->global_parameters.GlobNofFP; i++){
			if(this->my_fpmodels[i].intercept_coast_model(&this->my_comodel->coast_polygon)==true){
				//first model is coast model
				coup_buff.index_first_model=0;
				//second model is floodplain model
				coup_buff.index_second_model=i;
				list.set_new_model_coupling(&coup_buff);
			}
		}
		//coupling
		if(list.get_number_couplings()>0){
			//set the couplings
			this->coupling_managment.add_fp2co(list.get_number_couplings());
			//allocate coupling classes
			this->coupling_managment.allocate_coupling_class_fp2co();
			for(int i=0; i<list.get_number_couplings(); i++){
				//init it
				this->coupling_managment.coupling_fp2co[i].set_ptr_coupling(this->my_comodel, &(this->my_fpmodels[list.get_model_coupling(i).index_second_model]));
			}
		}

		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Make geometrical interceptions between floodplain and floodplain models
void Hyd_Hydraulic_System::make_geometrical_interception_fp2fp(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	if(this->global_parameters.GlobNofFP>1){
		ostringstream cout;
		cout << "Geometrical interception between floodplain-models..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//set prefix for output
		ostringstream prefix;
		prefix << "FP_FP> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//make the interception
		int count=1;
		//list for counting an initializing the couplings
		Hyd_Coupling_Model_List list;
		_hyd_model_coupling coup_buff;
		for(int i=0; i<this->global_parameters.GlobNofFP-1; i++){
			for(int j=count; j<this->global_parameters.GlobNofFP; j++){
				if(this->my_fpmodels[i].intercept_model(&(this->my_fpmodels[j]), &coup_buff)==true){
					list.set_new_model_coupling(&coup_buff);
				}
			}
			count++;
		}
		if(list.get_number_couplings()>0){
			//count the couplings
			this->coupling_managment.add_fp2fp(list.get_number_couplings());
			//allocate coupling classes
			this->coupling_managment.allocate_coupling_class_fp2fp();
			for(int i=0; i< list.get_number_couplings(); i++){
				//set it
				this->coupling_managment.coupling_fp2fp[i].set_ptr_coupling(&(this->my_fpmodels[list.get_model_coupling(i).index_first_model]), &(this->my_fpmodels[list.get_model_coupling(i).index_second_model]));
			}
		}
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Make geometrical interceptions between floodplain and river models
void Hyd_Hydraulic_System::make_geometrical_interception_fp2rv(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	if(this->global_parameters.GlobNofFP>0){
		ostringstream cout;
		cout << "Geometrical interception between floodplain- and river-models..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//set prefix for output
		ostringstream prefix;
		prefix << "FP_RV> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//make the interception
		//list for counting an initializing the couplings
		Hyd_Coupling_Model_List list;
		_hyd_model_coupling coup_buff;
		for(int i=0; i<this->global_parameters.GlobNofFP; i++){
			for(int j=0; j<this->global_parameters.GlobNofRV; j++){
				if(this->my_fpmodels[i].intercept_model(&(this->my_rvmodels[j]))==true){
					//first model is river model
					coup_buff.index_first_model=j;
					//second model is floodplain model
					coup_buff.index_second_model=i;
					list.set_new_model_coupling(&coup_buff);
				}
			}
		}

		if(list.get_number_couplings()>0){
			//set the couplings
			this->coupling_managment.add_rv2fp(list.get_number_couplings());
			//allocate coupling classes
			this->coupling_managment.allocate_coupling_class_rv2fp();
			for(int i=0; i<list.get_number_couplings(); i++){
				//init it
				this->coupling_managment.coupling_rv2fp[i].set_ptr_coupling(&(this->my_fpmodels[list.get_model_coupling(i).index_second_model]), &(this->my_rvmodels[list.get_model_coupling(i).index_first_model]));
			}
		}
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Make the geometrical interceptions between coast model and river models
void Hyd_Hydraulic_System::make_geometrical_interception_rv2co(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	if(this->global_parameters.GlobNofRV>0 && this->global_parameters.coastmodel_applied==true){
		ostringstream cout;
		cout << "Geometrical interception between rivermodels and coastmodel..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//set prefix for output
		ostringstream prefix;
		prefix << "RV_CO> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//list for counting an initializing the couplings
		Hyd_Coupling_Model_List list;
		_hyd_model_coupling coup_buff;
		//make the interception
		for(int i=0; i<this->global_parameters.GlobNofRV; i++){
			if(this->my_rvmodels[i].intercept_coast_model(&(this->my_comodel->coast_polygon))==true){
				//first model is coast  model
				coup_buff.index_first_model=0;
				//second model is river model
				coup_buff.index_second_model=i;
				list.set_new_model_coupling(&coup_buff);
			}
		}
		if(list.get_number_couplings()>0){
			//set the couplings
			this->coupling_managment.add_rv2co(list.get_number_couplings());
			//allocate the coupling class
			this->coupling_managment.allocate_coupling_class_rv2co();
			for(int i=0; i< list.get_number_couplings(); i++){
				//init it
				this->coupling_managment.coupling_rv2co[i].set_ptr_coupling(this->my_comodel, &(this->my_rvmodels[list.get_model_coupling(i).index_second_model]));
			}
		}
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Make the geometrical interceptions between river- and river models
void Hyd_Hydraulic_System::make_geometrical_interception_rv2rv(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	if(this->global_parameters.GlobNofRV>1){
		ostringstream cout;
		cout << "Geometrical interception between rivermodels and rivermodels..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		//set prefix for output
		ostringstream prefix;
		prefix << "RV_RV> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//list for counting an initializing the couplings
		Hyd_Coupling_Model_List list;
		_hyd_model_coupling coup_buff;
		//make the interception
		int count=1;
		for(int i=0; i<this->global_parameters.GlobNofRV-1; i++){
			for(int j=count; j<this->global_parameters.GlobNofRV; j++){
				try{
					//first check it
					this->my_rvmodels[i].check_river_modelsby_interception(&this->my_rvmodels[j]);
					this->my_rvmodels[j].check_river_modelsby_interception(&this->my_rvmodels[i]);
				}
				catch(Error msg){
					throw msg;
				}

				if(this->my_rvmodels[j].get_diversion_channel_flag()==false){
					if(this->my_rvmodels[i].intercept_river_model(&(this->my_rvmodels[j]))==true){
						//first model is river model (inflow)
						coup_buff.index_first_model=i;
						//second model is river model (outflow)
						coup_buff.index_second_model=j;
						list.set_new_model_coupling(&coup_buff);
					}
				}
				if(this->my_rvmodels[i].get_diversion_channel_flag()==false){
					if(this->my_rvmodels[j].intercept_river_model(&(this->my_rvmodels[i]))==true){
						//first model is river model (inflow)
						coup_buff.index_first_model=j;
						//second model is river model (outflow)
						coup_buff.index_second_model=i;
						list.set_new_model_coupling(&coup_buff);
					}
				}
			}
			count++;
		}
		if(list.get_number_couplings()>0){
			//set the couplings
			this->coupling_managment.add_rv2rv(list.get_number_couplings());
			//allocate the coupling class
			this->coupling_managment.allocate_coupling_class_rv2rv();
			for(int i=0; i< list.get_number_couplings(); i++){
				//init it
				this->coupling_managment.coupling_rv2rv[i].set_ptr_coupling(&(this->my_rvmodels[list.get_model_coupling(i).index_first_model]), &(this->my_rvmodels[list.get_model_coupling(i).index_second_model]));
			}
		}
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Initialize the automatic coupling
void Hyd_Hydraulic_System::init_automatic_coupling(void){
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	//set prefix for output
	ostringstream prefix;
	prefix << "COUP> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	try{
		this->coupling_managment.init_couplings();
	}
	catch(Error msg){
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Check the models
void Hyd_Hydraulic_System::check_models(void){
	//check river models
	try{
		for(int i=0; i< this->global_parameters.GlobNofRV;i++){
			this->my_rvmodels[i].check_river_model();
		}
	}
	catch(Error msg){
		throw msg;
	}

	//check river temp models
	try {
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				this->my_temp_model[i].check_temp_model();
			}
		}
	}
	catch (Error msg) {
		throw msg;
	}
}
//make the calculation the internal loop (output time steps)
void Hyd_Hydraulic_System::make_calculation_internal(void){
	//starttime for this internal time step

	this->internal_time=this->output_time-this->global_parameters.GlobTStep;

	//ostringstream out;
	//out<< this->get_identifier_prefix()<<" Start internal"<<endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);

	//loop over the internal timesteps
	do{
		//calculate optimal internal timestep
		this->internal_timestep_current=this->check_internal_timestep();
		this->internal_timestep_current = this->internal_timestep_base;

		//evaluate the internal timestep
		if(this->next_internal_time+this->internal_timestep_current<=this->output_time){
			this->next_internal_time=this->next_internal_time+this->internal_timestep_current;
		}
		else{
			this->internal_timestep_current=this->output_time-this->next_internal_time;
			this->next_internal_time=this->output_time;
		}

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//sycronisation of the models; set boundary condition

		//out << this->get_identifier_prefix() << " syncron" << endl;
		//Sys_Common_Output::output_hyd->output_txt(&out);

		this->profiler.profile("make_syncron_rivermodel", Profiler::profilerFlags::START_PROFILING);
		this->make_syncron_rivermodel();
		this->profiler.profile("make_syncron_rivermodel", Profiler::profilerFlags::END_PROFILING);
		//2DGPU
		this->profiler.profile("make_syncron_floodplainmodel", Profiler::profilerFlags::START_PROFILING);
		this->make_syncron_floodplainmodel();
		this->profiler.profile("make_syncron_floodplainmodel", Profiler::profilerFlags::END_PROFILING);
		if(this->global_parameters.coastmodel_applied==true){
			this->my_comodel->make_syncronisation((this->internal_time+this->internal_timestep_current*0.5)-this->global_parameters.get_startime());
		}
		//syncronisation between the models via couplings; set exchange terms between the models
		this->profiler.profile("synchronise_couplings", Profiler::profilerFlags::START_PROFILING);
		this->coupling_managment.synchronise_couplings((this->internal_time+this->internal_timestep_current*0.5)-this->global_parameters.get_startime(),this->internal_timestep_current, false, this->total_internal_timestep, &profiler);
		this->profiler.profile("synchronise_couplings", Profiler::profilerFlags::END_PROFILING);

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//get the maximum change of the river models (here just the correct old value is calculated)
		//double buff1;
		//double buff2;
		//this->get_max_changes_rivermodel(&buff1, &buff2, false, this->internal_timestep_current);
		//get the maximum change of the floodplain models (here just the correct old value is calculated)
		//this->get_max_changes_floodplainmodel(&buff1, false);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//out << this->get_identifier_prefix() << " RV" << endl;
		//Sys_Common_Output::output_hyd->output_txt(&out);

		//calculation river models
		this->profiler.profile("make_calculation_rivermodel", Profiler::profilerFlags::START_PROFILING);
		this->make_calculation_rivermodel();
		this->profiler.profile("make_calculation_rivermodel", Profiler::profilerFlags::END_PROFILING);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//out << this->get_identifier_prefix() << " FP" << endl;
		//Sys_Common_Output::output_hyd->output_txt(&out);

		//calculation floodplain models
		//2DGPU
		this->profiler.profile("make_calculation_floodplainmodel", Profiler::profilerFlags::START_PROFILING);
		this->make_calculation_floodplainmodel();
		this->profiler.profile("make_calculation_floodplainmodel", Profiler::profilerFlags::END_PROFILING);
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();



		//max values and hydrological balance of the models
		this->profiler.profile("make_hyd_balance_max_rivermodel", Profiler::profilerFlags::START_PROFILING);
		this->make_hyd_balance_max_rivermodel();
		this->profiler.profile("make_hyd_balance_max_rivermodel", Profiler::profilerFlags::END_PROFILING);
		this->profiler.profile("make_hyd_balance_max_floodplainmodel", Profiler::profilerFlags::START_PROFILING);
		this->make_hyd_balance_max_floodplainmodel();
		this->profiler.profile("make_hyd_balance_max_floodplainmodel", Profiler::profilerFlags::END_PROFILING);

		//syncronisation of observation points
		this->obs_point_managment.syncron_obs_points(this->next_internal_time - this->global_parameters.get_startime());

		this->internal_time=this->next_internal_time;

		this->timestep_internal_counter++;
		this->total_internal_timestep++;

		//reset the solver tolerances for the next step
		this->reset_solver_tolerances();
	}//end of interal loop
	while(abs(this->internal_time-this->output_time)>constant::sec_epsilon);
	//reset time difference
	this->diff_real_time=0.0;


	//out << this->get_identifier_prefix() << " loop out" << endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);

	//time calculations
	//transform the seconds of the output time step into hours, day etc
	this->model_time_str=functions::convert_seconds2string(this->output_time);
	//set the actual time
	this->diff_real_time=this->actual_time;
	time(&this->actual_time);
	this->diff_real_time=this->actual_time-this->diff_real_time;
	//time difference
	this->real_time_str=functions::convert_seconds2string(this->actual_time-this->start_time);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

	//time string
	string time;


	time=functions::convert_time2time_str(this->internal_time);

	//out << this->get_identifier_prefix() << " output" << endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);

	//output the results of the river models to file
	this->profiler.profile("output_calculation_steps_rivermodel2file", Profiler::profilerFlags::START_PROFILING);
	this->output_calculation_steps_rivermodel2file(this->internal_time);
	this->profiler.profile("output_calculation_steps_rivermodel2file", Profiler::profilerFlags::END_PROFILING);
	
	//output the results of the floodplain models to file
	this->profiler.profile("output_calculation_steps_floodplainmodel2file", Profiler::profilerFlags::START_PROFILING);
	this->output_calculation_steps_floodplainmodel2file(this->internal_time);
	this->profiler.profile("output_calculation_steps_floodplainmodel2file", Profiler::profilerFlags::END_PROFILING);
	

	//output the break results per step to file
	this->coupling_managment.output_coupling_calculation_steps2file(this->internal_time);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

	this->output_is_running=false;
	this->output_is_allowed=false;
	this->output_is_required=true;
	emit output_required(this->thread_number);

	this->waitloop_output_calculation2display();
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	this->output_calculation_steps_rivermodel2display(this->internal_time);
	this->output_calculation_steps_rivermodel2database(this->internal_time, time);
	this->output_calculation_steps_floodplainmodel2display(this->internal_time);
	this->output_calculation_steps_floodplainmodel2database(this->internal_time, time);
	if(Hyd_Hydraulic_System::qt_thread_applied==true){
		Sys_Common_Output::output_hyd->insert_separator(0);
	}
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	this->output_is_required=false;
	this->output_is_running=false;
	this->output_is_allowed=false;

	//reset internal timestep counter
	this->timestep_internal_counter=0;
}
//Make the temperature calculation for the internal loop
void Hyd_Hydraulic_System::make_temp_calculation_internal(void) {
	//starttime for this internal time step

	this->internal_time = this->output_time - this->global_parameters.GlobTStep;

	//ostringstream out;
	//out<< this->get_identifier_prefix()<<" Start internal"<<endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);

	//loop over the internal timesteps
	do {
		//calculate optimal internal timestep
		this->internal_timestep_current = this->check_internal_timestep();
		this->internal_timestep_current = this->internal_timestep_base;

		//evaluate the internal timestep
		if (this->next_internal_time + this->internal_timestep_current <= this->output_time) {
			this->next_internal_time = this->next_internal_time + this->internal_timestep_current;
		}
		else {
			this->internal_timestep_current = this->output_time - this->next_internal_time;
			this->next_internal_time = this->output_time;
		}

		this->make_syncron_tempmodel();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//calculation  models
		this->make_calculation_tempmodel();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();		

		//max values and hydrological balance of the models
		this->make_hyd_balance_max_tempmodel();
		
		//syncronisation of observation points
		this->obs_point_managment.syncron_temp_obs_points(this->next_internal_time - this->global_parameters.get_startime());


		this->internal_time = this->next_internal_time;

		this->timestep_internal_counter++;
		this->total_internal_timestep++;

		//reset the solver tolerances for the next step
		this->reset_solver_tolerances();
	}//end of interal loop
	while (abs(this->internal_time - this->output_time) > constant::sec_epsilon);
	//reset time difference
	this->diff_real_time = 0.0;

	//time calculations
	//transform the seconds of the output time step into hours, day etc
	this->model_time_str = functions::convert_seconds2string(this->output_time);
	//set the actual time
	this->diff_real_time = this->actual_time;
	time(&this->actual_time);
	this->diff_real_time = this->actual_time - this->diff_real_time;
	//time difference
	this->real_time_str = functions::convert_seconds2string(this->actual_time - this->start_time);
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

	//time string
	string time;


	time = functions::convert_time2time_str(this->internal_time);

	//output the results of the  models to file
	this->output_calculation_steps_tempmodel2file(this->internal_time);

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

	this->output_is_running = false;
	this->output_is_allowed = false;
	this->output_is_required = true;
	emit output_required(this->thread_number);

	this->waitloop_output_calculation2display();
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	this->output_calculation_steps_tempmodel2display(this->internal_time);
	this->output_calculation_steps_tempmodel2database(this->internal_time, time);

	if (Hyd_Hydraulic_System::qt_thread_applied == true) {
		Sys_Common_Output::output_hyd->insert_separator(0);
	}
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	this->output_is_required = false;
	this->output_is_running = false;
	this->output_is_allowed = false;

	//reset internal timestep counter
	this->timestep_internal_counter = 0;

}
//Reset the solver-tolerances of each model
void Hyd_Hydraulic_System::reset_solver_tolerances(void){
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		//this->warning_number=this->warning_number+this->my_rvmodels[i].get_warn_counter();
		this->my_rvmodels[i].reset_solver_tolerances();
	}
    for(int i=0; i< this->global_parameters.GlobNofFP;i++){
        this->warning_number=this->warning_number+this->my_fpmodels[i].get_warn_counter();
        this->my_fpmodels[i].reset_solver_tolerances();
    }
}
//Reset all coupling discharges of the river models
void Hyd_Hydraulic_System::reset_coupling_discharge_river_models(void){
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		this->my_rvmodels[i].reset_coupling_discharge();;
	}
}
//Make the syncronisation of the river models for each internal step
void Hyd_Hydraulic_System::make_syncron_rivermodel(void){
	//make syncronisation(boundary conditions)
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_rvmodels[i].make_syncronisation((this->internal_time+this->internal_timestep_current*0.5)-this->global_parameters.get_startime());
	}
}
//Make the syncronisation of the temperature models for each internal step
void Hyd_Hydraulic_System::make_syncron_tempmodel(void) {
	//make syncronisation(boundary conditions)
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_temp_model[i].make_syncronisation((this->internal_time + this->internal_timestep_current*0.5) - this->global_parameters.get_startime());
	}

}
//Get the maximum change in a element of a river model as well as the maximum change of the explicitly velocity head
void Hyd_Hydraulic_System::get_max_changes_rivermodel(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep){
	double buff_h=0.0;
	double buff_v=0.0;
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_rvmodels[i].get_max_changes_section(&buff_h, &buff_v, timecheck, timestep);
		if(buff_h>*max_change_h){
			*max_change_h=buff_h;
		}
		if(buff_v>*max_change_v){
			*max_change_v=buff_v;
		}
	}
}
//Calculate the hydrological balance and the maximum values of the river models for each internal step
void Hyd_Hydraulic_System::make_hyd_balance_max_rivermodel(void){
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_rvmodels[i].make_hyd_balance_max(this->next_internal_time-this->global_parameters.get_startime());
	}
}
//Calculate the hydrological balance and the maximum values of the temperature models for each internal step
void Hyd_Hydraulic_System::make_hyd_balance_max_tempmodel(void) {
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_temp_model[i].make_hyd_balance_max(this->next_internal_time - this->global_parameters.get_startime());
	}

}
//Reset the solver of the river models
void Hyd_Hydraulic_System::reset_solver_rv_models(void){
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_rvmodels[i].reset_solver();
	}
}
//make the calculation of the river models
void Hyd_Hydraulic_System::make_calculation_rivermodel(void){
	//solve it
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_rvmodels[i].solve_model(this->next_internal_time-this->global_parameters.get_startime(), this->get_identifier_prefix(false));
	}
}
//Make the calculation of the temperature models for each internal step
void Hyd_Hydraulic_System::make_calculation_tempmodel(void) {
	//solve it
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_temp_model[i].solve_model(this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false));
	}
}
//Make the syncronisation of the floodplain models for each internal step
void Hyd_Hydraulic_System::make_syncron_floodplainmodel(void){
	//make syncronisation(boundary conditions)
	//2DGPU
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_fpmodels[i].make_syncronisation((this->internal_time+this->internal_timestep_current*0.5)-this->global_parameters.get_startime());
	}
}
//Get the maximum change in a element of a floodplain model
void Hyd_Hydraulic_System::get_max_changes_floodplainmodel(double *max_change_h, const bool timecheck){
	double buff_h=0.0;
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_fpmodels[i].get_max_changes_elements(&buff_h, timecheck);
		if(buff_h>*max_change_h){
			*max_change_h=buff_h;
		}
	}
}
//Calculate the hydrological balance and the maximum values of the floodplain models for each internal step
void Hyd_Hydraulic_System::make_hyd_balance_max_floodplainmodel(void){
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->my_fpmodels[i].make_hyd_balance_max(this->next_internal_time-this->global_parameters.get_startime());
	}
}
//Reset the solver of the floodplain models
void Hyd_Hydraulic_System::reset_solver_fp_models(void){
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		this->my_fpmodels[i].reset_solver();
	}
}

//
static void solve_model_gpu_of_fp(Hyd_Model_Floodplain** fps, int numOfFp, const double next_time_point, const string system_id) {
	// We shouldn't need a mutex here as they are independent
	for (int i = 0; i < numOfFp; i++){
		fps[i]->solve_model_gpu(next_time_point, system_id);
	}
}

//make the calculation of the floodplain models
void Hyd_Hydraulic_System::make_calculation_floodplainmodel(void){
	//solve it
	//2DGPU
	unsigned int numCpuFp = 0;
	unsigned int numGpuFp = 0;

	for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if (my_fpmodels[i].Param_FP.get_scheme_info().scheme_type != model::schemeTypes::kDiffusiveCPU && this->global_parameters.get_opencl_available()) {
			numGpuFp++;
		}else {
			numCpuFp++;
		}
	}
	emit statusbar_Multi_hyd_solver_update(numCpuFp, numGpuFp);

	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		if(my_fpmodels[i].Param_FP.get_scheme_info().scheme_type != model::schemeTypes::kDiffusiveCPU && this->global_parameters.get_opencl_available()){
			this->my_fpmodels[i].solve_model_gpu(this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false));
		}else{
			this->my_fpmodels[i].solve_model(this->next_internal_time-this->global_parameters.get_startime(), this->get_identifier_prefix(false));
		}
	}

		/*
	
	int onGPU2 = 0;
	for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
		if (this->my_fpmodels[i].pManager->getSelectedDevice() == 2) {
			onGPU2++;
		}
	}

	if (onGPU2 > 0) {
		Hyd_Model_Floodplain** fpsGpu1 = new Hyd_Model_Floodplain * [this->global_parameters.GlobNofFP - onGPU2];
		Hyd_Model_Floodplain** fpsGpu2 = new Hyd_Model_Floodplain * [onGPU2];
	
		int c1 = 0;
		int c2 = 0;
		for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
			if (this->my_fpmodels[i].pManager->getSelectedDevice() == 1) {
				fpsGpu1[c1] = &this->my_fpmodels[i];
				c1++;
			}
			else {
				fpsGpu2[c2] = &this->my_fpmodels[i];
				c2++;
			}
		}
		m_futures.push_back(std::async(std::launch::async, solve_model_gpu_of_fp, fpsGpu1, c1, this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false)));
		m_futures.push_back(std::async(std::launch::async, solve_model_gpu_of_fp, fpsGpu2, c2, this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false)));

		// Wait for all tasks to finish before continuing
		for (auto& future : m_futures) {
			future.wait();
		}

	}
	else {

		for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {

			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			//if (constant::gpu2d_applied == false) {
			if (1) {
				//cout << "using gpu" << endl;
				this->my_fpmodels[i].solve_model_gpu(this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false));
			}
			else {
				this->my_fpmodels[i].solve_model(this->next_internal_time - this->global_parameters.get_startime(), this->get_identifier_prefix(false));
			}
		}
	}
	
	*/

}
//Output the results of the calculation steps of the river models to file
void Hyd_Hydraulic_System::output_calculation_steps_rivermodel2file(const double timestep){
	if(this->file_output_required==true){
		if (this->global_parameters.get_output_flags().tecplot_1d_required == true || this->global_parameters.get_output_flags().tecplot_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				//to the tecplot file
				if (this->global_parameters.get_output_flags().tecplot_1d_required == true) {
					this->my_rvmodels[i].output_result2tecplot_1d(timestep, this->timestep_counter);
				}
				if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
					this->my_rvmodels[i].output_result2tecplot_2d(timestep, this->timestep_counter);
				}
			}
		}
		if (this->global_parameters.get_output_flags().paraview_1d_required == true || this->global_parameters.get_output_flags().paraview_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				//to the csv / paraview file
				if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
					this->my_rvmodels[i].output_result2csv_1d(timestep, this->timestep_counter);
				}
				if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
					this->my_rvmodels[i].output_result2paraview_2d(timestep, this->timestep_counter);
				}
			}

		}
	}
}
//Output the calculation steps (time, solversteps etc) of the river models to display/console
void Hyd_Hydraulic_System::output_calculation_steps_rivermodel2display(const double timestep){
	for(int i=0; i< this->global_parameters.GlobNofRV;i++){
		//set prefix for output
		ostringstream prefix;
		prefix << this->get_identifier_prefix();
		prefix << "CALC> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//to console/display (for development)
		//this->my_rvmodels[i].output_result_members_per_timestep();

		this->my_rvmodels[i].output_solver_errors(timestep,this->timestep_counter, this->model_time_str, this->real_time_str,this->diff_real_time,this->total_internal_timestep, this->timestep_internal_counter);
		//rewind two times the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Output the calculation steps (time, solversteps etc) of the river models to databse
void Hyd_Hydraulic_System::output_calculation_steps_rivermodel2database(const double timestep, const string time) {
	if (this->database_is_set == false) {
		return;
	}
	//delete results
	if (this->timestep_counter == 0) {
		_Hyd_River_Profile::delete_instat_results_in_table(&this->database, this->system_id, this->hyd_sz.get_id(), this->break_sz);
	}
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		//to database 
		if (global_parameters.get_output_flags().database_instat_required == true) {

			this->my_rvmodels[i].output_result2database_2d(&this->database, this->break_sz, timestep, this->timestep_counter, time);
		}
	}
}
///Output the results of the calculation steps of the temperature models to file
void Hyd_Hydraulic_System::output_calculation_steps_tempmodel2file(const double timestep) {
	if (this->file_output_required == true) {
		if (this->global_parameters.get_output_flags().tecplot_1d_required == true || this->global_parameters.get_output_flags().tecplot_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				//to the tecplot file
				if (this->global_parameters.get_output_flags().tecplot_1d_required == true) {
					//this->my_temp_model[i].output_result2tecplot_1d(timestep, this->timestep_counter);
				}
				if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
					//this->my_temp_model[i].output_result2tecplot_2d(timestep, this->timestep_counter);
				}
			}
		}
		if (this->global_parameters.get_output_flags().paraview_1d_required == true || this->global_parameters.get_output_flags().paraview_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
				//to the csv / paraview file
				if (this->global_parameters.get_output_flags().paraview_1d_required == true) {
					this->my_temp_model[i].output_result2csv_1d(timestep, this->timestep_counter);
				}
				if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
					this->my_temp_model[i].output_result2paraview_2d(timestep, this->timestep_counter);
				}
			}

		}
	}
}
///Output the calculation steps (time, solversteps etc) of the temperature models to display/console
void Hyd_Hydraulic_System::output_calculation_steps_tempmodel2display(const double timestep) {
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		//set prefix for output
		ostringstream prefix;
		prefix << this->get_identifier_prefix();
		prefix << "CALC> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//to console/display (for development)
		//this->my_temp_model[i].output_result_members_per_timestep();

		this->my_temp_model[i].output_solver_errors(timestep, this->timestep_counter, this->model_time_str, this->real_time_str, this->diff_real_time, this->total_internal_timestep, this->timestep_internal_counter);
		//rewind two times the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
///Output the calculation steps (time, solversteps etc) of the temperature models to databse
void Hyd_Hydraulic_System::output_calculation_steps_tempmodel2database(const double timestep, const string time) {
	if (this->database_is_set == false) {
		return;
	}
	//delete results
	if (this->timestep_counter == 0) {
		HydTemp_Profile::delete_instat_results_in_table(&this->database, this->system_id, this->hyd_sz.get_id(), this->break_sz);
	}
	for (int i = 0; i < this->global_parameters.GlobNofRV; i++) {
		//to database 
		if (global_parameters.get_output_flags().database_instat_required == true) {

			this->my_temp_model[i].output_result2database_2d(&this->database, this->break_sz, timestep, this->timestep_counter, time);
		}
	}
}
//Output the results of the calculation steps of the floodplain models to file
void Hyd_Hydraulic_System::output_calculation_steps_floodplainmodel2file(const double timestep) {
	if (this->file_output_required == true) {
		//loop over the floodplain models
		if (this->global_parameters.get_output_flags().tecplot_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
				//to the tecplot file
				this->my_fpmodels[i].output_result2tecplot(timestep, this->timestep_counter);
			}
		}
		if (this->global_parameters.get_output_flags().bluekenue_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
				//to the bluekenue file
				this->my_fpmodels[i].output_result2bluekenue(timestep, this->timestep_counter, this->global_parameters.get_startime());
			}
		}
		if (this->global_parameters.get_output_flags().paraview_2d_required == true) {
			for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
				//to the paraview file 
				this->my_fpmodels[i].output_result2paraview(timestep, this->timestep_counter);
			}
		}
	}
}
//Output the results of the calculation steps of the floodplain models to display/console
void Hyd_Hydraulic_System::output_calculation_steps_floodplainmodel2display(const double timestep){
	//loop over the floodplain models
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
			//set prefix for output
		ostringstream prefix;
		prefix << this->get_identifier_prefix();
		prefix << "CALC> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		//to console/display (for development)
		//this->my_fpmodels[i].output_result_members_per_timestep();

		this->my_fpmodels[i].output_solver_errors(timestep,this->timestep_counter, this->model_time_str, this->real_time_str,this->diff_real_time,this->total_internal_timestep, this->timestep_internal_counter);
		//rewind two times the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Output the results of the calculation steps of the floodplain models to database
void Hyd_Hydraulic_System::output_calculation_steps_floodplainmodel2database(const double timestep, const string time) {
	if (this->database_is_set == false) {
		return;
	}
	//delete results
	if (this->timestep_counter == 0) {
		Hyd_Element_Floodplain::delete_data_in_instat_erg_table(&this->database, this->system_id, this->hyd_sz.get_id(), this->break_sz);
	}

	
	//loop over the floodplain models
	for (int i = 0; i < this->global_parameters.GlobNofFP; i++) {
		//to database 
		if (global_parameters.get_output_flags().database_instat_required == true) {
			
			this->my_fpmodels[i].output_result2database(&this->database, this->break_sz, timestep, this->timestep_counter, time);
		}
		
	}
}
//Clear all not needed data of the models before the solver is initialized
void Hyd_Hydraulic_System::clear_models(void){
	ostringstream cout;
	ostringstream prefix;
	if(this->global_parameters.GlobNofFP>0){
		prefix << "FP"<<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		cout<<"Clear floodplain models..."<< endl;

		Sys_Common_Output::output_hyd->output_txt(&cout);
		//floodplain models
		for(int i=0; i< this->global_parameters.GlobNofFP;i++){
			this->my_fpmodels[i].clear_model();
		}
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Set the starting number of warning(s) from the exception_class (sys)
void Hyd_Hydraulic_System::set_start_warning_number(void){
	this->warning_number_start=Warning::get_number_hyd_warnings();
}
//Set the final number of warning(s) from the exception_class (sys)
void Hyd_Hydraulic_System::set_final_warning_number(void){
	this->warning_number=this->warning_number+(Warning::get_number_hyd_warnings()-this->warning_number_start);
}
//Calculate the approximate memory requirement for the fooldplain models
string Hyd_Hydraulic_System::calculate_approx_memory_fpmodels(void){
    long long int mem=0;
	string mem_str;
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		mem=mem+this->my_fpmodels[i].Param_FP.calculate_approx_workspace();
	}
	mem_str=functions::convert_byte2string(mem);
    if(mem>constant::gbyte_size*8.0){
		ostringstream info;
		Warning msg=this->set_warning(0);
        info << "Estimated workspace  : " << mem_str << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
	return mem_str;
}
//Calculate the total was-wetted area for the floodplain models
double Hyd_Hydraulic_System::calculate_was_wetted_area_fp(double *area_without_coast){
	double area=0.0;
	(*area_without_coast)=0.0;
	for(int i=0; i< this->global_parameters.GlobNofFP;i++){
		area=area+this->my_fpmodels[i].calculate_was_wetted_area(area_without_coast);
	}
	return area;
}
//Wait loop for the output of the calculation to display/console (for multi threading)
void Hyd_Hydraulic_System::waitloop_output_calculation2display(void){
	//no waiting
	if(Hyd_Hydraulic_System::qt_thread_applied==false){
		return;
	}
	//wait loop
	while(this->output_is_allowed==false){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->msleep(3+this->thread_number*2);
		emit output_required(this->thread_number);
	}
	this->output_is_running=true;
}
//Check if OpenCl is available on the system
void Hyd_Hydraulic_System::check_opencl_available() {

	COpenCLSimpleManager simpleManager;

	// If Library not Found
	if (simpleManager.getOpenCLRuntimeAvailable() == false) {
		ostringstream info;
		Warning msg = this->set_warning(2);
		info << "OpenCL Runtime was not found" << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	// If Library not Found
	if (simpleManager.getOpenCLDevicesAvailable() == true) {
		this->global_parameters.set_opencl_available(true);
	}

}
//Check the internal time steps
double Hyd_Hydraulic_System::check_internal_timestep(void){
	double new_timestep=this->internal_timestep_current;

	double change_fp=0.0;
	double time_fp=0.0;

	double change_rv=0.0;
	double time_rv=0.0;

	double change_v_rv=0.0;
	double time_v_rv=0.0;

	//sycronisation of the models; set boundary condition
	this->make_syncron_rivermodel();
	this->make_syncron_floodplainmodel();
	if(this->global_parameters.coastmodel_applied==true){
		this->my_comodel->make_syncronisation((this->internal_time+new_timestep*0.5)-this->global_parameters.get_startime());
	}
	//syncronisation between the models via couplings
	this->coupling_managment.synchronise_couplings((this->internal_time+new_timestep*0.5)-this->global_parameters.get_startime(),new_timestep, true, this->total_internal_timestep, &this->profiler);

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

	//get the maximum change of the river models
	//this->get_max_changes_rivermodel(&change_rv, &change_v_rv, true, this->internal_timestep_current);

	//get the maximum change of the floodplain models
	//this->get_max_changes_floodplainmodel(&change_fp, true);

	//calculate the new internal time step
	//floodplain changes
	if(change_fp>0.0){
		time_fp=this->global_parameters.max_h_change_fp/change_fp;
	}
	else{
		time_fp=this->internal_timestep_base;
	}
	//river changes
	if(change_rv>0.0){
		time_rv=this->global_parameters.max_h_change_rv/change_rv;
	}
	else{
		time_rv=this->internal_timestep_base;
	}

	//changes of the explicit velocity head
	if(change_v_rv>0.0){
		time_v_rv=this->global_parameters.max_v_change_rv/change_v_rv;
	}
	else{
		time_v_rv=this->internal_timestep_base;
	}

	//take the min step
	new_timestep=min(time_fp,time_rv);
	new_timestep=min(time_v_rv,new_timestep);

	//set boundaries to the new timestep
	if(new_timestep<=this->internal_timestep_min){
		new_timestep=this->internal_timestep_min;
	}
	if(new_timestep>=this->internal_timestep_base){
		new_timestep=this->internal_timestep_base;
	}

	int step=this->internal_timestep_base/new_timestep;

	if(step>1){
		new_timestep=this->internal_timestep_base/step;
		//try with reset of fp-models
		if(step>4){
			//this->reset_solver_rv_models();
			//this->reset_solver_fp_models();
		}
	}
	else{
		new_timestep=this->internal_timestep_base;
	}

	return new_timestep;
}
//set the error
Error Hyd_Hydraulic_System::set_error(const int err_type){
	string place="Hyd_Hydraulic_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_..._models(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://the two hydraulic systems are not equal
			place.append("compare_hydraulic_system(Hyd_Hydraulic_System *compare_system)");
			reason="The checked hydraulic system are not equal";
			help="Check the hydraulic system and ensure their equality";
			type=20;
			break;
		case 2://the two hydraulic systems are not equal
			place.append("make_geometrical_interception_co2fp(void)");
			reason="The first point of the coastline polysegment is inside a floodplain polygon";
			help="Check the coastline polysegment; change the point order";
			type=11;
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
//Set the warning
Warning Hyd_Hydraulic_System::set_warning(const int warn_type){
	string place="Hyd_Hydraulic_System::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://approximate worksape requirement > 1gb
			place.append("calculate_approx_memory_fpmodels(void)") ;
            reason="The approximate workspace requirement for the floodplain models is greater than 8 GB; there could be dynamic allocation problems";
			reaction="No reaction";
			help= "Check the existing workspace of your PC";
			type=13;
			break;
		case 1://approximate worksape requirement > 1gb
			place.append("allocate_temp_model(void)");
			reason = "For applying a temperature model, 1d-river models are required";
			reaction = "The applied for the temperature model is set to false";
			help = "Check the model parameter setting in file";
			type = 1;
			break;
		case 2://approximate worksape requirement > 1gb
			place.append("check_opencl_available(void)");
			reason = "An OpenCl Runtime in not installed on this System";
			reaction = "GPU schemes will run on the CPU";
			help = "Install an OpenCL runtime based on your GPU Vendor.";
			type = 11;
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
