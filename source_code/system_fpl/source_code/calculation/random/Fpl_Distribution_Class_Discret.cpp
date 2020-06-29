//#include "Fpl_Distribution_Class_Discret.h"
#include "Fpl_Headers_Precompiled.h"


//init static values
Tables *Fpl_Distribution_Class_Discret::table=NULL;
Tables *Fpl_Distribution_Class_Discret::default_table=NULL;

//Default constructor
Fpl_Distribution_Class_Discret::Fpl_Distribution_Class_Discret(void){
	this->discret_points=NULL;
	this->absolut_middle=0.0;
	this->number_points=0;
	this->result_index=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Discret), _sys_system_modules::FPL_SYS);//count the memory
}
///Constructor with all parameters given
Fpl_Distribution_Class_Discret::Fpl_Distribution_Class_Discret(const int number_points, _fpl_discrete_dist_point *points, const bool calculated_middle, const _fpl_boundaries boundaries){
	this->number_points=number_points;
	this->discret_points=points;
	this->absolut_middle=0.0;
	this->boundaries=boundaries;
	//alloc the points
	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].abs_discrete_value=points[i].abs_discrete_value;
		this->discret_points[i].calc_prob=points[i].calc_prob;
		this->discret_points[i].probability=points[i].probability;
		this->discret_points[i].rel_discrete_value=points[i].rel_discrete_value;
	}

	try{
		this->check_input(calculated_middle);

	}
	catch(Error msg){
		throw msg;
	}

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Distribution_Class_Discret), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Distribution_Class_Discret::~Fpl_Distribution_Class_Discret(void){
	this->delete_discrete_points();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Distribution_Class_Discret), _sys_system_modules::FPL_SYS);//count the memory
}
//____________
//public
//Set the database table for the parameters of the discrete distribution types (user-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Discret::set_table( QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret::table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_discret_user;
		string tab_col[5];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_id;
		tab_col[2]=fpl_label::discret_value_id;
		tab_col[3]=fpl_label::discret_value;
		tab_col[4]=fpl_label::discret_probability;

		//set new table
		try{
			Fpl_Distribution_Class_Discret::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Fpl_Distribution_Class_Discret::table->set_name( ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Discret::set_table( QSqlDatabase database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Discret::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the discrete distribution types (user-values) (static)
void Fpl_Distribution_Class_Discret::create_table(QSqlDatabase *ptr_database){
		if(Fpl_Distribution_Class_Discret::table==NULL){
			ostringstream cout;
			cout << "Create random discrete type distributed variable database table (user defined)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_discret_user;
			const int num_col=5;
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


			try{
				Fpl_Distribution_Class_Discret::table= new Tables();
				if(Fpl_Distribution_Class_Discret::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Discret::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Discret::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Discret::close_table();
	}
}
//Delete all data in the database table for the parameters of the discrete distribution types (user-values) (static)
void Fpl_Distribution_Class_Discret::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Discret::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Discret::table->delete_data_in_table(ptr_database);
}
//Delete data in the database table for the parameters of the discrete distribution types (user-values) for a given random variable id (static)
void Fpl_Distribution_Class_Discret::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id){
	try{
		Fpl_Distribution_Class_Discret::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel results;
	ostringstream test_filter;
	test_filter<< "Delete ";
	test_filter << " from " << Fpl_Distribution_Class_Discret::table->get_table_name();
	test_filter << " where ";
	test_filter << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::variable_id) << " = " << rand_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Discret::delete_data_in_table(QSqlDatabase *ptr_database, const int rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the database table for the parameters of the discrete distribution types (default-values): it sets the table name and the name of the columns and allocate them (static)
void Fpl_Distribution_Class_Discret::set_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret::default_table==NULL){
		//make specific input for this class
		const string tab_id_name=fpl_label::tab_rand_discret_default;
		string tab_col[7];
		tab_col[0]=fpl_label::glob_id;
		tab_col[1]=fpl_label::variable_name;
		tab_col[2]=fpl_label::default_name;
		tab_col[3]=fpl_label::discret_value_id;
		tab_col[4]=fpl_label::discret_value;
		tab_col[5]=fpl_label::discret_probability;
		tab_col[6]=label::description;

		//set new table
		try{
			default_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			default_table->set_name(ptr_database, _sys_table_type::fpl);
		}
		catch(bad_alloc&t ){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Mean::set_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Fpl_Distribution_Class_Discret::close_table();
			throw msg;
		}
	}
}
//Create the database table for the parameters of the discrete distribution types (default-values) (static)
bool Fpl_Distribution_Class_Discret::create_default_table(QSqlDatabase *ptr_database){
	if(Fpl_Distribution_Class_Discret::default_table==NULL){
			ostringstream cout;
			cout << "Create random discrete type distributed variable database table (default)..." << endl ;
			Sys_Common_Output::output_fpl->output_txt(&cout);
			//make specific input for this class
			const string tab_name=fpl_label::tab_rand_discret_default;
			const int num_col=7;
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

			tab_col[6].name=label::description;
			tab_col[6].type=sys_label::tab_col_type_string;

			try{
				Fpl_Distribution_Class_Discret::default_table= new Tables();
				if(Fpl_Distribution_Class_Discret::default_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::fpl)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_fpl->output_txt(&cout);
					Fpl_Distribution_Class_Discret::close_table();
					return false;
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Fpl_Distribution_Class_Discret::create_default_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Fpl_Distribution_Class_Discret::close_table();
				throw msg;
			}

		Fpl_Distribution_Class_Discret::close_table();
		return true;
	}
	return false;
}
//Delete all data in the database table for the parameters of the discrete distribution types (default-values) (static)
void Fpl_Distribution_Class_Discret::delete_data_in_default_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Distribution_Class_Discret::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in table
	Fpl_Distribution_Class_Discret::default_table->delete_data_in_table(ptr_database);
	//restore predefined default variables
	Fpl_Distribution_Class_Discret::write_default_variables2table(ptr_database);
}
//Prepare and write the predefined default variables to the database table (static)
void Fpl_Distribution_Class_Discret::write_default_variables2table(QSqlDatabase *ptr_database){
	const int number=2;
	_fpl_default_discrete buff[number];

	buff[0].variable_name=fpl_label::water_density;
	buff[0].default_name=fpl_label::standard;
	buff[0].description="Density of normal water";
	buff[0].number=3;
	buff[0].points=new _fpl_discrete_dist_point[buff[0].number];
	buff[0].points[0].abs_discrete_value=995.0;
	buff[0].points[0].probability=10.0;
	buff[0].points[1].abs_discrete_value=1000.0;
	buff[0].points[1].probability=80.0;
	buff[0].points[2].abs_discrete_value=1009.0;
	buff[0].points[2].probability=10.0;

	buff[1].variable_name=fpl_label::water_density;
	buff[1].default_name="salty";
	buff[1].description="Density of salty water, e.g. sea water";
	buff[1].number=2;
	buff[1].points=new _fpl_discrete_dist_point[buff[0].number];
	buff[1].points[0].abs_discrete_value=1006.0;
	buff[1].points[0].probability=90.0;
	buff[1].points[1].abs_discrete_value=1010.0;
	buff[1].points[1].probability=10.0;	
	//...add more values; think to change the number

	try{
		Fpl_Distribution_Class_Discret::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//set them to database table
	//mysql query with the query-class
	QSqlQuery model(*ptr_database);

	//evaluate the global identifier
	int id_glob=Fpl_Distribution_Class_Discret::default_table->maximum_int_of_column(Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::glob_id),ptr_database)+1;
	ostringstream total;

	//set the query via a query string
	ostringstream query_string_fix;
	query_string_fix << "INSERT INTO  " << Fpl_Distribution_Class_Discret::default_table->get_table_name();
	query_string_fix <<" ( ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::variable_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::default_name) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_value_id) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_value) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_probability) <<" , ";
	query_string_fix << Fpl_Distribution_Class_Discret::default_table->get_column_name(label::description) <<" ) ";


	for(int i=0; i< number; i++){
		int value_counter=0;
		for(int j=0; j<buff[i].number; j++){
			total.str("");
			ostringstream query_string;
			query_string << " VALUES ( ";
			query_string <<  id_glob << " , " ;
			query_string <<"'"<< buff[i].variable_name <<"'"<< " , " ;
			query_string <<"'"<< buff[i].default_name <<"'"<< " , " ;
			query_string << value_counter << " , " ;
			query_string << buff[i].points[j].abs_discrete_value << " , " ;
			query_string << buff[i].points[j].probability << " , " ;
			query_string <<"'"<< buff[i].description <<"'"<< " ) " ;
			
		
			total <<query_string_fix.str() <<  query_string.str();

			Data_Base::database_request(&model, total.str(), ptr_database);
			id_glob++;
			value_counter++;
		}
		delete []buff[i].points;
	}

}
///Close and delete the database table for the parameters of the discrete distribution types the user-value-tables as well as the default table
void Fpl_Distribution_Class_Discret::close_table(void){
	if(Fpl_Distribution_Class_Discret::table!=NULL){
		delete Fpl_Distribution_Class_Discret::table;
		Fpl_Distribution_Class_Discret::table=NULL;
	}
	if(Fpl_Distribution_Class_Discret::default_table!=NULL){
		delete Fpl_Distribution_Class_Discret::default_table;
		Fpl_Distribution_Class_Discret::default_table=NULL;
	}
}
//Copy the parameters of the distribution type in the database table (static)
void Fpl_Distribution_Class_Discret::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id){
	try{
		Fpl_Distribution_Class_Discret::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=0;
	glob_id=Fpl_Distribution_Class_Discret::table->maximum_int_of_column(Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::glob_id) ,ptr_database)+1;

	QSqlQueryModel model;
	ostringstream test_filter;
	test_filter<< "SELECT ";
	test_filter << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::glob_id) <<"  ";
	test_filter << " from " << Fpl_Distribution_Class_Discret::table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::variable_id) << " = " << old_rand_id;
	
	//submit it to the datbase
	Data_Base::database_request(&model, test_filter.str(), ptr_database);
	if(model.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Fpl_Distribution_Class_Discret::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " <<Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
		info << "Table error info: " << model.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	

	QSqlQueryModel model1;
	for(int i=0; i< model.rowCount(); i++){
		test_filter.str("");
		test_filter << "INSERT INTO "<< Fpl_Distribution_Class_Discret::table->get_table_name();
		test_filter << " SELECT " << glob_id <<" , " << new_rand_id<<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value_id)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value)  <<" , ";
		test_filter <<  Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_probability)  <<"  ";
		test_filter << " FROM " << Fpl_Distribution_Class_Discret::table->get_table_name() <<" ";
		test_filter << " WHERE " << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::glob_id) << " = ";
		test_filter << model.record(i).value(Fpl_Random_Variables::table->get_column_name(fpl_label::glob_id).c_str()).toInt();
		Data_Base::database_request(&model1, test_filter.str(), ptr_database);
		if(model1.lastError().isValid()==true){
			Error msg;
			msg.set_msg("Fpl_Distribution_Class_Discret::copy_parameters(QSqlDatabase *ptr_database, const int old_rand_id, const int new_rand_id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
			info << "Table error info: " << model1.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		glob_id++;
	}

}
//Set the parameter via console
void Fpl_Distribution_Class_Discret::set_input(void){
	double sum_percent=100.0;//for counting the sum to 100 for the discret distribution
	bool flag=false;
	//read input
	cout << "You chose a discret distribution!" << endl;
	//numbers of discret values
	do{
		cout << "Give numbers of discret values!" << endl;
		//if((flag=(cin  >> this->number_points))==false){//check if the input is valif
		//	cout << "Wrong input! Do it again" << endl;
		//	cin.clear();//clear the error flags
		//	cin.ignore();//clear cin
		//}
		//else if(this->number_points <= 0 ){
		//	cout << "No negative or zero value allowed! Do it again" << endl;
		//}

	}
	while( flag==false || this->number_points <= 0);

	//allocate the points
	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}

	//fill in the values
	for (int i=0; i<this->number_points; i++){
		if(i==this->number_points-1){
			cout << "It is the last discret value" << endl;
		}
		//discrete value
		do{
			cout << "Give in the discrete value with number " << i+1 << endl;
			//if((flag=(cin  >> this->discret_points[i].abs_discrete_value))==false){
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
		}
		while(flag==false);
		//prbability
		do{
			cout << "Still " << sum_percent << label::percentage << " left !" << endl;
			cout << "Give in the probability to the discrete value " << this->discret_points[i].abs_discrete_value << " in [%] with the number " << i+1 << endl;
			//if((flag=(cin  >> this->discret_points[i].probability))==false){
			//	cout << "Wrong input! Do it again" << endl;
			//	cin.clear();//clear the error flags
			//	cin.ignore();//clear cin
			//}
			//else if(this->discret_points[i].probability > sum_percent ||  this->discret_points[i].probability <= 0.0){
			//	cout << "This percentage is not possible! Maximum left " << sum_percent << endl;
			//}
		}
		while(flag==false || this->discret_points[i].probability > sum_percent ||  this->discret_points[i].probability <= 0.0);
		
		//sum up
		sum_percent=sum_percent-this->discret_points[i].probability;
	}

	this->check_input(false);
}
//Set the input values per given parameters
void Fpl_Distribution_Class_Discret::set_input(const int number_points, _fpl_discrete_dist_point *points, const bool calculated_middle, const _fpl_boundaries boundaries){
	this->number_points=number_points;
	this->absolut_middle=0.0;
	this->boundaries=boundaries;
	//alloc the points
	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].abs_discrete_value=points[i].abs_discrete_value;
		this->discret_points[i].calc_prob=points[i].calc_prob;
		this->discret_points[i].probability=points[i].probability;
		this->discret_points[i].rel_discrete_value=points[i].rel_discrete_value;
	}
	try{
		this->check_input(calculated_middle);

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the input values per given parameters
void Fpl_Distribution_Class_Discret::set_input(const int number_points, _fpl_discret2mean_data *points, const bool calculated_middle, const _fpl_boundaries boundaries){
	this->number_points=number_points;
	this->absolut_middle=0.0;
	this->boundaries=boundaries;
	//alloc the points
	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].abs_discrete_value=points[i].discret_point.abs_discrete_value;
		this->discret_points[i].calc_prob=points[i].discret_point.calc_prob;
		this->discret_points[i].probability=points[i].discret_point.probability;
		this->discret_points[i].rel_discrete_value=points[i].discret_point.rel_discrete_value;
	}
	try{
		this->check_input(calculated_middle);

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the parameter via database table for a user defined variable
void Fpl_Distribution_Class_Discret::set_input(const int position, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	try{
		Fpl_Distribution_Class_Discret::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Discret::table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::variable_id) << " = " << position;
	test_filter << " order by " << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value_id);
	
	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->number_points= results.rowCount();

	if(this->number_points==0){
		Error msg=this->set_error(1);	
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
		info << "Variable id     : " << position << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}


	//input 
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].probability=results.record(i).value((Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_probability)).c_str()).toDouble();
		if(calculated_mean==false){
			this->discret_points[i].abs_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
		}
		else{
			this->discret_points[i].rel_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			this->relativ_to_absolut();
		}

	}

	try{
		this->check_input(calculated_mean);

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the parameter via database table for a default variable
void Fpl_Distribution_Class_Discret::set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries){
	this->boundaries=boundaries;
	QSqlQueryModel results;
	try{
		Fpl_Distribution_Class_Discret::set_default_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Fpl_Distribution_Class_Discret::default_table->get_table_name();
	test_filter << " where " ;
	test_filter << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::variable_name) << " = '" << name <<"'";
	test_filter << " and ";
	test_filter << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::default_name) << " = '" << default_name<<"'";
	test_filter << " order by " << Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_value_id);

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg=this->set_error(6);
		ostringstream info;
		info << "Table Name      : " << Fpl_Distribution_Class_Discret::default_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->number_points= results.rowCount();

	if(this->number_points==0){
		Error msg=this->set_error(7);	
		ostringstream info;
		info << "Table Name            : " << Fpl_Distribution_Class_Discret::default_table->get_table_name() << endl;
		info << "Variable name         : " << name << endl;
		info << "Variable default name : " << default_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}


	//input 
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].probability=results.record(i).value((Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_probability)).c_str()).toDouble();
		if(calculated_mean==false){
			this->discret_points[i].abs_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
		}
		else{
			this->discret_points[i].rel_discrete_value=results.record(i).value((Fpl_Distribution_Class_Discret::default_table->get_column_name(fpl_label::discret_value)).c_str()).toDouble();
			
		}
	}

	if(calculated_mean==true){
		this->relativ_to_absolut();
	}

	try{
		this->check_input(calculated_mean);

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the parameter via file for a user defined variable
void Fpl_Distribution_Class_Discret::set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries){
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
	while(ifile->atEnd()==false && this->number_points==0);


	must_found=1;
	if(must_found_counter!=must_found){
		Error msg=this->set_error(10);
		throw msg;
	}

	try{
		this->allocate_discrete_points();
	}
	catch(Error msg){
		throw msg;
	}

	
	//input points
	int counter_point=0;
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

			if(calculated_mean==false){
				point_buff >> this->discret_points[counter_point].probability >> this->discret_points[counter_point].abs_discrete_value;
			}
			else{
				point_buff >> this->discret_points[counter_point].probability >> this->discret_points[counter_point].rel_discrete_value;
			}
			if(point_buff.fail()==true){
				Error msg=this->set_error(11);
				ostringstream info;
				info << "Read line : " << myline << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			else{
				counter_point++;
			}
		}
	}
	while(ifile->atEnd()==false && counter_point<this->number_points);

	if(counter_point!=this->number_points){
		Error msg=this->set_error(12);
		ostringstream info;
		info << "Number found    : " << counter_point << endl;
		info << "Number required : " << this->number_points << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	if(calculated_mean==true){
		this->relativ_to_absolut();
	}

	try{
		this->check_input(calculated_mean);

	}
	catch(Error msg){
		throw msg;
	}


}
//Transfer the data of the distribution type to database table
void Fpl_Distribution_Class_Discret::transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean){

	try{
		Fpl_Distribution_Class_Discret::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Fpl_Distribution_Class_Discret::table->maximum_int_of_column(Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::glob_id), ptr_database)+1;

	//transfer the data
	ostringstream query_string;
	//header
	query_string << "INSERT INTO  " << Fpl_Distribution_Class_Discret::table->get_table_name();
	query_string <<" ( ";
	query_string << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::glob_id) <<" , ";
	query_string << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::variable_id) <<" , ";
	query_string << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value_id) <<" , ";

	query_string << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_value) <<" , ";
	query_string << Fpl_Distribution_Class_Discret::table->get_column_name(fpl_label::discret_probability) <<" ) ";
	query_string << " VALUES ";
	for(int i=0; i< this->number_points; i++){
		query_string <<" ( ";
		query_string << glob_id << " , " ;
		query_string << position << " , " ;
		query_string << i << " , " ;

		if(calculated_mean==false){
			query_string << this->discret_points[i].abs_discrete_value << " , " ;
		}
		else{
			query_string << this->discret_points[i].rel_discrete_value << " , " ;
		}
		query_string << this->discret_points[i].probability ;
		query_string <<" ) ";
		if(i<this->number_points-1){
			query_string << " , ";
		}
		glob_id++;
	}

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, query_string.str(), ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Name                : " << Fpl_Distribution_Class_Discret::table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);	
	}


}
//Output the member of the distribution type to console/display
void Fpl_Distribution_Class_Discret::output_member(bool detailed){
	ostringstream cout;	
	cout << " Number of values      : " << this->number_points << endl;
	cout << " Number " <<W(10)<< " Value " << W(13) << " Probability " << label::percentage << endl; 
	for(int i=0; i<this->number_points; i++){
		cout << "  " << i << W(12) << this->discret_points[i].abs_discrete_value << W(12) << this->discret_points[i].probability << endl ;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout,detailed);
}
//Generate for the distribution a deterministic value (mid_value+constant)
_fpl_result Fpl_Distribution_Class_Discret::make_deterministic(void){

	this->random_result.end_result=0.0;
	this->random_result.inter_result=0.0;

	double max_prob=0.0;

	for(int i=0;i< this->number_points ;i++){
		//use the mid value
		//this->random_result.end_result=this->random_result.end_result+(this->discret_points[i].probability/100.0)* this->discret_points[i].abs_discrete_value;
		//take the most probable one
		if(max_prob<this->discret_points[i].probability){
			this->random_result.end_result=this->discret_points[i].abs_discrete_value;
			max_prob=this->discret_points[i].probability;
		}

	}
	this->random_result.inter_result=this->random_result.end_result;
	return this->random_result;
};
//Generate for the distribution a random distributed value
_fpl_result Fpl_Distribution_Class_Discret::make_random(void){
	double random;
	double boundary_left=0.0;
	double boundary_right=this->discret_points[0].probability/100.00;
	this->random_result.end_result=0.0;
	this->random_result.inter_result=0.0;

	//generation of the uniformly-distributed numbers [0..1]
	random=this->ran1();

	//case random<=0 or random>=1: it is not possible for the log-function
	while(random<=0 || random >=1.0){
		random=this->ran1();
	}
	//find the value between the boundaries where the random number lies
	for(int i=0;i<this->number_points;i++){
		if(random>= boundary_left && random < boundary_right){
			//give the discret value as result
			this->random_result.end_result=this->discret_points[i].abs_discrete_value;
			this->result_index=i;
			break;
		}
		else{
			//set new boundaries for the next interval
			boundary_left=boundary_left+this->discret_points[i].probability/100.0;
			if(i!=this->number_points-1){
				boundary_right=boundary_right+this->discret_points[i+1].probability/100.0;
			}
			else{
				boundary_right=1.0;
			}
		}
	}
	this->random_result.inter_result=this->random_result.end_result;
	
	return this->random_result;
}
//Statistic of the generation process: mean, standard deviation and minimum and maximum values are calculated for multiple random generations of the variable
void Fpl_Distribution_Class_Discret::check_statistic(void){

	//counting the numbers of method calls
	this->counter_calc++;
	for(int i=0; i<this->number_points; i++){
		//search for the correspondig x-value with epsilon-interval
		if(abs(this->random_result.end_result-this->discret_points[i].abs_discrete_value)<1.0e-8){
			//calculate the probability of occurence +1
			this->discret_points[i].calc_prob=(this->discret_points[i].calc_prob*((double)this->counter_calc-1.0)+1.0)/((double)this->counter_calc);
		}
		else{//no occurence
			this->discret_points[i].calc_prob=(this->discret_points[i].calc_prob*((double)this->counter_calc-1.0))/((double)this->counter_calc);
		}
	}
	//init the min and max values
	if(this->counter_calc==1){
		this->max_calc=this->random_result.end_result;
		this->min_calc=this->random_result.end_result;
	}
	//find max
	if(this->random_result.end_result > this->max_calc){
		this->max_calc=this->random_result.end_result;
	}
	//find min
	if(this->random_result.end_result <this-> min_calc){
		this->min_calc=this->random_result.end_result;
	}
}
//Statistic of the generation process: output it to console/display
void Fpl_Distribution_Class_Discret::output_statistic(const bool calc_flag, bool detailed){
	ostringstream cout;
	cout << " Numbers of generation : " << this->counter_calc << endl;
	if(this->counter_calc=!0){
		cout << " Minimum               : " << this->min_calc << endl;
		cout << " Maximum               : " << this->max_calc << endl;
	}
	if(calc_flag==true){
		cout << " Dynamic mean settings : " << this->counter_set_mid << endl;
		if(this->counter_set_mid!=0){
			cout << " Mean of dynamic mean  : " << this->mid_calc/(double)this->counter_set_mid << " (" << this->mean_calc <<")"<< endl;
		}
		else{
			cout << " No dynamic mean is set"<<endl;
		}
	}
	cout << " Number " <<W(12)<< " Value " << W(12) << " Probability (calculated/given/difference)" << label::percentage << endl; 
	for(int i=0; i<this->number_points; i++){
		cout << "  " << i << W(12) << this->discret_points[i].abs_discrete_value;
		cout << W(12) << this->discret_points[i].calc_prob*100.0 <<" / " << this->discret_points[i].probability << " / ";
		cout << this->discret_points[i].probability-this->discret_points[i].calc_prob*100.0 << endl;
	}
	Sys_Common_Output::output_fpl->output_txt(&cout, detailed);
}
//Reset the values for checking the statistics
void Fpl_Distribution_Class_Discret::reset_statistic(void){
	_Fpl_Distribution_Class::reset_statistic();
	for(int i=0; i<this->number_points; i++){
		this->discret_points[i].calc_prob=0.0;	
	}
}
//Set a new mid value of the distribution (dynamic mid-setting)
void Fpl_Distribution_Class_Discret::set_new_middle(const double new_middle){
	this->absolut_middle=new_middle;
	//sumup the new middle
	this->mid_calc=this->mid_calc+this->absolut_middle;
	this->counter_set_mid++;

	this->relativ_to_absolut();
}
//Get calculated exact probability for the distribution for the random generator check
double Fpl_Distribution_Class_Discret::get_calulculated_exact_prob(const double value, const double stepsize){
	double res=0.0;

	for(int i=0; i<this->number_points; i++){
		if(discret_points[i].abs_discrete_value>value-0.5*stepsize && discret_points[i].abs_discrete_value<=value+0.5*stepsize){
			res=discret_points[i].probability/100.0;
			break;
		}
	}

	return res;
}
//Get the index of the result (relevant by discret and discret2mean distribution type)
int Fpl_Distribution_Class_Discret::get_result_index(void){
	return this->result_index;
}
//Get the number of points (relevant by discret and discret2mean distribution type)
int Fpl_Distribution_Class_Discret::get_number_point(void){
	return this->number_points;
}
//Get the value per given point index
double Fpl_Distribution_Class_Discret::get_point_value(const int index){
	if(index>=0 && index <this->number_points){
		return this->discret_points[index].abs_discrete_value;
	}
	else{
		return 0.0;
	}
}
//___________
//private
//Allocate the discrete points
void Fpl_Distribution_Class_Discret::allocate_discrete_points(void){
	this->delete_discrete_points();
	try{
		this->discret_points=new _fpl_discrete_dist_point[this->number_points];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_discrete_dist_point)*this->number_points, _sys_system_modules::FPL_SYS);
	}
	catch (bad_alloc &t){
		Error msg=set_error(5);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}

	//init values
	for(int i=0; i< this->number_points; i++){
		this->discret_points[i].abs_discrete_value=0.0;
		this->discret_points[i].calc_prob=0.0;
		this->discret_points[i].probability=0.0;
		this->discret_points[i].rel_discrete_value=0.0;
	}
}
//Delete discrete points
void Fpl_Distribution_Class_Discret::delete_discrete_points(void){
	if(this->discret_points!=NULL){
		delete []this->discret_points;
		this->discret_points=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_discrete_dist_point)*this->number_points, _sys_system_modules::FPL_SYS);
	}
}
//Check the parameter of the distribution type
void Fpl_Distribution_Class_Discret::check_input(const bool calculate_middle){
	double check_sum=0.0;

	//check if there are probabilties untill 100 % and probability is between 0 and 100%
	for (int i=0;i<this->number_points;i++){//sum up the probability in % of the discrte distribution
		check_sum=check_sum+this->discret_points[i].probability;
		if(this->discret_points[i].probability<0.0 || this->discret_points[i].probability>100.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Percentage found : "<< this->discret_points[i].probability<< label::percentage<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->discret_points[i].abs_discrete_value < this->boundaries.min && calculate_middle==false){
			//external warning
			Warning msg=this->set_warning(1);
			msg.output_msg(1);
			//reaction
			this->discret_points[i].abs_discrete_value=this->boundaries.min;
		}
		if(this->discret_points[i].abs_discrete_value>this->boundaries.max && calculate_middle==false){
			//external warning
			Warning msg=this->set_warning(2);
			msg.output_msg(1);
			//reaction
			this->discret_points[i].abs_discrete_value=this->boundaries.max;
		}
		//calculate absolute middle
		this->absolut_middle=this->absolut_middle+this->discret_points[i].abs_discrete_value*this->discret_points[i].probability/100.0;
	}

	//check if sum is 100 %
	if(abs(check_sum-100.0)>1e-2){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "Sum of percentage found : "<< check_sum<< label::percentage<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Find the key-values for the file input of the distribution type
void Fpl_Distribution_Class_Discret::find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_no_discret);
	if(pos>=0 && wrong_input==false && calculated_flag==false){
		buffer=myline.substr(fpl_label::key_no_discret.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_points;
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
		Error msg=this->set_error(10);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Convert the values from relative values to absolute values; required for the dynamic mid-value setting
void Fpl_Distribution_Class_Discret::relativ_to_absolut(void){
	//here the discrete values are relativ values [%]
	for (int i=0; i<this->number_points; i++){
		this->discret_points[i].abs_discrete_value=this->absolut_middle+this->absolut_middle*this->discret_points[i].rel_discrete_value/100.0;
	}
}
//Set error(s)
Error Fpl_Distribution_Class_Discret::set_error(const int err_type){
		string place="Fpl_Distribution_Class_Discret::";
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
		case 3://Probabilities not between 0 and 100% (error)
			place.append("check_input(void)") ;
			reason="One discret probability is not between 0% and 100%";
			help="Check the variable"; 
			type=3;
			break;
		case 4://sum of probabilities is not 100%
			place.append("check_input(void)") ;
			reason="The sum of the discret probabilities is not 100%";
			help="Check the variable"; 
			type=3;
			break;
		case 5://bad alloc
			place.append("allocate_discrete_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://wrong sql request
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Invalid database request";
			help="Check the database";
			break;
		case 7://no variable found
			place.append("set_input(const string name, const string default_name, QSqlDatabase *ptr_database, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="No distribution type is found to the random variable";
			help="Check the random variable and the distribution type";
			break;
		case 8://do not find the end of the distribution type information in file
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find the end of the distribution type information in file";
			help="The the distribution type information has to end with !$END_DIST_TYPE";
			type=1;
			break;
		case 9://wrong input
			place.append("find_key_values_file(const string myline, int *must_found_counter, const bool calculated_flag)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the distribution type information of the discret-type in file";
			type=1;
			break;
		case 10://do not find all required distribution type infos
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Can not find all required discret type distribution information";
			help="Check if all discret type distribution information are set in the file";
			info << "Required information:"<<endl;
			info << " " << fpl_label::key_no_discret<< endl;
			type=1;
			break;
		case 11://wrong input
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the points of the discret distribution in file";
			type=1;
			break;
		case 12://not all points found
			place.append("set_input(QFile *ifile, int *line_counter, const bool calculated_mean, const _fpl_boundaries boundaries)");
			reason="Not all required points of the discret distribution are found";
			help="Check the points of the discret distribution in file";
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
Warning Fpl_Distribution_Class_Discret::set_warning(const int warn_type){
		string place="Fpl_Distribution_Class_Discret::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 1://value is < min boundarie and nonnegative flag is set
			place.append("check_input(const bool calculate_middle)") ;
			reason="The value of the variable greater than min boundary";
			reaction="x-value is set to min_boundary" ;
			help= "Check the variable";
			info << "New x-value " << this->boundaries.min; 
			type=3;
			break;
		case 2://mean is < 0.0 and nonnegative flag is set
			place.append("check_input(const bool calculate_middle)") ;
			reason="The value of the variable greater than max boundary";
			reaction="x-value is set to max_boundary";
			help= "Check the variable";
			info << "New x-value " << this->boundaries.max; 
			type=3;
			break;
		case 3://input datas can not submitted
			place.append("transfer_disttype2database(const int position, QSqlDatabase *ptr_database, const bool calculated_mean)");
			reason="Can not submit the distribution type data (discret-type) to the database";
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

