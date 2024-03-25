#include "Dam_Headers_Precompiled.h"
//#include "Dam_Pys_Function.h"

//init static members
Tables *Dam_Pys_Function::function_table=NULL;
Tables *Dam_Pys_Function::point_table=NULL;

//Default constructor
Dam_Pys_Function::Dam_Pys_Function(void){
	this->global_index=-1;
	this->criteria=_dam_pys_criteria::crit_age_50_59;

	 //count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Pys_Function), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Pys_Function::~Dam_Pys_Function(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Pys_Function), _sys_system_modules::DAM_SYS);
}
//_________________
//public
//Read in the damage function per file
void Dam_Pys_Function::read_function(ifstream *ifile, int *line_counter){

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
		string crit_buff;
		int col=functions::count_number_columns(myline);
		my_stream << myline;
		if(col == 2){
			my_stream >> this->number_points >> crit_buff;
			try{
				this->criteria=Dam_Pys_Function::convert_txt2criteria(crit_buff);				
			}
			catch(Error msg){
				ostringstream info;
				info << "Error occurs near line: "<< *line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else{
			string bool_buff;
			my_stream >> this->number_points >>  crit_buff >>bool_buff;
			try{
				this->stepwise_flag=functions::convert_string2boolean(bool_buff);
			}
			catch(Error msg){
				ostringstream info;
				info << "Error occurs near line: "<< *line_counter << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			try{
				this->criteria=Dam_Pys_Function::convert_txt2criteria(crit_buff);				
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
				
				pos=myline.rfind("!END_FUNCTION");
				if(pos>=0){
					myline.erase(pos+15);
				}
				//stop when end is reach
				if(myline=="!END_FUNCTION"|| ifile->eof()==true){
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
		info<<"Function criteria   : " <<Dam_Pys_Function::convert_criteria2txt(this->criteria) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the pscho-social score function data to a database: the general settings of the functions as well as the function points
void Dam_Pys_Function::transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database, const int category_id){

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model->setTable((Dam_Pys_Function::function_table->get_table_name()).c_str());

	//insert row
	model->insertRows(0, 1);
						
	//insert data
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Function::function_table->get_column_name(dam_label::category_id)).c_str())),category_id);
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Function::function_table->get_column_name(dam_label::score_function_criteria)).c_str())),Dam_Pys_Function::convert_criteria2txt(this->criteria).c_str());
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Function::function_table->get_column_name(dam_label::stepwise)).c_str())),this->stepwise_flag);
	//submit it to the datbase
	Data_Base::database_submit(model);
	
	if(model->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Category                  : " << category_id << endl;
		info << "Criteria                  : " << Dam_Pys_Function::convert_criteria2txt(this->criteria) << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Input the the pscho-social score function data per database: the general settings of the functions as well as the function points
void Dam_Pys_Function::input_function_perdatabase(const QSqlTableModel *results, const int glob_index){
	int func_id=-1;
	string buffer;

	func_id=results->record(glob_index).value((Dam_Pys_Function::function_table->get_column_name(dam_label::function_id)).c_str()).toInt();
	buffer=results->record(glob_index).value((Dam_Pys_Function::function_table->get_column_name(dam_label::score_function_criteria)).c_str()).toString().toStdString();
	this->stepwise_flag=results->record(glob_index).value((Dam_Pys_Function::function_table->get_column_name(dam_label::stepwise)).c_str()).toBool();
		
	try{
		this->criteria=Dam_Pys_Function::convert_txt2criteria(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Function::function_table->get_table_name() << endl;
		info << "Global function id        : " << func_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	QSqlTableModel point_results(0,results->database());
	//number of points
	this->number_points=Dam_Pys_Function::select_relevant_function_points_database(&point_results, func_id);

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
		info<< "Function criteria   : " <<Dam_Pys_Function::convert_criteria2txt(this->criteria) << endl;
		info << "Global function id        : " << func_id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Output the members of the score-function to display/console
void Dam_Pys_Function::output_member(void){
	ostringstream prefix;
	prefix << "SCORE > ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "FUNCTION"<<endl;
	cout << " No. points                     " << this->number_points << endl;
	cout << " Crtiteria                      " << Dam_Pys_Function::convert_criteria2txt(this->criteria) << endl;
	cout << " Linear (false)/stepwise (true) " << functions::convert_boolean2string(this->stepwise_flag) << endl;   
	Sys_Common_Output::output_dam->output_txt(&cout,false);
	cout << "No." << W(9) <<"Crit_value" << label::percentage <<W(19)<< "Score_value" <<label::no_unit<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);

	_Dam_2value_Function::output_member();

	Sys_Common_Output::output_dam->rewind_userprefix();

}
//Create the database table for the general information of the score-function (static)
void Dam_Pys_Function::create_function_table(QSqlDatabase *ptr_database){
		if(Dam_Pys_Function::function_table==NULL){
			ostringstream cout;
			cout << "Create psycho-social score-function database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_score_func;
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

			tab_col[1].name=dam_label::category_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::score_function_criteria;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=dam_label::stepwise;
			tab_col[3].type=sys_label::tab_col_type_bool;

			tab_col[4].name=label::description;
			tab_col[4].type=sys_label::tab_col_type_string;


			try{
				Dam_Pys_Function::function_table= new Tables();
				if(Dam_Pys_Function::function_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Pys_Function::create_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Pys_Function::close_function_table();
				throw msg;
			}

		Dam_Pys_Function::close_function_table();
	}
	
	//create appending table
	Dam_Pys_Function::create_point_table(ptr_database);

}
//Set the database table for the general information of the score-functions: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Function::set_function_table(QSqlDatabase *ptr_database){
	if(Dam_Pys_Function::function_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_score_func;
		string tab_col[5];

		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::category_id;
		tab_col[2]=dam_label::score_function_criteria;
		tab_col[3]=dam_label::stepwise;
		tab_col[4]=label::description;


		try{
			Dam_Pys_Function::function_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Pys_Function::function_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Pys_Function::set_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Pys_Function::close_function_table();
			throw msg;
		}
	}
	//set appending table
	Dam_Pys_Function::set_point_table(ptr_database);

}
//Delete all data in the database table for the general information of the score-functions (static)
void Dam_Pys_Function::delete_data_in_function_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Pys_Function::function_table->delete_data_in_table(ptr_database);

	//delete data in appending 
	Dam_Pys_Function::delete_data_in_point_table(ptr_database);

}
//Close and delete the database table for the general information of the score-functions (static)
void Dam_Pys_Function::close_function_table(void){
	if(Dam_Pys_Function::function_table!=NULL){
		delete Dam_Pys_Function::function_table;
		Dam_Pys_Function::function_table=NULL;
	}
	//close appending tables
	Dam_Pys_Function::close_point_table();

}
//Get the maximum value of the global index of the psycho-social category function database table (static)
int Dam_Pys_Function::get_max_glob_id_category_function_table(QSqlDatabase *ptr_database){
	int glob_id;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Dam_Pys_Function::function_table->maximum_int_of_column(Dam_Pys_Function::function_table->get_column_name(dam_label::function_id),ptr_database);

	return glob_id;
}
//Select and count the number of relevant psycho-social categorye functions in a database table (static)
int Dam_Pys_Function::select_relevant_functions_database(QSqlTableModel *results, const int category_id, const bool with_output){
	int number=0;
	try{
		Dam_Pys_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant psycho-social  score-function in database of the category " << category_id<<"..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Pys_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Pys_Function::function_table->get_column_name(dam_label::function_id) << " >= " << "0";
	filter << " and " ;
	filter << Dam_Pys_Function::function_table->get_column_name(dam_label::category_id) << " = " << category_id;


	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Function::select_relevant_functions_database(QSqlTableModel *results, const int category_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Pys_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social score function(s) for category "<< category_id <<" are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Convert a string into a psycho-social criteria (_dam_pys_criteria) (static)
_dam_pys_criteria Dam_Pys_Function::convert_txt2criteria(const string txt){

	_dam_pys_criteria criteria;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lupper_case(&buffer);

	if(buffer==dam_label::crit_age_50_59){
		criteria=_dam_pys_criteria::crit_age_50_59;
	}
	else if(buffer==dam_label::crit_age_80){
		criteria=_dam_pys_criteria::crit_age_80;
	}
	else if(buffer==dam_label::crit_female){
		criteria=_dam_pys_criteria::crit_female;
	}
	else if(buffer==dam_label::crit_owner){
		criteria=_dam_pys_criteria::crit_owner;
	}
	else{
		Error msg;
		msg.set_msg("Dam_Pys_Function::convert_txt2criteria(const string txt)", "Can not convert this criteria", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert psycho-social criteria: " << txt << endl;
		info << "Possible criteria: "<< endl;
		info << "  "<<dam_label::crit_age_50_59  << endl;
		info << "  "<<dam_label::crit_age_80  << endl;
		info << "  "<<dam_label::crit_female  << endl;
		info << "  "<<dam_label::crit_owner  << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	return criteria;
}
//Convert a psycho-social criteria (_dam_pys_criteria) into a string  (static)
string Dam_Pys_Function::convert_criteria2txt(const _dam_pys_criteria criteria){
	string buffer;
	switch(criteria){
		case _dam_pys_criteria::crit_age_50_59:
			buffer=dam_label::crit_age_50_59;
			break;
		case _dam_pys_criteria::crit_age_80:
			buffer=dam_label::crit_age_80;
			break;
		case _dam_pys_criteria::crit_female:
			buffer=dam_label::crit_female;
			break;
		case _dam_pys_criteria::crit_owner:
			buffer=dam_label::crit_owner;
			break;
		default:
			buffer=label::unknown_type;
	}


	return buffer;
}
//Get the criteria type of the function
_dam_pys_criteria Dam_Pys_Function::get_criteria_type(void){
	return this->criteria;
}
//____________
//private
//Transfer the psycho-social criteria function points to a database
void Dam_Pys_Function::transfer_function_point2database(QSqlDatabase *ptr_database, const int function_id){
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int id_glob=Dam_Pys_Function::get_max_glob_id_criteria_function_point_table(ptr_database)+1;

	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_Pys_Function::point_table->get_table_name();
	query_header <<" ( ";
	query_header << Dam_Pys_Function::point_table->get_column_name(dam_label::glob_id) <<" , ";
	query_header << Dam_Pys_Function::point_table->get_column_name(dam_label::function_id) <<" , ";
	query_header << Dam_Pys_Function::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_header << Dam_Pys_Function::point_table->get_column_name(dam_label::criteria_value) <<" , ";
	query_header << Dam_Pys_Function::point_table->get_column_name(dam_label::score_value) <<" ) ";
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
		info << "Table Name                : " << Dam_Pys_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Global function id        : " << function_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
}
//Input the psycho-social criteria function points per database
void Dam_Pys_Function::input_function_points_perdatabase(const QSqlTableModel *results, const int index){
	this->function_points[index].x_value=results->record(index).value((Dam_Pys_Function::point_table->get_column_name(dam_label::criteria_value)).c_str()).toDouble();
	this->function_points[index].y_value=results->record(index).value((Dam_Pys_Function::point_table->get_column_name(dam_label::score_value)).c_str()).toDouble();
}
//Create the database table for the psycho-social score-functions points (static)
void Dam_Pys_Function::create_point_table(QSqlDatabase *ptr_database){
		if(Dam_Pys_Function::point_table==NULL){
			ostringstream cout;
			cout << "Create psycho-social score-function point database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_score_func_point;
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

			tab_col[3].name=dam_label::criteria_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=dam_label::score_value;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;



			try{
				Dam_Pys_Function::point_table= new Tables();
				if(Dam_Pys_Function::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Pys_Function::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Pys_Function::close_point_table();
				throw msg;
			}

		Dam_Pys_Function::close_point_table();
	}

}
//Set the database table for the psycho-social score-functions points: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Function::set_point_table(QSqlDatabase *ptr_database){
	if(Dam_Pys_Function::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_score_func_point;
		string tab_col[5];
		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=dam_label::criteria_value;
		tab_col[3]=dam_label::score_value;
		tab_col[4]=dam_label::glob_id;



		try{
			Dam_Pys_Function::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Pys_Function::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Pys_Function::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Pys_Function::close_point_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the psycho-social score-functions points (static)
void Dam_Pys_Function::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Pys_Function::point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the psycho-social score-functions points (static)
void Dam_Pys_Function::close_point_table(void){
	if(Dam_Pys_Function::point_table!=NULL){
		delete Dam_Pys_Function::point_table;
		Dam_Pys_Function::point_table=NULL;
	}
}
//Select and count the number of relevant points for the psycho-social criteria function in a database table (static)
int Dam_Pys_Function::select_relevant_function_points_database(QSqlTableModel *results, const int index){
	int number=0;
	try{
		Dam_Pys_Function::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Pys_Function::point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Pys_Function::point_table->get_column_name(dam_label::function_id) << " = " << index;

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Function::select_relevant_function_point_database(QSqlTableModel *results,  const int index)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Pys_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();
	return number;
}
//Get the maximum value of the global index of the psycho-social criteria function point database table (static)
int Dam_Pys_Function::get_max_glob_id_criteria_function_point_table(QSqlDatabase *ptr_database){
	int glob_id;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	glob_id=Dam_Pys_Function::point_table->maximum_int_of_column(Dam_Pys_Function::point_table->get_column_name(dam_label::glob_id),ptr_database);

	return glob_id;
}
//Check the function points
void Dam_Pys_Function::check_function(void){
	
	for(int i=0; i< this->number_points; i++){

		if(this->function_points[i].x_value<0.0 ||	this->function_points[i].x_value>100.0){
			ostringstream info;
			info << "Point id     : " << i << endl;
			info << "Value        : " << this->function_points[i].x_value <<endl;
			Error msg=this->set_error(5);
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->function_points[i].y_value<0.0){
			ostringstream info;
			info << "Point id     : " << i << endl;
			info << "Value        : " << this->function_points[i].y_value <<endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;

		}
	}
}
//Set error(s)
Error Dam_Pys_Function::set_error(const int err_type){
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
			reason="The function starts with !BEGIN_FUNCTION but no values are found after";
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
		case 5://criteria values are < 0.0 or >100.0
			place.append("check_function_points(void)");
			reason="The criteria values of the score function (x-value) can not be <0.0 or >100.0";
			help="Check the score function points in file";
			type=25;
			break;
		case 6://score values are < 0.0 
			place.append("check_function_points(void)");
			reason="The score values of the score function (y-value) can not be <0.0 ";
			help="Check the score function points in file";
			type=25;
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
Warning Dam_Pys_Function::set_warning(const int warn_type){
	string place="Dam_Pys_Function::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the psycho-social score function to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("transfer_function_point2database(QSqlDatabase *ptr_database, const int id_glob)");
			reason="Can not submit the point data of the psycho-social score function to the database";
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