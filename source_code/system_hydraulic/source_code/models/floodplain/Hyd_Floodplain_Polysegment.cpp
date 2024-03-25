#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplain_Polysegment.h"

//init static members
Tables *Hyd_Floodplain_Polysegment::polysegment_table=NULL;
Tables *Hyd_Floodplain_Polysegment::polysegment_point_table=NULL;

//constructor
Hyd_Floodplain_Polysegment::Hyd_Floodplain_Polysegment(void){
	this->line_points=NULL;
	this->no_flow_flag=false;
	this->line_type=_hyd_floodplain_polysegment_type::DIKELINE;
	this->intercepted_elem_type=_hyd_elem_type::STANDARD_ELEM;
	this->index=-1;
	this->name=hyd_label::dike_line;

	this->list_part_points=NULL;
	this->number_part_points=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Polysegment), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Floodplain_Polysegment::~Hyd_Floodplain_Polysegment(void){
	this->delete_dikeline_points();
	this->delete_part_point_list();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplain_Polysegment), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Input the members per file
void Hyd_Floodplain_Polysegment::input_members(const int index, const string filename, const double offset_x, const double offset_y){
	
	ifstream ifile;
	//open file
	ifile.open(filename.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Filename: " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter=0;
	string myline;
	int pos=-1;
	stringstream my_stream;
	int found_index=-1;
	int found_points=0;
	bool found_flag=false;
	Geo_Point *point_buff=NULL;
	
	this->index=index;

	try{
		do{
			//search for the begin of the curve
			do{
				getline(ifile, myline,'\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_carriageReturn(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
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
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_carriageReturn(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			}
			while(myline.empty()==true && ifile.eof()!=true);

			//read it in
			my_stream << myline;
			string closed_buff;
			string name_buff;
			my_stream >> found_index >>name_buff >> found_points; 
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              : " << filename << endl;
				info << "Input must be         : index   name   number_points"<< endl;
				info << "Error occurs near line: "<< line_counter << endl;
				Error msg=this->set_error(1);
				msg.make_second_info(info.str());
				throw msg;
			}
			my_stream >> closed_buff;
			my_stream.clear();
			my_stream.str("");

			//compare it with the given index
			if(found_index==index){
				found_flag=true;
				this->name=name_buff;
				
				try{
					this->set_is_closed_str(closed_buff);
					this->set_number_of_points(found_points);
				}
				catch(Error msg){
					ostringstream info;
					info << "Polysegment index     : " << this->index << endl;
					info << "Linetype              : " << this->convert_polysegment_type2txt(this->line_type)<< endl;
					info << "Filename              : " << filename << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			else{
				found_index=-1;
			}

		}while(found_flag!=true);


		if(found_flag!=true){
			ostringstream info;
			info << "Searched index of the polysegment: " << this->index << endl;
			info << "Linetype                         : " << this->convert_polysegment_type2txt(this->line_type)<< endl;
			info << "Filename                         : " << filename << endl;
			Error msg=this->set_error(2);
			msg.make_second_info(info.str());
			throw msg;
		}

		int counter=0;
		int col=0;
		double x_buffer=0.0;
		double y_buffer=0.0;
		double height_buff=0.0;
		double base_buff=0.0;
		double poleni_buff=constant::poleni_const;
		bool overflow_buff=true;
		string overflow_str;
		bool break_buff=false;
		string break_str;
		string aprupt_fails_str;
		_hyd_break_parameters break_param_buff;

		
		try{
			point_buff=new Geo_Point[this->number_points];
		}catch(bad_alloc &){
			Error msg=this->set_error(5);
			throw msg;
		}


		//read in the bound points
		for(int i=0; i<this->number_points; i++){
			poleni_buff=constant::poleni_const;
			overflow_buff=true;
			break_buff=false;
			break_param_buff.abrupt_fails_flag=false;
			break_param_buff.abrupt_opening=0.0;
			break_param_buff.resistance=1e20;
			base_buff=0.0;

			//delete the comments
			getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_carriageReturn(&myline);
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
				col=functions::count_number_columns(myline);
				my_stream << myline;
				if(this->line_type==_hyd_floodplain_polysegment_type::COASTLINE){
					try{
						if(col==4){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff;
						}
						else if(col==5){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff>> overflow_str;
							overflow_buff=functions::convert_string2boolean(overflow_str);
						}
						else if(col==6){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff>> overflow_str>>poleni_buff;
							overflow_buff=functions::convert_string2boolean(overflow_str);
						}
						else if(col==7){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff>> break_str>>aprupt_fails_str>>break_param_buff.resistance;
							break_buff=functions::convert_string2boolean(break_str);
							break_param_buff.abrupt_fails_flag=functions::convert_string2boolean(aprupt_fails_str);
							if(break_param_buff.abrupt_fails_flag==true){
								//Warning
								ostringstream info;
								info << "Filename              : " << filename << endl;
								info << "Error occurs near line: "<< line_counter << endl;
								info << "Point id              : "<<i+1<<endl;
								Warning msg=this->set_warning(4);
								msg.make_second_info(info.str());
								msg.output_msg(2);

							}

						}
						else if(col==8){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff>> break_str>>aprupt_fails_str>>break_param_buff.resistance >> break_param_buff.abrupt_opening;
							break_buff=functions::convert_string2boolean(break_str);
							break_param_buff.abrupt_fails_flag=functions::convert_string2boolean(aprupt_fails_str);
							if(break_param_buff.abrupt_fails_flag==false){
								//Warning
								ostringstream info;
								info << "Filename              : " << filename << endl;
								info << "Error occurs near line: "<< line_counter << endl;
								info << "Point id              : "<<i+1<<endl;
								Warning msg=this->set_warning(5);
								msg.make_second_info(info.str());
								msg.output_msg(2);

							}

						}
						else if(col==9){
							my_stream >> x_buffer >> y_buffer >> height_buff >> base_buff>> break_str>>aprupt_fails_str>>break_param_buff.resistance >> overflow_str>>poleni_buff;
							break_buff=functions::convert_string2boolean(break_str);
							overflow_buff=functions::convert_string2boolean(overflow_str);
							break_param_buff.abrupt_fails_flag=functions::convert_string2boolean(aprupt_fails_str);
							if(break_param_buff.abrupt_fails_flag==true){
								//Warning
								ostringstream info;
								info << "Filename              : " << filename << endl;
								info << "Error occurs near line: "<< line_counter << endl;
								info << "Point id              : "<<i+1<<endl;
								Warning msg=this->set_warning(4);
								msg.make_second_info(info.str());
								msg.output_msg(2);

							}

						}
						else if(col==10){
							my_stream >> x_buffer >> y_buffer >> height_buff>> base_buff >> break_str>>aprupt_fails_str>>break_param_buff.resistance >> break_param_buff.abrupt_opening >> overflow_str>>poleni_buff;
							break_buff=functions::convert_string2boolean(break_str);
							overflow_buff=functions::convert_string2boolean(overflow_str);
							break_param_buff.abrupt_fails_flag=functions::convert_string2boolean(aprupt_fails_str);
							if(break_param_buff.abrupt_fails_flag==false){
								//Warning
								ostringstream info;
								info << "Filename              : " << filename << endl;
								info << "Error occurs near line: "<< line_counter << endl;
								info << "Point id              : "<<i+1<<endl;
								Warning msg=this->set_warning(5);
								msg.make_second_info(info.str());
								msg.output_msg(2);

							}
						}
						else{
							//Error
							Error msg=this->set_error(7);
							throw msg;
						}
					}catch(Error msg){
						ostringstream info;
						info << "Filename              : " << filename << endl;
						info << "Error occurs near line: "<< line_counter << endl;
						info << "Point id              : "<<i+1<<endl;
						msg.make_second_info(info.str());
						throw msg;
					}
				}
				else{
					try{
						if(col==3){
							my_stream >> x_buffer >> y_buffer >> height_buff ;
						}
						else if(col==4){
							my_stream >> x_buffer >> y_buffer >> height_buff >> overflow_str;
							overflow_buff=functions::convert_string2boolean(overflow_str);
						}
						else if(col==5){
							my_stream >> x_buffer >> y_buffer >> height_buff >> overflow_str>>poleni_buff;
							overflow_buff=functions::convert_string2boolean(overflow_str);
						}
						else{
							//Error
							Error msg=this->set_error(7);
							throw msg;
						}
						base_buff=height_buff-1;
					}
					catch(Error msg){
						ostringstream info;
						info << "Filename              : " << filename << endl;
						info << "Error occurs near line: "<< line_counter << endl;
						info << "Point id              : "<<i+1<<endl;
						msg.make_second_info(info.str());
						throw msg;
					}
				}

				
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << filename << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Point id              : "<<i+1<<endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				//set the point buffer
				point_buff[counter].set_point_coordinate(x_buffer+offset_x, y_buffer+offset_y);
				//set the height the height of the first point is relevant!!			
				if(counter < this->number_points){
					this->line_points[counter].set_point_coordinate(x_buffer+offset_x, y_buffer+offset_y);
					this->line_points[counter].set_abs_height(height_buff);
					this->line_points[counter].set_base_height(base_buff);
					this->line_points[counter].set_break_flag(break_buff);
					this->line_points[counter].set_overflow_flag(overflow_buff);
					this->line_points[counter].set_poleni_fac(poleni_buff);
					this->line_points[counter].set_break_param(break_param_buff);
				}
				counter++;

			}
			else{
				i--;
			}

		}
		//check if all is read
		if(counter!=this->number_points && ifile.eof()!=true){
			ostringstream info;
			info << "Filename              : " << filename << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(4);
			msg.make_second_info(info.str());
			throw msg;
		}

		//check for th !END flag
		do{
		getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_carriageReturn(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		}
		while(myline.empty()==true && ifile.eof()!=true);

		pos=myline.rfind("!END");
		if(pos>=0){
			myline.erase(pos+4);
		}
		//no !END flag is found
		if(myline!="!END"){
			ostringstream info;
			info << "Filename " << filename << endl;
			info << "Warning occurs near line: "<< line_counter << endl;
			Warning msg=this->set_warning(3);
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}



		try{
			//points to polysegment
			this->set_points(point_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Polysegment index :" << this->index << endl;
			info << "Linetype          :" << this->convert_polysegment_type2txt(this->line_type)<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//delete the point_buff
		if(point_buff!=NULL){
			delete []point_buff;
			point_buff=NULL;
		}
	}
	catch(Error msg){
		//delete the point_buff
		if(point_buff!=NULL){
			delete []point_buff;
			point_buff=NULL;
		}
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();

	try{
		this->set_distances2begin();
		this->check_dikeline_points();
	}
	catch(Error msg){
		ostringstream info;
		info << "Filename              : " << filename << endl;
		info << "Error occurs near line: "<< line_counter << endl;
		info << "Dike line name        : "<< this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Create the database table for the polysegment data in a database (static)
void Hyd_Floodplain_Polysegment::create_table(QSqlDatabase *ptr_database){
		if(Hyd_Floodplain_Polysegment::polysegment_table==NULL){
			ostringstream cout;
			cout << "Create polysegment (dike-, coastline) data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fp_polyseg;
			const int num_col=10;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=hyd_label::polyseg_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;


			tab_col[1].name=label::areastate_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].default_value="0";
			tab_col[1].key_flag=true;

			tab_col[2].name=label::measure_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].default_value="0";
			tab_col[2].key_flag=true;

						
			tab_col[3].name=label::applied_flag;
			tab_col[3].type=sys_label::tab_col_type_bool;
			tab_col[3].default_value="true";
			tab_col[3].key_flag=true;

			tab_col[4].name=hyd_label::polyseg_name;
			tab_col[4].type=sys_label::tab_col_type_string;

			tab_col[5].name=hyd_label::polyseg_closed;
			tab_col[5].type=sys_label::tab_col_type_bool;
			tab_col[5].default_value="false";

			tab_col[6].name=hyd_label::polyseg_coast;
			tab_col[6].type=sys_label::tab_col_type_bool;
			tab_col[6].default_value="false";
			tab_col[6].key_flag=true;
			
			tab_col[7].name=hyd_label::genmod_id;
			tab_col[7].type=sys_label::tab_col_type_int;
			tab_col[7].unsigned_flag=true;
			tab_col[7].key_flag=true;

			tab_col[8].name=hyd_label::polyseg_out;
			tab_col[8].type=sys_label::tab_col_type_polyline;

			tab_col[9].name=label::description;
			tab_col[9].type=sys_label::tab_col_type_string;

			try{
				Hyd_Floodplain_Polysegment::polysegment_table= new Tables();
				if(Hyd_Floodplain_Polysegment::polysegment_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Floodplain_Polysegment::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Floodplain_Polysegment::close_table();
				throw msg;
			}

		Hyd_Floodplain_Polysegment::close_table();
		//create point table
		Hyd_Floodplain_Polysegment::create_point_table(ptr_database);
	}
}
//Set the database table for the polysegment data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Floodplain_Polysegment::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Floodplain_Polysegment::polysegment_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fp_polyseg;
		string tab_id_col[10];

		tab_id_col[0]=hyd_label::polyseg_glob_id;
		tab_id_col[1]=label::areastate_id;
		tab_id_col[2]=label::measure_id;
		tab_id_col[3]=hyd_label::polyseg_closed;
		tab_id_col[4]=hyd_label::polyseg_coast;
		tab_id_col[5]=hyd_label::genmod_id;
		tab_id_col[6]=hyd_label::polyseg_out;
		tab_id_col[7]=label::description;
		tab_id_col[8]=label::applied_flag;
		tab_id_col[9]=hyd_label::polyseg_name;

		try{
			Hyd_Floodplain_Polysegment::polysegment_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Floodplain_Polysegment::polysegment_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polysegment::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Floodplain_Polysegment::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the polysegment data (static)
void Hyd_Floodplain_Polysegment::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Floodplain_Polysegment::polysegment_table->delete_data_in_table(ptr_database);

	//delete the data in the polygon point table
	Hyd_Floodplain_Polysegment::delete_point_data_in_table(ptr_database);

}
//Delete data in the database table for the polysegment data by a given system id (static)
void Hyd_Floodplain_Polysegment::delete_data_in_table(QSqlDatabase *ptr_database, _sys_system_id id){
	int number=0;
	QSqlQueryModel results;
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " " ;
	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);


	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::delete_data_in_table(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results.rowCount();

	int glob_id=0;
	QSqlQueryModel del_results;
	//delete the appending points in database table
	for(int i=0; i< number; i++){
		glob_id=results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id)).c_str()).toInt();
		Hyd_Floodplain_Polysegment::delete_point_data_in_table(ptr_database, glob_id);
	
		//delete polysegment
		test_filter.str("");
		test_filter<< "Delete ";
		test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
		test_filter << " where ";
		test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " = " << glob_id << " ";

		Data_Base::database_request(&del_results, test_filter.str(), ptr_database);

		//check the request
		if(del_results.lastError().isValid()){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polysegment::delete_data_in_table(QSqlDatabase *ptr_database, _sys_system_id id)","Invalid database request", "Check the database", 2, false);
			ostringstream info;
			info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
			info << "Table error info: " << del_results.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Close and delete the database table for the polysegment data (static)
void Hyd_Floodplain_Polysegment::close_table(void){
	if(Hyd_Floodplain_Polysegment::polysegment_table!=NULL){
		delete Hyd_Floodplain_Polysegment::polysegment_table;
		Hyd_Floodplain_Polysegment::polysegment_table=NULL;
	}
	//close the point table
	Hyd_Floodplain_Polysegment::close_point_table();
}
//Check if a valid coast-line is set in the database table, for applying the coast model (static)
bool Hyd_Floodplain_Polysegment::check_coast_model_applied_in_database(QSqlDatabase *ptr_database, _sys_system_id id){
	bool flag=false;
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	Sys_Database_Browser my_browser;
	//set table name
	my_browser.set_tablename(Hyd_Floodplain_Polysegment::polysegment_table->get_table_name());
	//set the where-filter
	ostringstream where_filter;
	where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	where_filter << " and ";
	where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr; 
	where_filter << " and ";
	where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast) << " = true" ; 

	my_browser.set_where_filter(where_filter.str());
	//launch the request
	QSqlQueryModel *results;
	results=my_browser.table_to_model(ptr_database);

	if(results->lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::check_coast_model_applied_in_database(QSqlDatabase *ptr_database, _sys_system_id id)","No valid database request", "Check the code", 6, false);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info          : " << results->lastError().text().toStdString() << endl;
		info << "Areastate/Measureno.      : " << id.area_state <<"/"<< id.measure_nr<< endl; 
		msg.make_second_info(info.str());
		throw msg;
	}

	//number of results
	if(results->rowCount()>0){
		return true;
	}
	//check for additional models in the 0 measure-state
	if(id.measure_nr!=id.area_state){
		//set table name
		my_browser.set_tablename(Hyd_Floodplain_Polysegment::polysegment_table->get_table_name());
		//clear filter 
		where_filter.clear();
		where_filter.str("");
		where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
		where_filter << " and ";
		where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << 0; 
		where_filter << " and ";
		where_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast) << " = true" ; 

		my_browser.set_where_filter(where_filter.str());
		//launch the request
		QSqlQueryModel *results;
		results=my_browser.table_to_model(ptr_database);

		if(results->lastError().isValid()==true){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polysegment::check_coast_model_applied_in_database(QSqlDatabase *ptr_database, _sys_system_id id)","No valid database request", "Check the code", 6, false);
			ostringstream info;
			info << "Table Name                : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
			info << "Table error info          : " << results->lastError().text().toStdString() << endl;
			info << "Areastate/Measureno.      : " << id.area_state <<"/"<< id.measure_nr<< endl; 
			msg.make_second_info(info.str());
			throw msg;
		}
		//number of results
		if(results->rowCount()>0){
			return true;
		}
	}

	return flag;

}
//Switch the applied-flag for the section in the database table for a defined system state and id (static)
void Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int glob_id){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	query_string  << " AND ";
	query_string  <<  Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " = " << glob_id ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Switch the applied-flag for the section in the database table for a defined system state and id (static)
void Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::applied_flag) << " = "<< functions::convert_boolean2string(flag);
	query_string  << " WHERE ";
	query_string  << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	query_string  << " AND ";
	query_string  <<  Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr ;
	


	Data_Base::database_request(&query, query_string.str(), ptr_database);
	if(query.lastError().isValid()==true){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::switch_applied_flag_polysegment_table(QSqlDatabase *ptr_database, const _sys_system_id id, const bool flag)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Select and count the number of relevant polysegments in a database table (static)
int Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const int model_id, const bool coast_flag , const bool with_output){
	
	int number=0;
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << "Search for relevant polylines in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "Search for relevant coastline in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_closed) ;
	
	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::genmod_id) << " = " << model_id;
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast) << " = " << functions::convert_boolean2string(coast_flag);
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);


	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results->rowCount();

	if(coast_flag==true && number >1){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","There are 2 coast line found, just 1 is possible", "Check the database table", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << number << " relevant polylines are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "The relevant coast line is found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant polysegments in a database table (static)
int Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag , const bool with_output){
	
	int number=0;
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << "Search for relevant polylines in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "Search for relevant coastline in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::genmod_id) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_closed) ;
	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast) << " = " << functions::convert_boolean2string(coast_flag);
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and (";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " or " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;
	test_filter << " order by " << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);


	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results->rowCount();

	if(coast_flag==true && number >1){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","There are 2 coast line found, just 1 is possible", "Check the database table", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << number << " relevant polylines are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "The relevant coast line is found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Select and count the number of polysegments in a database table (static)
int Hyd_Floodplain_Polysegment::select_polysegments_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const _sys_system_id id, const bool coast_flag , const bool with_output){
	int number=0;
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << "Search for relevant polylines in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "Search for relevant coastline in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::genmod_id) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_closed) ;
	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast) << " = " << functions::convert_boolean2string(coast_flag);
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id) << " = " << id.area_state;
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << "  " ;
	test_filter << " order by " << Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);


	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results->rowCount();

	if(coast_flag==true && number >1){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_polysegments_database(QSqlTableModel *results, const int model_id, const bool coast_flag , const bool with_output)","There are 2 coast line found, just 1 is possible", "Check the database table", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	if(with_output==true && coast_flag==false){
		ostringstream cout;
		cout << number << " relevant polylines are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}
	else if(with_output==true && coast_flag==true){
		ostringstream cout;
		cout << "The relevant coast line is found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Set the polysegment data from a database selection (static)
void Hyd_Floodplain_Polysegment::input_members(const QSqlQueryModel *results, QSqlDatabase *ptr_database, const int index){
	int global_number=0;

	try{
		this->index=index;
		//set the global curve number
		global_number=results->record(index).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id)).c_str()).toInt();
		this->is_closed_flag=results->record(index).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_closed)).c_str()).toBool();
		this->name=results->record(index).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name)).c_str()).toString().toStdString();
		
		
		QSqlQueryModel point_results;
		//number of points
		int number_p=0;
		number_p=Hyd_Floodplain_Polysegment::select_relevant_points_database(&point_results , ptr_database, global_number);
		this->set_number_of_points(number_p);

		Geo_Point *point_buff=NULL;
		double x_buff=0.0;
		double y_buff=0.0;

		//allocate the points
		try{
			point_buff=new Geo_Point[this->number_points];
		}catch(bad_alloc &){
			Error msg=this->set_error(5);
			throw msg;
		}
		_hyd_break_parameters buff;
		//read in points
		for(int i=0; i< this->number_points; i++){
			x_buff=point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_x)).c_str()).toDouble();
			y_buff=point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_y)).c_str()).toDouble();
			point_buff[i].set_point_coordinate(x_buff, y_buff);
			this->line_points[i].set_point_coordinate(x_buff, y_buff);
			this->line_points[i].set_abs_height(point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_h)).c_str()).toDouble());
			this->line_points[i].set_base_height(point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_base)).c_str()).toDouble());
			this->line_points[i].set_poleni_fac(point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_pol)).c_str()).toDouble());
			this->line_points[i].set_overflow_flag(point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_overflow)).c_str()).toBool());
			this->line_points[i].set_break_flag(point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_break)).c_str()).toBool());
			buff.abrupt_fails_flag=point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_abrupt_break)).c_str()).toBool();
			buff.resistance=point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_resistance)).c_str()).toDouble();
			buff.abrupt_opening=point_results.record(i).value((Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_open_width)).c_str()).toDouble();
			this->line_points[i].set_break_param(buff);
		}

		//points to polysegment
		this->set_points(point_buff);

		//delete the point_buff
		if(point_buff!=NULL){
			delete []point_buff;
			point_buff=NULL;
		}
	}
	catch(Error msg){
		ostringstream info;
		info << "Polysegment index    :" << this->index << endl;
		info << "Global Id            :" << global_number << endl;
		info << "Name of the dikeline : " << this->name<<endl;
		info << "Linetype             :" << this->convert_polysegment_type2txt(this->line_type)<< endl;
		info << "Table name           :" << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name()<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	this->set_distances2begin();
	this->check_dikeline_points();

}
//Transfer the polysegment data of a file into a database table
void Hyd_Floodplain_Polysegment::transfer_polysegment2database_table(QSqlDatabase *ptr_database, const int model_id, const bool coast_flag){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Floodplain_Polysegment::polysegment_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Hyd_Floodplain_Polysegment::polysegment_table->maximum_int_of_column(Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id),ptr_database)+1;
			
	//insert row
	model.insertRows(0, 1);
																								
	//insert data
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::genmod_id)).c_str())),model_id);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::areastate_id)).c_str())),this->system_id.area_state);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::measure_id)).c_str())),this->system_id.measure_nr);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(label::applied_flag)).c_str())),true);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_closed)).c_str())),this->is_closed_flag);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name)).c_str())),this->name.c_str());
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_coast)).c_str())),coast_flag);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << id_glob << endl;
		info << "To floodplain model       : " << model_id <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}


	//set a polygon datatype of the polygon to the datbase table	
	//set the query
	Sys_Database_Browser brows_buff;
	brows_buff.set_tablename(Hyd_Floodplain_Polysegment::polysegment_table->get_table_name());
	//set a where filter
	ostringstream buff_filter;
	buff_filter<< Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id) << " = " << id_glob;
	brows_buff.set_where_filter(buff_filter.str());
	//get the query string
	string str_query_buff;
	str_query_buff =brows_buff.get_inserting_polyline_filter(Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_out), this);
	
	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, str_query_buff, ptr_database);
	
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polysegment::polysegment_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << id_glob << endl;
		info << "To floodplain model       : " << model_id <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

	//transfer the point data
	this->transfer_polysegment_point2database_table(ptr_database, id_glob);
}
//Set number of points
void Hyd_Floodplain_Polysegment::set_number_of_points(const int number){
	Geo_Polysegment::set_number_of_points(number);
	this->allocate_dikeline_points();
}
//Set dikeline polysegment by point list
void Hyd_Floodplain_Polysegment::set_dike_line_by_list(Hyd_Floodplain_Dikeline_Point_List *list){

	this->set_number_of_points(list->get_number_points());

	for(int i=0; i< list->get_number_points(); i++){
		this->line_points[i]=*(list->get_ptr_point(i));

	}
	try{
		this->set_dikelinepoints_to_segment();
	
		this->set_distances2begin();
	}catch(Error msg){
		throw msg;
	}
	
}
//Fill marked points with heights
void Hyd_Floodplain_Polysegment::fill_marked_points_heights(const double marker){
	int index_down=-1;
	int index_up=-1;
	double dist_down=0.0;
	double dist_up=0.0;
	double dist_total=0.0;
	double dist_point=0.0;
	double int_h=0.0;

	for(int i=0; i< this->number_points; i++){
		index_down=-1;
		index_up=-1;
		if(this->line_points[i].get_abs_height()==marker){
			dist_point=this->line_points[i].get_distance2begin();
			for(int j=0;j<this->number_points; j++){
				if(this->line_points[j].get_abs_height()!=marker && j<i){
					index_down=j;
				}
				else if(this->line_points[j].get_abs_height()!=marker && j>i){
					if(index_up<0){
						index_up=j;
					}
				}

			}
			if(index_down>=0 && index_down<this->number_points){
				dist_down=this->line_points[index_down].get_distance2begin();
			}
			else{
				dist_down=0.0;
				index_down=0;
			}
			if(index_up>=0 && index_up<this->number_points){
				dist_up=this->line_points[index_up].get_distance2begin();
			}
			else{
				dist_up=this->line_points[this->number_points-1].get_distance2begin();
				index_up=this->number_points-1;
			}

			dist_total=dist_up-dist_down;
			dist_down=dist_point-dist_down;
			dist_up=dist_up-dist_point;
			int_h=(1.0-dist_down/dist_total)*this->line_points[index_down].get_abs_height()+(1.0-dist_up/dist_total)*this->line_points[index_up].get_abs_height();

			this->line_points[i].set_abs_height(int_h);




		}


	}

}
//Get pointer to the dike line points
Hyd_Floodplain_Dikeline_Point* Hyd_Floodplain_Polysegment::get_ptr_dike_line_points(const int index){
	if( index >=0 && index < this->number_points){
		return &(this->line_points[index]);
	}
	else{
		return NULL;
	}
}
///Set the dikeline points to Geo_Segement
void Hyd_Floodplain_Polysegment::set_dikelinepoints_to_segment(void){
	Geo_Point *point_buff=NULL;
	try{
		point_buff=new Geo_Point[this->number_points];
	}catch(bad_alloc &){
		Error msg=this->set_error(8);
		throw msg;
	}

	for(int i=0;i<this->number_points; i++){

		point_buff[i].set_point_coordinate(this->line_points[i].get_xcoordinate(), this->line_points[i].get_ycoordinate());
	}
	this->set_points(point_buff);
	if(point_buff!=NULL){
		delete []point_buff;
		point_buff=NULL;
	}
}
//Set the line index 
void Hyd_Floodplain_Polysegment::set_index(const int index){
	this->index=index;
}
//Get the line index 
int Hyd_Floodplain_Polysegment::get_index(void){
	return this->index;
}
//Get the name of the polysegment
string Hyd_Floodplain_Polysegment::get_name(void){
	return this->name;
}
//Set the name of the polygon
void Hyd_Floodplain_Polysegment::set_name(string nam){
	this->name=nam;
}
//Output the members
void Hyd_Floodplain_Polysegment::output_members(void){
	ostringstream prefix;
	prefix << "PLINE_"<< this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->number_segments>0){
		cout << "Polyline "<< this->name << " No. " << this->index<< " with " << this->number_segments << " segments " << endl;
		cout << "Linetype " << this->convert_polysegment_type2txt(this->line_type) << " is closed: " << functions::convert_boolean2string(this->is_closed_flag)<<endl;
		cout << "Intercepted element type: " << Hyd_Element_Floodplain::convert_elem_type2txt(this->intercepted_elem_type) << endl;
		cout << "Length:" << W(8) << P(2)<< this->get_total_distance() << label::m<<endl; 
		cout << W(3)<< "No. " << W(8) << ",x1"<< label::m << W(8) << ",y1"<< label::m << W(8) << ",x2"<< label::m << W(8) << ",y2" << label::m<< W(8) << ",h" << label::m << W(8) << ",l" << label::m << W(8) << ",l_tot"<< label::m<<endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		double sum_dist=0.0;
		for(int i=0; i< this->number_segments; i++){
			cout << W(3)<< i <<","<< W(12) << P(5)<< FORMAT_FIXED_REAL<< this->my_segment[i].point1.get_xcoordinate() << ",";
			cout << W(13) << P(5) << FORMAT_FIXED_REAL<< this->my_segment[i].point1.get_ycoordinate() << ",";
			cout << W(13) << P(5) << FORMAT_FIXED_REAL<< this->my_segment[i].point2.get_xcoordinate() << ",";
			cout << W(13) << P(5) << FORMAT_FIXED_REAL<< this->my_segment[i].point2.get_ycoordinate() << ",";
			if(this->line_points!=NULL){
				cout << W(13) << P(2) << FORMAT_FIXED_REAL<< this->line_points[i].get_abs_height() << ",";
			}
			else{
				cout << W(20);
			}
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->my_segment[i].get_distance() << ",";
			Sys_Common_Output::output_hyd->output_txt(&cout,true);

			cout << W(13) << P(2) << FORMAT_FIXED_REAL << sum_dist <<endl;
			Sys_Common_Output::output_hyd->output_txt(&cout,true);
			sum_dist=sum_dist+this->my_segment[i].get_distance();
		}
	}
	else{
		cout << " No segments are set ! " << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
	}

	
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output the polysegment to tecplot
void Hyd_Floodplain_Polysegment::output2tecplot(ofstream *output_file){	
	
	if(this->number_segments>0){
		//output a text
		*output_file <<"TEXT X="<<this->my_segment[0].point1.get_xcoordinate() <<", Y="<< this->my_segment[0].point1.get_ycoordinate()<<" ";
		*output_file << "T=\"";
		*output_file<< this->convert_polysegment_type2txt(this->line_type) << " No. " << this->index ;
		if(this->no_flow_flag==true){
			*output_file<<" noflow";	
		}
		if(this->is_closed_flag==true){
			*output_file<<" closed";
		}
		*output_file << "\"";   
		*output_file<<", CS=GRID, C=GREEN, HU=GRID, H=0.5, AN=HEADLEFT, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<(this->my_segment[0].point1.direction_xaxsis(&(this->my_segment[0].point2))) <<"\""<<endl;
		*output_file << endl;
		//output text at each line
		if(this->no_flow_flag==false){
			for(int i=0; i< this->number_segments; i++){
				*output_file <<"TEXT X="<<(this->my_segment[i].point1.get_xcoordinate()+this->my_segment[i].point2.get_xcoordinate())*0.5 <<", Y="<< (this->my_segment[i].point1.get_ycoordinate()+this->my_segment[i].point2.get_ycoordinate())*0.5 <<" ";
				*output_file << "T=\"Height ";
				*output_file<< this->line_points[i].get_abs_height() << label::m;
				*output_file << "\"";   
				*output_file<<", CS=GRID, C=GREEN, HU=GRID, H=0.3, AN=HEADCENTER, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<(this->my_segment[i].point1.direction_xaxsis(&(this->my_segment[i].point2))) <<"\""<<endl;
				*output_file << endl;
			}
		}

		//output geometry
		*output_file << "#Polysegment Type:  " << this->convert_polysegment_type2txt(this->line_type) << "; No. " << this->index << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=GREEN, LT=0.75, L=DOTTED, CS=GRID, ZN=1" << endl;
		*output_file <<"1" << endl;
		*output_file <<this->number_segments+1 << endl;
		*output_file << "#coordinates x  y" << endl;
		for(int i=0; i< this->number_segments; i++){
			*output_file << this->my_segment[i].point1.get_xcoordinate() << "  " << this->my_segment[i].point1.get_ycoordinate() << endl;		
		}
		//last point
		*output_file << this->my_segment[this->number_segments-1].point2.get_xcoordinate() << "  " << this->my_segment[this->number_segments-1].point2.get_ycoordinate() << endl;
		*output_file << endl;
	}
}
//Set the no_flow_flag
void Hyd_Floodplain_Polysegment::set_no_flow_flag(const bool flag){
	this->no_flow_flag=flag;
}
//Get the no_flow_flag
bool Hyd_Floodplain_Polysegment::get_no_flow_flag(void){
	return this->no_flow_flag;
}
//Get the height of an segment for a given index
double Hyd_Floodplain_Polysegment::get_segments_height(const int index){
	return this->line_points[index].get_abs_height();
}
//Get the Poleni-factor of an segment for a given index
double Hyd_Floodplain_Polysegment::get_segments_poleni(const int index){
	return this->line_points[index].get_poleni_fac();
}
//Get a no overflow flag of an segment for a given index
bool Hyd_Floodplain_Polysegment::get_segments_no_overflow(const int index){
	if(this->no_flow_flag==true){
		return true;
	}
	else{
		return !this->line_points[index].get_overflow_flag();
	}

}
//Set the type of the line (_hyd_floodplain_polysegment_type)
void Hyd_Floodplain_Polysegment::set_line_type(_hyd_floodplain_polysegment_type type){
	this->line_type=type;
	if(this->line_type==_hyd_floodplain_polysegment_type::COASTLINE){
		this->name=hyd_label::coast_line;
	}
	else if(this->line_type==_hyd_floodplain_polysegment_type::DIKELINE){
		this->name=hyd_label::dike_line;
	}
	else if(this->line_type==_hyd_floodplain_polysegment_type::FP_BOUNDARYLINE){
		this->name=hyd_label::fp_boundaryline;
	}
	else if(this->line_type==_hyd_floodplain_polysegment_type::RIVERBANKLINE){
		this->name=hyd_label::river_bankline;
	}
	else if(this->line_type==_hyd_floodplain_polysegment_type::RIVERLINE){
		this->name=hyd_label::river_line;
	}
}
//Get the type of the line (_hyd_floodplain_polysegment_type)
_hyd_floodplain_polysegment_type Hyd_Floodplain_Polysegment::get_line_type(void){
	return this->line_type;
}
//Set the element type (_hyd_elem_type) to set to elements which are intercepted by this line
void Hyd_Floodplain_Polysegment::set_element_type(_hyd_elem_type type){
	this->intercepted_elem_type=type;
	if(this->intercepted_elem_type==_hyd_elem_type::RIVER_ELEM){
		this->no_flow_flag=true;
		this->index=0;
	}
}
//Get the element type (_hyd_elem_type) to set to elements which are intercepted by this line
_hyd_elem_type Hyd_Floodplain_Polysegment::get_element_type(void){
	return this->intercepted_elem_type;
}
//Set if the line is closed (true) or not (false) per string
void Hyd_Floodplain_Polysegment::set_is_closed_str(string txt){
	_Hyd_Parse_IO::string2lower(&txt);
	if(txt=="closed"){
		this->is_closed_flag=true;
	}
	else{ 
		if(this->is_closed_flag!=true){
			this->is_closed_flag=false;
		}
	}
}
//(static) Conversion function for output from enumerator _hyd_floodplain_polysegment_type to text
string Hyd_Floodplain_Polysegment::convert_polysegment_type2txt(_hyd_floodplain_polysegment_type type){
	string txt;	
	switch(type){
		case _hyd_floodplain_polysegment_type::COASTLINE:
			txt=hyd_label::coast_line;
			break;
		case _hyd_floodplain_polysegment_type::DIKELINE:
			txt=hyd_label::dike_line;
			break;
		case _hyd_floodplain_polysegment_type::RIVERLINE:
			txt=hyd_label::river_line;
			break;
		case _hyd_floodplain_polysegment_type::RIVERBANKLINE:
			txt=hyd_label::river_bankline;
			break;
		case _hyd_floodplain_polysegment_type::FP_BOUNDARYLINE:
			txt=hyd_label::fp_boundaryline;
			break;
		default:
			txt=label::unknown_type;
	}
	return txt;
}
//Clone the polysegment
void Hyd_Floodplain_Polysegment::clone_polysegment(Hyd_Floodplain_Polysegment *seg){
	this->no_flow_flag=seg->no_flow_flag;
	this->line_type=seg->line_type;
	this->intercepted_elem_type=seg->intercepted_elem_type;
	this->index=seg->index;
	Geo_Polysegment::copy_polysegment(seg);

	if(seg->line_points!=NULL){
		this->allocate_dikeline_points();
		for(int i=0; i< this->number_points; i++){
			this->line_points[i]=seg->line_points[i];
		}
	}
}
//Get the pointer to the upwards dikeline point
Hyd_Floodplain_Dikeline_Point* Hyd_Floodplain_Polysegment::get_ptr_upwards_point(const int segment_id){
	if(segment_id<0 || segment_id>this->number_segments-1){
		return NULL;
	}
	else{
		return &(this->line_points[segment_id]);
	}
}
//Get the pointer to the downwards dikeline point
Hyd_Floodplain_Dikeline_Point* Hyd_Floodplain_Polysegment::get_ptr_downwards_point(const int segment_id){
	if(segment_id<0 || segment_id>this->number_segments-1){
		return NULL;
	}
	else if(segment_id==this->number_segments-1){
		return &(this->line_points[0]);
	}
	else{
		return &(this->line_points[segment_id+1]);
	}
}
//Find the pointer to the dikeline points up and down the dikeline to a given station
bool Hyd_Floodplain_Polysegment::find_ptr_up_down_point(const double station, Hyd_Floodplain_Dikeline_Point **up, Hyd_Floodplain_Dikeline_Point **down){
	*up=NULL;
	*down=NULL;


	//the given station is not in the coast dikeline
	if(station < 0.0){
		*up=&(this->line_points[0]);
		return false;
	}
	if(station >this->get_total_distance()){
		*down=&(this->line_points[this->number_points-1]);
		return false;
	}


	//find the point up and down the coastline
	for(int i=0; i< this->number_points; i++){
		if(i>0 && station<this->line_points[i].get_distance2begin()){
			*up=&(this->line_points[i]);
			*down=&(this->line_points[i-1]);
			break;
		}
		//station is directly at a dikeline point
		if(abs(station-this->line_points[i].get_distance2begin())<constant::meter_epsilon){
			*up=&(this->line_points[i]);
			*down=&(this->line_points[i]);
			break;
		}
	}
	return true;
}
//Get a polyline of the dikeline points, which are represented by an fpl-section
void Hyd_Floodplain_Polysegment::get_polyline_fpl_section(Geo_Polysegment *polyline,Hyd_Floodplain_Dikeline_Point *start, Hyd_Floodplain_Dikeline_Point *end){

	try{
		this->set_part_point_list(end,start);
	}
	catch(Error msg){
		throw msg;
	}


	Geo_Point *buffer=NULL;
	buffer=new Geo_Point[this->number_part_points];

	for(int i=0; i< this->number_part_points; i++){
		buffer[i]=*(this->list_part_points[i]);
	}
	polyline->set_number_of_points(this->number_part_points);
	polyline->set_points(buffer);
	if(buffer!=NULL){
		delete []buffer;
	}
}
//Idealise the points of the dikeline, which are represented by an fpl-section, with the data of the fpl-section
void Hyd_Floodplain_Polysegment::idealise_points_by_fpl(Hyd_Floodplain_Dikeline_Point *start, Hyd_Floodplain_Dikeline_Point *end, _hyd_idealisation_parameters ideal_params, _hyd_breach_start_parameters *start_params, const int section_id){
	//determine mid-point as breach starting point
	Geo_Polysegment buffer;
	//here also the list of the involved profiles are set
	this->get_polyline_fpl_section(&buffer, start, end);
	//set the start parameters
	start_params->id_related_station=start->get_number();
	start_params->distance2related=buffer.get_total_distance()/2.0;

	for(int i=0; i< this->number_part_points; i++){
		if(i<this->number_part_points-1){
			this->list_part_points[i]->idealise_points_fpl(ideal_params, section_id,false);
		}
		else{
			this->list_part_points[i]->idealise_points_fpl(ideal_params, section_id,true);
		}
	}
}
//Get the number of involved points to a fpl-section
int Hyd_Floodplain_Polysegment::get_number_involved_points(void){
	return this->number_part_points;
}
//Get a pointer to the involved points by a given index
Hyd_Floodplain_Dikeline_Point* Hyd_Floodplain_Polysegment::get_ptr_involved_points(const int index){
	if(index<0 || index>=this->number_part_points){
		return NULL;
	}
	else{
		return this->list_part_points[index];
	}
}
//Set distances of the dikeline points to the beginning of the polysegement
void Hyd_Floodplain_Polysegment::set_distances2begin(void){
	for(int i=0; i< this->number_points; i++){
		try{
			this->line_points[i].set_distance2begin(this->get_distance_along_polysegment(&this->line_points[i]));
		}
		catch(Error msg){
			ostringstream info;
			info << "Point id             : " << i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//___________
//private
//Allocate the points of the dikeline
void Hyd_Floodplain_Polysegment::allocate_dikeline_points(void){
	this->delete_dikeline_points();
	try{
		this->line_points=new Hyd_Floodplain_Dikeline_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(6);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number_points; i++){
		this->line_points[i].set_number(i);
		this->line_points[i].set_systemid(this->system_id);
	}
}
//Delete the points of the dikeline
void Hyd_Floodplain_Polysegment::delete_dikeline_points(void){
	if(this->line_points!=NULL){
		delete []this->line_points;
		this->line_points=NULL;
	}
}
//Allocate the list of the pointer of a part of the dikeline points
void Hyd_Floodplain_Polysegment::allocate_part_point_list(void){
	this->delete_part_point_list();
	try{
		this->list_part_points=new Hyd_Floodplain_Dikeline_Point*[number_part_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(20);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init
	for(int i=0; i< number_part_points; i++){
		this->list_part_points[i]=NULL;
	}
}
//Delete the list of pointer of a part of the dikeline points
void Hyd_Floodplain_Polysegment::delete_part_point_list(void){
	if(this->list_part_points!=NULL){
		delete []this->list_part_points;
		this->list_part_points=NULL;
	}
}
//Set the list of the pointer of a part of the dikeline points by two given points
void Hyd_Floodplain_Polysegment::set_part_point_list(Hyd_Floodplain_Dikeline_Point *up, Hyd_Floodplain_Dikeline_Point *down){
	this->delete_part_point_list();
	this->number_part_points=0;

	if(up==NULL || down==NULL){
		return;
	}
	else{
		int counter=0;
		//count number of point
		for(int i=0; i< this->number_points; i++){
			if(this->line_points[i].get_distance2begin()>=down->get_distance2begin() && this->line_points[i].get_distance2begin()<=up->get_distance2begin()){
				counter++;
			}
		}
		this->number_part_points=counter;
		this->allocate_part_point_list();
		counter=0;
		for(int i=0; i< this->number_points; i++){
			if(this->line_points[i].get_distance2begin()>=down->get_distance2begin() && this->line_points[i].get_distance2begin()<=up->get_distance2begin()){
				this->list_part_points[counter]=&(this->line_points[i]);
				counter++;
			}
		}
	}
}
//Create the database table for the polysegment point data in a database (static)
void Hyd_Floodplain_Polysegment::create_point_table(QSqlDatabase *ptr_database){
		if(Hyd_Floodplain_Polysegment::polysegment_point_table==NULL){
			ostringstream cout;
			cout << "Create floodplain polysegment point data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fp_polyseg_point;
			const int num_col=14;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].primary_key_flag=false;
				tab_col[i].unsigned_flag=false;
			}

			tab_col[0].name=hyd_label::polyseg_point_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].primary_key_flag=true;
			tab_col[0].unsigned_flag=true;

			tab_col[1].name=hyd_label::polyseg_glob_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].key_flag=true;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=hyd_label::polyseg_point_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].key_flag=true;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=hyd_label::polyseg_point_x;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].default_value="0.0";

			tab_col[4].name=hyd_label::polyseg_point_y;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].default_value="0.0";

			tab_col[5].name=hyd_label::polyseg_point_h;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].default_value="0.0";

			tab_col[6].name=hyd_label::polyseg_point_base;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].default_value="0.0";

			tab_col[7].name=hyd_label::polyseg_point_overflow;
			tab_col[7].type=sys_label::tab_col_type_bool;
			tab_col[7].default_value="true";

			tab_col[8].name=hyd_label::polyseg_point_pol;
			tab_col[8].type=sys_label::tab_col_type_double;
			tab_col[8].default_value="0.577";
			tab_col[8].unsigned_flag=true;

			tab_col[9].name=hyd_label::polyseg_point_break;
			tab_col[9].type=sys_label::tab_col_type_bool;
			tab_col[9].default_value="false";

			tab_col[10].name=hyd_label::polyseg_point_abrupt_break;
			tab_col[10].type=sys_label::tab_col_type_bool;
			tab_col[10].default_value="false";

			tab_col[11].name=hyd_label::polyseg_point_resistance;
			tab_col[11].type=sys_label::tab_col_type_double;
			tab_col[11].default_value="10000000.0";
			tab_col[11].unsigned_flag=true;

			tab_col[12].name=hyd_label::polyseg_point_open_width;
			tab_col[12].type=sys_label::tab_col_type_double;
			tab_col[12].default_value="0.0";
			tab_col[12].unsigned_flag=true;

			tab_col[13].name=hyd_label::polyseg_point;
			tab_col[13].type=sys_label::tab_col_type_point;

			

			try{
				Hyd_Floodplain_Polysegment::polysegment_point_table= new Tables();
				if(Hyd_Floodplain_Polysegment::polysegment_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Floodplain_Polysegment::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Floodplain_Polysegment::close_point_table();
				throw msg;
			}

		Hyd_Floodplain_Polysegment::close_point_table();
	}
}
//Set the database table for the polysegment point data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Floodplain_Polysegment::set_point_table(QSqlDatabase *ptr_database){
	if(Hyd_Floodplain_Polysegment::polysegment_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fp_polyseg_point;
		string tab_id_col[14];

		tab_id_col[0]=hyd_label::polyseg_glob_id;
		tab_id_col[1]=hyd_label::polyseg_point_id;
		tab_id_col[2]=hyd_label::polyseg_point_x;
		tab_id_col[3]=hyd_label::polyseg_point_y;
		tab_id_col[4]=hyd_label::polyseg_point_h;
		tab_id_col[5]=hyd_label::polyseg_point_glob_id;
		tab_id_col[6]=hyd_label::polyseg_point_overflow;
		tab_id_col[7]=hyd_label::polyseg_point_pol;
		tab_id_col[8]=hyd_label::polyseg_point_break;
		tab_id_col[9]=hyd_label::polyseg_point_abrupt_break;
		tab_id_col[10]=hyd_label::polyseg_point_resistance;
		tab_id_col[11]=hyd_label::polyseg_point_open_width;
		tab_id_col[12]=hyd_label::polyseg_point;
		tab_id_col[13]=hyd_label::polyseg_point_base;


		try{
			Hyd_Floodplain_Polysegment::polysegment_point_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Floodplain_Polysegment::polysegment_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polysegment::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Floodplain_Polysegment::close_point_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the polysegment point data (static)
void Hyd_Floodplain_Polysegment::delete_point_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Floodplain_Polysegment::polysegment_point_table->delete_data_in_table(ptr_database);

}
//Delete all data in the database table for the polysegment point data (static)
void Hyd_Floodplain_Polysegment::delete_point_data_in_table(QSqlDatabase *ptr_database, const int glob_id){
	try{
		Hyd_Floodplain_Polysegment::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	QSqlQueryModel del_results;
	ostringstream test_filter;
	
	//delete polysegment
	test_filter.str("");
	test_filter<< "Delete ";
	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_glob_id) << " = " << glob_id << " ";

	Data_Base::database_request(&del_results, test_filter.str(), ptr_database);

	//check the request
	if(del_results.lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::delete_point_data_in_table(QSqlDatabase *ptr_database, const int glob_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name() << endl;
		info << "Table error info: " << del_results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Transfer the polysegment point data of a file into a database table
void Hyd_Floodplain_Polysegment::transfer_polysegment_point2database_table(QSqlDatabase *ptr_database, const int poly_id){
	//mysql query with the query_model
	QSqlQuery query_buff(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polysegment::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=Hyd_Floodplain_Polysegment::polysegment_point_table->maximum_int_of_column(Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_glob_id),ptr_database)+1;
	
	ostringstream query_header;
	query_header << "INSERT INTO  " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name();
	query_header <<" ( ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_glob_id) <<" , ";

	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_glob_id) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_id) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_x) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_y) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_h) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_base) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_overflow) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_pol) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_break) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_abrupt_break) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_resistance) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_open_width) <<" , ";
	query_header << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point) <<" ) ";


	query_header << " VALUES ";
	ostringstream query_data;
	

	if(this->number_segments>0){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << poly_id <<" , ";
		query_data << 0<<" , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->my_segment[0].point1.get_xcoordinate() <<" , ";
		query_data << this->my_segment[0].point1.get_ycoordinate() <<" , ";
		query_data.clear();
		query_data << this->line_points[0].get_abs_height() <<" , ";
		query_data << this->line_points[0].get_base_height() <<" , ";
		query_data << functions::convert_boolean2string(this->line_points[0].get_overflow_flag()) <<" , ";
		query_data << this->line_points[0].get_poleni_fac() <<" , ";
		query_data << functions::convert_boolean2string(this->line_points[0].get_break_flag()) <<" , ";
		query_data << functions::convert_boolean2string(this->line_points[0].get_break_param().abrupt_fails_flag) <<" , ";
		query_data << this->line_points[0].get_break_param().resistance <<" , ";
		query_data << this->line_points[0].get_break_param().abrupt_opening <<" , ";
		query_data <<Geo_Point::get_point2sql_string(this->my_segment[0].point1.get_xcoordinate(), this->my_segment[0].point1.get_ycoordinate());
		query_data <<" ) " << " ,";
		glob_id++;

	}

	for(int i=0; i< this->number_segments; i++){
		if(this->is_closed_flag==true && i==(this->number_segments-1)){
			break;
		}
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << poly_id <<" , ";
		query_data << i+1 <<" , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->my_segment[i].point2.get_xcoordinate() <<" , ";
		query_data << this->my_segment[i].point2.get_ycoordinate() <<" , ";
		query_data.clear();
		if(i<this->number_segments-1){
			query_data << this->line_points[i+1].get_abs_height() <<" , ";
			query_data << this->line_points[i+1].get_base_height() <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[i+1].get_overflow_flag()) <<" , ";
			query_data << this->line_points[i+1].get_poleni_fac() <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[i+1].get_break_flag()) <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[i+1].get_break_param().abrupt_fails_flag) <<" , ";
			query_data << this->line_points[i+1].get_break_param().resistance <<" , ";
			query_data << this->line_points[i+1].get_break_param().abrupt_opening <<" , ";
		}
		else{
			query_data << this->line_points[this->number_segments-1].get_abs_height() <<" , ";
			query_data << this->line_points[this->number_segments-1].get_base_height() <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[this->number_segments-1].get_overflow_flag()) <<" , ";
			query_data << this->line_points[this->number_segments-1].get_poleni_fac() <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[this->number_segments-1].get_break_flag()) <<" , ";
			query_data << functions::convert_boolean2string(this->line_points[this->number_segments-1].get_break_param().abrupt_fails_flag) <<" , ";
			query_data << this->line_points[this->number_segments-1].get_break_param().resistance <<" , ";
			query_data << this->line_points[this->number_segments-1].get_break_param().abrupt_opening <<" , ";
		}
		query_data <<Geo_Point::get_point2sql_string(this->my_segment[i].point2.get_xcoordinate(), this->my_segment[i].point2.get_ycoordinate());
		query_data <<" ) " << " ,";
		glob_id++;
			
	}

	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Polysegment Id            : " << poly_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}
}
//Close and delete the database table for the polysegment point data (static)
void Hyd_Floodplain_Polysegment::close_point_table(void){
	if(Hyd_Floodplain_Polysegment::polysegment_point_table!=NULL){
		delete Hyd_Floodplain_Polysegment::polysegment_point_table;
		Hyd_Floodplain_Polysegment::polysegment_point_table=NULL;
	}
}
//Select and count the number of relevant polysegment points in a database table (static)
int Hyd_Floodplain_Polysegment::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int global_segment_id){
	int number=0;
	try{
		Hyd_Floodplain_Polysegment::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_x) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_y) << " , " ;
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_h) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_base) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_pol) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_overflow) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_break) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_abrupt_break) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_resistance) << " , ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_open_width) << "  ";

	test_filter << " from " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_glob_id) << " = " << global_segment_id;
	test_filter << " order by " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_column_name(hyd_label::polyseg_point_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polysegment::select_relevant_points_database(QSqlTableModel *results , const int global_segment_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polysegment::polysegment_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results->rowCount();


	return number;
}
//Check the dikeline points
void Hyd_Floodplain_Polysegment::check_dikeline_points(void){

	try{
		for(int i=0; i< this->number_points; i++){
			this->line_points[i].check_dikeline_point();
		}
	}
	catch(Error msg){
		ostringstream info;
		info<< "Name of the dikeline : " << this->name<<endl;
		info <<"Linetype             :" << this->convert_polysegment_type2txt(this->line_type)<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Hyd_Floodplain_Polysegment::set_error(const int err_type){
	string place="Hyd_Floodplain_Polysegment::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not open file
			place.append("input_members(const int index, const string filename)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 1://wrong input sequenze
			place.append("input_members(const int index, const string filename)");
			reason="Wrong input by reading the index and the number of points";
			help="Check the file";
			type=5;
			break;
		case 2://do not found the index
			place.append("input_members(const int index, const string filename)");
			reason="Can not find the given index";
			help="Check the file and the given index";
			type=5;
			break;
		case 3://can not read in the x-ycoordinates properly
			place.append("input_members(const int index, const string filename)");
			reason="Can not read in the x-, y-coordinates and height properly of the dikeline_polysegment";
			help="Check the coordinates and height in file";
			type=5;
			break;
		case 4://not all points are found
			place.append("input_members(const int index, const string filename)");
			reason="Not all points for the polysegment are found";
			help="Check the number of points and the given point coordinates";
			info << "Number of searched points " << this->number_segments << endl;
			type=5;
			break;
		case 5://can not allocate the point buffer
			place.append("input_members(const int index, const string filename)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
		case 6://can not allocate the point buffer
			place.append("allocate_dikeline_points(void)");
			reason="Can not allocate the memory for the dikeline points";
			help="Check the memory";
			type=10;
		case 7://not all info there
			place.append("input_members(const int index, const string filename)");
			reason="Not all relevant point data are found in file";
			help="Check the point data in file";
			type=5;
		case 8://can not allocate the point buffer
			place.append("set_dikelinepoints_to_segment(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
Warning Hyd_Floodplain_Polysegment::set_warning(const int warn_type){
		string place="Hyd_Floodplain_Polysegment::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_polysegment2database_table(QSqlDatabase *ptr_database, const int model_id, const bool coast_flag)");
			reason="Can not submit the floodplain polysegment data to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted the data
			place.append("transfer_polysegment2database_table(QSqlDatabase *ptr_database, const int model_id, const bool coast_flag)");
			reason="Can not submit the floodplain polysegment data as a polyline data type to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted the data
			place.append("transfer_polysegment_point2database_table(QSqlDatabase *ptr_database, const int poly_id)");
			reason="Can not submit the floodplain polygon point data to the database";
			help="Check the database";
			type=2;
			break;
		case 3://no !END-flag is found in file
			place.append("input_members(const int index, const string filename)");
			reason="The point data block of the polysegment is not closed with !END in file";
			help="Check the file and the number of point specification in file";
			type=1;
			break;
		case 4://no aprubt breaching is possibnle
			place.append("input_members(const int index, const string filename)");
			reason="There not enough data given for an abrupt breaching (abrupt_fails=true)";
			help="Check the breaching information of the point";
			reaction="A breaching in the point following segment is not taken into account";
			type=1;
			break;
		case 5://no aprubt breaching is possibnle
			place.append("input_members(const int index, const string filename)");
			reason="There are to many data given for an continuous breaching (abrupt_fails=false)";
			help="Check the breaching information of the point";
			reaction="A breaching in the point following segment is not taken into account";
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