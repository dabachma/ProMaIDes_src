#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2RV_Diversion.h"

//init static members
Tables *Hyd_Coupling_RV2RV_Diversion::diversion_channel_table=NULL;

//constructor
Hyd_Coupling_RV2RV_Diversion::Hyd_Coupling_RV2RV_Diversion(void){
	this->diversion_rv_model=NULL;
	this->index_diversion_rv_model=-1;
	this->coupling_struct=NULL;
	this->inflow_type=_hyd_1d_outflow_types::WEIR_OUT;
	this->lateral_flag_inflow=true;
	this->index=-1;
	this->index_inflow_rv=-1;
	this->index_outflow_rv=-1;
	this->outflow_river_flag=true;
	this->index_outflow_profile_upwards=-1;
	this->index_outflow_profile_down=-1;
	this->outflow_profile_up=NULL;
	this->outflow_profile_down=NULL;
	this->total_distance_outflow=0.0;
	this->distance_outflow_downwards=0.0;
	this->horizontal_backwater_flag_outflow=false;
	this->horizontal_backwater_flag_outflow_upstream=false;
	this->mid_fac_down_outflow=0.0;
	this->mid_fac_up_outflow=0.0;
	this->coupling_flag_outflow=false;
	this->left_flag_outflow=false;
	this->distance2river_begin_outflow=0.0;

	this->mid_height_inflow=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2RV_Diversion), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2RV_Diversion::~Hyd_Coupling_RV2RV_Diversion(void){
	if(this->coupling_struct!=NULL){
		delete this->coupling_struct;
		this->coupling_struct=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2RV_Diversion), _sys_system_modules::HYD_SYS);
}
//_______________
//public
//Input of the parameters per file via a parser Hyd_Parse_Coupling_Diversion
void Hyd_Coupling_RV2RV_Diversion::input_parameter_per_file(const int index, const string file){
	this->index=index;
	//set prefix for output
	ostringstream prefix;
	prefix << "DVC_" << this->index <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	Hyd_Parse_Coupling_Diversion parser;

	ostringstream cout;
	cout << "Read the diversion channel parameters from file " <<file << "..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	//set the file name 
	parser.set_input_filename(file);
	try{
		//open file
		parser.open_input_file();
		//read in the parameters
		parser.parse_params(this->index);
	}
	catch(Error msg){
		parser.close_input_file();
		throw msg;
	}

	//close the file
	parser.close_input_file();

	//set the diversion channel parameters
	this->set_diversion_channel_params(parser.get_params());
	//allocate the coupling structure
	this->allocate_coupling_structure();
	try{
		//set the data of the coupling structure
		this->coupling_struct->set_coupling_structure_data(parser.get_coupling_structure_data());
	}catch(Error msg){
		ostringstream info;
		info <<" Index of the diversion channel " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Input of the diversion channel parameters from a database selection
void Hyd_Coupling_RV2RV_Diversion::input_parameter_per_database(const QSqlTableModel *results, const int index, const bool output_flag){
	//set the index
	this->index=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_glob_id)).c_str()).toInt();

	if(output_flag==true){
		//set prefix for output
		ostringstream prefix;
		prefix << "DVC_" << this->index <<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		ostringstream cout;
		cout << "Read the diversion channel parameters from database..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	
	}

	//read out the results
	this->index_diversion_rv_model=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_rv_id)).c_str()).toInt();
	this->index_inflow_rv=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dvin_rv_id)).c_str()).toInt();
	this->index_outflow_rv=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dvout_rv_id)).c_str()).toInt();
	this->inflow_type=_Hyd_Coupling_Structure::convert_string2coupling_type(results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_type)).c_str()).toString().toStdString());

	try{
		//allocate the coupling structure
		this->allocate_coupling_structure();

		_hyd_coupling_structure_data buffer;
	
		buffer.controlled_flag=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_control)).c_str()).toBool();
		buffer.controlled_param=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_control_param)).c_str()).toDouble();
		buffer.width=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_width)).c_str()).toDouble();
		buffer.uncontrollable_param=results->record(index).value((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_uncontrol_param)).c_str()).toDouble();
		//set the data of the coupling structure
		this->coupling_struct->set_coupling_structure_data(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info <<"Index of the diversion channel : " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;

	}



	if(output_flag==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}



}
//Create the database table for the diversion channel data (static)
void Hyd_Coupling_RV2RV_Diversion::create_table(QSqlDatabase *ptr_database){
		if(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table==NULL){
			ostringstream cout;
			cout << "Create diversion channel data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_coup_dv_channel;
			const int num_col=13;
			_Sys_data_tab_column tab_col[num_col];


			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].primary_key_flag=false;
				tab_col[i].unsigned_flag=false;
			}
			tab_col[0].name=hyd_label::coup_dv_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=label::areastate_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].default_value="0";
			tab_col[1].key_flag=true;

			tab_col[2].name=label::measure_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].default_value="0";
			tab_col[2].key_flag=true;

			tab_col[3].name=label::applied_flag;
			tab_col[3].type=sys_label::tab_col_type_bool;
			tab_col[3].default_value="true";
			tab_col[3].key_flag=true;

			tab_col[4].name=hyd_label::coup_dv_rv_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=hyd_label::coup_dvin_rv_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=hyd_label::coup_dvout_rv_id;
			tab_col[6].type=sys_label::tab_col_type_int;

			tab_col[7].name=hyd_label::struct_hyd_type;
			tab_col[7].type=sys_label::tab_col_type_string;

			tab_col[8].name=hyd_label::struct_hyd_width;
			tab_col[8].type=sys_label::tab_col_type_double;

			tab_col[9].name=hyd_label::struct_hyd_control;
			tab_col[9].type=sys_label::tab_col_type_bool;
			tab_col[9].default_value="false";

			tab_col[10].name=hyd_label::struct_hyd_control_param;
			tab_col[10].type=sys_label::tab_col_type_double;

			tab_col[11].name=hyd_label::struct_hyd_uncontrol_param;
			tab_col[11].type=sys_label::tab_col_type_double;

			tab_col[12].name=label::description;
			tab_col[12].type=sys_label::tab_col_type_string;


			try{
				Hyd_Coupling_RV2RV_Diversion::diversion_channel_table= new Tables();
				if(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Coupling_RV2RV_Diversion::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Coupling_RV2RV_Diversion::close_table();
				throw msg;
			}

		Hyd_Coupling_RV2RV_Diversion::close_table();
	}
}
//Set the database table for the diversion channel data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Coupling_RV2RV_Diversion::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_coup_dv_channel;
		string tab_id_col[13];
		tab_id_col[0]=hyd_label::coup_dv_glob_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=hyd_label::coup_dv_rv_id;
		tab_id_col[4]=hyd_label::coup_dvin_rv_id;
		tab_id_col[5]=hyd_label::coup_dvout_rv_id;
		tab_id_col[6]=hyd_label::struct_hyd_type;
		tab_id_col[7]=hyd_label::struct_hyd_control_param;
		tab_id_col[8]=hyd_label::struct_hyd_width;
		tab_id_col[9]=hyd_label::struct_hyd_control;
		tab_id_col[10]=hyd_label::struct_hyd_uncontrol_param;
		tab_id_col[11]=label::description;
		tab_id_col[12]=label::applied_flag;

		try{
			Hyd_Coupling_RV2RV_Diversion::diversion_channel_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Coupling_RV2RV_Diversion::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Coupling_RV2RV_Diversion::close_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the diversion channel data (static)
void Hyd_Coupling_RV2RV_Diversion::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2RV_Diversion::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the diversion channel data (static)
void Hyd_Coupling_RV2RV_Diversion::close_table(void){
	if(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table!=NULL){
		delete Hyd_Coupling_RV2RV_Diversion::diversion_channel_table;
		Hyd_Coupling_RV2RV_Diversion::diversion_channel_table=NULL;
	}
}
//Select and count the number of relevant diversion channels in a database table (static)
int Hyd_Coupling_RV2RV_Diversion::select_relevant_dv_channels_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output){
	int number=0;

	try{
		Hyd_Coupling_RV2RV_Diversion::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream prefix;
			prefix << "DVC> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			ostringstream cout;
			cout << "Search for relevant diversion channels in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

		//give the table name to the query model
	results->setTable((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) " ;
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2RV_Diversion::select_relevant_dv_channels_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_glob_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant diversion channel(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	return number;

}
//Transfer the diversion channel data of a file into a database table
void Hyd_Coupling_RV2RV_Diversion::transfer_diversion_channel2database_table(QSqlDatabase *ptr_database){
	//set prefix for output
	ostringstream prefix;
	prefix << "DVC_" << this->index <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout <<"Transfer diversion channel data to database ..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2RV_Diversion::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->maximum_int_of_column(Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_glob_id),ptr_database)+1;;
	//insert row
	model.insertRows(0, 1);						
																																				
	//set data
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_glob_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(label::applied_flag)).c_str())),true);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dv_rv_id)).c_str())),this->index_diversion_rv_model);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dvin_rv_id)).c_str())),this->index_inflow_rv);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::coup_dvout_rv_id)).c_str())),this->index_outflow_rv);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_type)).c_str())), _Hyd_Coupling_Structure::convert_coupling_type2string(this->inflow_type).c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_control_param)).c_str())),this->coupling_struct->get_coupling_structure_data().controlled_param);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_width)).c_str())),this->coupling_struct->get_coupling_structure_data().width);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_control)).c_str())),this->coupling_struct->get_coupling_structure_data().controlled_flag);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_column_name(hyd_label::struct_hyd_uncontrol_param)).c_str())),this->coupling_struct->get_coupling_structure_data().uncontrollable_param);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Coupling_RV2RV_Diversion::diversion_channel_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Global Id                 : " << id_glob <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Convert the indices of the coupled models into pointer
void Hyd_Coupling_RV2RV_Diversion::convert_indices2pointer(Hyd_Model_River *river_models, const int number_rv_models){

	//search for the diversion river model
	if(this->index_diversion_rv_model<0 || this->index_diversion_rv_model>=number_rv_models){
		Error msg=this->set_error(2);
		throw msg;
	}
	this->diversion_rv_model=&(river_models[this->index_diversion_rv_model]);
	this->diversion_rv_model->set_diversion_channel_flag(true);
	//inflow
	if(this->index_inflow_rv<0 || this->index_inflow_rv>=number_rv_models || this->index_inflow_rv==this->index_diversion_rv_model){
		Error msg=this->set_error(3);
		throw msg;
	}
	this->inflow_rv_model=&(river_models[this->index_inflow_rv]);
	//outflow
	if(this->index_outflow_rv<0 || this->index_outflow_rv==this->index_diversion_rv_model){
		this->outflow_river_flag=false;
	}
	else{
		this->outflow_river_flag=true;
		this->outflow_rv_model=&(river_models[this->index_outflow_rv]);
	}
}
//The couplings are initialized
void Hyd_Coupling_RV2RV_Diversion::init_coupling(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "DVC> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow), ";
	cout << "diversion channel " << this->diversion_rv_model->Param_RV.get_river_number();
	if(this->outflow_river_flag==true){
		cout << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)" ;
	}
	cout << "..."<< endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{
		int river_section_index=-2;
		//set the inflow profile index
		river_section_index=this->inflow_rv_model->find_river_section_index_inside(true, this->diversion_rv_model->get_river_midline_first_section());
		if(river_section_index<0 || river_section_index>=this->inflow_rv_model->Param_RV.get_number_profiles()-1){
			Error msg=this->set_error(4);
			throw msg;
		}

		this->coupling_flag_inflow=true;

		if(river_section_index==0 && this->inflow_rv_model->Param_RV.get_number_profiles()>2){
			this->inflow_profile_up=&this->inflow_rv_model->inflow_river_profile;
			this->index_inflow_profile_upwards=0;
			this->inflow_profile_down=&(this->inflow_rv_model->river_profiles[0]);
			this->index_inflow_profile_down=1;
		}
		else if(river_section_index==0 && this->inflow_rv_model->Param_RV.get_number_profiles()==2){
			this->inflow_profile_up=&this->inflow_rv_model->inflow_river_profile;
			this->index_inflow_profile_upwards=0;
			this->inflow_profile_down=&this->inflow_rv_model->outflow_river_profile;
			this->index_inflow_profile_down=1;
		}
		else if(river_section_index>0 && river_section_index==this->inflow_rv_model->Param_RV.get_number_profiles()-2){
			this->inflow_profile_up=&(this->inflow_rv_model->river_profiles[this->inflow_rv_model->Param_RV.get_number_profiles()-3]);	
			this->index_inflow_profile_upwards=river_section_index;
			this->inflow_profile_down=&this->inflow_rv_model->outflow_river_profile;
			this->index_inflow_profile_down=river_section_index+1;
		}
		else{
			this->inflow_profile_up=&(this->inflow_rv_model->river_profiles[river_section_index-1]);	
			this->index_inflow_profile_upwards=river_section_index;
			this->inflow_profile_down=&(this->inflow_rv_model->river_profiles[river_section_index]);
			this->index_inflow_profile_down=river_section_index+1;
		}
				
		//set the distances
		this->calculate_distances_inflow(this->inflow_rv_model->get_river_section_polygon(this->index_inflow_profile_upwards), this->diversion_rv_model->get_river_midline_first_section());
		this->calculate_mid_factors();
		//set q inflow is given
		this->diversion_rv_model->inflow_river_profile.set_q_inflow_is_given();
		
		//set the global sill height
		this->mid_height_inflow=this->inflow_profile_down->typ_of_profile->get_global_z_min()*this->mid_fac_down_inflow+
			this->inflow_profile_up->typ_of_profile->get_global_z_min()*this->mid_fac_up_inflow;
		this->mid_height_inflow=max(this->mid_height_inflow, this->diversion_rv_model->inflow_river_profile.typ_of_profile->get_global_z_min());
		//set it to the structure
		this->coupling_struct->set_global_sill_height(this->mid_height_inflow);


		//set the outflow profile
		if(this->outflow_river_flag==true){
			river_section_index=-2;
			river_section_index=this->outflow_rv_model->find_river_section_index_inside(false, this->diversion_rv_model->get_river_midline_last_section());
			//check it
			if(river_section_index<0 || river_section_index>=this->outflow_rv_model->Param_RV.get_number_profiles()-1){
				Error msg=this->set_error(5);
				throw msg;
			}
			this->coupling_flag_outflow=true;


			if(river_section_index==0 && this->outflow_rv_model->Param_RV.get_number_profiles()>2){
				this->outflow_profile_up=&this->outflow_rv_model->outflow_river_profile;
				this->index_outflow_profile_upwards=0;
				this->outflow_profile_down=&(this->outflow_rv_model->river_profiles[0]);
				this->index_outflow_profile_down=1;
			}
			else if(river_section_index==0 && this->outflow_rv_model->Param_RV.get_number_profiles()==2){
				this->outflow_profile_up=&this->outflow_rv_model->outflow_river_profile;
				this->index_outflow_profile_upwards=0;
				this->outflow_profile_down=&this->outflow_rv_model->outflow_river_profile;
				this->index_outflow_profile_down=1;
			}
			else if(river_section_index>0 && river_section_index==this->outflow_rv_model->Param_RV.get_number_profiles()-2){
				this->outflow_profile_up=&(this->outflow_rv_model->river_profiles[this->outflow_rv_model->Param_RV.get_number_profiles()-3]);	
				this->index_outflow_profile_upwards=river_section_index;
				this->outflow_profile_down=&this->outflow_rv_model->outflow_river_profile;
				this->index_outflow_profile_down=river_section_index+1;
			}
			else{
				this->outflow_profile_up=&(this->outflow_rv_model->river_profiles[river_section_index-1]);	
				this->index_outflow_profile_upwards=river_section_index;
				this->outflow_profile_down=&(this->outflow_rv_model->river_profiles[river_section_index]);
				this->index_outflow_profile_down=river_section_index+1;
			}
			//set the distances of outflow
			this->calculate_distances_outflow(this->outflow_rv_model->get_river_section_polygon(this->index_outflow_profile_upwards), this->diversion_rv_model->get_river_midline_last_section());
			this->calculate_mid_factors_outflow();
			//set h is given to the diversion channel
			this->diversion_rv_model->outflow_river_profile.set_h_outflow_is_given();
			//check it
			this->check_connection();
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow), ";
		info << "diversion channel " << this->diversion_rv_model->Param_RV.get_river_number();
		if(this->outflow_river_flag==true){
			info  << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)" ;
		}
		info  <<  endl ;	
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Synchronise the models
void Hyd_Coupling_RV2RV_Diversion::synchronise_models(void){
	try{
		//inflow of the channel
		if(this->coupling_flag_inflow==true){
			//inflow to the diversion channel
			double value_h1=0.0;
			double value_h2=0.0;
			double discharge=0.0;
			value_h1=this->calculate_mid_waterlevel();
			value_h2=this->diversion_rv_model->inflow_river_profile.get_actual_global_waterlevel();

			//calculate the discharge over the coupling structure
			discharge=this->coupling_struct->get_discharge(value_h1, value_h2);

			value_h1 = value_h2;

			if(this->horizontal_backwater_flag_inflow==false && this->horizontal_backwater_flag_inflow_upstream==false){
				this->inflow_profile_up->add_coupling_discharge_1d(discharge*this->mid_fac_up_inflow);
				this->inflow_profile_down->add_coupling_discharge_1d(discharge*this->mid_fac_down_inflow);
			}
			else if(this->horizontal_backwater_flag_inflow==true){
				this->inflow_profile_down->add_coupling_discharge_1d(discharge);
			}
			else if(this->horizontal_backwater_flag_inflow_upstream==true){
				this->inflow_profile_up->add_coupling_discharge_1d(discharge);
			}
			//set the discharge to the diversion channel
			this->diversion_rv_model->inflow_river_profile.add_q_inflow_coupling((-1.0)*discharge);

		}
		//outflow of the channel
		if(this->coupling_flag_outflow==true){
			double value_h1=0.0;
			double discharge=0.0;
			//get the discharge
			discharge=this->diversion_rv_model->outflow_river_profile.get_actual_river_discharge();
			//set the discharge
			if(this->horizontal_backwater_flag_outflow==false && this->horizontal_backwater_flag_outflow_upstream==false){
				this->outflow_profile_up->add_coupling_discharge_1d(discharge*this->mid_fac_up_outflow);
				this->outflow_profile_down->add_coupling_discharge_1d(discharge*this->mid_fac_down_outflow);
			}
			else if(this->horizontal_backwater_flag_outflow==true){
				this->outflow_profile_down->add_coupling_discharge_1d(discharge);
			}
			else if(this->horizontal_backwater_flag_outflow_upstream==true){
				this->outflow_profile_up->add_coupling_discharge_1d(discharge);
			}
			//get the waterlevel
			value_h1=this->calculate_mid_waterlevel_outflow();
			//set it to the diversion channel
			this->diversion_rv_model->outflow_river_profile.set_h_outflow_global(value_h1);
		}		
	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow), ";
		info << "diversion channel " << this->diversion_rv_model->Param_RV.get_river_number();
		if(this->outflow_river_flag==true){
			info  << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)"; 
		}
		info  << endl ;	
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reset the coupling discharges
void Hyd_Coupling_RV2RV_Diversion::reset_coupling_discharges(void){
	if(this->coupling_flag_inflow==true){
		//reset the inflow discharge
		this->inflow_profile_up->reset_coupling_discharge_1d();
		this->inflow_profile_down->reset_coupling_discharge_1d();
		this->diversion_rv_model->inflow_river_profile.reset_q_inflow_coupling();
	}
	//reset the outflow discharge
	if(this->coupling_flag_outflow==true){
		this->outflow_profile_down->reset_coupling_discharge_1d();
		this->outflow_profile_up->reset_coupling_discharge_1d();
	}
}
//Output the members
void Hyd_Coupling_RV2RV_Diversion::output_members(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "DVC_"<<this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout <<"RIVER" <<endl;
	cout << " River model number inflow                  : " << W(7) << this->index_inflow_rv << endl;
	cout << " River model number dv-channel              : " << W(7) << this->index_diversion_rv_model << endl;
	if(this->index_outflow_rv>=0){
		cout << " River model number outflow                 : " << W(7) << this->index_outflow_rv << endl;
	}
	else{
		cout << " River model number outflow                 : " << W(7) << label::not_set << endl;
	}
	cout <<"STRUCTURE" << endl;
	cout <<" Type                                        : " << W(7) << _Hyd_Coupling_Structure::convert_coupling_type2string(this->inflow_type)<<endl;
	this->coupling_struct->output_members(&cout);


	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_RV2RV_Diversion::output_header_coupled_indices(ostringstream *cout){
		*cout << "River to river as diversion channel..." << endl ;
		*cout << "No." << W(12) << "Id_RVin" << W(15)<< "Id_RVprofile" << W(13) << "z_mid_in" << label::m;
		*cout << "Left_bank_in"<< W(15) << "Dist_begin_in" << label::m << W(15); 
		*cout << W(15) << "Id_Div_channel" << W(12) << "Id_RVout"<< W(15) << "Id_RVprofile" << W(13)<< "z_mid_out" << label::m;
		*cout << W(3) << "Left_bank_out"<< W(16) << "Dist_begin_out" << label::m << endl;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indexes of the coupled model
void Hyd_Coupling_RV2RV_Diversion::output_index_coupled_models(ostringstream *cout, const int number){

	*cout << number << W(12) << P(1) << FORMAT_FIXED_REAL;
	if(this->coupling_flag_inflow==true){
		*cout << this->index_inflow_rv<< W(15);
		*cout << this->index_inflow_profile_upwards << W(15);
		*cout << this->mid_height_inflow<< W(15);
		*cout << functions::convert_boolean2string(this->left_flag_inflow) << W(15);
		*cout << this->distance2river_begin_inflow << W(15) ;
		*cout << this->index_diversion_rv_model <<W(15);
		if(this->coupling_flag_outflow==true){
			*cout << this->index_outflow_rv << W(15);
			*cout << this->index_outflow_profile_upwards<<W(15)<< this->mid_height <<W(15) ;
			*cout << functions::convert_boolean2string(this->left_flag_outflow) << W(15);
			*cout << this->distance2river_begin_outflow <<  endl;
		}
		else{
			*cout <<"No outflow coupling is applied" << endl;
		}

	}
	else{
		*cout <<"No coupling is applied; see the warnings!" << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(cout);
}
///Insert the junction to a list of RV2FP-coupling points at inflow profile of the diversion channel
void Hyd_Coupling_RV2RV_Diversion::insert_junction_inflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index){

	if(this->coupling_flag_inflow==false){
		return;
	}
	if(list->get_leftriver_bank_line()!=this->left_flag_inflow){
		return;
	}
	if(river_index<0){
		return;
	}
	if(river_index!=this->inflow_rv_model->Param_RV.get_river_number()){
		return;
	}
	try{
		//set the coupling point
		if (list->find_coupling_point(this->distance2river_begin_inflow, &this->inflow_point) == NULL) {
			return;
		};
		//set the distance
		this->inflow_point.set_total_distance_along_polysegment(this->distance2river_begin_inflow);
		//it is the right list=> insert the junction
		list->insert_constricting_coupling_points(this->diversion_rv_model->inflow_river_profile.typ_of_profile->get_width_end_start_profile(), &this->inflow_point);
	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow), ";
		info << "diversion channel " << this->diversion_rv_model->Param_RV.get_river_number();
		if(this->outflow_river_flag==true){
			info  << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)"; 
		}
		info  <<  endl ;
		info << "Try to insert the coupling point of the inflow profile of the diversion channel to the rv2fp-list of the inflow river" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
///Insert the junction to a list of RV2FP-coupling points at outflow profile of the diversion channel
void Hyd_Coupling_RV2RV_Diversion::insert_junction_outflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index){
	if(this->coupling_flag_outflow==false){
		return;
	}
	if(list->get_leftriver_bank_line()!=this->left_flag_inflow){
		return;
	}
	if(river_index<0){
		return;
	}
	if(river_index!=this->outflow_rv_model->Param_RV.get_river_number()){
		return;
	}
	try{
		//set the coupling point
		if (list->find_coupling_point(this->distance2river_begin_outflow, &this->outflow_point) == NULL) {
			return;
		};
		this->inflow_point.set_total_distance_along_polysegment(this->distance2river_begin_outflow);
		//it is the right list=> insert the junction
		list->insert_constricting_coupling_points(this->diversion_rv_model->outflow_river_profile.typ_of_profile->get_width_end_start_profile(), &this->outflow_point);

	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow), ";
		info << "diversion channel " << this->diversion_rv_model->Param_RV.get_river_number();
		if(this->outflow_river_flag==true){
			info  << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)"; 
		}
		info  << endl ;	
		info << "Try to insert the coupling point of the outflow profile of the diversion channel to the rv2fp-list of the outflow river" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}

//Clone the river to river couplings via diversion channel
void Hyd_Coupling_RV2RV_Diversion::clone_couplings(Hyd_Coupling_RV2RV_Diversion *coupling, Hyd_Hydraulic_System *system){
	Hyd_Coupling_RV2RV::clone_couplings(coupling, system);
	this->system_id=coupling->get_sys_system_id();
	this->index_diversion_rv_model=coupling->index_diversion_rv_model;
	
	this->inflow_type=coupling->inflow_type;
	this->lateral_flag_inflow=coupling->lateral_flag_inflow;
	this->index=coupling->index;
	this->index_inflow_rv=coupling->index_inflow_rv;
	this->index_outflow_rv=coupling->index_outflow_rv;
	this->outflow_river_flag=coupling->outflow_river_flag;
	this->index_outflow_profile_upwards=coupling->index_outflow_profile_upwards;
	this->index_outflow_profile_down=coupling->index_outflow_profile_down;
	
	
	this->total_distance_outflow=coupling->total_distance_outflow;
	this->distance_outflow_downwards=coupling->distance_outflow_downwards;
	this->horizontal_backwater_flag_outflow=coupling->horizontal_backwater_flag_outflow;
	this->horizontal_backwater_flag_outflow_upstream=coupling->horizontal_backwater_flag_outflow_upstream;
	this->mid_fac_down_outflow=coupling->mid_fac_down_outflow;
	this->mid_fac_up_outflow=coupling->mid_fac_up_outflow;
	this->coupling_flag_outflow=coupling->coupling_flag_outflow;
	this->left_flag_outflow=coupling->left_flag_outflow;
	this->distance2river_begin_outflow=coupling->distance2river_begin_outflow;
	this->mid_height_inflow=coupling->mid_height_inflow;

	this->outflow_point=coupling->outflow_point;

	if(coupling->diversion_rv_model!=NULL){
		this->diversion_rv_model=system->get_ptr_river_model(coupling->diversion_rv_model->Param_RV.get_river_number());
	}
	if(this->outflow_rv_model!=NULL){
		this->outflow_profile_up=this->outflow_rv_model->get_ptr_river_profile(this->index_outflow_profile_upwards);
		this->outflow_profile_down=this->outflow_rv_model->get_ptr_river_profile(this->index_outflow_profile_down);
	}

	this->allocate_coupling_structure();
	this->coupling_struct->clone_coupling_structure(coupling->coupling_struct);

}
//_______________
//private
///Allocate the given coupling structure type
void Hyd_Coupling_RV2RV_Diversion::allocate_coupling_structure(void){
	try{

		switch(this->inflow_type){
			case _hyd_1d_outflow_types::GATE_OUT:
				this->coupling_struct=new Hyd_Coupling_Structure_Gate;
				break;
			case _hyd_1d_outflow_types::WEIR_OUT:
				this->coupling_struct=new Hyd_Coupling_Structure_Weir;
				break;
			default:
				Error msg=this->set_error(1);
				throw msg;
		}
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}

}
//Set the diversion channel member with a structure where the parser has gathered the informations
void Hyd_Coupling_RV2RV_Diversion::set_diversion_channel_params(_hyd_div_channel_params parameter){
	this->index_diversion_rv_model=parameter.rv_model_no;
	this->index_inflow_rv=parameter.rv_inflow_no;
	this->index_outflow_rv=parameter.rv_outflow_no;
	this->inflow_type=parameter.type;
}
//Calculate the distance to the inflow profiles via an interception of a midline segment of one river section and a river section polygon
void Hyd_Coupling_RV2RV_Diversion::calculate_distances_inflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline){
	Geo_Interception_Point_List list;
	//there must be one or two interception point
	river_section_polygon->calculate_segment_interception(&list, river_section_midline);
	list.sort_distance();
	_geo_multisegment_interception key_point=list.get_last_point();
	

	//point is intercepted
	//upwards profile
	if(key_point.index_point_is_intercepted==0 || key_point.index_point_is_intercepted==3){
		if(key_point.index_point_is_intercepted==0){
			this->total_distance_inflow=river_section_polygon->get_segment(0)->get_distance();
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_inflow=true;
		}
		else{
			this->total_distance_inflow=river_section_polygon->get_segment(2)->get_distance();
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_inflow=false;

		}
		this->distance_inflow_downwards=this->total_distance_inflow;
	}
	//downwards profile
	else if(key_point.index_point_is_intercepted==1 || key_point.index_point_is_intercepted==2){
		
		if(key_point.index_point_is_intercepted==1){
			this->left_flag_inflow=true;
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_inflow=river_section_polygon->get_segment(0)->get_distance();
		}
		else{
			this->left_flag_inflow=false;
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_inflow=river_section_polygon->get_segment(2)->get_distance();
		}
		this->distance_inflow_downwards=0.0;
	}

	//segment is intercepted
	//upstream
	else if(key_point.index_is_intercept==0){
		//decide which side
		Geo_Interception_Point_List list1;
		_geo_interception_point point1;
		//left side
		point1=this->inflow_rv_model->river_leftline.calc_first_interception(river_section_midline);
		if(point1.interception_flag==true){
			this->left_flag_inflow=true;
			this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
			this->distance_inflow_downwards=this->total_distance_inflow;	
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		}
		else{
			//right side
			point1=this->inflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
			this->left_flag_inflow=false;
			this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
			this->distance_inflow_downwards=this->total_distance_inflow;	
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		}
	}
	//downstream
	else if(key_point.index_is_intercept==2){
		//decide which side
		Geo_Interception_Point_List list1;
		_geo_interception_point point1;
		//left side
		point1=this->inflow_rv_model->river_leftline.calc_first_interception(river_section_midline);
		if(point1.interception_flag==true){
			this->left_flag_inflow=true;
			this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
			this->distance_inflow_downwards=0.0;	
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		}
		else{
			//right side
			point1=this->inflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
			this->left_flag_inflow=false;
			this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
			this->distance_inflow_downwards=0.0;	
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		}
	}
	//right bank
	else if(key_point.index_is_intercept==1){
		this->left_flag_inflow=false;
		this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
		this->distance_inflow_downwards=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));	
	}
	//left bank
	else if(key_point.index_is_intercept==3){
		this->left_flag_inflow=true;
		this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
		this->distance_inflow_downwards=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));
	}

	//check if the upwards profile is a weir profile
	if(this->inflow_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		this->total_distance_inflow=1.0;
		this->distance_inflow_downwards=0.0;
	}
	if(this->inflow_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		this->total_distance_inflow=1.0;
		this->distance_inflow_downwards=1.0;
	}
}

//Calculate the distance to the outflow profiles via an interception of a midline segment of one river section and a river section polygon
void Hyd_Coupling_RV2RV_Diversion::calculate_distances_outflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline){
	Geo_Interception_Point_List list;
	//there must be one or two interception point
	river_section_polygon->calculate_segment_interception(&list, river_section_midline);
	list.sort_distance();
	_geo_multisegment_interception key_point=list.get_first_point();
	

	//point is intercepted
	//upwards profile
	if(key_point.index_point_is_intercepted==0 || key_point.index_point_is_intercepted==3){
		if(key_point.index_point_is_intercepted==0){
			this->total_distance_outflow=river_section_polygon->get_segment(0)->get_distance();
			this->distance2river_begin_outflow=this->outflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_outflow=true;
		}
		else{
			this->total_distance_outflow=river_section_polygon->get_segment(2)->get_distance();
			this->distance2river_begin_outflow=this->outflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_outflow=false;

		}
		this->distance_outflow_downwards=this->total_distance_outflow;
	}
	//downwards profile
	else if(key_point.index_point_is_intercepted==1 || key_point.index_point_is_intercepted==2){
		
		if(key_point.index_point_is_intercepted==1){
			this->left_flag_outflow=true;
			this->distance2river_begin_outflow=this->outflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_outflow=river_section_polygon->get_segment(0)->get_distance();
		}
		else{
			this->left_flag_outflow=false;
			this->distance2river_begin_outflow=this->outflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_outflow=river_section_polygon->get_segment(2)->get_distance();
		}
		this->distance_outflow_downwards=0.0;
	}

	//segment is intercepted
	//upstream
	else if(key_point.index_is_intercept==3){
		//decide which side
		Geo_Interception_Point_List list1;
		_geo_interception_point point1;
		//left side
		point1=this->outflow_rv_model->river_leftline.calc_last_interception(river_section_midline);
		if(point1.interception_flag==true){
			this->left_flag_outflow=true;
			this->total_distance_outflow=this->outflow_rv_model->river_leftline.my_segment[this->index_outflow_profile_upwards].get_distance();
			this->distance_outflow_downwards=this->total_distance_outflow;	
			this->distance2river_begin_outflow=this->outflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		}
		else{
			//right side
			point1=this->outflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
			this->left_flag_outflow=false;
			this->total_distance_outflow=this->outflow_rv_model->river_rightline.my_segment[this->index_outflow_profile_upwards].get_distance();
			this->distance_outflow_downwards=this->total_distance_outflow;	
			this->distance2river_begin_outflow=this->outflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		}
	}
	//downstream
	else if(key_point.index_is_intercept==1){
		//decide which side
		Geo_Interception_Point_List list1;
		_geo_interception_point point1;
		//left side
		point1=this->outflow_rv_model->river_leftline.calc_last_interception(river_section_midline);
		if(point1.interception_flag==true){
			this->left_flag_outflow=true;
			this->total_distance_outflow=this->outflow_rv_model->river_leftline.my_segment[this->index_outflow_profile_upwards].get_distance();
			this->distance_outflow_downwards=0.0;	
			this->distance2river_begin_outflow=this->outflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		}
		else{
			//right side
			point1=this->outflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
			this->left_flag_outflow=false;
			this->total_distance_outflow=this->outflow_rv_model->river_rightline.my_segment[this->index_outflow_profile_upwards].get_distance();
			this->distance_outflow_downwards=0.0;	
			this->distance2river_begin_outflow=this->outflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		}
	}
	//right bank
	else if(key_point.index_is_intercept==2){
		this->left_flag_outflow=false;
		this->distance2river_begin_outflow=this->outflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_outflow=this->outflow_rv_model->river_rightline.my_segment[this->index_outflow_profile_upwards].get_distance();
		this->distance_outflow_downwards=this->outflow_rv_model->river_rightline.my_segment[this->index_outflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));	
	}
	//left bank
	else if(key_point.index_is_intercept==0){
		this->left_flag_outflow=true;
		this->distance2river_begin_outflow=this->outflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_outflow=this->outflow_rv_model->river_leftline.my_segment[this->index_outflow_profile_upwards].get_distance();
		this->distance_outflow_downwards=this->outflow_rv_model->river_leftline.my_segment[this->index_outflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));
	}

	//check if the upwards profile is a weir profile
	if(this->outflow_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		this->total_distance_outflow=1.0;
		this->distance_outflow_downwards=0.0;
	}
	if(this->outflow_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		this->total_distance_outflow=1.0;
		this->distance_outflow_downwards=1.0;
	}
}

//Check the connection of the rivers; the global z_min coordinate if the diversion channel´s outflow profile have to be greater/equal to the mid z_min coordinate of the profiles or the outflow river
void Hyd_Coupling_RV2RV_Diversion::check_connection(void){

	this->mid_height=this->outflow_profile_down->typ_of_profile->get_global_z_min()*this->mid_fac_down_outflow+
		this->outflow_profile_up->typ_of_profile->get_global_z_min()*this->mid_fac_up_outflow;
	//check it
	if(this->diversion_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()< this->mid_height ){
		Error msg=this->set_error(6);
		ostringstream info;
		info << "Global z_min diversion channel : " <<this->diversion_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()<< label::m <<endl;
		info << "Global z_min outflow profile   : " <<this->mid_height<< label::m <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Calculate the current mid-waterlevel via interpolation in the outflow river
double Hyd_Coupling_RV2RV_Diversion::calculate_mid_waterlevel_outflow(void){
	if(this->coupling_flag_outflow==false){
		return 0.0;
	}
	double value=0.0;
	this->horizontal_backwater_flag_outflow=false;
	horizontal_backwater_flag_outflow_upstream=false;
	
	if(this->outflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->outflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
		value=this->outflow_profile_down->get_actual_global_waterlevel()*this->mid_fac_down_outflow+
			this->outflow_profile_up->get_actual_global_waterlevel()*this->mid_fac_up_outflow;
	}
	else if(this->outflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->outflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
		value=this->outflow_profile_up->get_actual_global_waterlevel()*this->mid_fac_up_outflow;
		horizontal_backwater_flag_outflow_upstream=true;
	}
	else if(this->outflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->outflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
		value=this->outflow_profile_down->get_actual_global_waterlevel();
		this->horizontal_backwater_flag_outflow=true;
	}
	else if(this->outflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->outflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
		value=this->outflow_profile_down->typ_of_profile->get_global_z_min()*this->mid_fac_down_outflow+
			this->outflow_profile_up->typ_of_profile->get_global_z_min()*this->mid_fac_up_outflow;
	}
	
	return value;
}

//Calculate the mid factors for the interpolation of the waterlevel in the outflow river
void Hyd_Coupling_RV2RV_Diversion::calculate_mid_factors_outflow(void){	
	this->mid_fac_down_outflow=(1.0-this->distance_outflow_downwards/this->total_distance_outflow);
	this->mid_fac_up_outflow=(this->distance_outflow_downwards/this->total_distance_outflow);
}
//Set error(s)
Error Hyd_Coupling_RV2RV_Diversion::set_error(const int err_type){
	string place="Hyd_Coupling_RV2RV_Diversion::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_coupling_structure(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://unkown type of a 1d coutpling structure
			place.append("allocate_coupling_structure(void)");
			reason="The type of the 1d coupling structure is not known";
			help="Check the coupling structure type";
			info << "Index of the diversion channel coupling " << this->index << endl;
			type=16;
			break;
		case 2://Index of the river model representing the diversion channel is not known
			place.append("convert_indices2pointer(Hyd_Model_River *river_models, const int number_rv_models)");
			reason="The index of the river model representing the diversion channel is not known";
			help="Check the coupling parameters";
			info << "Index of the diversion channel coupling " << this->index << endl;
			type=16;
			break;
		case 3://Index of the inflow river model  is not known
			place.append("convert_indices2pointer(Hyd_Model_River *river_models, const int number_rv_models)");
			reason="The index of the inflow river model is not known or is similar to the index of the river model representing the diversion channel";
			help="Check the coupling parameters";
			info << "Index of the diversion channel coupling " << this->index << endl;
			type=16;
			break;
		case 4://The inflow profile of the diversion channel do not intercept the inflow river model
			place.append("init_coupling(void)");
			reason="The inflow profile of the diversion channel do not intercept the inflow river model";
			help="Check the coupling parameters";
			info << "Index of the diversion channel coupling " << this->index << endl;
			type=16;
			break;
		case 5://The outflow profile of the diversion channel do not intercept the outflow river model
			place.append("init_coupling(void)");
			reason="The outflow profile of the diversion channel do not intercept the outflow river model";
			help="Check the coupling parameters";
			info << "Index of the diversion channel coupling " << this->index << endl;
			type=16;
			break;
		case 6://The outflow profile of the diversion channel do not intercept the outflow river model
			place.append("check_connection(void)");
			reason="The global z-min coordinate of the outflow profile (diversion channel) have to be >= than the inflow profile z-min coordinate";
			help="Check the coupling profiles";
			info << "Index river profile inflow river: " << this->index_outflow_profile_upwards << endl;
			type=16;
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
Warning Hyd_Coupling_RV2RV_Diversion::set_warning(const int warn_type){
		string place="Hyd_Coupling_RV2RV_Diversion::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_diversion_channel2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the diversion channel coupling data to the database";
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