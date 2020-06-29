//#include "Fpl_Distribution_Class_Triangle.h"
#include "Fpl_Headers_Precompiled.h"

//static members
Tables *Fpl_Distribution_Class_Triangle::table=NULL;
Tables *Fpl_Distribution_Class_Triangle::default_table=NULL;

///Default constructor
Fpl_Distribution_Class_Triangle::Fpl_Distribution_Class_Triangle(void){
	this->min=0.0;
	this->mid=0.0;
	this->max=0.0;
	this->max_rel=0.0;
	this->min_rel=0.0;
	this->constant=0.0;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Triangle), _sys_system_modules::FPL_SYS);//count the memory
}
///Constructor with all parameters given
Fpl_Distribution_Class_Triangle::Fpl_Distribution_Class_Triangle(const double var_min , const double var_mid, const double var_max, const double var_constant, const _fpl_boundaries boundaries, const bool calculated_mid){
	this->mid=var_mid;
	this->constant=var_constant;
	this->boundaries=boundaries;
	if(calculated_mid==false){
		this->min=var_min;
		this->max=var_max;
		this->max_rel=0.0;
		this->min_rel=0.0;
		this->absolut_to_relativ();
	}
	else{
		this->min=0.0;
		this->max=0.0;
		this->max_rel=var_max;
		this->min_rel=var_min;
		this->relativ_to_absolut();
	}

	//check the values
	this->check_input();
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Triangle), _sys_system_modules::FPL_SYS);//count the memory
}
///Default destructor
Fpl_Distribution_Class_Triangle::~Fpl_Distribution_Class_Triangle(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Distribution_Class_Triangle), _sys_system_modules::FPL_SYS);//count the memory
}
//_____________
//public
//Set the database table for the parameters of the triangle distribution types (user-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Triangle::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Triangle::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_triangle_user;
		string tab_col[6];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_id;
		tab_col[2]=fpl_label::triangle_mid;
		tab_col[3]=fpl_label::triangle_min;
		tab_col[4]=fpl_label::triangle_max;
		tab_col[5]=fpl_label::constant;

		try{
			//set new table
			Fpl_Distribution_Class_Triangle::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Triangle::table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Triangle::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Triangle::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the triangle distribution types (user-values) (static)
void Fpl_Distribution_Class_Triangle::create_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Triangle::table==NULL){
			ostringstream cout;
			cout << "Create random triangle type distributed variable database table (user defined)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_triangle_user;
			const int num_col=6;
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

			tab_col[1].name=fpl_label::variable_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=fpl_label::triangle_mid;
			tab_col[2].type=sys_label::tab_col_type_double;

			tab_col[3].name=fpl_label::triangle_min;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::triangle_max;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::constant;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].default_value="0.0";

			try{
				Fpl_Distribution_Class_Triangle::table= new Tables();
				if(Fpl_Distribution_Class_Triangle::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Triangle::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Triangle::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Triangle::close_table();
	}
}
//Delete all data in the database table for the parameters of the triangle distribution types (user-values) (static)
void Fpl_Distribution_Class_Triangle::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Triangle::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Triangle::table->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the parameters of the triangle distribution types (user-values) for a given random variable id (static)
void Fpl_Distribution_Class_Triangle::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id){
	try{
		Fpl_Distribution_Class_Triangle::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Distribution_Class_Triangle::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::variable_id) << " = " << rand_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Triangle::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the parameters of the triangle distribution types (default-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Triangle::set_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Triangle::default_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_triangle_default;
		string tab_col[8];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_name;
		tab_col[2]=fpl_label::default_name;
		tab_col[3]=fpl_label::triangle_mid;
		tab_col[4]=fpl_label::triangle_min;
		tab_col[5]=fpl_label::triangle_max;
		tab_col[6]=fpl_label::constant;
		tab_col[7]=label::description;

		//set new table
		try{
			Fpl_Distribution_Class_Triangle::default_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Triangle::default_table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc &t){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Triangle::set_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Triangle::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the triangle distribution types (default-values) (static)
bool Fpl_Distribution_Class_Triangle::create_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Triangle::default_table==NULL){
			ostringstream cout;
			cout << "Create random triangle type distributed variable database table (default)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_triangle_default;
			const int num_col=8;
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

			tab_col[1].name=fpl_label::variable_name;
			tab_col[1].type=sys_label::tab_col_type_string;
			tab_col[1].key_flag=true;

			tab_col[2].name=fpl_label::default_name;
			tab_col[2].type=sys_label::tab_col_type_string;
			tab_col[2].key_flag=true;

			tab_col[3].name=fpl_label::triangle_mid;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::triangle_min;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::triangle_max;
			tab_col[5].type=sys_label::tab_col_type_double;

			tab_col[6].name=fpl_label::constant;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=label::description;
			tab_col[7].type=sys_label::tab_col_type_string;

			try{
				Fpl_Distribution_Class_Triangle::default_table= new Tables();
				if(Fpl_Distribution_Class_Triangle::default_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					Fpl_Distribution_Class_Triangle::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Triangle::create_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Triangle::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Triangle::close_table();
		return true;
	}
	return false;
}
//Delete all data in the database table for the parameters of the triangle distribution types (default-values) (static)
void Fpl_Distribution_Class_Triangle::delete_data_in_default_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Triangle::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Triangle::default_table->delete_data_in_table(ptr_database);
	//restore predefined default variables
	Fpl_Distribution_Class_Triangle::write_default_variables2table(ptr_database);
}
//Prepare and write the predefined default variables to the database table (static)
void Fpl_Distribution_Class_Triangle::write_default_variables2table(QSqlDatabase *ptr_database){
	const int number=2;
	_fpl_default_triangle buff[number];

	buff[0].variable_name=fpl_label::water_density;
	buff[0].default_name=fpl_label::standard;
	buff[0].mid=1000.0;
	buff[0].max=1005.0;
	buff[0].min=997.0;
	buff[0].constant=0.0;
	buff[0].description="Density of normal water";

	buff[1].variable_name=fpl_label::water_density;
	buff[1].default_name="salty";
	buff[1].mid=1005.0;
	buff[1].max=1010.0;
	buff[1].min=1000.0;
	buff[1].constant=0.0;
	buff[1].description="Density of salty water, e.g. sea water";
	//...add more values; think to change the number

	try{
		Fpl_Distribution_Class_Triangle::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//set them to database table
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Distribution_Class_Triangle::default_table->maximum_int_of_column(Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Distribution_Class_Triangle::default_table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::default_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_mid) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_min) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_max) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::constant) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Triangle::default_table->get_column_name(label::description) <<" ) ";


	for(int i=0; i< number; i++){
		total.str("");
		ostringstream query_string;
		query_string << " VALUES ( ";
		query_string <<  id_glob << " , " ;
		query_string <<"'"<< buff[i].variable_name <<"'"<< " , " ;
		query_string <<"'"<< buff[i].default_name <<"'"<< " , " ;
		query_string << buff[i].mid << " , " ;
		query_string << buff[i].min << " , " ;
		query_string << buff[i].max << " , " ;
		query_string << buff[i].constant <<" , ";
		query_string <<"'"<< buff[i].description <<"'"<< " ) " ;
		
	
		total <<query_string_fix.str() <<  query_string.str();

		Data_Base::database_request(&model, total.str(), ptr_database);
		id_glob++;
	}

}
//Close and delete the database table for the parameters of the triangle distribution types the user-value-tables as well as the default table (static)
void Fpl_Distribution_Class_Triangle::close_table(void){
	if(Fpl_Distribution_Class_Triangle::table!=NULL){
		delete Fpl_Distribution_Class_Triangle::table;
		Fpl_Distribution_Class_Triangle::table=NULL;
	}
	if(Fpl_Distribution_Class_Triangle::default_table!=NULL){
		delete Fpl_Distribution_Class_Triangle::default_table;
		Fpl_Distribution_Class_Triangle::default_table=NULL;
	}
}
//Copy the parameters of the distribution type in the database table (static)
void Fpl_Distribution_Class_Triangle::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id){
	try{
		Fpl_Distribution_Class_Triangle::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Distribution_Class_Triangle::table->maximum_int_of_column(Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	
	QSqlQueryModel model1;
	ostringstream test_filter;

	test_filter << "INSERT INTO "<< Fpl_Distribution_Class_Triangle::table->get_table_name();
	test_filter << " SELECT " << glob_id <<" , " << new_rand_id<<" , ";
	test_filter <<  Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_mid)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_min)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_max)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::constant)  <<"  ";

	test_filter << " FROM " << Fpl_Distribution_Class_Triangle::table->get_table_name() <<" ";
	test_filter << " WHERE " << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::variable_id) << " = ";
	test_filter << old_rand_id;
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Triangle::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//read input per console
void Fpl_Distribution_Class_Triangle::set_input(void){
	//read input
	bool flag=false;
	do{
		//min value
		do{
			cout << "You chose a triangle distribution class!" << endl;
			cout << "Give the min please! " << endl;
			//if(!(flag=(cin  >> this->min))){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}while(flag==false);

		//mid-value
		do{
			cout << "Give the mid please! " << endl;
			//if(!(flag=(cin  >> this->mid))){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}while(flag==false);
		//max-value
		do{
			cout << "Give the max please! " << endl;
			//if(!(flag=(cin  >> this->max))){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}while(flag==false);
		//constant
		do{
			cout << "Give the constant please!" << endl;
			//if(!(flag=(cin  >> this->constant))){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}while(flag==false);


		if(this->mid<this->min || this->max < this->min || this->max< this->mid){
			cout << "There is a problem with min, mid, max! Give new values" << endl;
		}
	}
	while(this->mid<this->min || this->max < this->min || this->max< this->mid);
	this->absolut_to_relativ();
}
//Set the parameter via database table for a user defined variable
void Fpl_Distribution_Class_Triangle::set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;

	int number=0;
	try{
		Fpl_Distribution_Class_Triangle::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}



	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Triangle::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::variable_id) << " = " << position;
	
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(1);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(0);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}


	//input
	this->mid=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_mid)).c_str()).toDouble();
	this->constant=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::constant)).c_str()).toDouble();
	this->min=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
	this->max=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();

	if(calculated_mean==false){
		this->min=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
		this->max=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();
		this->absolut_to_relativ();
	}
	else{
		this->min_rel=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
		this->max_rel=results.record(0).value((Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();
		this->relativ_to_absolut();
	}

	this->check_input();
}
//Set the parameter via database table for a default variable
void Fpl_Distribution_Class_Triangle::set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	int number=0;
	try{
		Fpl_Distribution_Class_Triangle::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Triangle::default_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::variable_name) << " = '" << name <<"'";
	test_filter << " and ";
	test_filter << Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::default_name) << " = '" << default_name<<"'";

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Triangle::default_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(3);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Triangle::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(1);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Triangle::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	//input
	this->mid=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_mid)).c_str()).toDouble();
	this->constant=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::constant)).c_str()).toDouble();
	this->min=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
	this->max=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();

	if(calculated_mean==false){
		this->min=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
		this->max=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();
		this->absolut_to_relativ();
	}
	else{
		this->min_rel=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_min)).c_str()).toDouble();
		this->max_rel=results.record(0).value((Fpl_Distribution_Class_Triangle::default_table->get_column_name(fpl_label::triangle_max)).c_str()).toDouble();
		this->relativ_to_absolut();
	}

	this->check_input();
}
//Set the parameter via file for a user defined variable
void Fpl_Distribution_Class_Triangle::set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;

	//read in
	string myline;
	QString qmyline;
	int must_found_counter=0;
	int must_found=2;
	bool found_flag=false;

	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_dist_type){
			found_flag=true;
			break;
		}
		else if(myline==fpl_label::end_random){
			break;
		}
		else if(myline==fpl_label::begin_random){
			break;
		}
		else if(myline==fpl_label::begin_dist_type){
			break;
		}
		else if(myline.empty()==false){
			this->find_key_values_file(myline, &must_found_counter, calculated_mean);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(4);
		throw msg;
	}

	if(calculated_mean==true){
		must_found=2;
	}
	else{
		must_found=3;
	}

	if(must_found_counter!=must_found){
		Error msg=this->set_error(6);
		throw msg;
	}

	if(calculated_mean==true){
		this->relativ_to_absolut();
	}
	else{
		this->absolut_to_relativ();
	}

	this->check_input();

}
//Transfer the data of the distribution type to database table
void Fpl_Distribution_Class_Triangle::transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean){
	try{
		Fpl_Distribution_Class_Triangle::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Distribution_Class_Triangle::table->maximum_int_of_column(Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::glob_id), ptr_database)+1;


	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Distribution_Class_Triangle::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::variable_id) <<" , ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_mid) <<" , ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_min) <<" , ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::triangle_max) <<" , ";
	query_string << Fpl_Distribution_Class_Triangle::table->get_column_name(fpl_label::constant) <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << position<< " , " ;
	query_string << this->mid << " , " ;
	if(calculated_mean==false){
		query_string << this->min << " , " ;
		query_string << this->max << " , " ;
	}
	else{
		query_string << this->min_rel << " , " ;
		query_string << this->max_rel << " , " ;
	}
	query_string << this->constant << " ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(5);
		ostringstream info;
		info << "Table Name                : " << Fpl_Distribution_Class_Triangle::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Set the input of the distribution type via dialog for checking the random generator and the transformation
bool Fpl_Distribution_Class_Triangle::set_input_check(void){
	FplGui_Check_Disttype_Triangle_Dia my_dia;

	if(my_dia.start_dialog()==false){
		return false;
	}
	else{

		this->min=my_dia.get_min();
		this->mid=my_dia.get_mid();
		this->max=my_dia.get_max();
		this->constant=my_dia.get_constant();
		this->absolut_to_relativ();
		try{
			this->check_input();
		}
		catch(Error msg){
			throw msg;
		}
		return true;
	}
}
//Output the member of the distribution type to console/display
void Fpl_Distribution_Class_Triangle::output_member(bool detailed){
	ostringstream buff_rel_min;
	buff_rel_min<<this->min_rel;
	ostringstream buff_rel_max;
	buff_rel_max<<this->max_rel;
	if(this->mid==0.0){
		buff_rel_min.str("");
		buff_rel_max.str("");
		buff_rel_min<<"NaN ";
		buff_rel_max<<"NaN ";
	}
	ostringstream cout;
	cout << " Minimum absolut (rel) : " << this->min << " ("<<buff_rel_min.str()<<label::percentage<<")"<< endl;
	cout << " Mid value             : " << this->mid << endl;
	cout << " Maximum absolut (rel) : " << this->max << " ("<<buff_rel_max.str()<<label::percentage<<")"<< endl;
	cout << " Constant              : " << this->constant << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	}
//Generate for the distribution a determinstic value (mid_value+constant)
_fpl_result Fpl_Distribution_Class_Triangle::make_deterministic(void){
	this->random_result.end_result=this->mid+this->constant;
	this->random_result.inter_result=this->mid+this->constant;
	return this->random_result;	
};
//Generate for the distribution a random distributed value
_fpl_result Fpl_Distribution_Class_Triangle::make_random(void){
	double random;
	
	//generation of the uniformly-distributed numbers [0..1]
	random=this->ran1();
	//case random<=0 or random>=1: it is not possible for the log-function
	while(random<=0 || random >=1.0){
		random=this->ran1();
	}

	//transformation to triangle-distributed numbers
	if(random<=(this->mid-this->min)/(this->max-this->min)){
		this->random_result.end_result=sqrt(random*((this->max-this->min)*(this->mid-this->min)))+this->min;
	}
	else if(random>(this->mid-this->min)/(this->max-this->min)){
		this->random_result.end_result=this->max-sqrt((1.0-random)*((this->max-this->min)*(this->max-this->mid)));
	}

	this->random_result.end_result=this->random_result.end_result+this->constant;
	this->random_result.inter_result=this->random_result.end_result;
return this->random_result;
}
//Statistic of the generation process: output it to console/display
void Fpl_Distribution_Class_Triangle::output_statistic(const bool calc_flag, bool detailed){
	ostringstream cout;
	cout << " Numbers of generation : " << this->counter_calc << endl;
	cout << " Mean calculated       : " << this->mean_calc <<endl;
	cout << " Standard calculated   : " << this->std_dev_calc << endl;
	if(calc_flag==false){
		cout << " Mid value             : " << this->mid <<endl;
	}
	else{
		cout << " Dynamic mean settings : " << this->counter_set_mid << endl;
		if(this->counter_set_mid!=0){
			cout << " Mean of dynamic mean  : " << this->mid_calc/(double)this->counter_set_mid << " (" << this->mean_calc <<")"<< endl;
		}
		else{
			cout << " No dynamic mean is set"<<endl;
		}
	}
	if(this->counter_calc!=0){
		cout << " Minimum               : " << this->min_calc << " (" << this->min+this->constant<<") " << " [" <<(this->min+this->constant)-this->min_calc<<"]"<< endl;
		cout << " Maximum               : " << this->max_calc << " (" << this->max+this->constant<<") " << " [" <<(this->max+this->constant)-this->max_calc<<"]"<< endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);	
}
//Set a new mid value of the distribution (dynamic mid-setting)
void Fpl_Distribution_Class_Triangle::set_new_middle(const double new_mid){
	this->mid=new_mid;
	//sumup the new middle
	this->mid_calc=this->mid_calc+mid; //for statistical reason
	this->counter_set_mid++;//for statistical reason
	//calculate from the relativ value (max/min) the absolute max/min
	this->relativ_to_absolut();
}
//Get calculated exact probability for the distribution for the random generator check
double Fpl_Distribution_Class_Triangle::get_calulculated_exact_prob(const double value, const double){
	double res=0.0;
	double buff=value-this->constant;
	//calculation of the triangle-distribution
	if(buff>=this->min && buff<this->mid){
		res=(2.0*(buff-this->min)/((this->max-this->min)*(this->mid-this->min)));
	}
	else if(buff>=this->mid && buff <=this->max){
		res=(2.0*(this->max-buff)/((this->max-this->min)*(this->max-this->mid)));	
	}
	else{
		res=0.0;
	}
	return res;
}
//________________
//private
//check the input
void Fpl_Distribution_Class_Triangle::check_input(void){

	if( this->mid<this->boundaries.min || this->min<this->boundaries.min){
		//external warning
		Warning msg=this->set_warning(3);
		msg.output_msg(1);
		//reaction
		if(this->mid<this->boundaries.min){
			this->mid=this->boundaries.min;
		}
		if(this->min<this->boundaries.min){
			this->min=this->boundaries.min;
		}
	}
	if( this->mid>this->boundaries.max || this->max>this->boundaries.max){
		//external warning
		Warning msg=this->set_warning(4);
		msg.output_msg(1);
		//reaction
		if(this->mid>this->boundaries.max){
			this->mid=this->boundaries.max;
		}
		if(this->max>this->boundaries.max){
			this->max=this->boundaries.max;
		}
	}
	if(this->mid < this->min || this->max < this->min || this->max< this->mid ){
		//warning
		Warning msg=this->set_warning(2);
		msg.output_msg(1);
		//reaction
		if (this->min > this->mid){//adapt the values 
			this->min=this->mid;
		}
		else if(this->max < this->mid){
			this->max=this->mid;		
		}	
	}
}
//Find the key-values for the file input of the distribution type
void Fpl_Distribution_Class_Triangle::find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_mid_value);
	if(pos>=0 && wrong_input==false && calculated_flag==false){
		buffer=myline.substr(fpl_label::key_mid_value.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->mid;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(fpl_label::key_max_value);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_max_value.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		if(calculated_flag==false){
			buffer1 >> this->max;
		}
		else{
			buffer1>>this->max_rel;
		}
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(fpl_label::key_min_value);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_min_value.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		if(calculated_flag==false){
			buffer1 >> this->min;
		}
		else{
			buffer1>>this->min_rel;
		}
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_constant);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_constant.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1>>this->constant;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(5);
		msg.make_second_info(info.str());
		throw msg;
	}
}	
//Convert the absolute values (max/min) to relative values (max/min); required for the dynamic mid-value setting
void Fpl_Distribution_Class_Triangle::absolut_to_relativ(void){
	//here the min and max are relativ values [%]
	if(this->mid==0.0 && this->min==0.0 && this->max==0.0){
		this->min_rel=0.0;
		this->max_rel=0.0;
	}
	else if(this->mid==0.0){
		if(this->min==0.0){
			this->min_rel=0.0;
			this->max_rel=((this->max-(1e-22))/1e-22)*100.0;
		}
		else if(this->max==0.0){
			this->max_rel=0.0;
			this->min_rel=(((1e-22)-this->min)/1e-22)*100.0;
		}
		else{
			this->max_rel=((this->max-(1e-22))/1e-22)*100.0;
			this->min_rel=(((1e-22)-this->min)/1e-22)*100.0;
		}
	}
	else{
		this->min_rel=((this->mid-this->min)/this->mid)*100.0;
		this->max_rel=((this->max-this->mid)/this->mid)*100.0;
	}

}
//Convert the relative value (max/min) to absolute values (max/min); required for the dynamic mid-value setting
void Fpl_Distribution_Class_Triangle::relativ_to_absolut(void){
	//here the min and max are relativ values [%]
	this->min=this->mid-this->mid*this->min_rel/100.0;
	this->max=this->mid+this->mid*this->max_rel/100.0;
}
//Set error(s)
Error Fpl_Distribution_Class_Triangle::set_error(const int err_type){
		string place="Fpl_Distribution_Class_Triangle::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://wrong sql request
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 1://no variable found
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 2://wrong sql request
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 3://no variable found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 4://do not find the end of the general section information in file
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find the end of the distribution type information in file";
			help="The the distribution type information has to end with !$END_DIST_TYPE";
			type=1;
			break;
		case 5://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the distribution type information of the triangle-type in file";
			type=1;
			break;
		case 6://do not find all required section infos
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find all required triangle type distribution information";
			help="Check if all mean type distribution information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_min_value<< endl;
			info << " " << fpl_label::key_max_value<< endl;
			info << "Optional if calculated mid, else required:"<<endl;
			info << " " << fpl_label::key_mid_value<< endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_constant<< endl;
			type=1;
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
Warning Fpl_Distribution_Class_Triangle::set_warning(const int warn_type){
		string place="Fpl_Distribution_Class_Triangle::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the random variable triangle type distribution";
			type=2;	
			break;
		case 1://no unique result found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the random variable default triangle type distribution";
			type=2;	
			break;
		case 2://min mid max do not match
			place.append("check_input(void)") ;
			reason="Min, Mid , Max- Values are not valid";
			reaction="Values Min/Max are adapted to Mid Value";
			help= "Check the variable";
			info << "; Mid " << mid << "; Min " << min << "; Max " << max << "; Constant " << constant <<  endl;
			type=3;
			break;
		case 3://mean is < boundary min 
			place.append("check_input(void)") ;
			reason="The mid/min is smaller than the min boundary";
			reaction="Value is set to min boundary";
			help= "Check the variable";
			info << "; Mid " << mid << "; Min " << min << "; Max " << max << "; Constant " << constant <<  endl;
			info << "New Value "<< this->boundaries.min << endl;
			type=3;
			break;
		case 4://mean is < 0.0 and nonnegative flag is set
			place.append("check_input(void)") ;
			reason="The mid/max is greater than the max boundary";
			reaction="Value is set to max boundary";
			help= "Check the variable";
			info << "; Mid " << mid << "; Min " << min << "; Max " << max << "; Constant " << constant <<  endl;
			info << "New Value "<< this->boundaries.max << endl;
			type=3;
			break;
		case 5://input datas can not submitted
			place.append("transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)");
			reason="Can not submit the distribution type data (triangle-type) to the database";
			help="Check the database";
			type=2;
			break;
		default:
			place.append("set_warning(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
};