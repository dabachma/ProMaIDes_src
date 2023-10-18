#include "Dam_Headers_Precompiled.h"
//#include "Dam_Eco_Btype_Damage_Function.h"
//init static members
Tables *Dam_Eco_Btype_Damage_Function::function_table=NULL;
Tables *Dam_Eco_Btype_Damage_Function::point_table=NULL;

//Default constructor
Dam_Eco_Btype_Damage_Function::Dam_Eco_Btype_Damage_Function(void){

	this->impact_type=_dam_impact_type::WATER_H;
	this->weight=1.0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Eco_Btype_Damage_Function), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Eco_Btype_Damage_Function::~Dam_Eco_Btype_Damage_Function(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Eco_Btype_Damage_Function), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Read in the damage function per file
void Dam_Eco_Btype_Damage_Function::read_function(ifstream *ifile, int *line_counter){

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
			this->check_line(myline, *line_counter);

			pos=myline.rfind("!BEGIN_FUNCTION");
			if(pos>=0){
				myline.erase(pos+15);
			}
		}
		while(myline!="!BEGIN_FUNCTION" && ifile->eof()!=true);
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
		string impact_type_buff;
		int col=functions::count_number_columns(myline);
		my_stream << myline;
		if(col == 2){
			my_stream >> this->number_points >> impact_type_buff;
		}
		else{
			string bool_buff;
			my_stream >> this->number_points >> impact_type_buff >> this->weight >> bool_buff;
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
		try{
			this->impact_type=Dam_Impact_Values::convert_txt2dam_impact_type(impact_type_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Error occurs near line: "<< *line_counter << endl;
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
				this->check_line(myline, *line_counter);
				pos=myline.rfind("!END_FUNCTION");
				if(pos>=0){
					myline.erase(pos+13);
				}
				//stop when end is reach
				if(myline=="!END_FUNCTION"|| ifile->eof()==true){
					break;
				}
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
		info<<"Impact value   : " <<Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the biotope-type damage function data to a database: the general settings of the functions as well as the function points
void Dam_Eco_Btype_Damage_Function::transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database, const int biotope_id){
	//mysql query with the table_model
	//QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model->setTable((Dam_Eco_Btype_Damage_Function::function_table->get_table_name()).c_str());

	//insert row
	model->insertRows(0, 1);
						
	//insert data
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::biotope_type_id)).c_str())),biotope_id);
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::impact_type)).c_str())),Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type).c_str());
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::func_weight)).c_str())),this->weight);
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::stepwise)).c_str())),this->stepwise_flag);
	//submit it to the datbase
	Data_Base::database_submit(model);
	
	if(model->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Btype_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Biotope-type              : " << biotope_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Input the biotope-type damage function data per database: the general settings of the functions as well as the function points
void Dam_Eco_Btype_Damage_Function::input_function_perdatabase(const QSqlTableModel *results, const int glob_index){
	int func_id=-1;
	string buffer;

	func_id=results->record(glob_index).value((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str()).toInt();
	buffer=results->record(glob_index).value((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::impact_type)).c_str()).toString().toStdString();
	this->weight=results->record(glob_index).value((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::func_weight)).c_str()).toDouble();
	this->stepwise_flag=results->record(glob_index).value((Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::stepwise)).c_str()).toBool();
		
	try{
		this->impact_type=Dam_Impact_Values::convert_txt2dam_impact_type(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Btype_Damage_Function::function_table->get_table_name() << endl;
		info << "Global function id        : " << func_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	QSqlTableModel point_results(0,results->database());
	//number of points
	this->number_points=Dam_Eco_Btype_Damage_Function::select_relevant_function_points_database(&point_results, func_id);

	//allocate the points
	this->allocate_points();
	//read in points
	for(int i=0; i< this->number_points; i++){
		this->input_function_points_perdatabase(&point_results, i);
	}

	try{
		this->check_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Global function id        : " << func_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the impact type of the function
_dam_impact_type Dam_Eco_Btype_Damage_Function::get_impact_type(void){
	return this->impact_type;
}
//Get the weight of the vulnerability values of this impact type
double Dam_Eco_Btype_Damage_Function::get_weight_vul_value(void){
	return this->weight;
}
//Get the weighted cost factor by a given impact value
double Dam_Eco_Btype_Damage_Function::get_weighted_cost_factor(Dam_Impact_Values *impact){
	double impact_dur_day=0.0;
	switch(this->impact_type){
			case _dam_impact_type::WATER_D:
				impact_dur_day=impact->get_impact_duration()/(double)constant::day_second;
				//calculate the additional duration for the watervolume left in the element
				impact_dur_day=impact_dur_day;
				return (this->get_y_value(impact_dur_day)*this->weight);
				break;
			case _dam_impact_type::WATER_V:
				return (this->get_y_value(impact->get_impact_v_tot())*this->weight);
				break;
			case _dam_impact_type::WATER_H:
				return (this->get_y_value(impact->get_impact_h())*this->weight);
				break;
			default:
				Warning msg=this->set_warning(2);
				msg.output_msg(4);
				return 0.0;
	}
}
//Output the members of the damage function to display/console
void Dam_Eco_Btype_Damage_Function::output_member(void){
	ostringstream cout;
	cout << " Impact value              " << Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type) << endl;
	cout << " No. points                " << this->number_points << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);
	cout << "No." << W(9) <<"Impact" <<W(18)<< "Vul_value" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);

	_Dam_2value_Function::output_member();

}
//Create the database table for the general information of the biotope-type damage functions (static)
void Dam_Eco_Btype_Damage_Function::create_function_table(QSqlDatabase *ptr_database){
		if(Dam_Eco_Btype_Damage_Function::function_table==NULL){
			ostringstream cout;
			cout << "Create ecological biotope-type damage function database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_eco_btype_func;
			const int num_col=5;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=dam_label::function_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::biotope_type_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=dam_label::impact_type;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=dam_label::func_weight;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;
			tab_col[3].default_value="1.0";

			tab_col[4].name=dam_label::stepwise;
			tab_col[4].type=sys_label::tab_col_type_bool;


			try{
				Dam_Eco_Btype_Damage_Function::function_table= new Tables();
				if(Dam_Eco_Btype_Damage_Function::function_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Eco_Btype_Damage_Function::create_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Eco_Btype_Damage_Function::close_function_table();
				throw msg;
			}

		Dam_Eco_Btype_Damage_Function::close_function_table();
	}
	
	//create appending table
	Dam_Eco_Btype_Damage_Function::create_point_table(ptr_database);

}
//Set the database table for the general information of the biotope-type damage functions: it sets the table name and the name of the columns and allocate them (static)
void Dam_Eco_Btype_Damage_Function::set_function_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Btype_Damage_Function::function_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_eco_btype_func;
		string tab_col[5];
		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::biotope_type_id;
		tab_col[2]=dam_label::impact_type;
		tab_col[3]=dam_label::func_weight;
		tab_col[4]=dam_label::stepwise;

		try{
			Dam_Eco_Btype_Damage_Function::function_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Eco_Btype_Damage_Function::function_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Eco_Btype_Damage_Function::set_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Eco_Btype_Damage_Function::close_function_table();
			throw msg;
		}
	}
	//set appending table
	Dam_Eco_Btype_Damage_Function::set_point_table(ptr_database);

}
//Delete all data in the database table for the general information of the biotope-type damage functions (static)
void Dam_Eco_Btype_Damage_Function::delete_data_in_function_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Eco_Btype_Damage_Function::function_table->delete_data_in_table(ptr_database);

	//delete data in appending 
	Dam_Eco_Btype_Damage_Function::delete_data_in_point_table(ptr_database);
}
//Close and delete the database table for the general information of the biotope-type damage functions (static)
void Dam_Eco_Btype_Damage_Function::close_function_table(void){
	if(Dam_Eco_Btype_Damage_Function::function_table!=NULL){
		delete Dam_Eco_Btype_Damage_Function::function_table;
		Dam_Eco_Btype_Damage_Function::function_table=NULL;
	}
	//close appending tables
	Dam_Eco_Btype_Damage_Function::close_point_table();

}
//Get the maximum value of the global index of the biotope-type function database table (static)
int Dam_Eco_Btype_Damage_Function::get_max_glob_id_biotope_function_table(QSqlDatabase *ptr_database){
	int glob_id;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Dam_Eco_Btype_Damage_Function::function_table->maximum_int_of_column(Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::function_id),ptr_database);

	return glob_id;
}
///Select and count the number of relevant biotope-type damage functions in a database table (static)
int Dam_Eco_Btype_Damage_Function::select_relevant_functions_database(QSqlTableModel *results, const int biotope_id, const bool with_output){
	int number=0;
	try{
		Dam_Eco_Btype_Damage_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant ecological biotope-type damage functions in database of the biotope-types " << biotope_id<<"..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Eco_Btype_Damage_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::function_id) << " >= " << "0";
	filter << " and " ;
	filter << Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::biotope_type_id) << " = " << biotope_id;


	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Btype_Damage_Function::select_relevant_functions_database(QSqlTableModel *results,  const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Btype_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Eco_Btype_Damage_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant ecological biotope-type damage function(s) for biotiope-type "<< biotope_id <<" are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//____________
//private
//Transfer the biotope-type damage function points to a database
void Dam_Eco_Btype_Damage_Function::transfer_function_point2database(QSqlDatabase *ptr_database, const int function_id){

	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int id_glob=get_max_glob_id_biotope_function_point_table(ptr_database)+1;

	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_Eco_Btype_Damage_Function::point_table->get_table_name();
	query_header <<" ( ";
	query_header << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::glob_id) <<" , ";
	query_header << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::function_id) <<" , ";
	query_header << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_header << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::impact_value) <<" , ";
	query_header << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::rel_damage) <<" ) ";
	query_header << " VALUES ";
	ostringstream query_data;

	for(int i=0; i< this->number_points; i++){
		query_data << " ( ";
		query_data << id_glob <<" , ";
		query_data << function_id <<" , ";
		query_data << i <<" , ";
		query_data << this->function_points[i].x_value <<" , ";
		query_data << this->function_points[i].y_value <<" ) " << " ,";
		id_glob++;

	}
	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Btype_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Global function id        : " << function_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

}
//Input the biotope-type damage function points per database
void Dam_Eco_Btype_Damage_Function::input_function_points_perdatabase(const QSqlTableModel *results, const int index){
	this->function_points[index].x_value=results->record(index).value((Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::impact_value)).c_str()).toDouble();
	this->function_points[index].y_value=results->record(index).value((Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::rel_damage)).c_str()).toDouble();

}
//Create the database table for the biotope-type damage functions points (static)
void Dam_Eco_Btype_Damage_Function::create_point_table(QSqlDatabase *ptr_database){
		if(Dam_Eco_Btype_Damage_Function::point_table==NULL){
			ostringstream cout;
			cout << "Create ecological biotope-type damage function point database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_eco_btype_func_point;
			const int num_col=5;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=dam_label::glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::function_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=dam_label::point_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=dam_label::impact_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=dam_label::rel_damage;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;



			try{
				Dam_Eco_Btype_Damage_Function::point_table= new Tables();
				if(Dam_Eco_Btype_Damage_Function::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Eco_Btype_Damage_Function::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Eco_Btype_Damage_Function::close_point_table();
				throw msg;
			}

		Dam_Eco_Btype_Damage_Function::close_point_table();
	}
}
//Set the database table for the biotope-type damage functions points: it sets the table name and the name of the columns and allocate them (static)
void Dam_Eco_Btype_Damage_Function::set_point_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Btype_Damage_Function::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_eco_btype_func_point;
		string tab_col[5];
		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=dam_label::impact_value;
		tab_col[3]=dam_label::rel_damage;
		tab_col[4]=dam_label::glob_id;


		try{
			Dam_Eco_Btype_Damage_Function::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Eco_Btype_Damage_Function::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Eco_Btype_Damage_Function::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Eco_Btype_Damage_Function::close_point_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the biotope-type damage functions points (static)
void Dam_Eco_Btype_Damage_Function::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Eco_Btype_Damage_Function::point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the biotope-type damage functions points (static)
void Dam_Eco_Btype_Damage_Function::close_point_table(void){
	if(Dam_Eco_Btype_Damage_Function::point_table!=NULL){
		delete Dam_Eco_Btype_Damage_Function::point_table;
		Dam_Eco_Btype_Damage_Function::point_table=NULL;
	}
}
//Select and count the number of relevant points for the biotope-type damage function in a database table (static)
int Dam_Eco_Btype_Damage_Function::select_relevant_function_points_database(QSqlTableModel *results, const int index){
	int number=0;
	try{
		Dam_Eco_Btype_Damage_Function::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Eco_Btype_Damage_Function::point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::function_id) << " = " << index;

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Btype_Damage_Function::select_relevant_function_point_database(QSqlTableModel *results,  const int index)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Btype_Damage_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();
	return number;
}
//Get the maximum value of the global index of the biotope-type function point database table (static)
int Dam_Eco_Btype_Damage_Function::get_max_glob_id_biotope_function_point_table(QSqlDatabase *ptr_database){
	int glob_id;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Dam_Eco_Btype_Damage_Function::point_table->maximum_int_of_column(Dam_Eco_Btype_Damage_Function::point_table->get_column_name(dam_label::glob_id),ptr_database);

	return glob_id;
}
//Set error(s)
Error Dam_Eco_Btype_Damage_Function::set_error(const int err_type){
	string place="Dam_Eco_Btype_Damage_Function::";
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
			help="Check the damage function in file";
			type=1;
			break;
		case 2://given curve number is not found 
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="The function starts with !BEGIN_FUNCTION but no values are found after";
			help="Check the damage function in file";
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
Warning Dam_Eco_Btype_Damage_Function::set_warning(const int warn_type){
	string place="Dam_Eco_Btype_Damage_Function::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the biotope-type damage function to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("transfer_function_point2database(QSqlDatabase *ptr_database, const int id_glob)");
			reason="Can not submit the point data of the biotope-type damage function to the database";
			help="Check the database";
			type=2;
			break;
		case 2://no appropriate impact type for calculation
			place.append("get_y_value(Dam_Impact_Values *impact)");
			reason="The given impact type is not known for a biotope-type damage calculation";
			help="Implement the new impact type for a biotope-type";
			reaction="As result a 0.0 is returned";
			type=5;
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