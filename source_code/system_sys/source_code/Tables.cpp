//#include "Tables.h"
#include "Sys_Headers_Precompiled.h"

//init static members
string Tables::table_file_name=label::not_set;
string Tables::project_prefix="proj1_";

//Default constructor
Tables::Tables(void): table_suffix("_prm"){
	this->column=NULL;
	this->modul_type=_sys_system_modules::SYS_SYS;
	//set the table names
	this->table_name.id=label::not_set;
	this->table_name.name=label::not_set;
	this->table_name.found_flag=false;
	this->col_num=0;
}
//Constructor with a given id-name for the table and the id-names for the column; also the number of table columns is given
Tables::Tables(const string id_name, const string id_column[], const int column_number): table_suffix("_prm"){
	this->column=NULL;
	this->modul_type=_sys_system_modules::SYS_SYS;
	//set the table names
	this->table_name.id=id_name;
	this->table_name.name=label::not_set;
	this->table_name.found_flag=false;
	this->col_num=column_number;

	this->allocate_columns();
	//set the column_id
	for(int i=0; i<this->col_num; i++){
		this->column[i].id=id_column[i];
		this->column[i].name=label::not_set;	
		this->column[i].found_flag=false;
	}
}
//Default destructor
Tables::~Tables(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Tables), this->modul_type);
	if(this->column!=NULL){
		delete []this->column;
		this->column=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_sys_id_name)*this->col_num, this->modul_type);
	}	
}
//___________________________________________
//public
//Set the file name, where the tables are specified (id-name to used name), per console (table-file) (static)
void Tables::set_file_name(void){
	cout << endl;
	cout << "=======================================================================" <<endl;
	cout << "Give the name of the file where the tables are specified?" << endl;
	cin >> Tables::table_file_name;
}
//Set the file name, where the tables are specified (id-name to used name), directly (table-file) (static)
void Tables::set_file_name(const string name_table_file){
	Tables::table_file_name=name_table_file;
}
//Get the table-file name; here the table names are specified (id-name to used name) (static)
string Tables::get_table_file_name(void){
	return Tables::table_file_name;
}
//Set the id_name for the name of the database table
void Tables::set_id_name_table(const string id_name){
	//set the table names
	this->table_name.id=id_name;
	this->table_name.name=label::not_set;
	this->table_name.found_flag=false;

}
//Set the id_name for the clomun-names of the database table and the number of columns
void Tables::set_id_name_columns(const string id_column[], const int column_number){
	this->col_num=column_number;

	this->allocate_columns();
	//set the column_id
	for(int i=0; i<this->col_num; i++){
		this->column[i].id=id_column[i];
		this->column[i].name=label::not_set;	
		this->column[i].found_flag=false;
	}
}
//Set and combine the used name of the table and the columns with the id-names from the table-file
void Tables::set_name(QSqlDatabase *ptr_database, _sys_table_type actuel_type){
	this->tabletype=actuel_type;
	this->table_type2system_type();
	Sys_Memory_Count::self()->add_mem(sizeof(Tables), this->modul_type);
	Sys_Memory_Count::self()->add_mem(sizeof(_sys_id_name)*this->col_num, this->modul_type);

	this->set_output_prefix();
	ostringstream cout;
	cout << "First Initiation of table " << this->table_name.id  <<  " ..." <<endl;
	Sys_Common_Output::output_system->output_txt(cout.str(), true);

	string buffer;
	ifstream table_file;
	
	//open the file for reading
	table_file.open(table_file_name.c_str());
	//check the file
	if(!(table_file)){	
		//set error
		throw (set_error(1));
	}
	//set the name and the column name
	bool end_table_flag=false;
	bool name_found_flag=false;

	while(getline(table_file, buffer) && end_table_flag==false){
		try{
			//erase comments and leading and ending tabs/whitespaces
			functions::clean_string(&buffer);
			if(!buffer.empty()){
				if((this->found_expression(sys_label::table_begin, buffer))){
					while(getline(table_file, buffer) && end_table_flag==false){
						//erase comments and leading and ending tabs/whitespaces
						functions::clean_string(&buffer);
						if(!buffer.empty()){
							if((this->found_expression(this->table_name.id, buffer))){
								table_name.name=this->found_string(this->table_name.id, buffer);
								name_found_flag=true;
							}
							else if((this->found_expression(sys_label::table_end, buffer)) && name_found_flag==true){
										end_table_flag=true;
							}
							else if(name_found_flag==true){
								for(int i=0; i<this->col_num; i++){ 
									if((found_expression(this->column[i].id, buffer))){
										//this are the expression for which is searched; the string after (expect whitespace) is taken as parameter
										this->column[i].name=this->found_string(this->column[i].id, buffer);
									}
								}

							}
						}
					}
				}
			}
		}
		catch(Error msg){
			Sys_Common_Output::output_system->rewind_userprefix();
			throw msg;
		}		
	}
	//close the datafile
	table_file.close();
	//check
	cout.str("");
	cout << "Check the table: " << this->table_name.name<<  " ..." << endl;
	try{
		if(this->check_names(ptr_database)){//failure is catch in the method
			cout << " Table ok!" << endl;
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_system->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_system->output_txt(cout.str(), true);
	Sys_Common_Output::output_system->rewind_userprefix();


}
//Create a table in the database, if it does not exists
bool Tables::create_non_existing_tables(const string name, _Sys_data_tab_column columns[], const int column_number, QSqlDatabase *ptr_database, _sys_table_type actuel_type){

	//set the table names
	this->table_name.id=name;
	this->table_name.name=functions::convert_string2lower_case(name);
	this->table_name.found_flag=false;
	this->col_num=column_number;
	this->tabletype=actuel_type;
	this->table_type2system_type();
	Sys_Memory_Count::self()->add_mem(sizeof(Tables), this->modul_type);


	//check if the table exists
	QStringList tables;
	Data_Base::database_tables(&tables, ptr_database);

	string tab_name;
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		ostringstream buff;
		buff << Sys_Project::get_complete_project_database_schemata_name()<<"."<<this->get_total_table_name();
		tab_name=buff.str();
	}
	else{
		tab_name=this->get_total_table_name();
	}


	for(int i=0; i<tables.size();i++){;

		if(tables.at(i).toStdString()==tab_name){
			//the table exists
			return false;
		}
	}
	//create new table
	bool prim_key=false;
	bool key=false;




	ostringstream query_string;
	query_string <<"CREATE TABLE ";
	query_string << tab_name << " ( " <<endl;

	for(int i=0; i< this->col_num; i++){
		query_string << functions::convert_string2lower_case(columns[i].name) << "  " ;
		if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			if(columns[i].type==sys_label::tab_col_type_polyline || columns[i].type==sys_label::tab_col_type_polygon
				|| columns[i].type==sys_label::tab_col_type_point){
				query_string << " GEOMETRY " ;
			}
			else if(columns[i].type==sys_label::tab_col_type_double){
				query_string << " DOUBLE PRECISION " ;
			}
			else{
				query_string << columns[i].type << "  " ;
			}

		}
		else{
			if(columns[i].type==sys_label::tab_col_type_polyline || columns[i].type==sys_label::tab_col_type_polygon
				|| columns[i].type==sys_label::tab_col_type_point){
				query_string << " GEOMETRY " ;
			}
			else{
				query_string << columns[i].type << "  " ;
			}
		}
		if(columns[i].unsigned_flag==true && Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
			query_string << " UNSIGNED " ;
		}
		if(columns[i].default_value.empty()==false){
			query_string << " DEFAULT " << columns[i].default_value;
		}
		if(i<this->col_num-1){
			query_string << ",";
			query_string << endl;
		}
		//check if there is an key flag
		if(columns[i].key_flag==true){
			key=true;
		}
		//check if there is an primary keyflag
		if(columns[i].primary_key_flag==true){
			prim_key=true;
		}

	}
	//set the primary key
	if(prim_key==true){
		query_string << ",";
		query_string << endl;
		query_string <<"PRIMARY KEY( ";
		bool first_set=false;
		for(int i=0; i< this->col_num; i++){
			if(columns[i].primary_key_flag==true){
				if(first_set==true){
					query_string << ", " ;
				}
				query_string << functions::convert_string2lower_case(columns[i].name);
				first_set=true;
			}
		}
		query_string << ") ";
	}

	query_string <<endl;
	//end string
	query_string << ")";

	//development
	/*string buff_test;
	buff_test= query_string.str();*/

	//set the query
	QSqlQuery query(*ptr_database);
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()){
			Error msg=this->set_error(8);
			ostringstream info;
			info << "Table Name                : " << this->table_name.name << endl;
			info << "Table error info          : " << query.lastError().text().toStdString() << endl;
			string buffer;
			string buffer2;
			buffer=query.lastError().text().toStdString();
			buffer2=query_string.str();
			msg.make_second_info(info.str());
			throw msg;
	}
	query_string.str("");

	//set the index
	ostringstream id_name;
	id_name<<"uid_";
	if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		id_name<< this->get_total_table_name() ;
	}
	else if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		id_name << Sys_Project::get_complete_project_database_schemata_name()<<"_"<<this->get_total_table_name() ;
	}

	if(key==true){
		query_string <<"CREATE INDEX ";
		query_string <<id_name.str() << " ON "; 
		if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
			query_string << this->get_total_table_name() << "  " <<endl;
		}
		else if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			query_string << Sys_Project::get_complete_project_database_schemata_name()<<"."<<this->get_total_table_name() << "  " <<endl;
		}
		query_string<<"  (";
	
		
		

		bool first_set=false;
		for(int i=0; i< this->col_num; i++){
			if(columns[i].key_flag==true){
				if(first_set==true){
					query_string << ", " ;
				}
				query_string << functions::convert_string2lower_case(columns[i].name);
				first_set=true;
			}
		}
		query_string <<endl;
		//end string
		query_string << ")";


		//set the query
		QSqlQuery query_index(*ptr_database);
		Data_Base::database_request(&query_index, query_string.str(), ptr_database);
		if(query.lastError().isValid()){
				Error msg=this->set_error(8);
				ostringstream info;
				info << "Table Name                : " << this->table_name.name << endl;
				info << "Table error info          : " << query.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				throw msg;
		}
	}

	//QThread::sleep(2);

	//expand the tables file
	ostringstream file_txt;
	file_txt << sys_label::table_begin<<   "  #auto_generated"<<endl;
	file_txt << "#table name"<<endl;
	file_txt << "    " << this->table_name.id << "    " << this->get_total_table_name()<<endl;
	file_txt << "#column name"<<endl;
	for(int i=0; i< this->col_num; i++){
		file_txt << "    " << columns[i].name << "    " << functions::convert_string2lower_case(columns[i].name)<<endl;
	}
	file_txt << sys_label::table_end <<"   #auto_generated "<<endl;
	file_txt <<endl;


	ofstream table_file;
	//ios_base::openmode mode = ios_base::out 
	//open the file for reading
	table_file.open(table_file_name.c_str(), ios_base::app);
	//check the file
	if(!(table_file)){	
		//set error
		throw (set_error(1));
	}
	table_file << file_txt.str();
	table_file.close();

	return true;
}
//Get the used name of the table; needed for a database query
string Tables::get_table_name(void){
	if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		return this->table_name.name;
	}
	else if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		ostringstream buffer;
		buffer << Sys_Project::get_complete_project_database_schemata_name()<<"."<<this->table_name.name;
		return buffer.str();
	}
	else{
		return this->table_name.name;
	}
}
//Get the used name of a column with a given id-name; needed for a database query
string Tables::get_column_name(const string id){
	this->my_locker.lock();
	string buffer_name;
	for(int i=0; i< this->col_num; i++){
		if(id==this->column[i].id){
			buffer_name=this->column[i].name;
			break;
		}	
	}
	this->my_locker.unlock();
	return buffer_name;
}
///Get the used name of a column with a given id-name; needed for a database query; table name is added
string Tables::get_column_name_table(const string id) {
	this->my_locker.lock();
	string buffer_name; 
	buffer_name.append(this->table_name.name);
	buffer_name += ".";
	for (int i = 0; i < this->col_num; i++) {
		if (id == this->column[i].id) {
			buffer_name += this->column[i].name;
			break;
		}
	}
	this->my_locker.unlock();
	return buffer_name;
}
//Delete the whole data in the database table
void Tables::delete_data_in_table(QSqlDatabase *ptr_database){
	//delete the table
	QSqlQuery query(*ptr_database);
	ostringstream query_string;
	if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		query_string <<"DELETE FROM " << this->get_table_name();
	}
	else if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		query_string <<"TRUNCATE TABLE " << this->get_table_name();
	}

	bool test;
	test=ptr_database->isOpen();

	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg=this->set_error(9);
		ostringstream info;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		info << "Table name      : " << this->get_table_name() <<endl;
		info << "Query           : " << query_string.str() <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the maximum value as integer of a given column; used for evaluation the global identifier of data records
int Tables::maximum_int_of_column(const string column_name, QSqlDatabase *ptr_database){
	int max=0;
	ostringstream request;
	request << "Select max(" << column_name<<") ";
	request << " from " << this->get_table_name() ;

	QSqlQuery query(*ptr_database);
	Data_Base::database_request(&query, request.str(), ptr_database);
	query.first();
	max=query.value(0).toInt();
	if(max<0){
		max=0;
	}
	return max;
}

//Create an index to a given column in table (return value true)
void Tables::create_index2column(QSqlDatabase *ptr_database, const string column_name) {
	ostringstream request;
	request << "CREATE INDEX "<< this->table_name.name<<"_"<<column_name<<"_idx ON ";
	request << this->get_table_name()<< "("<<column_name<<")";

	QSqlQuery query(*ptr_database);
	Data_Base::database_request(&query, request.str(), ptr_database);
	query.first();

}
///Create a spatial index to a given column in table (return value true)
void Tables::create_spatial_index2column(QSqlDatabase *ptr_database, const string column_name) {
	ostringstream request;
	request << "CREATE INDEX " << this->table_name.name << "_" << column_name << "_geom_idx ON ";
	request << this->get_table_name() << " USING GIST (" << column_name << ")";

	QSqlQuery query(*ptr_database);
	Data_Base::database_request(&query, request.str(), ptr_database);
	query.first();
}
//Output the table name and column names: id-names as well as used names
void Tables::output_tab_col(void){
	this->set_output_prefix();
	ostringstream cout;
	//table name and id
	cout << "Tablename-ID " << this->table_name.id << " Tablename " << this->table_name.name << endl; 
	//number of cloumn
	cout << " Number of column " << this->col_num << endl; 
	//column name and id
	for(int i=0;i<this->col_num; i++){
		cout << " Column-ID " << this->column[i].id << " Columnname " << this->column[i].name << endl; 
	}
	Sys_Common_Output::output_system->output_txt(cout.str(), false);
	Sys_Common_Output::output_system->reset_userprefix();

}
//Set the projetc prefix (static)
void Tables::set_project_prefix(const string project_name){
	Tables::project_prefix=project_name;
	Tables::project_prefix.append("_");
}
//Add columns to a database table (static)
void Tables::add_columns(QSqlDatabase *ptr_database, const string name, const string new_column_name, const string type_column, const bool unsigened, const string default_value, _sys_table_type actuel_type){


	//set the table names
	this->table_name.id=name;
	this->table_name.name=functions::convert_string2lower_case(name);
	this->table_name.found_flag=true;

	this->tabletype=actuel_type;
	this->table_type2system_type();
	Sys_Memory_Count::self()->add_mem(sizeof(Tables), this->modul_type);

	string tab_name;
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		ostringstream buff;
		buff << Sys_Project::get_complete_project_database_schemata_name()<<"."<<this->get_total_table_name();
		tab_name=buff.str();
	}
	else{
		tab_name=this->get_total_table_name();
	}


	ostringstream query_string;
	query_string <<"ALTER TABLE ";
	query_string << tab_name << " ADD  ";
	query_string << functions::convert_string2lower_case(new_column_name) << "  " ;
		if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
			if(type_column==sys_label::tab_col_type_polyline || type_column==sys_label::tab_col_type_polygon
				|| type_column==sys_label::tab_col_type_point){
				query_string << " GEOMETRY " ;
			}
			else if(type_column==sys_label::tab_col_type_double){
				query_string << " DOUBLE PRECISION " ;
			}
			else{
				query_string << type_column << "  " ;
			}

		}
		else{
			if(type_column==sys_label::tab_col_type_polyline || type_column==sys_label::tab_col_type_polygon
				|| type_column==sys_label::tab_col_type_point){
				query_string << " GEOMETRY " ;
			}
			else{
				query_string << type_column << "  " ;
			}
		}
		if(unsigened==true && Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
			query_string << " UNSIGNED " ;
		}
		if(default_value.empty()==false){
			query_string << " DEFAULT " << default_value;
		}

	
	//end string
	query_string << "";

	//development
	//string buff_test;
	//buff_test= query_string.str();

	//set the query
	QSqlQuery query(*ptr_database);
	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Table Name                : " << this->table_name.name << endl;
			info << "Table error info          : " << query.lastError().text().toStdString() << endl;
			string buffer;
			string buffer2;
			buffer=query.lastError().text().toStdString();
			buffer2=query_string.str();
			msg.make_second_info(info.str());
			throw msg;
	}
	query_string.str("");

}
//Add columns to the project file
void Tables::add_columns_file(const string filename, const string name, const string new_column_name){
	ifstream ifile;
	string myline;
	string buffer;

	//open file for reading
	ifile.open(filename.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(11);
		ostringstream info;
		info << "Filename: " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	string table_info;
	bool table_begin=false;
	int pos=0;
	bool found=false;
	//read in file
	do{
		getline(ifile, myline,'\n');
		if(table_begin==false){
			pos=myline.find(sys_label::table_begin);
			if(pos>=0){
				table_begin=true;
			}
		}
		pos=-1;
		if(table_begin==true){
			buffer=myline;
			functions::clean_string(&buffer);
			pos=buffer.find(name);
			if(pos>=0 && found==false){
				found=true;
				myline=buffer;
				myline.append("\n");
				myline.append("    ");
				myline.append(new_column_name);
				myline.append("    ");
				myline.append(functions::convert_string2lower_case(new_column_name));
				myline.append("  #Added column ");
			}
		}
		table_info.append(myline);
		table_info.append("\n");
	}
	while(ifile.eof()==false);
	ifile.close();

	if(table_info.find_last_of("\n")==table_info.length()-1){
		table_info.erase(table_info.length()-1);
	}

	ofstream ofile;
	//open for writing
	ofile.open(filename.c_str(), ios_base::out);
	if(ofile.is_open()==false){
		Error msg=this->set_error(11);
		ostringstream info;
		info << "Filename: " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	ofile << table_info;
	ofile.flush();
	//close file
	ofile.close();
}
//Get the number of columns
int Tables::get_number_col(void){
	return this->col_num;
}
//Get the pointer to the columns
_sys_id_name* Tables::get_ptr_col(void){
	return this->column;
}
//_______________________________________
//private
//Allocate the column _sys_id_name structure for the columns of the database table
void Tables::allocate_columns(void){
	//create a new array for the columns
	try{
		this->column=new _sys_id_name[this->col_num];

	}
	catch(bad_alloc&){
		Error msg=this->set_error(7);
		throw msg;
	}
	//init it
	for(int i=0; i<this->col_num; i++){
		this->column[i].id=label::not_set;
		this->column[i].name=label::not_set;	
		this->column[i].found_flag=false;
	}

}
//Return the user specifed name following the keyword (expression), which is found in the table-file after the id-name
string Tables::found_string(const string expression, const string search_string){
	string buffer_begin=search_string;
	int pos=0;
	string buffer=search_string;
	string result="";
	bool found_flag_exp=false;
	bool found_flag_res=false;

		do{
			pos=buffer.find(" ");
			//search for tabs
			if(pos<0){
				pos=buffer.find("\t");
			}
			//devide keyword and following string
			if(pos>=0){
				//begin of search string, normally the keyword
				buffer_begin=buffer.substr(0,pos);
				//rest of search string, normally the result string
				buffer=buffer.substr(pos+1,buffer.size());
			}
			else{
				buffer="";
			}
			//compare to the keyword
			if(buffer_begin==expression){
				//found the keyword
				found_flag_exp=true;
				functions::clean_string(&buffer);
				if(buffer.empty()==false){
					//found the result string 
					result=buffer;
					found_flag_res=true;
				}
				else{
					found_flag_res=false;
				}
		
			}
		//end when buffer size is reached or all is found
		}while(buffer.size()!=0 && (found_flag_res==false && found_flag_exp==false));

		//keyword is found but no result string after
		if (found_flag_exp==true && found_flag_res==false){
			Error msg=this->set_error(2);
			stringstream info;
			info << "Expression: " << expression << endl;
			msg.make_second_info(info.str());
			throw(msg);
		}
return result;
}

//Search for a keyword (expression) in the table-file (here the id-names); return true, when the id-name is founded in the table-file
bool Tables::found_expression(const string expression, const string search_string){
	string buffer_begin=search_string;
	int pos=0;
	string buffer=search_string;
	bool found_flag_exp=false;
	

		do{
			pos=buffer.find(" ");
			//search for tabs
			if(pos<0){
				pos=buffer.find("\t");
			}
			if(pos>=0){//if a whitespace is find
				buffer_begin=buffer.substr(0,pos);//begin of search string:= key word
				buffer=buffer.substr(pos+1,buffer.size());//rest of search:= user specifies name
			}
			else{
				buffer="";
			}
			//clean the string
			functions::clean_string(&buffer_begin);
			if(buffer_begin==expression){
				//found the keyword
				found_flag_exp=true;
			}
		//end when buffer size is reached or all is found
		}while(buffer.size()!=0 && (found_flag_exp==false));

return found_flag_exp;
}
//Check, if the table, specified by the table name, exists in the database 
bool Tables::check_names(QSqlDatabase *ptr_database){	

	QStringList tables;
	Data_Base::database_tables(&tables, ptr_database);
	QSqlRecord column;
	bool ok_flag=false;
	string buffer_name;
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		buffer_name=Sys_Project::get_complete_project_database_schemata_name();
		buffer_name.append(".");
		buffer_name.append(table_name.name);
	}
	else{
		buffer_name=table_name.name;
	}
	
	for(int i=0; i<tables.size();i++){
		if(tables.at(i).toStdString()==buffer_name){
			//get the column names in the database table
			//column=(ptr_database->record(tables.at(i)));
			//Data_Base::database_table_columns(&column, tables.at(i), ptr_database); //replace function
			Data_Base::database_table_columns_query(&column, Sys_Project::get_complete_project_database_schemata_name() , table_name.name, ptr_database);

			this->table_name.found_flag=true;
			try{
				if(this->check_column_names(column)){
					//all columns are found
					ok_flag=true;
				};
			}
			catch(Error msg){
				throw msg;
			}
		}
	}
	//the table is not found
	if(!this->table_name.found_flag){
		Error msg=set_error(4);
		throw msg;
	}

return ok_flag;
}
//Check, if the columns of the table, specified by the column names, exist in the database 
bool Tables::check_column_names(const QSqlRecord column_name){
	int counter=0;
	int find_counter=0;
	bool column_ok=false;
		do{
			for(int i=0; i< this->col_num; i++){
				if (this->column[i].name==column_name.fieldName(counter).toStdString()){
					this->column[i].found_flag=true;
					find_counter++;
					break;
				}
			}
		counter++;
		}
		while(counter<column_name.count());

		if(find_counter==col_num){
			//all is found
			column_ok=true;
		}
		else{
			Error msg=this->set_error(3);
			throw(msg);
		}
	return column_ok;
}

//Convert the enumerator _sys_table_type to a string
string Tables::convert_table_type2string(const _sys_table_type type){
	string buff;

	switch(type){
		case (_sys_table_type::hyd):
			buff=sys_label::str_hyd;
			break;
		case (_sys_table_type::fpl):
			buff=sys_label::str_fpl;
			break;
		case (_sys_table_type::madm):
			buff=sys_label::str_madm;
			break;
		case (_sys_table_type::dam):
			buff=sys_label::str_dam;
			break;
		case (_sys_table_type::cost):
			buff=sys_label::str_cost;
			break;
		case (_sys_table_type::alt):
			buff=sys_label::str_alt;
			break;
		default:
			buff=label::unknown_type;
	}
	return buff;
}
//Get the total table name completly with project-prefix and type-prefix; the name is in the lower case
string Tables::get_total_table_name(void){
	ostringstream buff;

	if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		buff << Sys_Project::get_complete_project_database_table_prefix_name();
	}
	buff << functions::convert_string2lower_case(this->table_name.name);
	buff << this->table_suffix;


	return buff.str();
}
//Set the prefix for the output
void Tables::set_output_prefix(void){
	switch(this->tabletype){
		case _sys_table_type::fpl:
			Sys_Common_Output::output_system->set_userprefix("FPL_TAB> ");
			break;
		case _sys_table_type::hyd:
			Sys_Common_Output::output_system->set_userprefix("HYD_TAB> ");
			break;
		case _sys_table_type::madm:
			Sys_Common_Output::output_system->set_userprefix("MADM_TAB> ");
			break;
		case _sys_table_type::dam:
			Sys_Common_Output::output_system->set_userprefix("DAM_TAB> ");
			break;
		case _sys_table_type::risk:
			Sys_Common_Output::output_system->set_userprefix("RISK_TAB> ");
			break;
		default:
			Sys_Common_Output::output_system->set_userprefix("NO_TAB> ");
			break;
	}

}
//Table type to system type
void Tables::table_type2system_type(void){
	switch(this->tabletype){
		case _sys_table_type::fpl:
			this->modul_type=_sys_system_modules::FPL_SYS;
			break;
		case _sys_table_type::hyd:
			this->modul_type=_sys_system_modules::HYD_SYS;
			break;
		case _sys_table_type::madm:
			this->modul_type=_sys_system_modules::MADM_SYS;
			break;
		case _sys_table_type::dam:
			this->modul_type=_sys_system_modules::DAM_SYS;
			break;
		case _sys_table_type::risk:
			this->modul_type=_sys_system_modules::RISK_SYS;
			break;
		case _sys_table_type::alt:
			this->modul_type=_sys_system_modules::ALT_SYS;
			break;
		case _sys_table_type::cost:
			this->modul_type=_sys_system_modules::COST_SYS;
			break;
		default:
			this->modul_type=_sys_system_modules::SYS_SYS;
			break;
	}
}
//Set error(s)
Error Tables::set_error(const int err_type){
		string place="Tables::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 1:
			//external msg
			place.append("set_name(const string file_name)/create_non_existing_tables(..)");
			reason="Can not open file" ;
			help="Check the filename";
			info <<" Filename : " << table_file_name << endl;
			fatal=true;//fatal error;
			type=5;
			break;
		case 2:
			//external msg
			place.append("found_string(const string expression, const string search_string)");
			reason="No result is found to the expression";
			help="Check the file and the expression";
			info << "Filename  : " << table_file_name << endl;
			info << "Tablename : " << table_name.name << endl;
			fatal=true;//fatal error;
			type=8;
			break;
		case 3:
			place.append("check_column_names(const QSqlRecord column_name)");
			reason="Not all columns found";
			help="Check the column names in the file and the column names in the database of the table";
			info << "Filename         : " << table_file_name << endl;
			info << "Tablename        : " << table_name.name << endl;
			info << "Not found columns: " <<endl; 
			for(int i=0; i< this->col_num; i++){
				if(this->column[i].found_flag==false){
					info <<" ID-name: "<< this->column[i].id << "; User-name: " << this->column[i].name << endl;
				}
			}
			fatal=true;//fatal error;
			type=8;
			break;
		case 4:
			place.append("check_names(QSqlDatabase *ptr_database)");
			reason="Table is not found";
			help="Check the table name in the file and the table name in the database";
			info << "Filename   : " << table_file_name << endl;
			info << "Tablename  : " << table_name.name << endl;
			info << "ID-name    : " << table_name.id << endl;
			fatal=true;//fatal error;
			type=8;
			break;
		case 5:
			place.append("delete_all_rows(....)");
			reason="Table is not found; can not delete the rows";
			help="Check the table ";
			info << "Tablename : " << table_name.name << endl;
			fatal=true;//fatal error;
			type=9;
			break;
		case 6:
			place.append("show_table(....)");
			reason="Table is not found; can not display the table";
			help="Check the table ";
			info << "Tablename : " << table_name.name << endl;
			type=9;
			break;
		case 7:
			place.append("Tables(const string id_name, const string id_column[], const int column_number)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 8://can not create table
			place.append("create_non_existing_tables(const string name, _Sys_data_tab_column columns[], const int column_number, QSqlDatabase *ptr_database, _sys_table_type actuel_type)");
			reason="Can not create a new database table";
			help="Check the database";
			type=2;
			break;
		case 9://can not delete table
			place.append("delete_data_in_table(QSqlDatabase *ptr_database)");
			reason="Can not delete the data in the table";
			help="Check the database";
			type=2;
			break;
		case 10://can not add column to table
			place.append("add_columns(QSqlDatabase *ptr_database, const string name, const string new_column_name, const string type_column, const bool unsigened, const string default_value)");
			reason="Can not add a new column to database table";
			help="Check the database";
			type=2;
			break;
		case 11://can open file
			place.append("add_columns_file(const string filename, const string name, const string new_column_name)");
			reason="Can not open the project file";
			help="Check the file";
			type=5;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place,reason,help,type,fatal);
	msg.make_second_info(info.str());

return msg;
}
//Set the warning
Warning Tables::set_warning(const int warn_type){
	string place="Tables::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		//case 0://can not delete table
		//	place.append("delete_data_in_table(QSqlDatabase *ptr_database)");
		//	reason="Can not delete the data in the table";
		//	help="Check the database";
		//	type=2;
		//	break;
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