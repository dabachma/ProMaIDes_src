#include "Dam_Headers_Precompiled.h"
//#include "Dam_Eco_Btype.h"

//init static members
Tables *Dam_Eco_Btype::table=NULL;

//Default constructor
Dam_Eco_Btype::Dam_Eco_Btype(void):	m(0.63), b(-0.83){
		
	this->name=label::not_set;
	this->id=-1;
	this->cost=0.0;
	this->dam_function=NULL;
	this->number_dam_function=0;
	this->risk_factor=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Eco_Btype), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Eco_Btype::~Dam_Eco_Btype(void){

	this->delete_damage_func();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Eco_Btype), _sys_system_modules::DAM_SYS);
}
//_______________
//public
//Read in one ecologic biotope-type from file
void Dam_Eco_Btype::read_biotope_type_per_file(ifstream *ifile, int *line_counter){
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
		if(col == 4){
			my_stream >> this->number_dam_function >> this->id >> this->cost >> this->name;
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
		this->allocate_damage_func();

		for(int i=0; i< this->number_dam_function;i++){
			this->dam_function[i].read_function(ifile, line_counter);
		}
	}
	catch(Error msg){
		ostringstream info;
		info<<"Biotope-type id     : " <<this->id << endl;
		info<<"Biotope-type name   : " <<this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the biotope-type data to a database: the general settings of the biotope-type as well as the damage functions
void Dam_Eco_Btype::transfer_input_members2database(const int id_glob, QSqlTableModel *model, QSqlDatabase *ptr_database){
	//mysql query with the table_model

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	Dam_Damage_System::check_stop_thread_flag();
	//give the table name to the query model
	model->setTable((Dam_Eco_Btype::table->get_table_name()).c_str());
	
	//insert row
	model->insertRows(0, 1);
	//insert data
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::glob_id)).c_str())),id_glob);
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str())),this->id);
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str())),this->name.c_str());
	model->setData(model->index(0,model->record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_value)).c_str())),this->cost);
	//submit it to the datbase
	Data_Base::database_submit(model);
	
	if(model->lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Btype::table->get_table_name() << endl;
		info << "Table error info          : " << model->lastError().text().toStdString() << endl;
		info << "Biotope-type id           : " << this->id << endl;
		info << "Biotope-type name         : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
	
	int func_id_glob;
	func_id_glob=Dam_Eco_Btype_Damage_Function::get_max_glob_id_biotope_function_table(ptr_database)+1;

	//transfer the functions
	for(int i=0; i< this->number_dam_function; i++){
		this->dam_function[i].transfer_input_members2database(func_id_glob, model, ptr_database, this->id);
		func_id_glob++;
	}
}
//Add biotope-type data to a database: the general settings of the functions (biotope-type information) as well as the function points
void Dam_Eco_Btype::add_input_members2database(QSqlDatabase *ptr_database){
//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlTableModel model(NULL, *ptr_database);

	Dam_Damage_System::check_stop_thread_flag();
	//give the table name to the query model
	model.setTable((Dam_Eco_Btype::table->get_table_name()).c_str());


	int glob_id;
	glob_id=Dam_Eco_Btype::get_max_glob_id_biotope_table(ptr_database)+1;

	this->id=Dam_Eco_Btype::check_biotope_type_id_is_used(ptr_database, this->id);

	//insert row
	model.insertRows(0, 1);
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::glob_id)).c_str())),glob_id);
	model.setData(model.index(0,model.record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str())),this->id);
	model.setData(model.index(0,model.record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str())),this->name.c_str());
	model.setData(model.index(0,model.record().indexOf((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_value)).c_str())),this->cost);
	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Eco_Btype::table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Biotope-type id           : " << this->id << endl;
		info << "Biotope-type name         : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
	
	int func_id_glob;
	func_id_glob=Dam_Eco_Btype_Damage_Function::get_max_glob_id_biotope_function_table(ptr_database)+1;

	//transfer the functions
	for(int i=0; i< this->number_dam_function; i++){
		this->dam_function[i].transfer_input_members2database(func_id_glob, &model, ptr_database, this->id);
		func_id_glob++;
	}






}
//Input the the biotope-type data to a database: the general settings of the biotope-type as well as the damage functions
void Dam_Eco_Btype::input_biotope_type_perdatabase(const QSqlTableModel *results, const int glob_index){
	

	this->id=results->record(glob_index).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str()).toInt();
	this->name=results->record(glob_index).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str()).toString().toStdString();
	this->cost=results->record(glob_index).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_value)).c_str()).toDouble();


	QSqlTableModel point_results(0,results->database());
	//number of points
	this->number_dam_function=Dam_Eco_Btype_Damage_Function::select_relevant_functions_database(&point_results, this->id, false);

	//allocate the points
	this->allocate_damage_func();
	//read in points
	for(int i=0; i< this->number_dam_function; i++){
		this->dam_function[i].input_function_perdatabase(&point_results, i);
	}
}
//Calculate the damages to a biotope-type by a given impact value
double Dam_Eco_Btype::calculate_damages(Dam_Impact_Values *impact){
	double cost_factor=0.0;
	double sum_weigth=0.0;
	//element was not affected
	if(impact->get_was_wet_flag()==false){
		this->risk_factor=0;
		return cost_factor;
	}

	//sum up
	for(int i=0; i< this->number_dam_function; i++){
		sum_weigth=sum_weigth+this->dam_function[i].get_weight_vul_value();
		cost_factor=cost_factor+this->dam_function[i].get_weighted_cost_factor(impact);
	}
	//linear weighting
	cost_factor=cost_factor/sum_weigth;
	cost_factor=cost_factor*this->m+this->b;

	if(cost_factor>1.0){
		cost_factor=1.0;
	}
	else if(cost_factor<0.0){
		cost_factor=0.0;
	}
	//evaluate the risk factor
	if(cost_factor >0.0 && cost_factor <0.2){
		this->risk_factor=1;
	}
	else if(cost_factor >=0.2 && cost_factor <0.4){
		this->risk_factor=2;
	}
	else if(cost_factor >=0.4 && cost_factor <0.6){
		this->risk_factor=3;
	}
	else if(cost_factor >=0.6 && cost_factor <0.8){
		this->risk_factor=4;
	}
	else if(cost_factor >=0.8 && cost_factor <=1.0){
		this->risk_factor=5;
	}

	return cost_factor;
}
//Get the index of the biotope-type
int Dam_Eco_Btype::get_index(void){
	return this->id;
}
//Get the name of the biotope-type
string Dam_Eco_Btype::get_name(void){
	return this->name;
}
//Get the risk factor
int Dam_Eco_Btype::get_risk_factor(void){
	return this->risk_factor;
}
//Get the cost-value of the biotope-type
double Dam_Eco_Btype::get_cost_value(void){
	return this->cost;
}
//Output the members of the damage function to display/console
void Dam_Eco_Btype::output_member(void){
	ostringstream prefix;
	prefix << "BTY> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "GENERAL"<<endl;
	cout << " Name                      " << this->name << endl;
	cout << " Index                     " << this->id<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	cout << " Costvalue                 " << P(2)<< FORMAT_FIXED_REAL << this->cost << label::euro_per_sqm<<endl;
	cout << " Number damage function(s) " << this->number_dam_function << endl;
	cout << "FUNCTIONS"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);
	if(this->number_dam_function >0){
		for(int i=0; i< this->number_dam_function; i++){
			this->dam_function[i].output_member();
		}
	}
	else{
		cout << " No damage functions are set ! " << endl;
		Sys_Common_Output::output_dam->output_txt(&cout,true);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}

//Create the database table for the general information of the biotope-type
void Dam_Eco_Btype::create_table(QSqlDatabase *ptr_database){
		if(Dam_Eco_Btype::table==NULL){
			ostringstream cout;
			cout << "Create ecological biotope-type database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_eco_btype;
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

			tab_col[1].name=dam_label::biotope_type_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::biotope_type_name;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=dam_label::biotope_type_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;
			tab_col[3].default_value="0.0";

			tab_col[4].name=label::description;
			tab_col[4].type=sys_label::tab_col_type_string;


			try{
				Dam_Eco_Btype::table= new Tables();
				if(Dam_Eco_Btype::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Eco_Btype::create_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Eco_Btype::close_table();
				throw msg;
			}

		Dam_Eco_Btype::close_table();
	}

}
//Set the database table for the general information of the biotope-type: it sets the table name and the name of the columns and allocate them
void Dam_Eco_Btype::set_table(QSqlDatabase *ptr_database){
	if(Dam_Eco_Btype::table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_eco_btype;
		string tab_col[5];

		tab_col[0]=dam_label::glob_id;
		tab_col[1]=dam_label::biotope_type_id;
		tab_col[2]=dam_label::biotope_type_name;
		tab_col[3]=dam_label::biotope_type_value;
		tab_col[4]=label::description;


		try{
			Dam_Eco_Btype::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Eco_Btype::table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Eco_Btype::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Eco_Btype::close_table();
			throw msg;
		}
	}
	Dam_Eco_Btype_Damage_Function::set_function_table(ptr_database);
}
//Delete all data in the database table for the general information of the biotope-type
void Dam_Eco_Btype::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Eco_Btype::table->delete_data_in_table(ptr_database);
	Dam_Eco_Btype_Damage_Function::delete_data_in_function_table(ptr_database);

}
//Close and delete the database table for the general information of the biotope-type
void Dam_Eco_Btype::close_table(void){
	if(Dam_Eco_Btype::table!=NULL){
		delete Dam_Eco_Btype::table;
		Dam_Eco_Btype::table=NULL;
	}
	Dam_Eco_Btype_Damage_Function::close_function_table();
}
//Get the maximum value of the global index of the biotope-type database table (static)
int Dam_Eco_Btype::get_max_glob_id_biotope_table(QSqlDatabase *ptr_database){
	int id_glob;
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Eco_Btype::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	id_glob=Dam_Eco_Btype::table->maximum_int_of_column(Dam_Eco_Btype::table->get_column_name(dam_label::glob_id),ptr_database);
	return id_glob;
}
//Select and count the number of relevant biotope-types in a database table (static)
int Dam_Eco_Btype::select_relevant_biotope_types_database(QSqlTableModel *results,  const bool with_output){
 	int number=0;
	try{
		Dam_Eco_Btype::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant ecological biotope-types in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Eco_Btype::table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Eco_Btype::table->get_column_name(dam_label::glob_id) << " >= " << "0";

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Btype::select_relevant_biotope_types_database(QSqlTableModel *results,  const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Btype::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Eco_Btype::table->get_column_name(dam_label::glob_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant ecological biotope-type(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	return number;
}
//Select and count the number of relevant biotope-types in a database table by a given biotope-type (static)
int Dam_Eco_Btype::select_relevant_biotope_types_database(QSqlTableModel *results, const int btype_id){
	 	int number=0;
	try{
		Dam_Eco_Btype::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Eco_Btype::table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Eco_Btype::table->get_column_name(dam_label::glob_id) << " = " << btype_id;

	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Eco_Btype::select_relevant_biotope_types_database(QSqlTableModel *results,  const int btype_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Eco_Btype::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Eco_Btype::table->get_column_name(dam_label::glob_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	return number;
}
//Get a pointer to the bitope-type function by a given index
Dam_Eco_Btype_Damage_Function* Dam_Eco_Btype::get_ptr_btype_function(const int index){
	if(index<0 || index >= this->number_dam_function){
		return NULL;
	}
	else{

		return &this->dam_function[index];
	}
}
//_______________
//private
///Allocate the damage function(s)
void Dam_Eco_Btype::allocate_damage_func(void){
	try{
		this->dam_function=new Dam_Eco_Btype_Damage_Function[this->number_dam_function];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Delete the damage function(s)
void Dam_Eco_Btype::delete_damage_func(void){
	if(this->dam_function!=NULL){
		delete []this->dam_function;
		this->dam_function=NULL;
	}
}
//Check if a biotope-type id is already in use; it this is the case return a new biotope-type id
int Dam_Eco_Btype::check_biotope_type_id_is_used(QSqlDatabase *ptr_database, const int id){
	QSqlTableModel results(NULL, *ptr_database);
	int number=0;
	number=Dam_Eco_Btype::select_relevant_biotope_types_database(&results, false);
	bool new_test=false;
	int new_id=id;

	do{
		new_test=false;
		for(int i=0; i<number; i++){
			if(new_id==results.record(i).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str()).toInt()){
				new_id++;
				new_test=true;
				break;
			}
		}
	}while(new_test==true);

	return new_id;
}
//Set error(s)
Error Dam_Eco_Btype::set_error(const int err_type){
	string place="Dam_Eco_Btype::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_damage_func(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://wrong input
			place.append("read_biotope_type_per_file(ifstream *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage function in file";
			type=1;
			break;
		case 2://following infos are not found
			place.append("read_biotope_type_per_file(ifstream *ifile, int *line_counter)");
			reason="The biotope-type starts with !BEGIN but no values are found after";
			help="Check the damage function in file";
			type=1;
			break;
		case 3://number of columns is wrong
			place.append("read_biotope_type_per_file(ifstream *ifile, int *line_counter)");
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
Warning Dam_Eco_Btype::set_warning(const int warn_type){
	string place="Dam_Eco_Btype::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the biotope-type to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("add_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the biotope-type to the database";
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
