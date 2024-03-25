#include "Dam_Headers_Precompiled.h"
//#include "Dam_Ecn_Damage_Function.h"

//init static members
Tables *Dam_Ecn_Damage_Function::function_table=NULL;
Tables *Dam_Ecn_Damage_Function::point_table=NULL;


///Default constructor
Dam_Ecn_Damage_Function::Dam_Ecn_Damage_Function(void){

	
	this->no_points=0;
	this->landuse_info.default_stockvalue=0.0;
	this->landuse_info.def_stock_5=0.0;
	this->landuse_info.def_stock_95=0.0;
	this->landuse_info.id=-1;
	this->landuse_info.name=label::not_set;

	this->result.mid_result=0.0;
	this->result.quantile_5_result=0.0;
	this->result.quantile_95_result=0.0;

	this->index=-1;

	this->points=NULL;
	impact_type=_dam_impact_type::WATER_H;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Ecn_Damage_Function), _sys_system_modules::DAM_SYS);
}
///Default destructor
Dam_Ecn_Damage_Function::~Dam_Ecn_Damage_Function(void){

	this->delete_points();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Ecn_Damage_Function), _sys_system_modules::DAM_SYS);
}
//______________
//public
//Read in the damage function per file
void Dam_Ecn_Damage_Function::read_function(ifstream *ifile, int *line_counter, const int index){

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
		if(col==5){
			my_stream >> this->no_points >> this->landuse_info.id  >> landuse_info.name >> impact_type_buff>> landuse_info.default_stockvalue;
			landuse_info.def_stock_5=landuse_info.default_stockvalue;
			landuse_info.def_stock_95=landuse_info.default_stockvalue;
		}
		else{
			my_stream >> this->no_points >> this->landuse_info.id  >> landuse_info.name >> impact_type_buff>> landuse_info.default_stockvalue >> landuse_info.def_stock_5 >> landuse_info.def_stock_95;
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
		for(int i=0; i<this->no_points; i++){

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
				if(nr_col==2){
					my_stream >> this->points[i].impact_value >> this->points[i].rel_damage;
					this->points[i].quant_5_rel_damage=this->points[i].rel_damage;
					this->points[i].quant_95_rel_damage=this->points[i].rel_damage;
					if(my_stream.fail()==true){
						ostringstream info;
						info << "Wrong input sequenze  :" << my_stream.str() << endl;
						info << "Error occurs near line: "<< *line_counter << endl;
						Error msg=this->set_error(6);
						msg.make_second_info(info.str());
						throw msg;
					}
				}
				else if(nr_col==4){
					my_stream >> this->points[i].impact_value >> this->points[i].rel_damage>> this->points[i].quant_5_rel_damage >> this->points[i].quant_95_rel_damage;
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
		if(counter!=this->no_points){
			ostringstream info;
			info << "Given number of points: " <<this->no_points <<endl;
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
		info << "Landuse name : " << this->landuse_info.name << endl;
		info << "Landuse id   : " << this->landuse_info.id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}


}
//Transfer the damage function data to a database: the general settings of the functions (land-use information) as well as the function points
void Dam_Ecn_Damage_Function::transfer_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	model.setTable((Dam_Ecn_Damage_Function::function_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Damage_Function::function_table->maximum_int_of_column(Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id),ptr_database)+1;

	//insert row
	model.insertRows(0, 1);
						
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str())),this->landuse_info.id);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_name)).c_str())),this->landuse_info.name.c_str());
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_value)).c_str())),this->landuse_info.default_stockvalue);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_5_quant)).c_str())),this->landuse_info.def_stock_5);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_95_quant)).c_str())),this->landuse_info.def_stock_95);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::impact_type)).c_str())),Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type).c_str());

	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Landuse index             : " << this->landuse_info.id << endl;
		info << "Landuse name              : " << this->landuse_info.name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Add damage function data to a database: the general settings of the functions (land-use information) as well as the function points
void Dam_Ecn_Damage_Function::add_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	this->landuse_info.id=this->check_landuse_id_is_used(ptr_database, this->landuse_info.id);

	//give the table name to the query model
	model.setTable((Dam_Ecn_Damage_Function::function_table->get_table_name()).c_str());

	//evaluate the global identifier
	int id_glob=Dam_Ecn_Damage_Function::function_table->maximum_int_of_column(Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id),ptr_database)+1;

	//insert row
	model.insertRows(0, 1);
						
	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str())),id_glob);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str())),this->landuse_info.id);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_name)).c_str())),this->landuse_info.name.c_str());
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_value)).c_str())),this->landuse_info.default_stockvalue);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_5_quant)).c_str())),this->landuse_info.def_stock_5);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_95_quant)).c_str())),this->landuse_info.def_stock_95);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::impact_type)).c_str())),Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type).c_str());

	//submit it to the datbase
	Data_Base::database_submit(&model);
	
	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Landuse index             : " << this->landuse_info.id << endl;
		info << "Landuse name              : " << this->landuse_info.name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}

	//transfer the points
	this->transfer_function_point2database(ptr_database, id_glob);
}
//Input the damage function data per database: the general settings of the functions (land-use information) as well as the function points
void Dam_Ecn_Damage_Function::input_function_perdatabase(const QSqlTableModel *results, const int glob_index){
	
	string buffer;

	this->index=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id)).c_str()).toInt();
	this->landuse_info.id=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str()).toInt();
	this->landuse_info.name=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_name)).c_str()).toString().toStdString();
	this->landuse_info.default_stockvalue=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_value)).c_str()).toDouble();
	this->landuse_info.def_stock_5=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_5_quant)).c_str()).toDouble();
	this->landuse_info.def_stock_95=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::stock_95_quant)).c_str()).toDouble();
	buffer=results->record(glob_index).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::impact_type)).c_str()).toString().toStdString();
	
	try{
		this->impact_type=Dam_Impact_Values::convert_txt2dam_impact_type(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Landuse index             : " << this->landuse_info.id << endl;
		info << "Landuse name              : " << this->landuse_info.name << endl;
		msg.make_second_info(info.str());
	}
	try{
		QSqlTableModel point_results(0,results->database());
		//number of points
		this->no_points=Dam_Ecn_Damage_Function::select_relevant_function_points_database(&point_results, this->index);

		//allocate the points
		this->alloc_points();
		//read in points
		for(int i=0; i< this->no_points; i++){
			this->input_function_points_perdatabase(&point_results, i);
		}

		//check the function
		this->check_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Landuse name : " << this->landuse_info.name << endl;
		info << "Landuse id   : " << this->landuse_info.id << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Get the number of points 
int Dam_Ecn_Damage_Function::get_number_points(void){
	return this->no_points;
}
//Get a pointer to the function points by a given index
_dam_ecn_function_point* Dam_Ecn_Damage_Function::get_ptr_function_points(const int index){
	if(index <0 || index >= this->no_points){
		return NULL;
	}
	else{
		return &this->points[index];
	}
}
//Get the land-use information
_dam_ecn_landuse_info Dam_Ecn_Damage_Function::get_land_use_info(void){
	return this->landuse_info;
}
//Get the impact type
_dam_impact_type Dam_Ecn_Damage_Function::get_impact_type(void){
	return this->impact_type;
}	
//Interpolate the damage function for the given impact value
_dam_ecn_function_point Dam_Ecn_Damage_Function::interpolate_function(const double impact_value){
	_dam_ecn_function_point interpolated_result;
	interpolated_result.impact_value=impact_value;
	interpolated_result.quant_5_rel_damage=0.0;
	interpolated_result.quant_95_rel_damage=0.0;
	interpolated_result.rel_damage=0.0;

	//is below the first value
	if(impact_value <= this->points[0].impact_value){
		interpolated_result.quant_5_rel_damage=points[0].quant_5_rel_damage;
		interpolated_result.quant_95_rel_damage=points[0].quant_95_rel_damage;
		interpolated_result.rel_damage=points[0].rel_damage;
		return interpolated_result;
	}
	//is above the last value
	if(impact_value >= this->points[this->no_points-1].impact_value){
		interpolated_result.quant_5_rel_damage=points[this->no_points-1].quant_5_rel_damage;
		interpolated_result.quant_95_rel_damage=points[this->no_points-1].quant_95_rel_damage;
		interpolated_result.rel_damage=points[this->no_points-1].rel_damage;
		return interpolated_result;
	}


	int end=0;
	int nodes=0;

	const int fMax[2] = { this->no_points/50, this->no_points/10 };// determination of stepwide for approximate search (two stepwide or one stepwide depending of number of nodes)
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
			if(nodes>=this->no_points-1){
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
	//relative damage
	interpolated_result.rel_damage=this->points[nodes-1].rel_damage+ (this->points[nodes].rel_damage-this->points[nodes-1].rel_damage)
		/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);

	//5%-quantil
	interpolated_result.quant_5_rel_damage=this->points[nodes-1].quant_5_rel_damage+ (this->points[nodes].quant_5_rel_damage-this->points[nodes-1].quant_5_rel_damage)
		/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);

	//95%-quantil
	interpolated_result.quant_95_rel_damage=this->points[nodes-1].quant_95_rel_damage+ (this->points[nodes].quant_95_rel_damage-this->points[nodes-1].quant_95_rel_damage)
		/ (this->points[nodes].impact_value -this->points[nodes-1].impact_value)*(impact_value-this->points[nodes-1].impact_value);

	

	return interpolated_result;
}
//Interpolate the damage function for the given impact value
_dam_ecn_function_point Dam_Ecn_Damage_Function::interpolate_function(Dam_Impact_Values *impact){
	_dam_ecn_function_point interpolated_result;
	interpolated_result.impact_value=0.0;
	interpolated_result.quant_5_rel_damage=0.0;
	interpolated_result.quant_95_rel_damage=0.0;
	interpolated_result.rel_damage=0.0;

	switch(this->impact_type){
		case _dam_impact_type::WATER_D:
			interpolated_result=this->interpolate_function(impact->get_impact_duration());
			break;
		case _dam_impact_type::WATER_H:
			interpolated_result=this->interpolate_function(impact->get_impact_h());
			break;
		case _dam_impact_type::WATER_V:
			interpolated_result=this->interpolate_function(impact->get_impact_v_tot());
			break;
		case _dam_impact_type::WATER_VH:
			interpolated_result=this->interpolate_function(impact->get_impact_vh());
			break;
		default:
			return interpolated_result;
	}
	return interpolated_result;
}
//Output the members of the damage function to display/console
void Dam_Ecn_Damage_Function::output_member(void){
	ostringstream prefix;
	prefix << "DF_"<< this->index<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "LANDUSE"<<endl;
	cout << " Id                      " << this->landuse_info.id << endl;
	cout << " Name                    " << this->landuse_info.name << endl;
	cout << " Default stockvalue      " << P(2)<< FORMAT_FIXED_REAL << this->landuse_info.default_stockvalue<< label::euro << endl;
	cout << " 5%-quantile stockvalue  " << P(2)<< FORMAT_FIXED_REAL << this->landuse_info.def_stock_5<< label::euro << endl;
	cout << " 95%-quantile stockvalue " << P(2)<< FORMAT_FIXED_REAL << this->landuse_info.def_stock_95<< label::euro << endl;
	cout << "FUNCTION" << endl;
	cout << " Impact value            " << Dam_Impact_Values::convert_dam_impact_type2txt(this->impact_type) << endl;
	cout << " No. points              " << this->no_points << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,false);
	cout << "No." << W(9) <<"Impact" <<W(18)<< "Relative_dam_[%]" << W(15)<< "5%-quantile" <<W(15)<< "95%-quantile" << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);

	if(this->no_points>0){
		for(int i=0; i< this->no_points; i++){
			cout << W(3)<< i << W(8) << P(2)<< FORMAT_FIXED_REAL<< this->points[i].impact_value;
			cout << W(14) << this->points[i].rel_damage ;
			cout << W(14) << this->points[i].quant_5_rel_damage;
			cout << W(14) << this->points[i].quant_95_rel_damage << endl;		
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
void Dam_Ecn_Damage_Function::create_function_table(QSqlDatabase *ptr_database){
		if(Dam_Ecn_Damage_Function::function_table==NULL){
			ostringstream cout;
			cout << "Create economical damage function database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_ecn_func;
			const int num_col=8;
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

			tab_col[1].name=dam_label::landuse_id;
			tab_col[1].type=sys_label::tab_col_type_int;
			tab_col[1].unsigned_flag=true;

			tab_col[2].name=dam_label::landuse_name;
			tab_col[2].type=sys_label::tab_col_type_string;

			tab_col[3].name=dam_label::stock_value;
			tab_col[3].type=sys_label::tab_col_type_double;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=dam_label::stock_5_quant;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=dam_label::stock_95_quant;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=dam_label::impact_type;
			tab_col[6].type=sys_label::tab_col_type_string;

			tab_col[7].name=label::description;
			tab_col[7].type=sys_label::tab_col_type_string;


			try{
				Dam_Ecn_Damage_Function::function_table= new Tables();
				if(Dam_Ecn_Damage_Function::function_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Ecn_Damage_Function::create_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Ecn_Damage_Function::close_function_table();
				throw msg;
			}

		Dam_Ecn_Damage_Function::close_function_table();
	}
	
	//create appending table
	Dam_Ecn_Damage_Function::create_point_table(ptr_database);

}
//Set the database table for the general information of the damage functions: it sets the table name and the name of the columns and allocate them (static)
void Dam_Ecn_Damage_Function::set_function_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Damage_Function::function_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_ecn_func;
		string tab_col[8];

		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::landuse_id;
		tab_col[2]=dam_label::landuse_name;
		tab_col[3]=dam_label::stock_value;
		tab_col[4]=dam_label::stock_5_quant;
		tab_col[5]=dam_label::stock_95_quant;
		tab_col[6]=dam_label::impact_type;
		tab_col[7]=label::description;

		try{
			Dam_Ecn_Damage_Function::function_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Ecn_Damage_Function::function_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Ecn_Damage_Function::set_function_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str()); 
			throw msg;	
		}
		catch(Error msg){
			Dam_Ecn_Damage_Function::close_function_table();
			throw msg;
		}
	}
	//set appending table
	Dam_Ecn_Damage_Function::set_point_table(ptr_database);
}
//Delete all data in the database table for the general information of the damage functions (static)
void Dam_Ecn_Damage_Function::delete_data_in_function_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Damage_Function::set_function_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}


	//delete the table
	Dam_Ecn_Damage_Function::function_table->delete_data_in_table(ptr_database);

	//delete data in appending 
	Dam_Ecn_Damage_Function::delete_data_in_point_table(ptr_database);

}
//Close and delete the database table for the general information of the damage functions (static)
void Dam_Ecn_Damage_Function::close_function_table(void){
	if(Dam_Ecn_Damage_Function::function_table!=NULL){
		delete Dam_Ecn_Damage_Function::function_table;
		Dam_Ecn_Damage_Function::function_table=NULL;
	}
	//close appending tables
	Dam_Ecn_Damage_Function::close_point_table();
}
//Select and count the number of relevant damage functions in a database table (static)
int Dam_Ecn_Damage_Function::select_relevant_functions_database(QSqlTableModel *results, const bool with_output){

	int number=0;
	try{
		Dam_Ecn_Damage_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant economical damage functions in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	//give the table name to the query model
	results->setTable((Dam_Ecn_Damage_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id) << " >= " << "0";

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Damage_Function::select_relevant_functions_database(QSqlTableModel *results,  const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant economical damage function(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage functions in a database table by a given function id
int Dam_Ecn_Damage_Function::select_relevant_functions_database(QSqlTableModel *results, const int function_id){
		int number=0;
	try{
		Dam_Ecn_Damage_Function::set_function_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Ecn_Damage_Function::function_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id) << " = " << function_id;

	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Damage_Function::select_relevant_functions_database(QSqlTableModel *results,  const int function_id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::function_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();


	return number;
}
//Get the result per landuse id
_dam_ecn_result Dam_Ecn_Damage_Function::get_result_landuse_id(void){
	return this->result;
}
//Calculate the damage per landuse id by using the calculated results in database
void Dam_Ecn_Damage_Function::calculate_damage_landuse_id(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){

	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQuery results(*ptr_database);

	//immob
	ostringstream test_filter;
	test_filter<< "Select SUM(";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) << "), SUM(";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant) << ") , SUM( ";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant) << ") ";

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name() << " , "<<Dam_Ecn_Element::elem_erg_table->get_table_name();
	test_filter << " where " ;
	
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::immob_id)<<" = " <<this->landuse_info.id;
	
	test_filter  << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter  << " AND ";
	test_filter  << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter  << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND (";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;

	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id);
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id);
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag);


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Names               : " << Dam_Ecn_Element::elem_table->get_table_name() <<", "<< Dam_Ecn_Element::elem_erg_table->get_table_name()<<endl;
		info << "Table error info          : " << results.lastError().text().toStdString() << endl;
		info << "Landuse id                : " << this->landuse_info.id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
		return;
	}
	results.first();

	this->result.mid_result=results.value(0).toDouble();
	this->result.quantile_95_result=results.value(1).toDouble();
	this->result.quantile_5_result=results.value(2).toDouble();


	//mob
	test_filter.str("");
	test_filter<< "Select SUM(";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) << "), SUM(";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant) << ") , SUM( ";
	test_filter <<  Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant) << ") ";

	test_filter << " from " << Dam_Ecn_Element::elem_table->get_table_name() << " , "<<Dam_Ecn_Element::elem_erg_table->get_table_name();
	test_filter << " where " ;
	
	test_filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::mob_id)<<" = " <<this->landuse_info.id;
		
	test_filter  << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) << " = " << bound_sz;
	test_filter  << " AND ";
	test_filter  << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) << " = '" << break_sz<<"'";
	test_filter  << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag) << "= true";
	test_filter << " AND ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) << " = "  << id.area_state;
	test_filter << " AND (";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << 0 ;
	test_filter << " OR " ;
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) << " = " << id.measure_nr;
	test_filter << " ) " ;

	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(dam_label::raster_id) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id);
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(dam_label::elem_id) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::elem_id);
	test_filter << " and ";
	test_filter << Dam_Ecn_Element::elem_erg_table->get_table_name()<<"."<<Dam_Ecn_Element::elem_table->get_column_name(label::applied_flag) << " = " << Dam_Ecn_Element::elem_table->get_table_name() << "."<<Dam_Ecn_Element::elem_erg_table->get_column_name(label::applied_flag);


	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	if(results.lastError().isValid()==true){
		Warning msg=this->set_warning(3);
		ostringstream info;
		info << "Table Names               : " << Dam_Ecn_Element::elem_table->get_table_name() <<", "<< Dam_Ecn_Element::elem_erg_table->get_table_name()<<endl;
		info << "Table error info          : " << results.lastError().text().toStdString() << endl;
		info << "Landuse id                : " << this->landuse_info.id << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
		return;
	}
	results.first();

	this->result.mid_result=this->result.mid_result+results.value(0).toDouble();
	this->result.quantile_95_result=this->result.quantile_95_result+results.value(1).toDouble();
	this->result.quantile_5_result=this->result.quantile_5_result+results.value(2).toDouble();


}
//Output the sum over the land use id of the damage results to display/console
void Dam_Ecn_Damage_Function::output_result_landuse_damage(ostringstream *cout){
	(*cout) << "  "<< landuse_info.id << " , " << this->landuse_info.name<< " , " << P(2)<< FORMAT_FIXED_REAL << this->result.mid_result << " , "<< this->result.quantile_5_result<< " , "<< this->result.quantile_95_result << endl;
}
//______________
//private
//Transfer the damage function points to a database
void Dam_Ecn_Damage_Function::transfer_function_point2database(QSqlDatabase *ptr_database, const int id_func){
	//mysql query with the table_model
	QSqlQuery query_buff(*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	int glob_id=Dam_Ecn_Damage_Function::point_table->maximum_int_of_column(Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::glob_id),ptr_database)+1;


	ostringstream query_header;
	query_header << "INSERT INTO  " << Dam_Ecn_Damage_Function::point_table->get_table_name();
	query_header <<" ( ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::glob_id) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::function_id) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::point_id) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::impact_value) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage_5_quant) <<" , ";
	query_header << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage_95_quant) <<" ) ";
	query_header << " VALUES ";
	ostringstream query_data;


	for(int i=0; i< this->no_points; i++){
		query_data << " ( ";
		query_data << glob_id <<" , ";
		query_data << id_func <<" , ";
		query_data << i<<" , ";
		query_data << this->points[i].impact_value <<" , ";
		query_data << this->points[i].rel_damage <<" , ";
		query_data << this->points[i].quant_5_rel_damage <<" , ";
		query_data <<this->points[i].quant_95_rel_damage <<" ) " << " ,";
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
		info << "Table Name                : " << Dam_Ecn_Damage_Function::function_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Landuse index             : " << this->landuse_info.id << endl;
		info << "Landuse name              : " << this->landuse_info.name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);	
	}
}
//Input the damage function points per database
void Dam_Ecn_Damage_Function::input_function_points_perdatabase(const QSqlTableModel *results, const int index){
	this->points[index].impact_value=results->record(index).value((Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::impact_value)).c_str()).toDouble();
	this->points[index].rel_damage=results->record(index).value((Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage)).c_str()).toDouble();
	this->points[index].quant_5_rel_damage=results->record(index).value((Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage_5_quant)).c_str()).toDouble();
	this->points[index].quant_95_rel_damage=results->record(index).value((Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::rel_damage_95_quant)).c_str()).toDouble();
}
//Allocate the necessary damage function points, consisting of a value (corresponding to the impact value, e.g. waterlevel) and a relative damage [%]
void Dam_Ecn_Damage_Function::alloc_points(void){
	try{
		this->points=new _dam_ecn_function_point[this->no_points];
		Sys_Memory_Count::self()->add_mem(sizeof(_dam_ecn_function_point)*this->no_points, _sys_system_modules::DAM_SYS);

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//init the points
	for(int i=0; i< this->no_points; i++){
		this->points[i].impact_value=0.0;
		this->points[i].rel_damage=0.0;
		this->points[i].quant_5_rel_damage=0.0;
		this->points[i].quant_95_rel_damage=0.0;
	}
}
//Delete the damage function points
void Dam_Ecn_Damage_Function::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
		Sys_Memory_Count::self()->minus_mem(sizeof(_dam_ecn_function_point)*this->no_points, _sys_system_modules::DAM_SYS);

	}
}
//Check the damage function
void Dam_Ecn_Damage_Function::check_function(void){
	
	if(this->landuse_info.def_stock_5>this->landuse_info.default_stockvalue || this->landuse_info.def_stock_95< this->landuse_info.default_stockvalue){
		Error msg=this->set_error(10);
		ostringstream info;
		info << "Default stockvalue        : " <<this->landuse_info.default_stockvalue << label::euro << endl;
		info << "Default stockvalue (5%q)  : " <<this->landuse_info.def_stock_5 << label::euro << endl;
		info << "Default stockvalue (95%q) : " <<this->landuse_info.def_stock_95 << label::euro << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->no_points==0){
		return;
	}

	//check first point
	if(abs(this->points[0].impact_value)>constant::zero_epsilon || abs(this->points[0].rel_damage)>constant::zero_epsilon){
		Error msg=this->set_error(4);
		throw msg;
	}


	//impact value have to be increasing
	for(int i=0; i< this->no_points; i++){

		
		//impact values can not be negative
		if(this->points[i].impact_value<0.0){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Impact value point " << i <<" : " <<this->points[i].impact_value << endl;
			msg.make_second_info(info.str());
			throw msg;
			
		}
		//impact values can not be negative or above 100%
		if(this->points[i].rel_damage<0.0 || this->points[i].rel_damage>100.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Relative damage point " << i <<" : " <<this->points[i].rel_damage << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		if(this->points[i].quant_5_rel_damage<0.0 || this->points[i].quant_5_rel_damage>100.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Relative damage (5%q) point " << i <<" : " <<this->points[i].quant_5_rel_damage << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		if(this->points[i].quant_95_rel_damage<0.0 || this->points[i].quant_95_rel_damage>100.0){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Relative damage (95%q) point " << i <<" : " <<this->points[i].quant_95_rel_damage << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}

		try{
			this->check_quantiles(&this->points[i]);
		}
		catch(Error msg){
			ostringstream info;
			info << "Point number :" << i << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		if(i<this->no_points-1){
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
//Check the quantiles of the damage function points
void Dam_Ecn_Damage_Function::check_quantiles(_dam_ecn_function_point *point){

	if(point->quant_5_rel_damage>point->rel_damage || point->quant_95_rel_damage<point->rel_damage){
		Error msg=this->set_error(5);
		ostringstream info;
		info << "Relative damage        : " <<this->points->rel_damage << endl;
		info << "Relative damage (5%q)  : " <<this->points->quant_5_rel_damage << endl;
		info << "Relative damage (95%q) : " <<this->points->quant_95_rel_damage << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Create the database table for the damage functions points (static)
void Dam_Ecn_Damage_Function::create_point_table(QSqlDatabase *ptr_database){
		if(Dam_Ecn_Damage_Function::point_table==NULL){
			ostringstream cout;
			cout << "Create economical damage function point database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_ecn_func_point;
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

			tab_col[4].name=dam_label::rel_damage;
			tab_col[4].type=sys_label::tab_col_type_double;
			tab_col[4].unsigned_flag=true;

			tab_col[5].name=dam_label::rel_damage_5_quant;
			tab_col[5].type=sys_label::tab_col_type_double;
			tab_col[5].unsigned_flag=true;

			tab_col[6].name=dam_label::rel_damage_95_quant;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;


			try{
				Dam_Ecn_Damage_Function::point_table= new Tables();
				if(Dam_Ecn_Damage_Function::point_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Ecn_Damage_Function::create_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;	
			}
			catch(Error msg){
				Dam_Ecn_Damage_Function::close_point_table();
				throw msg;
			}

		Dam_Ecn_Damage_Function::close_point_table();
	}

}
//Set the database table for the damage functions points: it sets the table name and the name of the columns and allocate them (static)
void Dam_Ecn_Damage_Function::set_point_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Damage_Function::point_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_ecn_func_point;
		string tab_col[7];
		tab_col[0]=dam_label::function_id;
		tab_col[1]=dam_label::point_id;
		tab_col[2]=dam_label::impact_value;
		tab_col[3]=dam_label::rel_damage;
		tab_col[4]=dam_label::rel_damage_5_quant;
		tab_col[5]=dam_label::rel_damage_95_quant;
		tab_col[6]=dam_label::glob_id;


		try{
			Dam_Ecn_Damage_Function::point_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Ecn_Damage_Function::point_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Ecn_Damage_Function::set_point_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;	
		}
		catch(Error msg){
			Dam_Ecn_Damage_Function::close_point_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the damage functions points (static)
void Dam_Ecn_Damage_Function::delete_data_in_point_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Damage_Function::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Ecn_Damage_Function::point_table->delete_data_in_table(ptr_database);
}
//Close and delete the database table for the damage functions points (static)
void Dam_Ecn_Damage_Function::close_point_table(void){
	if(Dam_Ecn_Damage_Function::point_table!=NULL){
		delete Dam_Ecn_Damage_Function::point_table;
		Dam_Ecn_Damage_Function::point_table=NULL;
	}
}
//Select and count the number of relevant points for the damage function in a database table (static)
int Dam_Ecn_Damage_Function::select_relevant_function_points_database(QSqlTableModel *results, const int index){

	int number=0;
	try{
		Dam_Ecn_Damage_Function::set_point_table(&results->database());
	}
	catch(Error msg){
		throw msg;
	}

	//give the table name to the query model
	results->setTable((Dam_Ecn_Damage_Function::point_table->get_table_name()).c_str());

	//generate the filter
	ostringstream filter;
	filter << Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::function_id) << " = " << index;
	
	//set the filter
	results->setFilter(filter.str().c_str());
	//database request
	Data_Base::database_request(results);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Damage_Function::select_relevant_function_point_database(QSqlTableModel *results,  const int index)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Damage_Function::point_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//sort it
	results->sort(results->fieldIndex(Dam_Ecn_Damage_Function::point_table->get_column_name(dam_label::point_id).c_str()) , Qt::AscendingOrder);
	
	number= results->rowCount();


	return number;
}
//Check if a land use id is already in use; it this is the case return a new land-use id
int Dam_Ecn_Damage_Function::check_landuse_id_is_used(QSqlDatabase *ptr_database, const int id){
	QSqlTableModel results(NULL, *ptr_database);
	int number=0;
	number=Dam_Ecn_Damage_Function::select_relevant_functions_database(&results, false);
	bool new_test=false;
	int new_id=id;

	do{
		new_test=false;
		for(int i=0; i<number; i++){
			if(new_id==results.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str()).toInt()){
				new_id++;
				new_test=true;
				break;
			}
		}
	}while(new_test==true);

	return new_id;
}
//Set error(s)
Error Dam_Ecn_Damage_Function::set_error(const int err_type){
		string place="Dam_Ecn_Damage_Function::";
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
			type=21;
			break;
		case 4://the first point must have 0.0/0.0
			place.append("check_function(void))");
			reason="The start point of the damage function must be 0.0/0.0";
			help="Check the damage function";
			type=21;
			break;
		case 5://the quantiles are not properly set
			place.append("check_quantiles(_dam_ecn_function_point *point)");
			reason="The 5%quantile must be < the relative damage; the 95%quantile must be > the relative damage";
			help="Check the quantiles of the damage function";
			type=21;
			break;
		case 6://wrong input
			place.append("read_function(ifstream *file, int *line_counter, const int index)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage function in file";
			type=1;
			break;
		case 7://given curve number is not found 
			place.append("read_function(ifstream *file, int *line_counter, const int index)");
			reason="The function starts with !BEGIN but no values are found after";
			help="Check the damage function in file";
			type=1;
			break;
		case 8://not all points are found
			place.append("read_function(ifstream *file, int *line_counter, const int index)");
			reason="Not all points of the damage function are found";
			help="Check the file";
			type=1;
			break;
		case 9://wrong number of columns 
			place.append("read_function(ifstream *file, int *line_counter, const int index)");
			reason="The number of columns for specifiying the damage function points have to be 4 (with quantiles) or 2";
			help="Check the file";
			type=1;
			break;
		case 10://the quantiles of the stock value are not properly set
			place.append("check_function(void)");
			reason="The 5%quantile must be < the default stock value; the 95%quantile must be > the default stock value";
			help="Check the quantiles of the default stock values";
			type=21;
			break;
		case 11://number of columns is wrong
			place.append("read_function(ifstream *file, int *line_counter, const int index)");
			reason="There is a problem with the input; wrong number of columns";
			help="Check the damage function in file";
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
Warning Dam_Ecn_Damage_Function::set_warning(const int warn_type){
	string place="Dam_Ecn_Damage_Function::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the damage function (landuse-info) to the database";
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
			reason="Can not submit the general data of the damage function (landuse-info) to the database";
			help="Check the database";
			type=2;
			break;
		case 3://land use id calculation 
			place.append("calculate_damage_landuse_id(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz)");
			reason="Can not calculate the economical damages per landuse id";
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