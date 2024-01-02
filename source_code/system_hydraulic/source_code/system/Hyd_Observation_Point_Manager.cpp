#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Observation_Point_Manager.h"

//init static members
Tables *Hyd_Observation_Point_Manager::obs_point_table=NULL;

//Default constructor
Hyd_Observation_Point_Manager::Hyd_Observation_Point_Manager(void){

	this->number_obs_point=0;
	this->obs_point=NULL;

	this->number_obs_rv=0;
	this->number_obs_fp=0;

	this->counter_used=0;


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Observation_Point_Manager), _sys_system_modules::HYD_SYS);
}
//Default destructor
Hyd_Observation_Point_Manager::~Hyd_Observation_Point_Manager(void){


	this->delete_obs_point();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Observation_Point_Manager), _sys_system_modules::HYD_SYS);
}
//____________________
//public
//Create the database table for the observation point data in a database (static)
void Hyd_Observation_Point_Manager::create_table(QSqlDatabase *ptr_database){
	if(Hyd_Observation_Point_Manager::obs_point_table==NULL){
		ostringstream cout;
		cout << "Create observation point table..." << endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//make specific input for this class
		const string tab_name=hyd_label::tab_obs_point;
		const int num_col=5;
		_Sys_data_tab_column tab_col[num_col];
		//init
		for(int i=0; i< num_col; i++){
			tab_col[i].key_flag=false;
			tab_col[i].unsigned_flag=false;
			tab_col[i].primary_key_flag=false;
		}


		tab_col[0].name=hyd_label::obs_point_id;
		tab_col[0].type=sys_label::tab_col_type_int;
		tab_col[0].unsigned_flag=true;
		tab_col[0].primary_key_flag=true;


		tab_col[1].name=hyd_label::obs_point_x;
		tab_col[1].type=sys_label::tab_col_type_double;
		tab_col[1].default_value="0.0";

		tab_col[2].name=hyd_label::obs_point_y;
		tab_col[2].type=sys_label::tab_col_type_double;
		tab_col[2].default_value="0.0";

		tab_col[3].name=hyd_label::obs_point_name;
		tab_col[3].type=sys_label::tab_col_type_string;

		tab_col[4].name=hyd_label::obs_point;
		tab_col[4].type=sys_label::tab_col_type_point;

		try{
			Hyd_Observation_Point_Manager::obs_point_table= new Tables();
			if(Hyd_Observation_Point_Manager::obs_point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::hyd)==false){
				cout << " Table exists" << endl ;
				Sys_Common_Output::output_hyd->output_txt(&cout);
			};
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Observation_Point_Manager::create_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Hyd_Observation_Point_Manager::close_table();
			throw msg;
		}
	}
}
//Set the database table for the observation point data: it sets the table name and the name of the columns and allocate them (static)
void Hyd_Observation_Point_Manager::set_table(QSqlDatabase *ptr_database, const bool not_close){
	if(Hyd_Observation_Point_Manager::obs_point_table==NULL){
		//make specific input for this class
		const string tab_id_name=hyd_label::tab_obs_point;
		string tab_id_col[5];
		tab_id_col[0]=hyd_label::obs_point_id;
		tab_id_col[1]=hyd_label::obs_point_x;
		tab_id_col[2]=hyd_label::obs_point_y;
		tab_id_col[3]=hyd_label::obs_point_name;
		tab_id_col[4]=hyd_label::obs_point;


		try{
			Hyd_Observation_Point_Manager::obs_point_table= new Tables(tab_id_name, tab_id_col, sizeof(tab_id_col)/sizeof(tab_id_col[0]));
			Hyd_Observation_Point_Manager::obs_point_table->set_name(ptr_database, _sys_table_type::hyd);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Hyd_Observation_Point_Manager::set_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			if(not_close==false){
				Hyd_Observation_Point_Manager::close_table();
			}
			throw msg;
		}
	}
}
//Delete all data in the database table for the observation point data (static)
void Hyd_Observation_Point_Manager::delete_data_in_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Observation_Point_Manager::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Hyd_Observation_Point_Manager::obs_point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the observation point data (static)
void Hyd_Observation_Point_Manager::close_table(void){
	if(Hyd_Observation_Point_Manager::obs_point_table!=NULL){
		delete Hyd_Observation_Point_Manager::obs_point_table;
		Hyd_Observation_Point_Manager::obs_point_table=NULL;
	}
}
//Get the number of observation points
int Hyd_Observation_Point_Manager::get_number_obs_points(void){
	return this->number_obs_point;
}
//Get a pointer to the  observation point list
Hyd_Observation_Point* Hyd_Observation_Point_Manager::get_ptr_obs_point(void){
	return this->obs_point;
}
//Read in the observation points per file
void Hyd_Observation_Point_Manager::input_obs_point(const string file){

	if(file==label::not_set){
		return;
	}

	//set prefix for output
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	//output
	ostringstream cout;
	cout << "Read the observation points from file " << file << " ..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	ifstream ifile;
	//open file
	ifile.open(file.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter=0;
	string myline;
	int pos=-1;
	stringstream my_stream;
	Geo_Point point_buff;


	
		
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
	my_stream >> this->number_obs_point; 
	if(my_stream.fail()==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << my_stream.str() << endl;
		info << "Filename              : " << file << endl;
		info << "Input must be         : number_points"<< endl;
		info << "Error occurs near line: "<< line_counter << endl;
		Error msg=this->set_error(2);
		msg.make_second_info(info.str());
		throw msg;
	}
	my_stream.clear();
	my_stream.str("");


	int counter=0;
	int col=0;
	double x_buffer=0.0;
	double y_buffer=0.0;
	string name;
	Geo_Point_List list_buffer;

	

	//read in the bound points
	do{
		name=label::not_set;
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
			if(col==2){
				my_stream >> x_buffer >> y_buffer;
			}
			else if(col>=3){
				my_stream >> x_buffer >> y_buffer >> name;
			}
					
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              : " << file << endl;
				info << "Error occurs near line: "<< line_counter << endl;
				Error msg=this->set_error(2);
				msg.make_second_info(info.str());
				throw msg;
			}
			my_stream.clear();
			my_stream.str("");
			counter++;
			point_buff.set_point_coordinate(x_buffer, y_buffer);
			point_buff.set_point_name(name);
			list_buffer.set_new_point(&point_buff);

		}
	}
	while(counter<this->number_obs_point && ifile.eof()!=true);

	//check if all is read
	if(counter!=(this->number_obs_point)){
		ostringstream info;
		info << "Filename              : " << file << endl;
		info << "Error occurs near line: "<< line_counter << endl;
		Error msg=this->set_error(3);
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
		info << "Filename " << file << endl;
		info << "Warning occurs near line: "<< line_counter << endl;
		Warning msg=this->set_warning(0);
		msg.make_second_info(info.str());
		msg.output_msg(2);
	}

	try{
		//points to obs points
		this->allocate_obs_point();
		for(int i=0; i<this->number_obs_point; i++){
			this->obs_point[i].set_geo_point_info(list_buffer.get_ptr_point(i));
		}
	}
	catch(Error msg){
		throw msg;
	}


	//close file
	ifile.close();
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
	
}
//Read in the obervation point per database
void Hyd_Observation_Point_Manager::input_obs_point(QSqlDatabase *ptr_database, const bool output){

	//set prefix for output
	ostringstream prefix;
	if(output==true){
		prefix << "OBS> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	}

	//output
	ostringstream cout;
	if(output==true){
		cout << "Read the observation points from database..."<< endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
	}

	QSqlQueryModel buffer;
	try{
		this->number_obs_point=this->select_relevant_points_database(&buffer,ptr_database);
		this->allocate_obs_point();
	}
	catch(Error msg){
		if(output==true){
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	

	double x_buff=0.0;
	double y_buff=0.0;
	string name;

	for(int i=0; i< this->number_obs_point; i++){
		x_buff=buffer.record(i).value((Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_x)).c_str()).toDouble();
		y_buff=buffer.record(i).value((Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_y)).c_str()).toDouble();
		this->obs_point[i].set_point_coordinate(x_buff, y_buff);
		name=buffer.record(i).value((Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_name)).c_str()).toString().toStdString();
		this->obs_point[i].set_point_name(name);
	}

	if(output==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Transfer the data of the observation points to database table
void Hyd_Observation_Point_Manager::transfer_obs_points2database(QSqlDatabase *ptr_database, const string file){
	//output
	ostringstream cout;
	cout << "Transfer the observation points from a file to a database table ..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		//read in the globals per file
		this->input_obs_point(file);
	}
	catch(Error msg){
		throw msg;
		
	}
	if(this->number_obs_point==0){
		return;
	}

	//set them to database
	//mysql query with the query_model
	QSqlQuery query_buff(*ptr_database);
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Observation_Point_Manager::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	int glob_id=Hyd_Observation_Point_Manager::obs_point_table->maximum_int_of_column(Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_id),ptr_database)+1;
	
	ostringstream query_header;
	query_header << "INSERT INTO  " << Hyd_Observation_Point_Manager::obs_point_table->get_table_name();
	query_header <<" ( ";
	query_header << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_id) <<" , ";

	query_header << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_x) <<" , ";
	query_header << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_y) <<" , ";
	query_header << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_name) <<" , ";
	query_header << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point) <<" ) ";



	query_header << " VALUES ";
	ostringstream query_data;
	
	for(int i=0; i< this->number_obs_point; i++){

		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << FORMAT_FIXED_REAL << P(8);
		query_data << this->obs_point[i].get_xcoordinate() <<" , ";
		query_data << this->obs_point[i].get_ycoordinate() <<" , ";
		
		query_data <<"'"<< this->obs_point[i].get_point_name() <<"' , ";
		query_data.clear(); 
		query_data <<Geo_Point::get_point2sql_string(this->obs_point[i].get_xcoordinate(), this->obs_point[i].get_ycoordinate());
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
		info << "Table Name                : " << Hyd_Observation_Point_Manager::obs_point_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);	
	}

}
//Initialize the observation points
void Hyd_Observation_Point_Manager::init_obs_points(const int num_rv, Hyd_Model_River *river, const int num_fp, Hyd_Model_Floodplain *floodplain, const int no_output, const int no_internal){
	//set prefix for output
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Initialize the observation points..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	bool found_flag=false;

	for(int j=0; j<this->number_obs_point; j++){
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		found_flag=false;
		//check the river models
		for(int i=0; i<num_rv; i++){
			found_flag=this->obs_point[j].init_obs_point_river(&(river[i]),i);
			if(found_flag==true){
				break;
			}

		}
		if(found_flag==false){
			//check the floodplain models
			for(int i=0; i<num_fp; i++){
				found_flag=this->obs_point[j].init_obs_point_floodplain(&(floodplain[i]),i);
				if(found_flag==true){
					break;
				}
			}
		}

		if(found_flag==true){
			this->obs_point[j].set_number_time_point(no_output, no_internal);
		}
	}

	this->count_number_rv_fp_obs_point();

	//check the points
	for(int i=0; i< this->number_obs_point; i++){
		if(this->obs_point[i].get_index_elem_prof()<0){
			//Set warning
			ostringstream info;
			info << "Point id      : " << i << endl;
			info << "Name          : " << this->obs_point[i].get_point_name() << endl;
			info << "x-coordinate  : "<< this->obs_point[i].get_xcoordinate() << label::m<<endl;
			info << "y-coordinate  : "<< this->obs_point[i].get_ycoordinate() << label::m<<endl;
			Warning msg=this->set_warning(2);
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
///Initialize the observation points of temperature models
void Hyd_Observation_Point_Manager::init_temp_obs_points(const int num_rv, HydTemp_Model *temp_model, const int no_output, const int no_internal) {
	//set prefix for output
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Initialize the observation points of the temperature model..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	bool found_flag = false;

	for (int j = 0; j < this->number_obs_point; j++) {
		Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
		found_flag = false;
		//check the river models
		for (int i = 0; i < num_rv; i++) {
			found_flag = this->obs_point[j].init_temp_obs_point_river(&(temp_model[i]), i);
			if (found_flag == true) {
				break;
			}

		}

		if (found_flag == true) {
			this->obs_point[j].set_number_time_point(no_output, no_internal);
		}
	}

	this->count_number_rv_fp_obs_point();

	//check the points
	for (int i = 0; i < this->number_obs_point; i++) {
		if (this->obs_point[i].get_index_elem_prof() < 0) {
			//Set warning
			ostringstream info;
			info << "Point id      : " << i << endl;
			info << "Name          : " << this->obs_point[i].get_point_name() << endl;
			info << "x-coordinate  : " << this->obs_point[i].get_xcoordinate() << label::m << endl;
			info << "y-coordinate  : " << this->obs_point[i].get_ycoordinate() << label::m << endl;
			Warning msg = this->set_warning(2);
			msg.make_second_info(info.str());
			msg.output_msg(2);
		}
	}

	Hyd_Multiple_Hydraulic_Systems::check_stop_thread_flag();
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Output members
void Hyd_Observation_Point_Manager::output_setted_members(void){
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Observation points..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	cout << "GENERAL"<<endl ;
	cout << "Number of observation points (total)      : " << this->number_obs_point<<endl ;
	if(this->number_obs_rv>0){
		cout << "Number of observation points (River)      : " << this->number_obs_rv<<endl ;
	}
	if(this->number_obs_fp>0){
		cout << "Number of observation points (Floodplain) : " << this->number_obs_fp<<endl ;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	if(this->number_obs_point>0){
		cout << "POINTS"<<endl ;
		cout << " Id. " <<W(10)<< "    x-coor [m]      "<<W(18)<< "        y-coor [m] " << W(10) << "            Name    ";
		cout << W(10)<<"      FP(true)      " <<W(10)<< "      Index Model      "<<W(10)<< "    Index Elem/Prof   " <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
		for(int i=0; i< this->number_obs_point; i++){
			this->obs_point[i].output_setted_members(&cout, i);
			Sys_Common_Output::output_hyd->output_txt(&cout,true);

		}
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output members of the temperature observation points
void Hyd_Observation_Point_Manager::output_setted_temp_members(void) {
	ostringstream cout;
	//set prefix for output
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	cout << "Temperature observation points..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	cout << "GENERAL" << endl;
	cout << "Number of observation points (total)      : " << this->number_obs_point << endl;
	if (this->number_obs_rv > 0) {
		cout << "Number of observation points (River)      : " << this->number_obs_rv << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(&cout);
	if (this->number_obs_point > 0) {
		cout << "POINTS" << endl;
		cout << " Id. " << W(10) << "    x-coor [m]      " << W(18) << "        y-coor [m] " << W(10) << "            Name    ";
		cout << W(10) << "      FP(true)      " << W(10) << "      Index Model      " << W(10) << "    Index Elem/Prof   " << endl;
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
		for (int i = 0; i < this->number_obs_point; i++) {
			this->obs_point[i].output_setted_members(&cout, i);
			Sys_Common_Output::output_hyd->output_txt(&cout, true);

		}
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Syncronize the observation points
void Hyd_Observation_Point_Manager::syncron_obs_points(const double time_point){
	this->counter_used++;
	for(int i=0; i< this->number_obs_point; i++){
		this->obs_point[i].synchron_obs_point(time_point);
	}
}
//Syncronize the observation points for temoerature calculation
void Hyd_Observation_Point_Manager::syncron_temp_obs_points(const double time_point) {
	this->counter_used++;
	for (int i = 0; i < this->number_obs_point; i++) {
		this->obs_point[i].synchron_temp_obs_point(time_point);
	}
}
//Output the data of the observation points to tecplot file
void Hyd_Observation_Point_Manager::output_obs_points2tecplot_file(const string filename_rv, const string filename_fp){

	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Output the observation points to tecplot file..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	if(this->number_obs_rv>0){
		this->output_obs_point_rv2file(filename_rv);
	}
	if(this->number_obs_fp>0){
		this->output_obs_point_fp2file(filename_fp);
	}

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the data of the observation points to ParaView / cvs file
void Hyd_Observation_Point_Manager::output_obs_points2paraview_file(const string filename_rv, const string filename_fp) {

	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Output the observation points to paraview file..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	if (this->number_obs_rv > 0) {
		this->output_obs_point_rv2csvfile(filename_rv);
	}
	if (this->number_obs_fp > 0) {
		this->output_obs_point_fp2csvfile(filename_fp);
	}

	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the data of the temperature observation points to ParaView / cvs file
void Hyd_Observation_Point_Manager::output_temp_obs_points2paraview_file(const string filename_rv) {
	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Output the temperature observation points to paraview file..." << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	if (this->number_obs_rv > 0) {
		this->output_temp_obs_point_rv2csvfile(filename_rv);
	}

	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Clear the observation points
void Hyd_Observation_Point_Manager::clear_obs_points(void){
	for(int i=0; i< this->number_obs_point; i++){
		this->obs_point[i].clear_obs_point();
	}
	this->counter_used=0;
}
//Clone the observation points
void Hyd_Observation_Point_Manager::clone_obs_points(Hyd_Observation_Point_Manager *src,  Hyd_Model_River *river,  Hyd_Model_Floodplain *floodplain){

	ostringstream prefix;
	prefix << "OBS> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Clone the observation points..."<< endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);


	try{
		this->number_obs_point=src->number_obs_point;
		this->number_obs_rv=src->number_obs_rv;
		this->number_obs_fp=src->number_obs_fp;
		this->counter_used=src->counter_used;

		this->allocate_obs_point();
		for(int i=0; i< this->number_obs_point; i++){
			this->obs_point[i].clone_obs_points(&(src->obs_point[i]),river, floodplain);
		}




	}
	catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Total reset of the manager
void Hyd_Observation_Point_Manager::total_reset(void){
	this->delete_obs_point();
	this->number_obs_point=0;
	this->number_obs_rv=0;
	this->number_obs_fp=0;
	this->counter_used=0;
}
//__________________
//private
//Allocate the obeservation points
void Hyd_Observation_Point_Manager::allocate_obs_point(void){
		try{
			this->obs_point=new Hyd_Observation_Point[this->number_obs_point];

		}catch(bad_alloc &){
			Error msg=this->set_error(0);
			throw msg;
		}
}
//Delete the observation points
void Hyd_Observation_Point_Manager::delete_obs_point(void){
	if(this->obs_point!=NULL){
		delete []this->obs_point;
		this->obs_point=NULL;
	}
	this->number_obs_rv=0;
	this->number_obs_fp=0;
}
//Select and count the number of relevant polysegment points in a database table (static)
int Hyd_Observation_Point_Manager::select_relevant_points_database(QSqlQueryModel *results, QSqlDatabase *ptr_database){
	int number=0;
	try{
		Hyd_Observation_Point_Manager::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";	
	test_filter << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_x) << " , " ;
	test_filter << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_y) << " , " ;
	test_filter << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_name) << "  ";
	
	test_filter << " from " << Hyd_Observation_Point_Manager::obs_point_table->get_table_name();
	test_filter << " order by " << Hyd_Observation_Point_Manager::obs_point_table->get_column_name(hyd_label::obs_point_id).c_str();

	Data_Base::database_request(results, test_filter.str(), ptr_database);

	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Hyd_Observation_Point_Manager::select_relevant_points_database(QSqlTableModel *results)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Hyd_Observation_Point_Manager::obs_point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
	number= results->rowCount();


	return number;
}
//Count number river- and floodplain obeservation points
void Hyd_Observation_Point_Manager::count_number_rv_fp_obs_point(void){
	for(int i=0; i< this->number_obs_point; i++){
		if(this->obs_point[i].get_model_flag()==true){
			this->number_obs_fp++;
		}
		else{
			this->number_obs_rv++;
		}
	}

}
//Output the observation points of river models to file
void Hyd_Observation_Point_Manager::output_obs_point_rv2file(const string file){

	ofstream output;

	output.open(file.c_str());
	//check if it is open
	if(output.is_open()==false){
		Error msg=this->set_error(4);
		ostringstream info;
		info << "File name " << file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	output << "TITLE = "<< "\" " <<  " Observation points River models " << " \"" << endl;
	output << "VARIABLES = " <<endl;
	output<< "\" " << " Time " << label::sec ;
	output<< "\" \"" << " h " << label::m ;
	output<< "\" \"" << " s " << label::m ;
	output<< "\" \"" << " v " << label::m_per_sec;
	output<< "\" \"" << " Fr " << label::no_unit;
	output<< "\" \"" << " Q " << label::qm_per_sec;
	output<< "\" " <<  endl << endl;

	for(int i=0; i< this->number_obs_point; i++){
		if(this->obs_point[i].get_model_flag()==false){
			this->obs_point[i].output_obs_point2file(&output, this->counter_used);
		}
	}

	output.close();

}
//Output the observation points of floodplain models to file
void Hyd_Observation_Point_Manager::output_obs_point_fp2file(const string file){

	ofstream output;
	output.open(file.c_str());
	//check if it is open
	if(output.is_open()==false){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "File name " << file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//output the file header
	output << "TITLE = "<< "\" " <<  " Observation points Floodplain models " << " \"" << endl;
	output << "VARIABLES = " <<endl;
	output<< "\" " << " Time " << label::sec ;
	output<< "\" \"" << " h " << label::m ;
	output<< "\" \"" << " s " << label::m ;
	output<< "\" \"" << " v " << label::m_per_sec;
	output<< "\" \"" << " v_x " << label::m_per_sec;
	output<< "\" \"" << " v_y " << label::m_per_sec;
	output<< "\" \"" << " ds_dt " << label::m_per_min;
	output<< "\" \"" << " ds_dt_coupling " << label::m_per_sec;
	output<< "\" " <<  endl << endl;

	for(int i=0; i< this->number_obs_point; i++){
		if(this->obs_point[i].get_model_flag()==true){
			this->obs_point[i].output_obs_point2file(&output, this->counter_used);
		}
	}


	output.close();

}
//Output the observation points of river models to csv file
void Hyd_Observation_Point_Manager::output_obs_point_rv2csvfile(const string file) {
	
	for (int i = 0; i < this->number_obs_point; i++) {
		string buff_name = file;
		if (this->obs_point[i].get_model_flag() == false) {
			ofstream output;
			buff_name += "_";
			buff_name += this->obs_point[i].get_point_name();
			buff_name += hyd_label::csv;

			output.open(buff_name.c_str());
			//check if it is open
			if (output.is_open() == false) {
				Error msg = this->set_error(4);
				ostringstream info;
				info << "File name " << buff_name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			//output the file header
			output << " Time " << label::sec<<",";
			output << " h " << label::m << ",";
			output << " s " << label::m << ",";
			output << " v " << label::m_per_sec << ",";
			output << " Fr " << label::no_unit << ",";
			output << " Q " << label::qm_per_sec ;
			output <<  endl;

			this->obs_point[i].output_obs_point2csvfile(&output, this->counter_used);
			output.close();
		}	
	}
}
//Output the observation points of floodplain models to csvfile
void Hyd_Observation_Point_Manager::output_obs_point_fp2csvfile(const string file) {
	
	for (int i = 0; i < this->number_obs_point; i++) {
		string buff_name = file;
		if (this->obs_point[i].get_model_flag() == true) {
			ofstream output;
			buff_name += "_";
			buff_name += this->obs_point[i].get_point_name();
			buff_name += hyd_label::csv;
			output.open(buff_name.c_str());
			//check if it is open
			if (output.is_open() == false) {
				Error msg = this->set_error(5);
				ostringstream info;
				info << "File name " << buff_name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			//output the file header
			output  << " Time " << label::sec << ",";
			output  << " h " << label::m << ",";
			output  << " s " << label::m << ",";
			output  << " v " << label::m_per_sec << ",";
			output  << " v_x " << label::m_per_sec << ",";
			output  << " v_y " << label::m_per_sec << ",";
			output  << " ds_dt " << label::m_per_min << ",";
			output  << " ds_dt_coupling " << label::m_per_sec;
			output   << endl;

	
			this->obs_point[i].output_obs_point2csvfile(&output, this->counter_used);
			output.close();
		}
	}
}
//Output the temperature observation points of river models to csv file
void Hyd_Observation_Point_Manager::output_temp_obs_point_rv2csvfile(const string file) {
	for (int i = 0; i < this->number_obs_point; i++) {
		string buff_name = file;
		if (this->obs_point[i].get_model_flag() == false) {
			ofstream output;
			buff_name += "_";
			buff_name += this->obs_point[i].get_point_name();
			buff_name += hyd_label::csv;

			output.open(buff_name.c_str());
			//check if it is open
			if (output.is_open() == false) {
				Error msg = this->set_error(4);
				ostringstream info;
				info << "File name " << buff_name << endl;
				msg.make_second_info(info.str());
				throw msg;
			}

			//output the file header
			output << " Time " << label::sec << ",";
			output << " T " << label::kelvin << "";
			//output << " s " << label::m << ",";
			//output << " v " << label::m_per_sec << ",";
			//output << " Fr " << label::no_unit << ",";
			//output << " Q " << label::qm_per_sec;
			output << endl;

			this->obs_point[i].output_temp_obs_point2csvfile(&output, this->counter_used);
			output.close();
		}
	}

}
//set the error
Error Hyd_Observation_Point_Manager::set_error(const int err_type){
	string place="Hyd_Observation_Point_Manager::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;
	switch (err_type){
		case 0://bad alloc
			place.append("allocate_obs_point(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open file
			place.append("input_obs_point(const string file)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 2://wrong input sequenze
			place.append("input_obs_point(const string file)");
			reason="Wrong input by reading the number of observation points";
			help="Check the file";
			type=5;
			break;
		case 3://not all points are found
			place.append("input_obs_point(const string file)");
			reason="Not all observation points are found";
			help="Check the number of points and the given point coordinates";
			info << "Number of searched points " << this->number_obs_point << endl;
			type=5;
			break;
		case 4://can not open file
			place.append("output_obs_point_rv2file(const string file)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 5://can not open file
			place.append("output_obs_point_fp2file(const string file)");
			reason="Can not open file";
			help="Check the file";
			type=5;
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
Warning Hyd_Observation_Point_Manager::set_warning(const int warn_type){
		string place="Hyd_Observation_Point_Manager::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no !END-flag is found in file
			place.append("input_obs_point(const string file)");
			reason="The point data block of the observation point is not closed with !END in file";
			help="Check the file and the number of point specification in file";
			type=1;
			break;
		case 1://input datas can not submitted the data
			place.append("transfer_obs_points2database(QSqlDatabase *ptr_database, const string file)");
			reason="Can not submit the observation point data to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted the data
			place.append("init_obs_points(const int num_rv, Hyd_Model_River *river, const int num_fp, Hyd_Model_Floodplain *floodplain)");
			reason="The observation point is not in the given hydraulic system";
			help="Check the observation point";
			type=9;
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