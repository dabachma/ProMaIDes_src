#include "Dam_Headers_Precompiled.h"
//#include "Dam_Pys_Category.h"

//init static members
Tables *Dam_Pys_Category::table=NULL;

//Default constructor
Dam_Pys_Category::Dam_Pys_Category(void){
	this->no_score_func=0;
	this->score_function=NULL;
	this->name=label::not_set;
	this->index=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Pys_Category), _sys_system_modules::DAM_SYS);

}
//Default destructor
Dam_Pys_Category::~Dam_Pys_Category(void){
	this->delete_score_function();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Pys_Category), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Get the index of the category
int Dam_Pys_Category::get_index(void){
	return this->index;
}
//Get the name of the category
string Dam_Pys_Category::get_name(void){
	return this->name;
}
//Output the members
void Dam_Pys_Category::output_member(void){
	ostringstream prefix;
	prefix << "CAT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "CATEGORY"<<endl;
	cout << " Id                      " << this->index << endl;
	cout << " Name                    " << this->name << endl;
	cout << " No. score function(s)   " << this->no_score_func<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout,false);

	if(this->no_score_func>0){
		for(int i=0; i< this->no_score_func; i++){
			this->score_function[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in one psycho-social category from file
void Dam_Pys_Category::read_category_per_file(ifstream *ifile, int *line_counter){
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

		//read it in
		int col=functions::count_number_columns(myline);
		my_stream << myline;
		if(col == 3){
			my_stream >> this->no_score_func >> this->index >> this->name;
		}
		else{
			ostringstream info;
			info << "Found number of columns :" << col << endl;
			info << "Error occurs near line  : "<< *line_counter << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;

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
		//read in functions
		this->allocate_score_function();

		for(int i=0; i< this->no_score_func;i++){
			this->score_function[i].read_function(ifile, line_counter);
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Category id     : " <<this->index << endl;
		info<<"Category name   : " <<this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the psycho-social category data to a database: the general settings of the psycho-social category as well as the score function
void Dam_Pys_Category::transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database){
	//mysql query with the table_model
	try{
		Dam_Pys_Category::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	Dam_Damage_System::check_stop_thread_flag();
	//give the table name to the query model
	model->setTable((Dam_Pys_Category::table->get_table_name()).c_str());

	//insert row
	model->insertRows(0, 1);
	//insert data
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::glob_id)).c_str())),id_glob);
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::category_id)).c_str())),this->index);
	model->setData(model->index(0,model->record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::category_name)).c_str())),this->name.c_str());
	//submit it to the datbase
	Data_Base::database_submit(model);
	
	if(model->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Category::table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Category id               : " << this->index << endl;
		info << "Category name             : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
	
	int func_id_glob;
	func_id_glob=Dam_Pys_Function::get_max_glob_id_category_function_table(ptr_database)+1;

	//transfer the functions
	for(int i=0; i< this->no_score_func; i++){
		this->score_function[i].transfer_input_members2database(func_id_glob, model, ptr_database, this->index);
		func_id_glob++;
	}
}
//Add psycho-social category data to a database: the general settings of the psycho-social category as well as the function points
void Dam_Pys_Category::add_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	try{
		Dam_Pys_Category::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel model(0,*ptr_database);

	Dam_Damage_System::check_stop_thread_flag();
	//give the table name to the query model
	model.setTable((Dam_Pys_Category::table->get_table_name()).c_str());

	int glob_id;
	glob_id=Dam_Pys_Category::get_max_glob_id_category_table(ptr_database)+1;

	this->index=Dam_Pys_Category::check_category_id_is_used(ptr_database, this->index);

	//insert row
	model.insertRows(0, 1);
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::glob_id)).c_str())),glob_id);
	model.setData(model.index(0,model.record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::category_id)).c_str())),this->index);
	model.setData(model.index(0,model.record().indexOf((Dam_Pys_Category::table->get_column_name(dam_label::category_name)).c_str())),this->name.c_str());
	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Pys_Category::table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Category id               : " << this->index << endl;
		info << "Category name             : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
	
	int func_id_glob;
	func_id_glob=Dam_Pys_Function::get_max_glob_id_category_function_table(ptr_database)+1;

	//transfer the functions
	for(int i=0; i< this->no_score_func; i++){
		this->score_function[i].transfer_input_members2database(func_id_glob, &model, ptr_database, this->index);
		func_id_glob++;
	}
}
//Input the the psycho-social category data to a database: the general settings of the psycho-social category as well as the score functions
void Dam_Pys_Category::input_category_perdatabase(const QSqlTableModel *results, const int glob_index){
	this->index=results->record(glob_index).value((Dam_Pys_Category::table->get_column_name(dam_label::category_id)).c_str()).toInt();
	this->name=results->record(glob_index).value((Dam_Pys_Category::table->get_column_name(dam_label::category_name)).c_str()).toString().toStdString();
	
	QSqlTableModel point_results(0,results->database());
	//number of points
	this->no_score_func=Dam_Pys_Function::select_relevant_functions_database(&point_results, this->index, false);

	//allocate the points
	this->allocate_score_function();
	//read in points
	for(int i=0; i< this->no_score_func; i++){
		this->score_function[i].input_function_perdatabase(&point_results, i);
	}
}
//Get the score value for a given criteria and the corresponding value
double Dam_Pys_Category::get_score_value(const double value, const _dam_pys_criteria crit){
	double score =0.0;
	int index_func=-1;
	for(int i=0; i< this->no_score_func; i++){
		if(this->score_function[i].get_criteria_type()==crit){
			index_func=i;
			break;
		}
	}
	if(index_func>=0){
		score=this->score_function[index_func].get_y_value(value);
	}
	return score;
}
//Create the database table for psycho-social categories (static)
void Dam_Pys_Category::create_table(QSqlDatabase *ptr_database){
		if(Dam_Pys_Category::table==NULL){
			ostringstream cout;
			cout << "Create psycho-social category database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pys_category;
			const int num_col=4;
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

			tab_col[1].name=dam_label::category_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::category_name;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=label::description;
			tab_col[3].type=sys_label::tab_col_type_string;


			try{
				Dam_Pys_Category::table= new Tables();
				if(Dam_Pys_Category::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Pys_Category::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Pys_Category::close_table();
				throw msg;
			}

		Dam_Pys_Category::close_table();
	}
}
//Set the database table for psycho-social categories: it sets the table name and the name of the columns and allocate them (static)
void Dam_Pys_Category::set_table(QSqlDatabase *ptr_database){
	if(Dam_Pys_Category::table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pys_category;
		string tab_col[4];

		tab_col[0]=dam_label::category_id;
		tab_col[1]=dam_label::category_name;
		tab_col[2]=label::description;
		tab_col[3]=dam_label::glob_id;


		try{
			Dam_Pys_Category::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Pys_Category::table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Pys_Category::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Pys_Category::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for psycho-social categories (static)
void Dam_Pys_Category::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Category::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Pys_Category::table->delete_data_in_table(ptr_database);

	Dam_Pys_Function::delete_data_in_function_table(ptr_database);

}
//Close and delete the database table for psycho-social categories (static)
void Dam_Pys_Category::close_table(void){
	if(Dam_Pys_Category::table!=NULL){
		delete Dam_Pys_Category::table;
		Dam_Pys_Category::table=NULL;
	}
}
//Get the maximum value of the global index of the category database table (static)
int Dam_Pys_Category::get_max_glob_id_category_table(QSqlDatabase *ptr_database){
	int id_glob;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Pys_Category::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	id_glob=Dam_Pys_Category::table->maximum_int_of_column(Dam_Pys_Category::table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Select and count the number of relevant psycho-social categories in a database table (static)
int Dam_Pys_Category::select_relevant_category_database(QSqlTableModel *results, const bool with_output){
 	int number=0;
	try{
		Dam_Pys_Category::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant psycho-social categories in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Pys_Category::table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Pys_Category::table->get_column_name(dam_label::glob_id) << " >= " << "0";

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Category::select_relevant_category_database(QSqlTableModel *results, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Category::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Pys_Category::table->get_column_name(dam_label::glob_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant psycho-social categories are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	return number;

}
//Select and count the number of relevant psycho-social categories in a database table by a given category identifier (static)
int Dam_Pys_Category::select_relevant_category_database(QSqlTableModel *results, const int id){
	 	int number=0;
	try{
		Dam_Pys_Category::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Pys_Category::table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Pys_Category::table->get_column_name(dam_label::glob_id) << " = " << id;

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Pys_Category::select_relevant_category_database(QSqlTableModel *results, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Pys_Category::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Pys_Category::table->get_column_name(dam_label::glob_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	return number;
}
//Set the score criteria function to a plot widget
void Dam_Pys_Category::set_score_function2plot_widget(Sys_Plot_Wid *widget, _dam_pys_criteria criteria){

	for(int i=0; i< this->no_score_func; i++){
		if(criteria==this->score_function[i].get_criteria_type()){
			widget->set_number_curves(1);
			widget->set_axis_title("Criteria Percentage [%]", "Score [-]");
			widget->set_curve_characteristics(0, "Impact", QwtPlotCurve::Lines, Qt::black);
			this->score_function[i].set_members2plot_wid(widget);
			widget->show_plot();
			break;
		}

	}

}
//__________
//private
//Allocate the score function
void Dam_Pys_Category::allocate_score_function(void){
	try{
		this->score_function=new Dam_Pys_Function[this->no_score_func];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the score function
void Dam_Pys_Category::delete_score_function(void){
	if(this->score_function!=NULL){
		delete []this->score_function;
		this->score_function=NULL;
	}
}
//Check if a category id is already in use; it this is the case return a new category id
int Dam_Pys_Category::check_category_id_is_used(QSqlDatabase *ptr_database, const int id){
	QSqlTableModel results(NULL, *ptr_database);
	int number=0;
	number=Dam_Pys_Category::select_relevant_category_database(&results, false);
	bool new_test=false;
	int new_id=id;

	do{
		new_test=false;
		for(int i=0; i<number; i++){
			if(new_id==results.record(i).value((Dam_Pys_Category::table->get_column_name(dam_label::category_id)).c_str()).toInt()){
				new_id++;
				new_test=true;
				break;
			}
		}
	}while(new_test==true);

	return new_id;
}
//Set error(s)
Error Dam_Pys_Category::set_error(const int err_type){
	string place="Dam_Pys_Category::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_score_function(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://wrong input
			place.append("read_category_per_file(ifstream *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage function in file";
			type=1;
			break;
		case 2://following infos are not found
			place.append("read_category_per_file(ifstream *ifile, int *line_counter)");
			reason="The categories starts with !BEGIN but no values are found after";
			help="Check the damage function in file";
			type=1;
			break;
		case 3://number of columns is wrong
			place.append("read_category_per_file(ifstream *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong number of columns";
			help="Check the biotope-types in file";
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
Warning Dam_Pys_Category::set_warning(const int warn_type){
	string place="Dam_Pys_Category::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the categories to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("add_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the categories to the database";
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