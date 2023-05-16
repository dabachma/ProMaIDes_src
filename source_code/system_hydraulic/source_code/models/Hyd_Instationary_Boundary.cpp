//#include "Hyd_Instationary_Boundary.h"
#include "Hyd_Headers_Precompiled.h"

//init static members
Tables *Hyd_Instationary_Boundary::instat_bound_table=NULL;
Tables *Hyd_Instationary_Boundary::instat_bound_point_table=NULL;

//constructor
Hyd_Instationary_Boundary::Hyd_Instationary_Boundary(void){
	this->number=-1;
	this->number_of_points=0;
	this->boundary_points=NULL;
	this->value_actuel=0.0;
	this->index_left=0;
	this->specific_multiplicator=1.0;
	this->setted_type=_hyd_bound_type::point_type;
	this->given_type=_hyd_bound_type::point_type;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Instationary_Boundary), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Instationary_Boundary::~Hyd_Instationary_Boundary(void){

	this->delete_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Instationary_Boundary), _sys_system_modules::HYD_SYS);
}
//______________
//public
//read the boundary point (vlaue and hour h)
void Hyd_Instationary_Boundary::read_value(const string file_name, const int index){

	ifstream ifile;
	//open file
	ifile.open(file_name.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(7);
		ostringstream info;
		info << "Filename: " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//read in
	string myline;
	int pos=-1;
	stringstream my_stream;
	int found_index=-1;
	bool found_flag=false;
	int line_counter=0;
	

	this->number=index;

	try{
		do{
			//search for the begin of the curve
			do{
				getline(ifile, myline,'\n');
				line_counter++;
				functions::clean_string(&myline);

				pos=myline.rfind("!BEGIN");
				if(pos>=0){
					myline.erase(pos+6);
				}
			}
			while(myline!="!BEGIN" && ifile.eof()!=true);
			if(ifile.eof()==true){
				break;
			}

			
			do{
				//next line for the curve number and number of points
				getline(ifile, myline,'\n');
				line_counter++;
				functions::clean_string(&myline);
			}
			while(myline.empty()==true);
			//read it in
			my_stream << myline;
			string typ_buff;
			my_stream >> found_index >>this->number_of_points >>typ_buff;
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              :" << file_name << endl;
				info << "Error occurs near line: "<< line_counter << endl;
				Error msg=this->set_error(1);
				msg.make_second_info(info.str());
				throw msg;
			}
			try{
				this->given_type=Hyd_Instationary_Boundary::transfrom_txt2_instatboundtype(typ_buff);
			}
			catch(Error msg){
				throw msg;
			}
			my_stream.clear();
			my_stream.str("");
			

			//compare it with the given index
			if(found_index==index){
				found_flag=true;
			}
			else{
				found_index=-1;
				this->number_of_points=0;
			}

		}
		while(found_flag!=true);
		if(found_flag!=true){
			ostringstream info;
			info << "Searched curve number:" << index << endl;
			info << "Filename             :" << file_name << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}

		//allocate the points
		try{
			this->alloc_points();
		}
		catch(Error msg){
			//close file
			ifile.close();
			throw msg;
		}

		int counter=0;

		//read in the bound points
		for(int i=0; i<this->number_of_points; i++){
			this->boundary_points[i].hour=0.0;
			this->boundary_points[i].value=0.0;

			//delete the comments
			getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);

			if(myline.empty()!=true){
				pos=myline.rfind("!END");
				if(pos>=0){
					myline.erase(pos+4);
				}
				//stop when end is reach
				if(myline=="!END"|| ifile.eof()==true){
					break;
				}
				//read it in
				my_stream << myline;
				my_stream >> this->boundary_points[i].hour >> this->boundary_points[i].value;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  :" << my_stream.str() << endl;
					info << "Filename              :" << file_name << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(1);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				counter++;
			}
			else{
				i--;
				if (ifile.eof() == true) {
					break;
				}
			}

		}
		//check if all is read
		if(counter!=this->number_of_points){
			ostringstream info;
			info << "Filename " << file_name << endl;
			info << "Given number of points: " <<this->number_of_points <<endl;
			info << "Found number of points: " <<counter << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(5);
			msg.make_second_info(info.str());
			throw msg;
		}


		//check for th !END flag
		getline(ifile, myline,'\n');
		line_counter++;
		_Hyd_Parse_IO::erase_comment(&myline);
		_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
		_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		pos=myline.rfind("!END");
		if(pos>=0){
			myline.erase(pos+4);
		}
		//no !END flag is found
		if(myline!="!END"){
			ostringstream info;
			info << "Filename " << file_name << endl;
			info << "Warning occurs near line: "<< line_counter << endl;
			Warning msg=this->set_warning(1);
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}
	catch(Error msg){
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();

	try{
		this->check_time_values();
	}
	catch(Error msg){
		ostringstream info;
		info <<"Filename " << file_name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Create the database table for the instationary boundary curves (static)
void Hyd_Instationary_Boundary::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Instationary_Boundary::instat_bound_table==NULL){
		ostringstream cout;
		cout << "Create global instationary boundary data database table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_instat_bound;
		const int num_col=6;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}
		tab_col[0].name=hyd_label::instatbound_glob_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;

		tab_col[1].name=hyd_label::instatbound_curve_id;
		tab_col[1].type=sys_label::tab_col_type_int;
		tab_col[1].unsigned_flag=true;

		tab_col[2].name=hyd_label::sz_bound_id;
		tab_col[2].type=sys_label::tab_col_type_int;
		tab_col[2].unsigned_flag=true;
		tab_col[2].key_flag=true;

		tab_col[3].name=hyd_label::bounddata_type;
		tab_col[3].type=sys_label::tab_col_type_string;

		tab_col[4].name=hyd_label::genmod_type;
		tab_col[4].type=sys_label::tab_col_type_string;
	
		tab_col[5].name=hyd_label::genmod_id;
		tab_col[5].type=sys_label::tab_col_type_int;
		tab_col[5].unsigned_flag=true;
		tab_col[5].key_flag=true;



		try{
			Hyd_Instationary_Boundary::instat_bound_table= new Tables();
			if(Hyd_Instationary_Boundary::instat_bound_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Instationary_Boundary::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Instationary_Boundary::close_table();
			throw msg;
		}

		//create appending tables
		Hyd_Instationary_Boundary::create_point_table(ptr_database);

		Hyd_Instationary_Boundary::close_table();
	}
}
//Set the database table for the instationary boundary curves: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Instationary_Boundary::set_table(QSqlDatabase *ptr_database){

	if(Hyd_Instationary_Boundary::instat_bound_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_instat_bound;
		string tab_id_col[6];

		tab_id_col[0]=hyd_label::instatbound_glob_id;
		tab_id_col[1]=hyd_label::instatbound_curve_id;
		tab_id_col[2]=hyd_label::sz_bound_id;
		tab_id_col[3]=hyd_label::bounddata_type;
		tab_id_col[4]=hyd_label::genmod_type;
		tab_id_col[5]=hyd_label::genmod_id;


		try{
			Hyd_Instationary_Boundary::instat_bound_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Instationary_Boundary::instat_bound_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Instationary_Boundary::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Instationary_Boundary::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the instationary boundary curves (static)
void Hyd_Instationary_Boundary::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Instationary_Boundary::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	Hyd_Instationary_Boundary::instat_bound_table->delete_data_in_table(ptr_database);

	//delete data in appending table
	Hyd_Instationary_Boundary::delete_point_data_in_table(ptr_database);
}
//Close and delete the database table for the instationary boundary curves (static)
void Hyd_Instationary_Boundary::close_table(void){
	if(Hyd_Instationary_Boundary::instat_bound_table!=NULL){
		delete Hyd_Instationary_Boundary::instat_bound_table;
		Hyd_Instationary_Boundary::instat_bound_table=NULL;
	}
	//close appending table
	Hyd_Instationary_Boundary::close_point_table();
}
//Delete the boundary curve data in the database table for a given boundary scenario (static)
void Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(QSqlDatabase *ptr_database, const int sc_id){
	QSqlQueryModel results;

	try{
		Hyd_Instationary_Boundary::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//delete the points
	int number=0;
	ostringstream test_filter;
	test_filter<< "Select * ";
	test_filter << " from " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;
	Data_Base::database_request(&results, test_filter.str(), ptr_database);
	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//hyd_label::instatbound_glob_id
	number=results.rowCount();
	int id2del=0;

	for(int i=0; i< number; i++){
		id2del=results.record(i).value((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_glob_id)).c_str()).toInt();
		Hyd_Instationary_Boundary::delete_boundary_curve_points_by_curve_id(ptr_database, id2del);
	}	


	//delete the curves
	test_filter.str("");
	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::sz_bound_id) << " = " << sc_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Instationary_Boundary::delete_boundary_curve_by_scenario(QSqlDatabase *ptr_database, const int sc_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Create the database point table for the instationary boundary curves
void Hyd_Instationary_Boundary::create_point_table(QSqlDatabase *ptr_database){
		if(Hyd_Instationary_Boundary::instat_bound_point_table==NULL){
			ostringstream cout;
			cout << "Create global instationary boundary curvepoint data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_instatbound_point;
			const int num_col=5;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}
			tab_col[0].name=hyd_label::instatbound_point_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=hyd_label::instatbound_glob_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=hyd_label::instatbound_point_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=hyd_label::bounddata_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].default_value=="0.0";

			tab_col[4].name=hyd_label::instatbound_time;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;
			tab_col[4].default_value="0.0";


			try{
				Hyd_Instationary_Boundary::instat_bound_point_table= new Tables();
				if(Hyd_Instationary_Boundary::instat_bound_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Instationary_Boundary::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Instationary_Boundary::close_point_table();
				throw msg;
			}

		Hyd_Instationary_Boundary::close_point_table();
	}

}
//Set the database point table for the instationary boundary curves: it sets the table name and the name of the columns and allocate them
void Hyd_Instationary_Boundary::set_point_table(QSqlDatabase *ptr_database){
	if(Hyd_Instationary_Boundary::instat_bound_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_instatbound_point;
		string tab_id_col[5];

		tab_id_col[0]=hyd_label::instatbound_point_glob_id;
		tab_id_col[1]=hyd_label::instatbound_glob_id;
		tab_id_col[2]=hyd_label::instatbound_point_id;
		tab_id_col[3]=hyd_label::bounddata_value;
		tab_id_col[4]=hyd_label::instatbound_time;


		try{
			Hyd_Instationary_Boundary::instat_bound_point_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Instationary_Boundary::instat_bound_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Instationary_Boundary::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Instationary_Boundary::close_table();
			throw msg;
		}
	}

}
//Delete all point data in the database table for the instationary boundary curves
void Hyd_Instationary_Boundary::delete_point_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Instationary_Boundary::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete data in the table
	Hyd_Instationary_Boundary::instat_bound_point_table->delete_data_in_table(ptr_database);

}
//Close and delete the database point table for the instationary boundary curves
void Hyd_Instationary_Boundary::close_point_table(void){
	if(Hyd_Instationary_Boundary::instat_bound_point_table!=NULL){
		delete Hyd_Instationary_Boundary::instat_bound_point_table;
		Hyd_Instationary_Boundary::instat_bound_point_table=NULL;
	}
}
//Select and count the number of relevant instationary boundary curves in a database table (static)
int Hyd_Instationary_Boundary::select_relevant_curves_database(QSqlTableModel *results , const int model_id, _hyd_model_type model_type , const bool with_output, Hyd_Boundary_Szenario *ptr_hyd_sz){
	int number=0;
	try{
		Hyd_Instationary_Boundary::set_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant instationary boundary curves in database (";
		cout << Hyd_Hydraulic_System::convert_model_type2txt(model_type)<<")..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
}

	//give the table name to the query model
	results->setTable((Hyd_Instationary_Boundary::instat_bound_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::genmod_type) << " = '" << Hyd_Hydraulic_System::convert_model_type2txt(model_type) << "'";
	filter << " and ";
	filter << Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::genmod_id) << " = " << model_id;
	filter << " and ";
	filter << Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::sz_bound_id) << " = " << ptr_hyd_sz->get_id();


	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Instationary_Boundary::select_relevant_curves_database(QSqlTableModel *results , const int model_id, _hyd_model_type model_type , const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_curve_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant instationary boundary curve(s) are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}


	return number;
}
//Transfer the instationary boundary curves of a file into a database table
void Hyd_Instationary_Boundary::transfer_instat_curve2database_table(QSqlDatabase *ptr_database, _hyd_model_type model_type, const int model_id){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Instationary_Boundary::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Instationary_Boundary::instat_bound_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=0;
	
	id_glob=Hyd_Instationary_Boundary::instat_bound_table->maximum_int_of_column(Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_glob_id),ptr_database)+1;		
	//insert row
	model.insertRows(0, 1);
																			
																																					
	//insert data
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_glob_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::genmod_id)).c_str())),model_id);
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_curve_id)).c_str())),this->number);
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::sz_bound_id)).c_str())),this->hyd_sz.get_id());
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::bounddata_type)).c_str())),this->transfrom_instatboundtype2txt(this->given_type).c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::genmod_type)).c_str())),Hyd_Hydraulic_System::convert_model_type2txt(model_type).c_str());


	//submit it to the datbase
	Data_Base::database_submit(&model);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Instationary_Boundary::instat_bound_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Model type                : " << Hyd_Hydraulic_System::convert_model_type2txt(model_type)<< endl;
		info << "Model Id                  : " << model_id <<endl;
		info << "Curve Id (local)          : " << this->number <<endl;
		info << "Curve Id (gloabl)         : " << id_glob <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
	else{

		//transfer data to appending table
		this->transfer_instat_points2database_table(ptr_database, id_glob);
	}

}
//Set the instationary boundary curves from a given selection of a database table
void Hyd_Instationary_Boundary::read_value(QSqlTableModel *results, const int index){

	//set the global curve number
	int global_number=0;
	global_number=results->record(index).value((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_glob_id)).c_str()).toInt();
	this->number=results->record(index).value((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::instatbound_curve_id)).c_str()).toInt();
	this->given_type=Hyd_Instationary_Boundary::transfrom_txt2_instatboundtype(results->record(index).value((Hyd_Instationary_Boundary::instat_bound_table->get_column_name(hyd_label::bounddata_type)).c_str()).toString().toStdString());

	QSqlTableModel point_results(0,results->database());
	//number of points
	this->number_of_points=Hyd_Instationary_Boundary::select_relevant_points_database(&point_results,global_number);

	//allocate the points
	this->alloc_points();
	//read in points
	for(int i=0; i< this->number_of_points; i++){
		this->read_points(&point_results, i);
	}

	//check the time values
	this->check_time_values();


}
//Set the instationary boundary curves directly with given values
void Hyd_Instationary_Boundary::set_curve_dircetly(QList<double> time, QList<double> value, const _hyd_bound_type type) {
	this->given_type=type;

	if (time.count() != value.count()) {
		Error msg = this->set_error(12);
		ostringstream info;
		info << "Number time " << time.count() << endl;
		info << "Number values " << value.count() << endl;
		msg.make_second_info(info.str());
		throw msg;

	}

	this->number_of_points = time.count();

	//allocate the points
	this->alloc_points();
	//read in points
	for (int i = 0; i < this->number_of_points; i++) {
		this->boundary_points[i].value = value.at(i);
		this->boundary_points[i].hour = time.at(i) ;

	}

	//check the time values
	this->check_time_values();

}
//Set the type of the instationary boundary curve (e.g. a point length or an area);
void Hyd_Instationary_Boundary::set_type( _hyd_bound_type bound_type){
	this->setted_type=bound_type;
	if(this->setted_type!=this->given_type){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Required boundary type         : " << this->transfrom_instatboundtype2txt(this->setted_type) <<endl;
		info << "Given type in the boundary file: " << this->transfrom_instatboundtype2txt(this->given_type) << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set the specific value (e.g. a length or an area); This value is multiplied with the discharge to get always the unit m³/s
void Hyd_Instationary_Boundary::set_specific_value(const double specific_multiplicator){
	//specific multiplikator can not be set by a discharge point boundary or a waterlevel point boundary condition
	if(this->given_type!=_hyd_bound_type::point_type ||this->given_type!=_hyd_bound_type::waterlevel_type){
		this->specific_multiplicator=specific_multiplicator;
	}
}
//Check the curve values
void Hyd_Instationary_Boundary::check_curve(void) {
	if (this->setted_type == _hyd_bound_type::temperature) {
		for (int i = 0; i < this->number_of_points; i++) {
			if (this->boundary_points[i].value < 0.0) {
				//Set warning


			}

		}

	}
	else if (this->setted_type == _hyd_bound_type::percentage) {



	}


}
//output the curve
void Hyd_Instationary_Boundary::output_member(void){
	ostringstream prefix;
	prefix << "BC_"<< this->number<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->number_of_points>0){
		cout << "Curve No. " << this->number << " with " << this->number_of_points << " points " << endl;
		if(this->given_type==_hyd_bound_type::length_type){
			cout << " Discharge value is related to a length" << label::qm_per_secm  << endl;
		}
		else if(this->given_type==_hyd_bound_type::area_type){
			cout << " Discharge value is related to an area" <<label::qm_per_secsquarem <<endl;
		}
		else if(this->given_type==_hyd_bound_type::waterlevel_type){
			cout << " Waterlevel value " <<label::m <<endl;
		}
		else if (this->given_type == _hyd_bound_type::temperature) {
			cout << " Temperature value " << label::kelvin << endl;
		}
		else if (this->given_type == _hyd_bound_type::radiation) {
			cout << " Radiation value " << label::watt_per_square_m << endl;
		}
		else if (this->given_type == _hyd_bound_type::percentage) {
			cout << " Percentage value [0-1] " << label::no_unit << endl;
		}
		else if (this->given_type == _hyd_bound_type::percentage) {
			cout << " Speed value " << label::m_per_sec << endl;
		}
		else{
			cout <<" Discharge value is related to a point " << label::qm_per_sec<<endl;
		}
		cout << " No. " << " Time" << label::hour << "    Value " << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		for(int i=0; i< this->number_of_points; i++){
			cout << W(3)<< i << W(10) << P(3)<< FORMAT_FIXED_REAL<< this->boundary_points[i].hour << W(11) << P(3)<< FORMAT_FIXED_REAL << this->boundary_points[i].value<< endl;		
			Sys_Common_Output::output_hyd->output_txt(&cout,true);
		}
	}
	else{
		cout << " No points are set ! " << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//get the curve number
int Hyd_Instationary_Boundary::get_curve_number(void){
	return this->number;
}
//Clone the instationary boundary curve
void Hyd_Instationary_Boundary::clone_curve(Hyd_Instationary_Boundary *curve){
	this->number=curve->number;
	this->number_of_points=curve->number_of_points;
	this->specific_multiplicator=curve->specific_multiplicator;
	this->setted_type=curve->setted_type;
	this->given_type=curve->given_type;
	this->alloc_points();
	for(int i=0; i< this->number_of_points; i++){
		this->boundary_points[i]=curve->boundary_points[i];

	}
}
//calculate the actuel discharge
double Hyd_Instationary_Boundary::calculate_actuel_boundary_value(const double time_point){
	this->value_actuel=0.0;
	double min_distance_left=1e15;
	double distance=0.0;	

	for(int i=this->index_left;i<this->number_of_points;i++){//search at the last start_point (time is always ascending)
		distance=this->boundary_points[i].hour*3600.0-time_point;

		if(i==0 && distance > 0.0){//no left value is found;no extrapolation => value is set to 0.0
			this->value_actuel=0.0;
			return this->value_actuel;
		}
		if(distance <= 0.0 && abs(distance)<=min_distance_left){
			min_distance_left=abs(distance);
			this->index_left=i;
		}
		else if(distance > 0.0){
			break;
		}
	}

	//no right value is found; no extrapolation => value is set to 0.0
	if( this->index_left==this->number_of_points-1){
		this->value_actuel=0.0;
		return this->value_actuel;
	}

	//interpolation
	double deltay=this->boundary_points[this->index_left+1].value-this->boundary_points[this->index_left].value;
	double deltax=(this->boundary_points[this->index_left+1].hour-this->boundary_points[this->index_left].hour)*3600.0;
	this->value_actuel=this->boundary_points[this->index_left].value+(deltay/deltax)*(time_point-this->boundary_points[this->index_left].hour*3600.0);
	
	//calculated related to the unit m^3/s
	this->value_actuel=this->value_actuel*this->specific_multiplicator;

	return this->value_actuel;
}
//Check the duration of the boundary curve
void Hyd_Instationary_Boundary::check_boundary_curve_duration(const double needed_duration){
	double set_duration=0.0;
	if(this->number_of_points>0){
		set_duration=this->boundary_points[this->number_of_points-1].hour-this->boundary_points[0].hour;
		set_duration=set_duration*(double)constant::hour_second;
	}
	if(needed_duration>set_duration){
		Error msg=this->set_error(8);
		ostringstream info;
		info<<"Setted duration " << set_duration << label::sec <<endl;
		info<<"Needed duration " << needed_duration << label::sec <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if it is start by a given time in second
void Hyd_Instationary_Boundary::check_starting_time(const double time_sec){
	if(abs((time_sec-(double)constant::hour_second)*this->boundary_points[0].hour)>constant::sec_epsilon){
		Error msg=this->set_error(9);
		ostringstream info;
		info<<"Setted starttime " << this->boundary_points[0].hour/(double)constant::hour_second << label::sec <<endl;
		info<<"Needed startime " << time_sec << label::sec <<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Check if there are negative values or zero values in the boundary curve
void Hyd_Instationary_Boundary::check_boundary_curve_neg_null_values(void){	
	for(int i=0; i< this->number_of_points;i++){
		if(this->boundary_points[i].value<0.0){
			Error msg=this->set_error(6);
			ostringstream info;
			info<<"Pointno. " <<i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//transform text to enum _hyd_bound_type (static)
_hyd_bound_type Hyd_Instationary_Boundary::transfrom_txt2_instatboundtype(string txt){

	_hyd_bound_type buffer;

	_Hyd_Parse_IO::string2lower(&txt);

	if(txt==hyd_label::Point_Boundary){
		buffer=_hyd_bound_type::point_type;
	}
	else if(txt==hyd_label::Area_Boundary){
		buffer=_hyd_bound_type::area_type;
	}
	else if(txt==hyd_label::Length_Boundary){
		buffer=_hyd_bound_type::length_type;
	}
	else if(txt==hyd_label::Waterlevel_Boundary){
		buffer=_hyd_bound_type::waterlevel_type;
	}
	else if (txt == hyd_label::Temp_Boundary) {
		buffer = _hyd_bound_type::temperature;
	}
	else if (txt == hyd_label::Solar_rad_Boundary) {
		buffer = _hyd_bound_type::radiation;
	}
	else if (txt == hyd_label::Perc_Boundary) {
		buffer = _hyd_bound_type::percentage;
	}
	else if (txt == hyd_label::Speed_Boundary) {
		buffer = _hyd_bound_type::speed;
	}
	else{
		Error msg;
		string place="Hyd_Instationary_Boundary::";
		place.append("transfrom_txt2_instatboundtype(const string txt)");
		string reason="Instationary boundary type is not known";
		string help="Check the given boundary type";
		ostringstream info;
		info <<"Given type    :" << txt << endl;
		info <<"Possible types: "<<endl;
		info <<" " << hyd_label::Point_Boundary << endl;
		info <<" " << hyd_label::Area_Boundary <<endl;
		info <<" " << hyd_label::Length_Boundary<<endl;
		info <<" " << hyd_label::Waterlevel_Boundary<<endl;
		if (Sys_Project::get_project_type() == _sys_project_type::proj_hyd_temp) {
			info << "Additonally for the temperature model: " << endl;
			info << " " << hyd_label::Temp_Boundary << endl;
			info << " " << hyd_label::Solar_rad_Boundary << endl;
			info << " " << hyd_label::Perc_Boundary << endl;
			info << " " << hyd_label::Speed_Boundary << endl;
		}


		msg.set_msg(place, reason, help, 1, false);
		msg.make_second_info(info.str());
		throw msg;

	}

	return buffer;

}
//transform enum _hyd_bound_type to text (static)
 string Hyd_Instationary_Boundary::transfrom_instatboundtype2txt(const _hyd_bound_type type){
	if(type==_hyd_bound_type::area_type){
		return hyd_label::Area_Boundary;
	}
	else if(type==_hyd_bound_type::point_type){
		return hyd_label::Point_Boundary;
	}
	else if(type==_hyd_bound_type::length_type){
		return hyd_label::Length_Boundary;
	}
	else if(type==_hyd_bound_type::waterlevel_type){
		return hyd_label::Waterlevel_Boundary;
	}
	else if (type == _hyd_bound_type::temperature) {
		return hyd_label::Temp_Boundary;
	}
	else if (type == _hyd_bound_type::radiation) {
		return hyd_label::Solar_rad_Boundary;
	}
	else if (type == _hyd_bound_type::speed) {
		return hyd_label::Speed_Boundary;
	}
	else if (type == _hyd_bound_type::percentage) {
		return hyd_label::Perc_Boundary;
	}


	return label::not_set;


}
 //Clear the instationary boundary curve, delete the points
 void Hyd_Instationary_Boundary::clear_boundary_curve(void){
	this->delete_points();
	this->number=-1;
	this->number_of_points=0;
	this->value_actuel=0.0;
	this->index_left=0;
	this->specific_multiplicator=1.0;
	this->setted_type=_hyd_bound_type::point_type;
	this->given_type=_hyd_bound_type::point_type;
 }

//___________
//private
//check if the time values are sequentiell increasing and that there are no negative time values
void Hyd_Instationary_Boundary::check_time_values(void){

	//check if they are increasing
	for(int i=0; i<this->number_of_points-1; i++){
		if(this->boundary_points[i].hour>=this->boundary_points[i+1].hour){
			Error msg=this->set_error(10);
			ostringstream info;
			info << "Curve-id: " << this->number << endl;
			info<<"Between Pointno. " <<i <<" ("<< this->boundary_points[i].hour<<" h) and " << i+1 << " (" << this->boundary_points[i+1].hour << " h)"<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//check if there are negative time values
	for(int i=0; i<this->number_of_points-1; i++){
		if(this->boundary_points[i].hour<0.0){
			Error msg=this->set_error(11);
			ostringstream info;
			info<<" Pointno. " <<i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}

}
//allocate the boundary_points of the curve
void Hyd_Instationary_Boundary::alloc_points(void){
	try{
		this->boundary_points=new _hyd_bound_point[this->number_of_points];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(_hyd_bound_point)*this->number_of_points, _sys_system_modules::HYD_SYS);
}
 //Delete the boundary points
 void Hyd_Instationary_Boundary::delete_points(void){
	if(this->boundary_points!=NULL){
		delete []this->boundary_points;
		this->boundary_points=NULL;
		//count the memory
		Sys_Memory_Count::self()->minus_mem(sizeof(_hyd_bound_point)*this->number_of_points, _sys_system_modules::HYD_SYS);

	}
 }
//Transfer the instationary boundary curves of a file into a database table
void Hyd_Instationary_Boundary::transfer_instat_points2database_table(QSqlDatabase *ptr_database, const int curve_glob_id){
	//mysql query with the table_model
	QSqlQuery query_buff(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Instationary_Boundary::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//evaluate the global identifier
	int point_id_glob=Hyd_Instationary_Boundary::instat_bound_point_table->maximum_int_of_column(Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_point_glob_id),ptr_database)+1;

	ostringstream query_header;
	query_header << "INSERT INTO  " << Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name();
	query_header <<" ( ";
	query_header << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_point_glob_id) <<" , ";
	query_header << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_point_id) <<" , ";
	query_header << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_glob_id) <<" , ";
	query_header << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::bounddata_value) <<" , ";
	query_header << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_time) <<" ) ";

	query_header << " VALUES ";
	ostringstream query_data;
	for(int i=0; i< this->number_of_points; i++){
		query_data << " ( ";
		query_data << point_id_glob<<" , ";
		query_data << i<<" , ";
		query_data << curve_glob_id <<" , ";
		query_data << this->boundary_points[i].value <<" , ";
		query_data <<this->boundary_points[i].hour <<" ) " << " ,";
		point_id_glob++;
	}
	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Curve Id (global)         : " << curve_glob_id <<endl;
		info << "Curve Id (local)          : " << this->number <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
}
//Select and count the number of relevant instationary boundary curve points in a database table (static)
int Hyd_Instationary_Boundary::select_relevant_points_database(QSqlTableModel *results , const int global_curve_id){
	int number=0;
	try{
		Hyd_Instationary_Boundary::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_glob_id) << " = " << global_curve_id;

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Instationary_Boundary::select_relevant_points_database(QSqlTableModel *results , const int global_curve_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_time).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();


	return number;
}
//Delete the boundary curve point data in the database table for a given curve id (static)
void Hyd_Instationary_Boundary::delete_boundary_curve_points_by_curve_id(QSqlDatabase *ptr_database, const int curve_id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Hyd_Instationary_Boundary::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_glob_id) << " = " << curve_id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Instationary_Boundary::delete_boundary_curve_points_by_scenario(QSqlDatabase *ptr_database, const int curve_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Instationary_Boundary::instat_bound_point_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Read in the points of the boundary curve from a given selection of a database table
void Hyd_Instationary_Boundary::read_points(const QSqlTableModel *results, const int index){
	this->boundary_points[index].value=results->record(index).value((Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::bounddata_value)).c_str()).toDouble();
	this->boundary_points[index].hour=results->record(index).value((Hyd_Instationary_Boundary::instat_bound_point_table->get_column_name(hyd_label::instatbound_time)).c_str()).toDouble();

}
//set the error
Error Hyd_Instationary_Boundary::set_error(const int err_type){
	string place="Hyd_Instationary_Boundary::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 1://wrong input
			place.append("read_value(const string file_name, const int index)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the instationary boundary curve in file";
			type=1;
			break;
		case 2://desired type do not correpond to the given type in the file 
			place.append("set_type( _hyd_bound_type bound_type)");
			reason="Desired boundary type do not correspond to the given type ";
			help="Check the instationary boundary curve in file and required type of the element/profile";
			type=1;
			break;
		case 3://given curve number is not found 
			place.append("read_value(const string file_name, const int index)");
			reason="The given curve number is not found";
			help="Check the instationary boundary curve in file; numbering have to be sequenziell (start by 0, 1, 2 ..)";
			type=1;
			break;
		case 4://bad alloc
			place.append("alloc_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://not all points are found
			place.append("read_value(const string file_name, const int index)");
			reason="Not all instationary boundary points are found ";
			help="Check the file";
			type=1;
			break;
		case 6://waterlevel type: values are below 0.0
			place.append("check_boundary_curve_neg_null_values(void)");
			reason="Boundary values are <= 0.0";
			help="Check the curve";
			type=3;
			break;
		case 7://boundary file is not open
			place.append("read_value(const string file_name, const int index)");
			reason="Can not open the file of instationary boundary curves";
			help="Check the file";
			type=5;
			break;
		case 8://setted curve is to short for the calculation time
			place.append("check_boundary_curve_duration(const double needed_duration)");
			reason="The set duartion of the curve is not sufficient for the duration of modeltime";
			help="Check the curve";
			type=3;
			break;
		case 9://Startime of the curves do not correspond to the requiered one
			place.append("check_starting_time(const double time_sec)");
			reason="Startime of the curve does not correspond to the requiered one";
			help="Check the curve";
			type=3;
			break;
		case 10://The time values do not increase sequentielly
			place.append("check_time_values(void)");
			reason="The time values do not increase sequentielly";
			help="Check the curve";
			type=3;
			break;
		case 11://There are negative time values
			place.append("check_time_values(void)");
			reason="There are negative time values";
			help="Check the curve";
			type=3;
			break;
		case 12://There are negative time values
			place.append("Hyd_Instationary_Boundary::set_curve_dircetly(QList<double> time, QList<double> value, const _hyd_bound_type type)");
			reason = "The number of time and values is not equal";
			help = "Please check the given lists";
			type = 6;
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
Warning Hyd_Instationary_Boundary::set_warning(const int warn_type){
	string place="Hyd_Instationary_Boundary::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_instat_curve2database_table(QSqlDatabase *ptr_database, _hyd_model_type model_type, const int model_id)");
			reason="Can not submit the instationary boundary curve data to the database";
			help="Check the database";
			type=2;
			break;
		case 1://no !END-flag is found in file
			place.append("read_value(const string file_name, const int index)");
			reason="The point data block of the instationary boundary curve is not closed with !END in file";
			help="Check the file and the number of point specification in file";
			type=1;
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
};