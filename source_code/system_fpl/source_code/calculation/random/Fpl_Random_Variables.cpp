//#include "Fpl_Random_Variables.h"
#include "Fpl_Headers_Precompiled.h"

//init static variables
Tables *Fpl_Random_Variables::table=NULL;

//Default constructor
Fpl_Random_Variables::Fpl_Random_Variables(void){

	this->name=label::not_set;
	this->flag_calculated=false;
	this->flag_was_generated=false;
	this->boundaries.max=constant::max_boundary;
	this->boundaries.min=constant::min_boundary;
	this->boundary_counter=0;
	this->dist_typ_position=-1;
	this->default_name=fpl_label::given;
	this->unit=label::no_unit;
	this->distribution=NULL;
	this->distribution_type=_fpl_distribution_type::mean_type;
	this->zone_id=0;
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Random_Variables::~Fpl_Random_Variables(void){
	this->delete_dist_type();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Random_Variables), _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
//Set the database table for the parameters of the random variables: it sets the table name and the name of the columns and allocate them (static)
void Fpl_Random_Variables::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Random_Variables::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_random_variables;
		string tab_col[14];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::section_id;
		tab_col[2]=label::areastate_id;
		tab_col[3]=label::measure_id;
		tab_col[4]=label::applied_flag;
		tab_col[5]=fpl_label::variable_name;
		tab_col[6]=fpl_label::mecha_name;
		tab_col[7]=fpl_label::zone_id;
		tab_col[8]=fpl_label::default_name;
		tab_col[9]=fpl_label::distribution_type;
		tab_col[10]=fpl_label::min_boundary;
		tab_col[11]=fpl_label::max_boundary;
		tab_col[12]=fpl_label::section_type;
		tab_col[13]=fpl_label::distribution_unit;

		//set table
		try{
			Fpl_Random_Variables::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Random_Variables::table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("void Fpl_Random_Variables::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Random_Variables::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the random variables
void Fpl_Random_Variables::create_table(QSqlDatabase *ptr_database){
	if(Fpl_Random_Variables::table==NULL){
			ostringstream cout;
			cout << "Create random variables database table..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_random_variables;
			const int num_col=14;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=fpl_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=fpl_label::section_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::areastate_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].default_value="0";
			tab_col[2].key_flag=true;

			tab_col[3].name=label::measure_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;
			tab_col[3].default_value="0";
			tab_col[3].key_flag=true;

			tab_col[4].name=label::applied_flag;
			tab_col[4].type=sys_label::tab_col_type_bool;
			tab_col[4].default_value="true";
			tab_col[4].key_flag=true;
			
			tab_col[5].name=fpl_label::variable_name;
			tab_col[5].type=sys_label::tab_col_type_string;
			tab_col[5].key_flag=true;

			tab_col[6].name=fpl_label::mecha_name;
			tab_col[6].type=sys_label::tab_col_type_string;
			tab_col[6].key_flag=true;

			tab_col[7].name=fpl_label::section_type;
			tab_col[7].type=sys_label::tab_col_type_string;
			tab_col[7].key_flag=true;

			tab_col[8].name=fpl_label::zone_id;
			tab_col[8].type=sys_label::tab_col_type_int;
			tab_col[8].unsigned_flag=true;
			tab_col[8].default_value="0";
			tab_col[8].key_flag=true;

			tab_col[9].name=fpl_label::default_name;
			tab_col[9].type=sys_label::tab_col_type_string;

			tab_col[10].name=fpl_label::distribution_type;
			tab_col[10].type=sys_label::tab_col_type_string;

			tab_col[11].name=fpl_label::min_boundary;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="-99999.0";

			tab_col[12].name=fpl_label::max_boundary;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="99999.0";

			tab_col[13].name=fpl_label::distribution_unit;
			tab_col[13].type=sys_label::tab_col_type_string;

			try{
				Fpl_Random_Variables::table= new Tables();
				if(Fpl_Random_Variables::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Random_Variables::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Random_Variables::close_table();
				throw msg;
			}

		Fpl_Random_Variables::close_table();
	}
}
//Delete all data in the database table for the parameters of the random variables
void Fpl_Random_Variables::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Random_Variables::table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the parameters of the random variables
void Fpl_Random_Variables::delete_data_in_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//select 
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id.area_state;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Random_Variables::delete_data_in_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//delete the distribution types
		Fpl_Random_Variables::delete_distribution_type_database(ptr_database, &results);
	}
	catch(Error msg){
		throw msg;
	}

	//delete the variables
	//delete 
	test_filter.str("");
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << sec_id << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " = " << id.measure_nr << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id.area_state;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Random_Variables::delete_data_in_table(QSqlDatabase *ptr_database,  const _sys_system_id id, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the parameters of the random variables (static)
void Fpl_Random_Variables::close_table(void){
	if(Fpl_Random_Variables::table!=NULL){
		delete Fpl_Random_Variables::table;
		Fpl_Random_Variables::table=NULL;
	}
	//close the tables of the the dist_types
	Fpl_Distribution_Class_Mean::close_table();
	Fpl_Distribution_Class_Triangle::close_table();
	Fpl_Distribution_Class_Discret::close_table();
	Fpl_Distribution_Class_Discret_Mean::close_table();
	Fpl_Distribution_Class_Mean_Mean::close_table();
}
//Select and count the number of existing random variables in database table (static)
int Fpl_Random_Variables::select_variables_database(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type){
	int number=0;
	try{
		Fpl_Random_Variables::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
	test_filter << " and (";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name) << " = '" << name_var << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name) << " = '" << mechanism << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	test_filter << " ORDER BY ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);


	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("int Fpl_Random_Variables::select_variables_database(const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}
	
	number=results->rowCount();

	return number;
}
//Select and count the number of existing random variables in database table (static)
int Fpl_Random_Variables::select_variables_database(QSqlQueryModel *results, const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_id){
	int number=0;
	try{
		Fpl_Random_Variables::set_table(ptr_database);	
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
	test_filter << " and (";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name) << " = '" << name_var << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name) << " = '" << mechanism << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id)<< " = "<< zone_id;

	Data_Base::database_request(results, test_filter.str(), ptr_database);


	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("int Fpl_Random_Variables::select_variables_database(const _sys_system_id id_sys, const int section_id, const string name_var, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;	
	}
	
	number=results->rowCount();

	return number;

}
//Switch the applied-flag for the results of the random variables in the database table for a defined system state
void Fpl_Random_Variables::switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Random_Variables::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;

	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Random_Variables::switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the results of the random variables in the database table for a defined system state and id
void Fpl_Random_Variables::switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Fpl_Random_Variables::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << sec_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Random_Variables::switch_applied_flag_variable_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Copy the random-variable of a fpl-section of a given system id to another one in database tables (static)
void Fpl_Random_Variables::copy_fpl_random_variables(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id){
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Random_Variables::table->maximum_int_of_column(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id) <<" , ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type) <<" , ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::default_name) <<"  ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " = " << src.area_state;
	test_filter << " AND ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << src.measure_nr ;
	test_filter << " AND ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << sec_id ;

	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Random_Variables::copy_fpl_random_variables(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
	
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Random_Variables::table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::section_id)  <<" , ";
		test_filter <<  dest.area_state  <<" , ";
		test_filter <<  dest.measure_nr  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(label::applied_flag)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::section_type)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::default_name)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::min_boundary)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::max_boundary)  <<" , ";
		test_filter <<  Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_unit)  <<"  ";

		test_filter << " FROM " << Fpl_Random_Variables::table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Random_Variables::copy_results(QSqlDatabase *ptr_database, const _sys_system_id src, const _sys_system_id dest, const int sec_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//copy distribution type
		string default_name;
		default_name=model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::default_name).c_str()).toString().toStdString();

		if(default_name==fpl_label::given){
			_fpl_distribution_type dist_buff;
			dist_buff=_Fpl_Distribution_Class::convert_txt2dist_type(model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type).c_str()).toString().toStdString());

			if(dist_buff==_fpl_distribution_type::mean_type){
				Fpl_Distribution_Class_Mean::copy_parameters(ptr_database, model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt(),glob_id);
			}
			else if(dist_buff==_fpl_distribution_type::triangle_type){
				Fpl_Distribution_Class_Triangle::copy_parameters(ptr_database, model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt(),glob_id);
			}
			else if(dist_buff==_fpl_distribution_type::discret_type){
				Fpl_Distribution_Class_Discret::copy_parameters(ptr_database, model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt(),glob_id);
			}
			else if(dist_buff==_fpl_distribution_type::mean2mean_type){
				Fpl_Distribution_Class_Mean_Mean::copy_parameters(ptr_database, model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt(),glob_id);
			}
			else if(dist_buff==_fpl_distribution_type::discrete2mean_type){
				Fpl_Distribution_Class_Discret_Mean::copy_parameters(ptr_database, model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt(),glob_id);
			}

		}
		glob_id++;
	}
	try{

		Fpl_Random_Variables::switch_applied_flag_variable_table(ptr_database, src, false,sec_id);
	}
	catch(Error msg){
		throw msg;

	}
}
//Set the input for the random variable and the connected distribution type per console
void Fpl_Random_Variables::set_input(void){
	bool flag=false;
	this->zone_id=0;
	//read in the name
	if(this->name==label::not_set){
		cout << "Give the name of the Random variables !" << endl;
		cin >> this->name;
	}
	_fpl_distribution_type buffer_type=_fpl_distribution_type::unknown_dist_type;
	//distribution type
	do{
		//read in the inputdata for the random variablesdistribution type
		cout << "Give the distribution class please of variable" << this->name << " !" << endl;
		cout << "(0) mean, (1) triangle, (2) discrete, (3) mean2mean, (4) discret2mean " << endl;
		int buffer=0;

		//if((flag=(cin  >> buffer))==false){//check if the input is valif
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
		//else if((buffer_type=_Fpl_Distribution_Class::convert_int2dist_type(buffer))==_fpl_distribution_type::unknown_dist_type){
		//	cout << "Unknwon distribution type! Do it again! " << endl;
		//}
		//else{
		//	this->distribution_type=buffer_type;
		//}
	}
	while(flag==false || buffer_type==_fpl_distribution_type::unknown_dist_type);

	//alloc the distribution class
	try{
		this->allocate_dist_type();
	}
	catch(Error msg){
		stringstream info;
		info << "Variable_Name: " << name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//read in the distribution type
	this->distribution->set_input();
}
//Set the input for the random variable and the appending distribution type per file
void Fpl_Random_Variables::set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str, const bool must_zone_found){
	ostringstream cout;
	cout << "Search for random variable (calculated): " << name<< " ("<< functions::convert_boolean2string(calculated)<<")"<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	//dynamic mid/mean calculation flag 
	this->flag_calculated=calculated;
	//name of the variable
	this->name=name;
	//zone id
	this->zone_id=0;

	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;
	bool variable_found_flag=false;
	qint64 pos_file=0;
	int store_line=0;

	try{
		while(variable_found_flag==false){
			//search for the begin
			do{
				qmyline=ifile->readLine();
				myline=qmyline.toStdString();
				(*line_counter)++;
				functions::clean_string(&myline);
				if(myline==end_block_str){
					break;
				}

				if(myline==fpl_label::begin_random){
					found_flag=true;
					pos_file=ifile->pos();
					store_line=*line_counter;
					break;
				}
			}
			while(ifile->atEnd()==false);

			if(myline!=end_block_str){
				if(found_flag==false && this->flag_calculated==false ){
					Error msg=this->set_error(5);
					throw msg;
				}
				else{
					found_flag=false;
				}

			
				//search for the keywords
				do{
					qmyline=ifile->readLine();
					myline=qmyline.toStdString();
					(*line_counter)++;
					functions::clean_string(&myline);
					if(myline==fpl_label::end_random){
						found_flag=true;
						break;
					}
					else if(myline==end_block_str){
						break;
					}
					else if(myline.empty()==false){
						if(this->find_key_values_file(myline, &must_found_counter, &variable_found_flag, must_zone_found )==false){
							must_found_counter=0;
							break;	
						}
					}

				}
				while(ifile->atEnd()==false);

				if(ifile->atEnd()==true ||  variable_found_flag==true){
					break;
				}
			}
			else{
				break;
			}
		}

		if(variable_found_flag==false && this->flag_calculated==false){
			Error msg=this->set_error(9);
			ostringstream info;
			info << "This variable is not dynamically calculated, thus, it is required to set the data!"<<endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		else if(variable_found_flag==false && this->flag_calculated==true){
			this->set_distribution_types(ifile, line_counter);
			return;

		}
		if(found_flag==false){
			Error msg=this->set_error(6);
			throw msg;
		}

		int must_found=2;
		if(must_zone_found==true){
			must_found=3;
		}
		if(must_found_counter!=must_found){
			Error msg=this->set_error(7);
			throw msg;
		}

		if(Sys_Project::get_project_type()==_sys_project_type::proj_fpl_file && this->default_name!=fpl_label::given){
			Error msg=this->set_error(10);
			throw msg;

		}		

		ifile->seek(pos_file);
		*line_counter=store_line;
		this->set_distribution_types(ifile, line_counter);
	}
	catch(Error msg){
		ostringstream info;
		info<<"Variable name   : " <<this->name << endl;
		info<<"Zone id         : " << this->zone_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if a random variable is gievn in a file file
bool Fpl_Random_Variables::check_variable_in_file(QFile *ifile, int *line_counter, const string name, const string end_block_str){
	//dynamic mid/mean calculation flag 
	this->flag_calculated=false;
	//name of the variable
	this->name=name;


	//read in
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;
	int must_found_counter=0;
	bool variable_found_flag=false;
	qint64 pos_file=0;
	int store_line=0;

	try{
		while(variable_found_flag==false){
			//search for the begin
			do{
				qmyline=ifile->readLine();
				myline=qmyline.toStdString();
				(*line_counter)++;
				functions::clean_string(&myline);
				if(myline==end_block_str){
					break;
				}

				if(myline==fpl_label::begin_random){
					found_flag=true;
					pos_file=ifile->pos();
					store_line=*line_counter;
					break;
				}
			}
			while(ifile->atEnd()==false);

			if(myline!=end_block_str){
				if(found_flag==false && this->flag_calculated==false ){
					Error msg=this->set_error(5);
					throw msg;
				}
				else{
					found_flag=false;
				}

			
				//search for the keywords
				do{
					qmyline=ifile->readLine();
					myline=qmyline.toStdString();
					(*line_counter)++;
					functions::clean_string(&myline);
					if(myline==fpl_label::end_random){
						found_flag=true;
						break;
					}
					else if(myline==end_block_str){
						break;
					}
					else if(myline.empty()==false){
						if(this->find_key_values_file(myline, &must_found_counter, &variable_found_flag, false )==false){
							must_found_counter=0;
							break;	
						}
					}

				}
				while(ifile->atEnd()==false);

				if(ifile->atEnd()==true ||  variable_found_flag==true){
					break;
				}
			}
			else{
				break;
			}
		}
		return variable_found_flag;

	}
	catch(Error msg){
		ostringstream info;
		info<<"Variable name   : " <<this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Set the input via a dialog for random generator and transformation checking
bool Fpl_Random_Variables::set_input_check(_fpl_distribution_type dist_type){
	this->distribution_type=dist_type;
	this->zone_id=0;
	//alloc the distribution class
	try{
		this->allocate_dist_type();
	}
	catch(Error msg){
		stringstream info;
		info << "Variable name   : " << name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->distribution->set_input_check()==false){
		return false;
	}
	else{
		return true;
	}

}
//Check if there is a random variable in database
bool Fpl_Random_Variables::check_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_number){
	//dynamic mid/mean calculation flag 
	this->flag_calculated=calculated;
	//name of the variable
	this->name=name_var;
		//zone id
	this->zone_id=zone_number;
	//the table is set the name and the column names
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
	test_filter << " and (";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name) << " = '" << this->name << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name) << " = '" << mechanism << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id) << " = " << zone_number ;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);


	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(12);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	int number=results.rowCount();
	if(number>0){
		return true;
	}
	else{
		return false;
	}
}
//Set the input for the random variable per database table
void Fpl_Random_Variables::set_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_number, const bool with_output){
	//dynamic mid/mean calculation flag 
	this->flag_calculated=calculated;
	//name of the variable
	this->name=name_var;
	//zone id
	if(zone_number>0){
		this->zone_id=zone_number;
	}

	//output
	ostringstream cout;
	if(with_output==true){
		if(zone_number>0){
			cout << "Search for variable " << this->name << " in zone " << zone_number << "..."<< endl;
		}
		else{
			cout << "Search for variable " << this->name << "..."<< endl;
		}
		Sys_Common_Output::output_fpl->output_txt(&cout,false);
	}
	//the table is set the name and the column names
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}catch(Error msg){
		throw msg;
	}

	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Random_Variables::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::applied_flag) << "= true";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::areastate_id) << " ="  << id_sys.area_state;
	test_filter << " and (";
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Fpl_Random_Variables::table->get_column_name(label::measure_id) << " = " << id_sys.measure_nr;
	test_filter << " ) " ;
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) << " = " << section_id;	
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name) << " = '" << this->name << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name) << " = '" << mechanism << "'";
	test_filter << " and ";
	test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::section_type) << " = '" << section_type << "'";	
	if(zone_number>0){
		test_filter << " and ";
		test_filter << Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id) << " = " << zone_number ;
	}

	Data_Base::database_request(&results, test_filter.str(), ptr_database);


	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	int number=results.rowCount();
	//no results found
	if(number==0 && this->flag_calculated==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Variable name   : " << this->name << endl;
		info << "Section id      : " << section_id << endl;
		info << "Mechanism       : " << mechanism << endl;
		if(zone_number>0){
			info << "Zone id         : " << this->zone_id << endl;
		}
		msg.make_second_info(info.str());
		throw msg;
	}
	else if(number==0 && this->flag_calculated==true){
		return;
	}
	//multiple results found
	if(number>1){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Variable name   : " << this->name << endl;
		info << "Section id      : " << section_id << endl;
		info << "Mechanism       : " << mechanism << endl;
		if(zone_number>0){
			info << "Zone id         : " << this->zone_id << endl;
		}
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}

	//input values
	this->dist_typ_position=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id)).c_str()).toInt();
	this->default_name=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::default_name)).c_str()).toString().toStdString();
	this->unit=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_unit)).c_str()).toString().toStdString();
	this->boundaries.min=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::min_boundary)).c_str()).toDouble();
	this->boundaries.max=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::max_boundary)).c_str()).toDouble();
	this->distribution_type=Fpl_Distribution_Class_Mean::convert_txt2dist_type(results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type)).c_str()).toString().toStdString());
	this->zone_id=results.record(0).value((Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id)).c_str()).toInt();
}
//Set the input for the distribution type per database table; the parameter of the random variable has to be set before 
void Fpl_Random_Variables::set_distribution_types(QSqlDatabase *ptr_database){

	if(this->dist_typ_position<0 && this->default_name==fpl_label::given && this->flag_calculated==true){
		this->set_determinstic_calculated_type();
		return;
	}
	//set the distribution class per table
	try{	
		this->allocate_dist_type();
		if(this->default_name==fpl_label::given){
			//input per database user defined
			this->distribution->set_input(this->dist_typ_position, ptr_database, this->flag_calculated, this->boundaries);
		}
		else{
			//input per database default values
			this->distribution->set_input(this->name,this->default_name, ptr_database, this->flag_calculated, this->boundaries);
		}
	}
	catch(Error msg){
		stringstream info;
		info << "Variable name                           : " << this->name << endl;
		info << "Distribution position in database table : " << this->dist_typ_position << endl;
		info << "Distribution default name               : " << this->default_name<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the data of the random variables to database
void Fpl_Random_Variables::transfer_rand2database(QSqlDatabase *ptr_database, const _sys_system_id id_sys, const int section_id, const string mechanism, const string section_type){


	ostringstream cout;
	cout <<"Transfer random variable " << this->name << " to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	try{
		Fpl_Random_Variables::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Random_Variables::table->maximum_int_of_column(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Random_Variables::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::section_id) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(label::areastate_id) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(label::measure_id) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(label::applied_flag) <<" , ";

	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::mecha_name) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::zone_id) <<" , ";

	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::default_name) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::min_boundary) <<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::max_boundary)<<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_unit)<<" , ";
	query_string << Fpl_Random_Variables::table->get_column_name(fpl_label::section_type);
	query_string <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << section_id<< " , " ;
	query_string << id_sys.area_state << " , " ;
	query_string << id_sys.measure_nr << " , " ;
	query_string << "true" << " , " ;

	query_string <<"'"<< this->name << "' , " ;
	query_string <<"'"<< mechanism << "' , " ;
	query_string << this->zone_id << " , ";

	query_string <<"'"<< this->default_name << "' , " ;
	query_string <<"'"<< _Fpl_Distribution_Class::convert_dist_type2txt(this->distribution_type) << "' , " ;
	query_string << this->boundaries.min<< " , " ;
	query_string << this->boundaries.max<< " , " ;
	query_string <<"'"<< this->unit << "' , " ;
	query_string <<"'"<< section_type << "' ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Fpl_Random_Variables::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Section index             : " << section_id << endl;
		info << "Variable name             : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}


	if(this->default_name==fpl_label::given){
		//transfer the distribution type
		this->distribution->transfer_disttype2database(glob_id, ptr_database, this->flag_calculated);
	}

}
//Set the input for the distribution type per file; the parameter of the random variable has to be set before 
void Fpl_Random_Variables::set_distribution_types(QFile *ifile, int *line_counter){
	string myline;
	QString qmyline;
	stringstream my_stream;
	bool found_flag=false;

	//search for the begin
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);

		if(myline==fpl_label::begin_dist_type){
			found_flag=true;
			break;
		}
		if(myline==fpl_label::end_dist_type){;
			break;
		}
		if(myline==fpl_label::begin_random){;
			break;
		}
		if(myline==fpl_label::end_random){;
			break;
		}
	}
	while(ifile->atEnd()==false);


	if(found_flag==false && this->default_name==fpl_label::given && this->flag_calculated==true){
		this->set_determinstic_calculated_type();
		return;
	}
	else if(found_flag==false && this->default_name!=fpl_label::given && Sys_Project::get_project_type()!=_sys_project_type::proj_fpl_file){
		return;
	}
	else if(found_flag==false){
		Error msg=this->set_error(11);
		throw msg;
	}
	else{
		found_flag=false;
	}

	try{
		this->allocate_dist_type();
		//read it in
		this->distribution->set_input(ifile, line_counter, this->flag_calculated, this->boundaries);

	}
	catch(Error msg){
		throw msg;
	}


}
//Set a deterministic variable, where just the mid value is dynamically change during calculation
void Fpl_Random_Variables::set_determinstic_calculated_type(void){
/**
Main purpose of this method is the application for the result variables of the mechanisms, e.g. resistance
or stress, which are not randomly distributed. But they are dynamically calculated during a monte-carlo
simulation at each generation step. Thus, the mean of the dynamically set mean is important to store. 
*/

	this->flag_calculated=true;
	this->distribution_type=_fpl_distribution_type::mean_type;
	this->delete_dist_type();
	try{
		this->distribution= new Fpl_Distribution_Class_Mean(0.0, 0.0, 0.0,this->flag_calculated,this->boundaries,_fpl_mean_distribution_type::deterministic_type) ;
	}
	catch (bad_alloc &t){
		Error msg=set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Reset the flag that the variable was randomly generated
void Fpl_Random_Variables::reset_random_generated(void){
	this->flag_was_generated=false;
}
//Get the flag if the variable was randomly generated
bool Fpl_Random_Variables::get_random_generated(void){
	return this->flag_was_generated;
}
//Get calculated mean value of the distribution
double Fpl_Random_Variables::get_calculated_mean(void){
	return this->distribution->get_calculated_mean();
}
//Set the flag if the mean value is calculated for each calculation step (dynamic mean)
void Fpl_Random_Variables::set_calculated_flag(const bool flag){
	this->flag_calculated=flag;
}
//Set the name of the variable
void Fpl_Random_Variables::set_name(const string name){
	this->name=name;
}
//Set the boundaries for the result of the variable
void Fpl_Random_Variables::set_boundaries(const double min, const double max){
	this->boundaries.min=min;
	this->boundaries.max=max;
	this->distribution->set_boundaries(min,max);
}
//Set the maximum boundary for the result of the variable
void Fpl_Random_Variables::set_max_boundary(const double max){
	this->boundaries.max=max;
	this->distribution->set_boundaries(this->boundaries.min,max);
}
//Output the members to display/console
void Fpl_Random_Variables::output_member(bool detailed){
	ostringstream cout;
	ostringstream prefix;
	prefix << "RAND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << " Minimum boundary      : " << this->boundaries.min << this->unit<<endl;
	cout << " Name                  : " << this->name << this->unit  <<endl;
	cout << " Default name          : " << this->default_name <<endl;
	if(this->zone_id>=0){
		cout << " Zone id               : " << this->zone_id <<endl;
	}
	cout << " Maximum boundary      : " << this->boundaries.max << this->unit<<endl;
	cout << " Minimum boundary      : " << this->boundaries.min << this->unit<<endl;
	cout << " Mid dynamic           : " << functions::convert_boolean2string(this->flag_calculated) << endl;
	cout << " Distribution class    : " << _Fpl_Distribution_Class::convert_dist_type2txt(this->distribution_type) << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	//members of the distribution type
	if(this->distribution!=NULL){
		this->distribution->output_member(detailed);
	}
	else{
		cout <<"No distribution type is set!"<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the result of the randomly distributed variable (randomly:=true, deterministically:=false)
_fpl_result Fpl_Random_Variables::calculate_variables(const bool random_flag){
	if(random_flag==false){
		return this->make_deterministic();
	}
	else{
		return this->make_random();
	}
}
//Check the statistics of the random variables generation
void Fpl_Random_Variables::check_statistic(void){
	if(this->flag_was_generated==true){
		this->distribution->check_statistic();
	}
}
//Reset the statistic important values
void Fpl_Random_Variables::reset_statistics(void){
	this->distribution->reset_statistic();
	this->reset_random_generated();
}
//Output the statistics of the randomly generated variable to display/console
void Fpl_Random_Variables::output_statistic(bool detailed){
	ostringstream prefix;
	ostringstream cout;
	prefix << "RAND> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout << " Name                  : " << this->name << this->unit << endl;
	cout << " Distribution Class    : " << _Fpl_Distribution_Class::convert_dist_type2txt(this->distribution_type) << endl;
	cout << " Mid dynamic           : " << functions::convert_boolean2string(this->flag_calculated) << endl;
	if(this->boundary_counter>0){
		cout << " At boundary           : " << this->boundary_counter << endl;
		this->boundary_counter=0;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout, detailed);
	//statistics of the distrubution type
	this->distribution->output_statistic(this->flag_calculated, detailed);
	Sys_Common_Output::output_fpl->rewind_userprefix();
	//all members for the check statistics are reseted
	//this->distribution->reset_statistic();
}
//Output a deterministic result to display/console
void Fpl_Random_Variables::output_deterministic(void){
	//ostringstream prefix;
	ostringstream cout;
	/*prefix << "RV_"<<this->name<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());*/
	//cout << "DETERMINISTIC"<<endl;
	if(this->distribution_type==_fpl_distribution_type::mean2mean_type ||this->distribution_type==_fpl_distribution_type::discrete2mean_type){
		cout <<left<< W(20)<<this->name<<"  (1.) : " << this->get_result().inter_result<< endl;
		cout <<left<<W(20)<<this->name<<"  (2.) : " << this->get_result().end_result << this->unit << endl;
	}
	else{
		cout <<left<<W(20)<<this->name<<"   : " << this->get_result().end_result << this->unit << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout, false);
	//Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Set a new middle for the distribution classes: mean, mid (triangle), middle (discret), mean_1 (mean_mean), middle (discret_mean)
void Fpl_Random_Variables::set_new_middle(const double new_middle){
	if(flag_calculated==true && this->distribution!=NULL){
		//development
		/*ostringstream cout;
		cout << "Set new middle of Variable " << name << endl;
		Sys_Common_Output::output_fpl->output_txt(cout.str(),true);*/
		this->distribution->set_new_middle(new_middle);
		//distribution->output_member();
	}
	else{
		Warning msg=this->set_warning(1);
		msg.output_msg(1);
	}	
}
//Get the results directly from the distribution type
_fpl_result Fpl_Random_Variables::get_result(void){
	if(this->distribution!=NULL){
		return this->distribution->get_result();
	}
	else{
		_fpl_result buffer;
		buffer.end_result=0.0;
		buffer.inter_result=0.0;
		return buffer;
	}
}
//Get the distribution type of the random variable
_fpl_distribution_type Fpl_Random_Variables::get_distribution_type(void){
	return this->distribution_type;
}
//Get the name of the default distribution type; if no default distribution type is used "given" is set 
string Fpl_Random_Variables::get_default_name(void){
	return this->default_name;
}
//Get the determinstic mid-value (mean, middle etc.) of the distribution
double Fpl_Random_Variables::get_deterministic_mid(void){
	return this->distribution->make_deterministic().end_result;
}
//Get the zone-id
int Fpl_Random_Variables::get_zone_id(void){
	return this->zone_id;
}
//Set the position in database table of the distribution type
void Fpl_Random_Variables::set_dist_type_position(const int pos){
	this->dist_typ_position=pos;
}
//Set the default name
void Fpl_Random_Variables::set_default_name(const string def_name){
	this->default_name=def_name;
}
//Set the boundaries
void Fpl_Random_Variables::set_boundaries(const _fpl_boundaries bound){
	this->boundaries=bound;
}
//Set the distribution type
void Fpl_Random_Variables::set_distribution_type(const _fpl_distribution_type dist_type){
	this->distribution_type=dist_type;
}
//Set zone id
void Fpl_Random_Variables::set_zone_id(const int id){
	this->zone_id=id;
}
//Check given boundaries
void Fpl_Random_Variables::check_given_boundaries(const double max, const double min){

	if(this->boundaries.max>max){
		if(this->boundaries.max<constant::max_boundary){
			Warning msg=this->set_warning(3);
			ostringstream info;
			info<<"Variablename           : "<<this->name<<endl;
			info<<"Possible maximum value : "<<max<<this->unit<<endl;
			info<<"Given maximum boundary : "<<this->boundaries.max<<this->unit<<endl;
			msg.make_second_info(info.str());
			//msg.output_msg(1);
		}
		//reaction
		this->boundaries.max=max;
	}
	if(this->boundaries.min<min){
		if(this->boundaries.min>constant::min_boundary){
			Warning msg=this->set_warning(4);
			ostringstream info;
			info<<"Variablename           : "<<this->name<<endl;
			info<<"Possible minimum value : "<<min<<this->unit<<endl;
			info<<"Given minimum boundary : "<<this->boundaries.min<<this->unit<<endl;
			msg.make_second_info(info.str());
			//msg.output_msg(1);
		}
		//reaction
		this->boundaries.min=min;
	}

	if(this->flag_calculated==false && this->default_name==fpl_label::given){
		double mid=this->distribution->make_deterministic().end_result;
		if(mid>this->boundaries.max){
			//Error
			Error msg=this->set_error(13);
			ostringstream info;
			info<<"Variablename           : "<<this->name<<endl;
			info<<"Possible maximum value : "<<this->boundaries.max<<this->unit<<endl;
			info<<"Given mid value        : "<<mid<<this->unit<<endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		if(mid<this->boundaries.min){
			//Error
			Error msg=this->set_error(14);
			ostringstream info;
			info<<"Variablename           : "<<this->name<<endl;
			info<<"Possible minimum value : "<<this->boundaries.min<<this->unit<<endl;
			info<<"Given mid value        : "<<mid<<this->unit<<endl;
			msg.make_second_info(info.str());
			throw msg;

		}
	}

}
//Check for a positive variable
void Fpl_Random_Variables::check_positiv_variable(void){
	this->check_given_boundaries(constant::max_boundary, 1e-20);
}
//Check for a positive or zero variable
void Fpl_Random_Variables::check_positiv_zero_variable(void){
	this->check_given_boundaries(constant::max_boundary, 0.0);
}
//set the unit of the random variable
void Fpl_Random_Variables::set_unit(const string unit){
	this->unit=unit;
}
//_______________
//private
//Allocate the distribution type
void Fpl_Random_Variables::allocate_dist_type(void){
	this->delete_dist_type();
	try{
		switch(this->distribution_type){
			case (_fpl_distribution_type::mean_type):
				this->distribution= new Fpl_Distribution_Class_Mean;
				break;
			case (_fpl_distribution_type::triangle_type):
				this->distribution= new Fpl_Distribution_Class_Triangle;
				break;
			case (_fpl_distribution_type::discret_type):
				this->distribution= new Fpl_Distribution_Class_Discret;
				break;
			case (_fpl_distribution_type::mean2mean_type):
				this->distribution= new Fpl_Distribution_Class_Mean_Mean;
				break;
			case (_fpl_distribution_type::discrete2mean_type):
				this->distribution= new Fpl_Distribution_Class_Discret_Mean;
				break;
			default:
				Error msg=this->set_error(3);
				throw msg;
		}

	}
	catch (bad_alloc &t){
		Error msg=set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the distribution type
void Fpl_Random_Variables::delete_dist_type(void){
	if(this->distribution!=NULL){
		delete this->distribution;
		this->distribution=NULL;
	}
}
//Generate a random distributed variable
_fpl_result Fpl_Random_Variables::make_random(void){

	_fpl_result buffer;

	if(this->flag_was_generated==false){
		buffer=this->distribution->make_random();
		//check the boundaries
		if(buffer.end_result>this->boundaries.max){
			 buffer.end_result=this->boundaries.max;
			 this->distribution->set_result(buffer);
			 this->boundary_counter++;
		}
		if(buffer.end_result<this->boundaries.min){
			buffer.end_result=this->boundaries.min;
			this->distribution->set_result(buffer);
			this->boundary_counter++;
		}
		this->set_random_generated();
	}
	else{
		buffer=this->distribution->get_result();

	}
	return buffer;
}

//Generate a determinstic variable (mean, mid or a middle of the discret values)
_fpl_result Fpl_Random_Variables::make_deterministic(void){
	_fpl_result buffer;
	buffer=this->distribution->make_deterministic();
	this->set_random_generated();
	//check if negative results
	//check the boundaries
	if(buffer.end_result>this->boundaries.max){
		 buffer.end_result=this->boundaries.max;
		 this->distribution->set_result(buffer);
		 this->boundary_counter++;
	}
	if(buffer.end_result<this->boundaries.min){
		buffer.end_result=this->boundaries.min;
		this->distribution->set_result(buffer);
		this->boundary_counter++;
	}
	return buffer;
}
//Mark the variable, that it was is randomly generated
void Fpl_Random_Variables::set_random_generated(void){
	this->flag_was_generated=true;
}
//Delete the distribution type data in database table of the given random variable (static)
void Fpl_Random_Variables::delete_distribution_type_database(QSqlDatabase *ptr_database, QSqlQueryModel *results){

	string buffer_def;
	int buffer_pos;
	_fpl_distribution_type buffer_type;


	for(int i=0; i< results->rowCount(); i++){
		buffer_pos=results->record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id)).c_str()).toInt();
		buffer_def=results->record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::default_name)).c_str()).toString().toStdString();
		buffer_type=Fpl_Distribution_Class_Mean::convert_txt2dist_type(results->record(i).value((Fpl_Random_Variables::table->get_column_name(fpl_label::distribution_type)).c_str()).toString().toStdString());

		if(buffer_def==fpl_label::given){
			switch(buffer_type){
				case _fpl_distribution_type::mean_type:
					Fpl_Distribution_Class_Mean::delete_data_in_table(ptr_database, buffer_pos);
					break;
				case _fpl_distribution_type::triangle_type:
					Fpl_Distribution_Class_Triangle::delete_data_in_table(ptr_database, buffer_pos);
					break;
				case _fpl_distribution_type::discret_type:
					Fpl_Distribution_Class_Discret::delete_data_in_table(ptr_database, buffer_pos);
					break;
				case _fpl_distribution_type::mean2mean_type:
					Fpl_Distribution_Class_Mean_Mean::delete_data_in_table(ptr_database, buffer_pos);
					break;
				case _fpl_distribution_type::discrete2mean_type:
					Fpl_Distribution_Class_Discret_Mean::delete_data_in_table(ptr_database, buffer_pos);
					break;
				default:
					break;
			}
		}
	}
}
//Find the key-values for the file input of the random variable data
bool Fpl_Random_Variables::find_key_values_file(const string myline, int *must_found_counter, bool *name_found, const bool must_zone_found){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;


	pos=myline.find(fpl_label::key_name_var);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_name_var.length());
		functions::clean_string(&buffer);
		string name_buff;
		buffer1 << buffer;
		buffer1 >> name_buff;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else if(name_buff==this->name){
			*name_found=true;
			(*must_found_counter)++;
			return true;
		}
		else{
			return false;
		}
	}
	pos=myline.find(fpl_label::key_zone_id);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_zone_id.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->zone_id;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			if(must_zone_found==true){
				(*must_found_counter)++;
			}
			return true;
		}
	}
	pos=myline.find(fpl_label::key_dist_type);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_dist_type.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			this->distribution_type=_Fpl_Distribution_Class::convert_txt2dist_type(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
			throw msg;
		}
		(*must_found_counter)++;
		return true;
	}

	pos=myline.find(fpl_label::key_default_name);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_default_name.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->default_name;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return true;
		}
	}
	pos=myline.find(fpl_label::key_max_boundary);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_max_boundary.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->boundaries.max;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return true;
		}
	}
	pos=myline.find(fpl_label::key_min_boundary);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_min_boundary.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->boundaries.min;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return true;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(8);
		msg.make_second_info(info.str());
		throw msg;
	}
	return true;
}
//Set error(s)
Error Fpl_Random_Variables::set_error(const int err_type){
		string place="Fpl_Random_Variables::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;
	switch (err_type){
		case 0://invalid request
			place.append("set_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const int zone_number, const bool with_output)");
			reason="Invalid database request";
			help="Check the table in database";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://no results found (non fatal error)
			place.append("set_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const int zone_number, const bool with_output)");
			reason="No results found in table";
			help="Check the variable";
			type=2;
			break;
		case 2://bad alloc
			place.append("allocate_dist_type(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://unknown disttype
			place.append("allocate_dist_type(void)") ;
			reason="The distribution class is not known";
			help= "Check the variable";
			type=3;
			break;
		case 4://bad alloc
			place.append("set_determinstic_calculated_type(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://do not find the begin of the general section-type information in file
			place.append("set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str)");
			reason="Can not find the begin of the random variable in file";
			help="The random variable information has to start with !$BEGIN_RANDOM";
			type=1;
			break;
		case 6://do not find the end of the general section information in file
			place.append("set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str)");
			reason="Can not find the end of the random variable information in file";
			help="The random variable information has to end with !$END_RANDOM";
			type=1;
			break;
		case 7://do not find all required section infos
			place.append("set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str)");
			reason="Can not find all required random variable information";
			help="Check if all random variable information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_name_var<< endl;
			info << " " << fpl_label::key_dist_type<< endl;
			info << " " << fpl_label::key_zone_id << "(if zone-dependent variable)" <<endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_default_name<< " (just for database projects)"<< endl;
			info << " " << fpl_label::key_max_boundary<< endl;
			info << " " << fpl_label::key_min_boundary<< endl;
			type=1;
			break;
		case 8://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, bool *name_found)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the random variable information in file";
			type=1;
			break;
		case 9://variable is not fin in file
			place.append("set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str)");
			reason="The random variable is not found in the file";
			help="Check/add this variable to the file";
			type=1;
			break;
		case 10://no default value with by file projects
			place.append("set_input(QFile *ifile, int *line_counter, const string name, const bool calculated, const string end_block_str)");
			reason="The use of default values is not possible in file-projects";
			help="Specifiy the variable and their distribution type in file";
			type=1;
			break;
		case 11://do not find the begin of the general section-type information in file
			place.append("set_distribution_types(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the distribution type in file";
			help="The distribution type information has to start with !$BEGIN_DIST_TYPE";
			type=1;
			break;
		case 12://invalid request
			place.append("check_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const string section_type, const int zone_number)");
			reason="Invalid database request";
			help="Check the table in database";
			type=2;
			fatal=true;//fatal error;
			break;
		case 13://maximum boundary
			place.append("check_given_boundaries(const double max, const double min)") ;
			reason="The given mid value of the distribution is above the maximum boundary for this variable";
			help= "Check the distribution of the variable";
			type=3;
			break;
		case 14://minimum boundary
			place.append("check_given_boundaries(const double max, const double min)") ;
			reason="The given mid value of the distribution is below the mininum boundary for this variable";
			help= "Check the distribution of the variable";
			type=3;
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
Warning Fpl_Random_Variables::set_warning(const int warn_type){
		string place="Fpl_Random_Variables::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_input(const _sys_system_id id_sys, const int section_id, const string name_var, const bool calculated, QSqlDatabase *ptr_database, const string mechanism, const int zone_number, const bool with_output)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the variable";
			type=2;	
			break;
		case 1://it is not  a calculated random variable
			place.append("set_new_middle(const double new_middle)") ;
			reason="It is not a random variable, which has a dynamic mean setting";
			reaction="A new middle of the distribution classes can not be set";
			help= "Check the variable; set the flag_calculated by the method: void Fpl_Random_Variables::set_calculated(void)";
			info << "Variablename : " << this->name << endl;
			type=3;
			break;
		case 2://input datas can not submitted
			place.append("transfer_rand2database(QSqlDatabase *ptr_database, const _sys_system_id id_sys, const int section_id, const string mechanism, const string section_type, const int zone_number)");
			reason="Can not submit the random variable data to the database";
			help="Check the database";
			type=2;
			break;
		case 3://maximum boundary
			place.append("check_given_boundaries(const double max, const double min)") ;
			reason="The given maximum boundary is above the possible maximum value for this variable";
			reaction="The maximum boundary is adjusted to the maximum possible one";
			help= "Check the maximum boundary of the variable";
			type=3;
			break;
		case 4://minimum boundary
			place.append("check_given_boundaries(const double max, const double min)") ;
			reason="The given minimum boundary is below the possible minimum value for this variable";
			reaction="The minimum boundary is adjusted to the minimum possible one";
			help= "Check the minimum boundary of the variable";
			type=3;
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