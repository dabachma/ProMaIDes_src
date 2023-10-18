#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplain_Polygon.h"

//init static members
Tables *Hyd_Floodplain_Polygon::polygon_table=NULL;
Tables *Hyd_Floodplain_Polygon::polygon_point_table=NULL;

//constructor
Hyd_Floodplain_Polygon::Hyd_Floodplain_Polygon(void){
	this->index=-1;
	this->elementtype_inside=_hyd_elem_type::STANDARD_ELEM;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplain_Polygon), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Floodplain_Polygon::~Hyd_Floodplain_Polygon(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplain_Polygon), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Input the members per file
void Hyd_Floodplain_Polygon::input_members(const int index, const string filename){
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
	

	this->index=index;

	try{
		do{
			//search for the begin of the curve
			do{
				getline(ifile, myline,'\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
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
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			}
			while(myline.empty()==true);
			//read it in
			my_stream << myline;
			string typ_buff;
			my_stream >> found_index >>found_points;
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              : " << filename << endl;
				info << "Error occurs near line: "<< line_counter << endl;
				Error msg=this->set_error(1);
				msg.make_second_info(info.str());
				throw msg;
			}
			my_stream.clear();
			my_stream.str("");

			//compare it with the given index
			if(found_index==index){
				found_flag=true;
				try{
					this->set_number_points(found_points);
				}
				catch(Error msg){
					ostringstream info;
					info << "Polygon index         : " << this->index << endl;
					info << "Polygon type          : " << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) <<endl;
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
			info << "Searched index of the polygon: " << this->index << endl;
			info << "Polygon type                 : " << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) <<endl;
			info << "Filename                     : " << filename << endl;
			Error msg=this->set_error(2);
			msg.make_second_info(info.str());
			throw msg;
		}

		int counter=0;
		double x_buffer=0.0;
		double y_buffer=0.0;
		Geo_Point *point_buff=NULL;
		try{
			point_buff=new Geo_Point[this->number_segments];
		}catch(bad_alloc &){
			Error msg=this->set_error(5);
			throw msg;
		}


		//read in the bound points
		for(int i=0; i<this->number_segments; i++){

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
				my_stream >> x_buffer >> y_buffer;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << filename << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				
				point_buff[counter].set_point_coordinate(x_buffer, y_buffer);
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
		if(counter!=this->number_segments){
			ostringstream info;
			info << "Filename              : " << filename << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(4);
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
			info << "Filename " << filename << endl;
			info << "Warning occurs near line: "<< line_counter << endl;
			Warning msg=this->set_warning(3);
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}

		try{
			//points to polygon
			this->set_points(point_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Polygon index " << this->index << endl;
			info << "Polygon type " << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) <<endl;
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
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();


}
//Create the database table for the polygon data in a database (static)
void Hyd_Floodplain_Polygon::create_table(QSqlDatabase *ptr_database){
		if(Hyd_Floodplain_Polygon::polygon_table==NULL){
			ostringstream cout;
			cout << "Create floodplain polygon data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fp_polygon;
			const int num_col=5;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=hyd_label::polygon_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;
			
			tab_col[1].name=hyd_label::genmod_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=label::applied_flag;
			tab_col[2].type=sys_label::tab_col_type_bool;
			tab_col[2].default_value="true";
			tab_col[2].key_flag=true;

			tab_col[3].name=hyd_label::polygon_out;
			tab_col[3].type=sys_label::tab_col_type_polygon;

			tab_col[4].name=label::description;
			tab_col[4].type=sys_label::tab_col_type_string;


			try{
				Hyd_Floodplain_Polygon::polygon_table= new Tables();
				if(Hyd_Floodplain_Polygon::polygon_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Floodplain_Polygon::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Floodplain_Polygon::close_table();
				throw msg;
			}

		Hyd_Floodplain_Polygon::close_table();

		//create point table
		Hyd_Floodplain_Polygon::create_point_table(ptr_database);
	}

}
//Set the database table for the polygon data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Floodplain_Polygon::set_table(QSqlDatabase *ptr_database){
	if(Hyd_Floodplain_Polygon::polygon_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fp_polygon;
		string tab_id_col[5];

		tab_id_col[0]=hyd_label::polygon_glob_id;
		tab_id_col[1]=hyd_label::genmod_id;
		tab_id_col[2]=hyd_label::polygon_out;
		tab_id_col[3]=label::description;
		tab_id_col[4]=label::applied_flag;

		try{
			Hyd_Floodplain_Polygon::polygon_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Floodplain_Polygon::polygon_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polygon::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Floodplain_Polygon::close_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the polygon data (static)
void Hyd_Floodplain_Polygon::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polygon::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Floodplain_Polygon::polygon_table->delete_data_in_table(ptr_database);

	//delete the data in the polygon point table
	Hyd_Floodplain_Polygon::delete_point_data_in_table(ptr_database);

}
//Close and delete the database table for the polygon data (static)
void Hyd_Floodplain_Polygon::close_table(void){
	if(Hyd_Floodplain_Polygon::polygon_table!=NULL){
		delete Hyd_Floodplain_Polygon::polygon_table;
		Hyd_Floodplain_Polygon::polygon_table=NULL;
	}
	//close the point table
	Hyd_Floodplain_Polygon::close_point_table();
}
//Select and count the number of relevant polygons in a database table (static)
int Hyd_Floodplain_Polygon::select_relevant_polygons_database(QSqlQueryModel *results, QSqlDatabase *ptr_database ,const int model_id, const bool with_output){
	int number=0;

	try{
		Hyd_Floodplain_Polygon::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant noflow-polygons in database ..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id);
	test_filter << " from " << Hyd_Floodplain_Polygon::polygon_table->get_table_name();
	test_filter << " where ";
	test_filter << Hyd_Floodplain_Polygon::polygon_table->get_column_name(label::applied_flag) << " = true";
	test_filter << " and ";
	test_filter << Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::genmod_id) << " = " << model_id;
	test_filter << " order by " << Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polygon::select_relevant_polygons_database(QSqlTableModel *results,const int model_id, const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
		
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant noflow-polygons are found in database" << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	return number;
}
//Transfer the polygon data of a file into a database table
void Hyd_Floodplain_Polygon::transfer_polygons2database_table(QSqlDatabase *ptr_database, const int model_id){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polygon::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Hyd_Floodplain_Polygon::polygon_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Hyd_Floodplain_Polygon::polygon_table->maximum_int_of_column(Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id),ptr_database)+1;
			
	//insert row
	model.insertRows(0, 1);

	//insert data
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::genmod_id)).c_str())),model_id);
	model.setData(model.index(0,model.record().indexOf((Hyd_Floodplain_Polygon::polygon_table->get_column_name(label::applied_flag)).c_str())), true);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << id_glob << endl;
		info << "To floodplain model       : " << model_id <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}


	//set a polygon datatype of the polygon to the datbase table	
	//set the query
	Sys_Database_Browser brows_buff;
	brows_buff.set_tablename(Hyd_Floodplain_Polygon::polygon_table->get_table_name());
	//set a where filter
	ostringstream buff_filter;
	buff_filter<< Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id) << " = " << id_glob;
	brows_buff.set_where_filter(buff_filter.str());
	//get the query string
	string str_query_buff;
	str_query_buff =brows_buff.get_inserting_polygon_filter(Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_out), this);
	
	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, str_query_buff, ptr_database);
	
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Hyd_Floodplain_Polygon::polygon_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << id_glob << endl;
		info << "To floodplain model       : " << model_id <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

	//transfer the point data
	this->transfer_polygon_point2database_table(ptr_database, id_glob);
}
//Set the polygon data from a database selection
void Hyd_Floodplain_Polygon::input_members(const QSqlQueryModel *results, QSqlDatabase *ptr_database, const int index){
	int global_number=0;
	Geo_Point *point_buff=NULL;
	try{
		this->index=index;
		//set the global curve number
		global_number=results->record(index).value((Hyd_Floodplain_Polygon::polygon_table->get_column_name(hyd_label::polygon_glob_id)).c_str()).toInt();
		this->elementtype_inside=_hyd_elem_type::NOFLOW_ELEM;
		
		QSqlTableModel point_results(0,*ptr_database);
		//number of points
		int number_p=0;
		number_p=Hyd_Floodplain_Polygon::select_relevant_points_database(&point_results , global_number);
		this->set_number_points(number_p);

		
		double x_buff=0.0;
		double y_buff=0.0;

		//allocate the points
		try{
			point_buff=new Geo_Point[this->number_segments];
		}catch(bad_alloc &){
			Error msg=this->set_error(5);
			throw msg;
		}
		
		//read in points
		for(int i=0; i< this->number_segments; i++){
			x_buff=point_results.record(i).value((Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_x)).c_str()).toDouble();
			y_buff=point_results.record(i).value((Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_y)).c_str()).toDouble();
			point_buff[i].set_point_coordinate(x_buff, y_buff);
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
		if(point_buff!=NULL){
			delete []point_buff;
			point_buff=NULL;
		}
		ostringstream info;
		info << "Polygon index     :" << this->index << endl;
		info << "Global Id         :" << global_number << endl;
		info << "Elements inside   :" << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside)<< endl;
		info << "Table name        :" << Hyd_Floodplain_Polygon::polygon_table->get_table_name()<< endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//output the members
void Hyd_Floodplain_Polygon::output_members(void){
	ostringstream prefix;
	prefix << "POLY_"<< this->index<<"> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->number_segments>0){
		cout << "Polygon No. " << this->index<< " with " << this->number_segments << " segments " << endl;
		cout << "Type of elements inside: " << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) << endl; 
		cout << "Area" << W(8) << P(2)<< this->calculate_area() << label::squaremeter << "Perimeter" << W(8) << P(2)<< this->calculate_perimeter()<< label::m<<endl; 
		cout << W(3)<< "No. " << W(8) << ",x1" << label::m << W(8) << ",y1"<< label::m  << W(8) << ",x2" << label::m << W(8) << ",y2" << label::m << W(8) << ",l" << label::m <<endl; 
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		for(int i=0; i< this->number_segments; i++){
			cout << W(3)<< i <<","<< W(12) << P(2)<< FORMAT_FIXED_REAL<< this->segments[i].point1.get_xcoordinate() << ","  ;
			cout << W(13) << P(2)<< FORMAT_FIXED_REAL << this->segments[i].point1.get_ycoordinate() << ","  ;
			cout << W(13) << P(2)<< FORMAT_FIXED_REAL << this->segments[i].point2.get_xcoordinate() << ","  ;
			cout << W(13) << P(2)<< FORMAT_FIXED_REAL << this->segments[i].point2.get_ycoordinate() << ","  ;
			cout << W(13) << P(2) << FORMAT_FIXED_REAL << this->segments[i].get_distance()<< endl;
			Sys_Common_Output::output_hyd->output_txt(&cout,true);
		}
	}
	else{
		cout << " No points are set ! " << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
	}

	
	Sys_Common_Output::output_hyd->rewind_userprefix();


}
//Output the polygon to tecplot
void Hyd_Floodplain_Polygon::output2tecplot(ofstream *output_file){

	if(this->number_segments>0){
		//output a text
		*output_file <<"TEXT X="<<this->segments[0].point1.get_xcoordinate() <<", Y="<< this->segments[0].point1.get_ycoordinate()<<" ";
		*output_file << "T=\"";
		*output_file<< Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) << " No. " << this->index ;
		*output_file << "\"";   
		*output_file<<", CS=GRID, C=BLACK, HU=GRID, H=0.5, AN=HEADLEFT, BX=FILLED, BXF=WHITE, BXO=WHITE, ZN=1, A="<<this->segments[0].point1.direction_xaxsis(&(this->segments[0].point2)) <<"\""<<endl;
		*output_file << endl;
		//output geometry
		*output_file << "#Polyon Type:  " << Hyd_Element_Floodplain::convert_elem_type2txt(this->elementtype_inside) << "; No. " << this->index << endl;
		*output_file <<"GEOMETRY  X=" <<0.0 <<", Y="<<0.0<<", T=LINE, C=BLACK, LT=0.5, L=DASHDOT, CS=GRID, ZN=1" << endl;
		*output_file <<"1" << endl;
		*output_file <<this->number_segments+1 << endl;
		*output_file << "#coordinates x  y" << endl;
		for(int i=0; i< this->number_segments; i++){
			*output_file << this->segments[i].point1.get_xcoordinate() << "  " << this->segments[i].point1.get_ycoordinate() << endl;		
		}
		//last point
		*output_file << this->segments[this->number_segments-1].point2.get_xcoordinate() << "  " << this->segments[this->number_segments-1].point2.get_ycoordinate() << endl;
		*output_file << endl;
	}
}
//Set the element type (_hyd_elem_type) of the elements which are inside
void Hyd_Floodplain_Polygon::set_elementtype_inside(_hyd_elem_type elementtype){
	this->elementtype_inside=elementtype;
	if(this->elementtype_inside==_hyd_elem_type::RIVER_ELEM || this->elementtype_inside==_hyd_elem_type::COAST_ELEM){
		this->index=0;
	}
}
//Get the element type (_hyd_elem_type) of the elements which are inside
_hyd_elem_type Hyd_Floodplain_Polygon::get_elementtype_inside(void){
	return this->elementtype_inside;
}
//Clone the floodplain-polygon
void Hyd_Floodplain_Polygon::clone_polygon(Hyd_Floodplain_Polygon *poly){
	this->index=poly->index;
	this->elementtype_inside=poly->elementtype_inside;

	this->set_number_points(poly->number_segments);

	for(int i=0; i< this->number_segments; i++){
		this->segments[i]=poly->segments[i];

	}
}
//____________
//private
//Create the database table for the polygon point data in a database (static)
void Hyd_Floodplain_Polygon::create_point_table(QSqlDatabase *ptr_database){
		if(Hyd_Floodplain_Polygon::polygon_point_table==NULL){
			ostringstream cout;
			cout << "Create floodplain polygon point data database table..." << endl ;
			Sys_Common_Output::output_hyd->output_txt(&cout);
			//make specific input for this class
			const string tab_name=hyd_label::tab_fp_polygon_point;
			const int num_col=5;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].primary_key_flag=false;
				tab_col[i].unsigned_flag=false;
			}

			tab_col[0].name=hyd_label::polygon_point_glob_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].primary_key_flag=true;
			tab_col[0].unsigned_flag=true;

			tab_col[1].name=hyd_label::polygon_glob_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].key_flag=true;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=hyd_label::polygon_point_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].key_flag=true;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=hyd_label::polygon_point_x;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].default_value=="0.0";

			tab_col[4].name=hyd_label::polygon_point_y;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].default_value=="0.0";
			


			try{
				Hyd_Floodplain_Polygon::polygon_point_table= new Tables();
				if(Hyd_Floodplain_Polygon::polygon_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_hyd->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Hyd_Floodplain_Polygon::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Hyd_Floodplain_Polygon::close_point_table();
				throw msg;
			}

		Hyd_Floodplain_Polygon::close_point_table();
	}

}
//Set the database table for the polygon point data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Floodplain_Polygon::set_point_table(QSqlDatabase *ptr_database){
	if(Hyd_Floodplain_Polygon::polygon_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_fp_polygon_point;
		string tab_id_col[5];

		tab_id_col[0]=hyd_label::polygon_glob_id;
		tab_id_col[1]=hyd_label::polygon_point_id;
		tab_id_col[2]=hyd_label::polygon_point_x;
		tab_id_col[3]=hyd_label::polygon_point_y;
		tab_id_col[4]=hyd_label::polygon_point_glob_id;

		try{
			Hyd_Floodplain_Polygon::polygon_point_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Floodplain_Polygon::polygon_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Floodplain_Polygon::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Floodplain_Polygon::close_point_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the polygon point data (static)
void Hyd_Floodplain_Polygon::delete_point_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polygon::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Floodplain_Polygon::polygon_point_table->delete_data_in_table(ptr_database);

}
//Transfer the polygon point data of a file into a database table
void Hyd_Floodplain_Polygon::transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id){
	//mysql query with the query_model
	QSqlQuery query_buff(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Floodplain_Polygon::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=Hyd_Floodplain_Polygon::polygon_point_table->maximum_int_of_column(Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_glob_id),ptr_database)+1;

	ostringstream query_header;
	query_header << "INSERT INTO  " << Hyd_Floodplain_Polygon::polygon_point_table->get_table_name();
	query_header <<" ( ";
	query_header << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_glob_id) <<" , ";

	query_header << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_glob_id) <<" , ";
	query_header << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_id) <<" , ";
	query_header << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_x) <<" , ";
	query_header << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_y) <<" ) ";


	query_header << " VALUES ";
	ostringstream query_data;
	
	for(int i=0; i< this->number_segments; i++){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << poly_id <<" , ";
		query_data << i+1 <<" , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->segments[i].point1.get_xcoordinate() <<" , ";
		query_data << this->segments[i].point1.get_ycoordinate() ;
		query_data.clear();
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
		info << "Table Name                : " << Hyd_Floodplain_Polygon::polygon_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Polygon Id                : " << poly_id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

}
//Close and delete the database table for the polygon point data (static)
void Hyd_Floodplain_Polygon::close_point_table(void){
	if(Hyd_Floodplain_Polygon::polygon_point_table!=NULL){
		delete Hyd_Floodplain_Polygon::polygon_point_table;
		Hyd_Floodplain_Polygon::polygon_point_table=NULL;
	}

}
//Select and count the number of relevant polygon points in a database table (static)
int Hyd_Floodplain_Polygon::select_relevant_points_database(QSqlTableModel *results , const int global_polygon_id){
	int number=0;
	try{
		Hyd_Floodplain_Polygon::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Hyd_Floodplain_Polygon::polygon_point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_glob_id) << " = " << global_polygon_id;

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Floodplain_Polygon::select_relevant_points_database(QSqlTableModel *results , const int global_polygon_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Floodplain_Polygon::polygon_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Hyd_Floodplain_Polygon::polygon_point_table->get_column_name(hyd_label::polygon_point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();


	return number;
}
//set the error
Error Hyd_Floodplain_Polygon::set_error(const int err_type){
		string place="Hyd_Floodplain_Polygon::";
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
			reason="Can not read in the x-, y-coordinates properly of the noflow-polygon";
			help="Check the coordinates in file";
			type=5;
			break;
		case 4://not all points are found
			place.append("input_members(const int index, const string filename)");
			reason="Not all points for the noflow polygon are found";
			help="Check the number of points and the given point coordinates";
			info << "Number of searched points " << this->number_segments << endl;
			type=5;
			break;
		case 5://can not allocate the point buffer
			place.append("input_members(const int index, const string filename)");
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
Warning Hyd_Floodplain_Polygon::set_warning(const int warn_type){
		string place="Hyd_Floodplain_Polygon::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_polygons2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the floodplain polygon data to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted the data
			place.append("transfer_polygons2database_table(QSqlDatabase *ptr_database)");
			reason="Can not submit the floodplain polygon as a polygon data type to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted the data
			place.append("transfer_polygon_point2database_table(QSqlDatabase *ptr_database, const int poly_id)");
			reason="Can not submit the floodplain polygon point data  to the database";
			help="Check the database";
			type=2;
			break;
		case 3://no !END-flag is found in file
			place.append("input_members(const int index, const string filename)");
			reason="The point data block of the polygon is not closed with !END in file";
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