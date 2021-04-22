//#include "Data_Base.h"
#include "Sys_Headers_Precompiled.h"

//init static members
QMutex Data_Base::my_locker;
_sys_driver_type Data_Base::driver_type=_sys_driver_type::UNKNOWN_DR;
bool Data_Base::reconnect_flag=true;


//Default constructor
Data_Base::Data_Base(void){
	this->connection_ok=false;
	this->host_name=label::not_set;
	this->database_name=label::not_set;
	this->user_name=label::not_set;
	this->driver_name=label::not_set;
	this->password=label::not_set;
	this->password_save_flag=true;
	this->file_name_connection=label::not_set;



	Sys_Memory_Count::self()->add_mem(sizeof(Data_Base), _sys_system_modules::SYS_SYS);//count the memory
}
//Constructor with a given filename, where the database connection parameters are set
Data_Base::Data_Base(const string con_filename){
	
	this->connection_ok=false;
	this->host_name=label::not_set;
	this->database_name=label::not_set;
	this->user_name=label::not_set;
	this->password=label::not_set;
	this->password_save_flag=true;
	this->driver_name=label::not_set;
	this->file_name_connection=con_filename;
	
	//set the input for connection
	try{
		read_input();
	}
	catch(Error msg){
		throw msg;
	}

	Sys_Memory_Count::self()->add_mem(sizeof(Data_Base), _sys_system_modules::SYS_SYS);//count the memory

}
//Constructor with a given database connection parameters
Data_Base::Data_Base(const string hostname, const string databasename, const string username, const string pass_word, const string driver){
	//set the members
	this->connection_ok=false;
	this->host_name=hostname;
	this->database_name=databasename;
	this->user_name=username;
	this->password=pass_word;
	this->password_save_flag=true;
	this->driver_name=driver;
	this->file_name_connection="input_by_hand";



	//open database
	Sys_Memory_Count::self()->add_mem(sizeof(Data_Base), _sys_system_modules::SYS_SYS);//count the memory
}
//Constructor with a given database connection parameters as structure
Data_Base::Data_Base(const _sys_database_params param){
	//set the members
	this->connection_ok=false;
	this->host_name=param.host_name;
	this->database_name=param.database_name;
	this->user_name=param.user_name;
	this->password=param.password;
	this->password_save_flag=param.password_saved;
	this->driver_name=param.driver_name;
	this->driver_type=param.driver_type;
	this->file_name_connection="input_by_hand";




	//open database
	Sys_Memory_Count::self()->add_mem(sizeof(Data_Base), _sys_system_modules::SYS_SYS);//count the memory

}
//Default destructor
Data_Base::~Data_Base(void){
			

	Sys_Memory_Count::self()->minus_mem(sizeof(Data_Base), _sys_system_modules::SYS_SYS);//count the memory
	
}
//_______________________________________
//public
//Decide, which database action should be done (console dialog)
void Data_Base::action(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;
	try{
		do{
			//output requriered mem
			Sys_Memory_Count::self()->output_mem();
			//read in which action
			do{
				cout << endl;
				cout << "=======================================================================" <<endl;
				cout << "DATABASE-MENU " << endl;
				cout << " Which database action do you want to make ?" << endl;
				cout << "  (0) Go back to the SYTEM-MENU " << endl;
				cout << "  (1) Set the Database " << endl;
				cout << "  (2) Show Connection Parameters " << endl;
				cout << "  (3) Show Status of database " << endl;
				cout << "  (4) Close the database " << endl;
				cin >> action_number;
				if((flag=(cin.fail()))){//check if the input is valif
						cout << "Wrong input! Do it again" << endl;
						cin.clear();//clear the error flags
						cin.ignore();//clear cin
				}
				else if(action_number < 0 || action_number>4){
					cout << "Wrong number! Do it again! " << endl;
				}
			}
			while(flag || action_number<0 || action_number>4);

			//make the action
			switch(action_number){
				case 0:
					stop_flag=true;
					break;
				case 1:
					cout << "=======================================================================" <<endl;
					cout << " You want to connect to the database!"<<endl;
					this->set_input();
					break;
				case 2:
					cout << "=======================================================================" <<endl;
					cout << " Show the connection parameters:"<<endl;
					this->output_members();
					break;
				case 3:
					cout << "=======================================================================" <<endl;
					cout << " Show the status of the database"<<endl;
					this->output_con_status();
					break;
				case 4:
					cout << "=======================================================================" <<endl;
					cout << " The database will be closed"<<endl;
					this->close_database();
					break;
			}
		}
		while(!stop_flag);
	}
	catch(Error msg){
		throw msg;
	}
}
//Decide how the database connection parameters are set (console dialog)
void Data_Base::set_input(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;
	try{
		do{
			//read in which action
			do{
				cout << endl;
				cout << "=======================================================================" <<endl;
				cout << "DATABASE-CONNECT-MENU " << endl;
				cout << " Which database action do you want to make ?" << endl;
				cout << "  (0) Go back to the DATABASE-MENU " << endl;
				cout << "  (1) You want to set the connection paramter by hand " << endl;
				cout << "  (2) You want to set the connection paramter per file " << endl;
				cout << "  (3) Default !!just for the development of the program!! " << endl;
				cin >> action_number;
				if((flag=(cin.fail()))){//check if the input is valif
						cout << "Wrong input! Do it again" << endl;
						cin.clear();//clear the error flags
						cin.ignore();//clear cin
				}
				else if(action_number < 0 || action_number>3){
					cout << "Wrong number! Do it again! " << endl;
				}
			}
			while(flag || action_number<0 || action_number>4);

			switch(action_number){
				case 0:
					stop_flag=true;
					break;
				case 1://per console
					//this->set_input_byhand();
					stop_flag=true;
					break;
				case 2://per file
					this->set_input_byfile();
					stop_flag=true;
					break;
				case 3://default just for development
					this->set_input_development();
					stop_flag=true;
					break;
			}
		}
		while(!stop_flag);	
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the driver text of the database (static)
_sys_driver_type Data_Base::get_driver_type(void){
	return driver_type;
}
//Output the database connection parameters
void Data_Base::output_members(void){
	this->set_output_prefix();
	int str_length;
	string buffer;

	ostringstream cout;
	cout <<"CONNECTION PARAMETERS TO DATABASE " << this->database_name << endl;
	cout <<" Hostname                    : " << this->host_name << endl;
	cout <<" User name                   : " << this->user_name <<endl;
	cout <<" Driver name                 : " << this->driver_name <<endl;
	cout <<" Name of the connection file : "  << this->file_name_connection << endl;
	cout <<" Save password               : "  << functions::convert_boolean2string(this->password_save_flag) << endl;

	//output the password
	str_length=this->password.length();
	cout <<" Password                    : " ;

	//print x 
	for(int i=0; i<str_length-2;i++){
		cout << "x"; 
	}
	
	buffer=this->password.substr(str_length-2,str_length);
	cout << buffer << endl;
	cout <<" Name of the tables file     : " << Tables::get_table_file_name() << endl;
	cout <<" Connections option(s)       : " << this->database.connectOptions().toStdString() << endl;
	Sys_Common_Output::output_system->output_txt(&cout);
	Sys_Common_Output::output_system->rewind_userprefix();
}
//Output the database connection status (true:= database is connected; false:=database is not connected)
void Data_Base::output_con_status(void){
	this->set_output_prefix();
	bool flag=this->database.isOpen();
	ostringstream cout;
	cout << " Status of connection to database " << this->database_name << " (see also status bar) : ";
	cout << ((flag==true)?("connected"):("not connected")) << endl;
	Sys_Common_Output::output_system->output_txt(&cout, false);
	Sys_Common_Output::output_system->rewind_userprefix();
}
//Check and return database connection status (true:= database is connected; false:=database is not connected)
bool Data_Base::check_con_status(void){
	connection_ok=this->database.isOpen();
	return connection_ok;
}
//Get a pointer to the QSqlDatabase
QSqlDatabase* Data_Base::get_database(void){
	return &(this->database);
	//return &(QSqlDatabase::database(this->database_name.c_str()));
};
//Close the database connection
void Data_Base::close_database(void){
	if(this->database.isOpen()==false){
		return;
	}

	this->set_output_prefix();
	ostringstream cout;
	//close database
	if(this->database.isOpen()==true){
		cout << " Connection to database " << this->database_name << " is closed!" <<endl;
	}
	else{
		cout << " No database connection was established  " <<endl;
	}
	this->database.close();
	//reset the database object
	this->database=QSqlDatabase();
	QSqlDatabase::removeDatabase(this->database_name.c_str());

	//reset all con parameters
	this->connection_ok=false;
	this->host_name="not_set";
	this->database_name="not_set";
	this->user_name="not_set";
	this->password="not_set";
	this->file_name_connection="not_set";
	Tables::set_file_name(label::not_set);
	this->database.setConnectOptions();
	Sys_Common_Output::output_system->output_txt(&cout, true);
	Sys_Common_Output::output_system->rewind_userprefix();
}
//Refresh the database connection
void Data_Base::refresh_connection(void){
		this->database.close();
		this->database.open();
}
//Set the database connection parameters via console
void Data_Base::set_input_byhand(void){
	try{
		this->set_output_prefix();
		cout << endl;
		cout << "=======================================================================" <<endl;
		cout << "Give the parameters for the database connection!" << endl;
		cout << "Give the hostname! " << endl;
		cin >> this->host_name;
		cout << "Give the database name!" << endl;
		cin >> this->database_name;
		cout << "Give the username!" << endl;
		cin >> this->user_name;
		cout << "Give the password!" << endl;
		cin >> this->password;
		cout << "Give the driver name (possible QMYSQL or QPSQL)!" << endl;
		cin >> this->driver_name;
		Sys_Common_Output::output_system->rewind_userprefix();
		Tables::set_file_name();
		this->open_database();

	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via dialog
void Data_Base::set_input_dia_all(void){
	try{
		//create the dia
		Sys_Database_Dia database_dialog;
		//execute the dia
		if(database_dialog.exec()){//if ok set the parameter	
			this->host_name=database_dialog.host_name().toStdString();
			this->database_name=database_dialog.database_name().toStdString();
			this->user_name=database_dialog.user_name().toStdString();
			this->password=database_dialog.password().toStdString();
			this->password_save_flag=database_dialog.password_save_flag();
			this->driver_name=database_dialog.get_driver().toStdString();
			Tables::set_file_name(database_dialog.table_dir().toStdString());
			this->open_database();
		}		
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via a file; the filename/path is asked via console
void Data_Base::set_input_byfile(void){

	this->set_output_prefix();
	cout << endl;
	cout << "=======================================================================" <<endl;
	cout << "Give the filename for the database connection!" << endl;
	cin >> file_name_connection;

	Tables::set_file_name("./system/data_file/tables.tab");
	Sys_Common_Output::output_system->rewind_userprefix();
	try{
		this->read_input();
		this->open_database();

	}
	catch(Error msg){
		Sys_Common_Output::output_system->rewind_userprefix();
		throw msg;
	}
}
//Set the database connection parameters via a file; the filename/path is asked via dialog
void Data_Base::set_input_file_dialog(void){
	try{
		//create the dialog
		Sys_Database_File_Dia file_dialog;
		//execute the dialog
		if(file_dialog.exec()){//if ok set the parameter	
			this->file_name_connection = file_dialog.db_file_dir().toStdString();
			this->read_input();//sehr wichtig, connection parameters einlesen
			Tables::set_file_name(file_dialog.table_file_dir().toStdString());
			this->open_database();	
		}		
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via a file; the filename/path is implemented (it is just for development)
void Data_Base::set_input_development(void){
	try{
		//file_name_connection="./data_file/db_psql.con";
		//Tables::set_file_name("./data_file/tables_psql.tab");
		//file_name_connection="./data_file/db.con";
		//Tables::set_file_name("./data_file/tables.tab");
		file_name_connection="./data_file/db_rur_mysql.con";
		Tables::set_file_name("./data_file/tables_rur_mysql.tab");
		read_input();
		this->open_database();
		
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via the given structure
void Data_Base::set_input_given(_sys_database_params given){
	try{
		this->host_name=given.host_name;
		this->database_name=given.database_name;
		this->user_name=given.user_name;
		this->password=given.password;
		this->driver_name=given.driver_name;
		this->password_save_flag=given.password_saved;
		this->open_database();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via the given structure, additional the table file name is given
void Data_Base::set_input_given(_sys_database_params given, const string file_name){
	try{
		this->host_name=given.host_name;
		this->database_name=given.database_name;
		this->user_name=given.user_name;
		this->password=given.password;
		this->driver_name=given.driver_name;
		this->password_save_flag=given.password_saved;
		this->open_database();
		Tables::set_file_name(file_name);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the database connection parameters via a file, which is given
void Data_Base::set_input_byfile(const string file_name){

	try{
		
		this->file_name_connection = file_name;
		this->read_input();
		Tables::set_file_name(file_name);
		this->open_database();
	}
	catch(Error msg){
		throw msg;
	}

}
//Set a query to the database via QSqlTableModel (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_request(QSqlTableModel *table_model){

	my_locker.lock();
	table_model->select();
    QSqlDatabase ptr_database;
    ptr_database=table_model->database();
    if(table_model->lastError().isValid()==true && ptr_database.isOpen()==false){
        Data_Base::database_check_connection(&ptr_database);
		//renew the sql-command
		if(table_model->lastError().type()==3){
			table_model->select();
		}
	}
	my_locker.unlock();
}
//Set a query to the database via QSqlTableModel and a qiven query string (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_request(QSqlQuery *query, string querystring, QSqlDatabase *ptr_database){

	my_locker.lock();
	query->exec(querystring.c_str());
	
	if(query->lastError().isValid()==true && ptr_database->isOpen()==false){
		

		Data_Base::database_check_connection(ptr_database);
		//renew the sql-command
		if(query->isSelect()==true){
			query->clear();
			query->exec(querystring.c_str());
		}
		else{
			query->clear();
		}
		
	}
	my_locker.unlock();
}
//Set a query to the database via QSqlTableModel, a qiven query string and a pointer to the database (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_request(QSqlQueryModel *query_model, string querystring, QSqlDatabase *ptr_database){

	my_locker.lock();
	query_model->setQuery(querystring.c_str(), *ptr_database);
	if(query_model->lastError().isValid()==true && ptr_database->isOpen()==false ){
		
		

		Data_Base::database_check_connection(ptr_database);
		//renew the sql-command
		
		if(query_model->query().isSelect()==true){
			query_model->clear();
			query_model->setQuery(querystring.c_str(), *ptr_database);
		}
		else{
			query_model->clear();
		}

	}
	my_locker.unlock();
}
//Check the database connection in case of an error (static)
void Data_Base::database_check_connection(QSqlDatabase *ptr_database){


	if(ptr_database->isOpen()==false){
		time_t start;
		time_t current;
		time_t diff;
		time(&start);
		QString buffer;
		buffer=Sys_Output_Division::set_time().c_str();
	
		bool flag=false;
		int counter=0;
		double reconnect_time=3.0;
		ostringstream cout;
		ostringstream prefix;
		prefix<<"DB> ";
		//wait loop
		do{
			time(&current);
			diff=current-start;
			//try reconnect every 30 sec
			if(diff>(reconnect_time)){
				counter++;
				Sys_Common_Output::output_system->set_userprefix(prefix.str());
				cout << "Try to reconnect to database "<< ptr_database->databaseName().toStdString()<< " the " << counter << " time at " <<ctime(&current);
				Sys_Common_Output::output_system->output_txt(&cout);
				//close the database
				ptr_database->close();
				//try to open it
				ptr_database->open();
				//reset start time
				time(&start);
				//check if reconnect was succesfully
				if(ptr_database->isOpen()==true){
					flag=true;
					cout << "Reconnection to database "<< ptr_database->databaseName().toStdString()<< " was successfully!"<<endl;
					Sys_Common_Output::output_system->output_txt(&cout);
				}
				//open the reconnection window
				if(counter==50 && flag==false){
					Sys_Exception_Sender::self()->set_open_reconnection_dia(buffer);
				}
				//increase time interval for checking
				if(counter>500){
					break;
				}
				else if(counter>10){
					reconnect_time=30.0;
				}
				else if(counter>100){
					reconnect_time=60.0;
				}

				Sys_Common_Output::output_system->rewind_userprefix();
			}
		}
		while(flag==false && Data_Base::reconnect_flag==true);

		if(flag==false){
			Data_Base::reconnect_flag=true;
			Error msg;
			msg.set_msg("Data_Base::database_check_connection(QSqlDatabase *ptr_database)","There is no connection to the database. Try to reconnect 10 times!", "Check the database connection", 4, true);
			ostringstream info;
			info<< "Connection name: " << ptr_database->connectionName().toStdString() << endl;
			info<< "Database name  : " << ptr_database->databaseName().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		else{
			Sys_Exception_Sender::self()->set_close_reconnection_dia();
		}
	}

}
//Submit data to the database via QSqlTableModel (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_submit(QSqlTableModel *table_model){

	my_locker.lock();
	table_model->submitAll();
    QSqlDatabase ptr_database;
    ptr_database=table_model->database();

    if(table_model->lastError().isValid()==true || ptr_database.isOpen()==false){
        Data_Base::database_check_connection(&ptr_database);
		//renew the sql-command
		table_model->submitAll();
	}
	my_locker.unlock();
}
//Get a list of existing tables in the database (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_tables(QStringList *table_list, QSqlDatabase *ptr_database){
	
	my_locker.lock();
	*table_list=ptr_database->tables(QSql::Tables);
	
	if(ptr_database->lastError().isValid()==true || table_list->size()<=0){
		Data_Base::database_check_connection(ptr_database);
		//renew the sql-command
		*table_list=ptr_database->tables(QSql::Tables);
	}
	my_locker.unlock();
}
//Get a list of existing columns of a database table in the database (thread safe, due to lockinq with QMutex) (static)
void Data_Base::database_table_columns(QSqlRecord *columns, QString table_name, QSqlDatabase *ptr_database){
	my_locker.lock();
	(*columns)=ptr_database->record(table_name);



	if(ptr_database->lastError().isValid()==true || columns->count()<=0){
		Data_Base::database_check_connection(ptr_database);
		//renew the sql-command
		(*columns)=ptr_database->record(table_name);
	}
	my_locker.unlock();
}
//Get a list of existing columns of a database table in the database by an query (thread safe, due to lockinq with QMutex); please use this function instead of database_table_columns(...) (static)
void Data_Base::database_table_columns_query(QSqlRecord *columns, string schema_name, string table_name, QSqlDatabase *ptr_database) {

	
	QSqlQuery my_query(*ptr_database);
	ostringstream query_string;
	query_string << "SELECT column_name FROM information_schema.columns WHERE table_schema = '"<< schema_name<<"'";
	query_string << " AND table_name = '" << table_name << "'";

	

	Data_Base::database_request(&my_query, query_string.str(), ptr_database);




	if (my_query.lastError().isValid() == true) {
		Error msg;
		msg.set_msg("database_table_columns_query(QSqlRecord *columns, QString schema_name, QString table_name, QSqlDatabase *ptr_database)", "Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << table_name << endl;
		info << "View error info: " << my_query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	my_query.first();
	//transfer data to record
	do {
		QSqlField buffer;
		buffer.setName(my_query.record().value(0).toString());
		buffer.setValue(my_query.record().value(0).toString());
		
		columns->append(buffer);
		

	} while (my_query.next() == true);

}
//Convert the driver name into an enumerator of the driver types (static)
_sys_driver_type Data_Base::convert_txt2drivertype(const string txt){
	_sys_driver_type buffer;

	//this text string are implemented in QSql (see the Qt-assistent for the QSqlDatabase class)
	if(txt=="QPSQL"){
		buffer=_sys_driver_type::POSTGRESQL;
	}
	else if(txt=="QMYSQL"){
		buffer=_sys_driver_type::MYSQL;
	}
	else{
		buffer=_sys_driver_type::UNKNOWN_DR;
	}

	return buffer;
}
//Connect and open the database
void Data_Base::open_database(void){
	int counter=0;
	this->set_output_prefix();
	ostringstream cout;
	//set the connection
	//set the driver
	this->database = QSqlDatabase::addDatabase(this->driver_name.c_str(),sys_label::str_sys.c_str());
	//set the connection parameters
	this->database.setHostName(this->host_name.c_str());
	this->database.setDatabaseName(this->database_name.c_str());
	this->database.setUserName(this->user_name.c_str());
	this->driver_type=this->convert_txt2drivertype(this->driver_name);
	this->database.setConnectOptions("requiressl=1");

	if(this->password==label::not_set){
		//open dialog
		this->password="";
		Sys_Database_Dia database_dialog;
		database_dialog.hide_table_file_browser();
		database_dialog.enable_for_password();
		database_dialog.set_predef_parameter2dialog(this->get_database_conn_param());
		//execute the dia
		if(database_dialog.exec()!=0){//if ok set the parameter	
			this->password=database_dialog.password().toStdString();
			this->password_save_flag=database_dialog.password_save_flag();
		}
		else{
			return;
		}
	}

	if(this->password.length()==0){
		this->password=label::not_set;
	}

	//open database
	cout << "Open the connection to database: " << this->database_name  <<"..." <<endl;
	do{
		char szHex[33];
		if(counter==1 ){
			Sys_AES_Algo aes;
			aes.MakeKey("atcdeughiscqefrz", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
			char *str=new char[1000];
			strcpy(str,password.c_str());
			char szDataOut[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
			aes.HexStr2CharStr(str,(unsigned char*)szHex,16);
			aes.DecryptBlock(szHex, szDataOut);
			this->password=szDataOut;
		}
		this->database.setPassword(this->password.c_str());



		//this->database.open();
		//wrong connections
		if (!this->database.open()) {
			this->database.setConnectOptions(); // clear options
			this->database.open();
		}
		this->connection_ok=this->database.isOpen();
		counter++;
		if(counter==2){
			break;
		}
	}
	while(this->connection_ok==false);




	//check connection
	if(this->connection_ok==true){
		cout << "Connection to database " << this->database_name << " is successfully!" <<endl;
	}
	else{
		this->database.setConnectOptions();
		//set error
		cout << "Can not connect to database: " << this->database_name <<endl;
		cout << "Check the exceptions-file for error" <<endl;
		Sys_Common_Output::output_system->output_txt(&cout, true);
		Sys_Common_Output::output_system->rewind_userprefix();
		Error msg=this->set_error(0);
		throw msg;
	} 

	Sys_Common_Output::output_system->output_txt(&cout, true);
	Sys_Common_Output::output_system->rewind_userprefix();
}
//Output the database connection parameter with their keywords to a file
void Data_Base::output_database_connection2file(QFile *output_file){

	ostringstream txt;
	txt << sys_label::key_hostname << " "<< this->host_name << endl;
	txt << sys_label::key_driver_name << " "<<this->driver_name << endl;
	txt << sys_label::key_database_name << " "<< this->database_name << endl;
	txt << sys_label::key_user_name << " "<< this->user_name << endl;
	txt << sys_label::key_password_save<<" "<<functions::convert_boolean2string(this->password_save_flag)<<endl;
	if(this->password_save_flag==true){
		Sys_AES_Algo aes;
		aes.MakeKey("atcdeughiscqefrz", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
		char *str=new char[1000];
		char szHex[33];
		strcpy(str,password.c_str());
		char szDataOut[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		aes.EncryptBlock(str, szDataOut);
		aes.CharStr2HexStr((unsigned char*)szDataOut,szHex,16);

		txt << sys_label::key_password << " "<< szHex << endl;

		//txt << sys_label::key_password << " "<< this->password << endl;
	}
	output_file->write(txt.str().c_str());
}
//Output the database connection parameter with their keywords to a file
void Data_Base::output_database_connection2file(ofstream *output_file){

	ostringstream txt;
	txt << sys_label::key_hostname << " "<< this->host_name << endl;
	txt << sys_label::key_driver_name << " "<<this->driver_name << endl;
	txt << sys_label::key_database_name << " "<< this->database_name << endl;
	txt << sys_label::key_user_name << " "<< this->user_name << endl;
	txt << sys_label::key_password_save<<" "<<functions::convert_boolean2string(this->password_save_flag)<<endl;
	if(this->password_save_flag==true){
		Sys_AES_Algo aes;
		aes.MakeKey("atcdeughiscqefrz", "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16, 16);
		char *str=new char[1000];
		char szHex[33];
		strcpy(str,this->password.c_str());
		char szDataOut[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		aes.EncryptBlock(str, szDataOut);
		aes.CharStr2HexStr((unsigned char*)szDataOut,szHex,16);

		txt << sys_label::key_password << " "<< szHex << endl;
		//txt << sys_label::key_password << " "<< this->password << endl;
	}
	*output_file << txt.str();
	output_file->flush();
	output_file->clear();

}
//Get the database connection parameter 
_sys_database_params Data_Base::get_database_conn_param(void){
	_sys_database_params buffer;
	buffer.host_name=this->host_name;
	buffer.database_name=this->database_name;
	buffer.user_name=this->user_name;
	buffer.password=this->password;
	buffer.driver_name=this->driver_name;
	buffer.driver_type=this->driver_type;
	buffer.password_saved=this->password_save_flag;

	return buffer;
}
///Set the flag, for stopping the reconnection tries (static)
void Data_Base::set_reconnection_flag(const bool flag){
	Data_Base::reconnect_flag=flag;
}
//______________________________________
//private:
//Set the database connection parameters per file
void Data_Base::read_input(void){
	this->set_output_prefix();
	//reset the connection parameter
	this->host_name=label::not_set;
	this->database_name=label::not_set;
	this->user_name=label::not_set;
	this->password=label::not_set;
	this->driver_name=label::not_set;

	ifstream database_file;
	string buffer;
	
	ostringstream cout;	
	//open the file for reading
	cout <<"Open the database file to read in the connection parameters: " << this->file_name_connection << "..." <<endl;

	database_file.open(this->file_name_connection.c_str());
	//check the file
	if(!(database_file)){	
		//set error
		cout <<"Can not open the database file: " << this->file_name_connection << endl;
		cout <<"Ceck the ecxeption-file for error" << endl;
		Sys_Common_Output::output_system->rewind_userprefix();
		throw (this->set_error(1));
	}
	string buff_save=label::not_set;
	while(getline(database_file, buffer)){
		try{
			functions::clean_string(&buffer);
			if(!buffer.empty()){
				if(this->host_name==label::not_set){
					this->host_name=this->found_string(sys_label::key_hostname, buffer);//this are the expression for which is searched; the string after (expect whitespace) is taken as parameter
				}
				if(this->database_name==label::not_set){
					this->database_name=this->found_string(sys_label::key_database_name, buffer);
				}
				if(this->user_name==label::not_set){
					this->user_name=this->found_string(sys_label::key_user_name,buffer);
				}
				if(buff_save==label::not_set){
					buff_save=this->found_string(sys_label::key_password_save,buffer);
					if(buff_save!=label::not_set){
						this->password_save_flag=functions::convert_string2boolean(buff_save);
					}
				}
				if(this->password==label::not_set){
					this->password=this->found_string(sys_label::key_password,buffer);
				}
				if(this->driver_name==label::not_set){
					this->driver_name=this->found_string(sys_label::key_driver_name,buffer);
				}
			}
		}
		catch(Error msg){
			Sys_Common_Output::output_system->rewind_userprefix();
			throw msg;
		}
			
	}
	cout<<"Connection parameters are read!" << endl;
	Sys_Common_Output::output_system->output_txt(&cout, true);
	Sys_Common_Output::output_system->rewind_userprefix();
}

//Find the following string to an keyword (expression) out of the search_string
string Data_Base::found_string(const string expression, const string search_string){
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
				//begin of search string
				buffer_begin=buffer.substr(0,pos);
				//rest of search string
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
		}
		//end when buffer size is reached or all is found
		while(buffer.size()!=0 && (found_flag_res==false && found_flag_exp==false));

		//expression is found but no string after
		if (found_flag_exp==true && found_flag_res==false){
			Error msg=set_error(2);
			stringstream info;
			info << "Expression: " << expression << endl;
			msg.make_second_info(info.str());
			throw(msg);
		}
		if(result.empty()){
			return result=label::not_set;
		}
		else{
			return result;
		}

}
//Set the prefix for an output
void Data_Base::set_output_prefix(void){
	Sys_Common_Output::output_system->set_userprefix("DB> ");
}
//Set error(s)
Error Data_Base::set_error(const int err_type){
		string place="Data_Base::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://no connection
			place.append("open_database(void)");
			reason="No connection to database possible" ;
			help="Check the connection parameters";
			info << "Hostname           : " << this->host_name << endl;
			info << "Databasename       : " << this->database_name << endl;
			info << "Username           : " << this->user_name << endl;
			info << "Driver name        : " << this->driver_name <<endl;
			info << "Last error database: " << this->database.lastError().databaseText().toStdString() << endl;
			info << "Last error driver  : " << this->database.lastError().driverText().toStdString() << endl;
            info<< "Available drivers are: ";
            for(int i=0; i<this->database.drivers().length();i++){
                info<< this->database.drivers().at(i).toStdString()<<"; ";
            }
            info<<endl;

			fatal=true;//fatal error;
			type=4;
			break;
		case 1:
			//external msg
			place.append("read_input(void)");
			reason="Can not open file" ;
			help="Check the filename";
			info <<" Filename " << file_name_connection << endl;
			fatal=true;//fatal error;
			type=5;
			break;
		case 2:
			//external msg
			place.append("found_string(const string expression, const string search_string)");
			reason="No result is found to the expression";
			help="Check the file and the expression";
			info << "Filename " << file_name_connection << endl;
			fatal=true;//fatal error;
			type=5;
			break;
		default:
			place.append("set_errwarn(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place,reason,help,type,fatal);
	msg.make_second_info(info.str());

return msg;
}
