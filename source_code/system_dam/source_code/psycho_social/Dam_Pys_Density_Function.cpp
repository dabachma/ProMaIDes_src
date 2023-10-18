#include "Dam_Headers_Precompiled.h"
//#include "Dam_Pys_Density_Function.h"

//init static members
Tables *Dam_Pys_Density_Function::point_table=NULL;


//Default constructor
Dam_Pys_Density_Function::Dam_Pys_Density_Function(void){
	this->density_factor=1.0;
	this->function_is_set=false;
	this->stepwise_flag=true;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Pys_Density_Function), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Pys_Density_Function::~Dam_Pys_Density_Function(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Pys_Density_Function), _sys_system_modules::DAM_SYS);
}
//_________________
//public
//Read in the population-density function per file
void Dam_Pys_Density_Function::read_function(ifstream *ifile, int *line_counter){

	//read in
	string myline;
	int pos=-1;
	stringstream my_stream;
	bool found_flag=false;


	do{
		//search for the begin of the curve
		do{
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);
	
			pos=myline.rfind("!BEGIN");
			if(pos>=0){
				myline.erase(pos+6);
			}
		}
		while(myline!="!BEGIN" && ifile->eof()!=true);
		if(ifile->eof()==true){
			break;
		}

	
		do{
			//next line for the curve number and number of points
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);
		}
		while(myline.empty()==true);

		this->check_line(myline, *line_counter);

		//read it in
		int col=functions::count_number_columns(myline);
		my_stream << myline;
		if(col == 1){
			my_stream >> this->number_points;
		}
		else{
			string bool_buff;
			my_stream >> this->number_points >> bool_buff;
			try{
				this->stepwise_flag=functions::convert_string2boolean(bool_buff);
			}
			catch(Error msg){
				ostringstream info;
				info << "Error occurs near line: "<< *line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

		}

		if(my_stream.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Error occurs near line: "<< *line_counter << endl;
			Error msg=this->set_error(1);
			msg.make_second_info(info.str());
			throw msg;
		}
		my_stream.clear();
		my_stream.str("");
		found_flag=true;

	}
	while(found_flag!=true && ifile->eof()!=true);

	if(found_flag!=true){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Error occurs near line: "<< *line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//allocate the points
		this->allocate_points();

		int counter=0;

		//read in the points
		for(int i=0; i<this->number_points; i++){

			//delete the comments
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);

			if(myline.empty()!=true){
				
				pos=myline.rfind("!END");
				if(pos>=0){
					myline.erase(pos+4);
				}
				//stop when end is reach
				if(myline=="!END"|| ifile->eof()==true){
					break;
				}
				this->check_line(myline, *line_counter);
				//read it in
				int nr_col=functions::count_number_columns(myline);
				my_stream << myline;
				if(nr_col==2){
					my_stream >> this->function_points[i].x_value >> this->function_points[i].y_value;
					if(my_stream.fail()==true){
						ostringstream info;
						info << "Wrong input sequenze  :" << my_stream.str() << endl;
						info << "Error occurs near line: "<< *line_counter << endl;
						Error msg=this->set_error(1);
						msg.make_second_info(info.str());
						throw msg;
					}
				}
				else{
					ostringstream info;
					info << "Found number of columns :" << nr_col << endl;
					info << "Error occurs near line  : "<< *line_counter << endl;
					Error msg=this->set_error(4);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.str("");
				my_stream.clear();
				counter++;
			}
			else{
				i--;
			}

		}

		//check if all is read
		if(counter!=this->number_points){
			ostringstream info;
			info << "Given number of points: " <<this->number_points <<endl;
			info << "Found number of points: " <<counter << endl;
			info << "Error occurs near line: "<< *line_counter << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}
		//check the function
		this->check_function();
	}
	catch(Error msg){
		ostringstream info;
		info<<"Population-Density factor function"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the density function data to a database: the function points
void Dam_Pys_Density_Function::transfer_input_members2database(QSqlDatabase *ptr_database){
//mysql query with the table_model
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		 Dam_Pys_Density_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Dam_Pys_Density_Function::point_table->maximum_int_of_column(Dam_Pys_Density_Function::point_table->get_column_name(dam_label::point_id),ptr_database)+1;


	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_Pys_Density_Function::point_table->get_table_name();
	query_header <<" ( ";
	query_header << Dam_Pys_Density_Function::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_header << Dam_Pys_Density_Function::point_table->get_column_name(dam_label::pop_density) <<" , ";
	query_header << Dam_Pys_Density_Function::point_table->get_column_name(dam_label::density_factor) <<" ) ";
	query_header << " VALUES ";
	ostringstream query_data;


	for(int i=0; i< this->number_points; i++){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << this->function_points[i].x_value <<" , ";
		query_data <<this->function_points[i].y_value <<" ) " << " ,";
		glob_id++;
	}

	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Density_Function::point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

}
//Input the density function data per database
void Dam_Pys_Density_Function::input_function_perdatabase(QSqlDatabase *ptr_database){
	QSqlTableModel point_results(0,*ptr_database);

	try{
		Dam_Pys_Density_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	point_results.setTable((Dam_Pys_Density_Function::point_table->get_table_name()).c_str());

	//database request
	Data_Base::database_request(&point_results);
	//check the request
	if(point_results.lastError().isValid()){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Density_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << point_results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	point_results.sort(point_results.fieldIndex(Dam_Pys_Density_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	this->number_points= point_results.rowCount();


	//allocate the points
	this->allocate_points();


	//read the results out
	for(int i=0; i< this->number_points; i++){
		this->function_points[i].x_value=point_results.record(i).value((Dam_Pys_Density_Function::point_table->get_column_name(dam_label::pop_density)).c_str()).toDouble();
		this->function_points[i].y_value=point_results.record(i).value((Dam_Pys_Density_Function::point_table->get_column_name(dam_label::density_factor)).c_str()).toDouble();
	}


	if(this->number_points>0){
		this->function_is_set=true;
	}

	try{
		this->check_function();
	}
	catch(Error msg){
		throw msg;
	}

}
//Calculate and get the density factor
double Dam_Pys_Density_Function::calculate_density_factor(const double pop_density){
	this->density_factor=1.0;
	if(this->number_points==0){
		return this->density_factor;
	}
	else{
		this->density_factor=this->get_y_value(pop_density);
		return this->density_factor;
	}
}
//Get the flag, if the function is set
bool Dam_Pys_Density_Function::get_function_is_set(void){
	return this->function_is_set;
}
//Output the members of the population-density function to display/console
void Dam_Pys_Density_Function::output_member(void){
	ostringstream cout;
	cout << "Output the psycho-social density function..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	ostringstream prefix;
	prefix << "PYS> DENS_F> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	cout << "FUNCTION"<<endl;
	cout << " No. points                     " << this->number_points << endl;
	cout << " Linaer (false)/stepwise (true) " << functions::convert_boolean2string(this->stepwise_flag) << endl;   
	Sys_Common_Output::output_dam->output_txt(&cout,false);
	cout << "No." << W(9) <<"Value" <<W(18)<< "Factor" <<label::no_unit<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);

	_Dam_2value_Function::output_member();

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();

}
//Create the database table for the psycho-social density-functions points (static)
void Dam_Pys_Density_Function::create_point_table(QSqlDatabase *ptr_database){
		if( Dam_Pys_Density_Function::point_table==NULL){
			ostringstream cout;
			cout << "Create psycho-social density-function point database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_dens_func_point;
			const int num_col=3;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=dam_label::point_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::pop_density;
			tab_col[1].type=sys_label::tab_col_type_double;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::density_factor;
			tab_col[2].type=sys_label::tab_col_type_double;
			tab_col[2].unsigned_flag=true;



			try{
				 Dam_Pys_Density_Function::point_table= new Tables();
				if( Dam_Pys_Density_Function::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg(" Dam_Pys_Density_Function::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				 Dam_Pys_Density_Function::close_point_table();
				throw msg;
			}

		 Dam_Pys_Density_Function::close_point_table();
	}

}
//Set the database table for the psycho-social density-functions points: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Density_Function::set_point_table(QSqlDatabase *ptr_database){
	if( Dam_Pys_Density_Function::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_dens_func_point;
		string tab_col[3];
		tab_col[0]=dam_label::point_id;
		tab_col[1]=dam_label::pop_density;
		tab_col[2]=dam_label::density_factor;



		try{
			 Dam_Pys_Density_Function::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			 Dam_Pys_Density_Function::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg(" Dam_Pys_Density_Function::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			 Dam_Pys_Density_Function::close_point_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the psycho-social density-functions points (static)
void Dam_Pys_Density_Function::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		 Dam_Pys_Density_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	 Dam_Pys_Density_Function::point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the ppsycho-social density-functions points (static)
void Dam_Pys_Density_Function::close_point_table(void){
	if( Dam_Pys_Density_Function::point_table!=NULL){
		delete  Dam_Pys_Density_Function::point_table;
		 Dam_Pys_Density_Function::point_table=NULL;
	}
}
//Select the points and count the number of points of the psycho-social density function in a database table
int Dam_Pys_Density_Function::select_relevant_function_points_database(QSqlTableModel *results, const bool with_output){
 	int number=0;
	try{
		Dam_Pys_Density_Function::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant psycho-social density function points in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Pys_Density_Function::point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Pys_Density_Function::point_table->get_column_name(dam_label::point_id) << " >= " << "0";

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Density_Function::select_relevant_function_points_database(QSqlTableModel *results, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Density_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Pys_Density_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social density function points are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	return number;
}
//____________
//private
//Set error(s)
Error Dam_Pys_Density_Function::set_error(const int err_type){
	string place="Dam_Pys_Function::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1://wrong input
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the cost function in file";
			type=1;
			break;
		case 2://given curve number is not found 
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="The function starts with !BEGIN but no values are found after";
			help="Check the cost function in file";
			type=1;
			break;
		case 3://not all points are found
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="Not all points of the damage function are found";
			help="Check the file";
			type=1;
			break;
		case 4://number of columns is wrong
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="There is a problem with the input; wrong number of columns";
			help="Check the damage function in file";
			type=1;
			break;
		case 5://can not read the function in per database
			place.append("input_function_perdatabase(QSqlDatabase *ptr_database)");
			reason="Can not read in the density function points per database";
			help="Check the density function in database";
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
Warning Dam_Pys_Density_Function::set_warning(const int warn_type){
	string place="Dam_Pys_Density_Function::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the data of the density function to the database";
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