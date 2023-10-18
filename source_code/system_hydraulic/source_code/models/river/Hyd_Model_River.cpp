//#include "Hyd_Model_River.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *Hyd_Model_River::general_param_table=NULL;

//Constructor 
Hyd_Model_River::Hyd_Model_River(void){
	this->river_profiles=NULL;
	this->delta_distance_outflow_inflow=0.0;
	this->delta_height_outflow_inflow=0.0;
	this->number_inbetween_profiles=0;

	this->river_midline.set_element_type(_hyd_elem_type::RIVER_ELEM);
	this->river_midline.set_line_type(_hyd_floodplain_polysegment_type::RIVERLINE);
	this->river_midline.set_no_flow_flag(true);
	this->river_leftline.set_line_type(_hyd_floodplain_polysegment_type::RIVERBANKLINE);
	this->river_rightline.set_line_type(_hyd_floodplain_polysegment_type::RIVERBANKLINE);
	this->river_polygon.set_elementtype_inside(_hyd_elem_type::RIVER_ELEM);
	this->river_section_polygon.set_elementtype_inside(_hyd_elem_type::RIVER_ELEM);

	this->total_lateral_boundary_volume.volume_in=0.0;
	this->total_lateral_boundary_volume.volume_out=0.0;
	this->total_lateral_boundary_volume.volume_total=0.0;
	this->total_point_boundary_volume.volume_in=0.0;
	this->total_point_boundary_volume.volume_out=0.0;
	this->total_point_boundary_volume.volume_total=0.0;
	this->total_coupling_left_bank_volume.volume_in=0.0;
	this->total_coupling_left_bank_volume.volume_out=0.0;
	this->total_coupling_left_bank_volume.volume_total=0.0;
	this->total_coupling_right_bank_volume.volume_in=0.0;
	this->total_coupling_right_bank_volume.volume_out=0.0;
	this->total_coupling_right_bank_volume.volume_total=0.0;
	this->total_coupling_1d_volume.volume_in=0.0;
	this->total_coupling_1d_volume.volume_out=0.0;
	this->total_coupling_1d_volume.volume_total=0.0;
	this->total_structure_coupling_volume.volume_in=0.0;
	this->total_structure_coupling_volume.volume_out=0.0;
	this->total_structure_coupling_volume.volume_total=0.0;
	this->total_left_dikebreak_coupling_volume.volume_in=0.0;
	this->total_left_dikebreak_coupling_volume.volume_out=0.0;
	this->total_left_dikebreak_coupling_volume.volume_total=0.0;
	this->total_right_dikebreak_coupling_volume.volume_in=0.0;
	this->total_right_dikebreak_coupling_volume.volume_out=0.0;
	this->total_right_dikebreak_coupling_volume.volume_total=0.0;
	this->total_volume_error_zero_outflow=0.0;
    this->ilu_number=-2;

	this->diversion_channel_flag=false;

	this->number_river_profiles=0;
	this->number_weir_profiles=0;
	this->number_bridge_profiles=0;

	this->list_part_profiles=NULL;
	this->number_part_profiles=0;

	this->min_width = -1.0;
	this->max_width = -1.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Model_River)-sizeof(Hyd_Floodplain_Polysegment)*3-sizeof(Hyd_Floodplain_Polygon)-
		sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Model_River::~Hyd_Model_River(void){
	this->delete_part_profile_list();
	if(this->river_profiles!=NULL){
		delete []this->river_profiles;
		this->river_profiles=NULL;
	}
	this->close_tecplot_output();


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Model_River)-sizeof(Hyd_Floodplain_Polysegment)*3-sizeof(Hyd_Floodplain_Polygon)-
		sizeof(Hyd_Param_RV), _sys_system_modules::HYD_SYS);
}
//_________________
//public
//input the river model per file
void Hyd_Model_River::input_members(const string global_file, const int index, const string global_path){
		

	try{
		//read in global model parameters
		this->Param_RV.parameter_per_parser(global_file, index, global_path);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_" << this->Param_RV.RVNumber <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->Param_RV.check_members();
		//allocate the river profile with number of profiles
		this->allocate_river_profiles();
		//read in the profile members
		cout <<"Read in " << this->Param_RV.RVNofProf<< " profiles of the Rivermodel from file "<< this->Param_RV.RVGeomtryFile << " ..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->input_river_profiles_perfile();

		//read them in
		this->read_instat_boundarycurves();
		


	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Input the river model with the index per file with parser; the general settings as well as the profile data
void Hyd_Model_River::input_members(const Hyd_Param_RV param_rv){
	try{
		//read in global model parameters
		this->Param_RV=param_rv;
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_" << this->Param_RV.RVNumber <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		this->Param_RV.check_members();
		//allocate the river profile with number of profiles
		this->allocate_river_profiles();
		//read in the profile members
		cout <<"Read in " << this->Param_RV.RVNofProf<< " profiles of the Rivermodel from file "<< this->Param_RV.RVGeomtryFile << " ..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		this->input_river_profiles_perfile();

		//read them in
		this->read_instat_boundarycurves();
		


	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Transfer the river model data to a database; the general settings as well as the profile data
void Hyd_Model_River::transfer_input_members2database(QSqlDatabase *ptr_database){

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_" << this->Param_RV.RVNumber <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Model_River::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Model_River::general_param_table->get_table_name()).c_str());
					
	//insert row
	model.insertRows(0, 1);
	//insert data
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id)).c_str())),this->Param_RV.RVNumber);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(label::applied_flag)).c_str())),true);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::atol)).c_str())),this->Param_RV.abs_tolerance);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::rtol)).c_str())),this->Param_RV.rel_tolerance);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::v_explicit)).c_str())),this->Param_RV.user_setting.explict_v_is_used);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::prof_interface)).c_str())),this->Param_RV.user_setting.use_interface);
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::d1output)).c_str())),this->folder_output_db.c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::d2output)).c_str())),this->folder_output_db.c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name)).c_str())),this->Param_RV.RVName.c_str());

	//submit it to the datbase
	Data_Base::database_submit(&model);
	if(model.lastError().isValid()){
		Warning msg=set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Model river number        : " << this->Param_RV.RVNumber << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

	//transfer profile datas
	this->transfer_profile_members2database(ptr_database);

	//transfer instationary boundary curves
	for(int i=0; i< this->Param_RV.number_instat_boundary; i++){
		this->instat_boundary_curves[i].transfer_instat_curve2database_table(ptr_database, _hyd_model_type::RIVER_MODEL, this->Param_RV.get_river_number());
	}
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Transfer a hydraulic boundary szenario from file to a database
void Hyd_Model_River::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database){
/**
This transfer includes just the boundary conditions of the profiles (_Hyd_River_Profile) 
and the instationary boundary curves (Hyd_Instationary_Boundary). The id of the hyraulic boundary
scenario has to be set before.
*/

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_" << this->Param_RV.RVNumber <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"Transfer hydraulic boundary scenario of river model " <<this->Param_RV.RVNumber<<" ..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	try{
		this->inflow_river_profile.transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_RV.get_river_number());
		for(int i=0; i< this->number_inbetween_profiles; i++){
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			this->river_profiles[i].transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_RV.get_river_number());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();	
		}
		this->outflow_river_profile.transfer_hydraulic_boundary_sz2database(ptr_database, this->Param_RV.get_river_number());



		//transfer instationary boundary curves
		for(int i=0; i< this->Param_RV.number_instat_boundary; i++){
			this->instat_boundary_curves[i].transfer_instat_curve2database_table(ptr_database, _hyd_model_type::RIVER_MODEL, this->Param_RV.get_river_number());
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "River model number        : " << this->Param_RV.RVNumber << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Delete all data in the database table for the river model parameters as well as the river profile data (static) 
void Hyd_Model_River::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Model_River::set_table(ptr_database);
	}
	catch(Error msg){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//delete the table
	Hyd_Model_River::general_param_table->delete_data_in_table(ptr_database);

	//delete append table
	_Hyd_River_Profile::delete_data_in_table(ptr_database);
}
//Input the river model with the index from a database selection of the relevant river models; the general settings as well as the profile data
void Hyd_Model_River::input_members(const int index, const QSqlTableModel *query_result,QSqlDatabase *ptr_database, const bool just_profiles, const bool relevant, const bool output_flag){

	//set the river number
	this->Param_RV.RVNumber=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();

	if(output_flag==true){
		ostringstream cout;
		cout <<"Read in river model "<< this->Param_RV.RVNumber<<" from database..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix for output
		ostringstream prefix;
		prefix << "RV_" << this->Param_RV.RVNumber <<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	}

	//read out the results
	//solver tolerances
	this->Param_RV.abs_tolerance=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::atol)).c_str()).toDouble();
	this->Param_RV.rel_tolerance=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::rtol)).c_str()).toDouble();
	this->Param_RV.user_setting.explict_v_is_used=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::v_explicit)).c_str()).toBool();
	this->Param_RV.user_setting.use_interface=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::prof_interface)).c_str()).toBool();
	//name
	this->Param_RV.RVName=query_result->record(index).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
	
	
	//read in the profiles
	try{

		QSqlQueryModel prof_query_result;
		//get the number of profiles
		if(relevant==true){
			this->Param_RV.RVNofProf=_Hyd_River_Profile::select_relevant_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_RV.RVNumber, output_flag);
		}
		else{
			this->Param_RV.RVNofProf=_Hyd_River_Profile::select_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_RV.RVNumber);
		}
		//check the general river parameter
		this->Param_RV.check_members();
		//allocate the river profile with number of profiles
		this->allocate_river_profiles();
		

		//read in the profile members
		this->input_river_profiles_perdatabase(&prof_query_result, ptr_database, output_flag);

		if(just_profiles==false){
			//read in the boundary curves
			this->read_instat_boundary_curves(output_flag, ptr_database);
		}
		
	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;
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
void Hyd_Model_River::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Model_River::general_param_table==NULL){
			ostringstream cout;
			cout << "Create general river model data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_rv_gen;
			const int num_col=12;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=hyd_label::genmod_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::genmod_name;
			tab_col[1].type=sys_label::tab_col_type_string;

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

			tab_col[5].name=hyd_label::atol;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;
			tab_col[5].default_value="5e-7";

			tab_col[6].name=hyd_label::rtol;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;
			tab_col[6].default_value="5e-7";

			tab_col[7].name=hyd_label::prof_interface;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="false";

			tab_col[8].name=hyd_label::v_explicit;
			tab_col[8].type=sys_label::tab_col_type_bool;
			tab_col[8].default_value="false";

			tab_col[9].name=hyd_label::d1output;
			tab_col[9].type=sys_label::tab_col_type_string;

			tab_col[10].name=hyd_label::d2output;
			tab_col[10].type=sys_label::tab_col_type_string;

			tab_col[11].name=label::description;
			tab_col[11].type=sys_label::tab_col_type_string;

			try{
				Hyd_Model_River::general_param_table= new Tables();
				if(Hyd_Model_River::general_param_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Model_River::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Model_River::close_table();;
				throw msg;
			}

		Hyd_Model_River::close_table();
	}

}
//Set the database table for the general parameters of the river model: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Model_River::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Model_River::general_param_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_rv_gen;
		string tab_id_col[12];

		tab_id_col[0]=hyd_label::genmod_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=hyd_label::atol;
		tab_id_col[4]=hyd_label::rtol;
		tab_id_col[5]=hyd_label::d1output;
		tab_id_col[6]=hyd_label::d2output;
		tab_id_col[7]=hyd_label::genmod_name;
		tab_id_col[8]=label::applied_flag;
		tab_id_col[9]=label::description;
		tab_id_col[10]=hyd_label::prof_interface;
		tab_id_col[11]=hyd_label::v_explicit;


		try{
			Hyd_Model_River::general_param_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Model_River::general_param_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Model_River::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Model_River::close_table();
			throw msg;
		}
	}
}
//Close and delete the database table for the general parameters of the river model (static)
void Hyd_Model_River::close_table(void){
	if(Hyd_Model_River::general_param_table!=NULL){
		delete Hyd_Model_River::general_param_table;
		Hyd_Model_River::general_param_table=NULL;
	}
	//close append tables
	_Hyd_River_Profile::close_table();
}
//Select and count the number of relevant river models in a database table (static)
int Hyd_Model_River::select_relevant_model_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output){
	int number=0;
	try{
		Hyd_Model_River::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream prefix;
			prefix << "RV> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

			ostringstream cout;
			cout << "Search for relevant river models in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);

	}

	//give the table name to the query model
	results->setTable((Hyd_Model_River::general_param_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Model_River::general_param_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << Hyd_Model_River::general_param_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << Hyd_Model_River::general_param_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << Hyd_Model_River::general_param_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) " ;
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Model_River::select_relevant_model_database(QSqlTableModel *results,_sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant river model(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}


	return number;
}
//Select and count the number of relevant river models in a database table with a given river number (static)
int Hyd_Model_River::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no){
	try{
		Hyd_Model_River::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int number=0;

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Hyd_Model_River::general_param_table->get_table_name();
	test_filter << " where " ;
	test_filter << Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id) << " = " << rv_no;
	
	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Hyd_Model_River::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results->rowCount();


	return number;
}
//Check the number of relevant river models in a database table (static)
int Hyd_Model_River::check_relevant_model_database(QSqlDatabase *ptr_database){
	QSqlQueryModel results;
	try{
		Hyd_Model_River::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int number=0;

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Hyd_Model_River::general_param_table->get_table_name();
		
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Warning msg;
		msg.set_msg("Hyd_Model_River::select_relevant_model_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int rv_no)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}
	
	number= results.rowCount();


	return number;
}
//Clone the members of the river model
void Hyd_Model_River::clone_model(Hyd_Model_River *river){
	ostringstream cout;
	this->system_id=river->get_sys_system_id();
	this->hyd_sz=river->hyd_sz;
	this->Param_RV=river->Param_RV;
	_Hyd_Model::clone_model(this);

	this->river_leftline.clone_polysegment(&river->river_leftline);
	this->river_midline.clone_polysegment(&river->river_midline);
	this->river_rightline.clone_polysegment(&river->river_rightline);
	this->river_polygon.clone_polygon(&river->river_polygon);

	//allocate the river profile with number of profiles
	this->allocate_river_profiles();

	this->diversion_channel_flag=river->diversion_channel_flag;

	this->number_river_profiles=river->number_river_profiles;
	this->number_weir_profiles=river->number_weir_profiles;
	this->number_bridge_profiles=river->number_bridge_profiles;
	this->delta_distance_outflow_inflow=river->delta_distance_outflow_inflow;
	this->delta_height_outflow_inflow=river->delta_height_outflow_inflow;


	cout <<"Clone river profiles..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->inflow_river_profile.clone_profile(&river->inflow_river_profile);
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].clone_profile(&river->river_profiles[i]);	
	}
	this->outflow_river_profile.clone_profile(&river->outflow_river_profile);
	this->outflow_river_profile.set_ptr_riverprofiles(this->river_profiles, this->number_inbetween_profiles, &this->inflow_river_profile);

	cout <<"Clone river model boundary curves..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	this->allocate_instat_boundarycurves();
	for(int i=0; i<this->Param_RV.number_instat_boundary; i++){
		this->instat_boundary_curves[i].clone_curve(&river->instat_boundary_curves[i]);
	}
	this->connect_profiles2instat_boundarycurves();
	
	cout <<"Cloning of the river model is finished"<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

}
//initialize the river profiles
void Hyd_Model_River::init_river_model(Hyd_Param_Material *material_table){

	try{
		ostringstream cout;
		//number of section between the profiles
		this->NEQ = this->Param_RV.RVNofProf-1;
        this->NEQ_real=this->NEQ;

		this->init_river_model_geometry();
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		//init the first profile
		this->inflow_river_profile.init_profile(material_table, this->Param_RV.user_setting);
		//this->inflow_river_profile.typ_of_profile->output_tables();
		//init the profiles
		for(int i=0; i< this->Param_RV.RVNofProf-2; i++){

			if(i%50==0 && i>0){
				cout << "Initialise profiles "<< i+1 <<" to " << i+50 <<"..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}

			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
			
			this->river_profiles[i].init_profile(material_table, this->Param_RV.user_setting);
			//this->river_profiles[i].typ_of_profile->output_tables();
		}
		//init the last profile
		this->outflow_river_profile.init_profile(material_table, this->Param_RV.user_setting);

		//calcuculate width
		this->calculate_river_width();

		if(this->Param_RV.number_instat_boundary>0){
			//connet to instationary boundary curves
			this->connect_profiles2instat_boundarycurves();
		}
		//check boundary
		this->check_instationary_boundary_found();

		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();

		//count the profile types
		this->count_profile_types();

		//set the pointer of the inbetween profiles and the inflow profile to the outflow profile
		this->outflow_river_profile.set_ptr_riverprofiles(this->river_profiles, this->number_inbetween_profiles, &this->inflow_river_profile);

	}
	catch(Error msg){

		ostringstream info;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;		
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Initialize the river geometry
void Hyd_Model_River::init_river_model_geometry(void){
		//calculate the distance
		this->calculate_distance_of_profiles();
		this->calculate_deltaz_in2out();


		//make the geometrical boundary of the river river polygon and the midline of the river
		this->init_river_geometrie();
}
//check the river model
void Hyd_Model_River::check_river_model(const bool output){
	try{
		ostringstream cout;
		if(output==true){
			cout<<"Check distances between river profile of river " << this->Param_RV.RVName <<"..."<<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
		}
		//inflow profile
		this->inflow_river_profile.check_profiles();

		//inbetween profiles
		for(int i=0; i<this->number_inbetween_profiles;i++){
			//check each profile
			if(i==0){
				this->river_profiles[i].check_profiles(&(this->inflow_river_profile),&(this->river_profiles[i+1]));
			}
			else if(i==this->number_inbetween_profiles-1){
				this->river_profiles[i].check_profiles(&(this->river_profiles[i-1]),&(this->outflow_river_profile));
			}
			else{
				this->river_profiles[i].check_profiles(&(this->river_profiles[i-1]),&(this->river_profiles[i+1]));
			}
	

		}

		//outflow profile
		if(this->number_inbetween_profiles==0){
			this->outflow_river_profile.check_profiles(&(this->inflow_river_profile),NULL);
		}
		else if(this->number_inbetween_profiles==1){
			this->outflow_river_profile.check_profiles(&(this->river_profiles[this->number_inbetween_profiles-1]),&(this->inflow_river_profile));
		}
		else{
			this->outflow_river_profile.check_profiles(&(this->river_profiles[this->number_inbetween_profiles-1]),&(this->river_profiles[this->number_inbetween_profiles-2]));
		}

		//check the distances if they do correspond to the geometrial distances of the given river
		//inflow
		//The distance can be smaller than the geometrical distances; if it is to less (tolerances times 0.5) than give a warning
		if(this->inflow_river_profile.get_distance2downstream()<this->river_minline.my_segment[0].get_distance()*0.5){
			Warning msg=this->set_warning(1);
			ostringstream info;
			info << "Riversection                      : " << 0 << endl;
			info << "Riverstation                      : " << this->inflow_river_profile.get_river_station() <<label::m << endl;
			info << "Given distance                    : " << this->inflow_river_profile.get_distance2downstream() <<label::m << endl;
			info << "Geometrical distance riverminline : " << this->river_minline.my_segment[0].get_distance() << label::m << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);

		}
		//the distance can be greater; if it is to much (tolerances times 1.5) than give a warning
		if(this->inflow_river_profile.get_distance2downstream()>this->river_minline.my_segment[0].get_distance()*2.0){
			Warning msg=this->set_warning(0);
			ostringstream info;
			info << "Riversection                      : " << 0 << endl;
			info << "Riverstation                      : " << this->inflow_river_profile.get_river_station() <<label::m << endl;
			info << "Given distance                    : " << this->inflow_river_profile.get_distance2downstream() <<label::m << endl;
			info << "Geometrical distance riverminline : " << this->river_minline.my_segment[0].get_distance() << label::m << endl;
			info << "Rivernumber                       : " << this->Param_RV.RVNumber <<endl;
			info << "Rivername                         : " << this->Param_RV.RVName << endl;
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
		//inbetween profiles
		for(int i=0; i< this->number_inbetween_profiles;i++){
			//The distance can not be smaller than the geometrical distances (here make tolerance 50%); give an error 
			if(this->river_profiles[i].get_distance2downstream()<this->river_minline.my_segment[i+1].get_distance()*0.5){
				Warning msg=this->set_warning(1);
				ostringstream info;
				info << "Riversection                      : " << i+1 << endl;
				info << "Riverstation                      : " << this->river_profiles[i].get_river_station() <<label::m << endl;
				info << "Given distance                    : " << this->river_profiles[i].get_distance2downstream() << label::m <<endl;
				info << "Geometrical distance riverminline : " << this->river_minline.my_segment[i+1].get_distance() << label::m << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
			//the distance can be greater; if it is to much (tolerances times 2, than give a warning
			if(this->river_profiles[i].get_distance2downstream()>this->river_minline.my_segment[i+1].get_distance()*2.0){
				Warning msg=this->set_warning(0);
				ostringstream info;
				info << "Riversection                      : " << i+1 << endl;
				info << "Riverstation                      : " << this->river_profiles[i].get_river_station() <<label::m << endl;
				info << "Given distance                    : " << this->river_profiles[i].get_distance2downstream() << label::m <<endl;
				info << "Geometrical distance riverminline : " << this->river_minline.my_segment[i+1].get_distance() << label::m << endl;
				info << "Rivernumber                       : " << this->Param_RV.RVNumber <<endl;
				info << "Rivername                         : " << this->Param_RV.RVName << endl;
				msg.make_second_info(info.str());
				msg.output_msg(2);
			}
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Rivernumber          : " << this->Param_RV.RVNumber <<endl;
		info << "Rivername            : " << this->Param_RV.RVName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	ostringstream cout;
	if(output==true){
		cout<<"Check river section polygons of river " << this->Param_RV.RVName <<"..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	//check the river section polygons
	this->set_river_section_first_profile();
	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->set_river_section_polygon(i);
	}
	this->set_river_section_polygon(this->number_inbetween_profiles);
	
}
//Compare the equality of two river models in terms of number of profiles and the river station of each profile
void Hyd_Model_River::compare_models(Hyd_Model_River *compare_model){
/**This comparison is primarily important for adding new hydraulic boundary scenarios. It ensures, that 
the new boundary conditions are set properly to the river model and his profiles. 
*/

	if(this->number_inbetween_profiles!=compare_model->number_inbetween_profiles){
		Error msg=this->set_error(18);
		ostringstream info;
		info << "Number river profiles in model " << this->Param_RV.RVNumber << ": "<<this->number_inbetween_profiles+2<< endl;
		info << "Number river profiles in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->number_inbetween_profiles+2<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//compare the river station
	if(abs(this->inflow_river_profile.get_river_station()-compare_model->inflow_river_profile.get_river_station())>constant::meter_epsilon){
		Error msg=this->set_error(18);
		ostringstream info;
		info << "Inflow riverstation in model " << FORMAT_FIXED_REAL << P(5) << this->Param_RV.RVNumber << ": "<<this->inflow_river_profile.get_river_station()<< label::m<< endl;
		info << "Inflow riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->inflow_river_profile.get_river_station()<<label::m<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i<this->number_inbetween_profiles; i++){
		if(abs(this->river_profiles[i].get_river_station()-compare_model->river_profiles[i].get_river_station())>constant::meter_epsilon){
			Error msg=this->set_error(18);
			ostringstream info;
			info << "Profile " << i+1<< " riverstation in model " << FORMAT_FIXED_REAL << P(5)<< this->Param_RV.RVNumber << ": "<<this->river_profiles[i].get_river_station()<< label::m<< endl;
			info << "Profile " << i+1<< " riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->river_profiles[i].get_river_station()<<label::m<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	if(abs(this->outflow_river_profile.get_river_station()-compare_model->outflow_river_profile.get_river_station())>constant::meter_epsilon){
		Error msg=this->set_error(18);
		ostringstream info;
		info << "Outflow riverstation in model " << FORMAT_FIXED_REAL << P(5) << this->Param_RV.RVNumber << ": "<<this->outflow_river_profile.get_river_station()<< label::m<< endl;
		info << "Outflow riverstation in model " << compare_model->Param_RV.RVNumber << ": "<<compare_model->outflow_river_profile.get_river_station()<<label::m<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Set a new hydraulic boundary scenario id to the river models
void Hyd_Model_River::set_new_hyd_bound_sz_id(Hyd_Boundary_Szenario new_sz){
	this->hyd_sz=new_sz;
	//set the boundary szenario data to the profiles
	this->inflow_river_profile.hyd_sz.set_members(&new_sz);
	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->river_profiles[i].hyd_sz.set_members(&new_sz);

	}
	this->outflow_river_profile.hyd_sz.set_members(&new_sz);
	//set the boundary szenario data to the instationary boundary curves
	for(int i=0; i< this->Param_RV.number_instat_boundary; i++){
		this->instat_boundary_curves[i].hyd_sz.set_members(&new_sz);
	}
}
//Get the diversion channel flag
bool Hyd_Model_River::get_diversion_channel_flag(void){
	return this->diversion_channel_flag;
}
//Set the diversion channel flag
void Hyd_Model_River::set_diversion_channel_flag(const bool flag){
	this->diversion_channel_flag=flag;
}
//Get the minimum river width
double Hyd_Model_River::get_min_width(void) {
	return this->min_width;
}
//Get the maximum river width
double Hyd_Model_River::get_max_width(void) {
	return this->max_width;
}
//Set minimum river width
void Hyd_Model_River::set_min_width(const double width) {
	this->min_width = width;

}
//Set maximum river width
void Hyd_Model_River::set_max_width(const double width) {
	this->max_width = width;
}
//Get a polygon for the river section (profile to profile i+1) a polygon with the given index
Hyd_Floodplain_Polygon* Hyd_Model_River::get_river_section_polygon(const int section_index){
	this->set_river_section_polygon(section_index);
	return &(this->river_section_polygon);
}
//Get one segment of the river midline for one section for the given section index
Geo_Segment* Hyd_Model_River::get_river_midline_section(const int section_index){
	return this->river_midline.get_segment(section_index);
}
//Get the first segment of the river midline
Geo_Segment* Hyd_Model_River::get_river_midline_first_section(void){
	return this->river_midline.get_first_segment();
}
//Get the last segment of the river midline
Geo_Segment* Hyd_Model_River::get_river_midline_last_section(void){
	return this->river_midline.get_last_segment();
}
//output the members
void Hyd_Model_River::output_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.RVNumber<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout<<"Output the general parameters..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//output the parameter of the river model
	this->Param_RV.output_members();

	//output
	cout<<"Output the river profiles ("<<this->Param_RV.RVNofProf<<")..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	
	//first profile
	this->inflow_river_profile.output_members();
	//output the river profile
	for(int i=0; i<this->Param_RV.RVNofProf-2; i++){
		this->river_profiles[i].output_members();
	}
	//last profile
	this->outflow_river_profile.output_members();


	if(this->Param_RV.number_instat_boundary>0){
		cout<<"Output the instationary boundary curves ("<<this->Param_RV.number_instat_boundary<<")..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);

		for(int i=0; i< this->Param_RV.number_instat_boundary; i++){
			this->instat_boundary_curves[i].output_member();
		}
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//output the setted and calculated members
void Hyd_Model_River::output_setted_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.RVNumber<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	
	ostringstream cout;
	cout << "GENERAL"<<endl;
	cout << " River lenght (distance between in- and outflow): " << this->delta_distance_outflow_inflow << label::m << endl;
	cout << " Height difference between inflow and outflow profile: " << this->delta_height_outflow_inflow << label::m << endl;
	cout << " Maximum width of the river: " << this->max_width << label::m << endl;
	cout << " Minimum width of the river: " << this->min_width << label::m << endl;
	cout << "PROFILETYPES" << endl;
	cout << " No. of river profiles      : " << W(15)  <<this->number_river_profiles << endl;
	cout << " No. of weir profiles       : " << W(15)  <<this->number_weir_profiles << endl;
	cout << " No. of bridge profile      : " << W(15)  <<this->number_bridge_profiles << endl;

	Sys_Common_Output::output_hyd->output_txt(&cout);

	//the river polygon
	cout << "Output the Riverpolygon..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->river_polygon.output_members();
	//the river midline
	cout << "Output the Rivermidline..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->river_midline.output_members();

	//first profile
	this->inflow_river_profile.output_setted_members();
	//output the river profile
	for(int i=0; i<this->number_inbetween_profiles; i++){
		this->river_profiles[i].output_setted_members();
	}
	//last profile
	this->outflow_river_profile.output_setted_members();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the geometrie to tecplot as 2d
void Hyd_Model_River::output_geometrie2tecplot_2d(void){
	//for 2d-output
	if(this->Param_RV.tecplot_outfile_name_2d!=label::not_set){
		//open the file
		string buffer = this->Param_RV.get_filename_geometrie2file_2d(hyd_label::tecplot);
		//Add file type
		buffer += hyd_label::dat;
		this->tecplot_output_2d_geo.open(buffer.c_str());
		//check if it is open
		if(this->tecplot_output_2d_geo.is_open()==false){
			Error msg=this->set_error(13);
			ostringstream info;
			info << "File name " << buffer << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else{
			//output the file header
			this->tecplot_output_2d_geo << "TITLE = "<< "\" " << this->Param_RV.RVName << " RVNo. " << this->Param_RV.RVNumber << " \"" << endl;
			this->tecplot_output_2d_geo << "VARIABLES = " <<endl;
			this->tecplot_output_2d_geo << "\" x " << label::m << "\" "<<endl;
			this->tecplot_output_2d_geo << "\" y " << label::m << "\" "<< endl;
			this->tecplot_output_2d_geo << "\" z " << label::m << "\" " << endl;
			this->tecplot_output_2d_geo << "\" type " << label::no_unit << "\" "<< endl;
			this->tecplot_output_2d_geo << "\" init-h " << label::m << "\" "<< endl;
			this->tecplot_output_2d_geo <<endl;
			this->tecplot_output_2d_geo << "#Type expression: 0 RIVER_TYPE; 1 WEIR_TYPE; 2 BRIDGE_TYPE" << endl << endl;

			//output zone header
			this->tecplot_output_2d_geo << "ZONE T= " << " \" Geometry \" " <<endl;
			this->tecplot_output_2d_geo << "I="<< 2 <<", J="<< this->Param_RV.RVNofProf<<", K=1" << endl;
			this->tecplot_output_2d_geo << "ZONETYPE=ORDERED" << endl;
			this->tecplot_output_2d_geo << "DATAPACKING=BLOCK" << endl;
			this->tecplot_output_2d_geo << "VARLOCATION=([1-2]=NODAL, [3-5]=CELLCENTERED)" << endl;
			this->tecplot_output_2d_geo << "DT=(SINGLE SINGLE SINGLE SHORTINT SINGLE)" << endl;
			this->tecplot_output_2d_geo << endl;
			int counter_endl=0;

			//x-coordinate
			this->tecplot_output_2d_geo <<"#x "<< label::m << " NODAL No. " << this->Param_RV.RVNofProf*2 <<endl;
			//inflow profile
			this->tecplot_output_2d_geo << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			this->tecplot_output_2d_geo << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d_geo << endl;
				}
				this->tecplot_output_2d_geo << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
				counter_endl++;
				this->tecplot_output_2d_geo << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			//outflow profile
			this->tecplot_output_2d_geo << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			this->tecplot_output_2d_geo << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			this->tecplot_output_2d_geo << endl << endl;

			//y-coordinate
			this->tecplot_output_2d_geo <<"#y "<< label::m << " NODAL No. " << this->Param_RV.RVNofProf*2 <<endl;
			counter_endl=0;
			//inflow profile
			this->tecplot_output_2d_geo << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			this->tecplot_output_2d_geo << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d_geo << endl;
				}
				this->tecplot_output_2d_geo << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
				counter_endl++;
				this->tecplot_output_2d_geo << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			//outflow profile
			this->tecplot_output_2d_geo << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			this->tecplot_output_2d_geo << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			this->tecplot_output_2d_geo << endl << endl;

			//z
			counter_endl=0;
			this->tecplot_output_2d_geo <<"#z "<< label::m << " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
			counter_endl=0;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d_geo << endl;
				}
				this->tecplot_output_2d_geo << this->river_profiles[i].typ_of_profile->get_global_z_min() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			//outflow profile
			this->tecplot_output_2d_geo << this->outflow_river_profile.typ_of_profile->get_global_z_min() << "  ";
			this->tecplot_output_2d_geo << endl << endl;

			//type
			counter_endl=0;
			this->tecplot_output_2d_geo <<"#type "<< label::no_unit << " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
			counter_endl=0;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d_geo << endl;
				}
				this->tecplot_output_2d_geo << this->river_profiles[i].get_profile_type() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			//outflow profile
			this->tecplot_output_2d_geo << this->outflow_river_profile.get_profile_type() << "  ";
			this->tecplot_output_2d_geo << endl << endl;

			//init-h
			counter_endl=0;
			this->tecplot_output_2d_geo <<"#init-h "<< label::m << " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
			counter_endl=0;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d_geo << endl;
				}
				this->tecplot_output_2d_geo << this->river_profiles[i].get_init_condition_value() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d_geo << endl;
			}
			//outflow profile
			this->tecplot_output_2d_geo << this->outflow_river_profile.get_init_condition_value()<< "  ";
			this->tecplot_output_2d_geo << endl << endl;

			//close the file
			this->tecplot_output_2d_geo.close();


		}
	}

}
///Output the geometrie to paraview as 2d
void Hyd_Model_River::output_geometrie2paraview_2d(void) {

	//get the file name
	string filename = this->Param_RV.get_filename_geometrie2file_2d(hyd_label::paraview);
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
	this->tecplot_output << "Geometry of river " << this->Param_RV.RVNumber << " with river name " << this->Param_RV.RVName << " (Data of polygon from downstream profile)"<< endl;
	//this->tecplot_output << "# Element-Type expression: 0 STANDARD_ELEM; 1 COAST_ELEM; 2 NOFLOW_ELEM; 3 NOINFO_ELEM; 4 RIVER_ELEM; 5 OTHER_FP_ELEM; 6 DIKELINE_ELEM" << endl << endl;
	this->tecplot_output << "ASCII" << endl;
	this->tecplot_output << "DATASET POLYDATA" << endl;
	this->tecplot_output << "POINTS " << (this->number_inbetween_profiles +2)*2<< " double" << endl;

	//output points
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output  << "  0 "<<endl;

	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;


	for (int i = 0; i < this->number_inbetween_profiles; i++) {
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

	}
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;
	this->tecplot_output << endl;

	this->tecplot_output << "POLYGONS " << this->number_inbetween_profiles+1 << " "<< (this->number_inbetween_profiles + 1)*5 << endl;
	for (int i = 0; i < (this->number_inbetween_profiles + 1); i++) {
		this->tecplot_output << "4 " ;
		if (i == 0) {
			this->tecplot_output << " 0 2 3 1" << endl;

		}
		else {
			int index = i * 2;
			this->tecplot_output << " "<< index <<" "<< index+2 << " "<< index+3 << " "<< index+1<< endl;
		}


	}

	//output celldata
	this->tecplot_output << "CELL_DATA " << this->number_inbetween_profiles + 1 << endl;
	string buff_unit;

	buff_unit = " z_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_global_z_min() << "  ";
	}

	//outflow profile
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_global_z_min() << "  ";
	this->tecplot_output << endl ;

	buff_unit = " type_"; 
	buff_unit += label::no_unit;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].get_profile_type() << "  ";
	}

	//outflow profile
	this->tecplot_output << this->outflow_river_profile.get_profile_type() << "  ";
	this->tecplot_output << endl;

	buff_unit = " init_h_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].get_init_condition_value() << "  ";
	}

	//outflow profile
	this->tecplot_output << this->outflow_river_profile.get_init_condition_value() << "  ";
	this->tecplot_output << endl;

	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();
}
//Output the result members for each riverprofile at every timestep
void Hyd_Model_River::output_result_members_per_timestep(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.RVNumber<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	_Hyd_River_Profile::output_result_members_per_timestep_header();
	//output intflow profile
	this->inflow_river_profile.output_result_members_per_timestep();
	//inbetween
	for(int i=0; i<this->number_inbetween_profiles; i++){
		this->river_profiles[i].output_result_members_per_timestep();
	}
	//outflow
	this->outflow_river_profile.output_result_members_per_timestep();
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the result members per timestep to tecplot as 1d
void Hyd_Model_River::output_result2tecplot_1d(const double timepoint, const int timestep_number){

	if(this->tecplot_output.is_open()==false){
		this->init_tecplot_output();
	}

	if (this->tecplot_output.is_open() == true) {
		//output the zone header
		this->tecplot_output << endl << "ZONE T= " << "\" " << timepoint << " \" " << " I = " << this->Param_RV.RVNofProf << endl;
		this->tecplot_output << "SOLUTIONTIME=" << timepoint << endl;
		this->tecplot_output << "STRANDID=" << timestep_number + 1 << endl;

		//output intflow profile
		this->inflow_river_profile.output_result_members_per_timestep(&this->tecplot_output);
		//inbetween
		for (int i = 0; i < this->number_inbetween_profiles; i++) {
			this->river_profiles[i].output_result_members_per_timestep(&this->tecplot_output);
		}
		//outflow
		this->outflow_river_profile.output_result_members_per_timestep(&this->tecplot_output);
		this->tecplot_output.flush();
	}
}
//Output the result members per timestep to tecplot as 2d
void Hyd_Model_River::output_result2tecplot_2d(const double timepoint, const int timestep_number){

	if(this->tecplot_output_2d.is_open()==false){
		this->init_tecplot_output();
	}
	
	if(this->tecplot_output_2d.is_open()==true){
		//output zone header
		this->tecplot_output_2d << "ZONE T= " << " \" 2d-Results Stepno. " <<timestep_number <<"; Time " << timepoint << label::sec<< "\" " <<endl;
		this->tecplot_output_2d << "I="<< 2 <<", J="<< this->Param_RV.RVNofProf<<", K=1" << endl;
		this->tecplot_output_2d << "ZONETYPE=ORDERED" << endl;
		this->tecplot_output_2d << "DATAPACKING=BLOCK" << endl;
		this->tecplot_output_2d << "SOLUTIONTIME=" <<timepoint << endl;
		this->tecplot_output_2d << "STRANDID=" << timestep_number+1 << endl;
		if(timestep_number!=0){
			this->tecplot_output_2d << "VARSHARELIST=([1-2]=1)"<<endl;
		}
		this->tecplot_output_2d << "VARLOCATION=([1-2]=NODAL, [3-5]=CELLCENTERED)" << endl;
		this->tecplot_output_2d << "DT=(SINGLE SINGLE SINGLE SINGLE SINGLE)" << endl;
		this->tecplot_output_2d << endl;
		int counter_endl=0;

		if(timestep_number==0){
			//x-coordinate
			this->tecplot_output_2d <<"#x "<< label::m << " NODAL No. " << this->Param_RV.RVNofProf*2 <<endl;
			//inflow profile
			this->tecplot_output_2d << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			this->tecplot_output_2d << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d << endl;
				}
				this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
				counter_endl++;
				this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			//outflow profile
			this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			counter_endl++;
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
			this->tecplot_output_2d << endl << endl;

			//y-coordinate
			this->tecplot_output_2d <<"#y "<< label::m << " NODAL No. " << this->Param_RV.RVNofProf*2 <<endl;
			counter_endl=0;
			//inflow profile
			this->tecplot_output_2d << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			this->tecplot_output_2d << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(counter_endl==5){
					counter_endl=0;
					this->tecplot_output_2d << endl;
				}
				this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
				counter_endl++;
				this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
				counter_endl++;
			}
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			//outflow profile
			this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			counter_endl++;
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
			this->tecplot_output_2d << endl << endl;
		}
		
		//h
		counter_endl=0;
		this->tecplot_output_2d <<"#h "<< label::m << " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
		counter_endl=0;
		for(int i=0; i< this->number_inbetween_profiles; i++){
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_actual_local_waterlevel_h() << "  ";
			counter_endl++;
		}
		if(counter_endl==5){
			counter_endl=0;
			this->tecplot_output_2d << endl;
		}
		//outflow profile
		this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_actual_local_waterlevel_h() << "  ";
		this->tecplot_output_2d << endl << endl;

		//s
		counter_endl=0;
		this->tecplot_output_2d <<"#s "<< label::m << " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
		counter_endl=0;
		for(int i=0; i< this->number_inbetween_profiles; i++){
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->river_profiles[i].get_actual_global_waterlevel() << "  ";
			counter_endl++;
		}
		if(counter_endl==5){
			counter_endl=0;
			this->tecplot_output_2d << endl;
		}
		//outflow profile
		this->tecplot_output_2d << this->outflow_river_profile.get_actual_global_waterlevel() << "  ";
		this->tecplot_output_2d << endl << endl;

		//v
		counter_endl=0;
		this->tecplot_output_2d <<"#v "<< label::m_per_sec<< " CELLCENTERED No. " << this->Param_RV.RVNofProf-1 << endl; 
		counter_endl=0;
		for(int i=0; i< this->number_inbetween_profiles; i++){
			if(counter_endl==5){
				counter_endl=0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_actual_flow_velocity() << "  ";
			counter_endl++;
		}
		if(counter_endl==5){
			counter_endl=0;
			this->tecplot_output_2d << endl;
		}
		//outflow profile
		this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_actual_flow_velocity() << "  ";
		this->tecplot_output_2d << endl << endl;


		//width
		counter_endl = 0;
		this->tecplot_output_2d << "#width " << label::m << " CELLCENTERED No. " << this->Param_RV.RVNofProf - 1 << endl;
		counter_endl = 0;
		for (int i = 0; i < this->number_inbetween_profiles; i++) {
			if (counter_endl == 5) {
				counter_endl = 0;
				this->tecplot_output_2d << endl;
			}
			this->tecplot_output_2d << this->river_profiles[i].typ_of_profile->get_width() << "  ";
			counter_endl++;
		}
		if (counter_endl == 5) {
			counter_endl = 0;
			this->tecplot_output_2d << endl;
		}
		//outflow profile
		this->tecplot_output_2d << this->outflow_river_profile.typ_of_profile->get_width() << "  ";
		this->tecplot_output_2d << endl << endl;

	}
}
///Output the result members per timestep to csv as 1d
void Hyd_Model_River::output_result2csv_1d(const double timepoint, const int timestep_number) {

	// for 1d - output
	//results per timesteps
	ofstream output_csv;
	output_csv.clear();
	if (this->Param_RV.tecplot_outfile_name_1d != label::not_set) {
		//open the file
		string buffer = this->Param_RV.get_filename_result2file_1d(hyd_label::paraview);
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
			info << "File name " << this->Param_RV.get_filename_result2file_1d(hyd_label::paraview) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		else {
			//output the file header
			output_csv << " Riverstation " << label::m << "," << " z_min " << label::m << ",";
			output_csv << " z_left_bank " << label::m << "," << " z_right_bank " << label::m << ",";
			output_csv << " s (global) " << label::m << ",";
			output_csv <<" h (local) " << label::m << ",";
			output_csv <<" v " << label::m_per_sec << ",";
			output_csv <<" fr " << label::no_unit << ",";
			output_csv << " width " << label::m << ",";
			output_csv <<" q " << label::qm_per_sec;
			output_csv <<  endl;
		}
		
	}

	//output per profile
	if (output_csv.is_open() == true) {

		//output intflow profile
		this->inflow_river_profile.output_result_members_per_timestep2csv(&output_csv);
		//inbetween
		for (int i = 0; i < this->number_inbetween_profiles; i++) {
			this->river_profiles[i].output_result_members_per_timestep2csv(&output_csv);
		}
		//outflow
		this->outflow_river_profile.output_result_members_per_timestep2csv(&output_csv);
		output_csv.flush();
		output_csv.close();
	}
}
//Output the result members per timestep to paraview as 2d
void Hyd_Model_River::output_result2paraview_2d(const double timepoint, const int timestep_number) {

	//get the file name
	string filename = this->Param_RV.get_filename_result2file_2d(hyd_label::paraview);
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
	output << "Values of river " << this->Param_RV.RVNumber << " with river name " << this->Param_RV.RVName << " time "<< timepoint <<" (Data of polygon from downstream profile)" << endl;
	output << "ASCII" << endl;
	output << "DATASET POLYDATA" << endl;
	output << "POINTS " << (this->number_inbetween_profiles + 2) * 2 << " double" << endl;

	//output points
	output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	output << "  0 " << endl;

	output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	output << "  0 " << endl;


	for (int i = 0; i < this->number_inbetween_profiles; i++) {
		output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		output << "  0 " << endl;

		output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		output << "  0 " << endl;

	}
	output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	output << "  0 " << endl;
	output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	output << "  0 " << endl;
	output << endl;

	output << "POLYGONS " << this->number_inbetween_profiles + 1 << " " << (this->number_inbetween_profiles + 1) * 5 << endl;
	for (int i = 0; i < (this->number_inbetween_profiles + 1); i++) {
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
	output << "CELL_DATA " << this->number_inbetween_profiles + 1 << endl;
	string buff_unit;

	buff_unit = " h_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		output << this->river_profiles[i].typ_of_profile->get_h()<< "  ";
	}
	//outflow profile
	output << this->outflow_river_profile.typ_of_profile->get_h() << "  ";
	output << endl;

	buff_unit = " s_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		output << this->river_profiles[i].typ_of_profile->get_s() << "  ";
	}
	//outflow profile
	output << this->outflow_river_profile.typ_of_profile->get_s() << "  ";
	output << endl;

	buff_unit = " vtot_";
	buff_unit += label::m_per_sec;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		output << this->river_profiles[i].typ_of_profile->get_vtot() << "  ";
	}
	//outflow profile
	output << this->outflow_river_profile.typ_of_profile->get_vtot() << "  ";
	output << endl;

	buff_unit = " Q_";
	buff_unit += label::qm_per_sec;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		output << this->river_profiles[i].get_Q() << "  ";
	}
	//outflow profile
	output << this->outflow_river_profile.get_Q() << "  ";
	output << endl;

	buff_unit = " width_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	output << "SCALARS" << "  " << buff_unit << " double" << endl;
	output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		output << this->river_profiles[i].typ_of_profile->get_width() << "  ";
	}
	//outflow profile
	output << this->outflow_river_profile.typ_of_profile->get_width() << "  ";
	output << endl;

	output.clear();
	output.close();

}
//Output the result members per timestep to databse as 2d
void Hyd_Model_River::output_result2database_2d(QSqlDatabase *ptr_database, const string break_sz, const double timepoint, const int timestep_number, const string time) {


	//the table is set (the name and the column names) and allocated
	try {
		_Hyd_River_Profile::set_erg_instat_table(ptr_database);
	}
	catch (Error msg) {
		throw msg;
	}
	int glob_id = _Hyd_River_Profile::erg_instat_table->maximum_int_of_column(_Hyd_River_Profile::erg_instat_table->get_column_name(label::glob_id), ptr_database) + 1;

	//polygon string
	string poly_buffer;
	this->set_river_section_first_profile();
	poly_buffer = this->river_section_polygon.get_polygon2sql_string();
	this->inflow_river_profile.output_instat_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz, time);

	for (int i = 0; i < this->number_inbetween_profiles; i++) {
		this->set_river_section_polygon(i);
		poly_buffer = river_section_polygon.get_polygon2sql_string();
		this->river_profiles[i].output_instat_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz, time);
	}
	this->set_river_section_polygon(this->number_inbetween_profiles);
	poly_buffer = river_section_polygon.get_polygon2sql_string();
	this->outflow_river_profile.output_instat_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz, time);

}
//output solver errors
void Hyd_Model_River::output_solver_errors(const double time_point, const int step_counter, const string timestring, const string realtime, const double diff_time, const int total_internal, const int internal_steps){

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_"<<this->Param_RV.RVNumber<<"> ";
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
//output final statistics of the river model
void Hyd_Model_River::output_final(void){	

	//set prefix for output
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.RVNumber<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	//solver
	this->output_solver_statistics();
	//hydrological balance
	this->calculate_total_hydrological_balance();
	this->output_hydrological_balance();
	//output the maximum values to console/display
	this->output_maximum_results();
	

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the maximum result members to tecplot
void Hyd_Model_River::output_result_max2tecplot(void){
	//output the maximum values
	this->output_maximum_results2tecplot();
}
///Output the maximum result members to csv
void Hyd_Model_River::output_result_max2csv(void) {
	//maximum results
	if (this->Param_RV.tecplot_outfile_name_1d != label::not_set) {
		//close the result file per time step
		this->tecplot_output.close();
		this->tecplot_output.clear();

		//reopen the file
		string buffer = this->Param_RV.get_filename_result2file_1d_maxvalues(hyd_label::paraview);
		//Add file type
		buffer += hyd_label::csv;
		this->tecplot_output.open(buffer.c_str());
		//check if it is open
		if (this->tecplot_output.is_open() == false) {
			Error msg = this->set_error(11);
			ostringstream info;
			info << "File name " << this->Param_RV.get_filename_result2file_1d_maxvalues(hyd_label::paraview) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//output the file header
		this->tecplot_output << " Riverstation " << label::m;
		this->tecplot_output << " ,z_min " << label::m;
		this->tecplot_output << " ,z_left_bank " << label::m;
		this->tecplot_output << " ,z_right_bank " << label::m;
		this->tecplot_output << " ,s_max " << label::m;
		this->tecplot_output << " ,t(s_max) " << label::sec;
		this->tecplot_output << " ,h_max " << label::m;
		this->tecplot_output << " ,t(h_max) " << label::sec;
		this->tecplot_output << " ,v_max " << label::m_per_sec;
		this->tecplot_output << " ,t(v_max) " << label::sec;
		this->tecplot_output << " ,was_dry " << label::no_unit;
		this->tecplot_output << " ,t(dry) " << label::sec;
		this->tecplot_output << " ,t(wet) " << label::sec;
		this->tecplot_output << ",width_max" << label::m;
		this->tecplot_output << ",t(width_max)" << label::sec;
		this->tecplot_output << ",CVol_1D_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_1D_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_Struc_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_Struc_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_OV_Le_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_OV_Le_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_OV_Ri_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_OV_Ri_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_DB_Le_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_DB_Le_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_DB_Ri_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_DB_Ri_out" << label::cubicmeter;
		this->tecplot_output << ",CVol_tot_in" << label::cubicmeter;
		this->tecplot_output << ",CVol_tot_out" << label::cubicmeter;
		this->tecplot_output << ",Qmax" << label::qm_per_sec;
		this->tecplot_output << ",t(Qmax)" << label::sec;

		this->tecplot_output << endl;

		//inflow profile
		this->inflow_river_profile.output_max_results2csvfile(&this->tecplot_output);

		//inbetween profiles
		for (int i = 0; i < this->number_inbetween_profiles; i++) {
			this->river_profiles[i].output_max_results2csvfile(&this->tecplot_output);

		}
		//outflow profile
		this->outflow_river_profile.output_max_results2csvfile(&this->tecplot_output);

		//close it
		this->tecplot_output.close();

	}

}
//Output the maximum result members to paraview as 2d
void Hyd_Model_River::output_result_max2paraview2d(void) {
	//get the file name
	string filename = this->Param_RV.get_filename_result2file_2d_maxvalues(hyd_label::paraview);
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
	this->tecplot_output << "Maximum values of river " << this->Param_RV.RVNumber << " with river name " << this->Param_RV.RVName << " (Data of polygon from downstream profile)" << endl;
	this->tecplot_output << "ASCII" << endl;
	this->tecplot_output << "DATASET POLYDATA" << endl;
	this->tecplot_output << "POINTS " << (this->number_inbetween_profiles + 2) * 2 << " double" << endl;

	//output points
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;

	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;


	for (int i = 0; i < this->number_inbetween_profiles; i++) {
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
		this->tecplot_output << "  0 " << endl;

	}
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_xcoordinate() << "  ";
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates().get_ycoordinate() << "  ";
	this->tecplot_output << "  0 " << endl;
	this->tecplot_output << endl;

	this->tecplot_output << "POLYGONS " << this->number_inbetween_profiles + 1 << " " << (this->number_inbetween_profiles + 1) * 5 << endl;
	for (int i = 0; i < (this->number_inbetween_profiles + 1); i++) {
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
	this->tecplot_output << FORMAT_FIXED_REAL << P(2) << "CELL_DATA " << this->number_inbetween_profiles + 1 << endl;
	string buff_unit;

	buff_unit = " h_max_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_max_h().maximum<< "  ";
	}
	//outflow profile
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_max_h().maximum << "  ";
	this->tecplot_output << endl;

	buff_unit = " s_max_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_max_s().maximum << "  ";
	}
	//outflow profile
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_max_s().maximum << "  ";
	this->tecplot_output << endl;

	buff_unit = " v_max_";
	buff_unit += label::m_per_sec;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_max_vtot().maximum << "  ";
	}
	//outflow profile
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_max_vtot().maximum << "  ";
	this->tecplot_output << endl;

	buff_unit = " width_max_";
	buff_unit += label::m;
	buff_unit = functions::clean_white_space(&buff_unit);
	this->tecplot_output << "SCALARS" << "  " << buff_unit << " double" << endl;
	this->tecplot_output << "LOOKUP_TABLE default" << endl;
	for (int i = 0; i < this->number_inbetween_profiles; i++) {

		this->tecplot_output << this->river_profiles[i].typ_of_profile->get_max_width().maximum << "  ";
	}
	//outflow profile
	this->tecplot_output << this->outflow_river_profile.typ_of_profile->get_max_width().maximum << "  ";
	this->tecplot_output << endl;

	
	

	//close the file
	this->tecplot_output.close();
	this->tecplot_output.clear();



}
//Output the maximum result members to a database table
void Hyd_Model_River::output_result_max2database(QSqlDatabase *ptr_database, const string break_sz){

	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_River_Profile::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=_Hyd_River_Profile::erg_table->maximum_int_of_column(_Hyd_River_Profile::erg_table->get_column_name(label::glob_id), ptr_database)+1;

	//polygon string
	string poly_buffer;
	this->set_river_section_first_profile();
	poly_buffer=this->river_section_polygon.get_polygon2sql_string();
	this->inflow_river_profile.output_max_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz);

	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->set_river_section_polygon(i);
		poly_buffer=river_section_polygon.get_polygon2sql_string();
		this->river_profiles[i].output_max_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz);
	}
	this->set_river_section_polygon(this->number_inbetween_profiles);
	poly_buffer=river_section_polygon.get_polygon2sql_string();
	this->outflow_river_profile.output_max_results(ptr_database, this->Param_RV.RVNumber, poly_buffer, &glob_id, break_sz);
}
//Intercept the model with a coast models (Hyd_Coast_Model)
bool Hyd_Model_River::intercept_coast_model(Hyd_Floodplain_Polygon *coastpolygon){
	bool coupling_flag=false;
	ostringstream cout;
	cout <<"Intercept RV-model "<< this->Param_RV.RVNumber<< " with CO-model " <<"..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//make interception
	if(coastpolygon->check_point_inside(&(this->outflow_river_profile.typ_of_profile->mid_point_global_x_y))==true ||
		coastpolygon->check_point_atboundary(&(this->outflow_river_profile.typ_of_profile->mid_point_global_x_y))==true){
			cout <<"The outflow of the rivermodel is in the coast-model " << endl;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			try{
				this->check_coast_modelby_interception(coastpolygon);
			}
			catch(Error msg){
				throw msg;
			}
			return coupling_flag=true;
	}
	return coupling_flag;
}
//Intercept the model with an other river model (Hyd_Model_River)
bool Hyd_Model_River::intercept_river_model(Hyd_Model_River *outflow_river){
	bool coupling_flag=false;
	ostringstream cout;
	cout <<"Intercept RV-model "<< this->Param_RV.RVNumber<< " (inflow) with RV-model " <<outflow_river->Param_RV.get_river_number() <<" (outflow)..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//make interception
	if(this->river_polygon.check_point_inside(&(outflow_river->outflow_river_profile.typ_of_profile->mid_point_global_x_y))==true ||
		this->river_polygon.check_point_atboundary(&(outflow_river->outflow_river_profile.typ_of_profile->mid_point_global_x_y))){
		cout <<"The outflow of the rivermodel " <<outflow_river->Param_RV.get_river_number()<<" is in the river-model "<<this->Param_RV.RVNumber << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		return coupling_flag=true;
	}
	return coupling_flag;
}
//Check the river models one another by interception
void Hyd_Model_River::check_river_modelsby_interception(Hyd_Model_River *check_river){
	Geo_Interception_Point_List list;
	int start_this=0;
	int start_checkriver=0;
	if(this->diversion_channel_flag==true){
		start_this=1;
	}
	if(check_river->get_diversion_channel_flag()==true){
		start_checkriver=1;
	}
	try{
		//check just the inbetween segments and points and the inbetween river polygons (not start and end)
		for(int j=start_this; j<this->number_inbetween_profiles;j++){
			this->set_river_section_polygon(j);
			for(int i=start_checkriver; i<check_river->river_midline.get_number_segments()-1;i++){
				//check for interception
				list.delete_list();
				this->river_section_polygon.calculate_segment_interception(&list, &(check_river->river_midline.my_segment[i]));
				if(list.get_number_points()>=1){
					Error msg=this->set_error(16);
					ostringstream info;
					info <<"Midline riversection                  : " << i << endl;
					info <<"Polygon riversection                  : " << j << endl;
					msg.make_second_info(info.str());
					throw msg;

				}
			}
		}
	}
	catch(Error msg){
		ostringstream info;
		info <<"Midline of river model                : " << check_river->Param_RV.get_river_number()<<endl;
		info <<"Polygon of river model                : " << this->Param_RV.get_river_number()<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Find the river section index (polygon) were a given point is inside; -1 the point is in no river section
int Hyd_Model_River::find_river_section_index_inside(Geo_Point *point){
	int river_section_index=-1;	
	for(int i=0; i<this->Param_RV.get_number_profiles()-1; i++){
		this->set_river_section_polygon(i);
		if(this->river_section_polygon.check_point_inside(point)==true){
			river_section_index=i;
			break;
		}
	}
	return river_section_index;
}
//Find the river section index (polygon) were a given point is inside; -1 the point is in no river section
int Hyd_Model_River::find_river_section_index_inside(const bool begin_flag, Geo_Segment *segment){
	int river_section_index=-1;
	Geo_Point *point;
	if(begin_flag==false){
		point=&(segment->point2);
	}
	else{
		point=&(segment->point1);
	}
	for(int i=0; i<this->Param_RV.get_number_profiles()-1; i++){
		this->set_river_section_polygon(i);
		if(this->river_section_polygon.check_point_inside(point)==true){
			river_section_index=i;
			return river_section_index;
		}
	}
	//point is at the boundary or not inside 
	Geo_Interception_Point_List list;
	for(int i=0; i<this->Param_RV.get_number_profiles()-1; i++){
		this->set_river_section_polygon(i);
		if(this->river_section_polygon.check_point_atboundary(point)==true){
			river_section_index=i;
			this->river_section_polygon.calculate_segment_interception(&list,segment);
			//with two interception points it is the good one
			if(list.get_number_points()==2){
				return river_section_index;
			}
			else{
				list.delete_list();
			}
		}
	}
	return river_section_index;
}
//Reset all coupling discharges of the model; use it before syncronization
void Hyd_Model_River::reset_coupling_discharge(void){

	this->inflow_river_profile.reset_coupling_discharge();

	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].reset_coupling_discharge();
	}
	this->outflow_river_profile.reset_coupling_discharge();
}
//Reset the model to the state before the calculation
void Hyd_Model_River::reset_model(Hyd_Param_Global *global_params){
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Reset river model and reinit solver..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	this->reset_coupling_discharge();
	
	this->reset_total_hydrological_balance();
	//inflow profile
	this->inflow_river_profile.reset_hydrobalance_maxvalues();
	//inbetween profiles
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].reset_hydrobalance_maxvalues();
	}
	//outflow profile
	this->outflow_river_profile.reset_hydrobalance_maxvalues();

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
void Hyd_Model_River::clear_boundary_condition(const bool with_output){
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if(with_output==true){
		cout << "Clear river model boundary condition..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	this->delete_instationary_boundary_curve();
	this->Param_RV.number_instat_boundary=0;

	//inflow profile
	this->inflow_river_profile.reset_boundary_data();

	//inbetween profiles
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].reset_boundary_data();
	}
	//outflow profile
	this->outflow_river_profile.reset_boundary_data();

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
void Hyd_Model_River::set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "RV_"<< this->Param_RV.get_river_number()<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{	
		//instationary boundary
		this->read_instat_boundary_curves(output_flag, ptr_database);

		QSqlTableModel prof_query_result(0, *ptr_database);
		//get the number of profiles
		int number=_Hyd_River_Profile::select_relevant_profiles_in_database(&prof_query_result, ptr_database, this->system_id, this->Param_RV.RVNumber, output_flag);

		if(this->Param_RV.RVNofProf!=number){
			Error msg=this->set_error(19);
					ostringstream info;
			info << "Table name               : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
			info << "Number found profiles    : " << number << endl;
			info << "Number required profiles : " << this->Param_RV.RVNofProf<< endl; 
			msg.make_second_info(info.str());
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
			throw msg;
		}



		//boundary data
		int glob_id=-1;
		//read out the results of the request for this index
		glob_id=prof_query_result.record(0).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();

		this->inflow_river_profile.hyd_sz.set_members(&this->hyd_sz);
		this->inflow_river_profile.input_boundary_data_per_database(ptr_database, glob_id);
		for(int i=0; i< this->number_inbetween_profiles; i++){
			glob_id=prof_query_result.record(i+1).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
			this->river_profiles[i].hyd_sz.set_members(&this->hyd_sz);
			this->river_profiles[i].input_boundary_data_per_database(ptr_database, glob_id);
			Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		}
		glob_id=prof_query_result.record(this->number_inbetween_profiles+1).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_glob_id)).c_str()).toInt();
		this->outflow_river_profile.hyd_sz.set_members(&this->hyd_sz);
		this->outflow_river_profile.input_boundary_data_per_database(ptr_database, glob_id);
        if(this->outflow_river_profile.get_h_outflow_is_given()==true){
           this->outflow_river_profile.reset_h_outflow_is_given();
           this->outflow_river_profile.set_h_outflow_is_given();
        }


		//connect it 
		this->connect_profiles2instat_boundarycurves();
		//check boundary
		this->check_instationary_boundary_found();
	}
	catch(Error msg){

		ostringstream info;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;		
		msg.make_second_info(info.str());
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//make the syncronisation between the models and the boundaries
void Hyd_Model_River::make_syncronisation(const double time_point){
	
	//for the inflow profile
	if(this->number_inbetween_profiles>0){
		this->inflow_river_profile.make_syncronisation(time_point, &this->river_profiles[0]);
	}
	else{
		this->inflow_river_profile.make_syncronisation(time_point, &this->outflow_river_profile);
	}

	//for outflow
	this->outflow_river_profile.make_syncronisation(time_point, NULL);

	//for the inbetween profiles
	for(int i=0; i< this->number_inbetween_profiles; i++){
		if(i<this->number_inbetween_profiles-1){
			this->river_profiles[i].make_syncronisation(time_point, &this->river_profiles[i+1]);
		}
		else{
			this->river_profiles[i].make_syncronisation(time_point, &this->outflow_river_profile);
		}
	}

	//transfer boundary/coupling discharges for special types (e.g. weir type, waterlevel is given for the inflow profile)
	if(this->number_inbetween_profiles>0){
		this->inflow_river_profile.transfer_coupling_boundary2profile(&this->river_profiles[0]);
	}
	//the last inbetweenprofile can not be a weir
	for(int i=0; i< this->number_inbetween_profiles-1;i++){
		this->river_profiles[i].transfer_coupling_boundary2profile(&this->river_profiles[i+1]);
	}
}
//Get the maximum change in a river section
void Hyd_Model_River::get_max_changes_section(double *max_change_h, double *max_change_v, const bool timecheck, const double timestep){
	double buff_h=0.0;
	double buff_v=0.0;

	//for the inflow profile
	this->inflow_river_profile.get_max_changes_section(&buff_h, &buff_v, timecheck);
	if(buff_h>*max_change_h){
		*max_change_h=buff_h;
	}
	if(buff_v>*max_change_v){
		*max_change_v=buff_v;
	}

	//for outflow
	this->outflow_river_profile.get_max_changes_section(&buff_h, &buff_v, timecheck, timestep);
	if(buff_h>*max_change_h){
		*max_change_h=buff_h;
	}
	if(buff_v>*max_change_v){
		*max_change_v=buff_v;
	}

	//for the inbetween profiles
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].get_max_changes_section(&buff_h, &buff_v, timecheck);
		if(buff_h>*max_change_h){
			*max_change_h=buff_h;
		}
		if(buff_v>*max_change_v){
			*max_change_v=buff_v;
		}
	}
}
//Calculate the hydrological balance and the maximum values of the model
void Hyd_Model_River::make_hyd_balance_max(const double time_point){

	//make for all the hydrological balance
	this->inflow_river_profile.calculate_hydrological_balance(time_point);
	for(int i=0; i< this->number_inbetween_profiles;i++){
		this->river_profiles[i].calculate_hydrological_balance(time_point);
	}
	this->outflow_river_profile.calculate_hydrological_balance(time_point);



}
//Reset the solver of the model
void Hyd_Model_River::reset_solver(void){
	if(this->inflow_river_profile.typ_of_profile!=NULL){
		this->set_solver_statistics();
		CVodeReInit(this->cvode_mem,this->inflow_river_profile.typ_of_profile->get_old_time_point(), this->results);
		this->no_reset++;
	}
}
//solve_model
void Hyd_Model_River::solve_model(const double next_time_point, const string system_id){
	try{
		//run the solver
		this->run_solver(next_time_point, system_id);

		for(int i=0; i < this->number_inbetween_profiles; i++){
		
			this->river_profiles[i].typ_of_profile->set_actuel_profilevalues_by_table((this->results_real[i+1]));
		}
		this->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_table(this->results_real[0]);

//std::cout << "abs " << this->setted_abs_tol<< " rel " << this->setted_rel_tol<<endl;



	}
	catch(Error msg){
		ostringstream info;
		info << "Time point  :" << next_time_point << endl;
		info << "Rivernumber :" << this->Param_RV.RVNumber << endl;
		string buffer = this->Param_RV.get_filename_result2file_1d(hyd_label::paraview);
		//Add time and file type
		buffer += "_";
		int time = (int)next_time_point;
		buffer += std::to_string(time);
		buffer += hyd_label::csv;
		info << "Exception output 1d-RV results are written in " << buffer << endl;
		info << "Please check it for unplausible results; check the 1d-RV model " <<  endl;
		this->output_result2csv_1d(next_time_point, -999);
		msg.make_second_info(info.str());
		throw msg;
	}	
}
//Close the output files for a tecplot output
void Hyd_Model_River::close_tecplot_output(void){

	if(this->tecplot_output_2d.is_open()==true){
		this->tecplot_output_2d.close();
	}
	if(this->tecplot_output_2d_geo.is_open()==true){
		this->tecplot_output_2d_geo.close();
	}
}
//Set the folder for the file-output in case of an database based calculation
void Hyd_Model_River::set_output_folder(const string folder){
	this->Param_RV.tecplot_outfile_name_1d=folder;
	this->Param_RV.tecplot_outfile_name_2d=folder;
	this->folder_output_db=folder;
}
//Find the pointer to the river profiles upstream and downstream to a given riverstation
bool Hyd_Model_River::find_ptr_upstream_downstream_prof(const double riverstation, _Hyd_River_Profile **up, _Hyd_River_Profile **down){
	*up=NULL;
	*down=NULL;
	//the given riverstation is not in the river model
	if(riverstation < this->outflow_river_profile.get_river_station()){
		*up=&this->outflow_river_profile;
		return false;
	}
	if(riverstation >this->inflow_river_profile.get_river_station()){
		*down=&this->inflow_river_profile;
		return false;
	}
	if(this->number_inbetween_profiles==0){
		*down=&this->outflow_river_profile;
		*up=&this->inflow_river_profile;
	}
	else{
		if(abs(riverstation-this->inflow_river_profile.get_river_station())<constant::meter_epsilon){
			*up=&this->inflow_river_profile;
			*down=&this->inflow_river_profile;
		}
		else if(abs(riverstation-this->outflow_river_profile.get_river_station())<constant::meter_epsilon){
			*up=&this->outflow_river_profile;
			*down=&this->outflow_river_profile;
		}
		else{
			*up=&this->inflow_river_profile;
			for(int i=0; i< this->number_inbetween_profiles; i++){
				if(riverstation<this->river_profiles[i].get_river_station()){
					*up=&this->river_profiles[i];
				}
				else if(abs(riverstation-this->river_profiles[i].get_river_station())<constant::meter_epsilon){
					*up=&this->river_profiles[i];
					*down=&this->river_profiles[i];
					break;
				}
				else{
					if(i==this->number_inbetween_profiles-1){
						*down=&this->outflow_river_profile;
						break;
					}
					else{
						*down=&this->river_profiles[i];
						break;
					}
				}
			}
		}
	}
	return true;
}
//Get a polyline of the profile, which are represented by an fpl-section
void Hyd_Model_River::get_polyline_fpl_section(const bool left_flag, Geo_Polysegment *polyline, _Hyd_River_Profile *prof_start, _Hyd_River_Profile *prof_end){

	//count number of profiles
	this->set_part_profile_list(prof_start, prof_end);
	polyline->set_number_of_points(this->number_part_profiles);

	Geo_Point *buffer=NULL;
	buffer=new Geo_Point[this->number_part_profiles];

	for(int i=0; i<this->number_part_profiles; i++){
		if(left_flag==false){
			buffer[i]=(this->list_part_profiles[i])->typ_of_profile->get_last_point()->get_global_x_y_coordinates();
		}
		else{
			buffer[i]=(this->list_part_profiles[i])->typ_of_profile->get_first_point()->get_global_x_y_coordinates();
		}

	}
	polyline->set_points(buffer);
	if(buffer!=NULL){
		delete []buffer;
	}
}
//Idealise the river profile(s), which are represented by an fpl-section, with the data of the fpl-section
void Hyd_Model_River::idealise_profile_by_fpl(const bool left_flag, _Hyd_River_Profile *prof_start, _Hyd_River_Profile *prof_end, _hyd_idealisation_parameters ideal_params, _hyd_breach_start_parameters *start_params, const int section_id){


	//determine mid-point as breach starting point
	Geo_Polysegment buffer;
	//here also the list of the involved profiles are set
	this->get_polyline_fpl_section(left_flag, &buffer, prof_start, prof_end);
	//set the start parameters
	start_params->id_related_station=prof_start->get_profile_number();
	start_params->distance2related=buffer.get_total_distance()/2.0;

	for(int i=0; i< this->number_part_profiles; i++){
		if(i==0){
			this->list_part_profiles[i]->idealise_profile_fpl(left_flag, ideal_params, _hyd_connection_types::INFLOW_CONN, section_id);
		}
		else if(i==this->number_part_profiles-1){
			this->list_part_profiles[i]->idealise_profile_fpl(left_flag, ideal_params, _hyd_connection_types::OUTFLOW_CONN, section_id);
		}
		else{
			this->list_part_profiles[i]->idealise_profile_fpl(left_flag, ideal_params, _hyd_connection_types::STANDARD_CONN, section_id);
		}
	}
}
//Get the number of involved profiles to a fpl-section
int Hyd_Model_River::get_number_involved_profiles(void){
	return this->number_part_profiles;
}
//Get a pointer to the involved profiles by a given index
_Hyd_River_Profile* Hyd_Model_River::get_ptr_involved_profile(const int index){
	if(index<0 || index>=this->number_part_profiles){
		return NULL;
	}
	else{
		return this->list_part_profiles[index];
	}
}
//Get the pointer to the profile by a given index
_Hyd_River_Profile* Hyd_Model_River::get_ptr_river_profile(const int index){
	if(index<0 || index >=this->Param_RV.RVNofProf){
		return NULL;
	}
	else{
		if(index==0){
			return &this->inflow_river_profile;
		}
		else if(index==this->Param_RV.RVNofProf-1){
			return &this->outflow_river_profile;
		}
		else{
			return &this->river_profiles[index-1];
		}

	}
}
//Get the pointer to the profile by a given number
_Hyd_River_Profile* Hyd_Model_River::get_ptr_river_profile_number(const int number){
	
	if(this->inflow_river_profile.get_profile_number()==number){
		return &this->inflow_river_profile;
	}
	if(this->outflow_river_profile.get_profile_number()==number){
		return &this->outflow_river_profile;
	}

	for(int i=0; i< this->number_inbetween_profiles; i++){
		if(this->river_profiles[i].get_profile_number()==number){
			return &this->river_profiles[i];
		}
	}

	return NULL;
}
//Set the total river bank to no overflow
void Hyd_Model_River::set_total_river2no_overflow(void){
	this->inflow_river_profile.set_left_right_bank2no_overflow();
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].set_left_right_bank2no_overflow();
	}

}
//Find the index of a river profile by a given point
int Hyd_Model_River::find_river_profile_id_by_point(Geo_Point *point){
	int id_buff=-1;


	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->set_river_section_polygon(i);
		if(this->river_section_polygon.check_point_inside_atboundary(point)==true){
			id_buff=i+1;
			return id_buff;
		}
	}

	this->set_river_section_polygon(this->number_inbetween_profiles);
	if(this->river_section_polygon.check_point_inside_atboundary(point)==true){
		id_buff=this->number_inbetween_profiles+1;
		return id_buff;
	}

	return id_buff;
}
//Transfer the river profile data to a database, delete the boundary condition and copy the existing boundary condition 
void Hyd_Model_River::transfer_profile_members2database(QSqlDatabase *ptr_database, Hyd_Model_River *source, const _sys_system_id base){

	//compare old and new river
	//number of profiles
	if(this->number_inbetween_profiles!=source->number_inbetween_profiles){
		ostringstream info;
		Error msg=this->set_error(23);
		info<< "Number existing profiles  : " << source->number_inbetween_profiles+2<< endl;
		info<< "Number replacing profiles : " << this->number_inbetween_profiles+2<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//check riverstation
	if(abs(this->inflow_river_profile.get_river_station()-source->inflow_river_profile.get_river_station())>constant::meter_epsilon){
		ostringstream info;
		Error msg=this->set_error(24);
		info<< "Riverstation of existing inflow profiles   : " << source->inflow_river_profile.get_river_station() <<label::m<< endl;
		info<< "Riverstation of replacing inflow profiles  : " << this->inflow_river_profile.get_river_station() <<label::m<< endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	for(int i=0; i< this->number_inbetween_profiles; i++){
		if(abs(this->river_profiles[i].get_river_station()-source->river_profiles[i].get_river_station())>constant::meter_epsilon){
			ostringstream info;
			Error msg=this->set_error(24);
			info<< "Riverstation of existing river profiles   : " << source->river_profiles[i].get_river_station() <<label::m<< endl;
			info<< "Riverstation of replacing river profiles  : " << this->river_profiles[i].get_river_station() <<label::m<< endl;
			info<< "Name of replacing river profiles          : " << this->river_profiles[i].get_profile_name()<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	if(abs(this->outflow_river_profile.get_river_station()-source->outflow_river_profile.get_river_station())>constant::meter_epsilon){
		ostringstream info;
		Error msg=this->set_error(25);
		info<< "Riverstation of existing outflow profiles   : " << source->outflow_river_profile.get_river_station() <<label::m<< endl;
		info<< "Riverstation of replacing outflow profiles  : " << this->outflow_river_profile.get_river_station() <<label::m<< endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	//append measure to profile name
	ostringstream buffer;
	buffer<<"_M"<<this->system_id.measure_nr;
	this->inflow_river_profile.append_str2name(buffer.str());
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->river_profiles[i].append_str2name(buffer.str());
	}
	this->outflow_river_profile.append_str2name(buffer.str());

	//transfer new one
	this->clear_boundary_condition(false);
	this->transfer_profile_members2database(ptr_database, false);

	//copy old bound condition to new one
	int glob_id_old=0;
	int glob_id_new=0;

	//copy inflow
	glob_id_old=source->inflow_river_profile.get_prof_glob_id();
	glob_id_new=this->inflow_river_profile.get_prof_glob_id();
	_Hyd_River_Profile::copy_boundary_condition(ptr_database, base, glob_id_old, glob_id_new);

	//copy inbetween
	for(int i=0; i< this->number_inbetween_profiles; i++){
		glob_id_old=source->river_profiles[i].get_prof_glob_id();
		glob_id_new=this->river_profiles[i].get_prof_glob_id();
		_Hyd_River_Profile::copy_boundary_condition(ptr_database, base, glob_id_old, glob_id_new);
	}

	//copy outflow
	glob_id_old=source->outflow_river_profile.get_prof_glob_id();
	glob_id_new=this->outflow_river_profile.get_prof_glob_id();
	_Hyd_River_Profile::copy_boundary_condition(ptr_database, base, glob_id_old, glob_id_new);

}
//_____________________________
//private
//allocate the river profile with number of profiles
void Hyd_Model_River::allocate_river_profiles(void){

	//number of section between the profiles
	this->NEQ = this->Param_RV.RVNofProf-1;
    this->NEQ_real=this->NEQ;
	this->number_inbetween_profiles=this->Param_RV.RVNofProf-2;

	//1-dimensional=> half bandwidth=1
	this->half_bandwidth=1;

	try{
		this->river_profiles=new Hyd_River_Profile_Connection_Standard[this->number_inbetween_profiles];
	}
	catch(bad_alloc &t){
		ostringstream info;
		Error msg=this->set_error(0);
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	this->set_upstream_downstream_profiles();

	//init: set the boundary szenario data/system infos to the profiles
	this->inflow_river_profile.hyd_sz.set_members(&this->hyd_sz);
	this->inflow_river_profile.set_systemid(this->system_id);
	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->river_profiles[i].hyd_sz.set_members(&this->hyd_sz);
		this->river_profiles[i].set_systemid(this->system_id);
	}
	this->outflow_river_profile.hyd_sz.set_members(&this->hyd_sz);
	this->outflow_river_profile.set_systemid(this->system_id);
}
//Set a polygon for the river section (profile to profile i+1) a polygon with the given index
void Hyd_Model_River::set_river_section_polygon(const int section_index){
	this->river_section_polygon.set_number_points(4);
	ostringstream river_station;
	ostringstream prof_name;
	_Hyd_River_Profile_Type *profile_up=NULL;
	_Hyd_River_Profile_Type *profile_down=NULL;

	Geo_Point *river_section_points=NULL;

	
	//just 1 section
	if(section_index==0 && this->number_inbetween_profiles==0){
		profile_up=this->inflow_river_profile.typ_of_profile;
		profile_down=this->outflow_river_profile.typ_of_profile;
		river_station << this->inflow_river_profile.get_river_station();
		prof_name << this->inflow_river_profile.get_profile_name();
	}
	//first river section
	else if(section_index==0 && this->number_inbetween_profiles!=0){
		profile_up=this->inflow_river_profile.typ_of_profile;
		profile_down=this->river_profiles[0].typ_of_profile;
		river_station << this->inflow_river_profile.get_river_station();
		prof_name << this->inflow_river_profile.get_profile_name();
	}
	//last river section
	else if(section_index==this->number_inbetween_profiles){
		profile_up=this->river_profiles[this->number_inbetween_profiles-1].typ_of_profile;
		profile_down=this->outflow_river_profile.typ_of_profile;
		river_station << this->river_profiles[this->number_inbetween_profiles-1].get_river_station();
		prof_name << this->river_profiles[this->number_inbetween_profiles-1].get_profile_name();
	}
	else{
		profile_up=this->river_profiles[section_index-1].typ_of_profile;
		profile_down=this->river_profiles[section_index].typ_of_profile;
		river_station << this->river_profiles[section_index-1].get_river_station();
		prof_name << this->river_profiles[section_index-1].get_profile_name();
	}

	


	//allocate the points
	try{
		river_section_points=new Geo_Point[4];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(22);
		ostringstream info;
		info<< "info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{

		//set the points of the two profiles
		river_section_points[0].set_point_name(hyd_label::river_section_point);
		river_section_points[0]=profile_up->get_first_point()->get_global_x_y_coordinates();
		river_section_points[1].set_point_name(hyd_label::river_section_point);
		river_section_points[1]=profile_up->get_last_point()->get_global_x_y_coordinates();

		river_section_points[2].set_point_name(hyd_label::river_section_point);
		river_section_points[2]=profile_down->get_last_point()->get_global_x_y_coordinates();
		river_section_points[3].set_point_name(hyd_label::river_section_point);
		river_section_points[3]=profile_down->get_first_point()->get_global_x_y_coordinates();

		//points to polygon
		this->river_section_polygon.set_number_points(4);
		this->river_section_polygon.set_points(river_section_points,false);

		////test to set all profile points! DANIEL
		//int all_p = 0;
		//all_p = profile_up->get_number_points() + profile_down->get_number_points();
		////allocate the points
		//try {
		//	river_section_points = new Geo_Point[all_p];
		//}
		//catch (bad_alloc &t) {
		//	Error msg = this->set_error(22);
		//	ostringstream info;
		//	info << "Info bad alloc: " << t.what() << endl;
		//	msg.make_second_info(info.str());
		//	throw msg;
		//}
		//int count = 0;

		//for (int i = 0; i < profile_up->get_number_points(); i++) {
		//	profile_up->get

		//	river_section_points[i].set_point_name(hyd_label::river_section_point);
		//	river_section_points[i].set_point_x_coordinate(profile_up->get_global_x(i));
		//	river_section_points[i].set_point_y_coordinate(profile_up->get_global_y(i));
		//	count++;

		//}
		//for (int i = profile_down->get_number_points()-1; i >=0; i--){
		//	river_section_points[count].set_point_name(hyd_label::river_section_point);
		//	river_section_points[count].set_point_x_coordinate(profile_down->get_global_x(i));
		//	river_section_points[count].set_point_y_coordinate(profile_down->get_global_y(i));
		//	count++;
		//}





		////points to polygon
		//this->river_section_polygon.set_number_points(all_p);
		//this->river_section_polygon.set_points(river_section_points);

	}
	catch(Error msg){
		ostringstream info;
		info << "River section id : "<< section_index << endl;
		info << "River name		  : " << this->Param_RV.RVName << endl;
		info << "River id         : " << this->Param_RV.RVNumber << endl;
		info << "Riverstation     : " << river_station.str()<< label::m<<endl;
		info << "Riverprofile name: " << prof_name.str()<<endl;

		msg.make_second_info(info.str());
		throw msg;
	}

	if(river_section_points!=NULL){
		delete []river_section_points;
		river_section_points=NULL;
	}
}
//Set a river polygon for storing the maximum result values of the inflow profile
void Hyd_Model_River::set_river_section_first_profile(void){
	this->river_section_polygon.set_number_points(4);

	Geo_Point river_section_points[4];

	river_section_points[0]=this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates();
	river_section_points[3]=this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates();
	river_section_points[1]=this->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates();
	river_section_points[2]=this->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates();
	
	try{
		river_section_points[0].set_point_name(hyd_label::river_section_point);
		river_section_points[1].set_point_name(hyd_label::river_section_point);
		river_section_points[1].add_values2coordinate(0.01, 0.01);
		river_section_points[2].set_point_name(hyd_label::river_section_point);
		river_section_points[2].add_values2coordinate(0.01, 0.01);
		river_section_points[3].set_point_name(hyd_label::river_section_point);
		this->river_section_polygon.set_points(river_section_points);
	}
	catch(Error msg){
		ostringstream info;
		info << "Inflow section "<< endl;
		info << "River name     : " << this->Param_RV.RVName << endl;
		info << "River id       : " << this->Param_RV.RVNumber << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Allocate the list of the pointer of a part of the river profiles
void Hyd_Model_River::allocate_part_profile_list(void){
	try{
		this->list_part_profiles=new _Hyd_River_Profile*[this->number_part_profiles];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(20);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< this->number_part_profiles; i++){
		this->list_part_profiles[i]=NULL;
	}
}
//Delete the list of pointer of a part of the river profiles
void Hyd_Model_River::delete_part_profile_list(void){
	if(this->list_part_profiles!=NULL){
		delete []this->list_part_profiles;
		this->list_part_profiles=NULL;
	}
}
//Set the list of the pointer of a part of the river profiles by two given profiles
void Hyd_Model_River::set_part_profile_list(_Hyd_River_Profile *up, _Hyd_River_Profile *down){
	this->delete_part_profile_list();
	this->number_part_profiles=0;
	if(up==NULL || down==NULL){
		return;
	}
	else{
		//count entries
		int counter=0;
		_Hyd_River_Profile *buffer=up;
		while(up->get_ptr_downstream_prof()!=NULL){
			buffer=buffer->get_ptr_downstream_prof();
			if(buffer!=down){
				counter++;
			}
			else{
				break;
			}
			if(counter>=this->number_inbetween_profiles){
				Error msg=this->set_error(21);
				throw msg;
			}
		}
		this->number_part_profiles=2+counter;
		this->allocate_part_profile_list();

		this->list_part_profiles[0]=up;
		
		buffer=up;
		//set the list
		for(int i=1; i< this->number_part_profiles-1; i++){
			buffer=buffer->get_ptr_downstream_prof();
			this->list_part_profiles[i]=buffer;

		}
		this->list_part_profiles[this->number_part_profiles-1]=down;

	}
}
//input the river profile per file
void Hyd_Model_River::input_river_profiles_perfile(void){
	//a profile is found
	bool found_flag=false;
	//file
	QFile profile_file;
	//count the lines in file
	int line_counter=0;
	//profile counter
	int profile_counter=0;

	//open file
	profile_file.setFileName(this->Param_RV.RVGeomtryFile.c_str());
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
		for(int i=0; i<this->Param_RV.RVNofProf; i++){
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
			//first profile
			if(i==0){
				this->inflow_river_profile.input_members(&profile_file, i, myline, &line_counter);
			}
			//last profile
			else if(i==this->Param_RV.RVNofProf-1){
				this->outflow_river_profile.input_members(&profile_file, i, myline, &line_counter);
			}
			//profiles in between
			else{
				this->river_profiles[i-1].input_members(&profile_file, i, myline, &line_counter);
			}

		}//end loop over profiles


		if(profile_counter<this->Param_RV.RVNofProf){
			Error msg=this->set_error(8);
			ostringstream info;
			info <<"Founded numbers of profile :" << profile_counter+1 << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}catch(Error msg){
		profile_file.close();
		ostringstream info;
		info << "Filename   : " <<this->Param_RV.RVGeomtryFile.c_str()<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	profile_file.close();
}
//Transfer the river profile data to a database
void Hyd_Model_River::transfer_profile_members2database(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream cout;
	if(with_output==true){
		cout <<"Transfer " << this->NEQ+1 << " profiles of the river model to database ..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	int id_glob=_Hyd_River_Profile::get_max_glob_id_prof_table(ptr_database)+1;
	QSqlQuery model(*ptr_database);
	//inflow
	this->inflow_river_profile.transfer_profile_members2database(id_glob, &model, ptr_database, this->Param_RV.get_river_number());
	id_glob++;
	//inbetween
	for(int i=0; i< this->number_inbetween_profiles; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		
		if(i%100==0 && i>0){
			if(with_output==true){
				cout << "Transfer river profile "<< i+1 <<" to " << i+100 <<" to database..."<< endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			}
		}
		this->river_profiles[i].transfer_profile_members2database(id_glob, &model, ptr_database, this->Param_RV.get_river_number());
		id_glob++;
	}
	//outflow
	this->outflow_river_profile.transfer_profile_members2database(id_glob, &model, ptr_database, this->Param_RV.get_river_number());

}
//Input the river profile data per database
void Hyd_Model_River::input_river_profiles_perdatabase(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag){
	if(output_flag==true){
		ostringstream cout;
		cout <<"Read in " << this->Param_RV.RVNofProf << " profiles of the river model from database ..."<<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	this->inflow_river_profile.input_members_per_database(0,query_result, ptr_database);
	for(int i=0; i< this->number_inbetween_profiles; i++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		this->river_profiles[i].input_members_per_database(i+1,query_result, ptr_database);
	}
	this->outflow_river_profile.input_members_per_database(this->number_inbetween_profiles+1,query_result, ptr_database);

}
//Calculate the distance between the profiles
void Hyd_Model_River::calculate_distance_of_profiles(void){
	//total river length
	this->delta_distance_outflow_inflow=this->inflow_river_profile.get_river_station()-this->outflow_river_profile.get_river_station();
	
	try{
		//segment length
		if(this->number_inbetween_profiles==0){//just an inflow and outflow profile
			this->outflow_river_profile.set_distance2upstream(this->inflow_river_profile.get_river_station());
			this->outflow_river_profile.set_distance2downstream(0.0);
			this->inflow_river_profile.set_distance2downstream(this->outflow_river_profile.get_distance2upstream());
		}
		else{
			for(int i=0; i< this->number_inbetween_profiles ;i++){
				if(i==0){//first segment
					this->river_profiles[i].set_distance2upstream(this->inflow_river_profile.get_river_station());
					this->inflow_river_profile.set_distance2downstream(this->river_profiles[i].get_distance2upstream());
				}
				else{//segments inbetween
					this->river_profiles[i].set_distance2upstream(this->river_profiles[i-1].get_river_station());
					this->river_profiles[i-1].set_distance2downstream(this->river_profiles[i].get_distance2upstream());
				}
			}
			//last segment
			this->outflow_river_profile.set_distance2upstream(this->river_profiles[this->number_inbetween_profiles-1].get_river_station());
			this->outflow_river_profile.set_distance2downstream(0.0);
			this->river_profiles[this->number_inbetween_profiles-1].set_distance2downstream(this->outflow_river_profile.get_distance2upstream());
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Calculate the geodetic height loss form inflow to outflow (delta_height_outflow_inflow)
void Hyd_Model_River::calculate_deltaz_in2out(void){
	this->delta_height_outflow_inflow=this->inflow_river_profile.typ_of_profile->get_global_z_min()-this->outflow_river_profile.typ_of_profile->get_global_z_min();
	if(this->delta_height_outflow_inflow<=0.0){
		Error msg=this->set_error(10);
		ostringstream info;
		info<< "Calculated slope is "<< this->delta_height_outflow_inflow<< label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Calculate the maximum / minimum width of the river
void Hyd_Model_River::calculate_river_width(void) {

	double buffer = 0.0;
	//calculate max width

	this->max_width = buffer;
	//inflow profile
	buffer = this->inflow_river_profile.get_river_width();
	if (buffer > this->max_width) {
		this->max_width = buffer;
	}
	//inbetween profile
	for(int i = 0; i < this->number_inbetween_profiles; i++) {
		buffer = this->river_profiles[i].get_river_width();
		if (buffer > this->max_width) {
			this->max_width = buffer;
		}
	}
	//outflow profile
	buffer = this->outflow_river_profile.get_river_width();
	if (buffer > this->max_width) {
		this->max_width = buffer;
	}



	//calculate min width
	buffer = 10000000000.0;
	this->min_width = buffer;
	//inflow profile
	buffer = this->inflow_river_profile.get_river_width();
	if (buffer < this->min_width) {
		this->min_width = buffer;
	}
	//inbetween profile
	for (int i = 0; i < this->number_inbetween_profiles; i++) {
		buffer = this->river_profiles[i].get_river_width();
		if (buffer < this->min_width) {
			this->min_width = buffer;
		}
	}
	//outflow profile
	buffer = this->outflow_river_profile.get_river_width();
	if (buffer < this->min_width) {
		this->min_width = buffer;
	}



}
//Initialize the river geometrie as a midline and a polygon
void Hyd_Model_River::init_river_geometrie(void){

	Geo_Point *buffer=NULL;
	//make first: the midline of the river
	try{
		buffer=new Geo_Point[this->Param_RV.RVNofProf]; 
	}
	catch(bad_alloc &){
		Error msg=this->set_error(12);
		throw msg;	
	}

	//inflow profile
	buffer[0]=this->inflow_river_profile.typ_of_profile->mid_point_global_x_y;
	//inbetween profile
	for(int i=0; i<this->number_inbetween_profiles;i++){
		buffer[i+1]=this->river_profiles[i].typ_of_profile->mid_point_global_x_y;
	}
	//outflow profile
	buffer[this->Param_RV.RVNofProf-1]=this->outflow_river_profile.typ_of_profile->mid_point_global_x_y;

	try{
		//give the points to to the midline of the river
		this->river_midline.set_number_of_points(this->Param_RV.RVNofProf);
		this->river_midline.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to generate the river midline of rivermodel " << this->Param_RV.RVNumber<< endl;
		info << "Check the global x-y -coordinates of the profiles" << endl;
		info << "The river midline points are printed in the HYD-logfile" << endl;
		info << "Just copy and paste to an empty file and read it with QGIS csv-import" << endl;
		info << "Check the course of the river midline" << endl;
		this->river_midline.output_members();
		msg.make_second_info(info.str());
		throw msg;
	}

	//set river minline
	//inflow profile
	buffer[0]=this->inflow_river_profile.typ_of_profile->min_point_global_x_y;
	//inbetween profile
	for(int i=0; i<this->number_inbetween_profiles;i++){
		buffer[i+1]=this->river_profiles[i].typ_of_profile->min_point_global_x_y;
	}
	//outflow profile
	buffer[this->Param_RV.RVNofProf-1]=this->outflow_river_profile.typ_of_profile->min_point_global_x_y;

	try{
		//give the points to to the midline of the river
		this->river_minline.set_number_of_points(this->Param_RV.RVNofProf);
		this->river_minline.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to generate the river minline of rivermodel " << this->Param_RV.RVNumber<< endl;
		info << "The river minline points are printed in the HYD-logfile" << endl;
		info << "Just copy and paste to an empty file and read it with QGIS csv-import" << endl;
		info << "Check the course of the river minline" << endl;
		this->river_minline.output_members();
		msg.make_second_info(info.str());
		throw msg;
	}






	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}


	//make second: the river polygon
	try{
		buffer=new Geo_Point[this->Param_RV.RVNofProf*2]; 
	}
	catch(bad_alloc &){
		Error msg=this->set_error(12);
		throw msg;
	}
	int counter=0;
	//in flowdirection on the left side
	//inflow profile
	buffer[counter]=this->inflow_river_profile.typ_of_profile->points[0].get_global_x_y_coordinates();
	counter++;
	//inbetween profile
	for(int i=0; i<this->number_inbetween_profiles;i++){
		buffer[counter]=this->river_profiles[i].typ_of_profile->points[0].get_global_x_y_coordinates();
		counter++;
	}
	//outflow profile
	buffer[counter]=this->outflow_river_profile.typ_of_profile->points[0].get_global_x_y_coordinates();
	counter++;

	//in flowdirection on the right side beginning from the last profile
	//outflow profile
	buffer[counter]=this->outflow_river_profile.typ_of_profile->points[this->outflow_river_profile.typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
	counter++;
	//inbetween profile
	for(int i=this->number_inbetween_profiles-1; i>=0 ;i--){
		buffer[counter]=this->river_profiles[i].typ_of_profile->points[this->river_profiles[i].typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
		counter++;
	}
	//inflow profile
	buffer[counter]=this->inflow_river_profile.typ_of_profile->points[this->inflow_river_profile.typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
	counter++;
		

	try{
		//give the points to to the river polygon
		this->river_polygon.set_number_points(this->Param_RV.RVNofProf*2);
		this->river_polygon.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to generate the river polygon of rivermodel "<< this->Param_RV.RVNumber << endl;
		info << "Check the global outer x-y -coordinates (left/right) of the profiles" << endl;
		info << "The river polygon points are printed in the HYD-logfile" << endl;
		info << "Just copy and paste to an empty file and read it with QGIS csv-import" << endl;
		info << "Check the course of the river polygons" << endl;
		this->river_polygon.output_members();
		msg.make_second_info(info.str());
		throw msg;
	}
	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

	//make third: the river left bank line
	try{
		buffer=new Geo_Point[this->Param_RV.RVNofProf]; 
	}
	catch(bad_alloc &){
		Error msg=this->set_error(12);
		throw msg;
	}
	counter=0;
	//in flowdirection on the left side
	//inflow profile
	buffer[counter]=this->inflow_river_profile.typ_of_profile->points[0].get_global_x_y_coordinates();
	counter++;
	//inbetween profile
	for(int i=0; i<this->number_inbetween_profiles;i++){
		buffer[counter]=this->river_profiles[i].typ_of_profile->points[0].get_global_x_y_coordinates();
		counter++;
	}
	//outflow profile
	buffer[counter]=this->outflow_river_profile.typ_of_profile->points[0].get_global_x_y_coordinates();
	counter++;

	try{
		//give the points to to the polysegment
		this->river_leftline.set_number_of_points(this->Param_RV.RVNofProf);
		this->river_leftline.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to generate the river leftline of rivermodel "<< this->Param_RV.RVNumber << endl;
		info << "The river leftline points are printed in the HYD-logfile" << endl;
		info << "Just copy and paste to an empty file and read it with QGIS csv-import" << endl;
		info << "Check the course of the river leftline" << endl;
		this->river_leftline.output_members();
		msg.make_second_info(info.str());
		throw msg;
	}

	//make forth: the river right bank line
	counter=0;
	//inflow profile
	buffer[counter]=this->inflow_river_profile.typ_of_profile->points[this->inflow_river_profile.typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
	counter++;
	//inbetween profile
	for(int i=0; i<this->number_inbetween_profiles;i++){
		buffer[counter]=this->river_profiles[i].typ_of_profile->points[this->river_profiles[i].typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
		counter++;
	}
	//outflow profile
	buffer[counter]=this->outflow_river_profile.typ_of_profile->points[this->outflow_river_profile.typ_of_profile->get_number_points()-1].get_global_x_y_coordinates();
	counter++;
	try{
		//give the points to to the polysegment
		this->river_rightline.set_number_of_points(this->Param_RV.RVNofProf);
		this->river_rightline.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Try to generate the river rightline of rivermodel "<< this->Param_RV.RVNumber << endl;
		info << "The river rightline points are printed in the HYD-logfile" << endl;
		info << "Just copy and paste to an empty file and read it with QGIS csv-import" << endl;
		info << "Check the course of the river rightline" << endl;
		this->river_rightline.output_members();
		msg.make_second_info(info.str());
		throw msg;
	}

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//set the init condition to the result vector
void Hyd_Model_River::set_initcond2resultvector(void){

	this->inflow_river_profile.set_init_condition();
	for(int i=0; i<this->number_inbetween_profiles;i++){
		this->river_profiles[i].set_init_condition();
	}
	//the last profile issetted; it gives the values forthe last segment
	this->results_real[0]=this->inflow_river_profile.get_init_condition_value();
	//the profile in between are setted; second profile of segment gives the values 
	for(int i=1; i< this->NEQ; i++){
		this->results_real[i]=this->river_profiles[i-1].get_init_condition_value();
	}
}
//allocate the instationary boundary curve 
void Hyd_Model_River::allocate_instat_boundarycurves(void){
	try{
		this->instat_boundary_curves=new Hyd_Instationary_Boundary[this->Param_RV.number_instat_boundary];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(3);
		throw msg;
	}
	//set the szenario data
	for(int i=0; i< this->Param_RV.number_instat_boundary; i++){
		this->instat_boundary_curves[i].hyd_sz.set_members(&this->hyd_sz);
		this->instat_boundary_curves[i].set_systemid(this->system_id);

	}
}
//read the instationary boundary curve from file
void Hyd_Model_River::read_instat_boundarycurves(void){

	if(this->Param_RV.number_instat_boundary==0){
		return;
	}
	ostringstream cout;
	//read in the instationary boundary curves
	cout <<"Read in " << this->Param_RV.number_instat_boundary << " instationary boundary curves of the Rivermodel from file"<< this->Param_RV.inst_boundary_file_name << " ..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//allocate instat boundary
	this->allocate_instat_boundarycurves();
	
	for (int i=0; i< this->Param_RV.number_instat_boundary; i++){
		try{
			this->instat_boundary_curves[i].read_value(this->Param_RV.inst_boundary_file_name, i);
		}
		catch(Error msg){
			ostringstream info;
			info <<"Instationary Boundary curve no. "<< i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Read in the instationary boundary curves from a database
void Hyd_Model_River::read_instat_boundary_curves(const bool output_flag, QSqlDatabase *ptr_database){

	QSqlTableModel query_result(0,*ptr_database);
	int number=0;


	number=Hyd_Instationary_Boundary::select_relevant_curves_database(&query_result, this->Param_RV.RVNumber, _hyd_model_type::RIVER_MODEL, output_flag, &this->hyd_sz);

	if(number<=0){
		this->Param_RV.number_instat_boundary=0;
		return;
	}
	else{
		this->Param_RV.number_instat_boundary=number;
		this->allocate_instat_boundarycurves();
		for(int i=0; i<number; i++){
			this->instat_boundary_curves[i].read_value(&query_result, i);
		}

	}

}
//connet the profiles to the instat boundary curves
void Hyd_Model_River::connect_profiles2instat_boundarycurves(void){	
	ostringstream cout;
	cout <<"Connect the river profiles with the pointwise instationary boundary curves..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	//first pointwise
	//search for the link (point wise)
	for(int i=0; i< this->Param_RV.number_instat_boundary;i++){
		//connect the first and last profile profile (point wise)
		try{
			this->inflow_river_profile.connect_instat_point_boundarycurve(&this->instat_boundary_curves[i]);
		}
		catch(Error msg){
			ostringstream info;
			info <<" Inflow profile  " << endl;
			info <<" Instationary Boundary curve no. "<< i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//connect all profiles in between (point wise)
		for(int j=0; j< this->number_inbetween_profiles; j++){
			try{
				this->river_profiles[j].connect_instat_point_boundarycurve(&this->instat_boundary_curves[i]);
			}
			catch(Error msg){
				ostringstream info;
				info <<" Profilenumber "<< j << endl;
				info <<" Instationary Boundary curve no. "<< i << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	


	//second lateral
	cout <<"Connect the river profiles with the lateral instationary boundary curves..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	//connect the inflow (lateral)
	for(int i=0; i< this->Param_RV.number_instat_boundary;i++){//loop over all curves
		try{
			this->inflow_river_profile.connect_instat_lateral_boundarycurve(&this->instat_boundary_curves[i]);
		}
		catch(Error msg){
			ostringstream info;
			info <<"First profile (inflow) " << endl;
			info <<"Instationary Boundary curve no. "<< i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//connect the profiles in between (lateral)
		for(int j=0; j< this->number_inbetween_profiles; j++){
			try{
				this->river_profiles[j].connect_instat_lateral_boundarycurve(&this->instat_boundary_curves[i]);
				
			}
			catch(Error msg){
				ostringstream info;
				info <<"Profilenumber "<< j << endl;
				info <<"Instationary Boundary curve no. "<< i << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}

	//third waterlevel boundary curve for the last profile
	cout <<"Connect the last river profile (outflow) with the waterlevel instationary boundary curves..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//connect the last profile profile (waterlevel boundary)
	for(int i=0; i< this->Param_RV.number_instat_boundary;i++){//loop over all curves
		try{
			this->outflow_river_profile.connect_instat_waterlevel_boundarycurve(&this->instat_boundary_curves[i]);
		}
		catch(Error msg){
			ostringstream info;
			info <<"Last profile (outflow) " << endl;
			info <<"Instationary Boundary curve no. "<< i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//check instationary boundary found
void Hyd_Model_River::check_instationary_boundary_found(void){
	//lateral
	//check the first profile (inflow) (lateral)
	if(this->inflow_river_profile.get_instat_lateral_boundarycurve_found()!=true){
		Error msg=this->set_error(4);
		ostringstream info;
		info <<"The first profile (inflow) could not be connected" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//check if all are found (starts by one because the first profile can not have any lateral boundary condition)
	for(int j=0; j< this->number_inbetween_profiles; j++){//loop over all elems
		if(this->river_profiles[j].get_instat_lateral_boundarycurve_found()!=true){
			Error msg=this->set_error(4);
			ostringstream info;
			info <<"Profilenumber "<< j+1 << endl;
			info <<"Profilename   "<< this->river_profiles[j].get_profile_name() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//pointwise
	//check if all needed curves are found (point wise)
	//check the first profile (point wise)
	if(this->inflow_river_profile.get_instat_point_boundarycurve_found()!=true){
		Error msg=this->set_error(4);
		ostringstream info;
		info <<"The first profile (inflow) could not be connected" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//check the profiles between (point wise)
	for(int j=0; j< this->number_inbetween_profiles; j++){//loop over all elems
		if(this->river_profiles[j].get_instat_point_boundarycurve_found()!=true){
			Error msg=this->set_error(4);
			ostringstream info;
			info <<"Profilenumber "<< j+1 << endl;
			info <<"Profilename   "<< this->river_profiles[j].get_profile_name() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	//waterlevel
	//check the last profile for the waterlevel boundary curve
	if(this->outflow_river_profile.get_instat_waterlevel_boundarycurve_found()!=true){
		Error msg=this->set_error(4);
		ostringstream info;
		info <<"The last profile (outflow) could not be connected with the instationary waterlevel boundary curve" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//set function to solve to the solver
void Hyd_Model_River::set_function2solver(void){
	int flag=-1;
	//set the function where the diff equation is specified
	//The function CVodeMalloc provides required problem and solution speci¯cations, allocates internal memory, and initializes cvode.
	flag = CVodeInit(this->cvode_mem, f1D_equation2solve, 0.0, this->results);

		
	this->setted_rel_tol=this->Param_RV.get_relative_solver_tolerance();
	this->setted_abs_tol=*(this->Param_RV.get_absolute_solver_tolerance());
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

	flag=CVodeSStolerances(this->cvode_mem, this->Param_RV.get_relative_solver_tolerance(),*this->Param_RV.get_absolute_solver_tolerance());
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
void Hyd_Model_River::output_solver_statistics(void){
	//set prefix for output
	ostringstream cout;
	cout << "SOLVER" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	_Hyd_Model::output_solver_statistics(&cout);

}
//Initialize the output file for a tecplot output
void Hyd_Model_River::init_tecplot_output(void){
	//for 1d-output
	//results per timesteps
	if(this->Param_RV.tecplot_outfile_name_1d!=label::not_set){
		if(this->tecplot_output.is_open()==false){
			//open the file
			string buffer = this->Param_RV.get_filename_result2file_1d(hyd_label::tecplot);
			//Add file type
			buffer += hyd_label::dat;
			this->tecplot_output.open(buffer.c_str());
			//check if it is open
			if(this->tecplot_output.is_open()==false){
				Error msg=this->set_error(11);
				ostringstream info;
				info << "File name " << this->Param_RV.get_filename_result2file_1d(hyd_label::tecplot) << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				//output the file header
				this->tecplot_output << "TITLE = "<< "\" " << this->Param_RV.RVName << " No. " << this->Param_RV.RVNumber << " \"" << endl;
				this->tecplot_output << "VARIABLES = " << "\" " << " Riverstation " << label::m << "\"   \"" << " zmin " << label::m << "\"";
				this->tecplot_output << " \"" << " z_left_bank " << label::m << "\" ";
				this->tecplot_output << " \"" << " z_right_bank " << label::m << "\" ";
				this->tecplot_output << " \"" << " s (global) " << label::m << "\"";
				this->tecplot_output << " \"" << " h (local) " << label::m <<   "\"";
				this->tecplot_output <<	" \"" <<  " v " << label::m_per_sec << "\"";
				this->tecplot_output <<	" \"" <<  " fr " << label::no_unit << "\"";
				this->tecplot_output << " \"" << " width " << label::m << "\"";
				this->tecplot_output << " \"" << " q " << label::qm_per_sec << "\" ";
				
				this->tecplot_output <<  endl << endl;
			}
		}
	}

	//for 2d-output
	if(this->Param_RV.tecplot_outfile_name_2d!=label::not_set){
		if(this->tecplot_output_2d.is_open()==false){
			//open the file
			string buffer = this->Param_RV.get_filename_result2file_2d(hyd_label::tecplot);
			//Add file type
			buffer += hyd_label::dat;
			this->tecplot_output_2d.open(buffer.c_str());
			//check if it is open
			if(this->tecplot_output_2d.is_open()==false){
				Error msg=this->set_error(11);
				ostringstream info;
				info << "File name " << this->Param_RV.get_filename_result2file_2d(hyd_label::tecplot)<< endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				//output the file header
				this->tecplot_output_2d << "TITLE = "<< "\" " << this->Param_RV.RVName << " RVNo. " << this->Param_RV.RVNumber << " \"" << endl;
				this->tecplot_output_2d << "VARIABLES = " <<endl;
				this->tecplot_output_2d << "\" x " << label::m << "\" "<<endl;
				this->tecplot_output_2d << "\" y " << label::m << "\" "<< endl;
				this->tecplot_output_2d << "\" h " << label::m << "\" " << endl;
				this->tecplot_output_2d <<	"\" s " << label::m << "\" "<< endl;
				this->tecplot_output_2d << "\" v_tot " << label::m_per_sec << "\" " << endl;
				this->tecplot_output_2d <<endl;
			}
		}
	}
}
//Initialize and output to the output file for a tecplot the maximum results (use it after the results per timesteps)
void Hyd_Model_River::output_maximum_results2tecplot(void){


	//maximum results
	if(this->Param_RV.tecplot_outfile_name_1d!=label::not_set){
		//close the result file per time step
		this->tecplot_output.close();
		this->tecplot_output.clear();

		//reopen the file
		string buffer = this->Param_RV.get_filename_result2file_1d_maxvalues(hyd_label::tecplot);
		//Add file type
		buffer += hyd_label::dat;
		this->tecplot_output.open(buffer.c_str());
		//check if it is open
		if(this->tecplot_output.is_open()==false){
			Error msg=this->set_error(11);
			ostringstream info;
			info << "File name " << this->Param_RV.get_filename_result2file_1d_maxvalues(hyd_label::tecplot) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//output the file header
		this->tecplot_output << "TITLE = "<< "\" " << this->Param_RV.RVName << " No. " << this->Param_RV.RVNumber << " \"" << endl;
		this->tecplot_output << "VARIABLES = " <<endl;
		this->tecplot_output<< "\" " << " Riverstation " << label::m ;
		this->tecplot_output<< "\" \"" << " z_min " << label::m ;
		this->tecplot_output << "\" \"" << " z_left_bank " << label::m;
		this->tecplot_output << "\" \"" << " z_right_bank " << label::m;
		this->tecplot_output<< "\" \"" << " s_max " << label::m ;
		this->tecplot_output<< "\"   \"" << " t(s_max) " << label::sec ;
		this->tecplot_output<< "\"  \"" << " h_max " << label::m ;
		this->tecplot_output<< "\"   \"" << " t(h_max) " << label::sec ;
		this->tecplot_output<< "\"   \"" << " v_max " << label::m_per_sec ;
		this->tecplot_output<< "\"   \"" <<  " t(v_max) " << label::sec ;
		this->tecplot_output<< "\"   \"" << " was_dry " << label::no_unit;
		this->tecplot_output<<   "\"   \"" << " t(dry) " << label::sec;
		this->tecplot_output<<   "\"   \"" << " t(wet) " << label::sec;
		this->tecplot_output << "\"   \"" << " width_max " << label::m;
		this->tecplot_output << "\"   \"" << " t(width_max) " << label::sec;
		this->tecplot_output<<  "\"   \"" <<"CVol_1D_in" << label::cubicmeter;
		this->tecplot_output<<  "\"   \"" <<"CVol_1D_out" << label::cubicmeter ;
		this->tecplot_output<<  "\"   \"" <<"CVol_Struc_in" << label::cubicmeter ;
		this->tecplot_output<<  "\"   \"" <<"CVol_Struc_out" << label::cubicmeter ;
		this->tecplot_output<<  "\"   \"" <<"CVol_OV_Le_in" << label::cubicmeter ;
		this->tecplot_output<< "\"   \"" << "CVol_OV_Le_out" << label::cubicmeter ; 
		this->tecplot_output<<  "\"   \"" <<"CVol_OV_Ri_in" << label::cubicmeter ;
		this->tecplot_output<<  "\"   \"" <<"CVol_OV_Ri_out" << label::cubicmeter ; 
		this->tecplot_output<<  "\"   \"" <<"CVol_DB_Le_in" << label::cubicmeter ;
		this->tecplot_output<<  "\"   \"" <<"CVol_DB_Le_out" << label::cubicmeter ; 
		this->tecplot_output<<  "\"   \"" <<"CVol_DB_Ri_in" << label::cubicmeter ;
		this->tecplot_output<< "\"   \"" << "CVol_DB_Ri_out" << label::cubicmeter ;
		this->tecplot_output << "\"   \"" <<"CVol_tot_in" <<label::cubicmeter ;
		this->tecplot_output << "\"   \"" <<"CVol_tot_out" <<label::cubicmeter ;
		this->tecplot_output << "\"   \"" <<"Qmax" <<label::qm_per_sec ;
		this->tecplot_output << "\"   \"" <<"t[Qmax]" <<label::sec ;
		this->tecplot_output<< "\" " <<  endl << endl;

		//inflow profile
		this->inflow_river_profile.output_max_results2file(&this->tecplot_output);

		//inbetween profiles
		for(int i=0; i<this->number_inbetween_profiles; i++){
			this->river_profiles[i].output_max_results2file(&this->tecplot_output);

		}
		//outflow profile
		this->outflow_river_profile.output_max_results2file(&this->tecplot_output);

		//close it
		this->tecplot_output.close();
	}
}
//Output the maximum results
void Hyd_Model_River::output_maximum_results(void){

	_Hyd_River_Profile::output_header_max_results();
	//inflow profile
	this->inflow_river_profile.output_max_results();

	//inbetween profiles
	for(int i=0; i<this->number_inbetween_profiles; i++){
		this->river_profiles[i].output_max_results();
	}
	//outflow profile
	this->outflow_river_profile.output_max_results();

}
//Count element types
void Hyd_Model_River::count_profile_types(void){

	//outflow and inflow profile are always river profiles
	this->number_river_profiles=2;
	//inbetween profiles
	for(int i=0; i<this->number_inbetween_profiles; i++){
		switch(this->river_profiles[i].get_profile_type()){
			case _hyd_profile_types::RIVER_TYPE:
				this->number_river_profiles++;	
				break;
			case _hyd_profile_types::WEIR_TYPE:
				this->number_weir_profiles++;	
				break;
			case _hyd_profile_types::BRIDGE_TYPE:
				this->number_bridge_profiles++;	
				break;
			default:
				Error msg=this->set_error(14);
				ostringstream info;
				info<<"Profile number " << i+1<< endl;
				msg.make_second_info(info.str());
				throw msg;
		}
	}
}
//Output the hydrological balance of the river model
void Hyd_Model_River::output_hydrological_balance(void){
	ostringstream cout;

	this->calculate_total_hydrological_balance();

	cout << "HYDROLOGICAL BALANCE " << label::cubicmeter << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	cout << " Total watervolume init-cond.             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_watervolume_init<< endl;
	cout << " Total watervolume left                   :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_watervolume<< endl;
	if(abs(this->total_volume_error_zero_outflow)>constant::zero_epsilon){
		cout << "  Total error dry outflow                 :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_volume_error_zero_outflow<< endl;
	}
	cout << " Total volume inflow profile              :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->inflow_river_profile.get_inflow_volume()<< endl;
	if(abs(this->inflow_river_profile.get_watervolume_difference_direct_coupling())>constant::zero_epsilon){
		cout << "  Error volume inflow (h_bound)           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->inflow_river_profile.get_watervolume_difference_direct_coupling()<< endl;
	}
	cout << " Total volume outflow profile             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->outflow_river_profile.get_outflow_volume()<< endl;
	if(abs(this->outflow_river_profile.get_watervolume_difference_direct_coupling())>constant::zero_epsilon){
		cout << "  Error volume outflow (h_bound)          :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->outflow_river_profile.get_watervolume_difference_direct_coupling()<< endl;
	}	
	cout << " Total boundaryvolume (point)             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_point_boundary_volume.volume_total<< endl;
	if(abs(this->total_point_boundary_volume.volume_in)>constant::zero_epsilon || abs(this->total_point_boundary_volume.volume_out)>constant::zero_epsilon){
		cout << "  Boundaryvolume in   (point)             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_point_boundary_volume.volume_in<< endl;
		cout << "  Boundaryvolume out  (point)             :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_point_boundary_volume.volume_out<< endl;
	}
	cout << " Total boundaryvolume (lateral)           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_lateral_boundary_volume.volume_total<< endl;
	if(abs(this->total_lateral_boundary_volume.volume_in)>constant::zero_epsilon || abs(this->total_lateral_boundary_volume.volume_out)>constant::zero_epsilon){
		cout << "  Boundaryvolume in   (lateral)           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_lateral_boundary_volume.volume_in<< endl;
		cout << "  Boundaryvolume out  (lateral)           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_lateral_boundary_volume.volume_out<< endl;
	}
	cout << " Total boundaryvolume                     :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_lateral_boundary_volume.volume_total+this->total_point_boundary_volume.volume_total << endl;
	cout << " Total couplingvolume (left bank)         :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_left_bank_volume.volume_total<< endl;
	if(abs(this->total_coupling_left_bank_volume.volume_in)>constant::zero_epsilon || abs(this->total_coupling_left_bank_volume.volume_out)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (left bank)         :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_left_bank_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (left bank)         :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_left_bank_volume.volume_out<< endl;
	}
	cout << " Total couplingvolume (right bank)        :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_right_bank_volume.volume_total<< endl;
	if(abs(this->total_coupling_right_bank_volume.volume_in)>constant::zero_epsilon || abs(this->total_coupling_right_bank_volume.volume_out)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (right bank)        :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_right_bank_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (right bank)        :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_right_bank_volume.volume_out<< endl;
	}
	cout << " Total couplingvolume (1d)                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_1d_volume.volume_total<< endl;
	if(abs(this->total_coupling_1d_volume.volume_out)>constant::zero_epsilon || abs(this->total_coupling_1d_volume.volume_in)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (1d)                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_1d_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (1d)                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_coupling_1d_volume.volume_out<< endl;
	}
	cout << " Total structure couplingvolume (2d)      :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_structure_coupling_volume.volume_total<< endl;
	if(abs(this->total_structure_coupling_volume.volume_out)>constant::zero_epsilon || abs(this->total_structure_coupling_volume.volume_in)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (2d)                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_structure_coupling_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (2d)                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_structure_coupling_volume.volume_out<< endl;
	}
	cout << " Total dikebreak couplingvolume (left)    :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_left_dikebreak_coupling_volume.volume_total<< endl;
	if(abs(this->total_left_dikebreak_coupling_volume.volume_out)>constant::zero_epsilon || abs(this->total_left_dikebreak_coupling_volume.volume_in)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (left bank)         :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_left_dikebreak_coupling_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (left bank)         :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_left_dikebreak_coupling_volume.volume_out<< endl;
	}
	cout << " Total dikebreak couplingvolume (right)   :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_right_dikebreak_coupling_volume.volume_total<< endl;
	if(abs(this->total_right_dikebreak_coupling_volume.volume_out)>constant::zero_epsilon || abs(this->total_right_dikebreak_coupling_volume.volume_in)>constant::zero_epsilon){	
		cout << "  Couplingvolume in   (right bank)        :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_right_dikebreak_coupling_volume.volume_in<< endl;
		cout << "  Couplingvolume out  (right bank)        :"<<W(15)<< P(1) << FORMAT_FIXED_REAL<< this->total_right_dikebreak_coupling_volume.volume_out<< endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	cout << " Sum of volumes                           :"<<W(15)<< P(1) << FORMAT_FIXED_REAL << this->sum_volumes << endl;
	cout << " Error [%]                                :"<<W(15)<< P(1) << FORMAT_FIXED_REAL << this->error_hydrological_balance << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
}
//Calculate the hydrological balance of the river model
void Hyd_Model_River::calculate_total_hydrological_balance(void){
	//reset the values
	this->reset_total_hydrological_balance();

	//calculate the init watervolume and the water volume at this time point
	this->total_watervolume_init=(this->inflow_river_profile.get_watervolume_init());
	this->total_watervolume=(this->inflow_river_profile.get_watervolume_element());
	for(int i=0; i< this->number_inbetween_profiles; i++){
		this->total_watervolume_init=this->total_watervolume_init+this->river_profiles[i].get_watervolume_init();
		this->total_watervolume=this->total_watervolume+this->river_profiles[i].get_watervolume_element();
	}

	//calculated it new for inbetween profiles
	for(int i=0; i<this->number_inbetween_profiles; i++){
		this->river_profiles[i].add_hydro_balance_point_boundary(&this->total_point_boundary_volume);
		this->river_profiles[i].add_hydro_balance_lateral_boundary(&this->total_lateral_boundary_volume);
		this->river_profiles[i].add_hydro_balance_leftbank_coupling(&this->total_coupling_left_bank_volume);
		this->river_profiles[i].add_hydro_balance_rightbank_coupling(&this->total_coupling_right_bank_volume);
		this->river_profiles[i].add_hydro_balance_coupling_1d(&this->total_coupling_1d_volume);
		this->river_profiles[i].add_hydro_balance_structure_coupling(&this->total_structure_coupling_volume);
		this->river_profiles[i].add_hydro_balance_dikebreak_left_coupling(&this->total_left_dikebreak_coupling_volume);
		this->river_profiles[i].add_hydro_balance_dikebreak_right_coupling(&this->total_right_dikebreak_coupling_volume);
		this->total_volume_error_zero_outflow=this->total_volume_error_zero_outflow+this->river_profiles[i].get_outflow_zero_error_volume();

	}
	//for the inflow profile
	this->inflow_river_profile.add_hydro_balance_point_boundary(&this->total_point_boundary_volume);
	this->inflow_river_profile.add_hydro_balance_lateral_boundary(&this->total_lateral_boundary_volume);
	this->inflow_river_profile.add_hydro_balance_leftbank_coupling(&this->total_coupling_left_bank_volume);
	this->inflow_river_profile.add_hydro_balance_rightbank_coupling(&this->total_coupling_right_bank_volume);
	this->inflow_river_profile.add_hydro_balance_coupling_1d(&this->total_coupling_1d_volume);
	this->inflow_river_profile.add_hydro_balance_structure_coupling(&this->total_structure_coupling_volume);
	this->inflow_river_profile.add_hydro_balance_dikebreak_left_coupling(&this->total_left_dikebreak_coupling_volume);
	this->inflow_river_profile.add_hydro_balance_dikebreak_right_coupling(&this->total_right_dikebreak_coupling_volume);
	this->total_volume_error_zero_outflow=this->total_volume_error_zero_outflow+this->inflow_river_profile.get_outflow_zero_error_volume();

	//for the outflow profile
	this->outflow_river_profile.add_hydro_balance_leftbank_coupling(&this->total_coupling_left_bank_volume);
	this->outflow_river_profile.add_hydro_balance_rightbank_coupling(&this->total_coupling_right_bank_volume);
	this->outflow_river_profile.add_hydro_balance_coupling_1d(&this->total_coupling_1d_volume);
	this->outflow_river_profile.add_hydro_balance_structure_coupling(&this->total_structure_coupling_volume);
	this->outflow_river_profile.add_hydro_balance_dikebreak_left_coupling(&this->total_left_dikebreak_coupling_volume);
	this->outflow_river_profile.add_hydro_balance_dikebreak_right_coupling(&this->total_right_dikebreak_coupling_volume);




	//make the sum
	this->sum_volumes=this->total_watervolume-(this->total_watervolume_init+
		this->total_point_boundary_volume.volume_total+this->total_lateral_boundary_volume.volume_total+
		this->total_coupling_left_bank_volume.volume_total+this->total_coupling_right_bank_volume.volume_total-
		this->outflow_river_profile.get_outflow_volume()+this->inflow_river_profile.get_inflow_volume()+this->inflow_river_profile.get_watervolume_difference_direct_coupling()+
		this->total_coupling_1d_volume.volume_total+this->total_structure_coupling_volume.volume_total+
		this->total_left_dikebreak_coupling_volume.volume_total+this->total_right_dikebreak_coupling_volume.volume_total+
		this->total_volume_error_zero_outflow);

	//calculate the error
	double abs_sum=abs(this->total_point_boundary_volume.volume_in)+abs(this->total_point_boundary_volume.volume_out)+
			abs(this->total_lateral_boundary_volume.volume_in)+abs(this->total_lateral_boundary_volume.volume_out)+
			abs(this->total_coupling_left_bank_volume.volume_in)+abs(this->total_coupling_left_bank_volume.volume_out)+
			abs(this->total_coupling_right_bank_volume.volume_in)+abs(this->total_coupling_right_bank_volume.volume_out)+
			abs(this->outflow_river_profile.get_outflow_volume())+abs(this->inflow_river_profile.get_inflow_volume())+
			abs(this->total_coupling_1d_volume.volume_in)+abs(this->total_coupling_1d_volume.volume_out)+
			abs(this->total_left_dikebreak_coupling_volume.volume_in)+abs(this->total_left_dikebreak_coupling_volume.volume_out)+
			abs(this->total_right_dikebreak_coupling_volume.volume_in)+abs(this->total_right_dikebreak_coupling_volume.volume_out)+
			abs(this->total_structure_coupling_volume.volume_in)+abs(this->total_structure_coupling_volume.volume_out);

	if(abs_sum>1e-3){
				this->error_hydrological_balance=(this->sum_volumes/abs_sum)*100.0;
	}

}
//Reset the total hydrological balance values
void Hyd_Model_River::reset_total_hydrological_balance(void){
	//reset the values
	this->total_watervolume=0.0;
	this->sum_volumes=0.0;
	this->total_lateral_boundary_volume.volume_in=0.0;
	this->total_lateral_boundary_volume.volume_out=0.0;
	this->total_lateral_boundary_volume.volume_total=0.0;
	this->total_point_boundary_volume.volume_in=0.0;
	this->total_point_boundary_volume.volume_out=0.0;
	this->total_point_boundary_volume.volume_total=0.0;
	this->total_coupling_left_bank_volume.volume_in=0.0;
	this->total_coupling_left_bank_volume.volume_out=0.0;
	this->total_coupling_left_bank_volume.volume_total=0.0;
	this->total_coupling_right_bank_volume.volume_in=0.0;
	this->total_coupling_right_bank_volume.volume_out=0.0;
	this->total_coupling_right_bank_volume.volume_total=0.0;
	this->total_coupling_1d_volume.volume_in=0.0;
	this->total_coupling_1d_volume.volume_out=0.0;
	this->total_coupling_1d_volume.volume_total=0.0;
	this->total_structure_coupling_volume.volume_in=0.0;
	this->total_structure_coupling_volume.volume_out=0.0;
	this->total_structure_coupling_volume.volume_total=0.0;
	this->total_left_dikebreak_coupling_volume.volume_in=0.0;
	this->total_left_dikebreak_coupling_volume.volume_out=0.0;
	this->total_left_dikebreak_coupling_volume.volume_total=0.0;
	this->total_right_dikebreak_coupling_volume.volume_in=0.0;
	this->total_right_dikebreak_coupling_volume.volume_out=0.0;
	this->total_right_dikebreak_coupling_volume.volume_total=0.0;
	this->error_hydrological_balance=0.0;
	this->total_volume_error_zero_outflow=0.0;
}
//Get the model description
string Hyd_Model_River::get_model_description(void){
	ostringstream txt;
	txt << "Rivermodel " << this->Param_RV.RVNumber <<", name : " << this->Param_RV.RVName<<endl;
	return txt.str();
}
//Check the coast model with the river model
void Hyd_Model_River::check_coast_modelby_interception(Hyd_Floodplain_Polygon *coastpolygon){
	//check if also the start of the river is inside
	if(coastpolygon->check_point_outside(&(this->inflow_river_profile.typ_of_profile->mid_point_global_x_y))==false){
		Error msg=this->set_error(17);
		ostringstream info;
		info << "Starting point of the river is inside the coastmodel"<< endl;
		info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
		info << "Rivername  : " << this->Param_RV.RVName << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	Geo_Interception_Point_List list;
	//check if there are interception with the rivermidline
	for(int j=0; j<this->number_inbetween_profiles;j++){
		list.delete_list();
		coastpolygon->calculate_segment_interception(&list,&this->river_midline.my_segment[j]);
		if(list.get_number_points()>0){
			Error msg=this->set_error(17);
			ostringstream info;
			info << "Riversegment "<< j << " is intercepting the coastline"<< endl;
			info << "Rivernumber: " << this->Param_RV.RVNumber << endl;
			info << "Rivername  : " << this->Param_RV.RVName << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Set the pointer to the upstream/downstream profile of the river profiles
void Hyd_Model_River::set_upstream_downstream_profiles(void){

	if(this->number_inbetween_profiles==0){
		this->inflow_river_profile.set_ptr_downstream_upstream_prof(&this->outflow_river_profile, NULL);
		this->outflow_river_profile.set_ptr_downstream_upstream_prof(NULL, &this->inflow_river_profile);
	}
	else{
		this->inflow_river_profile.set_ptr_downstream_upstream_prof(&(this->river_profiles[0]), NULL);

		for(int i=0; i< this->number_inbetween_profiles; i++){
			if(i==this->number_inbetween_profiles-1){
				this->river_profiles[i].set_ptr_downstream_upstream_prof(&this->outflow_river_profile, &(this->river_profiles[i-1]));
			}
			else{
				this->river_profiles[i].set_ptr_downstream_upstream_prof(&(this->river_profiles[i+1]), &(this->river_profiles[i-1]));
			}
		}
		this->outflow_river_profile.set_ptr_downstream_upstream_prof(NULL, &(this->river_profiles[number_inbetween_profiles-1]));

	}
}
//Set warning(s)
Warning Hyd_Model_River::set_warning(const int warn_type){
		string place="Hyd_Model_River::";
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
			info << "River model: " << this->Param_RV.RVName << "  No. " << this->Param_RV.RVNumber << endl;
			reaction="No reaction";
			type=12;	
			break;
		case 1://the distances given and geometrical do not correspond; the given one is smaller 
			place.append("check_river_model(void)");
			reason="The given distance between the profiles is half times smaller (tolerance) than the geometrial distances";
			help="Check the geometry of the river model";
			info << "River model: " << this->Param_RV.RVName<<"  No. "<< this->Param_RV.RVNumber << endl;
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
Error Hyd_Model_River::set_error(const int err_type){
		string place="Hyd_Model_River::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_river_profiles(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://geometry file is not open
			place.append("input_river_profiles_perfile(void)");
			reason="Can not open the file of river profiles";
			help="Check the file";
			info << "Filename is: " << this->Param_RV.RVGeomtryFile << endl;
			type=5;
			break;
		case 2://no zone found
			place.append("input_river_profiles_perfile(void)");
			reason="No Profile zone found";
			help="Check the file";
			info << "Filename is: " << this->Param_RV.RVGeomtryFile << endl;
			type=1;
			break;
		case 3://bad alloc
			place.append("allocate_instat_boundarycurves(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://not all curves wehere found by the connection
			place.append("connect_profiles2instat_boundarycurves(void)");
			reason="Not all boundary curves are found";
			help="Check the number of boundary curves in boundary file and in the main file";
			type=1;
			break;
		case 6://could not open the tecplot file
			place.append("output_result2csv_1d(const double timepoint, const int timestep_number)");
			reason = "Could not open the file for the paraview output of the river model";
			help = "Check the file";
			type = 5;
			break;
		case 7://could not open the  file
			place.append("output_geometrie2paraview_2d(void)");
			reason = "Could not open the file for the paraview output of the river model";
			help = "Check the file";
			type = 5;
			break;
		case 8://not all profiles are found
			place.append("calculate_distance_of_profiles(void)");
			reason="Not all profiles are found";
			help="Check the profile file and the setted number of profiles in the global file for this river number";
			type=15;
			break;
		case 10://no slope!
			place.append("calculate_deltaz_in2out(void)");
			reason="There has to be a slope from inflow to outflow";
			help="Check the minimum height of the inflow and outflow profile";
			type=15;
			break;
		case 11://could not open the tecplot file
			place.append("init_tecplot_output(void)");
			reason="Could not open the file for the tecplot output of the river model";
			help="Check the file";
			type=5;
			break;
		case 12://bad alloc
			place.append("init_river_geometrie(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 13://could not open the tecplot file
			place.append("output_geometrie2tecplot_2d(void)");
			reason="Could not open the file for the tecplot output (geometry) of the river model";
			help="Check the file";
			type=5;
			break;
		case 14://not all river profiles have a type
			place.append("count_profile_types(void)");
			reason="Not all river profiles have a type; there is a bug in the code";
			help="Check the river profiles generation";
			type=6;
			break;
		case 15://problem with the setting of the function to the solver
			place.append("set_function2solver(Hyd_Param_Global *global_params)");
			reason="Can not set the function to solve to the solver";
			help="Check the solver settings";
			type=17;
			break;
		case 16://there are multiple interception between river models
			place.append("check_river_modelsby_interception(Hyd_Model_River *check_river)");
			reason="There are multiple interception between the river midline and another polygon\n Interception are possible at the first or/and(diversion channel) last segment";
			help="Check the geometry of the river models";
			type=15;
			break;
		case 17://there are multiple interception with the coast model
			place.append("check_coast_modelby_interception(Hyd_Floodplain_Polygon *coastpolygon)");
			reason="There are not just the outflow profile in the coast model";
			help="Check the geometry of the river model and the coast model";
			type=15;
			break;
		case 18://the two river models are not equal
			place.append("compare_models(Hyd_Model_River *compare_model)");
			reason="The checked river models are not equal";
			help="Check the river models and ensure their equality";
			type=15;
			break;
		case 19://the number of profile do not match to the founded number in database
			place.append("set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database)");
			reason="The number of profile found in database do not equal to the given number";
			help="Check the table";
			type=2;;
			break;
		case 20://bad alloc
			place.append("allocate_part_profile_list(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 21://list setting is wrong
			place.append("set_part_profile_list(_Hyd_River_Profile *up, _Hyd_River_Profile *down)");
			reason="The list setting of a part of the profile pointer has an error";
			help="Check the method";
			type=6;
			break;
		case 22://bad alloc
			place.append("set_river_section_polygon(const int section_index)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 23://bad alloc
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, Hyd_Model_River *source, const _sys_system_id base)");
			reason="The number of profiles have to be equal";
			help="Check the number of profile in file";
			type=31;
			break;
		case 24://bad alloc
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, Hyd_Model_River *source, const _sys_system_id base)");
			reason="The riverstation of the inflow profiles have to be equal";
			help="Check the riverstation of inflow profile in file";
			type=31;
			break;
		case 25://bad alloc
			place.append("transfer_profile_members2database(QSqlDatabase *ptr_database, Hyd_Model_River *source, const _sys_system_id base)");
			reason="The riverstation of the outflow profiles have to be equal";
			help="Check the riverstation of outflow profile in file";
			type=31;
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
//___________________________________________
//static Main function for 1D diffusive wave modelling
#ifdef _WIN32
	int __cdecl f1D_equation2solve(realtype time, N_Vector results, N_Vector da_dt, void* river_data) {
#elif defined(__unix__) || defined(__unix)
	int __attribute__((cdecl)) f1D_equation2solve(realtype time, N_Vector results, N_Vector da_dt, void* river_data) {
#endif
	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	//cast the floodplain_data
	Hyd_Model_River *rv_data=(class Hyd_Model_River*)river_data;

	//connect the N_Vectors
	realtype *result_data=NULL;
	result_data=NV_DATA_S(results);

	realtype *dh_da_data=NULL;
	dh_da_data=NV_DATA_S(da_dt);

	//ostringstream out;
	//out << "   RV solver 1 "  << time<< endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);
	// Initialize da_dt with zero; not required
	//N_VConst(0.0, da_dt);

	
	//set the flow specific value of the inflow profile (the calculated area is given to the first profile of a segment)
	try{
		//check for upwards flow (just for an h-inflow boundary
		if(rv_data->inflow_river_profile.get_h_inflow_is_given()==true){
			if(rv_data->number_inbetween_profiles==0){
				//upwards flow
				if(rv_data->inflow_river_profile.get_global_h_boundary()<rv_data->outflow_river_profile.get_actual_global_waterlevel()){
					//mid of the boundary-h and the h of the first profile after the inflow
					double value =(rv_data->outflow_river_profile.get_actual_global_waterlevel()-rv_data->inflow_river_profile.typ_of_profile->get_global_z_min());
					value=(value+(rv_data->inflow_river_profile.get_global_h_boundary()-rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()))*0.5;
					value =rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()+value;
					rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel(value);

				}
				else{
					//full boundary-h is set
					rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel(rv_data->inflow_river_profile.get_global_h_boundary());
				}
			}
			else{
				//upwards flow
				if(rv_data->inflow_river_profile.get_global_h_boundary()<rv_data->river_profiles[0].get_actual_global_waterlevel() ){
					//mid of the boundary-h and the h of the first profile after the inflow
					double value =(rv_data->river_profiles[0].get_actual_global_waterlevel()-rv_data->inflow_river_profile.typ_of_profile->get_global_z_min());
					value=(value+(rv_data->inflow_river_profile.get_global_h_boundary()-rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()))*0.5;
					value =rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()+value;
					rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel(value);
				}
				else{
					//full boundary-h is set
					rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel(rv_data->inflow_river_profile.get_global_h_boundary());
				}
			}

		}
		//no h-boundary is set
		else{
			/*if(functions::check_infinite(result_data[0])==true){
				result_data[0]=0.0;
			}*/
			rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_table(result_data[0]);
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Inflow profile" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//out << "   RV solver 2 " << time << endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);
	
	//set the flow specific values for the inbetween profiles (the calculated area is given to the first profile of a segment)
	for(int i=0; i < rv_data->number_inbetween_profiles; i++){
		try{
			rv_data->river_profiles[i].typ_of_profile->set_actuel_profilevalues_by_table((result_data[i+1]));
			
		}
		catch(Error msg){
			ostringstream info;
			info << "Profilenumer    :" << i+1 << endl;
			info << "Riverstation    :" << rv_data->river_profiles[i].get_river_station() << label::m << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

	try{
		//check for upstream profiles local waterlevel for zero value and set for starting reason a 0.01 of the downstream waterlevel, if it is greater than z_min (starting reason for zero values)
		//set the flow specific values for the inbetween profiles (the calculated area is given to the first profile of a segment)
		//outflow
		if(rv_data->number_inbetween_profiles>0){
			if(rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && 
				rv_data->outflow_river_profile.get_actual_global_waterlevel()-rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min()>0.0 &&
				rv_data->outflow_river_profile.typ_of_profile->get_actual_local_waterlevel_h()> constant::dry_hyd_epsilon){
					if(rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min()>=rv_data->outflow_river_profile.typ_of_profile->get_global_z_min()){
						rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->set_actuel_profilevalues_by_waterlevel((rv_data->outflow_river_profile.get_actual_global_waterlevel()-
							rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min())*0.01+rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min());
					}
					else{
						rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->set_actuel_profilevalues_by_waterlevel(rv_data->outflow_river_profile.typ_of_profile->get_actual_local_waterlevel_h()*0.01+
							rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min());
					}
			}
		}
		//inbetween from down to up
		for(int i=rv_data->number_inbetween_profiles-1; i >=1 ; i--){
			if(rv_data->river_profiles[i-1].typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && 
				rv_data->river_profiles[i].get_actual_global_waterlevel()-rv_data->river_profiles[i-1].typ_of_profile->get_global_z_min()>0.0 &&
				rv_data->river_profiles[i].typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
					if(rv_data->river_profiles[i-1].typ_of_profile->get_global_z_min()>=rv_data->river_profiles[i].typ_of_profile->get_global_z_min()){
						rv_data->river_profiles[i-1].typ_of_profile->set_actuel_profilevalues_by_waterlevel((rv_data->river_profiles[i].get_actual_global_waterlevel()-
							rv_data->river_profiles[i-1].typ_of_profile->get_global_z_min())*0.01+rv_data->river_profiles[i-1].typ_of_profile->get_global_z_min());
					}
					else{
						rv_data->river_profiles[i-1].typ_of_profile->set_actuel_profilevalues_by_waterlevel(rv_data->river_profiles[i].typ_of_profile->get_actual_local_waterlevel_h()*0.01+
							rv_data->river_profiles[i-1].typ_of_profile->get_global_z_min());

					}
			}	
		}
		//inflow
		if(rv_data->number_inbetween_profiles>0){
			if(rv_data->inflow_river_profile.typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon  && 
				rv_data->river_profiles[0].get_actual_global_waterlevel()-rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()>0.0 &&
				rv_data->river_profiles[0].typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
					if(rv_data->inflow_river_profile.typ_of_profile->get_global_z_min()>=rv_data->river_profiles[0].typ_of_profile->get_global_z_min()){
						rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel((rv_data->river_profiles[0].get_actual_global_waterlevel()-
							rv_data->inflow_river_profile.typ_of_profile->get_global_z_min())*0.01+rv_data->inflow_river_profile.typ_of_profile->get_global_z_min());
					}
					else{
						rv_data->inflow_river_profile.typ_of_profile->set_actuel_profilevalues_by_waterlevel(rv_data->river_profiles[0].typ_of_profile->get_actual_local_waterlevel_h()*0.01+
							rv_data->inflow_river_profile.typ_of_profile->get_global_z_min());

					}
			}
		}
	}
	catch(Error msg){
		throw msg;
	}

	//out << "   RV solver 3 " << time << endl;
	//Sys_Common_Output::output_hyd->output_txt(&out);

	try{
		//set the actual river discharges
		//for the profile after the inflow profile
		if(rv_data->number_inbetween_profiles==0){//outprofile is next profile
			//´set the h for the outflow profile
			rv_data->outflow_river_profile.set_h_outflow_flow(rv_data->inflow_river_profile.typ_of_profile->get_global_z_min(),rv_data->inflow_river_profile.get_actual_global_waterlevel());
			//set the discharge through the outflow profile
			rv_data->outflow_river_profile.set_actual_river_discharge(&rv_data->inflow_river_profile);
		}
		else if(rv_data->number_inbetween_profiles==1){//riverprofile is the next profile; but then follows the outflow
			//if requiered use the normal flow approach for the calculation of the waterlevel for the outflow profile
			rv_data->outflow_river_profile.set_h_outflow_flow(rv_data->river_profiles[0].typ_of_profile->get_global_z_min(), rv_data->river_profiles[0].get_actual_global_waterlevel());
			//set the discharge for the only inbetween profile
			rv_data->river_profiles[0].set_actual_river_discharge(&(rv_data->inflow_river_profile), &(rv_data->outflow_river_profile));

			//set the discharge through the outflow profile
			rv_data->outflow_river_profile.set_actual_river_discharge(&(rv_data->river_profiles[0]));
		}
		else{
			//set the discharge for the first inbetween profile
			rv_data->river_profiles[0].set_actual_river_discharge(&(rv_data->inflow_river_profile), &(rv_data->river_profiles[1]));


			//set the discharge for the inbetween profiles
			for(int i=1; i< rv_data->number_inbetween_profiles-1; i++){
				rv_data->river_profiles[i].set_actual_river_discharge(&(rv_data->river_profiles[i-1]), &(rv_data->river_profiles[i+1]));
			}


			//last inbetween profile
			rv_data->river_profiles[rv_data->number_inbetween_profiles-1].set_actual_river_discharge(&(rv_data->river_profiles[rv_data->number_inbetween_profiles-2]),&(rv_data->outflow_river_profile));

			//if requiered use the normal flow approach for the calculation of the waterlevel for the outflow profile
			rv_data->outflow_river_profile.set_h_outflow_flow(rv_data->river_profiles[rv_data->number_inbetween_profiles-1].typ_of_profile->get_global_z_min(), rv_data->river_profiles[rv_data->number_inbetween_profiles-1].get_actual_global_waterlevel());
			
			
			//set the discharge through the outflow profile
			rv_data->outflow_river_profile.set_actual_river_discharge(&(rv_data->river_profiles[rv_data->number_inbetween_profiles-1]));

		}
	}
	catch(Error msg){
		throw msg;
	}

	//boundary discharges are setted by syncronisation
	try{
		//set the da_dt values for each river segment
		//set the inflow discharge
		if(rv_data->inflow_river_profile.get_h_inflow_is_given()==false){
			dh_da_data[0]=rv_data->inflow_river_profile.get_q_inflow();	
			//outprofile is next profile
			if(rv_data->number_inbetween_profiles==0){
				dh_da_data[0]=dh_da_data[0]+(-1.0)*rv_data->outflow_river_profile.get_calculated_river_discharge();
			}
			else{
				//discharge through profile for first segment
				dh_da_data[0]=dh_da_data[0]+(-1.0)*rv_data->river_profiles[0].get_actual_river_discharge();
			}
			dh_da_data[0]=rv_data->inflow_river_profile.get_total_boundary_coupling_discharge(time,dh_da_data[0]);

		}
		if(rv_data->number_inbetween_profiles!=0){
		
			//discharge through profile for last segment (!!!get actutal river discharge!!!!?)
			dh_da_data[rv_data->number_inbetween_profiles]=rv_data->river_profiles[rv_data->number_inbetween_profiles-1].get_actual_river_discharge()+(-1.0)*rv_data->outflow_river_profile.get_calculated_river_discharge();
			//dh_da_data[rv_data->number_inbetween_profiles] = rv_data->river_profiles[rv_data->number_inbetween_profiles - 1].get_actual_river_discharge() + (-1.0)*rv_data->outflow_river_profile.get_actual_river_discharge();
			dh_da_data[rv_data->number_inbetween_profiles]=rv_data->river_profiles[rv_data->number_inbetween_profiles-1].get_total_boundary_coupling_discharge(time,dh_da_data[rv_data->number_inbetween_profiles]);


			//for the inbetween profiles
			for(int i=1; i< rv_data->number_inbetween_profiles; i++){
				//discharge through profile for corresponding segment
				dh_da_data[i]=rv_data->river_profiles[i-1].get_actual_river_discharge()+(-1.0)*rv_data->river_profiles[i].get_actual_river_discharge();
				dh_da_data[i]=rv_data->river_profiles[i-1].get_total_boundary_coupling_discharge(time,dh_da_data[i]);
			}
		
		}

		//if a h-value is given as inflow boundary set the discharge out of the next downstream profile as inflow discharge
		if(rv_data->inflow_river_profile.get_h_inflow_is_given()==true ){
			rv_data->inflow_river_profile.set_q_inflow_interim(-1.0*dh_da_data[0], time);
			dh_da_data[0]=0.0;
		}

	
		//transform discharge [m³/s] into areas per second [m²/s] by dividing through the half segment length upstream/downstream
		dh_da_data[0]=dh_da_data[0]/(rv_data->inflow_river_profile.get_distance2downstream()*0.5);
		for(int i=0; i<rv_data->number_inbetween_profiles; i++){
			dh_da_data[i+1]=dh_da_data[i+1]/((rv_data->river_profiles[i].get_distance2upstream()+rv_data->river_profiles[i].get_distance2downstream())*0.5);
		}




		//calculate the hydrological balance for the outflow profile
		rv_data->outflow_river_profile.calculate_hydrological_balance_outflow(time);



		//calculate the stabilization discharge (numerical reasons)
		for(int i=0; i< rv_data->number_inbetween_profiles; i++){			
			rv_data->river_profiles[i].calculate_stabilization_discharge();			
		}
		rv_data->outflow_river_profile.calculate_stabilization_discharge();
		//add the stabilization discharge for wet dry flow (numerical reasons)
		dh_da_data[0]=dh_da_data[0]+rv_data->inflow_river_profile.get_stabilization_discharge(); 
		if(rv_data->number_inbetween_profiles==0){//outprofile is next profile
			dh_da_data[0]=dh_da_data[0]+rv_data->outflow_river_profile.get_stabilization_discharge();
		}
		else{
			for(int i=0; i< rv_data->number_inbetween_profiles; i++){
				if(i==0){
					dh_da_data[i]=dh_da_data[i]+rv_data->river_profiles[i].get_stabilization_discharge();
				}
				else{
				
					dh_da_data[i]=dh_da_data[i]+(-1.0)*rv_data->river_profiles[i-1].get_stabilization_discharge();
					dh_da_data[i]=dh_da_data[i]+rv_data->river_profiles[i].get_stabilization_discharge();
				}
			}

			//discharge through profile for last segment
			dh_da_data[rv_data->number_inbetween_profiles]=dh_da_data[rv_data->number_inbetween_profiles]+(-1.0)*rv_data->river_profiles[rv_data->number_inbetween_profiles-1].get_stabilization_discharge();
			dh_da_data[rv_data->number_inbetween_profiles]=dh_da_data[rv_data->number_inbetween_profiles]+rv_data->outflow_river_profile.get_stabilization_discharge();
		}
	}
	catch (Error msg) {
		throw msg;
	}
	return 0;
}
