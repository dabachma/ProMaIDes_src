#include "Dam_Headers_Precompiled.h"
//#include "Dam_People_Damage_Function.h"
//init static members
Tables *Dam_People_Damage_Function::function_table=NULL;
Tables *Dam_People_Damage_Function::point_table=NULL;

//Default constructor
Dam_People_Damage_Function::Dam_People_Damage_Function(void):  vh_bound_1(7.0), v_bound_1(2.0), dsdt_bound_2(0.5), h_bound_2(2.1){

	this->name=label::not_set;
	this->index=-1;
	this->impact_type=_dam_impact_type::WATER_H;
	this->number_points=0;
	this->points=NULL;
	this->id_zone=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_People_Damage_Function), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_People_Damage_Function::~Dam_People_Damage_Function(void){
	this->delete_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_People_Damage_Function), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Read in the damage function per file
void Dam_People_Damage_Function::read_function(ifstream *ifile, int *line_counter){
	//read in
	string myline;
	int pos=-1;
	stringstream my_stream;
	bool found_flag=false;

	this->index=index;


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
		
		string impact_type_buff;
		int col=functions::count_number_columns(myline);
		my_stream << myline;
		if(col==4){
			my_stream >> this->number_points >> this->index  >> this->name >> impact_type_buff;
		}
		else{
			ostringstream info;
			info << "Found number of columns    :" << col << endl;
			info << "Required number of columns :" << 4 << endl;
			info << "Error occurs near line     : "<< *line_counter << endl;
			Error msg=this->set_error(11);
			msg.make_second_info(info.str());
			throw msg;
		}
		if(my_stream.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Error occurs near line: "<< *line_counter << endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}
		try{
			this->impact_type=Dam_Impact_Values::convert_txt2dam_impact_type(impact_type_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Error occurs near line: "<< *line_counter << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		my_stream.clear();
		my_stream.str("");
		found_flag=true;

	}
	while(found_flag!=true && ifile->eof()!=true);

	if(found_flag!=true){
		Error msg=this->set_error(7);
		ostringstream info;
		info << "Error occurs near line: "<< *line_counter << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	try{
		//allocate the points
		this->alloc_points();

		int counter=0;

		//read in the points
		for(int i=0; i<this->number_points; i++){

			//delete the comments
			getline(*ifile, myline,'\n');
			(*line_counter)++;
			functions::clean_string(&myline);

			if(myline.empty()!=true){
				pos=myline.rfind("!END");
				if(pos>=0){
					myline.erase(pos+4);
				}
				//stop when end is reach
				if(myline=="!END"|| ifile->eof()==true){
					break;
				}
				//read it in
				int nr_col=functions::count_number_columns(myline);
				my_stream << myline;
				if(nr_col==4){
					my_stream >> this->points[i].impact_value >> this->points[i].rel_damage_zone1>>this->points[i].rel_damage_zone2>>this->points[i].rel_damage_zone3;
					if(my_stream.fail()==true){
						ostringstream info;
						info << "Wrong input sequenze  :" << my_stream.str() << endl;
						info << "Error occurs near line: "<< *line_counter << endl;
						Error msg=this->set_error(6);
						msg.make_second_info(info.str());
						throw msg;
					}
				}
				else{
					ostringstream info;
					info << "Found number of columns :" << nr_col << endl;
					info << "Error occurs near line  : "<< *line_counter << endl;
					Error msg=this->set_error(9);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.str("");
				my_stream.clear();
				counter++;
			}
			else{
				i--;
			}

		}

		//check if all is read
		if(counter!=this->number_points){
			ostringstream info;
			info << "Given number of points: " <<this->number_points<<endl;
			info << "Found number of points: " <<counter << endl;
			info << "Error occurs near line: "<< *line_counter << endl;
			Error msg=this->set_error(8);
			msg.make_second_info(info.str());
			throw msg;
		}
		//check the function
		this->check_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Risk2poeple type name : " << this->name << endl;
		info << "Risk2poeple type id   : " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Transfer the damage function data to a database: the general settings of the functions (risk2people type) as well as the function points
void Dam_People_Damage_Function::transfer_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_People_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Dam_People_Damage_Function::function_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Dam_People_Damage_Function::function_table->maximum_int_of_column(Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id),ptr_database)+1;

	//insert row
	model.insertRows(0, 1);
						
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_id)).c_str())),this->index);
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_name)).c_str())),this->name.c_str());
	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_People_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "People2risk index         : " << this->index << endl;
		info << "People2risk name          : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Add damage function data to a database: the general settings of the functions (people2risk type) as well as the function points
void Dam_People_Damage_Function::add_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_People_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Dam_People_Damage_Function::function_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Dam_People_Damage_Function::function_table->maximum_int_of_column(Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id),ptr_database)+1;

	this->index=check_category_id_is_used(ptr_database, this->index);
	//insert row
	model.insertRows(0, 1);
						
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_id)).c_str())),this->index);
	model.setData(model.index(0,model.record().indexOf((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_name)).c_str())),this->name.c_str());
	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_People_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "People2risk index         : " << this->index << endl;
		info << "People2risk name          : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Input the damage function data per database: the general settings of the functions (risk2people type) as well as the function points
void Dam_People_Damage_Function::input_function_perdatabase(const QSqlTableModel *results, const int glob_index){
	int func_id=0;

	func_id=results->record(glob_index).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str()).toInt();
	this->index=results->record(glob_index).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_id)).c_str()).toInt();
	this->name=results->record(glob_index).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_name)).c_str()).toString().toStdString();

	try{
		QSqlTableModel point_results(0,results->database());
		//number of points
		this->number_points=Dam_People_Damage_Function::select_relevant_function_points_database(&point_results, func_id);

		//allocate the points
		this->alloc_points();
		//read in points
		for(int i=0; i< this->number_points; i++){
			this->input_function_points_perdatabase(&point_results, i);
		}

		//check the function
		this->check_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Risk2poeple type name : " << this->name << endl;
		info << "Risk2poeple type id   : " << this->index << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the number of points 
int Dam_People_Damage_Function::get_number_points(void){
	return this->number_points;
}
//Get the index of the function
int Dam_People_Damage_Function::get_index(void){
	return this->index;
}
//Get the name of the people2risk damage function
string Dam_People_Damage_Function::get_name(void){
	return this->name;
}
//Get the impact type
_dam_impact_type Dam_People_Damage_Function::get_impact_type(void){
	return this->impact_type;
}
//Get the identifier of the zone, which was calculated
int Dam_People_Damage_Function::get_id_calculated_zone(void){
	return this->id_zone;
}
//Interpolate the damage function for the given impact value
double Dam_People_Damage_Function::interpolate_function(const double impact_value, const double vh, const double velocity, const double waterlevel_rise){
	_dam_people_function_point result;
	//init
	result.impact_value=impact_value;
	result.rel_damage_zone1=0.0;
	result.rel_damage_zone2=0.0;
	result.rel_damage_zone3=0.0;
	this->id_zone=-1;

	//is below the first value
	if(impact_value <= this->points[0].impact_value){
		result.rel_damage_zone1=points[0].rel_damage_zone1;
		result.rel_damage_zone2=points[0].rel_damage_zone2;
		result.rel_damage_zone3=points[0].rel_damage_zone3;

	}
	//is above the last value
	else if(impact_value >= this->points[this->number_points-1].impact_value){
		result.rel_damage_zone1=points[this->number_points-1].rel_damage_zone1;
		result.rel_damage_zone2=points[this->number_points-1].rel_damage_zone2;
		result.rel_damage_zone3=points[this->number_points-1].rel_damage_zone3;
	}
	else{
		int end=0;
		int nodes=0;

		const int fMax[2] = { this->number_points/50, this->number_points/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
		if(fMax[1]<=1){//use just the approaximate search by /10
			end=1;
		}
		else{//use both
			end=2;
		}

		// Search approximate 
		for (int j = end-1; j >= 0; j--){
			if(fMax[j]==0){
				break;
			}
			while(impact_value >= this->points[nodes].impact_value ){
				nodes=nodes+fMax[j];
				if(nodes>=this->number_points-1){
					break;
				}
			}
			nodes=nodes-fMax[j];
		}

		//search exact
		while(impact_value >= this->points[nodes].impact_value){
				nodes=nodes+1;
		}
		//linear interploation scheme
		//zone1
		result.rel_damage_zone1=this->points[nodes-1].rel_damage_zone1+ (this->points[nodes].rel_damage_zone1-this->points[nodes-1].rel_damage_zone1)
			/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);

		//zone2
		result.rel_damage_zone2=this->points[nodes-1].rel_damage_zone2+ (this->points[nodes].rel_damage_zone2-this->points[nodes-1].rel_damage_zone2)
			/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);
		//zone3
		result.rel_damage_zone3=this->points[nodes-1].rel_damage_zone3+ (this->points[nodes].rel_damage_zone3-this->points[nodes-1].rel_damage_zone3)
			/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);

	}

	//decide which value is returned
	if(vh>=this->vh_bound_1 && velocity>=this->v_bound_1){
		this->id_zone=3;
		return result.rel_damage_zone3;

	}
	else{

		if(waterlevel_rise>=this->dsdt_bound_2){
			if(impact_value>=this->h_bound_2){
				this->id_zone=2;
				return result.rel_damage_zone2;
	
			}
			else{
				this->id_zone=1;
				return result.rel_damage_zone1;
			}
		}
		else{
			this->id_zone=1;
			return result.rel_damage_zone1;
		}
	}
}
//Output the members of the damage function to display/console
void Dam_People_Damage_Function::output_member(void){
	ostringstream cout;
	cout << "RISK2PEOPLE"<<endl;
	cout << " Id                      " << this->index << endl;
	cout << " Name                    " << this->name << endl;
	cout << "FUNCTION" << endl;
	cout << " Impact value            " << Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type) << endl;
	cout << " No. points              " << this->number_points << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,false);
	cout << "No." << W(9) <<"Impact" <<W(18)<< "Relative_dam_Z1" << W(18)<< "Relative_dam_Z2" <<W(18)<< "Relative_dam_Z3" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);

	if(this->number_points>0){
		for(int i=0; i< this->number_points; i++){
			cout << W(3)<< i << W(8) << P(2)<< FORMAT_FIXED_REAL<< this->points[i].impact_value;
			cout << W(14) << this->points[i].rel_damage_zone1 ;
			cout << W(14) << this->points[i].rel_damage_zone2;
			cout << W(14) << this->points[i].rel_damage_zone3 << endl;		
			Sys_Common_Output::output_dam->output_txt(&cout,true);
		}
	}
	else{
		cout << " No points are set ! " << endl;
		Sys_Common_Output::output_dam->output_txt(&cout,true);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();

}
//Create the database table for the general information of the damage functions (static)
void Dam_People_Damage_Function::create_function_table(QSqlDatabase *ptr_database){
		if(Dam_People_Damage_Function::function_table==NULL){
			ostringstream cout;
			cout << "Create people2risk damage function database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pop_func;
			const int num_col=4;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}


			tab_col[0].name=dam_label::function_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::pop_type_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::pop_type_name;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=label::description;
			tab_col[3].type=sys_label::tab_col_type_string;


			try{
				Dam_People_Damage_Function::function_table= new Tables();
				if(Dam_People_Damage_Function::function_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_People_Damage_Function::create_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_People_Damage_Function::close_function_table();
				throw msg;
			}

		Dam_People_Damage_Function::close_function_table();
	}
	
	//create appending table
	Dam_People_Damage_Function::create_point_table(ptr_database);

}
//Set the database table for the general information of the damage functions: it sets the table name and the name of the columns and allocate them (static)
void Dam_People_Damage_Function::set_function_table(QSqlDatabase *ptr_database){
	if(Dam_People_Damage_Function::function_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pop_func;
		string tab_col[4];

		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::pop_type_id;
		tab_col[2]=dam_label::pop_type_name;
		tab_col[3]=label::description;

		try{
			Dam_People_Damage_Function::function_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_People_Damage_Function::function_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_People_Damage_Function::set_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_People_Damage_Function::close_function_table();
			throw msg;
		}
	}
	//set appending table
	Dam_People_Damage_Function::set_point_table(ptr_database);

}
//Delete all data in the database table for the general information of the damage functions (static)
void Dam_People_Damage_Function::delete_data_in_function_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_People_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_People_Damage_Function::function_table->delete_data_in_table(ptr_database);

	//delete data in appending 
	Dam_People_Damage_Function::delete_data_in_point_table(ptr_database);

}
//Close and delete the database table for the general information of the damage functions (static)
void Dam_People_Damage_Function::close_function_table(void){
	if(Dam_People_Damage_Function::function_table!=NULL){
		delete Dam_People_Damage_Function::function_table;
		Dam_People_Damage_Function::function_table=NULL;
	}
	//close appending tables
	Dam_People_Damage_Function::close_point_table();

}
///Select and count the number of relevant damage functions in a database table (static)
int Dam_People_Damage_Function::select_relevant_functions_database(QSqlTableModel *results, const bool with_output){
	int number=0;
	try{
		Dam_People_Damage_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant people2risk damage functions in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_People_Damage_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id) << " >= " << "0";

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_People_Damage_Function::select_relevant_functions_database(QSqlTableModel *results,  const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_People_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant people2risk damage function(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage functions in a database table by given identifier (static)
int Dam_People_Damage_Function::select_relevant_functions_database(QSqlTableModel *results, const int id){
		int number=0;
	try{
		Dam_People_Damage_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_People_Damage_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id) << " = " << id;

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_People_Damage_Function::select_relevant_functions_database(QSqlTableModel *results,  const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_People_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_People_Damage_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	return number;
}
//Set the function to the plot widget
void Dam_People_Damage_Function::set_function2plot_widget(Sys_Plot_Wid *widget){

	
	widget->set_number_curves(3);
	widget->set_axis_title("Waterlevel [m]", "Damage rate [%]");
	widget->set_curve_characteristics(0, "Breach zone III", QwtPlotCurve::Lines, Qt::red);
	widget->set_curve_characteristics(1, "Rapidly raising water zone II", QwtPlotCurve::Lines, Qt::darkYellow);
	widget->set_curve_characteristics(2, "Remaining zone I", QwtPlotCurve::Lines, Qt::blue);

	widget->set_legend(true);

	int no_po=0;
	double *x=NULL;
	double *y1=NULL;
	double *y2=NULL;
	double *y3=NULL;
	//count points
	if(this->number_points>1){
		no_po=this->number_points;
	}
	else{
		no_po=2;
	}

	//allocate 
	try{
		x=new double[no_po]; 
		y1=new double[no_po]; 
		y2=new double[no_po];
		y3=new double[no_po];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(12);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	if(this->number_points>1){
		//set points
		for(int i=0; i<this->number_points;i++){
			x[i]=this->points[i].impact_value;
			y1[i]=this->points[i].rel_damage_zone1;
			y2[i]=this->points[i].rel_damage_zone2;
			y3[i]=this->points[i].rel_damage_zone3;
		}

		widget->set_curve_data(0, no_po, x,y1);
		widget->set_curve_data(1, no_po, x,y2);
		widget->set_curve_data(2, no_po, x,y3);
	
	}
	else{
		if(this->number_points==1){
			x[0]=this->points[0].impact_value;
			x[1]=this->points[0].impact_value+5.0;
			y1[0]=this->points[0].rel_damage_zone1;
			y1[1]=this->points[0].rel_damage_zone1;
			y2[0]=this->points[0].rel_damage_zone2;
			y2[1]=this->points[0].rel_damage_zone2;
			y3[0]=this->points[0].rel_damage_zone3;
			y3[1]=this->points[0].rel_damage_zone3;
		}
		else{
			x[0]=0.0;
			x[1]=0.0+5.0;
			y1[0]=0.0;
			y1[1]=0.0;
			y2[0]=0.0;
			y2[1]=0.0;
			y3[0]=0.0;
			y3[1]=0.0;
		}
	}
	widget->set_curve_data(0, no_po, x,y3);
	widget->set_curve_data(1, no_po, x,y2);
	widget->set_curve_data(2, no_po, x,y1);

	widget->show_plot();








	//delete 
	if(x!=NULL){
		delete []x;
		x=NULL;
	}
	if(y1!=NULL){
		delete []y1;
		y1=NULL;
	}
	if(y2!=NULL){
		delete []y2;
		y2=NULL;
	}
	if(y3!=NULL){
		delete []y3;
		y3=NULL;
	}
}
//___________
//private
//Transfer the damage function points to a database
void Dam_People_Damage_Function::transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func){
	//mysql query with the table_model
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_People_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	int glob_id=Dam_People_Damage_Function::point_table->maximum_int_of_column(Dam_People_Damage_Function::point_table->get_column_name(dam_label::glob_id),ptr_database)+1;


	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_People_Damage_Function::point_table->get_table_name();
	query_header <<" ( ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::glob_id) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::function_id) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::impact_value) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_1) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_2) <<" , ";
	query_header << Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_3) <<" ) ";
	query_header << " VALUES ";
	ostringstream query_data;

	for(int i=0; i< this->number_points; i++){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << id_func <<" , ";
		query_data << i<<" , ";
		query_data << this->points[i].impact_value <<" , ";
		query_data << this->points[i].rel_damage_zone1 <<" , ";
		query_data << this->points[i].rel_damage_zone2 <<" , ";
		query_data <<this->points[i].rel_damage_zone3 <<" ) " << " ,";
		glob_id++;
	}
	string buff;
	query_header<< query_data.str();
	buff = query_header.str();
	buff.erase(buff.length()-1);
	Data_Base::database_request(&query_buff, buff, ptr_database);
	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(1);
		ostringstream info;
		info << "Table Name                : " << Dam_People_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "People2risk index         : " << this->index << endl;
		info << "People2risk name          : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
}
//Input the damage function points per database
void Dam_People_Damage_Function::input_function_points_perdatabase(const QSqlTableModel *results, const int index){
	this->points[index].impact_value=results->record(index).value((Dam_People_Damage_Function::point_table->get_column_name(dam_label::impact_value)).c_str()).toDouble();
	this->points[index].rel_damage_zone1=results->record(index).value((Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_1)).c_str()).toDouble();
	this->points[index].rel_damage_zone2=results->record(index).value((Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_2)).c_str()).toDouble();
	this->points[index].rel_damage_zone3=results->record(index).value((Dam_People_Damage_Function::point_table->get_column_name(dam_label::rel_dam_zone_3)).c_str()).toDouble();
}
//Allocate the damage function points
void Dam_People_Damage_Function::alloc_points(void){
	try{
		this->points=new _dam_people_function_point[this->number_points];
		Sys_Memory_Count::self()->add_mem(sizeof(_dam_people_function_point)*this->number_points, _sys_system_modules::DAM_SYS);

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init the points
	for(int i=0; i< this->number_points; i++){
		this->points[i].impact_value=0.0;
		this->points[i].rel_damage_zone1=0.0;
		this->points[i].rel_damage_zone2=0.0;
		this->points[i].rel_damage_zone3=0.0;
	}

}
//Delete the damage function points
void Dam_People_Damage_Function::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_dam_people_function_point)*this->number_points, _sys_system_modules::DAM_SYS);

	}
}
//Check the damage function
void Dam_People_Damage_Function::check_function(void){

	for(int i=0; i< this->number_points; i++){
		if(this->points[i].impact_value<0.0){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Impact value point " << i <<" : " <<this->points[i].impact_value << endl;
			msg.make_second_info(info.str());
			throw msg;
		}


		if(this->points[i].rel_damage_zone1<0.0 || this->points[i].rel_damage_zone1>1.0 ||
			this->points[i].rel_damage_zone2<0.0 || this->points[i].rel_damage_zone2>1.0 ||
			this->points[i].rel_damage_zone3<0.0 || this->points[i].rel_damage_zone3>1.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Impact value point " << i <<" : " <<this->points[i].rel_damage_zone1 << "; ";
			info <<this->points[i].rel_damage_zone2 << "; " << this->points[i].rel_damage_zone3<<endl;;
			msg.make_second_info(info.str());
			throw msg;

		}
		//set to samll values to zero
		if(this->points[i].rel_damage_zone1<1e-30){
			this->points[i].rel_damage_zone1=0.0;
		}
		if(this->points[i].rel_damage_zone2<1e-30){
			this->points[i].rel_damage_zone2=0.0;
		}
		if(this->points[i].rel_damage_zone3<1e-30){
			this->points[i].rel_damage_zone3=0.0;
		}

		if(i<this->number_points-1){
			//impact value have to be increasing
			if(this->points[i].impact_value>this->points[i+1].impact_value){
				Error msg=this->set_error(1);
				ostringstream info;
				info << "Impact value point " << i <<" : " <<this->points[i].impact_value << endl;
				info << "Impact value point " << i+1 <<" : " <<this->points[i+1].impact_value << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
}
//Create the database table for the damage functions points (static)
void Dam_People_Damage_Function::create_point_table(QSqlDatabase *ptr_database){
		if(Dam_People_Damage_Function::point_table==NULL){
			ostringstream cout;
			cout << "Create people2risk damage function point database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_pop_func_point;
			const int num_col=7;
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

			tab_col[1].name=dam_label::function_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;
			tab_col[1].key_flag=true;

			tab_col[2].name=dam_label::point_id;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;
			tab_col[2].key_flag=true;

			tab_col[3].name=dam_label::impact_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=dam_label::rel_dam_zone_1;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=dam_label::rel_dam_zone_2;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=dam_label::rel_dam_zone_3;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;


			try{
				Dam_People_Damage_Function::point_table= new Tables();
				if(Dam_People_Damage_Function::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_People_Damage_Function::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_People_Damage_Function::close_point_table();
				throw msg;
			}

		Dam_People_Damage_Function::close_point_table();
	}

}
//Set the database table for the damage functions points: it sets the table name and the name of the columns and allocate them (static)
void Dam_People_Damage_Function::set_point_table(QSqlDatabase *ptr_database){
	if(Dam_People_Damage_Function::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_pop_func_point;
		string tab_col[7];
		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=dam_label::impact_value;
		tab_col[3]=dam_label::rel_dam_zone_1;
		tab_col[4]=dam_label::rel_dam_zone_2;
		tab_col[5]=dam_label::rel_dam_zone_3;
		tab_col[6]=dam_label::glob_id;


		try{
			Dam_People_Damage_Function::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_People_Damage_Function::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_People_Damage_Function::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_People_Damage_Function::close_point_table();
			throw msg;
		}
	}

}
//Delete all data in the database table for the damage functions points (static)
void Dam_People_Damage_Function::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_People_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_People_Damage_Function::point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the damage functions points (static)
void Dam_People_Damage_Function::close_point_table(void){
	if(Dam_People_Damage_Function::point_table!=NULL){
		delete Dam_People_Damage_Function::point_table;
		Dam_People_Damage_Function::point_table=NULL;
	}
}
//Select and count the number of relevant points for the damage function in a database table (static)
int Dam_People_Damage_Function::select_relevant_function_points_database(QSqlTableModel *results, const int index){
	int number=0;
	try{
		Dam_People_Damage_Function::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_People_Damage_Function::point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_People_Damage_Function::point_table->get_column_name(dam_label::function_id) << " = " << index;
	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_People_Damage_Function::select_relevant_function_point_database(QSqlTableModel *results,  const int index)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_People_Damage_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_People_Damage_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();


	return number;
}
//Check if a category id is already in use; it this is the case return a new category id
int Dam_People_Damage_Function::check_category_id_is_used(QSqlDatabase *ptr_database, const int id){
	QSqlTableModel results(NULL, *ptr_database);
	int number=0;
	number=Dam_People_Damage_Function::select_relevant_functions_database(&results, false);
	bool new_test=false;
	int new_id=id;

	do{
		new_test=false;
		for(int i=0; i<number; i++){
			if(new_id==results.record(i).value((Dam_People_Damage_Function::function_table->get_column_name(dam_label::pop_type_id)).c_str()).toInt()){
				new_id++;
				new_test=true;
				break;
			}
		}
	}while(new_test==true);

	return new_id;
}
//Set error(s)
Error Dam_People_Damage_Function::set_error(const int err_type){
	string place="Dam_People_Damage_Function::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("alloc_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://impact values must increase
			place.append("check_function(void))");
			reason="The impact values of the damage function have to increase continuous";
			help="Check the damage function";
			type=21;
			break;
		case 2://impact values can not be negativ
			place.append("check_function(void))");
			reason="The impact values of the damage function can not be negative";
			help="Check the damage function";
			type=21;
			break;
		case 3://relative damage can not be negativ
			place.append("check_function(void))");
			reason="The relative damage of the damage function can not be negative or above 100 %";
			help="Check the damage function";
			type=22;
			break;
		case 6://wrong input
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage function in file";
			type=1;
			break;
		case 7://given curve number is not found 
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="The function starts with !BEGIN but no values are found after";
			help="Check the damage function in file";
			type=1;
			break;
		case 8://not all points are found
			place.append("read_function(ifstream *file, int *line_counter,)");
			reason="Not all points of the damage function are found";
			help="Check the file";
			type=1;
			break;
		case 9://wrong number of columns 
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="The number of columns for specifiying the damage function points have to be 4, the impact value and one value for the three zones";
			help="Check the file";
			type=1;
			break;
		case 11://number of columns is wrong
			place.append("read_function(ifstream *file, int *line_counter)");
			reason="There is a problem with the input; wrong number of columns";
			help="Check the damage function in file";
			type=1;
			break;
		case 12://bad alloc
			place.append("set_function2plot_widget(Sys_Plot_Wid *widget)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
Warning Dam_People_Damage_Function::set_warning(const int warn_type){
	string place="Dam_People_Damage_Function::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the damage function (people2risk type) to the database";
			help="Check the database";
			type=2;
			break;
		case 1://input datas can not submitted
			place.append("transfer_function_point2database(QSqlDatabase *ptr_database, const int id_glob)");
			reason="Can not submit the point data of the damage function to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not submitted
			place.append("add_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the damage function (people2risk type) to the database";
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