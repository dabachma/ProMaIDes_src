#include "Hyd_Headers_Precompiled.h"
//#include "_Hyd_Coupling_Dikebreak.h"
//init static members
Tables *_Hyd_Coupling_Dikebreak::break_data_table=NULL;
Tables *_Hyd_Coupling_Dikebreak::break_result_table=NULL;


//Default constructor
_Hyd_Coupling_Dikebreak::_Hyd_Coupling_Dikebreak(void): default_max_breach_width(200.0), init_breach_width(0.5),numerical_breach_v(1.0/60.0), f1(1.3), f2(0.04){

	this->index=-1;	
	this->index_section_fpl=-1;
	this->index_fp_model_start=-1;

	this->start_waterlevel_local=0.0;
	this->start_waterlevel_global=0.0;
	this->max_breach_width=default_max_breach_width;

	this->coupling_flag=false;
	this->downstream_open_flag=false;
	this->upstream_open_flag=false;
	this->started_flag=false;
	this->init_phase_finished=false;
	this->breach_counter=0;
	this->starting_time_point=0.0;
	this->breach_time=0.0;

	this->total_breach_width=0.0;
	this->delta_width_upstream=0.0;
	this->breach_width_upstream=0.0;
	this->distance_next_point_upstream=0.0;
	this->delta_width_downstream=0.0;
	this->breach_width_downstream=0.0;
	this->distance_next_point_downstream=0.0;

	this->break_params_upstream=NULL;
	this->break_params_downstream=NULL;
	this->deltah_upstream=NULL;
	this->deltah_downstream=NULL;

	this->counter_stress_upstream=0.0;
	this->counter_stress_downstream=0.0;

	this->transition_upstream_flag=false;
	this->transition_downstream_flag=false;
	this->transition_width_upstream=0.0;
	this->transition_width_downstream=0.0;

	this->wall_breach_flag=false;

	this->max_observed_waterlevel=0;

	this->mean_v=0.0;
	this->mean_q=0.0;
	this->total_mean_dh=0.0;

	this->file_name = label::not_set;
	this->file_name_csv = label::not_set;
	this->user_defined=true;


}
//Default destructor
_Hyd_Coupling_Dikebreak::~_Hyd_Coupling_Dikebreak(void){

	this->close_output_file();
	this->close_output_file_csv();

}
//________
//public
//Create the database table for the dikebreak/wallbreak data (static)
void _Hyd_Coupling_Dikebreak::create_table(QSqlDatabase *ptr_database){
		if(_Hyd_Coupling_Dikebreak::break_data_table==NULL){
			ostringstream cout;
			cout << "Create dike-/wallbreak coupling data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_coup_break;
			const int num_col=12;
			_Sys_data_tab_column tab_col[num_col];

			//init the coplumns
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}
			tab_col[0].name=hyd_label::coup_break_glob_id;
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

			tab_col[4].name=hyd_label::coup_break_rv_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=hyd_label::coup_break_prof_id;
			tab_col[5].type=sys_label::tab_col_type_int;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=hyd_label::coup_break_rel_dist;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=hyd_label::coup_break_left_bank;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="false";

			tab_col[8].name=hyd_label::coup_break_start_h;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="1.0";

			tab_col[9].name=hyd_label::coup_break_max_w;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="200.0";

			tab_col[10].name=hyd_label::coup_break_point;
			tab_col[10].type=sys_label::tab_col_type_point;

			tab_col[11].name=label::description;
			tab_col[11].type=sys_label::tab_col_type_string;


			try{
				_Hyd_Coupling_Dikebreak::break_data_table= new Tables();
				if(_Hyd_Coupling_Dikebreak::break_data_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("_Hyd_Coupling_Dikebreak::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				_Hyd_Coupling_Dikebreak::close_table();
				throw msg;
			}

		_Hyd_Coupling_Dikebreak::close_table();
	}

}
//Set the database table for the dikebreak/wallbreak data: it sets the table name and the name of the columns and allocate them (static)
void _Hyd_Coupling_Dikebreak::set_table(QSqlDatabase *ptr_database){
	if(_Hyd_Coupling_Dikebreak::break_data_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_coup_break;
		string tab_id_col[12];

		tab_id_col[0]=hyd_label::coup_break_glob_id;
		tab_id_col[1]=hyd_label::coup_break_rv_id;
		tab_id_col[2]=hyd_label::coup_break_prof_id;
		tab_id_col[3]=hyd_label::coup_break_rel_dist;
		tab_id_col[4]=hyd_label::coup_break_left_bank;
		tab_id_col[5]=hyd_label::coup_break_start_h;
		tab_id_col[6]=hyd_label::coup_break_max_w;
		tab_id_col[7]=label::description;
		tab_id_col[8]=label::areastate_id;
		tab_id_col[9]=label::measure_id;
		tab_id_col[10]=label::applied_flag;
		tab_id_col[11]=hyd_label::coup_break_point;

		try{
			_Hyd_Coupling_Dikebreak::break_data_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			_Hyd_Coupling_Dikebreak::break_data_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_Coupling_Dikebreak::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			_Hyd_Coupling_Dikebreak::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the dikebreak/wallbreak data (static)
void _Hyd_Coupling_Dikebreak::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	_Hyd_Coupling_Dikebreak::break_data_table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the dikebreak/wallbreak data (static)
void _Hyd_Coupling_Dikebreak::close_table(void){
	if(_Hyd_Coupling_Dikebreak::break_data_table!=NULL){
		delete _Hyd_Coupling_Dikebreak::break_data_table;
		_Hyd_Coupling_Dikebreak::break_data_table=NULL;
	}
}
///Select and count the number of dike-/wallbreak couplings in a database table (static)
int _Hyd_Coupling_Dikebreak::select_relevant_break_data_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output){
	int number=0;

	try{
		_Hyd_Coupling_Dikebreak::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	if(with_output==true){
			ostringstream prefix;
			prefix << "DBC> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			ostringstream cout;
			cout << "Search for relevant dike-/wallbreak couplings in database for river to floodplain model..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	
	//generate the filter
	ostringstream filter;
	filter <<"SELECT ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_prof_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rel_dist)  <<" , ";
	
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_left_bank)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_start_h)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::description)  <<" , ";

	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_max_w);

	filter << " FROM " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name();
	
	filter << " WHERE " ;
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id) << " >= 0"; ;
	filter << " and ";
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) ";
	filter << " ORDER BY "<< _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id);

	Data_Base::database_request(results, filter.str(), ptr_database);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::select_relevant_break_data_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
		
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant dike-/wallbreak coupling(s) or river to floodplain model are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	return number;

}
//Select and count the number of dike-/wallbreak couplings in a database table for coast to floodplain models (static)
int _Hyd_Coupling_Dikebreak::select_relevant_break_data_coast_database(QSqlDatabase *ptr_database, QSqlQueryModel *results, const _sys_system_id id, const bool with_output){
		int number=0;

	try{
		_Hyd_Coupling_Dikebreak::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	if(with_output==true){
			ostringstream prefix;
			prefix << "DBC> ";
			Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
			ostringstream cout;
			cout << "Search for relevant dike-/wallbreak couplings in database for coast to floodplain model..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	
	//generate the filter
	ostringstream filter;
	filter <<"SELECT ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_prof_id)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rel_dist)  <<" , ";
	
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_left_bank)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_start_h)  <<" , ";
	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::description)  <<" , ";

	filter <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_max_w);

	filter << " FROM " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name();
	
	filter << " WHERE " ;
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_rv_id) << " < 0"; ;
	filter << " and ";
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::applied_flag) << " = true";
	filter << " and ";
	filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	filter << " and (";
		filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) << " = " << 0 ;
		filter << " or " ;
		filter << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	filter << " ) ";
	filter << " ORDER BY "<< _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id);

	Data_Base::database_request(results, filter.str(), ptr_database);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::select_relevant_break_data_coast_database(QSqlTableModel *results, const _sys_system_id id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
		
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant dike-/wallbreak coupling(s) or coast to floodplain model are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}

	return number;
}
//Create the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::create_result_table(QSqlDatabase *ptr_database){
		if(_Hyd_Coupling_Dikebreak::break_result_table==NULL){
			ostringstream cout;
			cout << "Create dike-/wallbreak coupling result data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_coup_break_result;
			const int num_col=22;
			_Sys_data_tab_column tab_col[num_col];

			//init the coplumns
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}
			tab_col[0].name=label::glob_id;
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

			tab_col[4].name=hyd_label::sz_bound_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::sz_break_id;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=hyd_label::coup_break_glob_id;
			tab_col[6].type=sys_label::tab_col_type_int;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=hyd_label::coup_break_user_defined;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="true";

			tab_col[8].name=hyd_label::coup_break_start_time;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";

			tab_col[9].name=hyd_label::coup_break_width_total;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;
			tab_col[9].default_value="0.0";

			tab_col[10].name=hyd_label::coup_break_width_upstream;
			tab_col[10].type=sys_label::tab_col_type_double;
			tab_col[10].unsigned_flag=true;
			tab_col[10].default_value="0.0";

			tab_col[11].name=hyd_label::coup_break_width_downstream;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].unsigned_flag=true;
			tab_col[11].default_value="0.0";

			tab_col[12].name=hyd_label::coup_break_upstream_stop;
			tab_col[12].type=sys_label::tab_col_type_bool;

			tab_col[13].name=hyd_label::coup_break_downstream_stop;
			tab_col[13].type=sys_label::tab_col_type_bool;

			tab_col[14].name=hyd_label::coup_break_max_v;
			tab_col[14].type=sys_label::tab_col_type_double;
			tab_col[14].unsigned_flag=true;
			tab_col[14].default_value="0.0";

			tab_col[15].name=hyd_label::coup_break_max_v_time;
			tab_col[15].type=sys_label::tab_col_type_double;
			tab_col[15].default_value="0.0";

			tab_col[16].name=hyd_label::coup_break_total_volume;
			tab_col[16].type=sys_label::tab_col_type_double;
			tab_col[16].unsigned_flag=true;
			tab_col[16].default_value="0.0";

			tab_col[17].name=hyd_label::coup_break_volume_fp;
			tab_col[17].type=sys_label::tab_col_type_double;
			tab_col[17].unsigned_flag=true;
			tab_col[17].default_value="0.0";

			tab_col[18].name=hyd_label::coup_break_volume_rv;
			tab_col[18].type=sys_label::tab_col_type_double;
			tab_col[18].unsigned_flag=true;
			tab_col[18].default_value="0.0";

			tab_col[19].name=hyd_label::coup_break_x_start;
			tab_col[19].type=sys_label::tab_col_type_double;

			tab_col[20].name=hyd_label::coup_break_y_start;
			tab_col[20].type=sys_label::tab_col_type_double;

			tab_col[21].name=hyd_label::coup_break_polyline;
			tab_col[21].type=sys_label::tab_col_type_polyline;


			try{
				_Hyd_Coupling_Dikebreak::break_result_table= new Tables();
				if(_Hyd_Coupling_Dikebreak::break_result_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("_Hyd_Coupling_Dikebreak::create_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				_Hyd_Coupling_Dikebreak::close_result_table();
				throw msg;
			}

		_Hyd_Coupling_Dikebreak::close_result_table();
	}
}
//Set the database table for the dikebreak/wallbreak result data: it sets the table name and the name of the columns and allocate them
void _Hyd_Coupling_Dikebreak::set_result_table(QSqlDatabase *ptr_database){
	if(_Hyd_Coupling_Dikebreak::break_result_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_coup_break_result;
		string tab_col[22];

		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=risk_label::sz_break_id;
		tab_col[5]=hyd_label::coup_break_glob_id;
		tab_col[6]=hyd_label::coup_break_user_defined;
		tab_col[7]=hyd_label::coup_break_start_time;
		tab_col[8]=hyd_label::coup_break_width_total;
		tab_col[9]=hyd_label::coup_break_width_upstream;
		tab_col[10]=hyd_label::coup_break_width_downstream;
		tab_col[11]=hyd_label::coup_break_upstream_stop;
		tab_col[12]=hyd_label::coup_break_downstream_stop;
		tab_col[13]=hyd_label::coup_break_max_v;
		tab_col[14]=hyd_label::coup_break_max_v_time;
		tab_col[15]=hyd_label::coup_break_total_volume;
		tab_col[16]=hyd_label::coup_break_volume_fp;
		tab_col[17]=hyd_label::coup_break_volume_rv;
		tab_col[18]=hyd_label::coup_break_x_start;
		tab_col[19]=hyd_label::coup_break_y_start;
		tab_col[20]=hyd_label::coup_break_polyline;
		tab_col[21]=label::applied_flag;


		try{
			_Hyd_Coupling_Dikebreak::break_result_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			_Hyd_Coupling_Dikebreak::break_result_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("_Hyd_Coupling_Dikebreak::set_result_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			_Hyd_Coupling_Dikebreak::close_result_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	_Hyd_Coupling_Dikebreak::break_result_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const int hyd_bound_id){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(risk_label::sz_break_id) << " = '" << id_break <<"'";
	query_string << " AND ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_id <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const int hyd_bound_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	

}
//Delete all data in the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const bool like_flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string << " WHERE " ;
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(risk_label::sz_break_id) << " = '" << id_break<<"'";
	}
	else{
		query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << id_break<<"'";
	}
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::delete_data_in_result_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const string id_break, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	

}
//Close and delete the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::close_result_table(void){
	if(_Hyd_Coupling_Dikebreak::break_result_table!=NULL){
		delete _Hyd_Coupling_Dikebreak::break_result_table;
		_Hyd_Coupling_Dikebreak::break_result_table=NULL;
	}
}
//Switch the applied-flag in the database table for the dikebreak/wallbreak result data (static)
void _Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;



	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag in the database table for the dikebreak/wallbreak result data for a given boundary scenario (static)
void _Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
		//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;

	

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the results of a given system id to another one in database table (static)
void _Hyd_Coupling_Dikebreak::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
//the table is set (the name and the column names) and allocated
	try{
		_Hyd_Coupling_Dikebreak::set_result_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	int glob_id=0;
	glob_id=_Hyd_Coupling_Dikebreak::break_result_table->maximum_int_of_column(_Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id);
	test_filter << " from " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;


	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");
	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" results of the dike break(s) to the new measure state..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		test_filter.str("");
		test_filter << "INSERT INTO "<< _Hyd_Coupling_Dikebreak::break_result_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_glob_id)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_user_defined)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_start_time)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_total)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_upstream)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_width_downstream)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_upstream_stop)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_downstream_stop)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_max_v)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_max_v_time)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_total_volume)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_volume_fp)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_volume_rv)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_x_start)  <<" , ";
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_y_start)  <<" , ";	
		test_filter <<  _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(hyd_label::coup_break_polyline)  <<"  ";

		test_filter << " FROM " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() <<" ";
		test_filter << " WHERE " << _Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(_Hyd_Coupling_Dikebreak::break_result_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("_Hyd_Coupling_Dikebreak::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_result_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{
		_Hyd_Coupling_Dikebreak::switch_applied_flag_erg_table(ptr_database, src, false);
	}
	catch(Error msg){
		throw msg;
	}
}
//Reset the hydrological balance values and the maximum calculated values
void _Hyd_Coupling_Dikebreak::reset_hydro_balance_max_values(void){
	_Hyd_Coupling_Point::reset_hydro_balance_max_values();

	this->started_flag=false;
	this->max_observed_waterlevel=0.0;
	this->init_phase_finished=false;
	this->breach_counter=0;
	this->starting_time_point=0.0;
	this->breach_time=0.0;

	this->total_breach_width=0.0;
	this->delta_width_upstream=0.0;
	this->breach_width_upstream=0.0;;
	this->delta_width_downstream=0.0;
	this->breach_width_downstream=0.0;

	this->counter_stress_upstream=0.0;
	this->counter_stress_downstream=0.0;

	this->transition_upstream_flag=false;
	this->transition_downstream_flag=false;
	this->transition_width_upstream=0.0;
	this->transition_width_downstream=0.0;

	this->mean_v=0.0;
	this->mean_q=0.0;
	this->total_mean_dh=0.0;
	file_name=label::not_set;

}
//Set the flag if the breach development is user defined (true) or dynamically set (false)
void _Hyd_Coupling_Dikebreak::set_flag_user_defined(const bool flag){
	this->user_defined=flag;
}
//Get the breach index
int _Hyd_Coupling_Dikebreak::get_breach_index(void){
	return this->index;
}
//Get the index of the fpl-section
int _Hyd_Coupling_Dikebreak::get_index_fpl_section(void){
	return this->index_section_fpl;
}
//Get if the breach has started
bool _Hyd_Coupling_Dikebreak::get_breach_has_started(void){
	return this->started_flag;
}
//Get maximal obeserved waterlevel
double _Hyd_Coupling_Dikebreak::get_max_observed_waterlevel(void){
	return this->max_observed_waterlevel;
}
//Ouput the point to database as Geo_point data
void _Hyd_Coupling_Dikebreak::output_point2database(QSqlDatabase *ptr_database){
	if(ptr_database==NULL){
		return;
	}

	try{
		_Hyd_Coupling_Dikebreak::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;
	string buffer;
	buffer=Geo_Point::get_point2sql_string(this->x_coordinate, this->y_coordinate).c_str();
	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name();
	query_string << " SET " ;
	query_string  << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_point) << " = "<< buffer<<"";
	query_string  << " WHERE ";
	query_string  << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
	query_string  << " AND ";
	query_string  <<  _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(label::measure_id) << " = " << this->system_id.area_state ;
	query_string  << " AND ";
	query_string  << _Hyd_Coupling_Dikebreak::break_data_table->get_column_name(hyd_label::coup_break_glob_id) << " = "<< this->index;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("_Hyd_Coupling_Dikebreak::output_point2database(QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << _Hyd_Coupling_Dikebreak::break_data_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Clear break list
void _Hyd_Coupling_Dikebreak::clear_break_list(void) {

	this->break_info_list.counter_breach.clear();
	this->break_info_list.delta_h2start.clear();
	this->break_info_list.downstream_breach.clear();
	this->break_info_list.downstream_delta_h.clear();
	this->break_info_list.upstream_breach.clear();
	this->break_info_list.upstream_delta_h.clear();
	this->break_info_list.downstream_wall_stress.clear();
	this->break_info_list.upstream_wall_stress.clear();
	this->break_info_list.mean_q.clear();
	this->break_info_list.mean_v.clear();
	this->break_info_list.time.clear();
	this->break_info_list.time_breach.clear();
	this->break_info_list.total_breach.clear();



}
//__________
//protected
//Check the dikebreak parameters
void _Hyd_Coupling_Dikebreak::check_dikebreak_parameter(void){

	//start waterlevel (error)
	if(this->start_waterlevel_local<0.0){
		Error msg=this->set_error(1);
		throw msg;

	}
	//maximum breach width (warning)
	if(this->max_breach_width<=0.0){
		Warning msg=this->set_warning(0);
		msg.output_msg(2);
		//reaction
		this->max_breach_width=this->default_max_breach_width;

	}

}
//Calculate the breach width with an abrupt opening for walls
double _Hyd_Coupling_Dikebreak::calculate_abrupt_breach(_hyd_break_parameters *break_params, const double d_h, double *counter_stress){
	double d_breach=0.0;
	///sum up the stress
	(*counter_stress)=(*counter_stress)+(pow(abs(d_h),1.5)*this->delta_t/this->total_breach_width)*0.5;
	//check if stress is > than resistance
	if(*counter_stress>=break_params->resistance){
		*counter_stress=0.0;
		d_breach=break_params->abrupt_opening;
	}
	return d_breach;
}
//Calculate the breach width with continuous opening for dikes
double _Hyd_Coupling_Dikebreak::calculate_continuous_breach(_hyd_break_parameters *break_params, const double d_h){
	double grad_breach=0.0;
	double ideal_t=0.0;
	double fak1=this->f1*pow(constant::Cgg,0.5)/break_params->resistance;
	double fak2=this->f2*constant::Cgg/(break_params->resistance*(double)constant::hour_second);


	//ostringstream cout;

	//calculate the ideal time
	ideal_t=this->calculate_ideal_breach_time(fak1, fak2, abs(d_h));
	//if (abs(d_h) > 0.0) {

		//cout << "t_ideal " << ideal_t <<", d_h " << d_h<<" , fak1 " << fak1<< ", fak2 " <<fak2<< endl;
		//Sys_Common_Output::output_hyd->output_txt(&cout);
	//}
	
	//calculate the delta breach width
	grad_breach=fak1*fak2*pow(abs(d_h),1.5)*(1.0/(log(10.0)*(1.0+fak2*(ideal_t))));


	return grad_breach*0.5;
}
//Calculate the ideal time which would be needed by the given parameters for the breach width
double _Hyd_Coupling_Dikebreak::calculate_ideal_breach_time(const double faktor1, const double faktor2, const double d_h){
	double ideal_t=0.0;
	double pot=this->total_breach_width/(faktor1*pow(d_h,1.5));
	//numerical boundary
	if(d_h==0.0 || pot>=40.0){
		pot=40.0;
	}

	ideal_t=(1.0/faktor2)*((pow(10.0,pot))-1.0);
	return ideal_t;
}
//Calculate the total mean delta h at the breach heads 
void _Hyd_Coupling_Dikebreak::calculate_total_mean_delta_h(void){
	if(this->coupling_flag==true && this->started_flag==true){
		double mean=0.0;
		int counter=0;
		if(this->upstream_open_flag==true){
			mean=mean+(*this->deltah_upstream);
			counter++;
		}
		if(this->downstream_open_flag==true){
			mean=mean+(*this->deltah_downstream);
			counter++;
		}
		if(counter!=0){
			mean=mean/counter;
			this->total_mean_dh=this->total_mean_dh+(mean-this->total_mean_dh)/(this->breach_counter);
		}	
	}
}
//Close the output file
void _Hyd_Coupling_Dikebreak::close_output_file(void){
	if(this->output_file.is_open()==true){
		this->output_file.close();
	}
}
//Open the outpufile; if it is open it will be closed
void _Hyd_Coupling_Dikebreak::open_output_file(void){
	//close it
	this->close_output_file();

	if(this->file_name!=label::not_set){
		this->output_file.open(this->file_name.c_str());
		if(this->output_file.is_open()!=true){
			Error msg=this->set_error(0);
			throw msg;
		}
		this->output_header_result2file_tecplot();
	}
}
//Close the output file for csv
void _Hyd_Coupling_Dikebreak::close_output_file_csv(void) {
	if (this->output_file_csv.is_open() == true) {
		this->output_file_csv.close();
	}
}
//Open the outpufile for csv; if it is open it will be closed
void _Hyd_Coupling_Dikebreak::open_output_file_csv(void) {
	//close it
	this->close_output_file_csv();

	if (this->file_name_csv != label::not_set) {
		this->output_file_csv.open(this->file_name_csv.c_str());
		if (this->output_file_csv.is_open() != true) {
			Error msg = this->set_error(2);
			throw msg;
		}
		this->output_header_result2file_csv();
	}

}
//______
//private
//Set error(s)
Error _Hyd_Coupling_Dikebreak::set_error(const int err_type){
	string place="_Hyd_Coupling_Dikebreak::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://could not open the outputfile
			place.append("open_output_file(void)");
			reason="Could not open the tecplot-file for the output of the break development";
			help="Check the file";
			info << "Filename                : " <<this->file_name << endl;
			info << "Dikebreak coupling index: " << this->index << endl;
			type=5;
			break;
		case 1://local waterlevel is smaller than 0.0
			place.append("check_dikebreak_parameter(void)");
			reason="The given local waterlevel for dikebreak starting is < 0.0";
			help="Check the given starting waterlevel";
			info << "Dikebreak coupling index: " << this->index << endl;
			type=16;
			break;
		case 2://could not open the outputfile
			place.append("open_output_file_csv(void)");
			reason = "Could not open the csv-file for the output of the break development";
			help = "Check the file";
			info << "Filename                : " << this->file_name << endl;
			info << "Dikebreak coupling index: " << this->index << endl;
			type = 5;
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
Warning _Hyd_Coupling_Dikebreak::set_warning(const int warn_type){
	string place="_Hyd_Coupling_Dikebreak::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://no coupling between the river an dthe floodplain is possible
			place.append("check_dikebreak_parameter(void)") ;
			reason="The given maximum breach width is <=0.0";
			reaction="The maximum breach width is set to the dfeault breach width";
			help= "Check the given maximum breach width";
			info << "Default maximum breach width is :" << this->default_max_breach_width << label::m << endl;
			info << "Dikebreak coupling index        : " << this->index << endl;
			type=10;	
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