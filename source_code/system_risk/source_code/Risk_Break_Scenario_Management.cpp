#include "Risk_Headers_Precompiled.h"
//#include "Risk_Break_Scenario_Management.h"

//init static members
Tables *Risk_Break_Scenario_Management::table=NULL;
Tables *Risk_Break_Scenario_Management::table_fpl_sec_prob=NULL;

//Default constructor
Risk_Break_Scenario_Management::Risk_Break_Scenario_Management(void){

	this->scenarios2calc=NULL;
	this->number_sc2calc=0;
	this->number_fpl_section=0;
	this->fpl_sections=NULL;

	this->risk_type=_risk_type::scenario_risk;

	Sys_Memory_Count::self()->add_mem(sizeof(Risk_Break_Scenario_Management),_sys_system_modules::RISK_SYS);//count the memory

}
//Default destructor
Risk_Break_Scenario_Management::~Risk_Break_Scenario_Management(void){
	this->delete_sc2calc();
	this->delete_fpl_section();
	
	Sys_Memory_Count::self()->minus_mem(sizeof(Risk_Break_Scenario_Management),_sys_system_modules::RISK_SYS);//count the memory
}
//_________
//public
//Set the database table for the configuration: it sets the table name and the name of the columns and allocate them (static)
void Risk_Break_Scenario_Management::set_table(QSqlDatabase *ptr_database){
	if(Risk_Break_Scenario_Management::table==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_sz_break;
		string tab_col[12];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=risk_label::sz_break_id;
		tab_col[5]=risk_label::sz_break_prob;
		tab_col[6]=risk_label::risk_type;
		tab_col[7]=risk_label::sz_break_src;
		tab_col[8]=risk_label::sz_no_predict_break_prob;
		tab_col[9]=risk_label::sz_predict_break_prob;
		tab_col[10]=risk_label::sz_min_bound_break_prob;
		tab_col[11]=risk_label::sz_max_bound_break_prob;

		
		//set the table
		try{
			Risk_Break_Scenario_Management::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_Break_Scenario_Management::table->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_Break_Scenario_Management::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_Break_Scenario_Management::close_table();
			throw msg;
		}
	}
}
//Create the database table for configuration (static)
void Risk_Break_Scenario_Management::create_table(QSqlDatabase *ptr_database){
		if(Risk_Break_Scenario_Management::table==NULL){
			ostringstream cout;
			cout << "Create database table of break scenarios..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_sz_break;
			const int num_col=12;
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

			tab_col[3].name=hyd_label::sz_bound_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=risk_label::sz_break_id;
			tab_col[4].type=sys_label::tab_col_type_string;
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::sz_break_prob;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=risk_label::sz_min_bound_break_prob;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=risk_label::sz_max_bound_break_prob;
			tab_col[7].type=sys_label::tab_col_type_double;
			tab_col[7].unsigned_flag=true;

			tab_col[8].name=risk_label::sz_no_predict_break_prob;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=risk_label::sz_predict_break_prob;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].unsigned_flag=true;

			tab_col[10].name=risk_label::risk_type;
			tab_col[10].type=sys_label::tab_col_type_string;
			tab_col[10].key_flag=true;

			tab_col[11].name=risk_label::sz_break_src;
			tab_col[11].type=sys_label::tab_col_type_string;

			



			try{
				Risk_Break_Scenario_Management::table= new Tables();
				if(Risk_Break_Scenario_Management::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_Break_Scenario_Management::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_Break_Scenario_Management::close_table();
				throw msg;
			}
			Risk_Break_Scenario_Management::close_table();
		
	}
}
//Delete all data in the database table for the configuration to the default values (static)
void Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_Break_Scenario_Management::table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the probabilities of the break scenarios for the specified parameters (static)
void Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the probabilities of the break scenarios for the specified parameters (static)
void Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete all data in the database table for the probabilities of the break scenarios for the specified parameters (static)
void Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id){


	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc_id;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::delete_data_in_table(QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select the data in the database table probabilities of the break scenarios for the specified parameters (static)
int Risk_Break_Scenario_Management::select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc_id;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const int hyd_sc_id, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();
}
//Select the data in the database table probabilities of the break scenarios for the specified parameters 
int Risk_Break_Scenario_Management::select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '" << Risk_Break_Scenario::convert_risk_type2txt(type) <<"'";
	
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::select_data_in_table(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _risk_type type, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();
}
//Copy the results of a given system id to another one in database tables (static)
void Risk_Break_Scenario_Management::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest){
//the table is set (the name and the column names) and allocated
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Risk_Break_Scenario_Management::table->maximum_int_of_column(Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) ,ptr_database)+1;


	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id);
	test_filter << " from " << Risk_Break_Scenario_Management::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	
	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	test_filter.str("");

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Risk_Break_Scenario_Management::table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_min_bound_break_prob)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_max_bound_break_prob)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_no_predict_break_prob)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_predict_break_prob)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type)  <<" , ";
		test_filter <<  Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src)  <<"  ";

		test_filter << " FROM " << Risk_Break_Scenario_Management::table->get_table_name() <<" ";
		test_filter << " WHERE " << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) << " = ";
		test_filter << model.record(i).value(Risk_Break_Scenario_Management::table->get_column_name(label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Risk_Break_Scenario_Management::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}

}
//Check if the break scenario is existing in database table (static)
bool Risk_Break_Scenario_Management::check_break_scenario_exists(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_id, const string break_sc_id){
//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"SELECT * FROM ";
	query_string << Risk_Break_Scenario_Management::table->get_table_name();
	query_string  << " WHERE ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = "<< hyd_bound_id;
	query_string  << " AND ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id) << " = '"<< break_sc_id<<"'";
	query_string  << " AND ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '"<<Risk_Break_Scenario::convert_risk_type2txt(_risk_type::catchment_risk) << "'  ";


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::check_break_scenario_exists(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_id, const string break_sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(query.rowCount()>0){
		return true;
	}
	else{
		return false;
	}
}
//Close and delete the database table for the configuration (static)
void Risk_Break_Scenario_Management::close_table(void){
	if(Risk_Break_Scenario_Management::table!=NULL){
		delete Risk_Break_Scenario_Management::table;
		Risk_Break_Scenario_Management::table=NULL;
	}
}
//Set results to the table of the probabilities of the break scenarios
void Risk_Break_Scenario_Management::set_result2table(Risk_Break_Scenario *scenario, QSqlDatabase *ptr_database, const int glob_id){

	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery model(*ptr_database);
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Risk_Break_Scenario_Management::table->get_table_name();
	query_string <<" ( ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src) <<" , ";
	query_string << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob) <<" ) ";
	
	query_string << " VALUES ( ";
	query_string << glob_id << " , " ;
	query_string << scenario->get_sys_system_id().area_state << " , " ;
	query_string << scenario->get_sys_system_id().measure_nr << " , " ;
	query_string << scenario->get_hydraulic_boundary_scenario().get_id() << " , " ;
	query_string << "'"<< scenario->get_break_scenario_string()<< "' , " ;
	query_string << "'"<< Risk_Break_Scenario::convert_risk_type2txt(scenario->get_risk_type())<< "' , " ; 
	query_string << "'"<< Risk_Break_Scenario::convert_source2txt(scenario->get_source_scenario())<< "' , " ; 
	query_string << scenario->get_prob_scenario() << " ) " ;

	Data_Base::database_request(&model, query_string.str(), ptr_database);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                              : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info                        : " << model.lastError().text().toStdString() << endl;
		info << "Name of the hydraulic boundary scenario : " << scenario->get_hydraulic_boundary_scenario().get_name()<<endl;
		info << "Id of the hydraulic boundary scenario   : " << scenario->get_hydraulic_boundary_scenario().get_id()<<endl;
		info << "Id of the break scenario                : " << scenario->get_break_scenario_string()<<endl;
		
		msg.make_second_info(info.str());
		msg.output_msg(5);	
	}



}
//Set the database table for the probabilities of the fpl-sections: it sets the table name and the name of the columns and allocate them (static)
void Risk_Break_Scenario_Management::set_table_fpl_sec_prob(QSqlDatabase *ptr_database){
	if(Risk_Break_Scenario_Management::table_fpl_sec_prob==NULL){
		//make specific input for this class
		const string tab_id_name=risk_label::tab_risk_prob_sec;
		string tab_col[6];
		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=hyd_label::sz_bound_id;
		tab_col[4]=fpl_label::section_id;
		tab_col[5]=risk_label::sz_break_prob;


		
		//set the table
		try{
			Risk_Break_Scenario_Management::table_fpl_sec_prob= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Risk_Break_Scenario_Management::table_fpl_sec_prob->set_name(ptr_database, _sys_table_type::risk);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Risk_Break_Scenario_Management::set_table_fpl_sec_prob(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Risk_Break_Scenario_Management::close_table_fpl_sec_prob();
			throw msg;
		}
	}
}
//Create the database table for the probabilities of the fpl-sections (static)
void Risk_Break_Scenario_Management::create_table_fpl_sec_prob(QSqlDatabase *ptr_database){
		if(Risk_Break_Scenario_Management::table_fpl_sec_prob==NULL){
			ostringstream cout;
			cout << "Create database table of probabilities of the FPL-section(s)..." << endl ;
			Sys_Common_Output::output_risk->output_txt(&cout);
			//make specific input for this class
			const string tab_name=risk_label::tab_risk_prob_sec;
			const int num_col=6;
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

			tab_col[3].name=hyd_label::sz_bound_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].key_flag=true;

			tab_col[4].name=fpl_label::section_id;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].key_flag=true;

			tab_col[5].name=risk_label::sz_break_prob;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;	



			try{
				Risk_Break_Scenario_Management::table_fpl_sec_prob= new Tables();
				if(Risk_Break_Scenario_Management::table_fpl_sec_prob->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::risk)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_risk->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Risk_Break_Scenario_Management::create_table_fpl_sec_prob(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Risk_Break_Scenario_Management::close_table_fpl_sec_prob();
				throw msg;
			}
			Risk_Break_Scenario_Management::close_table_fpl_sec_prob();
		
	}
}
//Delete all data in the database table for the probabilities of the fpl-sections (static)
void Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table_fpl_sec_prob(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Risk_Break_Scenario_Management::table_fpl_sec_prob->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the probabilities of the fpl-sections for the specified parameters (static)
void Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table_fpl_sec_prob(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the probabilities of the fpl-sections (static)
void Risk_Break_Scenario_Management::close_table_fpl_sec_prob(void){
	if(Risk_Break_Scenario_Management::table_fpl_sec_prob!=NULL){
		delete Risk_Break_Scenario_Management::table_fpl_sec_prob;
		Risk_Break_Scenario_Management::table_fpl_sec_prob=NULL;
	}
}
//Select the data in the database table for the probabilities of the fpl-sections for the specified parameters (static)
int Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int boundary_id, const int section_id){
//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table_fpl_sec_prob(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::measure_id) << " = " << id.measure_nr;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(hyd_label::sz_bound_id) << " = " << boundary_id;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(fpl_label::section_id) << " = " << section_id;
	
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id, const int boundary_id, const int section_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return query->rowCount();

}
//Select the data in the database table for the probabilities of the fpl-sections (static) 
int Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id){
//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table_fpl_sec_prob(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name();
	query_string << " WHERE " ;
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::measure_id) << " = " << id.measure_nr;
	
	Data_Base::database_request(query, query_string.str(), ptr_database);
	if(query->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Risk_Break_Scenario_Management::select_data_in_table_fpl_sec_prob(QSqlQueryModel *query, QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name() << endl;
		info << "Table error info: " << query->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	return query->rowCount();

}
//Set the list of scenarios to calculate
void Risk_Break_Scenario_Management::set_list_sc2calc(const int number, Risk_Break_Scenario *sc2calc, QSqlDatabase *ptr_database){

	this->number_sc2calc=number;
	this->allocate_sc2calc();
	for(int i=0; i< this->number_sc2calc; i++){
		this->scenarios2calc[i]=sc2calc[i];
	}
	for(int i=0; i< this->number_sc2calc; i++){
		this->scenarios2calc[i].check_is_calculated(ptr_database);
	}
}
//Get the number of scenarios to calculate
int Risk_Break_Scenario_Management::get_number_sc2calc(void){
	return this->number_sc2calc;
}
//Get the number of hydraulic calculation, which are required for the current list of scenarios
int Risk_Break_Scenario_Management::get_number_hyd_calculation(QSqlDatabase *ptr_database){

	int counter=0;

	for(int i=0; i< this->number_sc2calc; i++){

		if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, this->system_id, this->scenarios2calc[i].get_hydraulic_boundary_scenario().get_id(), this->scenarios2calc[i].get_break_scenario_string())==false){
			counter++;
		}

	}

	return counter;
}
//Get a pointer to the scenarios to calculate
Risk_Break_Scenario *Risk_Break_Scenario_Management::get_ptr_sc2calc(void){
	return this->scenarios2calc;
}
//Get a pointer to one scenario to calculate by a given index
Risk_Break_Scenario *Risk_Break_Scenario_Management::get_ptr_sc2calc(const int index){
	if(index<0 || index>=this->number_sc2calc){
		return NULL;
	}
	else{

		return &this->scenarios2calc[index];
	}
}
//Ask one break scenario, which should be calculated per dialog
bool Risk_Break_Scenario_Management::ask_sc2calc_per_dialog(QWidget *parent, QSqlDatabase *ptr_database, const _risk_type type){
	this->risk_type=type;
	
	//scenario risk
	if(this->risk_type==_risk_type::scenario_risk){
		RiskGui_Risk_Scenario_Dia my_dia(parent);
		my_dia.set_dialog(ptr_database, this->system_id, type);
		

		if(my_dia.start_dialog()==true){
			RiskGui_Break_Height_Dia my_dia1(parent);
			my_dia1.set_section_id(ptr_database, my_dia.get_break_scenario_data().id_fpl_sec, this->system_id);
			if(my_dia1.start_dialog()==true){
				_fpl_break_info buffer;
				buffer=my_dia.get_break_scenario_data();
				buffer.break_height=my_dia1.get_break_height();

				Risk_Break_Scenario buff_sc;
				buff_sc.set_systemid(this->system_id);
				buff_sc.set_break_scenario(1, &buffer, my_dia.get_hyd_boundary_scenario_data());
				buff_sc.set_source_scenario(_risk_break_sc_source::sc_user_defined);

				this->set_list_sc2calc(1, &buff_sc, ptr_database);
				return true;
			}
			else{
				return false;
			}
		}
		else{
			return false;
		}
	}
	//catchment area risk
	else if(this->risk_type==_risk_type::catchment_risk){
		Hyd_Boundary_Scenario_List buffer;
		_risk_thread_type buff_thread;
		buff_thread=_risk_thread_type::risk_calc_catchment_hand;
		if(this->make_list_hyd_bound_sc_calculation(ptr_database, &buff_thread, &buffer)==false){
			return false;
		}

		RiskGui_Catchment_Scenario_Dia my_dia(parent);
		
		this->total_break_list.set_total_risk_break_list(ptr_database, this->system_id);
		this->total_break_list.set_predicted_risk_result2break_sc(ptr_database);
		my_dia.set_up_dialog(&this->total_break_list);

		if(my_dia.start_dialog()==false){
			return false;
		}
		else{

			//transfer from total list to list to calc
			this->number_sc2calc=this->total_break_list.count_number_selected();
			this->total_break_list.sort_list_after_number_involved_section(true);
			this->allocate_sc2calc();
			int counter=0;
			for(int i=0; i<this->total_break_list.get_number_in_list(); i++){
				if(this->total_break_list.get_ptr_scenario(i)->get_is_selected()==true){
					this->scenarios2calc[counter]=*(this->total_break_list.get_ptr_scenario(i));
					counter++;

				}

			}

			return true;
		}
	}

	return false;
}
//Set a list for the nobreak risk states for the catchment area risk approach
void Risk_Break_Scenario_Management::set_nobreak_risk_state_list(QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Load list of nobreak risk states for the calculation..."<< endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	
	this->number_sc2calc=this->hyd_bound_manager.counter_number_selected_scenarios();;
	this->allocate_sc2calc();

	int counter=0;
	for(int i=0; i<this->hyd_bound_manager.get_number_sz(); i++){
		if(this->hyd_bound_manager.get_ptr_sz(i)->get_is_selected()==true){
			this->scenarios2calc[counter].set_break_scenario(0, NULL, this->hyd_bound_manager.get_ptr_sz(i));
			this->scenarios2calc[counter].set_source_scenario(_risk_break_sc_source::sc_fixed);
			counter++;
		}
	}

	this->shorten_list2not_calculated(ptr_database);
	
}
//Set a list for the one-break risk states for the catchment area risk approach
void Risk_Break_Scenario_Management::set_one_break_risk_state_list(QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Load list of one-break risk states for the calculation of the catchment area risk approach..."<< endl ;
	Sys_Common_Output::output_risk->output_txt(&cout);

	Risk_Catchment_Break_Scenario_List one_break_list;
	this->make_list_one_break_sc(ptr_database, &one_break_list, &this->hyd_bound_manager);


	this->set_list_sc2calc(one_break_list.get_number_in_list(), one_break_list.get_total_list(), ptr_database);
	one_break_list.output_list();
	

}
///Set a list for the break risk states for the catchment area risk approach for a automatic calculation
void Risk_Break_Scenario_Management::set_auto_break_risk_state_list(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params){
	ostringstream cout;
	cout << "Load list of break risk states for the automatic optimised calculation of the catchment area risk approach..."<< endl ;
	Sys_Common_Output::output_risk->output_txt(&cout, true);
	this->total_break_list.prepare_list_auto_generation(ptr_database, this->system_id, params);
	//transfer from total list to list to calc
	this->number_sc2calc=this->total_break_list.get_number_in_list();
	this->allocate_sc2calc();

	this->total_break_list.sort_list_after_number_involved_section(true);

	for(int i=0; i<this->number_sc2calc; i++){
		this->scenarios2calc[i]=*(this->total_break_list.get_ptr_scenario(i));
	}
}
//Set total scenario list to calculate for the catchment risk calculation (all are hydraulically calculated)
void Risk_Break_Scenario_Management::set_total_sc2calc_catchment_risk(QSqlDatabase *ptr_database){
	ostringstream cout;
	cout << "Load total list of risk states for the calculation of the catchment area risk approach..."<< endl ;
	Sys_Common_Output::output_risk->output_txt(&cout, true);


	this->total_break_list.set_total_risk_break_list(ptr_database, this->system_id);
	//transfer from total list to list to calc
	this->number_sc2calc=this->total_break_list.count_number_hyd_calculated();
	this->allocate_sc2calc();
	int counter=0;
	for(int i=0; i<this->total_break_list.get_number_in_list(); i++){
		if(this->total_break_list.get_ptr_scenario(i)->get_is_hydraulic_calculated()==true){
			this->scenarios2calc[counter]=*(this->total_break_list.get_ptr_scenario(i));
			counter++;

		}

	}
}
//Get number of relevant fpl sections of the system
int Risk_Break_Scenario_Management::get_number_fpl_section(void){
	return this->number_fpl_section;
}
//Get the array of the relevant fpl-section of the system
Fpl_Section* Risk_Break_Scenario_Management::get_ptr_fpl_sections(void){
	return this->fpl_sections;
}
//Get one relevant fpl-section by a given section id
Fpl_Section* Risk_Break_Scenario_Management::get_ptr_fpl_section(const int section_id){
	
	for(int i=0; i< this->number_fpl_section; i++){
		if(section_id==this->fpl_sections[i].get_id_section()){
			return &this->fpl_sections[i];
		}
	}
	return NULL;
}
//Get one relevant fpl-section by a given index
Fpl_Section* Risk_Break_Scenario_Management::get_ptr_fpl_section_index(const int index){
	if(index<0 || index>=this->number_fpl_section){
		return NULL;
	}
	else{

		return &this->fpl_sections[index];
	}
}
//Load relevant fpl sections of the system from database
void Risk_Break_Scenario_Management::load_fpl_section(QSqlDatabase *ptr_database , const bool output){
	QSqlQueryModel result;
	this->number_fpl_section=Fpl_Section::select_relevant_section_database(&result, ptr_database, this->system_id, false);

	if(this->number_fpl_section>0){
		ostringstream cout;
		if(output==true){
			cout << "Load "<< this->number_fpl_section << " FPL-section(s) from database..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
		this->allocate_fpl_section();
		for(int i=0; i< this->number_fpl_section; i++){
			this->fpl_sections[i].input_section_perdatabase(&result, i, ptr_database);
		}
		if(output==true){
			cout << "Loading of "<< this->number_fpl_section << " FPL-section(s) from database is finished"<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);
		}
	}
}
//Calculate and set the probabilities of the scenarios, which wehre calculated, to the database table
void Risk_Break_Scenario_Management::set_scenario_prob2table(QSqlDatabase *ptr_database){
	//calculate the results
	for(int i=0; i< this->number_sc2calc; i++){
		this->scenarios2calc[i].calculate_probability(ptr_database);
	}
	//set the results to table
	int glob_id=0;

	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Risk_Break_Scenario_Management::table->maximum_int_of_column(Risk_Break_Scenario_Management::table->get_column_name(label::glob_id),ptr_database)+1;
	
	for(int i=0; i< this->number_sc2calc; i++){
		this->set_result2table(&this->scenarios2calc[i], ptr_database, glob_id);
		glob_id++;
	}
}
//Get a list of the break scenario to calculate compacted in _hyd_break2nobreak_sc structure (mainly for the damage calculation)
int Risk_Break_Scenario_Management::get_list_break_sc2calc(_dam_break2nobreak_sc **list){

	try{
		*list=new _dam_break2nobreak_sc[this->number_sc2calc];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	} 

	for(int i=0; i< this->number_sc2calc; i++){
		(*list)[i].break_sc=this->scenarios2calc[i].get_break_scenario_string();
		(*list)[i].combi_name=this->scenarios2calc[i].get_description_string();
		(*list)[i].hyd_sc_name=this->scenarios2calc[i].get_hydraulic_boundary_scenario().get_name();
		(*list)[i].id_hyd_sc=this->scenarios2calc[i].get_hydraulic_boundary_scenario().get_id();
		(*list)[i].must_calc=true;
		(*list)[i].dam_was_output=false;
	}

	return this->number_sc2calc;
}
//Make list of scenarios with break waterlevels via Monte Carlo analysis (test)
void Risk_Break_Scenario_Management::make_list_sc_via_monte_carlo(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params){


	
	try{
		//load the fpl section
		this->load_fpl_section(ptr_database);
		//delete the table
		Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(ptr_database,this->system_id);


	}
	catch(Error msg){
		throw msg;
	}
	this->catchment_sc_manager.set_break_scenario_generation_param(params);

	ostringstream prefix;
	for(int i=0; i<this->hyd_bound_manager.get_number_sz(); i++){
		try{
			if(this->hyd_bound_manager.get_ptr_sz(i)->get_is_selected()==false){
				continue;
			}
			prefix <<this->hyd_bound_manager.get_ptr_sz(i)->get_name()<<"> ";
			Sys_Common_Output::output_risk->set_userprefix(&prefix);

			//make mc-analysis for the probabilities of the break scenarios
			this->catchment_sc_manager.generate_break_scenario_list(ptr_database, this->number_fpl_section, this->fpl_sections, this->hyd_bound_manager.get_ptr_sz(i));
			
			this->catchment_sc_manager.output_scenario_list();
			this->transfer_list_break_scenario_tree2list();
			Risk_Break_Scenario_Management::delete_data_in_table(ptr_database, this->risk_type, this->hyd_bound_manager.get_ptr_sz(i)->get_id(), this->system_id);
			this->set_scenario_prob2table(ptr_database);
			
			for(int j=0; j<this->number_fpl_section; j++){
				this->fpl_sections[j].transfer_interval_data2database(ptr_database, this->hyd_bound_manager.get_ptr_sz(i)->get_id());
			}

			//calculate the probabilities of the fpl-section
			//transfer to a list
			Risk_Catchment_Break_Scenario_List buff;
			buff.merging_lists(this->catchment_sc_manager.get_ptr_break_sc_list(), this->catchment_sc_manager.get_number_break_sc());
			this->calc_transfer_prob_fpl_sec(&buff, ptr_database, this->hyd_bound_manager.get_ptr_sz(i));


			this->catchment_sc_manager.reset_scenario_tree();
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		Sys_Common_Output::output_risk->rewind_userprefix();
	}
	this->update_boundary_mid_probability_db_table(ptr_database, this->catchment_sc_manager.get_ptr_control_params()->number_mc);
}
//Update probabilities of the scenario list with new evaluated waterlevels
void Risk_Break_Scenario_Management::update_scenario_probability(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params){
	

	Risk_Catchment_Break_Scenario_List list;
	ostringstream cout;

	try{
		//set the list of hydraulic scenarios
		this->hyd_bound_manager.set_szenario_per_db(ptr_database);

		//output
		cout <<"Update the probabilities of the break scenario(s) with new hydraulic information..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		//set the existing fpl-section
		this->load_fpl_section(ptr_database);

		//set the list gerneration parameter
		this->catchment_sc_manager.set_break_scenario_generation_param(params);

		//delete the table
		Risk_Break_Scenario_Management::delete_data_in_table_fpl_sec_prob(ptr_database,this->system_id);

	}
	catch(Error msg){
		throw msg;
	}

	ostringstream prefix;
	//loop over all hydraulic boundary scenarios
	for(int i=0; i<this->hyd_bound_manager.get_number_sz(); i++){
		try{
			Risk_System::check_stop_thread_flag();
			prefix <<this->hyd_bound_manager.get_ptr_sz(i)->get_name()<<"> ";
			Sys_Common_Output::output_risk->set_userprefix(&prefix);

			list.set_risk_break_list_hyd_bound(ptr_database, this->system_id, this->hyd_bound_manager.get_ptr_sz(i));
			list.delete_not_calculated_in_list();
			list.sort_list_after_number_involved_section(true);
			//list.output_list();
			
			int counter=0;
			bool predict_flag=false;
			do{
				//make new list of break scenarios for updating
				if(predict_flag==true && this->catchment_sc_manager.get_ptr_control_params()->use_predict_values==true){
					this->catchment_sc_manager.generate_new_break_scenario_list_update(ptr_database,this->number_fpl_section, this->fpl_sections, this->hyd_bound_manager.get_ptr_sz(i), &list, predict_flag);
				}
				else if(predict_flag==false ){
					this->catchment_sc_manager.generate_new_break_scenario_list_update(ptr_database,this->number_fpl_section, this->fpl_sections, this->hyd_bound_manager.get_ptr_sz(i), &list, predict_flag);
				}
				//reset the table
				Risk_Break_Scenario_Management::reset_probabilities_in_table(ptr_database, this->hyd_bound_manager.get_ptr_sz(i)->get_id(),predict_flag); 
				//transfer the results to the database
				Risk_Catchment_Break_Scenario_List buff;
				buff.merging_lists(this->catchment_sc_manager.get_ptr_break_sc_list(), this->catchment_sc_manager.get_number_break_sc());
				for(int j=0; j< buff.get_number_in_list(); j++){
					this->update_probabilities_in_table(ptr_database, buff.get_ptr_scenario(j)->get_hydraulic_boundary_scenario().get_id(), buff.get_ptr_scenario(j)->get_break_scenario_string(), buff.get_ptr_scenario(j)->get_prob_scenario(), predict_flag); 
				}
				
				//output the list
				prefix <<"UPDATE> ";
				Sys_Common_Output::output_risk->set_userprefix(&prefix);
				if(predict_flag==true){
					prefix << "PRE> ";
					Sys_Common_Output::output_risk->set_userprefix(&prefix);
				}
				else{
					prefix << "NO_PRE> ";
					Sys_Common_Output::output_risk->set_userprefix(&prefix);
				}
				//buff.output_list();
				this->catchment_sc_manager.output_scenario_list();
				Sys_Common_Output::output_risk->rewind_userprefix();
				Sys_Common_Output::output_risk->rewind_userprefix();

				//count and switch
				counter++;
				predict_flag=true;
			}
			while(counter<2);


			//calculate the mid probability
			this->middle_probability_new_hyd_info_db_table(ptr_database);
			Risk_Catchment_Break_Scenario_List buff_mid;
			buff_mid.set_risk_break_list_hyd_bound(ptr_database, this->system_id, this->hyd_bound_manager.get_ptr_sz(i));
			this->calc_transfer_prob_fpl_sec(&buff_mid, ptr_database, this->hyd_bound_manager.get_ptr_sz(i));
			
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
		Sys_Common_Output::output_risk->rewind_userprefix();

	}

	this->catchment_sc_manager.reset_scenario_tree();
	this->update_boundary_mid_probability_db_table(ptr_database, this->catchment_sc_manager.get_ptr_control_params()->number_mc);
}
//Set the risk type of the manager
void Risk_Break_Scenario_Management::set_risk_type(const _risk_type type){
	this->risk_type=type;
	if(this->risk_type==_risk_type::nobreak_risk){
		Error msg=this->set_error(5);
		throw msg;
	}
}
//Set the system-id (_sys_system_id) of the object
void Risk_Break_Scenario_Management::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->hyd_bound_manager.set_systemid(actuel_id);
	this->catchment_sc_manager.set_systemid(actuel_id);
}
//Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
bool Risk_Break_Scenario_Management::ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, QWidget *parent, _risk_thread_type *type, const _risk_type risktype, int *number_selected){

	this->risk_type=risktype;

	Hyd_Boundary_Scenario_List my_list;


	if(this->make_list_hyd_bound_sc_calculation(ptr_database, type, &my_list)==false){
		*number_selected=0;
		return true;
	}

	ostringstream buff;
	if(*type==_risk_thread_type::risk_calc_catchment_nobreak){
		buff <<"Select hydraulic boundary scenario(s) for risk states calculation"<<endl;
		buff <<"of nobreak scenarios (CA-risk type <=> NOB-risk type)"<<endl;
		buff <<"(boundary scenarios, which are already calculated are not shown)" <<endl;
	}
	else if(*type==_risk_thread_type::risk_generate_catchment_state){
		buff <<"Select hydraulic boundary scenario(s) for the generation of"<<endl;
		buff <<"risk states list (CA-risk type)"<<endl;
		buff <<"(boundary scenarios, which are already generated are not shown)" <<endl;
	}
	else if(*type==_risk_thread_type::risk_calc_catchment_one_break){
		buff <<"Select hydraulic boundary scenario(s) for risk states calculation"<<endl; 
		buff <<"of one break scenarios (CA-risk type)"<<endl;
		buff <<"(boundary scenarios, which are already calculated are not shown)" <<endl;
	}	
	else if(*type==_risk_thread_type::risk_calc_nobreak){
		buff <<"Select hydraulic boundary scenario(s) for risk states calculation"<<endl;
		buff <<"of nobreak scenarios (NOB-risk type <=> CA-risk type)"<<endl;
		buff <<"(boundary scenarios, which are already calculated are not shown)" <<endl;
	}
	else{
		buff <<"Select scenario(s) for unknown handling from the available hydraulic boundary scenario(s)" <<endl;
	}

	return this->hyd_bound_manager.ask_boundary_scenarios_per_dialog(buff.str(), parent, &my_list, number_selected);


}
//Cut the sccenario list to calculate by a given index
void Risk_Break_Scenario_Management::cut_list_sc2calc(const int index){

	if(index >=this->number_sc2calc){
		//cut nothing
		return;
	}
	else if(index <=0){
		//cut all
		this->number_sc2calc=0;
		this->delete_sc2calc();
	}
	else{
		
		Risk_Catchment_Break_Scenario_List buffer;

		for(int i=0; i<index; i++){
			buffer.add_scenario2list(&this->scenarios2calc[i]);
		}
		
		this->number_sc2calc=buffer.get_number_in_list();
		try{
			this->allocate_sc2calc();
		}
		catch(Error msg){
			throw msg;
		}

		for(int i=0; i<buffer.get_number_in_list(); i++){
			this->scenarios2calc[i]=*buffer.get_ptr_scenario(i);
		}

		buffer.clear_list();
	}

}
//Make a list of hydraulic boundary scenarios, which are required for calculation/generation
bool Risk_Break_Scenario_Management::make_list_hyd_bound_sc_calculation(QSqlDatabase *ptr_database, _risk_thread_type *type,  Hyd_Boundary_Scenario_List *list){
/**
This method can be also used to check if all results are generated for a catchment risk approach. The results are sequenced 
checked depending from the given thread type. It should be only used for the catchment area thread types.

*/

	Hyd_Boundary_Szenario_Management buff_hyd;
	buff_hyd.set_systemid(this->system_id);
	buff_hyd.set_szenario_per_db(ptr_database);


	for(int i=0; i<buff_hyd.get_number_sz(); i++){
		if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, this->system_id, buff_hyd.get_ptr_sz(i)->get_id(), "CA")==false){
			list->add_scenario2list(buff_hyd.get_ptr_sz(i));
		}
	}

	if(*type==_risk_thread_type::risk_calc_catchment_nobreak || *type==_risk_thread_type::risk_calc_nobreak ){
		if(list->get_number_in_list()==0){
			
			ostringstream txt;
			if(*type==_risk_thread_type::risk_calc_catchment_nobreak){
				Sys_Diverse_Text_Dia dialog2;
				txt<<"All hydraulic boundary nobreak scenarios are already calculated."<< endl;
				txt<<"You can continue with the generation of the break scenarios."<< endl;
				txt<<"Do you want to continue with the next step?"<< endl;
				dialog2.set_dialog_question(txt.str());
				if(dialog2.start_dialog()==false){
					return false;
				}
				else{
					*type=_risk_thread_type::risk_generate_catchment_state;
				}
			}
			else if(*type==_risk_thread_type::risk_calc_nobreak){
				Sys_Diverse_Text_Dia dialog2(true);
				txt<<"All hydraulic boundary nobreak scenarios are already calculated."<< endl;
				dialog2.set_dialog_question(txt.str());
				dialog2.start_dialog();
				return false;
			}
		}
		else{
			return true;
		}
	}


	if(*type==_risk_thread_type::risk_generate_catchment_state || *type==_risk_thread_type::risk_calc_catchment_one_break
		|| *type== _risk_thread_type::risk_calc_catchment_hand || *type== _risk_thread_type::risk_calc_catchment_auto || *type== _risk_thread_type::risk_calc_catchment_cumulated){
		if(list->get_number_in_list()!=0){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"Not all hydraulic boundary nobreak scenarios are calculated (CA-risk type)."<< endl;
			txt<<"Calculate first all nobreak scenarios."<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		else{
			list->clear_list();
			QSqlQueryModel query;
			for(int i=0; i<buff_hyd.get_number_sz(); i++){ 
				if(Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database, this->risk_type, buff_hyd.get_ptr_sz(i)->get_id(), this->system_id)==0){
					list->add_scenario2list(buff_hyd.get_ptr_sz(i));
				}
			}
			if(list->get_number_in_list()==0 && *type==_risk_thread_type::risk_generate_catchment_state){
				Sys_Diverse_Text_Dia dialog3;
				ostringstream txt;
				txt<<"All break scenario list(s) are generated for the hydraulic boundary scenarios (CA-risk type)."<< endl;
				txt<<"Do you want to delete them and generate the list(s) again?"<< endl;
				dialog3.set_button_text("Yes", "No");
				dialog3.set_dialog_question(txt.str());
				if(dialog3.start_dialog()==false){
					Sys_Diverse_Text_Dia dialog2;
					ostringstream txt;
					txt<<"You can continue with the calculation of the one break scenarios."<< endl;
					txt<<"Do you want to continue with the next step?"<< endl;
					dialog2.set_dialog_question(txt.str());
					if(dialog2.start_dialog()==false){
						return false;
					}
					else{
						*type=_risk_thread_type::risk_calc_catchment_one_break;
					}
					
				}
				else{
					//delete thema
					Risk_Break_Scenario_Management::delete_data_in_table(ptr_database, this->risk_type, this->system_id);
					//make new list
					list->clear_list();
					for(int i=0; i<buff_hyd.get_number_sz(); i++){ 
						if(Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database, this->risk_type, buff_hyd.get_ptr_sz(i)->get_id(), this->system_id)==0){
							list->add_scenario2list(buff_hyd.get_ptr_sz(i));
						}
					}
					
					return true;
				}


			}
			else if(*type==_risk_thread_type::risk_generate_catchment_state){
				return true;
			}
		}
	}

	if(*type==_risk_thread_type::risk_calc_catchment_one_break
		|| *type== _risk_thread_type::risk_calc_catchment_hand || *type== _risk_thread_type::risk_calc_catchment_auto || *type== _risk_thread_type::risk_calc_catchment_cumulated){
		if(list->get_number_in_list()!=0){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"Not all break scenario list(s) are generated for the hydraulic boundary scenarios (CA-risk type)."<< endl;
			txt<<"Generate first all break scenario list."<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		else{
			list->clear_list();
			Risk_Catchment_Break_Scenario_List one_break_list;
			buff_hyd.set_all_sc_bound2selected();
			this->make_list_one_break_sc(ptr_database, &one_break_list, &buff_hyd);

			for(int i=0; i<one_break_list.get_number_in_list(); i++){

				if(Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, this->system_id, one_break_list.get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id(), one_break_list.get_ptr_scenario(i)->get_break_scenario_string(), false)
					==false){
					list->add_scenario2list(buff_hyd.get_ptr_sz_id_given(one_break_list.get_ptr_scenario(i)->get_hydraulic_boundary_scenario().get_id()));
				}
			}
			if(list->get_number_in_list()==0 && *type==_risk_thread_type::risk_calc_catchment_one_break) {
				Sys_Diverse_Text_Dia dialog2(true);
				ostringstream txt;
				txt<<"All hydraulic boundary one-break scenarios are already calculated (CA-risk type)."<< endl;
				txt<<"You can continue with the calculation of the break scenarios (automatic/user defined)."<< endl;
				dialog2.set_dialog_question(txt.str());
				dialog2.start_dialog();
				return false;
			}
			else if(*type==_risk_thread_type::risk_calc_catchment_one_break){
				return true;
			}
		}
	}

	if(*type== _risk_thread_type::risk_calc_catchment_hand || *type== _risk_thread_type::risk_calc_catchment_auto || *type== _risk_thread_type::risk_calc_catchment_cumulated){
		if(list->get_number_in_list()!=0){
			Sys_Diverse_Text_Dia dialog2(true);
			ostringstream txt;
			txt<<"Not all one-break scenario(s) are generated for the hydraulic boundary scenarios (CA-risk type)."<< endl;
			txt<<"Generate first all one-break scenario(s)."<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		else{
			return true;
		}

	}

	return false;
}
//Calculate and output the statistics of the catchment area risk approach
void Risk_Break_Scenario_Management::calc_output_catchment_area_risk_statistic(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params){
	
	Hyd_Boundary_Szenario_Management buff_hyd;
	buff_hyd.set_systemid(this->system_id);
	buff_hyd.set_szenario_per_db(ptr_database);
	buff_hyd.set_all_sc_bound2selected();
	Risk_Catchment_Break_Scenario_List one_break_list;
	this->make_list_one_break_sc(ptr_database, &one_break_list, &buff_hyd);

	this->total_break_list.calculate_output_statistic_total_break_list(ptr_database, this->system_id, params.prob_bound, &one_break_list, &buff_hyd);
}
//Predict detailed risk results for not calculated break scenario(s)
void Risk_Break_Scenario_Management::predict_detailed_risk_results(QSqlDatabase *ptr_database, const _risk_ca_scenario_control params){

	if(params.use_predict_values==false){
		return;
	}



	Risk_Catchment_Break_Scenario_List list;
	Risk_Catchment_Break_Scenario_List list2;
	ostringstream cout;

	try{
		//set the list of hydraulic scenarios
		this->hyd_bound_manager.set_szenario_per_db(ptr_database);

		//output
		cout <<"Predict detailed risk results for the not calculated break scenario(s)..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	
		//delete the table
		Risk_Damage_Predictor::delete_data_in_resulttable_predicted_detailed(ptr_database,this->system_id);

	}
	catch(Error msg){
		throw msg;
	}


	ostringstream prefix;
	//loop over all hydraulic boundary scenarios
	for(int i=0; i<this->hyd_bound_manager.get_number_sz(); i++){
		try{
			Risk_System::check_stop_thread_flag();
			prefix <<this->hyd_bound_manager.get_ptr_sz(i)->get_name()<<"> ";
			Sys_Common_Output::output_risk->set_userprefix(&prefix);


			list.set_risk_break_list_hyd_bound(ptr_database, this->system_id, this->hyd_bound_manager.get_ptr_sz(i));
			list2.set_risk_break_list_hyd_bound(ptr_database, this->system_id, this->hyd_bound_manager.get_ptr_sz(i));
			list.delete_calculated_in_list();
			list.sort_list_after_number_involved_section(true);
			list2.sort_list_after_number_involved_section(false);

			//output
			cout <<"Predict detailed risk results for "<< list.get_number_in_list()<<" break scenario(s)..."<<endl;
			Sys_Common_Output::output_risk->output_txt(&cout);

			//loop over the break scenarios
			for(int j=0; j< list.get_number_in_list(); j++){
				Risk_Damage_Predictor predictor;
				predictor.set_systemid(this->system_id);
				predictor.predict_damages(ptr_database, list.get_ptr_scenario(j), &list2);
			}
			Sys_Common_Output::output_risk->rewind_userprefix();
		}
		catch(Error msg){
			Sys_Common_Output::output_risk->rewind_userprefix();
			throw msg;
		}
	}
}
//___________
//private
//Allocate the scenario list, whcih should be calculated
void Risk_Break_Scenario_Management::allocate_sc2calc(void){
	this->delete_sc2calc();
	try{
		this->scenarios2calc=new Risk_Break_Scenario[this->number_sc2calc];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i<this->number_sc2calc; i++){
		if(this->risk_type==_risk_type::scenario_risk){
			this->scenarios2calc[i].set_risk_type(_risk_type::scenario_risk);
		}
		else if(this->risk_type==_risk_type::catchment_risk){
			this->scenarios2calc[i].set_risk_type(_risk_type::catchment_risk);
		}
		this->scenarios2calc[i].set_systemid(this->system_id);
	}
}
//Delete the scenario list, which should be calculated
void Risk_Break_Scenario_Management::delete_sc2calc(void){
	if(this->scenarios2calc!=NULL){
		delete []this->scenarios2calc;
		this->scenarios2calc=NULL;
	}
}
//Allocate the relevant fpl-sections of the system
void Risk_Break_Scenario_Management::allocate_fpl_section(void){
	this->delete_fpl_section();
	try{
		this->fpl_sections=new Fpl_Section[this->number_fpl_section];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i<this->number_fpl_section; i++){
		this->fpl_sections[i].set_systemid(this->system_id);
	}
}
//Delete the relevant fpl-sections of the system
void Risk_Break_Scenario_Management::delete_fpl_section(void){
	if(this->fpl_sections!=NULL){
		delete []this->fpl_sections;
		this->fpl_sections=NULL;
	}
}
//Shorten the list to scenarios, which are not yet calculated
void Risk_Break_Scenario_Management::shorten_list2not_calculated(QSqlDatabase *ptr_database){
	//check which are already calculated
	for(int i=0; i< this->number_sc2calc; i++){
		this->scenarios2calc[i].check_is_calculated(ptr_database);
	}
	//count the number
	int counter=0;
	for(int i=0; i< this->number_sc2calc; i++){
		if(this->scenarios2calc[i].get_is_calculated()==false){
			counter++;
		}
	}
	//create a buffer
	int number_buffer=this->number_sc2calc;
	Risk_Break_Scenario *buffer;
	buffer=NULL;
	try{

		buffer=new Risk_Break_Scenario[number_buffer];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//copy the list
	for(int i=0; i< this->number_sc2calc; i++){
        buffer[i]=(this->scenarios2calc[i]);
	}

	
	//set the new list
	this->number_sc2calc=counter;
	counter=0;
	try{
		this->allocate_sc2calc();
	}
	catch(Error msg){
		if(buffer!=NULL){
			delete []buffer;
			buffer=NULL;
		}
		throw msg;
	}
	for(int i=0; i< number_buffer; i++){
		if(buffer[i].get_is_calculated()==false){
			this->scenarios2calc[counter]=buffer[i];
			counter++;
		}
	}

	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}


}
//Transfer the list of the risk break scenario tree (\see Risk_Catchment_Scenario_Tree) to this list
void Risk_Break_Scenario_Management::transfer_list_break_scenario_tree2list(void){

	
	this->number_sc2calc=this->catchment_sc_manager.get_number_break_sc();
	this->allocate_sc2calc();
	Risk_Break_Scenario *buffer=NULL;
	buffer=this->catchment_sc_manager.get_ptr_break_sc_list();
	for(int i=0; i<this->number_sc2calc; i++){
		this->scenarios2calc[i]=buffer[i];
	}
}
//Load the list of involved river profiles to the fpl-section
void Risk_Break_Scenario_Management::load_involved_profile2fpl_section(Hyd_Hydraulic_System *my_hyd){

	if(this->number_fpl_section>0){
		
		ostringstream cout;
		cout << "Load the involved river profile(s)/coast dikeline point(s) to "<< this->number_fpl_section << " FPL-section(s) from database..."<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
		for(int i=0; i< this->number_fpl_section; i++){
			this->fpl_sections[i].load_list_involved_profiles_points(my_hyd);
		}
		cout << "Loading of the involved river profile(s)/coast diekline point(s) to the FPL-section(s) is finished"<<endl;
		Sys_Common_Output::output_risk->output_txt(&cout);
	}
}
//Make a list of one break scenarios for the catchment risk approach
void Risk_Break_Scenario_Management::make_list_one_break_sc(QSqlDatabase *ptr_database, Risk_Catchment_Break_Scenario_List *list, Hyd_Boundary_Szenario_Management *hyd_sc_manager){

	QSqlQueryModel query;
	int number=0;

	number=Risk_Break_Scenario_Management::select_data_in_table(&query, ptr_database, _risk_type::catchment_risk, this->system_id);
	string buffer;
	int hyd_bound_id=-1;
	double prob=0.0;
	_risk_break_sc_source src_buff;
	Risk_Break_Scenario sc_buff;
	sc_buff.set_risk_type(_risk_type::catchment_risk);
	sc_buff.set_systemid(this->system_id);


	for(int i=0; i<number; i++){
		buffer=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id)).c_str()).toString().toStdString();
		hyd_bound_id=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
		prob=query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob)).c_str()).toDouble();
		src_buff=Risk_Break_Scenario::convert_txt2scource(query.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src)).c_str()).toString().toStdString());

		if(src_buff!=_risk_break_sc_source::sc_fixed){
			continue;
		}

		if(hyd_sc_manager->get_ptr_sz_id_given(hyd_bound_id)==NULL || hyd_sc_manager->get_ptr_sz_id_given(hyd_bound_id)->get_is_selected()==false){
			continue;
		}

		sc_buff.set_break_scenario_str(*(hyd_sc_manager->get_ptr_sz_id_given(hyd_bound_id)),buffer);
		sc_buff.set_probability_sc(prob);
		sc_buff.set_source_scenario(src_buff);
		if(sc_buff.get_number_involved_sections()==1){
			sc_buff.set_breakheight_database(ptr_database);
			if(sc_buff.get_involved_section(0).break_height>0.00000001){
				if(Hyd_Hydraulic_System::check_hyd_results_calculated( ptr_database, this->system_id, hyd_bound_id, buffer)==false){
					sc_buff.set_is_hydraulic_calculated(false);
					list->add_scenario2list(&sc_buff);	
				}
			}
		}
	}

	list->add_break_height_by_database(ptr_database);
	list->sort_list_after_probability(true);
	
}
//Update the probability boundaries of the mid probability in database table
void Risk_Break_Scenario_Management::update_boundary_mid_probability_db_table(QSqlDatabase *ptr_database, const int number_mc_calc){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel buffer;
	QSqlQueryModel sender;
	ostringstream query_string;
	int number=0;
	number=Risk_Break_Scenario_Management::select_data_in_table(&buffer, ptr_database,_risk_type::catchment_risk, this->system_id);
	
	double delta_p=0.0;
	double prob=0.0;
	double prob_min=0.0;
	double prob_max=0.0;
	int glob_id=0;
	for(int i=0; i< number ; i++){
		query_string.str("");
		prob=buffer.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob)).c_str()).toDouble();
		glob_id=buffer.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(label::glob_id)).c_str()).toInt();
		//95% confidence interval => 2.0
		if(prob==0.0){
			delta_p=0.0;
		}
		else{
			delta_p=prob*2.0*pow((1.0-prob)/(prob*(double)number_mc_calc),0.5);
		}
		prob_max=prob+delta_p;
		if(prob_max>1.0){
			prob_max=1.0;
		}
		prob_min=prob-delta_p;
		if(prob_min<0.0){
			prob_min=0.0;
		}

		//send it to the database
		query_string <<"UPDATE ";
		query_string << Risk_Break_Scenario_Management::table->get_table_name();
		query_string << " SET " ;
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_min_bound_break_prob) << " = "<< prob_min <<" , " ;
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_max_bound_break_prob) << " = "<< prob_max <<"  " ;
		query_string  << " WHERE ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) << " = "  << glob_id;

		Data_Base::database_request(&sender, query_string.str(), ptr_database);
		if(sender.lastError().isValid()==true){
			Error msg=this->set_error(6);
			ostringstream info;
			info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
			info << "Table error info: " << sender.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

	}

}
//Calculate the mid probaility of the min/max probability as a results of the integration of new hydraulic results in database table
void Risk_Break_Scenario_Management::middle_probability_new_hyd_info_db_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel buffer;
	QSqlQueryModel sender;
	ostringstream query_string;
	int number=0;
	number=Risk_Break_Scenario_Management::select_data_in_table(&buffer, ptr_database,_risk_type::catchment_risk, this->system_id);

	double prob_mid=0.0;
	int glob_id=0;
	for(int i=0; i< number ; i++){
		query_string.str("");
		prob_mid=0.5*buffer.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_no_predict_break_prob)).c_str()).toDouble()+
			0.5*buffer.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_predict_break_prob)).c_str()).toDouble();
		glob_id=buffer.record(i).value((Risk_Break_Scenario_Management::table->get_column_name(label::glob_id)).c_str()).toInt();


		//send it to the database
		query_string <<"UPDATE ";
		query_string << Risk_Break_Scenario_Management::table->get_table_name();
		query_string << " SET " ;
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_prob) << " = "<< prob_mid <<"  " ;
		query_string  << " WHERE ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) << " = "  << glob_id;

		Data_Base::database_request(&sender, query_string.str(), ptr_database);
		if(sender.lastError().isValid()==true){
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
			info << "Table error info: " << sender.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Update the break scenarios with the new evaluated probabilities in database table after new hydraulic information are generated 
void Risk_Break_Scenario_Management::update_probabilities_in_table(QSqlDatabase *ptr_database, const int hyd_bound_id, const string break_sc_id, const double new_prob, const bool predict_flag){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	bool exists=Risk_Break_Scenario_Management::check_break_scenario_exists(ptr_database, this->system_id, hyd_bound_id, break_sc_id);

	if(exists==true){
		query_string <<"UPDATE ";
		query_string << Risk_Break_Scenario_Management::table->get_table_name();
		query_string << " SET " ;
		if(predict_flag==true){
			query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_predict_break_prob) << " = "<< new_prob <<"  " ;
		}
		else{
			query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_no_predict_break_prob) << " = "<< new_prob <<"  " ;
		}
		query_string  << " WHERE ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
		query_string  << " AND ";
		query_string  <<  Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr ;
		query_string  << " AND ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = "<< hyd_bound_id;
		query_string  << " AND ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id) << " = '"<< break_sc_id<<"'";
		query_string  << " AND ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " = '"<<Risk_Break_Scenario::convert_risk_type2txt(_risk_type::catchment_risk) << "'  ";

	}
	else{
		int glob_id=Risk_Break_Scenario_Management::table->maximum_int_of_column(Risk_Break_Scenario_Management::table->get_column_name(label::glob_id),ptr_database)+1;
		query_string <<"INSERT INTO ";
		query_string << Risk_Break_Scenario_Management::table->get_table_name();
		query_string << " ( " ;
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::glob_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " , ";

		query_string  << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::risk_type) << " , ";
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_break_src) << " , ";

		if(predict_flag==true){
			query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_predict_break_prob) << "  ";
		}
		else{
			query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_no_predict_break_prob) << "  ";

		}
		query_string <<" ) ";
		query_string << " VALUES ( ";
		query_string << glob_id << " , ";
		query_string << this->system_id.area_state<< " , ";
		query_string << this->system_id.measure_nr<< " , ";

		query_string << hyd_bound_id<< " , ";
		query_string << "'"<<break_sc_id << "' , ";

		query_string << "'"<<Risk_Break_Scenario::convert_risk_type2txt(_risk_type::catchment_risk) << "' , ";
		query_string << "'"<<Risk_Break_Scenario::convert_source2txt(_risk_break_sc_source::sc_generated)  << "' , ";
		query_string << new_prob<< "  ";

		query_string <<" ) ";

	}

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg=this->set_error(8);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Reset the probabilities of the break scenarios in table
void Risk_Break_Scenario_Management::reset_probabilities_in_table(QSqlDatabase *ptr_database, const int hyd_bound_id, const bool predict_flag){
//the table is set (the name and the column names) and allocated
	try{
		Risk_Break_Scenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;

	query_string <<"UPDATE ";
	query_string << Risk_Break_Scenario_Management::table->get_table_name();
	query_string << " SET " ;
	if(predict_flag==true){
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_predict_break_prob) << " = "<<0.0;
	}
	else{
		query_string  << Risk_Break_Scenario_Management::table->get_column_name(risk_label::sz_no_predict_break_prob) << " = "<<0.0;

	}	
	query_string  << " WHERE ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(label::areastate_id) << " = "  << this->system_id.area_state;
	query_string  << " AND ";
	query_string  <<  Risk_Break_Scenario_Management::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr ;
	query_string  << " AND ";
	query_string  << Risk_Break_Scenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = "<< hyd_bound_id;
	

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg=this->set_error(10);
		ostringstream info;
		info << "Table Name      : " << Risk_Break_Scenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}



}
//Calculate and transfer to database table the probabilities of each FPL-section for a given HYD-boundary scenario 
void Risk_Break_Scenario_Management::calc_transfer_prob_fpl_sec(Risk_Catchment_Break_Scenario_List *break_list, QSqlDatabase *ptr_database, Hyd_Boundary_Szenario *hyd_bound){


	double prob=0.0;
	ostringstream buff;
	Risk_Break_Scenario sc_buff;
		
	QSqlQueryModel query;
	ostringstream query_string;

	for(int i=0; i< this->number_fpl_section; i++){
		prob=0.0;
		query_string.str("");
		for(int j=0; j<this->catchment_sc_manager.get_ptr_control_params()->number_intervals-1; j++){
			buff.str("");
			buff <<"CA_"<<this->fpl_sections[i].get_id_section()<<"-"<<j;
			sc_buff.set_break_scenario_str(*hyd_bound, buff.str());
			prob=prob+break_list->sum_prob_included_scenario(&sc_buff);

		}
		//transfer it to database
		int glob_id=Risk_Break_Scenario_Management::table_fpl_sec_prob->maximum_int_of_column(Risk_Break_Scenario_Management::table->get_column_name(label::glob_id),ptr_database)+1;
		query_string <<"INSERT INTO ";
		query_string << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name();
		query_string << " ( " ;
		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::glob_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::areastate_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(label::measure_id) << " , ";

		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(hyd_label::sz_bound_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(fpl_label::section_id) << " , ";
		query_string  << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_column_name(risk_label::sz_break_prob) << "  ";

		query_string <<" ) ";
		query_string << " VALUES ( ";
		query_string << glob_id << " , ";
		query_string << this->system_id.area_state<< " , ";
		query_string << this->system_id.measure_nr<< " , ";

		query_string << hyd_bound->get_id()<< " , ";
		query_string << this->fpl_sections[i].get_id_section()<< " , ";
		query_string << prob << "  ";



		query_string <<" ) ";

		Data_Base::database_request(&query, query_string.str(), ptr_database);
		if(query.lastError().isValid()==true){
			Error msg=this->set_error(9);
			ostringstream info;
			info << "Table Name      : " << Risk_Break_Scenario_Management::table_fpl_sec_prob->get_table_name() << endl;
			info << "Table error info: " << query.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

	}


}
//Set error(s)
Error Risk_Break_Scenario_Management::set_error(const int err_type){
	string place="Risk_Break_Scenario_Management::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_sc2calc(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_fpl_section(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad alloc
			place.append("get_list_break_sc2calc(_hyd_break2nobreak_sc **list)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad alloc
			place.append("shorten_list2not_calculated(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://wrong risk type
			place.append("set_risk_type(void)");
			reason="Wrong risk type; risk type of the nobreak risk appraoch is not valid";
			help="Check the code";
			type=6;
			break;
		case 6://wrong database request
			place.append("update_boundary_mid_probability(QSqlDatabase *ptr_database, const int number_mc_calc)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 7://wrong database request
			place.append("middle_probability_new_hyd_info(QSqlDatabase *ptr_database)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 8://wrong database request
			place.append("update_probabilities_in_table(QSqlDatabase *ptr_database, const _sys_system_id id, const int hyd_bound_id, const string break_sc_id, const double new_prob)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 9://wrong database request
			place.append("calc_transfer_prob_fpl_sec(Risk_Catchment_Break_Scenario_List *break_list, QSqlDatabase *ptr_database, Hyd_Boundary_Szenario *hyd_bound, const int number_fpl_sec, Fpl_Section *section)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
			break;
		case 10://wrong database request
			place.append("reset_probabilities_in_table(QSqlDatabase *ptr_database, const int hyd_bound_id, const bool max_flag)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
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
//set warning
Warning Risk_Break_Scenario_Management::set_warning(const int warn_type){
		string place="Risk_Break_Scenario_Management::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("set_result2table(Risk_Break_Scenario *scenario, QSqlDatabase *ptr_database, const int glob_id)");
			reason="Can not submit the data of the river profile to the database";
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
