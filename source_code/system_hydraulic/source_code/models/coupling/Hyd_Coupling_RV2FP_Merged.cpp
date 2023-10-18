#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2FP_Merged.h"

//init static members
Tables *Hyd_Coupling_RV2FP_Merged::max_h_table=NULL;

//constructor
Hyd_Coupling_RV2FP_Merged::Hyd_Coupling_RV2FP_Merged(void){
	this->floodplain_model=NULL;
	this->river_model=NULL;
	this->list_left.set_leftriver_bank_line(true);
	this->list_right.set_leftriver_bank_line(false);
	this->direct_coupling_flag_out=false;
	this->direct_coupled_fpelem_out=NULL;
	this->direct_coupled_elem_index_out=-1;	
	this->direct_coupled_fp_out=NULL;
	this->direct_coupling_flag_in=false;
	this->direct_coupled_fpelem_in=NULL;
	this->direct_coupled_elem_index_in=-1;
	this->direct_coupled_fp_in=NULL;
	this->river_model_is_set=false;
	this->number_fp_models=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2FP_Merged)-sizeof(Hyd_Coupling_Point_RV2FP_List)*2, _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_Coupling_RV2FP_Merged::~Hyd_Coupling_RV2FP_Merged(void){
	if(this->floodplain_model!=NULL){
		delete []this->floodplain_model;
		this->floodplain_model=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2FP_Merged)-sizeof(Hyd_Coupling_Point_RV2FP_List)*2, _sys_system_modules::HYD_SYS);
}
//___________
//public
//Set the database table for the maximum waterlevels of the coupling points for each fpl-section: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Coupling_RV2FP_Merged::set_max_h_table(QSqlDatabase *ptr_database){
	if(Hyd_Coupling_RV2FP_Merged::max_h_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_coup_max;

		string tab_id_col[13];
		tab_id_col[0]=label::glob_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=label::applied_flag;
		tab_id_col[4]=hyd_label::sz_bound_id;
		tab_id_col[5]=risk_label::sz_break_id;
		tab_id_col[6]=fpl_label::section_id;
		tab_id_col[7]=hyd_label::coupling_point_max_h;
		tab_id_col[8]=hyd_label::maxh;
		tab_id_col[9]=hyd_label::coupling_point_max_h2break;
		tab_id_col[10]=hyd_label::maxh2break;
		tab_id_col[11]=hyd_label::time_maxh2break;
		tab_id_col[12]=hyd_label::time_maxh;



		try{
			Hyd_Coupling_RV2FP_Merged::max_h_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Coupling_RV2FP_Merged::max_h_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Coupling_RV2FP_Merged::set_max_h_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Coupling_RV2FP_Merged::close_max_h_table();
			throw msg;
		}
	}
}
//Create the database table for the maximum waterlevels of the coupling points for each fpl-section (static)
void Hyd_Coupling_RV2FP_Merged::create_max_h_table(QSqlDatabase *ptr_database){
	if(Hyd_Coupling_RV2FP_Merged::max_h_table==NULL){
		ostringstream cout;
		cout << "Create maximum waterlevel table for the fpl-connection..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_coup_max;
		const int num_col=13;
		_Sys_data_tab_column tab_col[num_col];
		//init
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
		tab_col[1].key_flag=true;

		tab_col[2].name=label::measure_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
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

		tab_col[6].name=fpl_label::section_id;
		tab_col[6].type=sys_label::tab_col_type_int;
		tab_col[6].unsigned_flag=true;
		tab_col[6].key_flag=true;

		tab_col[7].name=hyd_label::coupling_point_max_h;
		tab_col[7].type=sys_label::tab_col_type_int;
		tab_col[7].unsigned_flag=true;

		tab_col[8].name=hyd_label::maxh;
		tab_col[8].type=sys_label::tab_col_type_double;
		tab_col[8].unsigned_flag=true;
		tab_col[8].default_value="0.0";

		tab_col[9].name=hyd_label::time_maxh;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].unsigned_flag=true;
		tab_col[9].default_value="0.0";

		tab_col[10].name=hyd_label::coupling_point_max_h2break;
		tab_col[10].type=sys_label::tab_col_type_int;
		tab_col[10].unsigned_flag=true;

		tab_col[11].name=hyd_label::maxh2break;
		tab_col[11].type=sys_label::tab_col_type_double;
		tab_col[11].unsigned_flag=true;
		tab_col[11].default_value="0.0";

		tab_col[12].name=hyd_label::time_maxh2break;
		tab_col[12].type=sys_label::tab_col_type_double;
		tab_col[12].unsigned_flag=true;
		tab_col[12].default_value="0.0";



		try{
			Hyd_Coupling_RV2FP_Merged::max_h_table= new Tables();
			if(Hyd_Coupling_RV2FP_Merged::max_h_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Coupling_RV2FP_Merged::max_h_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Coupling_RV2FP_Merged::close_max_h_table();
			throw msg;
		}

	Hyd_Coupling_RV2FP_Merged::close_max_h_table();
	}

}
//Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section (static)
void Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Coupling_RV2FP_Merged::max_h_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section (static)
void Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier
void Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag){
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	if(like_flag==false){
		query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	}
	else{
		query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " LIKE '" << break_sz<<"'";
	}
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const string break_sz, const bool like_flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier (static)
void Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_sz, const string break_sc){
//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc <<"'";
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_bound_sz <<"";

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::delete_data_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_sz, const string break_sc)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
}
//Close and delete the database table for the maximum waterlevels of the coupling points for each fpl-section (static)
void Hyd_Coupling_RV2FP_Merged::close_max_h_table(void){
	if(Hyd_Coupling_RV2FP_Merged::max_h_table!=NULL){
		delete Hyd_Coupling_RV2FP_Merged::max_h_table;
		Hyd_Coupling_RV2FP_Merged::max_h_table=NULL;
	}
}
//Switch the applied-flag in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier (static)
void Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier (static)
void Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;



	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select the point id and the maximum waterlevel in the database table for the maximum waterlevels of the coupling points for each fpl-section for the given identifier
int Hyd_Coupling_RV2FP_Merged::set_hmax_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h){

	QSqlQueryModel query;
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h)  <<" , ";
	query_string <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh)  <<"  ";	
	query_string << " FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<<"'";
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(fpl_label::section_id) << " = " << fpl_sec;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::set_hmax_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//read the data out		
	*point_id=query.record(0).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h)).c_str()).toInt();
	*max_h=query.record(0).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh)).c_str()).toDouble();

	return query.rowCount();
}
//Select the point id and the maximum waterlevel in the database table for the maximum waterlevels for a break between river- and floodplainmodel
int Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h2break){

	QSqlQueryModel query;
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT ";
	query_string <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h2break)  <<" , ";
	query_string <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh2break)  <<"  ";	
	query_string << " FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<<"'";
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(fpl_label::section_id) << " = " << fpl_sec;


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, const int fpl_sec, int *point_id, double *max_h2break)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	//read the data out		
	*point_id=query.record(0).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h2break)).c_str()).toInt();
	*max_h2break=query.record(0).value((Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh2break)).c_str()).toDouble();

	return query.rowCount();
}
//Select all maximum waterlevel in the database table for the maximum waterlevels for a break between river- and floodplainmodel for a given scenario (static)
int Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, QSqlQueryModel *query){

	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream query_string;
	query_string <<"SELECT * ";	
	query_string << " FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string << " WHERE " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) << "= true";
	query_string  << " AND ";
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " ="  << id.area_state;
	query_string  << " AND (";
	query_string  <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << 0 ;
	query_string  << " OR " ;
	query_string  << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string  << " ) " ;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc;
	query_string << " AND ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sc<<"'";
	query_string << " ORDER BY ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::time_maxh);


	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::set_hmax2break_in_max_h_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_sc, const string break_sc, QSqlQueryModel *query)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	
	return query->rowCount();
}
//Copy the results of a given system id to another one in database table (static)
void Hyd_Coupling_RV2FP_Merged::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	int glob_id=0;
	glob_id=Hyd_Coupling_RV2FP_Merged::max_h_table->maximum_int_of_column(Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id);
	test_filter << " from " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) << " = " << src.measure_nr ;


	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	ostringstream cout;
	cout <<"Copy "<<model.rowCount()<<" maximum waterlevel results to the new measure state..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		test_filter.str("");
		test_filter << "INSERT INTO "<< Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh)  <<" , ";
		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h2break)  <<" , ";

		test_filter <<  Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh2break)  <<"  ";

		test_filter << " FROM " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() <<" ";
		test_filter << " WHERE " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Hyd_Coupling_RV2FP_Merged::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}
	try{
		Hyd_Coupling_RV2FP_Merged::switch_applied_flag_max_h_table(ptr_database, src, false);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output final results of the maximum waterlevel of the coupling points to database (static)
void Hyd_Coupling_RV2FP_Merged::output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc, const int fpl_sec_id, const _hyd_max_values max_h, const int point_id_h_max, const _hyd_max_values max_h2break, const int point_id_h_max2break){
//the table is set (the name and the column names) and allocated
	try{
		Hyd_Coupling_RV2FP_Merged::set_max_h_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	QSqlQuery model(*ptr_database);

	int glob_id=0;
	glob_id=Hyd_Coupling_RV2FP_Merged::max_h_table->maximum_int_of_column(Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id),ptr_database)+1;

	ostringstream query_string;
	query_string << "INSERT INTO  " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name();
	query_string <<" ( ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::glob_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::areastate_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::measure_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(label::applied_flag) <<" , ";

	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::time_maxh) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::coupling_point_max_h2break) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::maxh2break) <<" , ";
	query_string << Hyd_Coupling_RV2FP_Merged::max_h_table->get_column_name(hyd_label::time_maxh2break) <<" ) ";


	query_string << " VALUES ( ";
	query_string << glob_id << " , " ;
	query_string << id.area_state<< " , " ;
	query_string << id.measure_nr<< " , " ;
	query_string << bound_sc<< " , " ;
	query_string <<"'"<< id_break<< "' , " ;

	query_string <<""<< functions::convert_boolean2string(true) << " , " ;

	query_string << fpl_sec_id << " , " ;
	
	query_string << point_id_h_max << " , " ;
	query_string << max_h.maximum << " , ";
	query_string << max_h.time_point << " , ";
	query_string << point_id_h_max2break << " , " ;
	query_string << max_h2break.maximum << " , ";
	query_string << max_h2break.time_point << " ) ";


	Data_Base::database_request(&model, query_string.str(), ptr_database);
	
	if(model.lastError().isValid()){
		Warning msg;
		msg.set_msg("Hyd_Coupling_RV2FP_Merged::output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc, const int fpl_sec_id, const _hyd_max_values max_h, const int point_id_h_max, const _hyd_max_values max_h2break, const int point_id_h_max2break)","Invalid database request", "Check the database","Can not transfer the results",2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Coupling_RV2FP_Merged::max_h_table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}
}
//The pointer of the models for couplings are setted and the lists are merged
void Hyd_Coupling_RV2FP_Merged::set_ptr_coupling_with_merging(Hyd_Coupling_RV2FP *couplings){
	if(couplings==NULL){
		return;
	}
	else{
		if(this->river_model==NULL && couplings->get_is_merged()==false){
			this->river_model_is_set=true;
			this->river_model=couplings->get_river_model();
			this->add_floodplainmodel_pointer(couplings->get_floodplain_model());
			//set the defining polysegment
			this->list_left.set_defining_polysegment(couplings->list_left.get_defining_polysegment());
			this->list_right.set_defining_polysegment(couplings->list_right.get_defining_polysegment());
			//merge the lists
			couplings->list_left.merge_coupling_list(&this->list_left);
			couplings->list_right.merge_coupling_list(&this->list_right);
			couplings->set_is_merged();

		}
		else if(this->river_model==couplings->get_river_model() && couplings->get_is_merged()==false){
			this->add_floodplainmodel_pointer(couplings->get_floodplain_model());
			//merge the lists
			couplings->list_left.merge_coupling_list(&this->list_left);
			couplings->list_right.merge_coupling_list(&this->list_right);
			couplings->set_is_merged();
		}
	}
}
//Set up the list of fpl-section ids in the total merged lists (left/right)
void Hyd_Coupling_RV2FP_Merged::set_up_fpl_section_ids_list(void){
	this->list_left.set_list_fpl_sec_id();
	this->list_right.set_list_fpl_sec_id();
}
//Get the coupled river index
int Hyd_Coupling_RV2FP_Merged::get_river_index(void){
	if(this->river_model!=NULL){
		return this->river_model->Param_RV.get_river_number();
	}
	else{
		return -1;
	}
}
//Get a pointer to the coupled river model
Hyd_Model_River* Hyd_Coupling_RV2FP_Merged::get_river_model(void){
	return this->river_model;
}
//Get the flag if the river model is already setted
bool Hyd_Coupling_RV2FP_Merged::get_river_model_is_setted(void){
	return this->river_model_is_set;
}
//Get the number of coupled floodplains
int Hyd_Coupling_RV2FP_Merged::get_number_coupled_fp(void){
	return this->number_fp_models;
}
//Get the index of a coupled floodplain model by the given index (number of coupled floodplain models)
int Hyd_Coupling_RV2FP_Merged::get_index_coupled_fp_models(const int index){
	int index_fp=-1;
	if(index<0 || index >=this->number_fp_models){
		return index_fp=-1;
	}
	else{
		index_fp=this->floodplain_model[index]->Param_FP.get_floodplain_number();
	}

	return index_fp;
}
//Synchronise the models
void Hyd_Coupling_RV2FP_Merged::synchronise_models(const double timepoint, const double delta_t,  const bool time_check, const int internal_counter){
	try{
		this->list_left.syncronisation_models_bylistpoints(timepoint,delta_t, time_check, internal_counter);
		this->list_right.syncronisation_models_bylistpoints(timepoint, delta_t, time_check, internal_counter);
		if(time_check==false){
			this->synchronise_direct_coupling_out();
			this->synchronise_direct_coupling_in();
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Synchronisation between floodplains ";
		for(int i=0; i<this->number_fp_models; i++){
			info << this->floodplain_model[i]->Param_FP.get_floodplain_number() << ", ";
		}
		info<< " and river model " << this->river_model->Param_RV.get_river_number() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the maximum waterlevel gradient 
double Hyd_Coupling_RV2FP_Merged::get_maximum_h_grad(void){
	double buffer=0.0;
	double buffer_max=0.0;
	
		
	buffer=this->list_left.get_maximum_h_grad();
	if(buffer_max<buffer){
		buffer_max=buffer;
	}
	
	buffer=this->list_right.get_maximum_h_grad();
	if(buffer_max<buffer){
		buffer_max=buffer;
	}

	return buffer_max;
}
//Reset the coupling discharges
void  Hyd_Coupling_RV2FP_Merged::reset_coupling_discharges(void){
		this->list_left.reset_coupling_discharge();
		this->list_right.reset_coupling_discharge();
		this->reset_direct_coupling_discharges_out();
		this->reset_direct_coupling_discharges_in();
}
//Output the header for the coupled model indizes
void  Hyd_Coupling_RV2FP_Merged::output_header_coupled_indices(ostringstream *cout){
		*cout << "River to multiple floodplains..." << endl ;
		*cout << "No." << W(12) << "Id_RV" << W(12) << "No_FPs" << W(12) << "Id_FPs"<< endl ;
		Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the indexes of the coupled model
void  Hyd_Coupling_RV2FP_Merged::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	*cout << this->river_model->Param_RV.get_river_number() << W(12);
	*cout << this->number_fp_models<<W(12);
	for(int i=0; i<this->number_fp_models; i++){
			*cout << this->floodplain_model[i]->Param_FP.get_floodplain_number() << "  ";
	}
	*cout <<endl;
	if(this->direct_coupling_flag_in==true){
		*cout<< "2d->1D river inflow; Id_FP " << this->direct_coupled_fp_in->Param_FP.get_floodplain_number() << "; ID_Elem " << this->direct_coupled_elem_index_in << endl;
	}
	if(this->direct_coupling_flag_out==true){
		*cout<< "1d->2D river outflow; Id_FP " << this->direct_coupled_fp_out->Param_FP.get_floodplain_number() << "; ID_Elem " << this->direct_coupled_elem_index_out << endl;
	}

	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Output the setted coupling points in the list
void  Hyd_Coupling_RV2FP_Merged::output_setted_coupling_points(void){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RV_" << this->river_model->Param_RV.get_river_number()<<"_FP_MERG> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "List of coupling points (left river bank)" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->list_left.output_setted_members(&cout);
	cout << "List of coupling points (right river bank)" << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	this->list_right.output_setted_members(&cout);
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Calculate and output final results of the maximum waterlevel of the coupling points to database
void Hyd_Coupling_RV2FP_Merged::calc_output_final_results(QSqlDatabase *ptr_database, const _sys_system_id id, const string id_break, const int bound_sc){

	int sec_id=0;
	int number_sec=0;
	_hyd_max_values h_max;
	h_max.maximum=0.0;
	h_max.time_point=0.0;
	int point_id_h_max=0;
	_hyd_max_values h_max2break;
	h_max2break.maximum=0.0;
	h_max2break.time_point=0.0;
	int point_id_h_max2break=0;
	//left list
	number_sec=this->list_left.get_ptr_fpl_section_ids()->count();
	for(int i=0; i< number_sec; i++){
		sec_id=this->list_left.get_ptr_fpl_section_ids()->at(i);
		this->list_left.get_max_h2fpl(sec_id, &h_max, &point_id_h_max);
		this->list_left.get_max_h2break(sec_id, &h_max2break, &point_id_h_max2break);
		Hyd_Coupling_RV2FP_Merged::output_final_results(ptr_database, id, id_break, bound_sc, sec_id, h_max, point_id_h_max, h_max2break, point_id_h_max2break);
	}
	//right list
	number_sec=this->list_right.get_ptr_fpl_section_ids()->count();
	for(int i=0; i< number_sec; i++){
		sec_id=this->list_right.get_ptr_fpl_section_ids()->at(i);
		this->list_right.get_max_h2fpl(sec_id, &h_max, &point_id_h_max);
		this->list_right.get_max_h2break(sec_id, &h_max2break, &point_id_h_max2break);
		Hyd_Coupling_RV2FP_Merged::output_final_results(ptr_database, id, id_break, bound_sc, sec_id, h_max, point_id_h_max, h_max2break, point_id_h_max2break);
	}
	
	
}
//Check if the river outflow profile is in a floodplain model and is not coupeld to other river models=> outflow goes to the floodplain
void Hyd_Coupling_RV2FP_Merged::check_river_outflow2floodplain(void){

	//check if the last points of the couplings line are inside the floodplain
	if(this->list_left.get_number_couplings()>0 &&  this->list_right.get_number_couplings()>0){
		if(this->list_left.get_last_couplingpoint().get_coupling_flag()==true && this->list_right.get_last_couplingpoint().get_coupling_flag()==true){
			//check if there is an coupling or a boundary is set to the outflow profile => no direct coupling
			if(this->river_model->outflow_river_profile.get_h_outflow_is_given()!=true && this->river_model->outflow_river_profile.get_boundary_waterlevel_flag()!=true){
				this->direct_coupling_flag_out=true;
				try{
					//init the direct coupling if needed
					this->init_direct_coupling_out();
				}
				catch(Error msg){
					throw msg;
				}
			}
		}
	}
}
//Get the flag, if direct RV2FP-coupling out of the outflow-profile is set
bool Hyd_Coupling_RV2FP_Merged::get_direct_coupling_flag_out(void){
	return this->direct_coupling_flag_out;
}
//Check and set if the river inflow profile is in a floodplain model and is not coupeld to other river models=> inflow comes from the floodplain
void Hyd_Coupling_RV2FP_Merged::check_river_inflow2floodplain(void){

	if(this->list_left.get_number_couplings()>0 &&  this->list_right.get_number_couplings()>0){
		//check if the first points of the coupling line are inside the floodplain
		if(this->list_left.get_first_couplingpoint().get_coupling_flag()==true && this->list_right.get_first_couplingpoint().get_coupling_flag()==true){
			//check if there is an coupling or a boundary is set to the inflow profile => no direct coupling
			if(this->river_model->inflow_river_profile.get_q_inflow_is_given()!=true && this->river_model->inflow_river_profile.get_boundary_point_flag()!=true){
				this->direct_coupling_flag_in=true;
				try{
					//init the direct coupling if needed
					this->init_direct_coupling_in();
				}
				catch(Error msg){
					throw msg;
				}
			}
		}
	}
}
//Get the flag, if direct RV2FP-coupling into a inflow profile is set
bool Hyd_Coupling_RV2FP_Merged::get_direct_coupling_flag_in(void){
	return this->direct_coupling_flag_in;
}
//Check coupling from one bank line to the other side of the river; the coupling is than not possible
void Hyd_Coupling_RV2FP_Merged::check_coupling2_other_river_side(void){
	if(this->river_model!=NULL){
		this->list_left.check_intercepting_riverbankline2connecting_segment(&this->river_model->river_rightline);
		this->list_right.check_intercepting_riverbankline2connecting_segment(&this->river_model->river_leftline);
	}
}
//Check coupling from one bank line to the other side of other rivers; the coupling is than not possible
void Hyd_Coupling_RV2FP_Merged::check_coupling2_other_river_side(Hyd_Coupling_RV2FP_Merged *other_river){
	if(other_river!=NULL){
		bool same_flag=false;
		//check if there are same floodplains
		for(int i=0; i<this->number_fp_models; i++){
			for(int j=0; j < this->get_number_coupled_fp(); j++){
				if(this->get_index_coupled_fp_models(i)==other_river->get_index_coupled_fp_models(j)){
					same_flag=true;
					break;
				}
			}
		}
		//check the intercepting
		if(same_flag==true){
			this->list_left.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_rightline);
			this->list_right.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_rightline);
			this->list_left.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_leftline);
			this->list_right.check_intercepting_riverbankline2connecting_segment(&other_river->get_river_model()->river_leftline);
		}
	}
}
//Clone the river to floodplain merged coupling
void Hyd_Coupling_RV2FP_Merged::clone_couplings(Hyd_Coupling_RV2FP_Merged *coupling, Hyd_Hydraulic_System *system){
	this->direct_coupling_flag_out=coupling->direct_coupling_flag_out;
	this->direct_coupled_elem_index_out=coupling->direct_coupled_elem_index_out;	
	this->direct_coupling_flag_in=coupling->direct_coupling_flag_in;
	this->direct_coupled_elem_index_in=coupling->direct_coupled_elem_index_in;
	this->river_model_is_set=coupling->river_model_is_set;
	
	if(coupling->direct_coupled_fp_in!=NULL){
		this->direct_coupled_fp_in=system->get_ptr_floodplain_model(coupling->direct_coupled_fp_in->Param_FP.get_floodplain_number());	
		this->direct_coupled_fpelem_in=this->direct_coupled_fp_in->get_ptr_floodplain_elem(this->direct_coupled_elem_index_in);
		if(this->direct_coupled_fpelem_in!=NULL){
			this->direct_coupled_fpelem_in->element_type->set_coupling_data();
		}
	}
	if(coupling->direct_coupled_fp_out!=NULL){
		this->direct_coupled_fp_out=system->get_ptr_floodplain_model(coupling->direct_coupled_fp_out->Param_FP.get_floodplain_number());	
		this->direct_coupled_fpelem_out=this->direct_coupled_fp_out->get_ptr_floodplain_elem(this->direct_coupled_elem_index_out);
		if(this->direct_coupled_fpelem_out!=NULL){
			this->direct_coupled_fpelem_out->element_type->set_coupling_data();
		}
	}

	if(coupling->river_model!=NULL){
		this->river_model=system->get_ptr_river_model(coupling->river_model->Param_RV.get_river_number());
	}
	//set the fp-models
	for(int i=0; i< coupling->number_fp_models; i++){
		Hyd_Model_Floodplain *buffer;
		buffer=system->get_ptr_floodplain_model(coupling->floodplain_model[i]->Param_FP.get_floodplain_number());
		this->add_floodplainmodel_pointer(buffer);
	}
	//the lists
	this->list_left.clone_list(&coupling->list_left, system, this->river_model->Param_RV.get_river_number());
	this->list_right.clone_list(&coupling->list_right, system, this->river_model->Param_RV.get_river_number());
}
//__________
//private
//Add a pointer of additional floodplain models
void Hyd_Coupling_RV2FP_Merged::add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer){

	if(pointer==NULL){
		return;
	}

	//check if it is the same
	for(int i=0; i< this->number_fp_models; i++){
		if(this->floodplain_model[i]==pointer){
			return;
		}
	}

	//allocate a buffer
	Hyd_Model_Floodplain **buffer=NULL;
	try{
		(buffer)=new Hyd_Model_Floodplain*[this->number_fp_models];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy it
	for(int i=0; i< this->number_fp_models; i++){
		buffer[i]=this->floodplain_model[i];
	}
	//delete the pointer
	if(this->floodplain_model!=NULL){
		delete []this->floodplain_model;
		this->floodplain_model=NULL;
	}
	//count up
	this->number_fp_models++;
	//allocate new
	try{
		(this->floodplain_model)=new Hyd_Model_Floodplain*[this->number_fp_models];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//recopy it
	for(int i=0; i< this->number_fp_models-1; i++){
		this->floodplain_model[i]=buffer[i];
	}
	//set the last pointer
	this->floodplain_model[this->number_fp_models-1]=pointer;

	//delete the buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}
}
//Initialize the direct RV2FP-coupling out of the outflow-profile
void Hyd_Coupling_RV2FP_Merged::init_direct_coupling_out(void){
	if(this->direct_coupling_flag_out==true){
		//get the mid point of the outflow profile
		Geo_Point mid_point=this->river_model->outflow_river_profile.typ_of_profile->mid_point_global_x_y;

		for(int i=0; i< this->number_fp_models;i++){
			//get the element
			this->direct_coupled_elem_index_out=this->floodplain_model[i]->raster.find_elem_index_by_point_withboundary(&mid_point);
			if(this->direct_coupled_elem_index_out>=0){
				if(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_out].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
					this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_out].get_elem_type()==_hyd_elem_type::STANDARD_ELEM){
						this->direct_coupling_flag_out=true;
						this->direct_coupled_fpelem_out=&(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_out]);
						if(this->direct_coupled_fpelem_out!=NULL){
							this->direct_coupled_fpelem_out->element_type->set_coupling_data();
						}
						this->direct_coupled_fp_out=this->floodplain_model[i];	
				}
				else if(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_out].get_elem_type()==_hyd_elem_type::NOFLOW_ELEM){
						this->direct_coupling_flag_out=false;
						this->direct_coupled_fpelem_out=NULL;
						this->direct_coupled_elem_index_out=-1;
						this->direct_coupled_fp_out=NULL;
						//warning
						Warning msg=this->set_warning(0);
						ostringstream info;
						info << "River number      : " << this->river_model->Param_RV.get_river_number()<<endl;
						info << "River name        : " << this->river_model->Param_RV.get_river_name()<<endl;
						info << "Floodplain number : " << this->floodplain_model[i]->Param_FP.get_floodplain_number()<<endl;
						info << "Floodplain name   : " << this->floodplain_model[i]->Param_FP.get_floodplain_name()<<endl;
						msg.make_second_info(info.str());
						msg.output_msg(2);

				}
				//search for a convenient neighbouring element
				else{
					this->floodplain_model[i]->raster.search_convenient_coupling_element(&this->direct_coupled_elem_index_out,this->river_model->outflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates(), 
						this->river_model->outflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates(),true);
					if(this->direct_coupled_elem_index_out>=0){
						this->direct_coupling_flag_out=true;
						this->direct_coupled_fpelem_out=&(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_out]);
						this->direct_coupled_fp_out=this->floodplain_model[i];
					}
					//no convenient element is found
					else{
						this->direct_coupling_flag_out=false;
						this->direct_coupled_fpelem_out=NULL;
						this->direct_coupled_elem_index_out=-1;
						this->direct_coupled_fp_out=NULL;
					}

				}			
			}
			//no element is found
			else{
				this->direct_coupling_flag_out=false;
				this->direct_coupled_fpelem_out=NULL;
				this->direct_coupled_elem_index_out=-1;
				this->direct_coupled_fp_out=NULL;
			}
		}

	}
	//check it
	try{
		this->check_direct_coupling_out();
	}
	catch(Error msg){
		throw msg;
	}
	//set the is direct set flag to the outflowprofile
	if(this->direct_coupling_flag_out==true){
		this->river_model->outflow_river_profile.set_h_outflow_is_given();
	}
}
//Synchronise the direct RV2FP-coupling out of the outflow-profile
void Hyd_Coupling_RV2FP_Merged::synchronise_direct_coupling_out(void){
	if(this->direct_coupling_flag_out==true){
		double value=0.0;
		//get discharge from the river outflow profile
		value=this->river_model->outflow_river_profile.get_actual_river_discharge();
		//set it to the floodplain element
		this->direct_coupled_fpelem_out->element_type->add_coupling_discharge_rv_direct(value);
		//get the height of the floodplain element
		value=this->direct_coupled_fpelem_out->element_type->get_s_value();
		this->river_model->outflow_river_profile.set_h_outflow_global(value);

	}
}
//Reset the direct RV2FP-coupling out of the outflow-profile
void Hyd_Coupling_RV2FP_Merged::reset_direct_coupling_discharges_out(void){
	if(this->direct_coupling_flag_out==true){
		this->direct_coupled_fpelem_out->element_type->reset_coupling_discharge_rv_direct();
	}
}
//Check the direct coupling
void Hyd_Coupling_RV2FP_Merged::check_direct_coupling_out(void){
	if(this->direct_coupling_flag_out==true){
		if(this->river_model->outflow_river_profile.typ_of_profile->get_global_z_min()<this->direct_coupled_fpelem_out->get_z_value()){
			Error msg=this->set_error(0);
			ostringstream info;
			info <<"Outflow profile height: " << this->river_model->outflow_river_profile.typ_of_profile->get_global_z_min()<< label::m << endl;
			info <<"Element height: " <<this->direct_coupled_fpelem_out->get_z_value()<<label::m << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Initialize the direct RV2FP-coupling into a inflow-profile
void Hyd_Coupling_RV2FP_Merged::init_direct_coupling_in(void){
	if(this->direct_coupling_flag_in==true){
		//get the mid point of the inflow profile
		Geo_Point mid_point=this->river_model->inflow_river_profile.typ_of_profile->mid_point_global_x_y;
		for(int i=0; i< this->number_fp_models;i++){
		//get the element
			this->direct_coupled_elem_index_in=this->floodplain_model[i]->raster.find_elem_index_by_point_withboundary(&mid_point);
			if(this->direct_coupled_elem_index_in>=0){
				if(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_in].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
					this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_in].get_elem_type()==_hyd_elem_type::STANDARD_ELEM){
						this->direct_coupling_flag_in=true;
						this->direct_coupled_fpelem_in=&(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_in]);
						if(this->direct_coupled_fpelem_in!=NULL){
							this->direct_coupled_fpelem_in->element_type->set_coupling_data();
						}
						this->direct_coupled_fp_in=this->floodplain_model[i];
				}
				else if(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_in].get_elem_type()==_hyd_elem_type::NOFLOW_ELEM){
						this->direct_coupling_flag_in=false;
						this->direct_coupled_fpelem_in=NULL;
						this->direct_coupled_elem_index_in=-1;
						this->direct_coupled_fp_in=NULL;
						//warning
						Warning msg=this->set_warning(0);
						ostringstream info;
						info << "River number      : " << this->river_model->Param_RV.get_river_number()<<endl;
						info << "River name        : " << this->river_model->Param_RV.get_river_name()<<endl;
						info << "Floodplain number : " << this->floodplain_model[i]->Param_FP.get_floodplain_number()<<endl;
						info << "Floodplain name   : " << this->floodplain_model[i]->Param_FP.get_floodplain_name()<<endl;
						msg.make_second_info(info.str());
						msg.output_msg(2);


				}
				//search for a convenient neighbouring element
				else{
					this->floodplain_model[i]->raster.search_convenient_coupling_element(&this->direct_coupled_elem_index_in,this->river_model->inflow_river_profile.typ_of_profile->get_first_point()->get_global_x_y_coordinates(), 
						this->river_model->inflow_river_profile.typ_of_profile->get_last_point()->get_global_x_y_coordinates(),false);
					if(this->direct_coupled_elem_index_in>=0){
						this->direct_coupling_flag_in=true;
						this->direct_coupled_fpelem_in=&(this->floodplain_model[i]->floodplain_elems[this->direct_coupled_elem_index_in]);
						this->direct_coupled_fp_in=this->floodplain_model[i];
					}
					//no convenient element is found
					else{
						this->direct_coupling_flag_in=false;
						this->direct_coupled_fpelem_in=NULL;
						this->direct_coupled_elem_index_in=-1;
						this->direct_coupled_fp_in=NULL;
					}

				}			
			}
			//no element is found
			else{
				this->direct_coupling_flag_in=false;
				this->direct_coupled_fpelem_in=NULL;
				this->direct_coupled_elem_index_in=-1;
				this->direct_coupled_fp_in=NULL;
			}
		}
	}
	//check it
	try{
		this->check_direct_coupling_in();
	}
	catch(Error msg){
		throw msg;
	}
	//set the is direct set flag to the inflowprofile
	if(this->direct_coupling_flag_in==true){
		this->river_model->inflow_river_profile.set_h_inflow_is_given();
	}
}
//Synchronise the direct RV2FP-coupling into a inflow-profile
void Hyd_Coupling_RV2FP_Merged::synchronise_direct_coupling_in(void){
	if(this->direct_coupling_flag_in==true){
		double value=0.0;
		//get discharge from the river inflow profile
		value=this->river_model->inflow_river_profile.get_actual_river_discharge();
		//set it to the floodplain element
		this->direct_coupled_fpelem_in->element_type->add_coupling_discharge_rv_direct((-1.0)*value);
		//get the height of the floodplain element
		value=this->direct_coupled_fpelem_in->element_type->get_s_value();
		//set the height to the inflow profile
		this->river_model->inflow_river_profile.set_global_h_boundary(value);
	}
}
//Reset the direct RV2FP-coupling into a inflow-profile
void Hyd_Coupling_RV2FP_Merged::reset_direct_coupling_discharges_in(void){
	if(this->direct_coupling_flag_in==true){
		this->direct_coupled_fpelem_in->element_type->reset_coupling_discharge_rv_direct();
	}
}
//Check the direct coupling into a inflow-profile
void Hyd_Coupling_RV2FP_Merged::check_direct_coupling_in(void){
	if(this->direct_coupling_flag_in==true){
		if(this->river_model->inflow_river_profile.typ_of_profile->get_global_z_min()<this->direct_coupled_fpelem_in->get_z_value()){
			Error msg=this->set_error(1);
			ostringstream info;
			info <<"Inflow profile height: " << this->river_model->inflow_river_profile.typ_of_profile->get_global_z_min()<< label::m << endl;
			info <<"Element height: " <<this->direct_coupled_fpelem_in->get_z_value()<<label::m << endl;
			msg.make_second_info(info.str());
			throw msg;
		}	
	}
}
//set the error
Error Hyd_Coupling_RV2FP_Merged::set_error(const int err_type){
	string place="Hyd_Coupling_RV2FP_Merged::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://the element height is above the outflow profile
			place.append("check_direct_coupling_out(void)");
			reason="The direct coupled floodplain element is above the minimum global height of the outflow profile";
			help="Check geometry of the floodplain and the river outflow";
			info<<"Riverno.: "<< this->river_model->Param_RV.get_river_number() << endl;
			info <<"Floodplainno.: "<<this->direct_coupled_fp_out->Param_FP.get_floodplain_number();
			info << "; Elementno.: " << this->direct_coupled_elem_index_out << endl;
			type=16;
			break;
		case 1://the element height is above the iflow profile
			place.append("check_direct_coupling_in(void)");
			reason="The direct coupled floodplain element is above the minimum global height of the inflow profile";
			help="Check geometry of the floodplain and the river inflow";
			info<<"Riverno.: "<< this->river_model->Param_RV.get_river_number() << endl;
			info <<"Floodplainno.: "<<this->direct_coupled_fp_in->Param_FP.get_floodplain_number();
			info << "; Elementno.: " << this->direct_coupled_elem_index_in << endl;
			type=16;
			break;
		case 2://bad alloc
			place.append("add_floodplainmodel_pointer(Hyd_Model_Floodplain *pointer)");
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
Warning Hyd_Coupling_RV2FP_Merged::set_warning(const int warn_type){
		string place="Hyd_Coupling_RV2FP_Merged::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no direct inflow coupling is set
			place.append("init_direct_coupling_in(void)");
			reason="The inflow profile of the river is inside a floodplain directly at an explicitly set noflow-element";
			help="Consider the physical plausibility of the model";
			reaction="No direct inflow-coupling is set";
			type=10;
			break;
		case 1://no direct outflow coupling is set
			place.append("init_direct_coupling_in(void)");
			reason="The outflow profile of the river is inside a floodplain directly at an explicitly set noflow-element";
			help="Consider the physical plausibility of the model";
			reaction="No direct outflow-coupling is set";
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
};