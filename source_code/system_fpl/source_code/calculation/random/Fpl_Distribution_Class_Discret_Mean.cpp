//#include "Fpl_Distribution_Class_Discret_Mean.h"
#include "Fpl_Headers_Precompiled.h"

//init static members
Tables *Fpl_Distribution_Class_Discret_Mean::table=NULL;
Tables *Fpl_Distribution_Class_Discret_Mean::default_table=NULL;

//Default constructor
Fpl_Distribution_Class_Discret_Mean::Fpl_Distribution_Class_Discret_Mean(void){

	this->number_distribution=0;
	this->distribution=NULL;
	this->distribution_array=NULL;
	this->hit_number=0;
	
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Discret_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Distribution_Class_Discret_Mean::~Fpl_Distribution_Class_Discret_Mean(void){
	this->delete_discrete_distribution();
	this->delete_mean_type_distributions();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Distribution_Class_Discret_Mean), _sys_system_modules::FPL_SYS);//count the memory
}
//____________
//public
//Set the database table for the parameters of the discrete2mean distribution types (user-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Discret_Mean::set_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret_Mean::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_discret2mean_user;
		string tab_col[9];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_id;
		tab_col[2]=fpl_label::discret_value_id;
		tab_col[3]=fpl_label::discret_value;
		tab_col[4]=fpl_label::discret_probability;
		tab_col[5]=fpl_label::mean_type;
		tab_col[6]=fpl_label::mean_mean;
		tab_col[7]=fpl_label::mean_second_moment;
		tab_col[8]=fpl_label::constant;

		//set new table
		try{
			Fpl_Distribution_Class_Discret_Mean::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Discret_Mean::table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Discret_Mean::set_table( QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Discret_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the discrete2mean distribution types (user-values)
void Fpl_Distribution_Class_Discret_Mean::create_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret_Mean::table==NULL){
			ostringstream cout;
			cout << "Create random discrete2mean type distributed variable database table (user defined)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_discret2mean_user;
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

			tab_col[2].name=fpl_label::discret_value_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=fpl_label::discret_value;
			tab_col[3].type=sys_label::tab_col_type_double;

			tab_col[4].name=fpl_label::discret_probability;
			tab_col[4].unsigned_flag=true;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::mean_type;
			tab_col[5].type=sys_label::tab_col_type_string;

			tab_col[6].name=fpl_label::mean_mean;
			tab_col[6].type=sys_label::tab_col_type_double;

			tab_col[7].name=fpl_label::mean_second_moment;
			tab_col[7].unsigned_flag=true;
			tab_col[7].type=sys_label::tab_col_type_double;

			tab_col[8].name=fpl_label::constant;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.0";

			try{
				Fpl_Distribution_Class_Discret_Mean::table= new Tables();
				if(Fpl_Distribution_Class_Discret_Mean::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Discret_Mean::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Discret_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Discret_Mean::close_table();
	}
}
//Delete all data in the database table for the parameters of the discrete2mean distribution types (user-values)
void Fpl_Distribution_Class_Discret_Mean::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Discret_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Discret_Mean::table->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the parameters of the discrete2mean distribution types (user-values) for a given random variable id (static)
void Fpl_Distribution_Class_Discret_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id){
	try{
		Fpl_Distribution_Class_Discret_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::variable_id) << " = " << rand_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Discret_Mean::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the parameters of the discrete2mean distribution types (default-values): it sets the table name and the name of the columns and allocate them
void Fpl_Distribution_Class_Discret_Mean::set_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret_Mean::default_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_discret2mean_default;
		string tab_col[11];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_name;
		tab_col[2]=fpl_label::default_name;
		tab_col[3]=fpl_label::discret_value_id;
		tab_col[4]=fpl_label::discret_value;
		tab_col[5]=fpl_label::discret_probability;
		tab_col[6]=fpl_label::mean_type;
		tab_col[7]=fpl_label::mean_mean;
		tab_col[8]=fpl_label::mean_second_moment;
		tab_col[9]=fpl_label::constant;
		tab_col[10]=label::description;

		//set new table
		try{
			Fpl_Distribution_Class_Discret_Mean::default_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Discret_Mean::default_table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Discret_Mean::set_default_table( QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Discret_Mean::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the discrete2mean distribution types (default-values)
bool Fpl_Distribution_Class_Discret_Mean::create_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret_Mean::default_table==NULL){
			ostringstream cout;
			cout << "Create random discrete2mean type distributed variable database table (default)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_discret2mean_default;
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

			tab_col[3].name=fpl_label::discret_value_id;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=fpl_label::discret_value;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=fpl_label::discret_probability;
			tab_col[5].unsigned_flag=true;
			tab_col[5].type=sys_label::tab_col_type_double;

			tab_col[6].name=fpl_label::mean_type;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=fpl_label::mean_mean;
			tab_col[7].type=sys_label::tab_col_type_double;

			tab_col[8].name=fpl_label::mean_second_moment;
			tab_col[8].unsigned_flag=true;
			tab_col[8].type=sys_label::tab_col_type_double;

			tab_col[9].name=fpl_label::constant;
			tab_col[9].type=sys_label::tab_col_type_double;
			tab_col[9].default_value="0.0";

			tab_col[10].name=label::description;
			tab_col[10].type=sys_label::tab_col_type_string;

			try{
				Fpl_Distribution_Class_Discret_Mean::default_table= new Tables();
				if(Fpl_Distribution_Class_Discret_Mean::default_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					Fpl_Distribution_Class_Discret_Mean::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Discret_Mean::create_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Discret_Mean::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Discret_Mean::close_table();
		return true;
	}
	return false;
}
//Delete all data in the database table for the parameters of the discrete2mean distribution types (default-values)
void Fpl_Distribution_Class_Discret_Mean::delete_data_in_default_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Discret_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Discret_Mean::default_table->delete_data_in_table(ptr_database);
	//restore predefined default variables
	Fpl_Distribution_Class_Discret_Mean::write_default_variables2table(ptr_database);
}
//Prepare and write the predefined default variables to the database table (static)
void Fpl_Distribution_Class_Discret_Mean::write_default_variables2table(QSqlDatabase *ptr_database){
	const int number=1;
	_fpl_default_discrete2mean buff[number];

	buff[0].variable_name=fpl_label::wind_speed_direction;
	buff[0].default_name=fpl_label::standard;
	buff[0].description="Standard wind for Germany";
	buff[0].number=4;
	buff[0].distributions=new _fpl_discret2mean_data[buff[0].number];
	buff[0].distributions[0].discret_point.abs_discrete_value=0.0;
	buff[0].distributions[0].discret_point.probability=20.0;
	buff[0].distributions[0].mean_type=_fpl_mean_distribution_type::gumbel_type;
	buff[0].distributions[0].mean=14.0;
	buff[0].distributions[0].second_moment=2.9;
	buff[0].distributions[0].constant=0.0;
	buff[0].distributions[1].discret_point.abs_discrete_value=90.0;
	buff[0].distributions[1].discret_point.probability=40.0;
	buff[0].distributions[1].mean_type=_fpl_mean_distribution_type::gumbel_type;
	buff[0].distributions[1].mean=17.1;
	buff[0].distributions[1].second_moment=3.1;
	buff[0].distributions[1].constant=0.0;
	buff[0].distributions[2].discret_point.abs_discrete_value=180.0;
	buff[0].distributions[2].discret_point.probability=30.0;
	buff[0].distributions[2].mean_type=_fpl_mean_distribution_type::gumbel_type;
	buff[0].distributions[2].mean=16.2;
	buff[0].distributions[2].second_moment=3.0;
	buff[0].distributions[2].constant=0.0;
	buff[0].distributions[3].discret_point.abs_discrete_value=270.0;
	buff[0].distributions[3].discret_point.probability=10.0;
	buff[0].distributions[3].mean_type=_fpl_mean_distribution_type::gumbel_type;
	buff[0].distributions[3].mean=15.5;
	buff[0].distributions[3].second_moment=2.5;
	buff[0].distributions[3].constant=0.0;
	//...add more values; think to change the number

	try{
		Fpl_Distribution_Class_Discret_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//set them to database table
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Distribution_Class_Discret_Mean::default_table->maximum_int_of_column(Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Distribution_Class_Discret_Mean::default_table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::default_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_value_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_value) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_probability) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_type) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_mean) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_second_moment) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::constant) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(label::description) <<" ) ";


	for(int i=0; i< number; i++){
		int counter_dist=0;
		for(int j=0; j< buff[i].number; j++){
			total.str("");
			ostringstream query_string;
			query_string << " VALUES ( ";
			query_string <<  id_glob << " , " ;
			query_string <<"'"<< buff[i].variable_name <<"'"<< " , " ;
			query_string <<"'"<< buff[i].default_name <<"'"<< " , " ;
			query_string <<  counter_dist << " , " ;
			query_string <<  buff[i].distributions[j].discret_point.abs_discrete_value << " , " ;
			query_string <<  buff[i].distributions[j].discret_point.probability << " , " ;
			query_string <<"'"<< Fpl_Distribution_Class_Mean::convert_meandist_type2txt(buff[i].distributions[j].mean_type) <<"'"<< " , " ;
			query_string << buff[i].distributions[j].mean << " , " ;
			query_string << buff[i].distributions[j].second_moment<< " , " ;
			query_string << buff[i].distributions[j].constant <<" , ";
			query_string <<"'"<< buff[i].description <<"'"<< " ) " ;
			
			total <<query_string_fix.str() <<  query_string.str();

			Data_Base::database_request(&model, total.str(), ptr_database);
			id_glob++;
			counter_dist++;
		}
		delete []buff[i].distributions;
	}

}
//Close and delete the database table for the parameters of the discrete2mean distribution types the user-value-tables as well as the default table (static)
void Fpl_Distribution_Class_Discret_Mean::close_table(void){
	if(Fpl_Distribution_Class_Discret_Mean::table!=NULL){
		delete Fpl_Distribution_Class_Discret_Mean::table;
		Fpl_Distribution_Class_Discret_Mean::table=NULL;
	}
	if(Fpl_Distribution_Class_Discret_Mean::default_table!=NULL){
		delete Fpl_Distribution_Class_Discret_Mean::default_table;
		Fpl_Distribution_Class_Discret_Mean::default_table=NULL;
	}
}
//Copy the parameters of the distribution type in the database table (static)
void Fpl_Distribution_Class_Discret_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id){
	try{
		Fpl_Distribution_Class_Discret_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Distribution_Class_Discret_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::glob_id) <<"  ";
	test_filter << " from " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::variable_id) << " = " << old_rand_id;
	
	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Discret_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Distribution_Class_Discret_Mean::table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , " << new_rand_id<<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value_id)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_probability)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_type)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_mean)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_second_moment)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::constant)  <<"  ";

		
		test_filter << " FROM " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Discret_Mean::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}

}
//Set the parameter via console
void Fpl_Distribution_Class_Discret_Mean::set_input(void){
	double sum_percent=100.0;//for counting the sum to 100 for the discret distribution
	bool flag=false;
	_fpl_discret2mean_data *buffer=NULL;
	//read input

	//number distributions
	cout << "You chose a combination discrete type distribution to mean type distribution(s)!" << endl;
	do{
		cout << "Give numbers of discret values (number of mean type distributions)!" << endl;
		//if((flag=(cin  >> this->number_distribution))==false){//check if the input is valid
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
		//else if(this->number_distribution <= 0 ){
		//	cout << "No negative or zero value possible!" << endl;
		//}

	}
	while(flag==false || this->number_distribution <=0);

	try{
		buffer=new _fpl_discret2mean_data[this->number_distribution];
	}catch (bad_alloc &t){
		Error msg=set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}



	for (int i=0; i<this->number_distribution; i++){
		if(i==this->number_distribution-1){
			cout << "It is the last discrete value and mean type distribution" << endl;
		}
		//discrete value
		do{
			cout << "Give in the discrete value with number " << i+1 << endl;
			//if((flag=(cin  >> buffer[i].discret_point.abs_discrete_value).fail())==true){//check if the input is valid
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}
		while(flag==false);

		//probability
		do{
			cout << "Still " << sum_percent << label::percentage << " left !" << endl;
			cout << "Give in the probability to the discrete value " << buffer[i].discret_point.abs_discrete_value << " in [%] with the number " << i+1 << endl;
			//if((flag=(cin  >> buffer[i].discret_point.probability))==false){//check if the input is valid
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
			//else if(buffer[i].discret_point.probability > sum_percent ||  buffer[i].discret_point.probability <= 0.0){
			//	cout << "This percentage is not possible! Maximum left " << sum_percent << endl;
			//}
		}
		while(flag==false || buffer[i].discret_point.probability > sum_percent ||  buffer[i].discret_point.probability <= 0.0);

		//distribution mean value
		do{
			cout << "Give the mean value of mean type distribution " << i+1 << endl;
			//if((flag=(cin  >> buffer[i].mean))==false){//check if the input is valid
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}
		while(flag==false);
		
		//mean type distribution type
		_fpl_mean_distribution_type buffer_type=_fpl_mean_distribution_type::unknown_mean_type;
		do{ 
			cout << "Give the distribution type of mean type distribution " << i+1 << endl;
			cout << "(0) deterministic, (1) uniform, (2) normal, (3) expo, (4) lognormal, (5) gumbel, (6) weibull" << endl;
			int buffer_int_type=0;
			//if((flag=(cin  >> buffer_int_type))==false){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
			//else if((buffer_type=Fpl_Distribution_Class_Mean::convert_int2meandist_type(buffer_int_type))==_fpl_mean_distribution_type::unknown_mean_type){
			//	cout << "Unknwon mean-distribution type! Do it again! " << endl;
			//}
			//else{
			//	buffer[i].mean_type=buffer_type;
			//}
		}
		while(flag==false || buffer_type==_fpl_mean_distribution_type::unknown_mean_type);
	
		//standard deviation
		do{
			cout << "Give the standard deviation of mean type distribution "<< i+1 << endl;
			//if((flag=(cin  >> buffer[i].second_moment))==false){//check if the input is valif
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
			//else if(buffer[i].second_moment < 0.0){
			//	cout << "No negative standard deviation is possible! Do it again! " << endl;
			//}
		}
		while(flag==false || buffer[i].second_moment <0.0);

		sum_percent=sum_percent-buffer[i].discret_point.probability;

		buffer[i].constant=0.0;
		buffer[i].discret_point.calc_prob=0.0;
		buffer[i].discret_point.rel_discrete_value=0.0;
	}


	//set them
	this->set_input(this->number_distribution, buffer);


	//delete the data buffer
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}	
}
//Set the parameter with given parameter
void Fpl_Distribution_Class_Discret_Mean::set_input(const int number_dist, _fpl_discret2mean_data *data){
	this->number_distribution=number_dist;
	//allocate the distributions
	try{
		this->allocate_discrete_distribution();
		this->allocate_mean_type_distributions();
	}
	catch(Error msg){
		throw msg;
	}

	//set input of the first discrete type distribution
	this->distribution->set_input(this->number_distribution, data, 0.0, this->boundaries);
	//set input of the second mean type distribution(s)
	for(int i=0; i< this->number_distribution; i++){
		this->distribution_array[i].set_input(data[i].mean, data[i].second_moment, data[i].constant,false, this->boundaries, data[i].mean_type);
	}
}
//Set the parameter via database table for a user defined variable
void Fpl_Distribution_Class_Discret_Mean::set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;

	try{
		Fpl_Distribution_Class_Discret_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::variable_id) << " = " << position;
	test_filter << " order by " << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value_id);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->number_distribution= results.rowCount();

	if(this->number_distribution==0){
		Error msg=this->set_error(1);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		this->allocate_discrete_distribution();
		this->allocate_mean_type_distributions();
	}
	catch(Error msg){
		throw msg;
	}

	_fpl_discret2mean_data *buffer;
	buffer=new _fpl_discret2mean_data[this->number_distribution];

	try{
		//input
		for(int i=0; i< this->number_distribution; i++){
			buffer[i].mean=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_mean)).c_str()).toDouble();
			buffer[i].second_moment=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
			buffer[i].constant=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::constant)).c_str()).toDouble();
			buffer[i].mean_type=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_type)).c_str()).toString().toStdString());

			this->distribution_array[i].set_input(buffer[i].mean, buffer[i].second_moment, buffer[i].constant, false, this->boundaries, buffer[i].mean_type);
			this->distribution_array[i].check_input(calculated_mean);

			buffer[i].discret_point.probability=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_probability)).c_str()).toDouble();
			if(calculated_mean==false){
				buffer[i].discret_point.abs_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			}
			else{
				buffer[i].discret_point.rel_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			}
		}
		this->distribution->set_input(this->number_distribution, buffer, calculated_mean, this->boundaries);
		this->distribution->check_input(calculated_mean);

	}
	catch(Error msg){
		delete []buffer;
		throw msg;
	}
	delete []buffer;


}
//Set the parameter via database table for a default variable
void Fpl_Distribution_Class_Discret_Mean::set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	try{
		Fpl_Distribution_Class_Discret_Mean::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Discret_Mean::default_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::variable_name) << " = '" << name <<"'";
	test_filter << " and ";
	test_filter << Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::default_name) << " = '" << default_name<<"'";

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret_Mean::default_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->number_distribution= results.rowCount();

	if(this->number_distribution==0){
		Error msg=this->set_error(6);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Discret_Mean::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	

	try{
		this->allocate_discrete_distribution();
		this->allocate_mean_type_distributions();
	}
	catch(Error msg){
		throw msg;
	}

	_fpl_discret2mean_data *buffer;
	buffer=new _fpl_discret2mean_data[this->number_distribution];

	try{
		//input
		for(int i=0; i< this->number_distribution; i++){
			buffer[i].mean=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_mean)).c_str()).toDouble();
			buffer[i].second_moment=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_second_moment)).c_str()).toDouble();
			buffer[i].constant=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::constant)).c_str()).toDouble();
			buffer[i].mean_type=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(results.record(0).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::mean_type)).c_str()).toString().toStdString());


			this->distribution_array[i].set_input(buffer[i].mean, buffer[i].second_moment, buffer[i].constant, false, this->boundaries, buffer[i].mean_type);
			this->distribution_array[i].check_input(calculated_mean);

			buffer[i].discret_point.probability=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_probability)).c_str()).toDouble();
			if(calculated_mean==false){
				buffer[i].discret_point.abs_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			}
			else{
				buffer[i].discret_point.rel_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret_Mean::default_table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			}
		}
		this->distribution->set_input(this->number_distribution, buffer, calculated_mean, this->boundaries);
		this->distribution->check_input(calculated_mean);

	}
	catch(Error msg){
		delete []buffer;
		throw msg;
	}
	delete []buffer;
}
//Set the parameter via file for a user defined variable
void Fpl_Distribution_Class_Discret_Mean::set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;

	//read in
	string myline;
	QString qmyline;
	int must_found_counter=0;
	int must_found=0;

	//search for the keywords
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		if(myline==fpl_label::end_dist_type){
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
	while(ifile->atEnd()==false && this->number_distribution==0);

	must_found=1;
	if(must_found_counter!=must_found){
		Error msg=this->set_error(7);
		throw msg;
	}

	try{
		this->allocate_discrete_distribution();
		this->allocate_mean_type_distributions();
	}
	catch(Error msg){
		throw msg;
	}

	//inpu the data
	_fpl_discret2mean_data *buffer;
	buffer=new _fpl_discret2mean_data[this->number_distribution];

	try{
		int counter=0;
		do{
			qmyline=ifile->readLine();
			myline=qmyline.toStdString();
			(*line_counter)++;
			functions::clean_string(&myline);
			if(myline==fpl_label::end_dist_type){
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
				stringstream point_buff;
				point_buff << myline;
				string typ_buff;

				if(functions::count_number_columns(myline)==6){
					if(calculated_mean==false){
						point_buff >> buffer[counter].discret_point.probability >> buffer[counter].discret_point.abs_discrete_value 
							>> buffer[counter].mean >> typ_buff >> buffer[counter].second_moment >> buffer[counter].constant;
					}
					else{
						point_buff >> buffer[counter].discret_point.probability >> buffer[counter].discret_point.rel_discrete_value 
							>> buffer[counter].mean >> typ_buff >> buffer[counter].second_moment>> buffer[counter].constant;		
					}
				}
				else{
					//without cconstant
					if(calculated_mean==false){
						point_buff >> buffer[counter].discret_point.probability >> buffer[counter].discret_point.abs_discrete_value 
							>> buffer[counter].mean >> typ_buff >> buffer[counter].second_moment ;
						buffer[counter].constant=0.0;
					}
					else{
						point_buff >> buffer[counter].discret_point.probability >> buffer[counter].discret_point.rel_discrete_value 
							>> buffer[counter].mean >> typ_buff >> buffer[counter].second_moment;
						buffer[counter].constant=0.0;
					}

				}


				try{
					buffer[counter].mean_type=Fpl_Distribution_Class_Mean::convert_txt2meandist_type(typ_buff);
				}
				catch(Error msg){
					ostringstream info;
					info << "Read line : " << myline << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				if(point_buff.fail()==true){
					Error msg=this->set_error(11);
					ostringstream info;
					info << "Read line : " << myline << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				else{
					this->distribution_array[counter].set_input(buffer[counter].mean, buffer[counter].second_moment, buffer[counter].constant, false, this->boundaries, buffer[counter].mean_type);
					this->distribution_array[counter].check_input(calculated_mean);
					counter++;
				}
			}



		}
		while(ifile->atEnd()==false && counter<this->number_distribution);

		this->distribution->set_input(this->number_distribution, buffer, calculated_mean, this->boundaries);
		this->distribution->check_input(calculated_mean);

	}
	catch(Error msg){
		delete []buffer;
		throw msg;
	}


	delete []buffer;

}
//Transfer the data of the distribution type to database table
void Fpl_Distribution_Class_Discret_Mean::transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean){

	try{
		Fpl_Distribution_Class_Discret_Mean::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Distribution_Class_Discret_Mean::table->maximum_int_of_column(Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	//header
	query_string << "INSERT INTO  " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::variable_id) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value_id) <<" , ";
	
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_value) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::discret_probability) <<" , ";
	
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_type) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_mean) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::mean_second_moment) <<" , ";
	query_string << Fpl_Distribution_Class_Discret_Mean::table->get_column_name(fpl_label::constant) <<" ) ";
	
	query_string << " VALUES ";
	for(int i=0; i< this->number_distribution; i++){
		query_string <<" ( ";
		query_string << glob_id << " , " ;
		query_string << position << " , " ;
		query_string << i << " , " ;

		if(calculated_mean==false){
			query_string << this->distribution->discret_points[i].abs_discrete_value << " , " ;
		}
		else{
			query_string << this->distribution->discret_points[i].rel_discrete_value << " , " ;
		}
		query_string << this->distribution->discret_points[i].probability << " , " ;

		query_string <<"'"<< this->distribution_array[i].get_mean_distribution_type() << "' , " ;
		query_string << this->distribution_array[i].get_mean_value() << " , " ;
		query_string << this->distribution_array[i].get_std_dev() << " , " ;
		query_string << this->distribution_array[i].get_constant() ;
		query_string <<" ) ";
		if(i<this->number_distribution-1){
			query_string << " , ";
		}
		glob_id++;
	}

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Fpl_Distribution_Class_Discret_Mean::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}
}
//Output the member of the distribution type to console/display
void Fpl_Distribution_Class_Discret_Mean::output_member(bool detailed){
	ostringstream cout;
	cout << " Leading (first) discrete distribution:" << endl;	
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	if(this->distribution!=NULL){
		this->distribution->output_member(detailed);
	}
	else{
		cout << " No leading distribution is set" << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	}
	cout << " Subordinated mean type distribution(s): " << this->number_distribution<< endl;	
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
	for(int i=0; i< this->number_distribution; i++){		
		cout << " Distribution number : " << i << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
		this->distribution_array[i].output_member(detailed);
	}
}
//Generate for the distribution a deterministic value (weighted middle of the mean values + constant)
_fpl_result Fpl_Distribution_Class_Discret_Mean::make_deterministic(void){

	//make the mean value
	double max_prob=0.0;

	for(int i=0;i< this->number_distribution ;i++){
		//use the mid value
		this->random_result.inter_result=this->random_result.inter_result+(this->distribution->discret_points[i].probability)/100.00*(this->distribution->discret_points[i].abs_discrete_value);
		this->random_result.end_result=this->random_result.end_result+(this->distribution->discret_points[i].probability)/100.00*(this->distribution_array[i].mean+this->distribution_array[i].constant);
		if(max_prob<this->distribution->discret_points[i].probability){
			this->random_result.inter_result=this->distribution->discret_points[i].abs_discrete_value;
			this->random_result.end_result=this->distribution_array[i].mean+this->distribution_array[i].constant;
			max_prob=this->distribution->discret_points[i].probability;
		}
	
	}
	return this->random_result;
};
//Generate for the distribution a random distributed value
_fpl_result Fpl_Distribution_Class_Discret_Mean::make_random(void){
	//first random generation of the discrete distribution
	this->random_result.inter_result=this->distribution->make_random().end_result;
	//decide which mean type distribution should be taken with an epsilon interval
	for(int i=0; i<this->number_distribution; i++){
		if(abs(this->random_result.inter_result-this->distribution->discret_points[i].abs_discrete_value)<1.0e-8){	
			//generate the random variable with the choosen mean type distribution	
			this->hit_number=i;
			this->random_result.end_result=this->distribution_array[i].make_random().end_result;
		}
	}
	return this->random_result;
}
//Statistic of the generation process: mean, standard deviation and minimum and maximum values are calculated for multiple random generations of the variable
void Fpl_Distribution_Class_Discret_Mean::check_statistic(void){
	//check the statistic of the distribution
	this->distribution->check_statistic();
	this->distribution_array[this->hit_number].check_statistic();
	//check of the combination
	_Fpl_Distribution_Class::check_statistic();
}
//Statistic of the generation process: output it to console/display
void Fpl_Distribution_Class_Discret_Mean::output_statistic(const bool calc_flag, bool detailed){ 	
	ostringstream cout;
	cout << " Numbers of generation   : " << this->counter_calc << endl;
	cout << " Mean of combination     : " << this->mean_calc << endl;
	cout << " Standard of combination : " << this->std_dev_calc << endl;
	cout << " Statistics of the first (discrete) distribution : " << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);

	this->distribution->output_statistic(calc_flag);
	cout << " Statistics of the second (mean) distribution(s) : " << this->number_distribution<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);

	for(int i=0; i<this->number_distribution; i++){
		cout << " Distribution number :" << i << endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
		distribution_array[i].output_statistic(calc_flag);					
	}
}
//Reset the values for checking the statistics
 void Fpl_Distribution_Class_Discret_Mean::reset_statistic(void){
	//reset this value for the calculation of the statistics
	_Fpl_Distribution_Class::reset_statistic();
	this->distribution->reset_statistic();
	for(int i=0; i<this->number_distribution; i++){
		this->distribution_array[i].reset_statistic();
	}
 }
//Set a new mean value of the distribution (dynamic mean-setting of the first distribution
void Fpl_Distribution_Class_Discret_Mean::set_new_middle(const double new_middle){
	this->distribution->set_new_middle(new_middle);

	for (int i=0; i<this->number_distribution; i++){
		this->distribution_array[i].set_new_middle(this->distribution->discret_points[i].abs_discrete_value);
	}
}
//Get the index of the result (relevant by discret and discret2mean distribution type)
int Fpl_Distribution_Class_Discret_Mean::get_result_index(void){
	return this->hit_number;
}
//Get the number of points (relevant by discret and discret2mean distribution type)
int Fpl_Distribution_Class_Discret_Mean::get_number_point(void){
	return this->number_distribution;
}
//Get the value per given point index
double Fpl_Distribution_Class_Discret_Mean::get_point_value(const int index){
	if(index>=0 && index <this->distribution->get_number_point()){
		return this->distribution->discret_points[index].abs_discrete_value;
	}
	else{
		return 0.0;
	}
}
//____________
//private
//Allocate the leading discrete distribution
void Fpl_Distribution_Class_Discret_Mean::allocate_discrete_distribution(void){
	this->delete_discrete_distribution();
	try{
		this->distribution=new Fpl_Distribution_Class_Discret;
	}
	catch (bad_alloc &t){
		Error msg=set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the leading discrete distribution
void Fpl_Distribution_Class_Discret_Mean::delete_discrete_distribution(void){
	if(this->distribution!=NULL){
		delete this->distribution;
		this->distribution=NULL;
	}
}
//Allocate the second mean type distribution(s)
void Fpl_Distribution_Class_Discret_Mean::allocate_mean_type_distributions(void){
	this->delete_mean_type_distributions();
	try{
		this->distribution_array=new Fpl_Distribution_Class_Mean[this->number_distribution];
	}
	catch (bad_alloc &t){
		Error msg=set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the second mean type distribution(s)
void Fpl_Distribution_Class_Discret_Mean::delete_mean_type_distributions(void){
	if(this->distribution_array!=NULL){
		delete []this->distribution_array;
		this->distribution_array=NULL;
	}
}
//Find the key-values for the file input of the distribution type
void Fpl_Distribution_Class_Discret_Mean::find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_no_discret);
	if(pos>=0 && wrong_input==false && calculated_flag==false){
		buffer=myline.substr(fpl_label::key_no_discret.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_distribution;
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
		Error msg=this->set_error(8);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Distribution_Class_Discret_Mean::set_error(const int err_type){
		string place="Fpl_Distribution_Class_Discret_Mean::";
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
		case 2://bad_alloc
			place.append("allocate_discrete_distribution(void)") ;
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://bad_alloc
			place.append("allocate_mean_type_distributions(void)") ;
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://bad_alloc
			place.append("set_input(void)") ;
			reason="Can not allocate the memory for the data buffer";
			help="Check the memory";
			type=10;
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
		case 7://do not find all required distribution type infos
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find all required discret2mean type distribution information";
			help="Check if all discret2mean type distribution information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_no_discret<< endl;
			type=1;
			break;
		case 8://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the distribution type information of the discret2mean-type in file";
			type=1;
			break;
		case 9://do not find the end of the distribution type information in file
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find the end of the distribution type information in file";
			help="The the distribution type information has to end with !$END_DIST_TYPE";
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
Warning Fpl_Distribution_Class_Discret_Mean::set_warning(const int warn_type){
		string place="Fpl_Distribution_Class_Discret_Mean::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no result found
			place.append("set_input(const int position)") ;
			reason="No results found";
			reaction="All is set to 0";
			help= "Check the variable";
			type=2;	
			break;
		case 1://input datas can not submitted
			place.append("transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)");
			reason="Can not submit the distribution type data (discret2mean-type) to the database";
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
