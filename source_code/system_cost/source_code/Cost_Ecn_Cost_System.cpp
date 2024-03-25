#include "Cost_Headers_Precompiled.h"
//#include "Cost_Ecn_Cost_System.h"

//init static members
Tables *Cost_Ecn_Cost_System::table_cost=NULL;


//Default constructor
Cost_Ecn_Cost_System::Cost_Ecn_Cost_System(void){

	Sys_Memory_Count::self()->add_mem(sizeof(Cost_Ecn_Cost_System),_sys_system_modules::COST_SYS);//count the memory
}
//Default destructor
Cost_Ecn_Cost_System::~Cost_Ecn_Cost_System(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Cost_Ecn_Cost_System),_sys_system_modules::COST_SYS);//count the memory
}
//_______
//public
//Set the database table for the cost: it sets the table name and the name of the columns and allocate them (static)
void Cost_Ecn_Cost_System::set_table_cost(QSqlDatabase *ptr_database){
	if(Cost_Ecn_Cost_System::table_cost==NULL){
		//make specific input for this class
		const string tab_id_name=cost_label::tab_cost_ecn;
		string tab_col[11];

		tab_col[0]=label::glob_id;
		tab_col[1]=label::areastate_id;
		tab_col[2]=label::measure_id;
		tab_col[3]=cost_label::area_cost;
		tab_col[4]=cost_label::build_cost;
		tab_col[5]=cost_label::eco_trade_off;
		tab_col[6]=cost_label::eco_trade_off_cost;
		tab_col[7]=cost_label::matching_coeff;
		tab_col[8]=cost_label::plan_cost_hoai;
		tab_col[9]=cost_label::plan_cost;
		tab_col[10]=cost_label::total_cost;



		//set the table
		try{
			Cost_Ecn_Cost_System::table_cost= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Cost_Ecn_Cost_System::table_cost->set_name(ptr_database, _sys_table_type::cost);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Cost_Ecn_Cost_System::set_table_cost(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Cost_Ecn_Cost_System::close_table_cost();
			throw msg;
		}
	}

}
//Create the database table for cost (static)
void Cost_Ecn_Cost_System::create_table_cost(QSqlDatabase *ptr_database){
	if(Cost_Ecn_Cost_System::table_cost==NULL){
		ostringstream cout;
		cout << "Create database table of economical cost..." << endl ;
		Sys_Common_Output::output_cost->output_txt(&cout);
		//make specific input for this class
		const string tab_name=cost_label::tab_cost_ecn;
		const int num_col=11;
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

		tab_col[3].name=cost_label::area_cost;
		tab_col[3].type=sys_label::tab_col_type_double;
		tab_col[3].default_value="0.0";
		tab_col[3].unsigned_flag=true;

		tab_col[4].name=cost_label::build_cost;
		tab_col[4].type=sys_label::tab_col_type_double;
		tab_col[4].default_value="0.0";
		tab_col[4].unsigned_flag=true;

		tab_col[5].name=cost_label::eco_trade_off;
		tab_col[5].type=sys_label::tab_col_type_double;
		tab_col[5].default_value="2.0";
		tab_col[5].unsigned_flag=true;

		tab_col[6].name=cost_label::eco_trade_off_cost;
		tab_col[6].type=sys_label::tab_col_type_double;
		tab_col[6].default_value="0.0";
		tab_col[6].unsigned_flag=true;

		tab_col[7].name=cost_label::matching_coeff;
		tab_col[7].type=sys_label::tab_col_type_double;
		tab_col[7].default_value="0.0";
		tab_col[7].unsigned_flag=true;

		tab_col[8].name=cost_label::plan_cost_hoai;
		tab_col[8].type=sys_label::tab_col_type_bool;
		tab_col[8].default_value="true";

		tab_col[9].name=cost_label::plan_cost;
		tab_col[9].type=sys_label::tab_col_type_double;
		tab_col[9].default_value="0.0";
		tab_col[9].unsigned_flag=true;

		tab_col[10].name=cost_label::total_cost;
		tab_col[10].type=sys_label::tab_col_type_double;
		tab_col[10].default_value="0.0";
		tab_col[10].unsigned_flag=true;


		try{
			Cost_Ecn_Cost_System::table_cost= new Tables();
			if(Cost_Ecn_Cost_System::table_cost->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::cost)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_cost->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Cost_Ecn_Cost_System::create_table_cost(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Cost_Ecn_Cost_System::close_table_cost();
			throw msg;
		}
		Cost_Ecn_Cost_System::close_table_cost();
	
	}
}
//Delete all data in the database table for the cost to the default values (static)
void Cost_Ecn_Cost_System::delete_data_in_table_cost(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Cost_Ecn_Cost_System::table_cost->delete_data_in_table(ptr_database);
}
//Delete all data in the database table of the cost for the specified values (static)
void Cost_Ecn_Cost_System::delete_data_in_table_cost(QSqlDatabase *ptr_database, const _sys_system_id id){
	//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string << " WHERE " ;
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Cost_Ecn_Cost_System::delete_data_in_table_cost(QSqlDatabase *ptr_database, const _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete data in the database table of the cost for the specified values (static)
void Cost_Ecn_Cost_System::delete_data_in_table_cost(QSqlDatabase *ptr_database, const double areastate){
	//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	query_string <<"DELETE FROM " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string << " WHERE " ;
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) << " = " << areastate;

		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Cost_Ecn_Cost_System::delete_data_in_table_cost(QSqlDatabase *ptr_database, const double areastate)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select data in the database table of the cost for the specified values (static)
bool Cost_Ecn_Cost_System::select_data_in_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id, _cost_ecn_cost_data *data){
	//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQueryModel query;
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string << " WHERE " ;
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Cost_Ecn_Cost_System::select_data_in_table_cost(QSqlDatabase *ptr_database, const _sys_system_id id, _cost_ecn_cost_data *init)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if(query.rowCount()==0){
		return false;
	}
	else{
		data->area_cost=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::area_cost)).c_str()).toDouble();
		data->building_cost=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::build_cost)).c_str()).toDouble();
		data->eco_trade_off=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::eco_trade_off)).c_str()).toDouble();
		data->eco_trade_off_cost=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::eco_trade_off_cost)).c_str()).toDouble();
		data->planning_cost_hoai=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::plan_cost_hoai)).c_str()).toBool();
		data->matching_coeff=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::matching_coeff)).c_str()).toDouble();
		data->planning_cost=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::plan_cost)).c_str()).toDouble();

		data->total_cost=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::total_cost)).c_str()).toDouble();
		return true;
	}

}
//Check if data in the database table of the cost for the specified values are set (static)
bool Cost_Ecn_Cost_System::check_data_in_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id){
		//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQueryModel query;
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string << " WHERE " ;
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Cost_Ecn_Cost_System::check_data_in_table_cost(QSqlDatabase *ptr_database, const _sys_system_id id, _cost_ecn_cost_data *init)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


	if(query.rowCount()==0){
		return false;
	}
	else{
		return true;
	}
}
//Get the total cost of the database table for the specified parameters (static)
bool Cost_Ecn_Cost_System::get_total_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id, double *total){
		//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	QSqlQueryModel query;
	ostringstream query_string;
	query_string <<"SELECT * FROM " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string << " WHERE " ;
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) << " = " << id.area_state;
	query_string << " AND ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::measure_id) << " = " << id.measure_nr;
		
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Cost_Ecn_Cost_System::get_total_cost_table(QSqlDatabase *ptr_database, const _sys_system_id id, double *total)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	
	if(query.rowCount()==0){
		return false;
	}
	else{
		*total=query.record(0).value((Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::total_cost)).c_str()).toDouble();
		return true;
	}
}
//Close and delete the database table for the cost (static)
void Cost_Ecn_Cost_System::close_table_cost(void){
	if(Cost_Ecn_Cost_System::table_cost!=NULL){
		delete Cost_Ecn_Cost_System::table_cost;
		Cost_Ecn_Cost_System::table_cost=NULL;
	}
}
//Set the cost per dialog
bool Cost_Ecn_Cost_System::set_cost_per_dialog(QWidget *parent, QSqlDatabase *ptr_database, const _sys_system_id id){
	CostGui_Alternative_Ecn_Cost_Dia my_dia(parent);


	//select alternative data
	string alt_name_buff;
	string alt_descript_buff;
	if(Alt_System::get_name_descript_measure_table(ptr_database, id, &alt_name_buff, &alt_descript_buff)==false){
		Sys_Diverse_Text_Dia dialog2(true);
		ostringstream txt;
		txt<<"No measure is found in database table!"<< endl;
		txt<<"Can not set the ecnomical cost"<< endl;
		dialog2.set_dialog_question(txt.str());
		dialog2.start_dialog();
		return false;	
	}
	
	_cost_ecn_cost_data cost_buff;
	this->init_cost_struct(&cost_buff);


	//select existing cost data
	Cost_Ecn_Cost_System::select_data_in_cost_table(ptr_database, id, &cost_buff);


	//set the dialog
	my_dia.set_dialog(alt_name_buff, alt_descript_buff, cost_buff); 
	if(my_dia.start_dialog()==true){
		cost_buff=my_dia.get_cost_data();
		this->calculate_cost_structure(&cost_buff);
		//set cost to cost database table
		this->set_cost_data2table(ptr_database, id, cost_buff);
		return true;
	}
	else{
		return false;
	}

}
//______
//private
//Calculate the cost-structure
void Cost_Ecn_Cost_System::calculate_cost_structure(_cost_ecn_cost_data *data){
	data->building_cost=data->building_cost*data->matching_coeff;
	data->eco_trade_off_cost=data->building_cost*data->eco_trade_off/100.0;
	if(data->planning_cost_hoai==true){
		data->planning_cost=this->calculate_planning_cost_hoai(data->building_cost);
	}

	data->total_cost=data->area_cost+data->building_cost+data->eco_trade_off_cost+
		data->planning_cost;

}
//Initialize the cost structure
void Cost_Ecn_Cost_System::init_cost_struct(_cost_ecn_cost_data *init){
	init->area_cost=0.0;
	init->building_cost=1.0;
	init->eco_trade_off=2.0;
	init->eco_trade_off_cost=0.0;
	init->matching_coeff=1.0;
	init->planning_cost=0.0;
	init->planning_cost_hoai=true;
	init->total_cost=0.0;
}
//Calculate planning cost after HOAI
double Cost_Ecn_Cost_System::calculate_planning_cost_hoai(const double build_cost){

	const double low_bound_hoai=25500.0;
	const double low_bound_percent=14.1;

	double buff_cost=0.0;
	double buff_percent=14.1;
	if(build_cost<low_bound_hoai){
		buff_cost=build_cost*low_bound_percent/100.0;
	}
	else{
		buff_percent=(135.54/(pow(build_cost,0.2236))+0.0659);
		buff_cost=build_cost*buff_percent/100.0;
	}

	return buff_cost;
}
//Set cost data to database table
void Cost_Ecn_Cost_System::set_cost_data2table(QSqlDatabase *ptr_database, _sys_system_id id, const _cost_ecn_cost_data data){
	
	Cost_Ecn_Cost_System::delete_data_in_table_cost(ptr_database, id);
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Cost_Ecn_Cost_System::set_table_cost(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int id_glob=Cost_Ecn_Cost_System::table_cost->maximum_int_of_column(Cost_Ecn_Cost_System::table_cost->get_column_name(label::glob_id),ptr_database)+1;
			
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Cost_Ecn_Cost_System::table_cost->get_table_name();
	query_string <<" ( ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(dam_label::glob_id) <<" , ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::areastate_id) <<" , ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(label::measure_id) <<" , ";

	//data
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::area_cost) <<" , ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::build_cost) <<" , ";

	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::eco_trade_off) <<" , ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::eco_trade_off_cost) <<" , ";

	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::matching_coeff) <<" , ";

	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::plan_cost_hoai) <<" , ";
	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::plan_cost) <<" , ";

	query_string << Cost_Ecn_Cost_System::table_cost->get_column_name(cost_label::total_cost) <<" ) ";

	query_string << " VALUES ( ";
	query_string << id_glob << " , " ;
	query_string << id.area_state << " , " ;
	query_string << id.measure_nr << " , " ;
	//data
	query_string << data.area_cost<< " , " ;
	query_string << data.building_cost<< " , " ;

	query_string << data.eco_trade_off<< " , " ;
	query_string << data.eco_trade_off_cost<< " , " ;

	query_string << data.matching_coeff<< " , " ;

	query_string << functions::convert_boolean2string(data.planning_cost_hoai)<< " , " ;
	query_string << data.planning_cost<< " , " ;


	query_string << data.total_cost<< " ) " ;

	
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Cost_Ecn_Cost_System::table_cost->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(7);	
	}
}
//Set warning(s)
Warning Cost_Ecn_Cost_System::set_warning(const int warn_type){
	string place="Cost_Ecn_Cost_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://results datas can not submitted
			place.append("set_cost_data2table(QSqlDatabase *ptr_database, _sys_system_id id, const _cost_ecn_cost_data data)") ;
			reason="Can not submit the economical cost of the alternative to database table";
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
}
//Set error(s)
Error Cost_Ecn_Cost_System::set_error(const int ){
	string place="Cost_Ecn_Cost_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	//switch (err_type){
	//	//case 0://bad alloc
	//	//	place.append("allocate_file_names(void)");
	//	//	reason="Can not allocate the memory";
	//	//	help="Check the memory";
	//	//	type=10;
	//	//	break;
	//	default:
	//		place.append("set_error(const int err_type)");
	//		reason ="Unknown flag!";
	//		help="Check the flags";
	//		type=6;
	//}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
