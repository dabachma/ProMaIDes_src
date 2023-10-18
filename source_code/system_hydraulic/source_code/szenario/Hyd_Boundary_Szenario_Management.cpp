#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Boundary_Szenario_Management.h"

//init static members
Tables *Hyd_Boundary_Szenario_Management::table=NULL;

//Default constructor
Hyd_Boundary_Szenario_Management::Hyd_Boundary_Szenario_Management(void){
	this->sz_bound=NULL;
	this->number_sz_bound=0;
	this->data_dia=NULL;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Boundary_Szenario_Management), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Boundary_Szenario_Management::~Hyd_Boundary_Szenario_Management(void){

	this->delete_szenarios();
	this->delete_scenario_dia();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Boundary_Szenario_Management), _sys_system_modules::HYD_SYS);
}
//______________
//public
//Get number of szenarios
int Hyd_Boundary_Szenario_Management::get_number_sz(void){
	return this->number_sz_bound;
}
//Get a pointer to the szenario with the given index 
Hyd_Boundary_Szenario* Hyd_Boundary_Szenario_Management::get_ptr_sz(const int number){
	if(number >= 0 && number <this->number_sz_bound){
		return &(this->sz_bound[number]);
	}
	else{
		return NULL;
	}
}
//Get apointer to the scenario with a given scenario index
Hyd_Boundary_Szenario* Hyd_Boundary_Szenario_Management::get_ptr_sz_id_given(const int id){

	for(int i=0; i< this->number_sz_bound; i++){
		if(this->sz_bound[i].get_id()==id){
			return &this->sz_bound[i];
		}
	}

	return NULL;
}
//Create the database table for the boundary szenarios of an hydraulic system (static)
void Hyd_Boundary_Szenario_Management::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Boundary_Szenario_Management::table==NULL){
			ostringstream cout;
			cout << "Create boundary szenario database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_sz_bound;
			const int num_col=9;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=hyd_label::sz_bound_id;
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

			tab_col[4].name=hyd_label::sz_bound_anu;
			tab_col[4].type=sys_label::tab_col_type_int;
			tab_col[4].unsigned_flag=true;
			tab_col[4].default_value="1";

			tab_col[5].name=hyd_label::sz_bound_prob_max;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;
			tab_col[5].default_value="1.0";

			tab_col[6].name=hyd_label::sz_bound_prob_form;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;
			tab_col[6].default_value="1.0";

			tab_col[7].name=hyd_label::sz_bound_name;
			tab_col[7].type=sys_label::tab_col_type_string;

			tab_col[8].name=label::description;
			tab_col[8].type=sys_label::tab_col_type_string;



			try{
				Hyd_Boundary_Szenario_Management::table= new Tables();
				if(Hyd_Boundary_Szenario_Management::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Boundary_Szenario_Management::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Boundary_Szenario_Management::close_table();
				throw msg;
			}

		Hyd_Boundary_Szenario_Management::close_table();
	}
}
//Set the database table for the boundary szenarios of an hydraulic system: it sets the table name and the name of the columns and allocates them (static)
void Hyd_Boundary_Szenario_Management::set_table(QSqlDatabase *ptr_database){

	if(Hyd_Boundary_Szenario_Management::table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_sz_bound;
		string tab_id_col[9];
		tab_id_col[0]=hyd_label::sz_bound_id;
		tab_id_col[1]=hyd_label::sz_bound_anu;
		tab_id_col[2]=hyd_label::sz_bound_prob_form;
		tab_id_col[3]=hyd_label::sz_bound_name;
		tab_id_col[4]=label::description;
		tab_id_col[5]=label::areastate_id;
		tab_id_col[6]=label::measure_id;
		tab_id_col[7]=label::applied_flag;
		tab_id_col[8]=hyd_label::sz_bound_prob_max;


		try{
			Hyd_Boundary_Szenario_Management::table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Boundary_Szenario_Management::table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Boundary_Szenario_Management::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Boundary_Szenario_Management::close_table();
			throw msg;
		}
	}

}
//Delete all data in the database table the boundary szenarios of an hydraulic system (static)
void Hyd_Boundary_Szenario_Management::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Boundary_Szenario_Management::table->delete_data_in_table(ptr_database);

}
//Close and delete the database table for the boundary szenarios of an hydraulic system (static)
void Hyd_Boundary_Szenario_Management::close_table(void){
	if(Hyd_Boundary_Szenario_Management::table!=NULL){
		delete Hyd_Boundary_Szenario_Management::table;
		Hyd_Boundary_Szenario_Management::table=NULL;
	}
}
//Check if a base boundary scenario is set in the database table (static)
bool Hyd_Boundary_Szenario_Management::check_base_scenario_is_set(QSqlDatabase *ptr_database){
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel query_result(0,*ptr_database);

	//give the table name to the query model
	query_result.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());
	query_result.setFilter("");
	//database request
	Data_Base::database_request(&query_result);

	if(query_result.rowCount()>0){
		return true;
	}
	else{
		return false;
	}

}
//Switch the applied-flag for the hydraulic boundary scenario in the database table for a defined system state (static)
void Hyd_Boundary_Szenario_Management::switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Boundary_Szenario_Management::table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Boundary_Szenario_Management::switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending tables
	_Hyd_River_Profile::switch_applied_flag_boundary_table(ptr_database, id,flag);
	Hyd_Element_Floodplain::switch_applied_flag_boundary_table(ptr_database, id,flag);


}
//Switch the applied-flag for the hydraulic boundary scenario in the database table for a defined system state and id (static)
void Hyd_Boundary_Szenario_Management::switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int hyd_sc){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Boundary_Szenario_Management::table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = " << hyd_sc ;



	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Boundary_Szenario_Management::switch_applied_flag_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int hyd_sc)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//appending tables
	_Hyd_River_Profile::switch_applied_flag_boundary_table(ptr_database, id,hyd_sc,flag);
	Hyd_Element_Floodplain::switch_applied_flag_boundary_table(ptr_database, id,hyd_sc,flag);


}
//Select and count the number of boundary scenarios for the given measure state in a database table (static)
int Hyd_Boundary_Szenario_Management::select_boundary_scenario_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id){
	int number=0;
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Hyd_Boundary_Szenario_Management::table->get_table_name();
	test_filter << " where " ;
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " and ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	test_filter << " order by " <<  Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Warning msg;
		msg.set_msg("Hyd_Boundary_Szenario_Management::select_boundary_scenario_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", "", 2);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	number= results->rowCount();

	return number;
}
//Set the szenarios per database table
void Hyd_Boundary_Szenario_Management::set_szenario_per_db(QSqlDatabase *ptr_database){
	this->delete_szenarios();
	this->number_sz_bound=0;

	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel query_result(0,*ptr_database);

	//give the table name to the query model
	query_result.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());
	ostringstream test_filter;
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;
	test_filter << " ) " ;
	query_result.setFilter(test_filter.str().c_str());
	//database request
	Data_Base::database_request(&query_result);

	
	//check the request
	if(query_result.lastError().isValid()){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query_result.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	query_result.sort(query_result.fieldIndex(Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id).c_str()) , Qt::AscendingOrder);
	
	this->number_sz_bound= query_result.rowCount();

	if(this->number_sz_bound>0){
		this->allocate_szenarios();
		//set the values
		for(int i=0; i< this->number_sz_bound; i++){		
			this->sz_bound[i].id_sz=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			this->sz_bound[i].annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_anu)).c_str()).toInt();
			this->sz_bound[i].prob_event=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_form)).c_str()).toDouble();
			this->sz_bound[i].name=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_name)).c_str()).toString().toStdString();
			this->sz_bound[i].prob_annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_max)).c_str()).toDouble();
			this->sz_bound[i].description=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(label::description)).c_str()).toString().toStdString();

		}
	}
}
//Set the existing hydraulic boundary scenarios per database table
void Hyd_Boundary_Szenario_Management::set_existing_szenario_per_db(QSqlDatabase *ptr_database){
	this->delete_szenarios();
	this->number_sz_bound=0;

	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel query_result(0,*ptr_database);

	//give the table name to the query model
	query_result.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());
	ostringstream test_filter;

	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	test_filter << " and ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << this->system_id.measure_nr;

	query_result.setFilter(test_filter.str().c_str());
	//database request
	Data_Base::database_request(&query_result);

	
	//check the request
	if(query_result.lastError().isValid()){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query_result.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	query_result.sort(query_result.fieldIndex(Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id).c_str()) , Qt::AscendingOrder);
	
	this->number_sz_bound= query_result.rowCount();

	if(this->number_sz_bound>0){
		this->allocate_szenarios();
		//set the values
		for(int i=0; i< this->number_sz_bound; i++){		
			this->sz_bound[i].id_sz=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			this->sz_bound[i].annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_anu)).c_str()).toInt();
			this->sz_bound[i].prob_event=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_form)).c_str()).toDouble();
			this->sz_bound[i].name=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_name)).c_str()).toString().toStdString();
			this->sz_bound[i].prob_annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_max)).c_str()).toDouble();
			this->sz_bound[i].description=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(label::description)).c_str()).toString().toStdString();

		}
	}
}
//Set the hydraulic boundary scenarios of the area state per database table
void Hyd_Boundary_Szenario_Management::set_area_state_szenario_per_db(QSqlDatabase *ptr_database){
	this->delete_szenarios();
	this->number_sz_bound=0;

	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel query_result(0,*ptr_database);

	//give the table name to the query model
	query_result.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());
	ostringstream test_filter;

	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = " << this->system_id.area_state;
	
	query_result.setFilter(test_filter.str().c_str());
	//database request
	Data_Base::database_request(&query_result);

	
	//check the request
	if(query_result.lastError().isValid()){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << query_result.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	query_result.sort(query_result.fieldIndex(Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id).c_str()) , Qt::AscendingOrder);
	
	this->number_sz_bound= query_result.rowCount();

	if(this->number_sz_bound>0){
		this->allocate_szenarios();
		//set the values
		for(int i=0; i< this->number_sz_bound; i++){		
			this->sz_bound[i].id_sz=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str()).toInt();
			this->sz_bound[i].annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_anu)).c_str()).toInt();
			this->sz_bound[i].prob_event=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_form)).c_str()).toDouble();
			this->sz_bound[i].name=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_name)).c_str()).toString().toStdString();
			this->sz_bound[i].prob_annuality=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_max)).c_str()).toDouble();
			this->sz_bound[i].description=query_result.record(i).value((Hyd_Boundary_Szenario_Management::table->get_column_name(label::description)).c_str()).toString().toStdString();

		}
	}
}
//Select hydraulic boundary scenarios, which hydraulic results have not yet calculated
int Hyd_Boundary_Szenario_Management::select_scenarios_without_hyd_result(QSqlDatabase *ptr_database){
	this->set_szenario_per_db(ptr_database);

	QSqlQueryModel model;
	
	for(int i=0; i< this->number_sz_bound; i++){
		if(Hyd_Hydraulic_System::check_hyd_results_calculated(ptr_database, this->system_id, this->sz_bound[i].get_id(), "" )==false){
			//calculation is required
			this->sz_bound[i].set_is_selected(true);
		}
	}
	return this->counter_number_selected_scenarios();
}
///Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the available hydraulic boundary scenarios should be handled and return the number
int Hyd_Boundary_Szenario_Management::ask_boundary_scenarios_per_dialog(QSqlDatabase *ptr_database, const string txt, QWidget *parent){
	this->set_szenario_per_db(ptr_database);

	HydGui_Boundary_Scenario_Dia my_dia(parent);
	my_dia.set_txt(txt);

	//check if a action is possible
	if(this->number_sz_bound==0){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"There are no HYD data available in database!"<< endl;
		txt<<"No HYD action can be performed."<< endl;
		txt<<"Input first the HYD base scenario to database."<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return 0;
	}

	if(my_dia.start_dialog(this->sz_bound, this->number_sz_bound)==true){

		return this->counter_number_selected_scenarios();	
	}
	else{
		return 0;
	}
}
//Ask per dialog (HydGui_Boundary_Scenario_Dia), which of the availabe hydraulic boundary scenarios should be handled and return the number
bool Hyd_Boundary_Szenario_Management::ask_boundary_scenarios_per_dialog(const string txt, QWidget *parent, Hyd_Boundary_Scenario_List *list, int *number_selected){

	this->number_sz_bound=list->get_number_in_list();
	this->allocate_szenarios();
	for(int i=0; i<this->number_sz_bound; i++){
		this->sz_bound[i]=*(list->get_ptr_scenario(i));

	}

	HydGui_Boundary_Scenario_Dia my_dia(parent);
	my_dia.set_txt(txt);
	if(my_dia.start_dialog(this->sz_bound, this->number_sz_bound)==true){
		*number_selected=this->counter_number_selected_scenarios();
		return true;
	}
	else{
		*number_selected=false;
		return false;
	}
}
//Set per list, which of the availabe hydraulic boundary scenarios should be handled and return the number
int Hyd_Boundary_Szenario_Management::set_boundary_scenarios_per_list(QSqlDatabase *ptr_database, QList<int> list_id) {
	this->set_szenario_per_db(ptr_database);

	for (int i = 0; i < list_id.count(); i++) {
		for (int j = 0; j < this->number_sz_bound; j++) {
			if (this->sz_bound[j].get_id() == list_id.at(i)) {
				this->sz_bound[j].set_is_selected(true);
				break;
			}

		}
	}

	return this->counter_number_selected_scenarios();
}
//Delete the data in table and create a new base szenario with id =0
void Hyd_Boundary_Szenario_Management::delete_table_create_base_sz(QSqlDatabase *ptr_database){

	Hyd_Boundary_Szenario_Management::delete_data_in_table(ptr_database);
	Hyd_Boundary_Szenario_Management::close_table();


	Hyd_Boundary_Szenario buff_sz;
	buff_sz.set_members(0,1.0,1.0,hyd_label::sz_hyd_bound_base);

	this->insert_new_hyd_sz(&buff_sz, ptr_database, 0);

}
//Delete the data of a hydraulic boundary scenario in the database table by a given id (staic)
void Hyd_Boundary_Szenario_Management::delete_scenario_by_id(QSqlDatabase *ptr_database, const int sc_id, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	//delete 
	test_filter<< "DELETE ";
	test_filter << " FROM " << Hyd_Boundary_Szenario_Management::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;
	test_filter << " AND ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter<< Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Boundary_Szenario_Management::delete_scenario_by_id(QSqlDatabase *ptr_database, const int sc_id, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the data of a hydraulic boundary scenario in the database table by a given system state (static)
void Hyd_Boundary_Szenario_Management::delete_scenario(QSqlDatabase *ptr_database, const _sys_system_id id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}
	//delete 
	test_filter<< "DELETE ";
	test_filter << " FROM " << Hyd_Boundary_Szenario_Management::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " AND ";
	test_filter<< Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id) << " = " << id.measure_nr;


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Boundary_Szenario_Management::delete_scenario_by_id(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Append a new szenario to the database table; the szenario id is chosen after the already existing szenarios
int Hyd_Boundary_Szenario_Management::insert_new_hyd_sz(Hyd_Boundary_Szenario *ptr_sz, QSqlDatabase *ptr_database, const int index){
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	Hyd_Boundary_Szenario *buff_sz=NULL;
	bool is_alloc_here=false;
	
	if(ptr_sz==NULL){
		buff_sz=new Hyd_Boundary_Szenario;
		is_alloc_here=true;

	}
	else{
		buff_sz=ptr_sz;
		if(this->data_dia!=NULL){
			this->data_dia->get_scenario_data(buff_sz, index);

		}
		is_alloc_here=false;
	}


	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//give the table name to the query model
	model.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_sz=Hyd_Boundary_Szenario_Management::table->maximum_int_of_column(Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id),ptr_database)+1;
		

	//insert row
	model.insertRows(0, 1);

	//insert data
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id)).c_str())),id_sz);
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_anu)).c_str())),buff_sz->annuality);
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_prob_form)).c_str())),buff_sz->get_prob_occurence());
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_name)).c_str())),buff_sz->name.c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(label::description)).c_str())),buff_sz->description.c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((Hyd_Boundary_Szenario_Management::table->get_column_name(label::applied_flag)).c_str())),true);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	if(is_alloc_here==true && buff_sz!=0){
		delete buff_sz;
	}
	else{
		ptr_sz->set_id(id_sz);
	}

	
	if(model.lastError().isValid()){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Table Name                : " << Hyd_Boundary_Szenario_Management::table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Boundary sz id            : " << id_sz << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}

	Hyd_Boundary_Szenario_Management::close_table();

	//recalc probabilies of the max-event
	this->recalc_prob_maxevent(ptr_database);


	return id_sz;
}
//Get the id of a next possible hydraulic boundary scenario
int Hyd_Boundary_Szenario_Management::get_new_hyd_sz_id(QSqlDatabase *ptr_database){
	try{
		Hyd_Boundary_Szenario_Management::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//give the table name to the query model
	model.setTable((Hyd_Boundary_Szenario_Management::table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_sz=Hyd_Boundary_Szenario_Management::table->maximum_int_of_column(Hyd_Boundary_Szenario_Management::table->get_column_name(hyd_label::sz_bound_id),ptr_database)+1;
	return id_sz;
}
//Get a pointer to all read in boundary scenarios
Hyd_Boundary_Szenario* Hyd_Boundary_Szenario_Management::get_ptr_boundary_scenario(void){
	return this->sz_bound;
}
//Count the number of selected scenarios and return it; the selected scenarios are handled in some way, e.g. calculation, checking etc.
int Hyd_Boundary_Szenario_Management::counter_number_selected_scenarios(void){

	if (this->number_sz_bound==0){
		return 0;
	}
	else{
		int counter=0;
		for(int i=0; i <this->number_sz_bound; i++){
			if(this->sz_bound[i].get_is_selected()==true){
				counter++;
			}
		}
		return counter;
	}
}
//Ask the new scenario data per dialog
bool Hyd_Boundary_Szenario_Management::ask_scenario_data_per_dialog(const int number, string *file_name, QSqlDatabase *ptr_database,const bool new_base_sc, QWidget *parent){
	bool flag=false;
	try{
		this->allocate_scenario_dia(parent);
		this->data_dia->set_dialog(number, file_name, ptr_database, new_base_sc);
	}
	catch(Error msg){
		throw msg;
	}

	flag=this->data_dia->start_dialog();

	return flag;
}
//Set all existing scenarios to selected status
void Hyd_Boundary_Szenario_Management::set_all_sc_bound2selected(void){
	for(int i=0; i< this->number_sz_bound; i++){
		this->sz_bound[i].set_is_selected(true);
	}
}
//Set new boundary scenario directly without a GUI dialog for an import via the hydraulic system
void Hyd_Boundary_Szenario_Management::set_new_boundary_scenario_directly(Hyd_Boundary_Scenario_List *list, QWidget *parent){
	
	
	try {
		this->allocate_scenario_dia(parent);
		this->data_dia->set_new_boundary_scenario_directly(list);
	}
	catch (Error msg) {
		throw msg;
	}

}

//Allocate the boundary scenario data dialog
void Hyd_Boundary_Szenario_Management::allocate_scenario_dia(QWidget *parent){
	try{
		this->data_dia=new HydGui_Bound_Scenario_Data_Dia(parent);
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//____________
//private
//Recalculate the probability of the Max-event
void Hyd_Boundary_Szenario_Management::recalc_prob_maxevent(QSqlDatabase *ptr_database){

	Hyd_Boundary_Szenario_Management list;
	list.set_systemid(this->system_id);
	Hyd_Boundary_Scenario_List list2;
	list.set_area_state_szenario_per_db(ptr_database);
	list.sort_bound_sz_annu();

	//select relevant
	for(int i=0; i<list.get_number_sz(); i++){
		if(i==0){
			list2.add_scenario2list(list.get_ptr_sz(i));
		}
		else{
			if(list.get_ptr_sz(i)->get_annuality()!=list.get_ptr_sz(i-1)->get_annuality()){
				list2.add_scenario2list(list.get_ptr_sz(i));
			}
		}
	}
	//set upper boundary
	for(int i=0; i<list2.get_number_in_list(); i++){
		if(i==0){
			if(list2.get_ptr_scenario(i)->get_annuality()>1){
				list2.get_ptr_scenario(i)->set_prob_year(2.0/(double)list2.get_ptr_scenario(i)->get_annuality());
			}
			else{
				list2.get_ptr_scenario(i)->set_prob_year(1.0/(double)list2.get_ptr_scenario(i)->get_annuality());
			}
		}
		else{
			list2.get_ptr_scenario(i)->set_prob_year(0.5*(1.0/(double)list2.get_ptr_scenario(i-1)->get_annuality()+1.0/(double)list2.get_ptr_scenario(i)->get_annuality()));
		}
	}

	//occurence prob per differenz
	for(int i=0; i<list2.get_number_in_list()-1; i++){
		list2.get_ptr_scenario(i)->set_prob_year(list2.get_ptr_scenario(i)->get_prob_year()-list2.get_ptr_scenario(i+1)->get_prob_year());
	}

	//transfer to the first list
	for(int i=0; i<list.get_number_sz(); i++){
		for(int j=0; j<list2.get_number_in_list(); j++){
			if(list.get_ptr_sz(i)->get_annuality()==list2.get_ptr_scenario(j)->get_annuality()){
				list.get_ptr_sz(i)->set_prob_year(list2.get_ptr_scenario(j)->get_prob_year());
			}
		}
	}

	//transfer to database
	for(int i=0; i<list.get_number_sz(); i++){
		list.get_ptr_sz(i)->update_max_event_prob_database(ptr_database);
	}
}
//Sort the boundary scenarios after ascending annualtiy
void Hyd_Boundary_Szenario_Management::sort_bound_sz_annu(void){
	//make the buffer for sorting
	Hyd_Boundary_Szenario swap;
	int j=0;

	for (int i = 1; i < this->number_sz_bound; i++) {
		swap=this->sz_bound[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		while ( j >= 0 && swap.get_annuality() < this->sz_bound[j].get_annuality()){
			this->sz_bound[j+1]= this->sz_bound[j];
			j--;
		}
		this->sz_bound[j+1] = swap;
	}
}
//Allocate the boundary szenarios
void Hyd_Boundary_Szenario_Management::allocate_szenarios(void){
	this->delete_szenarios();
	try{
		this->sz_bound=new Hyd_Boundary_Szenario[this->number_sz_bound];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_sz_bound; i++){
		this->sz_bound[i].set_systemid(this->system_id);

	}
}
//Delete the boundary szenarios
void Hyd_Boundary_Szenario_Management::delete_szenarios(void){
	if(this->sz_bound!=NULL){
		delete []this->sz_bound;
		this->sz_bound=NULL;
	}
}
//Delete the boundary scenario data dialog
void Hyd_Boundary_Szenario_Management::delete_scenario_dia(void){
	if(this->data_dia!=NULL){
		delete this->data_dia;
		this->data_dia=NULL;
	}
}
//set the error
Error Hyd_Boundary_Szenario_Management::set_error(const int err_type){
		string place="Hyd_Boundary_Szenario_Management::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){

		case 0://bad alloc
			place.append("allocate_szenarios(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://problems with database query
			place.append("set_szenario_per_db(QSqlDatabase *ptr_database)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;	
			break;
		case 2://input datas can not submitted
			place.append("insert_new_hyd_sz(Hyd_Boundary_Szenario *ptr_sz, QSqlDatabase *ptr_database)");
			reason="Can not submit the hydraulic boundary szenario data";
			help="Check the database";
			type=6;
			break;
		case 3://bad alloc
			place.append("allocate_scenario_dia(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://problems with database query
			place.append("set_existing_szenario_per_db(QSqlDatabase *ptr_database)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=6;	
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