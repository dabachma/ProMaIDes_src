//#include "Fpl_Distribution_Class_Mean_Mean.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
Tables *Fpl_Distribution_Class_Mean_Mean::table=NULL;
Tables *Fpl_Distribution_Class_Mean_Mean::default_table=NULL;

///Default constructor
Fpl_Distribution_Class_Mean_Mean::Fpl_Distribution_Class_Mean_Mean(void){
	this->distribution1=NULL;
	this->distribution2=NULL;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Mean_Mean), _sys_system_modules::FPL_SYS);//count the memory	
}
///Constructor with all parameters given
Fpl_Distribution_Class_Mean_Mean::Fpl_Distribution_Class_Mean_Mean(_fpl_mean_distribution_type type1, const double mean1, const double second_moment1, const double constant1, const bool calculated_mean1, _fpl_mean_distribution_type type2, const double var_koef_2, const double constant2, const _fpl_boundaries boundaries){
	this->distribution1=NULL;
	this->distribution2=NULL;

	this->allocate_first_distribution(type1, mean1, second_moment1, constant1, calculated_mean1, boundaries);
	this->allocate_second_distribution(mean1, type2, var_koef_2, constant2, boundaries);

	//allocate and set the distribution

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Mean_Mean), _sys_system_modules::FPL_SYS);//count the memory	
}
///Default destructor
Fpl_Distribution_Class_Mean_Mean::~Fpl_Distribution_Class_Mean_Mean(void){
	this->delete_first_distribution();
	this->delete_second_distribution();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Distribution_Class_Mean_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Set the database table for the parameters of the mean2mean distribution types (user-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Mean_Mean::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean_Mean::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_mean2mean_user;
		string tab_col[9];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_id;
		tab_col[2]=fpl_label::mean2mean_type_1;
		tab_col[3]=fpl_label::mean2mean_mean_1;
		tab_col[4]=fpl_label::mean2mean_second_moment_1;
		tab_col[5]=fpl_label::mean2mean_constant_1;
		tab_col[6]=fpl_label::mean2mean_type_2;
		tab_col[7]=fpl_label::mean2mean_second_moment_2;
		tab_col[8]=fpl_label::mean2mean_constant_2;


		//set new table
		try{
			Fpl_Distribution_Class_Mean_Mean::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Mean_Mean::table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Mean_Mean::set_table( QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Mean_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the mean2mean distribution types (user-values) (static)
void Fpl_Distribution_Class_Mean_Mean::create_table(QSqlDatabase *ptr_database){
		if(Fpl_Distribution_Class_Mean_Mean::table==NULL){
			ostringstream cout;
			cout << "Create random mean2mean type distributed variable database table (user defined)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_mean2mean_user;
			const int num_col=9;
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

			tab_col[2].name=fpl_label::mean2mean_type_1;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=fpl_label::mean2mean_mean_1;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::mean2mean_second_moment_1;
			tab_col[4].unsigned_flag=true;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::mean2mean_constant_1;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].default_value="0.0";

			tab_col[6].name=fpl_label::mean2mean_type_2;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=fpl_label::mean2mean_second_moment_2;
			tab_col[7].unsigned_flag=true;
			tab_col[7].type=sys_label::tab_col_type_double;

			tab_col[8].name=fpl_label::mean2mean_constant_2;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";

			try{
				Fpl_Distribution_Class_Mean_Mean::table= new Tables();
				if(Fpl_Distribution_Class_Mean_Mean::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Mean_Mean::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Mean_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Mean_Mean::close_table();
	}
}
//Delete all data in the database table for the parameters of the mean2mean distribution types (user-values) (static)
void Fpl_Distribution_Class_Mean_Mean::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Mean_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Mean_Mean::table->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the parameters of the mean2mean distribution types (user-values) for a given random variable id (static)
void Fpl_Distribution_Class_Mean_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id){
	try{
		Fpl_Distribution_Class_Mean_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::variable_id) << " = " << rand_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Mean_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the parameters of the mean2mean distribution types (default-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Mean_Mean::set_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean_Mean::default_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_mean2mean_default;
		string tab_col[11];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_name;
		tab_col[2]=fpl_label::default_name;
		tab_col[3]=fpl_label::mean2mean_type_1;
		tab_col[4]=fpl_label::mean2mean_mean_1;
		tab_col[5]=fpl_label::mean2mean_second_moment_1;
		tab_col[6]=fpl_label::mean2mean_constant_1;
		tab_col[7]=fpl_label::mean2mean_type_2;
		tab_col[8]=fpl_label::mean2mean_second_moment_2;
		tab_col[9]=fpl_label::mean2mean_constant_2;
		tab_col[10]=label::description;


		//set new table
		try{
			Fpl_Distribution_Class_Mean_Mean::default_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Mean_Mean::default_table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Mean_Mean::set_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Mean_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the mean2mean distribution types (default-values) (static)
bool Fpl_Distribution_Class_Mean_Mean::create_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Mean_Mean::default_table==NULL){
			ostringstream cout;
			cout << "Create random mean2mean type distributed variable database table (default)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_mean2mean_default;
			const int num_col=11;
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

			tab_col[3].name=fpl_label::mean2mean_type_1;
			tab_col[3].type=sys_label::tab_col_type_string;

			tab_col[4].name=fpl_label::mean2mean_mean_1;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::mean2mean_second_moment_1;
			tab_col[5].unsigned_flag=true;
			tab_col[5].type=sys_label::tab_col_type_double;

			tab_col[6].name=fpl_label::mean2mean_constant_1;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=fpl_label::mean2mean_type_2;
			tab_col[7].type=sys_label::tab_col_type_string;

			tab_col[8].name=fpl_label::mean2mean_second_moment_2;
			tab_col[8].unsigned_flag=true;
			tab_col[8].type=sys_label::tab_col_type_double;

			tab_col[9].name=fpl_label::mean2mean_constant_2;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=label::description;
			tab_col[10].type=sys_label::tab_col_type_string;

			try{
				Fpl_Distribution_Class_Mean_Mean::default_table= new Tables();
				if(Fpl_Distribution_Class_Mean_Mean::default_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					Fpl_Distribution_Class_Mean_Mean::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Mean_Mean::create_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Mean_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Mean_Mean::close_table();
		return true;
	
	}
	return false;
}
//Delete all data in the database table for the parameters of the mean2mean distribution types (default-values) (static)
void Fpl_Distribution_Class_Mean_Mean::delete_data_in_default_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Mean_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Mean_Mean::default_table->delete_data_in_table(ptr_database);
	//restore predefined default variables
	Fpl_Distribution_Class_Mean_Mean::write_default_variables2table(ptr_database);
}
//Prepare and write the predefined default variables to the database table (static)
void Fpl_Distribution_Class_Mean_Mean::write_default_variables2table(QSqlDatabase *ptr_database){
	const int number=1;
	_fpl_default_mean2mean buff[number];

	buff[0].variable_name=fpl_label::water_density;
	buff[0].default_name=fpl_label::standard;
	buff[0].description="Density of normal water";
	buff[0].mean_1=1000.0;
	buff[0].second_moment_1=0.05;
	buff[0].constant_1=0.0;
	buff[0].type_1=_fpl_mean_distribution_type::normal_type;
	buff[0].var_coef_2=0.01;
	buff[0].constant_2=0.0;
	buff[0].type_2=_fpl_mean_distribution_type::normal_type;
	//...add more values; think to change the number

	try{
		Fpl_Distribution_Class_Mean_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//set them to database table
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Distribution_Class_Mean_Mean::default_table->maximum_int_of_column(Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Distribution_Class_Mean_Mean::default_table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::default_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_type_1) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_mean_1) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_second_moment_1) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_constant_1) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_type_2) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_second_moment_2) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_constant_2) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(label::description) <<" ) ";


	for(int i=0; i< number; i++){
		total.str("");
		ostringstream query_string;
		query_string << " VALUES ( ";
		query_string <<  id_glob << " , " ;
		query_string <<"'"<< buff[i].variable_name <<"'"<< " , " ;
		query_string <<"'"<< buff[i].default_name <<"'"<< " , " ;
		query_string <<"'"<< Fpl_Distribution_Class_Mean::convert_meandist_type2txt(buff[i].type_1) <<"'"<< " , " ;
		query_string << buff[i].mean_1 << " , " ;
		query_string << buff[i].second_moment_1<< " , " ;
		query_string << buff[i].constant_1 <<" , ";
		query_string <<"'"<< Fpl_Distribution_Class_Mean::convert_meandist_type2txt(buff[i].type_2) <<"'"<< " , " ;
		query_string << buff[i].var_coef_2<< " , " ;
		query_string << buff[i].constant_2 <<" , ";
		query_string <<"'"<< buff[i].description <<"'"<< " ) " ;
		
	
		total <<query_string_fix.str() <<  query_string.str();

		Data_Base::database_request(&model, total.str(), ptr_database);
		id_glob++;
	}

}
//Close and delete the database table for the parameters of the mean2mean distribution types the user-value-tables as well as the default table
void Fpl_Distribution_Class_Mean_Mean::close_table(void){
	if(Fpl_Distribution_Class_Mean_Mean::table!=NULL){
		delete Fpl_Distribution_Class_Mean_Mean::table;
		Fpl_Distribution_Class_Mean_Mean::table=NULL;
	}
	if(Fpl_Distribution_Class_Mean_Mean::default_table!=NULL){
		delete Fpl_Distribution_Class_Mean_Mean::default_table;
		Fpl_Distribution_Class_Mean_Mean::default_table=NULL;
	}
}
//Copy the parameters of the distribution type in the database table (static)
void Fpl_Distribution_Class_Mean_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id){

	try{
		Fpl_Distribution_Class_Mean_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Distribution_Class_Mean_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	
	QSqlQueryModel model1;
	ostringstream test_filter;

	test_filter << "INSERT INTO "<< Fpl_Distribution_Class_Mean_Mean::table->get_table_name();
	test_filter << " SELECT " << glob_id <<" , " << new_rand_id<<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_1)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_mean_1)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_1)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_1)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_2)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_2)  <<" , ";
	test_filter <<  Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_2)  <<"  ";

	test_filter << " FROM " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() <<" ";
	test_filter << " WHERE " << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::variable_id) << " = ";
	test_filter << old_rand_id;
	Data_Base::database_request(&model1, test_filter.str(), ptr_database);
	if(model1.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Mean_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Table error info: " << model1.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the parameter via console
void Fpl_Distribution_Class_Mean_Mean::set_input(void){
	double set_double=0.0;
	bool flag=false;
	try{
		//input of the first
		cout << "You chose a combination between two mean_type distributions!" << endl;
		cout << "Give the data of the 1. distribution !" << endl; 
		//init the first mean_type distributions
		this->allocate_first_distribution();
		//input via console of the first distribution
		this->distribution1->set_input();


		//second distribution type
		_fpl_mean_distribution_type buffer_type=_fpl_mean_distribution_type::unknown_mean_type;
		//distribution type 2. distribution
		do{
			cout << "Give the 2. Distribution type!"<< endl;
			cout << "(0) deterministic, (1) uniform, (2) normal, (3) expo, (4) lognormal, (5) gumbel, (6) weibull " << endl;
			int buffer=0;
			//if((flag=(cin  >> buffer))==false){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
			//else if((buffer_type=Fpl_Distribution_Class_Mean::convert_int2meandist_type(buffer))==_fpl_mean_distribution_type::unknown_mean_type){
			//	cout << "Unknwon distribution type! Do it again! " << endl;
			//}
		}while(flag==false || buffer_type==_fpl_mean_distribution_type::unknown_mean_type);

		//variation coefficient 2. distribution
		do{
			cout << "Give the Variation Coeffiecient for the 2. distribution please !" << endl;
			//if((flag=(cin  >> set_double))==false){//check if the input is valif
			//		cout << "Wrong input! Do it again" << endl;
			//		cin.clear();//clear the error flags
			//		cin.ignore();//clear cin
			//}
			//else if(set_double <0.0){
			//	cout << "No negative Standard deviation is possible! Do it again! " << endl;
			//}
		}while(flag==false || set_double <0.0);

		//init the second mean_type distributions
		this->allocate_second_distribution(0.0, buffer_type, set_double, 0.0, this->boundaries);
	}
	catch(bad_alloc& ){
		Error msg=set_error(1);
		throw msg;	
	}
	catch(Error msg){
		throw msg;
	}
	
}
//Set the parameter via database table for a user defined variable
void Fpl_Distribution_Class_Mean_Mean::set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;

	int number=0;
	try{
		Fpl_Distribution_Class_Mean_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}



	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::variable_id) << " = " << position;
	
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(4);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(0);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	_fpl_default_mean2mean buffer;
	buffer.default_name=label::not_defined;
	buffer.variable_name=label::not_defined;

	//input
	buffer.mean_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_mean_1)).c_str()).toDouble();
	buffer.constant_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_1)).c_str()).toDouble();
	buffer.second_moment_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_1)).c_str()).toDouble();
	buffer.type_1=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_1)).c_str()).toString().toStdString());

	buffer.constant_2=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_2)).c_str()).toDouble();
	buffer.var_coef_2=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_2)).c_str()).toDouble();
	buffer.type_2=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_2)).c_str()).toString().toStdString());

	//alloctae them
	try{
		this->allocate_first_distribution();
		this->allocate_second_distribution();
	}
	catch(Error msg){
		throw msg;
	}

	//set them
	this->distribution1->set_input(buffer.mean_1, buffer.second_moment_1, buffer.constant_1, calculated_mean, this->boundaries, buffer.type_1);
	this->distribution2->set_input(buffer.mean_1, buffer.var_coef_2, buffer.constant_2, true, this->boundaries, buffer.type_2);

	//check them
	try{
		this->distribution1->check_input(calculated_mean);
		this->distribution2->check_input(true);
	}
	catch(Error msg){
		throw msg;
	}

}
//Set the parameter via database table for a default variable
void Fpl_Distribution_Class_Mean_Mean::set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	int number=0;
	try{
		Fpl_Distribution_Class_Mean_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Mean_Mean::default_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::variable_name) << " = '" << name <<"'";
	test_filter << " and ";
	test_filter << Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::default_name) << " = '" << default_name<<"'";

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Mean_Mean::default_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results.rowCount();

	if(number==0){
		Error msg=this->set_error(6);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Mean_Mean::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(number>1){
		Warning msg=this->set_warning(1);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Mean_Mean::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
	}

	_fpl_default_mean2mean buffer;
	buffer.default_name=label::not_defined;
	buffer.variable_name=label::not_defined;

	//input
	buffer.mean_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_mean_1)).c_str()).toDouble();
	buffer.constant_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_constant_1)).c_str()).toDouble();
	buffer.second_moment_1=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_second_moment_1)).c_str()).toDouble();
	buffer.type_1=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_type_1)).c_str()).toString().toStdString());

	buffer.constant_2=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_constant_2)).c_str()).toDouble();
	buffer.var_coef_2=results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_second_moment_2)).c_str()).toDouble();
	buffer.type_2=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Mean_Mean::default_table->get_column_name(fpl_label::mean2mean_type_2)).c_str()).toString().toStdString());

	//alloctae them
	try{
		this->allocate_first_distribution();
		this->allocate_second_distribution();
	}
	catch(Error msg){
		throw msg;
	}

	//set them
	this->distribution1->set_input(buffer.mean_1, buffer.second_moment_1, buffer.constant_1, calculated_mean, this->boundaries, buffer.type_1);
	this->distribution2->set_input(buffer.mean_1, buffer.var_coef_2, buffer.constant_2, true, this->boundaries, buffer.type_2);

	//check them
	try{
		this->distribution1->check_input(calculated_mean);
		this->distribution2->check_input(true);
	}
	catch(Error msg){
		throw msg;
	}

}
//Set the parameter via file for a user defined variable
void Fpl_Distribution_Class_Mean_Mean::set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;

	//read in
	string myline;
	QString qmyline;
	int must_found_counter=0;
	int must_found=2;
	bool found_flag=false;

	_fpl_default_mean2mean buffer;
	buffer.default_name=label::not_defined;
	buffer.variable_name=label::not_defined;
	buffer.constant_1=0.0;
	buffer.constant_2=0.0;
	buffer.mean_1=0.0;
	buffer.second_moment_1=0.0;


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
			this->find_key_values_file(myline, &must_found_counter, calculated_mean, &buffer);
		}

	}
	while(ifile->atEnd()==false);

	if(found_flag==false){
		Error msg=this->set_error(8);
		throw msg;
	}

	if(calculated_mean==true){
		must_found=4;
	}
	else{
		must_found=5;
	}

	if(must_found_counter!=must_found){
		Error msg=this->set_error(9);
		throw msg;
	}

	//alloctae them
	try{
		this->allocate_first_distribution();
		this->allocate_second_distribution();
	}
	catch(Error msg){
		throw msg;
	}

	//set them
	this->distribution1->set_input(buffer.mean_1, buffer.second_moment_1, buffer.constant_1, calculated_mean, this->boundaries, buffer.type_1);
	this->distribution2->set_input(buffer.mean_1, buffer.var_coef_2, buffer.constant_2, true, this->boundaries, buffer.type_2);

	//check them
	try{
		this->distribution1->check_input(calculated_mean);
		this->distribution2->check_input(true);
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the data of the distribution type to database table
void Fpl_Distribution_Class_Mean_Mean::transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean){

	try{
		Fpl_Distribution_Class_Mean_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Distribution_Class_Mean_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	query_string << "INSERT INTO  " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::variable_id) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_1) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_mean_1) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_1) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_1) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_type_2) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_second_moment_2) <<" , ";
	query_string << Fpl_Distribution_Class_Mean_Mean::table->get_column_name(fpl_label::mean2mean_constant_2) <<" ) ";
	query_string << " VALUES ( ";

	query_string << glob_id << " , " ;
	query_string << position<< " , " ;
	query_string <<"'"<< this->distribution1->get_mean_distribution_type() << "' , " ;
	query_string << this->distribution1->get_mean_value() << " , " ;
	if(calculated_mean==false){
		query_string << this->distribution1->get_std_dev() << " , " ;
	}
	else{
		query_string << this->distribution1->get_var_coeff()<< " , " ;
	}
	query_string << this->distribution1->get_constant() << " , " ;
	query_string <<"'"<< this->distribution2->get_mean_distribution_type() << "' , " ;
	query_string << this->distribution2->get_var_coeff()<< " , " ;
	query_string << this->distribution2->get_constant() << " ) " ;

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Fpl_Distribution_Class_Mean_Mean::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Output the member of the distribution type to console/display
void Fpl_Distribution_Class_Mean_Mean::output_member(bool detailed){
	ostringstream cout;
	cout << " Leading (first) distribution:" << endl;	
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	if(this->distribution1!=NULL){
		this->distribution1->output_member();
	}
	else{
		cout << " No leading distribution is set" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	}
	cout << " Subordinated (second) distribution:" << endl;	
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	if(this->distribution2!=NULL){
		this->distribution2->output_member(detailed);
	}
	else{
		cout << " No subordinated distribution is set" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	}

}
//Generate for the distribution a deterministic value (mean_value+constant of the first distribution)
_fpl_result Fpl_Distribution_Class_Mean_Mean::make_deterministic(void){
	this->random_result.end_result=this->distribution1->mean;
	this->random_result.inter_result=this->distribution1->mean;
	return this->random_result;
};
//Generate for the distribution a random distributed value
_fpl_result Fpl_Distribution_Class_Mean_Mean::make_random(void){
	//first random generation of the first distribution is calculated => inter-result
	this->random_result.inter_result=(this->distribution1->make_random()).end_result;
	//mean of the second distribution ist changed with result of the first distribution 
	this->distribution2->set_new_middle(this->random_result.inter_result);
	//second random generation of the second distribution with the new mean value is calculated => end-result
	this->random_result.end_result=this->distribution2->make_random().end_result;
	return this->random_result;
}
//Statistic of the generation process: mean, standard deviation and minimum and maximum values are calculated for multiple random generations of the variable
void Fpl_Distribution_Class_Mean_Mean::check_statistic(void){
	//check statistics of the first distribution; there are no statistics of the second
	this->distribution1->check_statistic();
	//check of the combination
	_Fpl_Distribution_Class::check_statistic();
}
//Statistic of the generation process: output it to console/display
void Fpl_Distribution_Class_Mean_Mean::output_statistic(const bool calc_flag, bool detailed){
	ostringstream cout;
	cout << " Numbers of generation   : " << this->counter_calc << endl;
	cout << " Mean of combination     : " << this->mean_calc << endl;
	cout << " Standard of combination : " << this->std_dev_calc << endl;
	cout << "Statistics of the first distribution : " << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);	
	//output statistics of the first distribution; there are no statistics of the second
	this->distribution1->output_statistic(calc_flag, detailed);
}
//Reset the values for checking the statistics
void Fpl_Distribution_Class_Mean_Mean::reset_statistic(void){
	//reset this value for the calculation of the statistics
	_Fpl_Distribution_Class::reset_statistic();
	//reset statistics of the first distribution; there are no statistics of the second 
	this->distribution1->reset_statistic();
 }
//Set a new mean value of the distribution (dynamic mean-setting of the first distribution)
void Fpl_Distribution_Class_Mean_Mean::set_new_middle(const double new_mean1){
	this->distribution1->set_new_middle(new_mean1);	
}
//____________
//private
//Allocate the first (leading) distribution
void Fpl_Distribution_Class_Mean_Mean::allocate_first_distribution(void){
	this->delete_first_distribution();
	try{
		this->distribution1=new Fpl_Distribution_Class_Mean;
	}
	catch (bad_alloc &t){
		Error msg=set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Allocate the first (leading) distribution
void Fpl_Distribution_Class_Mean_Mean::allocate_first_distribution(_fpl_mean_distribution_type type1, const double mean1, const double second_moment1, const double constant1, const bool calculated_mean1, const _fpl_boundaries boundaries){
	this->delete_first_distribution();
	try{
		this->distribution1=new Fpl_Distribution_Class_Mean(mean1, second_moment1, constant1,calculated_mean1, boundaries, type1);
	}
	catch (bad_alloc &t){
		Error msg=set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}
}
//Delete the first (leading) distribution
void Fpl_Distribution_Class_Mean_Mean::delete_first_distribution(void){
	if(this->distribution1!=NULL){
		delete this->distribution1;
		this->distribution1=NULL;
	}
}
//Allocate the second (subordinated) distribution
void Fpl_Distribution_Class_Mean_Mean::allocate_second_distribution(void){
	this->delete_second_distribution();
	try{
		this->distribution2=new Fpl_Distribution_Class_Mean;
	}
	catch (bad_alloc &t){
		Error msg=set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Allocate the second (subordinated) distribution
void Fpl_Distribution_Class_Mean_Mean::allocate_second_distribution(const double mean1, _fpl_mean_distribution_type type2, const double var_koef_2, const double constant2, const _fpl_boundaries boundaries){
	this->delete_second_distribution();
	try{
		this->distribution2=new Fpl_Distribution_Class_Mean(mean1, var_koef_2, constant2, true, boundaries, type2);
	}
	catch (bad_alloc &t){
		Error msg=set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}
}
//Delete the second (subordinated) distribution
void Fpl_Distribution_Class_Mean_Mean::delete_second_distribution(void){
	if(this->distribution2!=NULL){
		delete this->distribution2;
		this->distribution2=NULL;
	}
}
//Find the key-values for the file input of the distribution type
void Fpl_Distribution_Class_Mean_Mean::find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag, _fpl_default_mean2mean *fill_it){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	//first distribution
	pos=myline.find(fpl_label::key_mean_value);
	if(pos>=0 && wrong_input==false && calculated_flag==false){
		buffer=myline.substr(fpl_label::key_mean_value.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> fill_it->mean_1;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}
	pos=myline.find(fpl_label::key_second_moment);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_second_moment.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> fill_it->second_moment_1;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}

	pos=myline.find(fpl_label::key_constant);
	int pos_2=myline.find("_2");
	if(pos>=0 && wrong_input==false && pos_2<0){
		buffer=myline.substr(fpl_label::key_constant.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1>>fill_it->constant_1;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}
	pos=myline.find(fpl_label::key_mean_type);
	pos_2=myline.find("_2");
	if(pos>=0 && wrong_input==false&& pos_2<0){
		buffer=myline.substr(fpl_label::key_mean_type.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			fill_it->type_1=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
			throw msg;
		}
		(*must_found_counter)++;
		return;
	}

	//second distribution
	pos=myline.find(fpl_label::key_mean_type_2);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_mean_type_2.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		try{
			fill_it->type_2=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(buffer1.str());
		}
		catch(Error msg){
			wrong_input=true;
			throw msg;
		}
		(*must_found_counter)++;
		return;
	}

	pos=myline.find(fpl_label::key_constant_2);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_constant_2.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1>>fill_it->constant_2;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}
	pos=myline.find(fpl_label::key_var_coeff_2);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_var_coeff_2.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1>>fill_it->var_coef_2;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			(*must_found_counter)++;
			return;
		}
	}


	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(7);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Distribution_Class_Mean_Mean::set_error(const int err_type){
		string place="Fpl_Distribution_Class_Mean_Mean::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1://bad_alloc
			place.append("allocate_first_distribution(_fpl_mean_distribution_type type1, const double mean1, const double second_moment1, const double constant1, const bool calculated_mean1, const _fpl_boundaries boundaries)") ;
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://bad_alloc
			place.append("allocate_second_distribution(const double mean1, _fpl_mean_distribution_type type2, const double var_koef_2, const double constant2, const _fpl_boundaries boundaries)") ;
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://wrong sql request
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 4://no variable found
			place.append("set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 5://wrong sql request
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 6://no variable found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 7://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag, _fpl_default_mean2mean *fill_it)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the distribution type information of the mean2mean-type in file";
			type=1;
			break;
		case 8://do not find the end of the distribtuiton type information in file
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find the end of the distribution type information in file";
			help="The the distribution type information has to end with !$END_DIST_TYPE";
			type=1;
			break;
		case 9://do not find all required section infos
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find all required mean2mean type distribution information";
			help="Check if all mean2mean type distribution information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_second_moment<< endl;
			info << " " << fpl_label::key_mean_type<< endl;
			info << " " << fpl_label::key_var_coeff_2<< endl;
			info << " " << fpl_label::key_mean_type_2<< endl;
			info << "Optional if calculated mean, else required:"<<endl;
			info << " " << fpl_label::key_mean_value<< endl;
			info << "Optional information:"<<endl;
			info << " " << fpl_label::key_constant<< endl;
			info << " " << fpl_label::key_constant_2<< endl;
			type=1;
			break;
		default:
			place.append("set_error(int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//Set warning(s)
Warning Fpl_Distribution_Class_Mean_Mean::set_warning(const int warn_type){
		string place="Fpl_Distribution_Class_Mean_Mean::";
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
			help= "Check the random variable mean2mean type distribution";
			type=2;	
			break;
		case 1://no unique result found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)") ;
			reason="No unique result is found";
			reaction="First result is taken";
			help= "Check the random variable default mean2mean type distribution";
			type=2;	
			break;
		case 2://input datas can not submitted
			place.append("transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)");
			reason="Can not submit the distribution type data (mean2mean-type) to the database";
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
