#include "Dam_Headers_Precompiled.h"
//#include "Dam_Ecn_Raster.h"

//init static members
Tables *Dam_Ecn_Raster::raster_table=NULL;

///Default constructor
Dam_Ecn_Raster::Dam_Ecn_Raster(void){
	this->ecn_elem=NULL;
	this->raster_is_set=false;
	this->type=_dam_raster_types::ecn_total;

	this->reset_result_members();

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Ecn_Raster), _sys_system_modules::DAM_SYS);
}
///Default destructor
Dam_Ecn_Raster::~Dam_Ecn_Raster(void){
	this->delete_raster_elem_info();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Ecn_Raster), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Read in the economic damage data from file: mobile, immobile and individuell stockvalues
void Dam_Ecn_Raster::read_all_damage_raster_per_file(string fname_mob, string fname_immob, string fname_mob_stock, string fname_immob_stock){

    try{
;

		//mobile damages
		if(fname_mob!=label::not_set){
			//damage function information
			if(this->raster_is_set==false){
				this->set_raster_type(_dam_raster_types::ecn_mob);
				this->read_damage_raster_per_file(fname_mob);
				//allocate the element information
				this->allocate_raster_elem_info();

				this->extract_mobile_damage2raster_elem(this);
				this->raster_is_set=true;
			}
			else{
				Dam_Ecn_Raster buffer;
				buffer.set_raster_type(_dam_raster_types::ecn_mob);
				buffer.read_damage_raster_per_file(fname_mob);
				if(this->compare_raster_infos(&buffer)==false){
					Error msg=this->set_error(1);
					ostringstream info;
					info << "Raster of the mobile damage categories"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				else{
					this->extract_mobile_damage2raster_elem(&buffer);
				}
			}
			//individual stock values
			if(this->raster_is_set==true && (fname_mob_stock!= label::not_set && fname_mob_stock!= "") ){
				Dam_Ecn_Raster buffer;
				buffer.set_raster_type(_dam_raster_types::ecn_mob_stock);
				buffer.read_damage_raster_per_file(fname_mob_stock);
				if(this->compare_raster_infos(&buffer)==false){
					Error msg=this->set_error(1);
					ostringstream info;
					info << "Raster of the individual mobile stock values"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				else{
					this->extract_mob_stock_value2raster_elem(&buffer);
				}
			}
		}

		//immobile damages
		if(fname_immob!=label::not_set){
			//damage function information
			if(this->raster_is_set==false){
				this->set_raster_type(_dam_raster_types::ecn_immob);
				this->read_damage_raster_per_file(fname_immob);
				//allocate the element information
				this->allocate_raster_elem_info();
				this->extract_immobile_damage2raster_elem(this);
				this->raster_is_set=true;
			}
			else{
				Dam_Ecn_Raster buffer;
				buffer.set_raster_type(_dam_raster_types::ecn_immob);
				buffer.read_damage_raster_per_file(fname_immob);



				if(this->compare_raster_infos(&buffer)==false){
					Error msg=this->set_error(1);
					ostringstream info;
					info << "Raster of the immobile damage categories"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				else{


					this->extract_immobile_damage2raster_elem(&buffer);



				}
			}
			//individual stock values
			if(this->raster_is_set==true && (fname_immob_stock!= label::not_set && fname_immob_stock!= "")){
				Dam_Ecn_Raster buffer;
				buffer.set_raster_type(_dam_raster_types::ecn_immob_stock);
				buffer.read_damage_raster_per_file(fname_immob_stock);
				if(this->compare_raster_infos(&buffer)==false){
					Error msg=this->set_error(1);
					ostringstream info;
					info << "Raster of the individual immobile stock values"<< endl;
					msg.make_second_info(info.str());
					throw msg;
				}
				else{
					this->extract_immob_stock_value2raster_elem(&buffer);
				}
			}
		}
	}
	catch(Error msg){
		ostringstream info;
		info <<"Raster-id       : " << this->number << endl;
		info <<"Raster-name     : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->set_raster_type(_dam_raster_types::ecn_total);
}
//Transfer the economic damage raster data to a database: the general settings of the raster (geometry etc.) as well as the element information
void Dam_Ecn_Raster::transfer_input_members2database(QSqlDatabase *ptr_database){
	//mysql query with the table_model
	QSqlTableModel model(0,*ptr_database);


	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	ostringstream cout;
	cout <<"Add the general data of the economical raster " << this->name <<" to the database ..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//give the table name to the query model
	model.setTable((Dam_Ecn_Raster::raster_table->get_table_name()).c_str());

	//evaluate the global identifier
	this->number=Dam_Ecn_Raster::raster_table->maximum_int_of_column(Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id),ptr_database)+1;

	//insert row
	model.insertRows(0, 1);

	//insert data
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str())),this->number);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str())),this->name.c_str());
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_x_db)).c_str())),this->no_elems_x);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_y_db)).c_str())),this->no_elems_y);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_x_origin_db)).c_str())),this->x_coor_origin);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_y_origin_db)).c_str())),this->y_coor_origin);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_cellsize_db)).c_str())),this->width_x);
	model.setData(model.index(0,model.record().indexOf((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_noinfo_db)).c_str())),this->no_info_value);

	//submit it to the datbase
	Data_Base::database_submit(&model);

	if(model.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info          : " << model.lastError().text().toStdString() << endl;
		info << "Raster index              : " << this->number << endl;
		info << "Raster name               : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
	//set a polygon datatype of the raster boundary to the datbase table
	//set the query
	Sys_Database_Browser brows_buff;
	brows_buff.set_tablename(Dam_Ecn_Raster::raster_table->get_table_name());
	//set a where filter
	ostringstream buff_filter;
	buff_filter<< Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << " = " << this->number;
	brows_buff.set_where_filter(buff_filter.str());
	//get the query string
	string str_query_buff;
	str_query_buff =brows_buff.get_inserting_polygon_filter(Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_polygon), &this->geometrical_bound);

	//Set the query
	QSqlQuery query_buff(*ptr_database);
	//launch the request
	Data_Base::database_request(&query_buff, str_query_buff, ptr_database);

	if(query_buff.lastError().isValid()){
		Warning msg=this->set_warning(2);
		ostringstream info;
		info << "Table Name                : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
		info << "Raster index              : " << this->number << endl;
		info << "Raster name               : " << this->name << endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}

	cout <<"Transfer " << this->number_polygons <<" elements to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id=Dam_Ecn_Element::get_max_glob_id_elem_table(ptr_database)+1;
	//get the header for the query
	string query_header;
	query_header=Dam_Ecn_Element::get_insert_header_elem_table(ptr_database);
	ostringstream query_data;
	ostringstream query_total;
	int counter=0;

	for(int i=0; i< this->number_polygons; i++){
		Dam_Damage_System::check_stop_thread_flag();
		query_data <<  this->ecn_elem[i].get_datastring_members2database(glob_id , this->number, this->width_x, this->x_coor_origin, this->y_coor_origin) << " ,";
		//count the global index
		glob_id++;
		counter++;

        if(i%25000==0 && i>0){
            cout << "Transfer economical raster elements "<< i <<" to " << i+25000 <<" (from "<<this->number_polygons<<")" <<" to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
        //send packages of 1000
        if(counter==500){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, ptr_database);

			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(5);
				ostringstream info;
				info << "Table Name                : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}
	//send the rest
	if(counter!=0){
		query_total<< query_header << query_data.str();
		//delete last komma
		string buff=query_total.str();
		buff.erase(buff.length()-1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter=0;
		if(query_buff.lastError().isValid()){
			Warning msg=this->set_warning(5);
			ostringstream info;
			info << "Table Name                : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

	query_buff.clear();
}
//Input the economic damage raster data per database: the general settings of the raster (geometry etc.)
void Dam_Ecn_Raster::input_raster_perdatabase_general_data(const QSqlQueryModel *results, const int glob_index){
	this->number=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id)).c_str()).toInt();
	this->name=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_name_db)).c_str()).toString().toStdString();
	this->no_elems_x=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_x_db)).c_str()).toInt();
	this->no_elems_y=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_y_db)).c_str()).toInt();
	this->x_coor_origin=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_x_origin_db)).c_str()).toDouble();
	this->y_coor_origin=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_y_origin_db)).c_str()).toDouble();
	this->width_x=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_cellsize_db)).c_str()).toDouble();
	this->width_y=this->width_x;
	this->no_info_value=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_noinfo_db)).c_str()).toDouble();
	this->connected_flag=results->record(glob_index).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected)).c_str()).toBool();
	try{
		this->check_raster_info();
		this->generate_boundary();
	}
	catch(Error msg){
		ostringstream info;
		info <<"Raster-id       : " << this->number << endl;
		info <<"Raster-name     : " << this->name << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Input the economic damage raster element data per database: element information, the general information has to be set before
void Dam_Ecn_Raster::input_raster_perdatabase_element_data(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		ostringstream prefix;
		prefix << "RAST_"<<this->number<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout <<"Load elements of economical raster "<<this->get_raster_name()<< " from database..." <<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->check_raster_info();

		QSqlQueryModel elem_results;

		//initialize the raster
		Geo_Raster::init_raster();
		this->allocate_raster_elem_info();
		this->set_sys_system_id(id);
		Geo_Raster::clear_raster();

		int number_poly=Dam_Ecn_Element::count_relevant_elements_database(&elem_results, ptr_database, id ,this->number);
		if(number_poly!=this->number_polygons){
			Error msg=this->set_error(2);
			ostringstream info;
			info<<"Required number     : " << this->number_polygons << endl;
			info<<"Found number        : " << number_poly << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		int counter=0;
		int counter2=0;
		//read in points
		for(int i=0; i< this->number_polygons; i++){
			if(i%10000==0 && i>0){
				Dam_Damage_System::check_stop_thread_flag();
				cout << "Input economical raster elements "<< i <<" to " << i+10000 <<"..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if(i==counter*constant::max_rows){
				elem_results.clear();
				Dam_Ecn_Element::select_relevant_elements_database(&elem_results, ptr_database, id ,this->number,   i, constant::max_rows);
				counter++;
				counter2=0;
			}
			this->ecn_elem[i].input_element_perdatabase(&elem_results , counter2);
			counter2++;
		}
	}
	catch(Error msg){
		ostringstream info;
		info <<"Raster-id       : " << this->number << endl;
		info <<"Raster-name     : " << this->name << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	ptr_database->close();
	ptr_database->open();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the members of geometrical damage raster to display/console
void Dam_Ecn_Raster::output_member(void){
	ostringstream prefix;

	prefix << "RAST_"<<this->number<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	Dam_Raster::output_member();
	//output of the elements just for development
	//ostringstream cout;
	//if(this->ecn_elem!=NULL){
	//	cout << "ELEMENT-CHARACTERISTICS"<<endl;
	//	cout << " No" <<W(13) << "Immob_id" << W(15)<< "Mob_id" <<W(15)<< "Stock_immob" << label::euro<<W(15)<< "Stock_mob"<< label::euro<< endl;
	//	Sys_Common_Output::output_dam->output_txt(&cout, true);
	//	for(int i=0; i< this->number_polygons; i++){
	//		cout <<" "<< i << W(13);
	//		this->ecn_elem[i].output_damage_members(&cout);
	//		cout << endl;
	//		Sys_Common_Output::output_dam->output_txt(&cout, true);
	//	}
	//}
	//else{
	//	cout <<"Elementcharacteristics not loaded!"<<endl;
	//	Sys_Common_Output::output_dam->output_txt(&cout, true);
	//}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the number of floodplain connected elements
void Dam_Ecn_Raster::output_number_fp_connected_elem(ostringstream *cout){
	*cout<<" FP-connected elements of " << this->name << "    " << this->count_number_fp_connected_elem() <<" ("<<this->number_polygons<<")"<<endl;
}
//Output the sum of the damage results to display/console
void Dam_Ecn_Raster::output_result_damage(void){
	ostringstream prefix;

	prefix << "RAST_"<<this->number<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "RESULTS"<<endl;
	cout << " Rastername             : " << this->name<< endl;
	cout << " Immobile (Mid/5%/95%)  : " << P(2)<< FORMAT_FIXED_REAL << this->result_immob_dam.mid_result << "  "<< this->result_immob_dam.quantile_5_result<< "  "<< this->result_immob_dam.quantile_95_result << label::euro << endl;
	cout << " Mobile (Mid/5%/95%)    : " <<  this->result_mob_dam.mid_result << "  "<< this->result_mob_dam.quantile_5_result<< "  "<< this->result_mob_dam.quantile_95_result << label::euro << endl;
	cout << " Total (Mid/5%/95%)     : " <<  this->result_total_dam.mid_result << "  "<< this->result_total_dam.quantile_5_result<< "  "<< this->result_total_dam.quantile_95_result << label::euro << endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the results of the immobile damages for the raster
_dam_ecn_result Dam_Ecn_Raster::get_immob_results(void){
	return this->result_immob_dam;
}
//Get the results of the mobile damages for the raster
_dam_ecn_result Dam_Ecn_Raster::get_mob_results(void){
	return this->result_mob_dam;
}
//Get the results of the total damages for the raster
_dam_ecn_result Dam_Ecn_Raster::get_total_results(void){
	return this->result_total_dam;
}
//Calculate the economical damages for the raster
void Dam_Ecn_Raster::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number){
	int counter=0;
	int counter_fp_id=0;
	Dam_Impact_Values *impact;
	//search for each damge element for the corresponding hydraulic impact element
	for(int i=0; i< this->number_polygons; i++){
		if(this->ecn_elem[i].get_index_floodplain()>=0){
			counter=0;
			//search for the floodplain index
			do{
				if(this->ecn_elem[i].get_index_floodplain()==impact_fp[counter_fp_id].get_index_floodplain()){
					break;
				}
				if(counter_fp_id==number-1){
					counter_fp_id=0;
				}
				else{
					counter_fp_id++;
				}
				counter++;
			}
			while(counter < number);
			//search for the element
			if(this->ecn_elem[i].get_index_floodplain_element()>=0 && this->ecn_elem[i].get_index_floodplain_element()<impact_fp[counter_fp_id].get_number_element()){
				impact=&(impact_fp[counter_fp_id].impact_values[this->ecn_elem[i].get_index_floodplain_element()]);
				//calculate the damges
				this->ecn_elem[i].calculate_damages(impact);
			}
		}
	}
}
//Output the economical damages for the raster to database
void Dam_Ecn_Raster::output_results2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output, const bool must_output){
	ostringstream prefix;
	prefix << "RAST_"<<this->number<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout << "Transfer the economical damage results of raster elements of raster " << this->name << " to database..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id=Dam_Ecn_Element::get_max_glob_id_erg_table(ptr_database)+1;
	//get the header for the query
	string query_header;
	query_header=Dam_Ecn_Element::get_insert_header_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter=0;
	string buffer_data;
	bool must_output2=false;

	for(int i=0; i<this->number_polygons; i++){
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->get_number_elements()<<") results of economical raster elements are transfered to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}
		if(i==this->number_polygons-1 && must_output==true && *was_output==false){
			must_output2=true;
		}

		buffer_data = this->ecn_elem[i].get_datastring_results2database(glob_id , bound_sz, break_sz, this->number, this->width_x,this->x_coor_origin, this->y_coor_origin,  must_output2);
		if(buffer_data!=label::not_set){
			query_data <<  buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
			*was_output=true;
		}
		//send packages of 100
		if(counter==100){
			query_total<< query_header << query_data.str();
			//delete last komma
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			query_data.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(6);
				ostringstream info;
				info << "Table Name                : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
		//this->elements[i].output_result_members2database(ptr_database, bound_sz, break_sz, this->number, this->width_x,this->x_coor_origin, this->y_coor_origin);
	}
	//send the rest
	if(counter!=0){
		query_total<< query_header << query_data.str();
		//delete last komma
		string buff=query_total.str();
		buff.erase(buff.length()-1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		query_data.str("");
		counter=0;
		if(query_buff.lastError().isValid()){
			Warning msg=this->set_warning(6);
			ostringstream info;
			info << "Table Name                : " << Dam_Ecn_Element::elem_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}

	query_buff.clear();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Connect the damage functions to the elements
void Dam_Ecn_Raster::connect_dam_function2elems(const int number, Dam_Ecn_Damage_Function *functions){
	//immobile damages
	for(int i=0; i< this->number_polygons; i++){
		for(int j=0; j<number; j++){
			Dam_Damage_System::check_stop_thread_flag();
			if(this->ecn_elem[i].connect_immob_dam2damage_function(this->get_no_info_asint(), &functions[j])==true){
				break;
			}
		}
	}
	//mobile damages
	for(int i=0; i< this->number_polygons; i++){
		Dam_Damage_System::check_stop_thread_flag();
		for(int j=0; j<number; j++){
			if(this->ecn_elem[i].connect_mob_dam2damage_function(this->get_no_info_asint(), &functions[j])==true){
				break;
			}
		}
	}
}
//Ceck if all damage elements could be connected to the damage function
void Dam_Ecn_Raster::check_damage_function_connection(void){
	QList<int> list_id_immob;
	QList<int> list_id_immob_number;

	QList<int> list_id_mob;
	QList<int> list_id_mob_number;

	int id=0;

	for(int i=0; i<this->number_polygons; i++){
		Dam_Damage_System::check_stop_thread_flag();

		if(this->ecn_elem[i].immob_dam_func_is_found()==false){
			if(list_id_immob.contains(this->ecn_elem[i].get_id_immobile_dam_func())==false){
				list_id_immob.append(this->ecn_elem[i].get_id_immobile_dam_func());
				list_id_immob_number.append(1);
			}
			else{
				id=list_id_immob.indexOf(this->ecn_elem[i].get_id_immobile_dam_func());
				list_id_immob_number.replace(id,list_id_immob_number.at(id)+1);
			}
			//reaction
			this->ecn_elem[i].set_noinfo_immob();
		}
		if(this->ecn_elem[i].mob_dam_func_is_found()==false){
			if(list_id_mob.contains(this->ecn_elem[i].get_id_mobile_dam_func())==false){
				list_id_mob.append(this->ecn_elem[i].get_id_mobile_dam_func());
				list_id_mob_number.append(1);
			}
			else{
				id=list_id_mob.indexOf(this->ecn_elem[i].get_id_mobile_dam_func());
				list_id_mob_number.replace(id,list_id_mob_number.at(id)+1);
			}
			//reaction
			this->ecn_elem[i].set_noinfo_mob();
		}
	}

	if(list_id_immob.length()>0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Rastername                     : " <<this->name<< endl;
		info << "Element of the immobile damage" << endl;
		info << "Following indices of damage function are not found"<<endl;
		info << "   Index    Number of elements "<<endl;
		for(int i=0; i<list_id_immob.length(); i++){
			info << "    "<<list_id_immob.at(i)<<"         " << list_id_immob_number.at(i) << endl;
		}
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
	if(list_id_immob.length()>0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Rastername                     : " <<this->name<< endl;
		info << "Element of the mobile damage" << endl;
		info << "Following indices of damage function are not found"<<endl;
		info << "   Index     Number of elements "<<endl;
		for(int i=0; i<list_id_mob.length(); i++){
			info << "    "<<list_id_mob.at(i)<<"          " << list_id_mob_number.at(i) << endl;
		}
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Set intercepted elements to a reduced area
void Dam_Ecn_Raster::set_intercepted_elem2reduced_area(Dam_Ecn_Raster *raster, QSqlDatabase *ptr_database, const _sys_system_id id){
	//check for interception of the boundaries
	if(this->geometrical_bound.check_polygon_interception(&raster->geometrical_bound)==_polygon_intercept::complete_outside ||
		this->geometrical_bound.check_polygon_interception(&raster->geometrical_bound)==_polygon_intercept::outside_with_boundary){
		return;
	}
	raster->input_raster_perdatabase_element_data(ptr_database, id);

	ostringstream cout;
	ostringstream prefix;
	prefix << "RAST_"<<raster->get_raster_number()<<"> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);
	//which has the smaller area
	if(this->get_area()<=raster->get_area()){
		for(int i=0; i< raster->get_number_elements(); i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<raster->get_number_elements()<<") economical raster elements are checked for interception..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				Dam_Damage_System::check_stop_thread_flag();
			}
			if(this->geometrical_bound.check_point_inside(raster->ecn_elem[i].get_mid_point())==true){
				raster->set_area_element2given(i, 0.0);
			}
		}
		raster->transfer_intercepted_elem_data2database(ptr_database);
	}
	else{
		for(int i=0; i< this->get_number_elements(); i++){
			if(i%10000==0 && i>0){
				cout << i <<" ("<<this->get_number_elements()<<") economical raster elements are checked for interception..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if(raster->geometrical_bound.check_point_inside(this->ecn_elem[i].get_mid_point())==true){
				this->set_area_element2given(i, 0.0);
				Dam_Damage_System::check_stop_thread_flag();
			}
		}
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	raster->clear_raster();
}
//Set intercepted elements to the indizes of the hydraulic floodplain
void Dam_Ecn_Raster::set_intercepted_elem2floodplain_id(Hyd_Model_Floodplain *fp_model){
	//check for interception of the boundaries
	if(this->geometrical_bound.check_polygon_interception(&fp_model->raster.geometrical_bound)==_polygon_intercept::complete_outside ||
		this->geometrical_bound.check_polygon_interception(&fp_model->raster.geometrical_bound)==_polygon_intercept::outside_with_boundary){
		return;
	}

	//check the size of the raster elements (damage elements are quadratic!)
	if(this->width_x>fp_model->Param_FP.get_geometrical_info().width_x || this->width_x>fp_model->Param_FP.get_geometrical_info().width_y){
		Warning msg=this->set_warning(4);
		ostringstream info;
		info << "Damage raster name      : " << this->name << endl;
		info << "Damage raster index     : " << this->number<< endl;
		info << "Element width (dam)     : " << this->width_x << label::m << endl;
		info << "Floodplain raster index : " << fp_model->Param_FP.get_floodplain_number() << endl;
		info << "Element width (hyd)     : " << fp_model->Param_FP.get_geometrical_info().width_x << label::m << "  " << fp_model->Param_FP.get_geometrical_info().width_y << label::m<< endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
	ostringstream cout;
	int counter=0;
	int id_fp_elem=0;
	bool found_flag=false;

	int relevant_hyd_elem[9];

	int end_counter=1;

	//Hyd_Floodplainraster_Polygon *raster_elems=NULL;

	bool *raster_elem_outside=NULL;
	try{
			raster_elem_outside=new bool[fp_model->get_number_elements()];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(3);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

	try{
		//fp_model->raster.get_complete_raster_polygons(&raster_elems);

		//check which hyd-polygons are outside of the dam-raster
		for(int i=0; i< fp_model->get_number_elements(); i++){
			fp_model->raster.set_geometrical_raster_polygon(i);
			if(this->geometrical_bound.check_polygon_interception(&(fp_model->raster.raster_elem))==_polygon_intercept::complete_outside){
				//raster_elems[i].set_is_outside_flag(true);
				raster_elem_outside[i]=true;
			}
			else{
				//raster_elems[i].set_is_outside_flag(false);
				raster_elem_outside[i]=false;
			}
		}

		//intercept the elements with floodplain boundary
		for(int i=0; i< this->number_polygons; i++){
		
			found_flag=false;
			if(i%10000==0 && i>0){
				cout << i <<" ("<<this->get_number_elements()<<") economical raster elements are checked for interception with the hydraulic..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				Dam_Damage_System::check_stop_thread_flag();
			}
			counter=0;
			//check if the mid_point is inside the floodplain boundary
			if(fp_model->raster.geometrical_bound.check_point_inside(&this->ecn_elem[i].mid_point)==true){
				do{
					Dam_Damage_System::check_stop_thread_flag();

					fp_model->set_relevant_elem_indices(id_fp_elem, relevant_hyd_elem);
					for(int j=0; j<end_counter; j++){
						if(relevant_hyd_elem[j]<0){
							continue;
						}
						//check if the hyd-elemet is outside
						if(raster_elem_outside[relevant_hyd_elem[j]]==true){
							continue;
						}
						//if(raster_elems[relevant_hyd_elem[j]].get_is_outside_flag()==true){
						//	continue;
						//}
						//search for the floodplain element
						fp_model->raster.set_geometrical_raster_polygon(relevant_hyd_elem[j]);
						if(fp_model->raster.raster_elem.check_point_outside(&this->ecn_elem[i].mid_point)==false){
							id_fp_elem=relevant_hyd_elem[j];
							if(fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::STANDARD_ELEM ||
								fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
								fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::RIVER_ELEM 
								){
									this->ecn_elem[i].set_index_floodplain(fp_model->Param_FP.get_floodplain_number());

									this->ecn_elem[i].set_index_floodplain_element(id_fp_elem);
									
							}
							found_flag=true;
							break;
						}
					}

					if(found_flag==true){
						//nine neighbouring elements
						end_counter=9;
						break;
					}

					//count the element id
					if(id_fp_elem>=fp_model->get_number_elements()-1){
						id_fp_elem=0;
					}
					else{
						id_fp_elem++;
					}
					end_counter=1;
					counter++;
				}
				while(counter<fp_model->get_number_elements());
			}
		}
	}
	catch(Error msg){
		/*if(raster_elems!=NULL){
			delete []raster_elems;
		}*/
		if(raster_elem_outside!=NULL){
			delete []raster_elem_outside;
		}
		throw msg;
	}

	/*if(raster_elems!=NULL){
		delete []raster_elems;
	}*/
	if(raster_elem_outside!=NULL){
		delete []raster_elem_outside;
	}
}
//Set intercepted elements with polygons to the new landuse-id
void Dam_Ecn_Raster::set_intercepted_elem2new_landuse(QSqlDatabase *ptr_database, const _sys_system_id id, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	ostringstream cout;
	_sys_system_id base;
	base.area_state=id.area_state;
	base.measure_nr=0;

	//intercept the elements with polygons
	for(int i=0; i< this->number_polygons; i++){
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->get_number_elements()<<") economical raster elements are checked for interception with the hydraulic..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		//check if the mid_point is inside polygon
		if(polys->check_point_inside(&this->ecn_elem[i].mid_point)==true){
			//set noinfo value
			if(current_id.count()==0 && new_id.count()==0){
				this->ecn_elem[i].set_noinfo_immob();
				this->ecn_elem[i].set_index_immob_dam(this->no_info_value);
				this->ecn_elem[i].set_noinfo_mob();
				this->ecn_elem[i].set_index_mob_dam(this->no_info_value);
				this->ecn_elem[i].transfer_input_members2database(ptr_database, this->number, this->width_x, this->x_coor_origin, this->y_coor_origin);
				Dam_Ecn_Element::switch_applied_flag_elem_table(ptr_database, base, this->ecn_elem[i].get_element_index(), this->number, false);
			}
			else{
				bool transfer_required;
				transfer_required=false;
				//immob function
				for(int j=0; j<current_id.count(); j++){
					if(this->ecn_elem[i].get_id_immobile_dam_func()==current_id.at(j)){
						if(j>=new_id.count()){
							//set noinfo
							this->ecn_elem[i].set_noinfo_immob();
							transfer_required=true;
						}
						else{
							this->ecn_elem[i].set_index_immob_dam(new_id.at(j));
							transfer_required=true;
						}
					}
					//mob function
					else if(this->ecn_elem[i].get_id_mobile_dam_func()==current_id.at(j)){
						if(j>=new_id.count()){
							//set noinfo
							this->ecn_elem[i].set_noinfo_mob();
							transfer_required=true;
						}
						else{
							this->ecn_elem[i].set_index_mob_dam(new_id.at(j));
							transfer_required=true;
						}
					}
				}
				if(transfer_required==true){
					this->ecn_elem[i].transfer_input_members2database(ptr_database, this->number, this->width_x, this->x_coor_origin, this->y_coor_origin);
					Dam_Ecn_Element::switch_applied_flag_elem_table(ptr_database, base, this->ecn_elem[i].get_element_index(), this->number, false);
				}
			}
		}
	}
}
//Transfer element data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
void Dam_Ecn_Raster::transfer_intercepted_elem_data2database(QSqlDatabase *ptr_database){
	//QSqlQuery elem_results(0,*ptr_database);

	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	cout << "Transfer new data of interception of the economical raster elements of raster " << this->name << " to database..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	ostringstream query_total;
	int counter=0;
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	for(int i=0; i<this->number_polygons; i++){
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->get_number_elements()<<") economical raster elements are transfered to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		//this->ecn_elem[i].set_interception_elem_data2database(ptr_database, &elem_results, this->number);

		if(this->ecn_elem[i].get_string_interception_elem_data2database(&query_total)==true){
			counter++;
		};

		//send packages of 100
		if(counter==100){
			//delete last semikolon
			string buff=query_total.str();
			buff.erase(buff.length()-1);
			Data_Base::database_request(&query_buff, buff, ptr_database);
			//delete them
			query_total.str("");
			counter=0;
			if(query_buff.lastError().isValid()){
				Warning msg=this->set_warning(8);
				ostringstream info;
				info << "Table Name                : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
				info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
				msg.make_second_info(info.str());
				msg.output_msg(4);
			}
		}
	}

	//send the rest
	if(counter!=0 ){
		Sys_Common_Output::output_dam->output_txt(&cout);
		//delete last semicolon
		string buff=query_total.str();
		buff.erase(buff.length()-1);
		Data_Base::database_request(&query_buff, buff, ptr_database);
		//delete them
		query_total.str("");
		counter=0;
		if(query_buff.lastError().isValid()){
			Warning msg=this->set_warning(8);
			ostringstream info;
			info << "Table Name                : " << Dam_Ecn_Element::elem_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
	query_buff.clear();
	cout << "Transfer new data of interception of the economical raster elements of raster " << this->name << " to database is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//set the connection is done to the raster
	this->set_connection_flag2database(true, ptr_database);
}
//Create the database table for the general parameters of the raster (static)
void Dam_Ecn_Raster::create_raster_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Raster::raster_table==NULL){
			ostringstream cout;
			cout << "Create economical raster data database table..." << endl ;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//make specific input for this class
			const string tab_name=dam_label::tab_ecn_raster;
			const int num_col=11;
			_Sys_data_tab_column tab_col[num_col];
			//init
			for(int i=0; i< num_col; i++){
				tab_col[i].key_flag=false;
				tab_col[i].unsigned_flag=false;
				tab_col[i].primary_key_flag=false;
			}

			tab_col[0].name=dam_label::raster_id;
			tab_col[0].type=sys_label::tab_col_type_int;
			tab_col[0].unsigned_flag=true;
			tab_col[0].primary_key_flag=true;

			tab_col[1].name=dam_label::raster_name_db;
			tab_col[1].type=sys_label::tab_col_type_string;

			tab_col[2].name=dam_label::raster_no_x_db;
			tab_col[2].type=sys_label::tab_col_type_int;
			tab_col[2].unsigned_flag=true;

			tab_col[3].name=dam_label::raster_no_y_db;
			tab_col[3].type=sys_label::tab_col_type_int;
			tab_col[3].unsigned_flag=true;

			tab_col[4].name=dam_label::raster_x_origin_db;
			tab_col[4].type=sys_label::tab_col_type_double;

			tab_col[5].name=dam_label::raster_y_origin_db;
			tab_col[5].type=sys_label::tab_col_type_double;

			tab_col[6].name=dam_label::raster_cellsize_db;
			tab_col[6].type=sys_label::tab_col_type_double;
			tab_col[6].unsigned_flag=true;

			tab_col[7].name=dam_label::raster_noinfo_db;
			tab_col[7].type=sys_label::tab_col_type_double;

			tab_col[8].name=dam_label::raster_connected;
			tab_col[8].type=sys_label::tab_col_type_bool;
			tab_col[8].default_value="false";

			tab_col[9].name=dam_label::raster_polygon;
			tab_col[9].type=sys_label::tab_col_type_polygon;

			tab_col[10].name=label::description;
			tab_col[10].type=sys_label::tab_col_type_string;

			try{
				Dam_Ecn_Raster::raster_table= new Tables();
				if(Dam_Ecn_Raster::raster_table->create_non_existing_tables(tab_name, tab_col, num_col,ptr_database, _sys_table_type::dam)==false){
					cout << " Table exists" << endl ;
					Sys_Common_Output::output_dam->output_txt(&cout);
				};
			}
			catch(bad_alloc& t){
				Error msg;
				msg.set_msg("Dam_Ecn_Raster::create_raster_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
				ostringstream info;
				info<< "Info bad alloc: " << t.what() << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			catch(Error msg){
				Dam_Ecn_Raster::close_table();
				throw msg;
			}

		Dam_Ecn_Raster::close_table();
	}
}
//Set the database table for the general parameters of the raster: it sets the table name and the name of the columns and allocate them (static)
void Dam_Ecn_Raster::set_raster_table(QSqlDatabase *ptr_database){
	if(Dam_Ecn_Raster::raster_table==NULL){
		//make specific input for this class
		const string tab_id_name=dam_label::tab_ecn_raster;
		string tab_col[11];
		tab_col[0]=dam_label::raster_id;
		tab_col[1]=dam_label::raster_name_db;
		tab_col[2]=dam_label::raster_no_x_db;
		tab_col[3]=dam_label::raster_no_y_db;
		tab_col[4]=dam_label::raster_x_origin_db;
		tab_col[5]=dam_label::raster_y_origin_db;
		tab_col[6]=dam_label::raster_cellsize_db;
		tab_col[7]=dam_label::raster_noinfo_db;
		tab_col[8]=dam_label::raster_polygon;
		tab_col[9]=label::description;
		tab_col[10]=dam_label::raster_connected;

		try{
			Dam_Ecn_Raster::raster_table= new Tables(tab_id_name, tab_col, sizeof(tab_col)/sizeof(tab_col[0]));
			Dam_Ecn_Raster::raster_table->set_name(ptr_database, _sys_table_type::dam);
		}
		catch(bad_alloc& t){
			Error msg;
			msg.set_msg("Dam_Ecn_Raster::set_raster_table(QSqlDatabase *ptr_database)","Can not allocate the memory", "Check the memory", 10, false);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		catch(Error msg){
			Dam_Ecn_Raster::close_table();
			throw msg;
		}
	}
}
//Delete all data in the database table for the general raster parameters as well as the raster elements data (static)
void Dam_Ecn_Raster::delete_data_in_raster_table(QSqlDatabase *ptr_database){
	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete the table
	Dam_Ecn_Raster::raster_table->delete_data_in_table(ptr_database);
}
//Delete all data in the database table for the general raster parameters as well as the raster elements data of the given id (static)
void Dam_Ecn_Raster::delete_data_in_raster_table(QSqlDatabase *ptr_database, const int id){
	QSqlQueryModel results;
	ostringstream test_filter;
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//delete
	test_filter<< "DELETE ";
	test_filter << " FROM " << Dam_Ecn_Raster::raster_table->get_table_name();
	test_filter << " WHERE ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << " = " << id;

	Data_Base::database_request(&results, test_filter.str(), ptr_database);

	//check the request
	if(results.lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Raster::delete_data_in_raster_table(QSqlDatabase *ptr_database, const int id)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info: " << results.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Close and delete the database table for the general parameters of raster (static)
void Dam_Ecn_Raster::close_table(void){
	if(Dam_Ecn_Raster::raster_table!=NULL){
		delete Dam_Ecn_Raster::raster_table;
		Dam_Ecn_Raster::raster_table=NULL;
	}
}
//Check if all economical raster are connected to the hydraulic system (static)
bool Dam_Ecn_Raster::check_all_raster_connected2hyd(QSqlDatabase *ptr_database){
	QSqlQueryModel results;
	int number=0;
	number=Dam_Ecn_Raster::select_relevant_raster_database(&results, ptr_database, false);

	bool connected=false;
	for(int i=0; i<number; i++){
		connected=false;
		connected=results.record(i).value((Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected)).c_str()).toBool();
		if(connected==false){
			return false;
		}
	}
	return true;
}
//Reset the flag, that the hydraulic connection is set (static)
void Dam_Ecn_Raster::reset_hydraulic_connection_flag(QSqlDatabase *ptr_database){
	QSqlQuery elem_results(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
		//generate the filter
	ostringstream filter;
	bool buffer=false;
	filter << "UPDATE " << Dam_Ecn_Raster::raster_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected).c_str() << " = " << functions::convert_boolean2string(buffer) << "  ";

	Data_Base::database_request(&elem_results, filter.str(), ptr_database);

}
//Reset the hydraulic connection in elements (static)
void Dam_Ecn_Raster::reset_hydraulic_connection_elements(QSqlDatabase *ptr_database){
	QSqlQuery elem_results(0,*ptr_database);
	ostringstream filter;
	try{
		Dam_Ecn_Element::set_elem_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	filter << "UPDATE " << Dam_Ecn_Element::elem_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " = -1 , ";
	filter << Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_elem_id).c_str() << " = -1 ";
	filter << " where " <<Dam_Ecn_Element::elem_table->get_column_name(dam_label::conn_fp_id).c_str() << " > " << -1 ;

	Data_Base::database_request(&elem_results, filter.str(), ptr_database);
}
//Select and count the number of relevant damage raster in a database table (static)
int Dam_Ecn_Raster::select_relevant_raster_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const bool with_output){
	int number=0;
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	if(with_output==true){
		ostringstream cout;
		cout << "Search for relevant economical damage raster in database..."<< endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_name_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_x_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_y_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_x_origin_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_y_origin_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_cellsize_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_noinfo_db);
	test_filter << " from " << Dam_Ecn_Raster::raster_table->get_table_name();
	test_filter << " where " <<Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << ">= 0";
	test_filter << " order by " << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Raster::select_relevant_raster_database(QSqlTableModel *results,  const bool with_output)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	if(with_output==true){
		ostringstream cout;
		cout << number << " relevant economical damage raster(s) are found in database" << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	return number;
}
//Select and count the number of relevant damage raster in a database table by a given raster number (static)
int Dam_Ecn_Raster::select_relevant_raster_database(QSqlQueryModel *results, QSqlDatabase *ptr_database, const int raster_no){
	int number=0;
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream test_filter;
	test_filter<< "Select ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_name_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_x_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_no_y_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_x_origin_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_y_origin_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_cellsize_db) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected) << " , ";
	test_filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_noinfo_db);
	test_filter << " from " << Dam_Ecn_Raster::raster_table->get_table_name();
	test_filter << " where " <<Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << "= "<< raster_no;
	test_filter << " order by " << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id);

	Data_Base::database_request(results, test_filter.str(), ptr_database);
	//check the request
	if(results->lastError().isValid()){
		Error msg;
		msg.set_msg("Dam_Ecn_Raster::select_relevant_raster_database(QSqlTableModel *results,  const int raster_no)","Invalid database request", "Check the database", 2, false);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info: " << results->lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	number= results->rowCount();

	return number;
}
//Sum up the total damage results from database
void Dam_Ecn_Raster::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	try{
		Dam_Ecn_Element::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//reset result members
	this->reset_result_members();

	//Set the query
	QSqlQueryModel results;

	//set query
	ostringstream query;
	query << "SELECT ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam) << " , ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam) << " , ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant) << " , ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant) << " , ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant) << " , ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant);
	query <<" FROM "<< Dam_Ecn_Element::elem_erg_table->get_table_name();
	query << " WHERE ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(label::areastate_id) <<" = " << id.area_state<< " AND ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(label::measure_id) <<" = " << id.measure_nr << " AND ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(hyd_label::sz_bound_id) <<" = " << bound_sz <<" AND ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(risk_label::sz_break_id) <<" = '" << break_sz << "' AND ";
	query << Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::raster_id) <<" = " << this->number;

	//set the query
	results.setQuery(query.str().c_str(), *ptr_database);

	//sum up
	for(int i =0; i< results.rowCount(); i++){
		this->result_immob_dam.mid_result=this->result_immob_dam.mid_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_dam)).c_str()).toDouble();
		this->result_mob_dam.mid_result=this->result_mob_dam.mid_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_dam)).c_str()).toDouble();

		this->result_immob_dam.quantile_5_result=this->result_immob_dam.quantile_5_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_5_quant)).c_str()).toDouble();
		this->result_mob_dam.quantile_5_result=this->result_mob_dam.quantile_5_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_5_quant)).c_str()).toDouble();

		this->result_immob_dam.quantile_95_result=this->result_immob_dam.quantile_95_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::immob_95_quant)).c_str()).toDouble();
		this->result_mob_dam.quantile_95_result=this->result_mob_dam.quantile_95_result+results.record(i).value((Dam_Ecn_Element::elem_erg_table->get_column_name(dam_label::mob_95_quant)).c_str()).toDouble();
	}

	this->result_total_dam.mid_result=this->result_immob_dam.mid_result+this->result_mob_dam.mid_result;
	this->result_total_dam.quantile_5_result=this->result_immob_dam.quantile_5_result+this->result_mob_dam.quantile_5_result;
	this->result_total_dam.quantile_95_result=this->result_immob_dam.quantile_95_result+this->result_mob_dam.quantile_95_result;
}
//_______________
//private
//Set the area of an element to the given area
void Dam_Ecn_Raster::set_area_element2given(const int index, const double area){
	if(index<0 || index > this->number_polygons){
		return;
	}
	else{
		this->ecn_elem[index].set_area_element(area);
	}
}
//Allocate the economic information of each raster element
void Dam_Ecn_Raster::allocate_raster_elem_info(void){
	if(this->ecn_elem==NULL){
		try{
			this->ecn_elem=new Dam_Ecn_Element[this->number_polygons];
		}
		catch(bad_alloc &t){
			Error msg=this->set_error(0);
			ostringstream info;
			info<< "Info bad alloc: " << t.what() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//set the area
		for(int i=0; i<this->number_polygons;i++){
			this->ecn_elem[i].set_area_element(this->area);
			this->ecn_elem[i].set_index_element(i);
		}
		this->transfer_midpoint2elem_info();
	}
}
//Delete the economic information of each raster element
void Dam_Ecn_Raster::delete_raster_elem_info(void){
	if(this->ecn_elem!=NULL){
		delete []this->ecn_elem;
		this->ecn_elem=NULL;
	}
}
//Extract the mobile damage categories to the raster element information
void Dam_Ecn_Raster::extract_mobile_damage2raster_elem(Dam_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->ecn_elem[i].set_index_mob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_mob();
		}
	}
}
//Extract the immobile damage categories to the raster element information
void Dam_Ecn_Raster::extract_immobile_damage2raster_elem(Dam_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->ecn_elem[i].set_index_immob_dam(raster->get_characteristics_asint(i));
		if(raster->get_characteristics_asint(i) == (int)this->no_info_value){
			this->ecn_elem[i].set_noinfo_immob();
		}
	}
}
//Extract individual set mobile stock values to the raster element information
void Dam_Ecn_Raster::extract_mob_stock_value2raster_elem(Dam_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->ecn_elem[i].set_indiv_mob_stock_value(raster->get_characteristics_asdouble(i));
	}
}
//Extract individual set immobile stock values to the raster element information
void Dam_Ecn_Raster::extract_immob_stock_value2raster_elem(Dam_Raster *raster){
	for(int i=0; i< this->number_polygons;i++){
		this->ecn_elem[i].set_indiv_immob_stock_value(raster->get_characteristics_asdouble(i));
	}
}
//Transfer the midpoint information from the raster element to the raster element information
void Dam_Ecn_Raster::transfer_midpoint2elem_info(void){
//    for(int i=0; i<this->number_polygons;i++){
//        this->set_geometrical_raster_polygon(i);
//        this->ecn_elem[i].mid_point=this->raster_elem.get_mid_point();
//    }


    //no segments and points of the raster are needed
    double x_buff=0.0;
    double y_buff=0.0;
    double buff_xorigin=this->x_coor_origin+this->width_x*0.5;
    double buff_yorigin=this->y_coor_origin+this->width_y*0.5;;
    int counterx=0;
    int countery=0;
    for(int i=0; i<this->number_polygons;i++){

        x_buff=buff_xorigin+((double)counterx)*this->width_x;
        y_buff=buff_yorigin+((double)countery)*this->width_y;
        counterx++;
        if(counterx==this->no_elems_x){
            counterx=0;
            countery++;
        }

        this->ecn_elem[i].mid_point.set_point_coordinate(x_buff,y_buff);
    }



}
//Set the system-id to the element information
void Dam_Ecn_Raster::set_sys_system_id(const _sys_system_id id){
	for(int i=0; i<this->number_polygons;i++){
		this->ecn_elem[i].set_systemid(id);
	}
}
//Count the number of the floodplain connected elements
int Dam_Ecn_Raster::count_number_fp_connected_elem(void){
	int count=0;
	for(int i=0; i< this->number_polygons; i++){
		if(this->ecn_elem[i].get_index_floodplain()>=0){
			count++;
		}
	}
	return count;
}
//Set the connection flag to database
void Dam_Ecn_Raster::set_connection_flag2database(const bool flag, QSqlDatabase *ptr_database){
	this->connected_flag=flag;

	QSqlQuery elem_results(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Ecn_Raster::set_raster_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
		//generate the filter
	ostringstream filter;
	filter << "UPDATE " << Dam_Ecn_Raster::raster_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_connected).c_str() << " = " << functions::convert_boolean2string(this->connected_flag) << "  ";
	filter << " WHERE " ;
	filter << Dam_Ecn_Raster::raster_table->get_column_name(dam_label::raster_id) << " = " << this->number;

	Data_Base::database_request(&elem_results, filter.str(), ptr_database);
	if(elem_results.lastError().isValid()){
		Warning msg=this->set_warning(7);
		ostringstream info;
		info << "Table Name      : " << Dam_Ecn_Raster::raster_table->get_table_name() << endl;
		info << "Table error info: " << elem_results.lastError().text().toStdString() << endl;
		info << "Raster-id       : "  << this->number <<endl;
		msg.make_second_info(info.str());
		msg.output_msg(4);
	}
}
//Reset the result members
void Dam_Ecn_Raster::reset_result_members(void){
	this->result_immob_dam.mid_result=0.0;
	this->result_mob_dam.mid_result=0.0;
	this->result_total_dam.mid_result=0.0;

	this->result_immob_dam.quantile_5_result=0.0;
	this->result_mob_dam.quantile_5_result=0.0;
	this->result_total_dam.quantile_5_result=0.0;

	this->result_immob_dam.quantile_95_result=0.0;
	this->result_mob_dam.quantile_95_result=0.0;
	this->result_total_dam.quantile_95_result=0.0;
}
//Set error(s)
Error Dam_Ecn_Raster::set_error(const int err_type){
	string place="Dam_Ecn_Raster::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_raster_elem_info(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://geometrical raster information are not equal
			place.append("read_all_damage_raster_per_file(string fname_mob, string fname_immob, string fname_stock)");
			reason="The geometrical raster information are not equal to the base raster information; the !name, !ncols, !nrows, !xllcorner, !yllcorner, !cellsize and !NODATA_value have to be equal";
			help="Check the geometrical raster information and the rastername";
			type=21;
			break;
		case 2://number founded polygon in database do not correspond to the required one
			place.append("input_raster_perdatabase_element_data(const QSqlTableModel *results, const int glob_index, const _sys_system_id id)");
			reason="The number of relevant elements do not correspond to the required number";
			help="Check the database";
			type=4;
			break;
		case 3://bad alloc
			place.append("set_intercepted_elem2floodplain_id(Hyd_Model_Floodplain *fp_model)");
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
Warning Dam_Ecn_Raster::set_warning(const int warn_type){
	string place="Dam_Ecn_Raster::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://damage function is not found
			place.append("check_damage_function_connection(void)");
			reason="Can not connect a damage function to the element";
			help="Check the available damage functions and the given id in the raster";
			reaction="Element is set to no-info";
			type=3;
			break;
		case 1://the raster are intercpeting each other
			place.append("set_intercepted_elem2reduced_area(Dam_Ecn_Raster *raster)");
			reason="The raster are intercepting each other";
			help="Check the raster";
			reaction="The elements of the raster with the greater elements are set to a zero area";
			type=9;
			break;
		case 2://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the general data of the damage raster (geometry etc.) to the database";
			help="Check the database";
			type=2;
			break;
		case 3://input datas can not submitted
			place.append("transfer_intercepted_elem_data2database(QSqlDatabase *ptr_database, const _sys_system_id id)");
			reason="Can not submit the data evaluated by interception of the damage raster elements to the database";
			help="Check the database";
			type=2;
			break;
		case 4://the dam-elements are bigger than the fp-elements
			place.append("set_intercepted_elem2floodplain_id(Hyd_Model_Floodplain *fp_model)");
			reason="The elements of the damage calculation are greater than the elements of the floodplain. The results of the interception are not unique";
			help="Check the economical and the floodplain raster";
			type=9;
			break;
		case 5://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the element data of the damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 6://input datas can not submitted
			place.append("output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz)");
			reason="Can not submit the element result data of the damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 7://input datas can not submitted
			place.append("set_connection_flag2database(const bool flag, QSqlDatabase *ptr_database)");
			reason="Can not submit the connection flag of the raster to database";
			help="Check the database";
			type=2;
			break;
		case 8://input datas can not changed
			place.append("transfer_intercepted_elem_data2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the element data of the damage raster to the database";
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
