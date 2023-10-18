#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2FP_Structure.h"

//init static members
Tables *Hyd_Coupling_RV2FP_Structure::coupling_structure_table=NULL;

//constructor
Hyd_Coupling_RV2FP_Structure::Hyd_Coupling_RV2FP_Structure(void){
	this->index=-1;
	this->index_rv_model=-1;
	this->index_fp_model=-1;
	this->left_flag=true;
	this->inflow_type=_hyd_1d_outflow_types::UNKNOWN_OUT;
	this->coupling_struct=NULL;
	this->index_related_profile=-1;
	this->distance_start_related=0.0;

	this->river_profile_down=NULL;
	this->river_profile_up=NULL;
	this->floodplain_elem=NULL;
	this->mid_height=0.0;
	this->mid_fac_up=0.0;
	this->mid_fac_down=0.0;
	this->mid_waterlevel=0.0;
	this->set_point_name(hyd_label::coupling_point_RV2FP_structure);

	this->horizontal_backwater_flag=false;
	this->horizontal_backwater_flag_upstream=false;

	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_counter=1.0;
	this->no_osci_counter=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2FP_Structure), _sys_system_modules::HYD_SYS);
}

//detructor
Hyd_Coupling_RV2FP_Structure::~Hyd_Coupling_RV2FP_Structure(void){
	if(this->coupling_struct!=NULL){
		delete this->coupling_struct;
		this->coupling_struct=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2FP_Structure), _sys_system_modules::HYD_SYS);
}
//___________
//public
//Input of the parameters per file via a parser Hyd_Parse_Coupling_Structure
void Hyd_Coupling_RV2FP_Structure::input_parameter_per_file(const int index, const string file){
	this->index=index;
	//set prefix for output
	ostringstream prefix;
	prefix << "SCC_" << this->index <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	Hyd_Parse_Coupling_RV2FP_Structure parser;

	ostringstream cout;
	cout << "Read the structure coupling parameters from file " <<file << "..." <<endl;
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
		ostringstream info;
		info <<"Index of the structure coupling " << this->index << endl;
		msg.make_second_info(info.str());
		parser.close_input_file();
		throw msg;
	}

	//close the file
	parser.close_input_file();

	try{
		//set the structure coupling parameters
		this->set_structure_coupling_params(parser.get_params());
		//allocate the coupling structure
		this->allocate_coupling_structure();
		
		//set the data of the coupling structure
		this->coupling_struct->set_coupling_structure_data(parser.get_coupling_structure_data());
	}
	catch(Error msg){
		ostringstream info;
		info <<"Index of the structure coupling : " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Input of the structure coupling parameters from a database selection
void Hyd_Coupling_RV2FP_Structure::input_parameter_per_database(const QSqlQueryModel *results, const int index, const bool output_flag){

	//set the index
	this->index=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id)).c_str()).toInt();

	if(output_flag==true){
		//set prefix for output
		ostringstream prefix;
		prefix << "SCC_" << this->index <<"> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
		ostringstream cout;
		cout << "Read the structure coupling parameters from database..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	

	}

	//read out the results
	this->index_rv_model=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rv_id)).c_str()).toInt();
	this->index_related_profile=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_prof_id)).c_str()).toInt();
	this->distance_start_related=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rel_dist)).c_str()).toDouble();
	this->left_flag=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_left_bank)).c_str()).toBool();
	this->inflow_type=_Hyd_Coupling_Structure::convert_string2coupling_type(results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_type)).c_str()).toString().toStdString());

	try{
		//allocate the coupling structure
		this->allocate_coupling_structure();

		_hyd_coupling_structure_data buffer;
	
		buffer.controlled_flag=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control)).c_str()).toBool();
		buffer.controlled_param=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control_param)).c_str()).toDouble();
		buffer.width=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_width)).c_str()).toDouble();
		buffer.uncontrollable_param=results->record(index).value((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_uncontrol_param)).c_str()).toDouble();
		//set the data of the coupling structure
		this->coupling_struct->set_coupling_structure_data(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info <<"Index of the structure coupling : " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	
	if(output_flag==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

}
//Create the database table for the coupling structure data (static)
void Hyd_Coupling_RV2FP_Structure::create_table(QSqlDatabase *ptr_database){
		if(Hyd_Coupling_RV2FP_Structure::coupling_structure_table==NULL){
			ostringstream cout;
			cout << "Create hydraulic coupling structure data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_coup_structure;
			const int num_col=15;
			_Sys_data_tab_column tab_col[num_col];

			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}
			tab_col[0].name=hyd_label::coup_struct_glob_id;
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

			tab_col[4].name=hyd_label::coup_struct_rv_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=hyd_label::coup_struct_prof_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=hyd_label::coup_struct_rel_dist;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=hyd_label::coup_struct_left_bank;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="false";

			tab_col[8].name=hyd_label::struct_hyd_type;
			tab_col[8].type=sys_label::tab_col_type_string;

			tab_col[9].name=hyd_label::struct_hyd_width;
			tab_col[9].type=sys_label::tab_col_type_double;

			tab_col[10].name=hyd_label::struct_hyd_control;
			tab_col[10].type=sys_label::tab_col_type_bool;
			tab_col[10].default_value="false";

			tab_col[11].name=hyd_label::struct_hyd_control_param;
			tab_col[11].type=sys_label::tab_col_type_double;

			tab_col[12].name=hyd_label::struct_hyd_uncontrol_param;
			tab_col[12].type=sys_label::tab_col_type_double;

			tab_col[13].name=hyd_label::struct_hyd_point;
			tab_col[13].type=sys_label::tab_col_type_point;

			tab_col[14].name=label::description;
			tab_col[14].type=sys_label::tab_col_type_string;

			try{
				Hyd_Coupling_RV2FP_Structure::coupling_structure_table= new Tables();
				if(Hyd_Coupling_RV2FP_Structure::coupling_structure_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Coupling_RV2FP_Structure::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Coupling_RV2FP_Structure::close_table();
				throw msg;
			}

		Hyd_Coupling_RV2FP_Structure::close_table();
	}

}
//Set the database table for the coupling structure data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Coupling_RV2FP_Structure::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Coupling_RV2FP_Structure::coupling_structure_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_coup_structure;
		string tab_id_col[15];

		tab_id_col[0]=hyd_label::coup_struct_glob_id;
		tab_id_col[1]=hyd_label::coup_struct_rv_id;
		tab_id_col[2]=hyd_label::coup_struct_prof_id;
		tab_id_col[3]=label::areastate_id;
		tab_id_col[4]=label::measure_id;
		tab_id_col[5]=hyd_label::coup_struct_rel_dist;
		tab_id_col[6]=hyd_label::coup_struct_left_bank;
		tab_id_col[7]=hyd_label::struct_hyd_type;
		tab_id_col[8]=hyd_label::struct_hyd_control_param;
		tab_id_col[9]=hyd_label::struct_hyd_width;
		tab_id_col[10]=hyd_label::struct_hyd_control;
		tab_id_col[11]=hyd_label::struct_hyd_uncontrol_param;
		tab_id_col[12]=label::description;
		tab_id_col[13]=label::applied_flag;
		tab_id_col[14]=hyd_label::struct_hyd_point;


		try{
			Hyd_Coupling_RV2FP_Structure::coupling_structure_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Coupling_RV2FP_Structure::coupling_structure_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Coupling_RV2FP_Structure::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Coupling_RV2FP_Structure::close_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the coupling structure data (static)
void Hyd_Coupling_RV2FP_Structure::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Structure::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	Hyd_Coupling_RV2FP_Structure::coupling_structure_table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the coupling structure data (static)
void Hyd_Coupling_RV2FP_Structure::close_table(void){
	if(Hyd_Coupling_RV2FP_Structure::coupling_structure_table!=NULL){
		delete Hyd_Coupling_RV2FP_Structure::coupling_structure_table;
		Hyd_Coupling_RV2FP_Structure::coupling_structure_table=NULL;
	}
}
//Select and count the number of relevant structure couplings in a database table (static)
int Hyd_Coupling_RV2FP_Structure::select_relevant_structures_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output){
	int number=0;

	try{
		Hyd_Coupling_RV2FP_Structure::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
			ostringstream prefix;
			prefix << "SCC> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			ostringstream cout;
			cout << "Search for relevant structure couplings in database ..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	//generate the filter
	ostringstream filter;
	filter <<"SELECT ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rv_id)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_prof_id)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rel_dist)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_left_bank)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_type)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control_param)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_width)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_uncontrol_param)  <<" , ";
	filter <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::description);

	filter << " FROM " << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name();
	
	filter << " WHERE " ;
	filter << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) " ;
	filter << " ORDER BY "<< Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id);


	//database request
	Data_Base::database_request(results, filter.str(), ptr_database);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Structure::select_relevant_structures_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
		
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant structure coupling(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	return number;
}
//Transfer the coupling structure data of a file into a database table
void Hyd_Coupling_RV2FP_Structure::transfer_coupling_structure2database_table(QSqlDatabase *ptr_database){
	//set prefix for output
	ostringstream prefix;
	prefix << "SCC_" << this->index <<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout <<"Transfer hydraulic structure coupling data to database ..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Structure::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Hyd_Coupling_RV2FP_Structure::coupling_structure_table->maximum_int_of_column(Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id),ptr_database)+1;;
	//insert row
	model.insertRows(0, 1);	
																																				
	//set data
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::applied_flag)).c_str())),true);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rv_id)).c_str())),this->index_rv_model);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_prof_id)).c_str())),this->index_related_profile);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_rel_dist)).c_str())),this->distance_start_related);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_left_bank)).c_str())),this->left_flag);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_type)).c_str())), _Hyd_Coupling_Structure::convert_coupling_type2string(this->inflow_type).c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control_param)).c_str())),this->coupling_struct->get_coupling_structure_data().controlled_param);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_width)).c_str())),this->coupling_struct->get_coupling_structure_data().width);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_control)).c_str())),this->coupling_struct->get_coupling_structure_data().controlled_flag);
	model.setData(model.index(0,model.record().indexOf((Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_uncontrol_param)).c_str())),this->coupling_struct->get_coupling_structure_data().uncontrollable_param);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Global Id                 : " << id_glob <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//The couplings are initialized with the already performed RV2FP couplings
void Hyd_Coupling_RV2FP_Structure::init_coupling(Hyd_Coupling_RV2FP_Merged *couplings, const int number){

	//flag if the connection is found
	bool found_flag=false;
	//pointer to the related existing coupling point, where this structure coupling is located
	Hyd_Coupling_Point_RV2FP *coupling_point=NULL;
	//list of the coupling point
	Hyd_Coupling_Point_RV2FP_List *list=NULL;

	//set prefix for output
	ostringstream prefix;
	prefix << "SCC> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling (index " << this->index <<") between river model " << this->index_rv_model << " via a hydraulic structure ";
	cout << "at distance from begin  " << this->get_total_distance_along_polysegment() << label::m;
	cout << "..."<< endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);



	//search for the coupling point
	for(int i=0; i<number; i++){
		if(couplings[i].get_river_index()==this->index_rv_model){
			//left bank
			if(this->left_flag==true){
				//set the list
				list=&couplings[i].list_left;
			}
			else{
				list=&couplings[i].list_right;
			}
			if(list==NULL){
				coupling_point=NULL;
			}
			else{
				//related profile is inbetween
				if(this->index_related_profile>0 && list->find_distance2related_profile(this->index_related_profile, &this->distance_along_polysegment)==true){
					this->set_total_distance_along_polysegment(this->distance_along_polysegment+this->distance_start_related);
					coupling_point=list->find_coupling_point(this->get_total_distance_along_polysegment(), this);

				}
				//related profile is inflow => the start of the line
				else if(this->index_related_profile==0){
					this->set_total_distance_along_polysegment(this->distance_start_related);
					coupling_point=list->find_coupling_point(this->get_total_distance_along_polysegment(), this);
				}
				else{
					coupling_point=NULL;
				}

			}
			if(coupling_point!=NULL){
				this->index_fp_model=coupling_point->get_floodplain_index();
				found_flag=true;
				break;
			}
		}
	}
	if(found_flag==false){
		Warning msg=this->set_warning(0);
		msg.output_msg(2);
		//reaction
		this->coupling_flag=false;
		list=NULL;
	}
	else{
		this->coupling_flag=true;
		//set the pointers to the relevant coupling elements
		this->river_profile_down=coupling_point->get_downstream_rvprofile();
		this->river_profile_up=coupling_point->get_upstream_rvprofile();
		this->floodplain_elem=coupling_point->get_floodplain_element();
		if(this->floodplain_elem!=NULL){
			this->floodplain_elem->element_type->set_coupling_data();
		}
		//set the mid factors and the height for the structure
		if(this->left_flag==true){
			this->set_midvalues_leftbank();
		}
		else{
			this->set_midvalues_rightbank();
		}
		//set the height to the structure
		this->coupling_struct->set_global_sill_height(this->mid_height);

		//set two points into the coupling list for the structure; there will be no break or overflow
		list->insert_constricting_coupling_points(this->coupling_struct->get_width(),this);

		if(this->river_profile_down->get_distance2upstream()<this->coupling_struct->get_width()){
			Error msg=this->set_error(2);
			throw msg;

		}
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Synchronise the models
void Hyd_Coupling_RV2FP_Structure::synchronise_models(const double timepoint, const double delta_t, const bool time_check){
	_Hyd_Coupling_Point::syncronisation_coupled_models();
	this->delta_t=delta_t;
	if(time_check==true){
		return;
	}

	if(this->coupling_flag==true){
		double q_buff=0.0;
		double h_floodplain=0.0;
			
		this->calculate_mid_waterlevel();
		
		h_floodplain=this->floodplain_elem->element_type->get_s_value();
		q_buff=this->coupling_struct->get_discharge(this->mid_waterlevel, h_floodplain);

		//smooth it for numerical reasons
		q_buff=this->smooth_coupling_discharge(q_buff, &this->old_q);
		this->current_q=q_buff;
		this->calculate_maximum_values(timepoint, this->current_q, &this->max_coupling_v);
		this->calculate_hydrological_balance(this->current_q, &this->coupling_volume);
		//discharge to element
		this->floodplain_elem->element_type->add_coupling_discharge_rv_structure((-1.0)*this->current_q);
		//discharge to river profile segment
		if(this->horizontal_backwater_flag==false && this->horizontal_backwater_flag_upstream==false){
			this->river_profile_up->add_structure_coupling_discharge(this->current_q*this->mid_fac_up);
			this->river_profile_down->add_structure_coupling_discharge(this->current_q*this->mid_fac_down);
		}
		else if(this->horizontal_backwater_flag==true){
			this->river_profile_down->add_structure_coupling_discharge(this->current_q);
		}
		else if(this->horizontal_backwater_flag_upstream==true){
			this->river_profile_up->add_structure_coupling_discharge(this->current_q);
		}

	}
}
//Reset the coupling discharges
void Hyd_Coupling_RV2FP_Structure::reset_coupling_discharge(void){
	_Hyd_Coupling_Point::reset_coupling_discharge();
	if(this->coupling_flag==true){
		this->river_profile_down->reset_structure_coupling_discharge();
		this->river_profile_up->reset_structure_coupling_discharge();
		this->floodplain_elem->element_type->reset_coupling_discharge_rv_structure();
	}

}
//Output the members
void Hyd_Coupling_RV2FP_Structure::output_members(void){
		ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "SCC_"<<this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout <<"RIVER" <<endl;
	cout << " River model number                         : " << W(7) << this->index_rv_model << endl;
	cout << " Left bank (true)/right bank (false)        : " << W(7) << functions::convert_boolean2string(this->left_flag) << endl;
	cout << " Distance to river begin                    : " << W(7) << this->distance_along_polysegment << label::m << endl;
	cout <<"STRUCTURE" << endl;
	cout <<" Type                                        : " << W(7) << _Hyd_Coupling_Structure::convert_coupling_type2string(this->inflow_type)<<endl;
	this->coupling_struct->output_members(&cout);
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_RV2FP_Structure::output_header_coupled_indices(ostringstream *cout){
	*cout << "River to floodplain as structure..." << endl ;
	*cout << "No." << W(12) << "Id_RV" << W(14)<< "Id_RVprofile" << W(14);
	*cout<< "Id_FP" << W(15) << "Id_FPelem" << W(12)<< "h_mid" << label::m ;
	*cout << W(15)<<"Dist_rv_begin"<< label::m << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indizes of the coupled model
void Hyd_Coupling_RV2FP_Structure::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	if(this->coupling_flag==true){
		*cout << this->index_rv_model << W(14);
		*cout << this->river_profile_up->get_profile_number() << W(14);
		*cout << this->index_fp_model <<W(15);
		*cout << this->floodplain_elem->get_elem_number()<< W(15);
		*cout << this->mid_height << W(15);
		*cout << this->distance_along_polysegment << endl;
	}
	else{
		*cout <<"  No structure coupling is applied; see the warnings!" << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output final result members
void Hyd_Coupling_RV2FP_Structure::output_final_results(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "SC_"<<this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout <<"FLOW THROUGH STRUCTURE" <<endl;
	if(this->coupling_flag==true){
		cout << " Maximum velocity (abs)                     : " << W(7) << P(1) << FORMAT_FIXED_REAL << this->max_coupling_v.maximum << label::m_per_sec<< endl;
		cout << " Time of max. velocity                      : " << W(7) << P(0) << FORMAT_FIXED_REAL<< this->max_coupling_v.time_point<< label::sec << endl;
		cout << " Total volume                               : " << W(7) << P(1) << FORMAT_FIXED_REAL<< this->coupling_volume.volume_total << label::cubicmeter<< endl;
		cout << " Volume to river " << this->index_rv_model<<"                        : " << W(7) << P(1) << FORMAT_FIXED_REAL<< this->coupling_volume.volume_in  << label::cubicmeter << endl;
		cout << " Volume to floodplain " << this->index_fp_model<<"                   : " << W(7) << P(1) << FORMAT_FIXED_REAL<< this->coupling_volume.volume_out  << label::cubicmeter << endl;
		cout << " Left bank (true)/ right bank (false)       : " << W(7)<< functions::convert_boolean2string(this->left_flag) << endl;
	
	}
	else{
		cout << " No breach development was possible; check the warnings" <<endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Reset the hydrological balance values and the maximum calculated values
void Hyd_Coupling_RV2FP_Structure::reset_hydro_balance_max_values(void){
	_Hyd_Coupling_Point::reset_hydro_balance_max_values();
	this->coupling_struct->reset_structure();
}
//Clone the structure coupling
void Hyd_Coupling_RV2FP_Structure::clone_couplings(Hyd_Coupling_RV2FP_Structure *coupling, Hyd_Hydraulic_System *system){
	_Hyd_Coupling_Point::operator =(*coupling);
	this->system_id=coupling->get_sys_system_id();
	this->index=coupling->index;
	this->index_rv_model=coupling->index_rv_model;
	this->index_fp_model=coupling->index_fp_model;
	this->left_flag=coupling->left_flag;
	this->inflow_type=coupling->inflow_type;
	this->coupling_struct=coupling->coupling_struct;
	this->index_related_profile=coupling->index_related_profile;
	this->distance_start_related=coupling->distance_start_related;

	this->mid_height=coupling->mid_height;
	this->mid_fac_up=coupling->mid_fac_up;
	this->mid_fac_down=coupling->mid_fac_down;
	this->mid_waterlevel=coupling->mid_waterlevel;

	this->horizontal_backwater_flag=coupling->horizontal_backwater_flag;
	this->horizontal_backwater_flag_upstream=coupling->horizontal_backwater_flag_upstream;

	//fp
	Hyd_Model_Floodplain *buff_fp;
	buff_fp=system->get_ptr_floodplain_model_index(this->index_fp_model);
	this->floodplain_elem=buff_fp->get_ptr_floodplain_elem(coupling->floodplain_elem->get_elem_number());
	if(this->floodplain_elem!=NULL){
		this->floodplain_elem->element_type->set_coupling_data();
	}

	//rv
	Hyd_Model_River *buff_rv;
	buff_rv=system->get_ptr_river_model_index(this->index_rv_model);
	if(coupling->river_profile_up!=NULL){
		this->river_profile_up=buff_rv->get_ptr_river_profile_number(coupling->river_profile_up->get_profile_number());		
	}
	if(coupling->river_profile_down!=NULL){
		this->river_profile_down=buff_rv->get_ptr_river_profile_number(coupling->river_profile_down->get_profile_number());		
	}
	//structure
	this->allocate_coupling_structure();
	this->coupling_struct->clone_coupling_structure(coupling->coupling_struct);
}
//Ouput the point to database as Geo_point data
void Hyd_Coupling_RV2FP_Structure::output_point2database(QSqlDatabase *ptr_database){
	if(ptr_database==NULL){
		return;
	}

	try{
		Hyd_Coupling_RV2FP_Structure::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;
	string buffer;
	buffer=Geo_Point::get_point2sql_string(this->x_coordinate, this->y_coordinate).c_str();
	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::struct_hyd_point) << " = "<< buffer<<"";
	query_string  << " WHERE ";
	query_string  << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(label::measure_id) << " = " << this->system_id.area_state ;
	query_string  << " AND ";
	query_string  << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_column_name(hyd_label::coup_struct_glob_id) << " = "<< this->index;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Structure::output_point2database(QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Structure::coupling_structure_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Reset the smoothing calculation members
void Hyd_Coupling_RV2FP_Structure::reset_smoothing(void){
	_Hyd_Coupling_Point::reset_smoothing();

	this->grad_q_current=0.0;
	this->grad_q_before=0.0;
	this->oscilation_counter=1.0;
	this->no_osci_counter=0.0;
	
}
//______________
//private
//Allocate the given coupling structure type
void Hyd_Coupling_RV2FP_Structure::allocate_coupling_structure(void){
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
//Set the structure coupling member with a structure where the parser has gathered the informations (_rv2fp_coupling_params)
void Hyd_Coupling_RV2FP_Structure::set_structure_coupling_params(_hyd_rv2fp_struct_coupling_params parameter){
	this->index_related_profile=parameter.profile_index;
	this->distance_start_related=parameter.distance;
	this->index_rv_model=parameter.rv_model_no;
	this->inflow_type=parameter.type;
	this->left_flag=parameter.left_flag;
}
//Set the mid-values (mid_height, mid_fac_up, mid_fac_down) for a coupling over the left bank
void Hyd_Coupling_RV2FP_Structure::set_midvalues_leftbank(void){
	//set the factors for the mid-value calculation depending on the distances
	double total_distance=0.0;
	double distance_up=0.0;
	double distance_down=0.0;
	//calculate the distances to the profiles
	total_distance=abs(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
	distance_up=abs(this->distance(&(this->river_profile_up->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
	distance_down=abs(this->distance(&(this->river_profile_down->typ_of_profile->get_first_point()->get_global_x_y_coordinates())));
	//set the factors via the distances
	this->mid_fac_down=(1.0-distance_down/total_distance);
	this->mid_fac_up=(1.0-distance_up/total_distance);

	//check for a weir profile
	if(this->river_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		//full weight to the downstream profile
		this->mid_fac_down=1.0;
		this->mid_fac_up=0.0;
	}
	if(this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		//full weight to the upstream profile
		this->mid_fac_down=0.0;
		this->mid_fac_up=1.0;
	}


	//set the mid_height via the factors (here the z_min points are relavant)
	this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
	//if the height of the element is above the river bank line set the element height
	this->mid_height=max(this->mid_height, this->floodplain_elem->get_z_value());


}
//Set the mid-values (mid_height, mid_fac_up, mid_fac_down) for a coupling over the right bank
void Hyd_Coupling_RV2FP_Structure::set_midvalues_rightbank(void){
	this->coupling_flag=true;
	//set the factors for the mid-value calculation depending on the distances
	double total_distance=0.0;
	double distance_up=0.0;
	double distance_down=0.0;
	//calculate the distances to the profiles
	total_distance=abs(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates().distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	distance_up=abs(this->distance(&(this->river_profile_up->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	distance_down=abs(this->distance(&(this->river_profile_down->typ_of_profile->get_last_point()->get_global_x_y_coordinates())));
	//set the factors via the distances
	this->mid_fac_down=(1.0-distance_down/total_distance);
	this->mid_fac_up=(1.0-distance_up/total_distance);

	//check for a weir profile
	if(this->river_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		//full weight to the downstream profile
		this->mid_fac_down=1.0;
		this->mid_fac_up=0.0;
	}
	if(this->river_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE){
		//full weight to the upstream profile
		this->mid_fac_down=0.0;
		this->mid_fac_up=1.0;
	}
	//set the mid_height via the factors (here the z_min points are relavant)
	this->mid_height=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();
	//if the height of the element is above the river bank line set the element height
	this->mid_height=max(this->mid_height, this->floodplain_elem->get_z_value());


}
//Calculate the current mid-waterlevel via the factors
void Hyd_Coupling_RV2FP_Structure::calculate_mid_waterlevel(void){

	if(this->coupling_flag==true){
		this->horizontal_backwater_flag=false;
		this->horizontal_backwater_flag_upstream=false;
		//set the mid_waterlevel via the factors
		if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->mid_fac_down*this->river_profile_down->get_actual_global_waterlevel()+this->mid_fac_up*this->river_profile_up->get_actual_global_waterlevel();
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->river_profile_up->get_actual_global_waterlevel();
			this->horizontal_backwater_flag_upstream=true;
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon&& this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->river_profile_down->get_actual_global_waterlevel();
			this->horizontal_backwater_flag=true;
		}
		else if(this->river_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->river_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			this->mid_waterlevel=this->mid_fac_down*this->river_profile_down->typ_of_profile->get_global_z_min()+this->mid_fac_up*this->river_profile_up->typ_of_profile->get_global_z_min();

		}
	}
}
//Smooth the coupling discharge with the coupling discharge calculated one internal timestep before
double Hyd_Coupling_RV2FP_Structure::smooth_coupling_discharge(const double q_current, double *old_q){
	double smooth_q=0.0;
	

	if(this->delta_t>0.0){
		this->grad_q_current=(q_current-*old_q)/(this->delta_t);
		if((this->grad_q_before>0.0 && this->grad_q_current<0.0)|| (this->grad_q_before<0.0 && this->grad_q_current>0.0)){
		//if((*old_q>0.0 && q_current<0.0)|| (*old_q<0.0 && q_current>0.0)){
			this->oscilation_counter=this->oscilation_counter+2.0;
			this->no_osci_counter=0.0;
			
			if(this->oscilation_counter>=100.0){
				this->oscilation_counter=100.0;
			}

		}
		else{
			this->oscilation_counter=this->oscilation_counter-this->no_osci_counter;
			if(this->oscilation_counter<1.0){
				this->oscilation_counter=1.0;
			}
			this->no_osci_counter=this->no_osci_counter+1.0;
		}

		this->grad_q_before=this->grad_q_current;
	}


	smooth_q=q_current*(1.0/this->oscilation_counter)+(*old_q)*(1.0-(1.0/this->oscilation_counter));
	

	
	*old_q=smooth_q;
	return smooth_q;
}
//Set error(s)
Error Hyd_Coupling_RV2FP_Structure::set_error(const int err_type){
	string place="Hyd_Coupling_RV2FP_Structure::";
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
		case 1://unkown type of a coupling structure
			place.append("allocate_coupling_structure(void)");
			reason="The type of the coupling structure is not known";
			help="Check the coupling structure type";
			type=16;
			break;
		case 2://the width of the structure is greater than the distance between the river profiles
			place.append("init_coupling(Hyd_Coupling_RV2FP *couplings, const int number)") ;
			reason="The width of the coupling structure is greater than the distance between the river profiles, where the structure is allocated";
			help= "Check the width of the structure and there location";
			info << "Structure coupling index: " << this->index << endl;
			info << "River index             : " << this->index_rv_model << endl;
			info << "Distance profiles (" << this->river_profile_up->get_profile_number()<<" -> " <<this->river_profile_down->get_profile_number() << ") : " << this->river_profile_down->get_distance2upstream()<< label::m  << endl;
			info << "Width structure         : " << this->coupling_struct->get_width()<< label::m << endl;
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
Warning Hyd_Coupling_RV2FP_Structure::set_warning(const int warn_type){
		string place="Hyd_Coupling_RV2FP_Structure::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no coupling between the river an dthe floodplain is possible
			place.append("init_coupling(Hyd_Coupling_RV2FP *couplings, const int number)") ;
			reason="A structure coupling between a river- and a floodplain model via a structure is not possible";
			reaction="The setted coupling is ignored";
			help= "Check if there is an intersection between the models";
			info << "Structure coupling index: " << this->index << endl;
			info << "River index             : " << this->index_rv_model << endl;
			type=10;	
			break;
		case 1://input datas can not submitted
			place.append("transfer_coupling_structure2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the hydraulic structure coupling data to the database";
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