#include "Dam_Headers_Precompiled.h"
//#include "Dam_Sc_Subcategory.h"

//init static members
Tables *Dam_Sc_Subcategory::table=NULL;

//Default constructor
Dam_Sc_Subcategory::Dam_Sc_Subcategory(void){
	this->name=label::not_set;
	this->index=-1;
	this->affected_score=0.0;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Sc_Subcategory), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Sc_Subcategory::~Dam_Sc_Subcategory(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Sc_Subcategory), _sys_system_modules::DAM_SYS);
}
//________
//public
//Transfer the subcategory data to a database
void Dam_Sc_Subcategory::transfer_input_members2database(QSqlDatabase *ptr_database){

	//mysql query with the query-class
	QSqlQuery model(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Subcategory::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	
	//evaluate the global identifier		
	//set the query via a query string
	ostringstream query_string;
	query_string << "INSERT INTO  " << Dam_Sc_Subcategory::table->get_table_name();
	query_string <<" ( ";
	query_string <<Dam_Sc_Subcategory::table->get_column_name(dam_label::subcategory_id) <<" , ";
	query_string << Dam_Sc_Subcategory::table->get_column_name(dam_label::category_name) <<" ) ";
	


	query_string << " VALUES ( ";
	query_string << this->index << " , " ;
	query_string <<"'"<< this->name<< "' ) " ;


	
	Data_Base::database_request(&model, query_string.str(), ptr_database);

	if(model.lastError().isValid()){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Table Name                : " << Dam_Sc_Subcategory::table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Subcategory Id            : " << this->index<< endl;
		msg.make_second_info(info.str());
		throw msg;	
	}
}
//Get the name of the subcategory
string Dam_Sc_Subcategory::get_subcategory_name(void){
	return this->name;
}
//Get the index of the subcategory
int Dam_Sc_Subcategory::get_id(void){
	return this->index;
}
//Input the simple counting subcategory data per database
void Dam_Sc_Subcategory::input_subcategory_perdatabase(const QSqlQueryModel *results, const int glob_index){

	this->index=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::subcategory_id)).c_str()).toInt();
	this->name=results->record(glob_index).value((Dam_Sc_Point::point_table->get_column_name(dam_label::category_name)).c_str()).toString().toStdString();
}
//Output the members
void Dam_Sc_Subcategory::output_members(ostringstream *cout){

	*cout << " " << this->index << "   "<< this->name << endl;

}
//Set members
void Dam_Sc_Subcategory::set_members(const int id, const string name){
	this->index=id;
	this->name=name;
}
//Add to affected score
void Dam_Sc_Subcategory::add_affected_score(const double add){
	this->affected_score=this->affected_score+add;
}
//Reset affected score
void Dam_Sc_Subcategory::reset_affected_score(void){
	this->affected_score=0.0;
}
//Get affected score
double Dam_Sc_Subcategory::get_affected_score(void){
	return this->affected_score;
}
//Create the database table for simple counting subcategories (static)
void Dam_Sc_Subcategory::create_table(QSqlDatabase *ptr_database){
		if(Dam_Sc_Subcategory::table==NULL){
			ostringstream cout;
			cout << "Create simple-counting subcategory database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_sc_subcategory;
			const int num_col=3;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=dam_label::subcategory_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::category_name;
			tab_col[1].type=sys_label::tab_col_type_string;

			tab_col[2].name=label::description;
			tab_col[2].type=sys_label::tab_col_type_string;


			try{
				Dam_Sc_Subcategory::table= new Tables();
				if(Dam_Sc_Subcategory::table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Sc_Subcategory::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Sc_Subcategory::close_table();
				throw msg;
			}

		Dam_Sc_Subcategory::close_table();
	}
}
//Set the database table for simple counting subcategories: it sets the table name and the name of the columns and allocate them (static)
void Dam_Sc_Subcategory::set_table(QSqlDatabase *ptr_database){
	if(Dam_Sc_Subcategory::table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_sc_subcategory;
		string tab_col[3];

		tab_col[0]=dam_label::subcategory_id;
		tab_col[1]=dam_label::category_name;
		tab_col[2]=label::description;


		try{
			Dam_Sc_Subcategory::table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Sc_Subcategory::table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Sc_Subcategory::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Sc_Subcategory::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for simple counting subcategories (static)
void Dam_Sc_Subcategory::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Subcategory::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Sc_Subcategory::table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for simple counting subcategories (static)
void Dam_Sc_Subcategory::close_table(void){
	if(Dam_Sc_Subcategory::table!=NULL){
		delete Dam_Sc_Subcategory::table;
		Dam_Sc_Subcategory::table=NULL;
	}
}
///Select and count the number of relevant subcategories in a database table (static)
int Dam_Sc_Subcategory::select_relevant_subcategory_database(QSqlQueryModel *results, QSqlDatabase *ptr_database){
	int number=0;
	try{
		Dam_Sc_Subcategory::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter <<  Dam_Sc_Subcategory::table->get_column_name(dam_label::subcategory_id) << " , ";
	test_filter <<  Dam_Sc_Subcategory::table->get_column_name(dam_label::category_name) << "  ";
	
	test_filter << " from " << Dam_Sc_Subcategory::table->get_table_name();
	test_filter << " order by " <<  Dam_Sc_Subcategory::table->get_column_name(dam_label::subcategory_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Sc_Subcategory::select_relevant_subcategory_database(QSqlQueryModel *results, QSqlDatabase *ptr_database)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Sc_Subcategory::table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	number= results->rowCount();


	return number;

}
//________
//private
//Set error(s)
Error Dam_Sc_Subcategory::set_error(const int err_type){
	string place="Dam_Sc_Subcategory::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_raster(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://transferring problems of data to database
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not transfer the data of the subcategories to database";
			help="Check the subcategories and the database";
			type=4;
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
