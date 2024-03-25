#include "Sys_Headers_Precompiled.h"
//#include "Sys_Project_Copier.h"

Sys_Project_Copier::Sys_Project_Copier(void){

	this->ptr_database=NULL;
	this->new_project_name=label::not_set;
	this->old_project_name=label::not_set;

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Project_Copier), _sys_system_modules::SYS_SYS);
}

Sys_Project_Copier::~Sys_Project_Copier(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_Project_Copier), _sys_system_modules::SYS_SYS);
}
//_______________
//public
//Run the thread
void Sys_Project_Copier::run(void){
	ostringstream prefix;
	ostringstream cout;

	if(this->ptr_database==NULL ){
		cout<<  "No database connection is set. To copy is not possible!" <<endl;
		Sys_Common_Output::output_system->output_txt(&cout,false);
	}

	prefix <<"COPY> ";	
	Sys_Common_Output::output_system->set_userprefix(prefix.str());

	cout<<  "Copy project database table " <<this->old_project_name<< " to new project database table " << this->new_project_name <<"..."<<endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);

	try{
		this->copy_relevant_database_tables(this->ptr_database, this->new_project_name);
	}
	catch(Error msg){
		msg.output_msg(0);
	}


	Sys_Common_Output::output_system->reset_prefix_was_outputed();
	cout<<  "Project database copy is finished!"<<endl;
	Sys_Common_Output::output_system->output_txt(&cout,false);

}
//Set the members for copying
void Sys_Project_Copier::set_copy_member(Data_Base *ptr_database, const string new_project, const string old_project){
	this->ptr_database=ptr_database;
	this->new_project_name=new_project;
	this->old_project_name=old_project;
}
//Copy relevant database tables
void Sys_Project_Copier::copy_relevant_database_tables(Data_Base *ptr_database, string new_project_name){

	///find name of project tables in database
	QSqlQuery model(*(ptr_database->get_database()));

	QStringList tables;
	Data_Base::database_tables(&tables, ptr_database->get_database());

	string table_start=this->old_project_name;
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		QSqlQuery my_query(*ptr_database->get_database());
		ostringstream filter;
		new_project_name.append("_prm");
		filter << "CREATE SCHEMA " << new_project_name;
		my_query.exec(filter.str().c_str());
		table_start=Sys_Project::get_complete_project_database_schemata_name();

	}



	ostringstream cout;
	string new_table;
	string old_table;
	//copy all tables
	for(int i=0; i<tables.size(); i++){
		if(tables.at(i).startsWith(table_start.c_str())==true){
			old_table=tables.at(i).toStdString();
			new_table=old_table.substr(table_start.size());
			new_table.insert(0,new_project_name);


			cout<<  "Copy database table " <<tables.at(i).toStdString() <<" to database table " << new_table.c_str() <<endl;
			Sys_Common_Output::output_system->output_txt(&cout,false);
			this->copy_one_database_table(old_table, new_table, &model, ptr_database);
		}

	}
}
//Copy one database table
void Sys_Project_Copier::copy_one_database_table(const string src, const string dest, QSqlQuery *model, Data_Base *ptr_database){


	ostringstream test_filter;
	if(Data_Base::get_driver_type()==_sys_driver_type::POSTGRESQL){
		test_filter<< "CREATE TABLE "<< dest << " (LIKE "<<src<<" INCLUDING INDEXES INCLUDING DEFAULTS) ;";
		test_filter<< "INSERT INTO "<< dest << " SELECT * FROM "<<src<<";";
	}
	else if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		test_filter<< "CREATE TABLE "<< dest << " LIKE "<<src<<" ";
	}
	
	
	//submit it to the datbase
	Data_Base::database_request(model, test_filter.str(), ptr_database->get_database());
	if(model->lastError().isValid()==true){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Table error info: " << model->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(Data_Base::get_driver_type()==_sys_driver_type::MYSQL){
		test_filter.str("");
		test_filter<< "INSERT INTO "<< dest << " SELECT * FROM "<<src<<"";
		Data_Base::database_request(model, test_filter.str(), ptr_database->get_database());
		if(model->lastError().isValid()==true){
			Error msg=this->set_error(0);
			ostringstream info;
			info << "Table error info: " << model->lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Set error(s)
Error Sys_Project_Copier::set_error(const int err_type){
	string place="Sys_Project_Copier::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){

		case 0://wrong sql syntax
			place.append("copy_one_database_table(const string src, const string dest, QSqlQueryModel *model, Data_Base *ptr_database)");
			reason="Invalid database request";
			help="Check the database";
			type=2;
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