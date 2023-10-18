#include "Dam_Headers_Precompiled.h"
//#include "Dam_Sc_Point_Manager.h"

//Default constructor
Dam_Sc_Point_Manager::Dam_Sc_Point_Manager(void){
	this->number_points=0;
	this->point=NULL;
	this->reset_result_value();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Sc_Point_Manager), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Sc_Point_Manager::~Dam_Sc_Point_Manager(void){
	this->delete_points();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Sc_Point_Manager), _sys_system_modules::DAM_SYS);
}
//________
//public
///Read in the simple counting point data from file
void Dam_Sc_Point_Manager::read_points_per_file(string fname){
	ifstream ifile;
	//open file
	ifile.open(fname.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << fname << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter=0;
	string myline;
	int pos=-1;
	stringstream my_stream;
	int found_points=0;

	try{
		//search for the begin
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
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}

		do{
			//next line for the curve number and number of points
			getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
		}
		while(myline.empty()==true && ifile.eof()!=true);

		if(ifile.eof()==true){
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(6);
			msg.make_second_info(info.str());
			throw msg;
		}

		//read it in
		my_stream << myline;
		my_stream >> found_points;

		if(my_stream.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Filename              : " << fname << endl;
			info << "Input must be         :  number_points"<< endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(2);
			msg.make_second_info(info.str());
			throw msg;
		}

		this->number_points=found_points;
		if(this->number_points>constant::max_elems){
			Error msg=this->set_error(7);
			throw msg;
		}

		my_stream.clear();
		my_stream.str("");

		int counter=0;
		int col=0;
		double x_buffer=0.0;
		double y_buffer=0.0;
		int cat_id_buff=-1;
		int subcat_id_buff=-1;
		double score_buff=0.0;
		double bound_buff=0.0;
		string name_buff=label::not_set;

		this->allocate_points();

		//read in the points
		do{
			name_buff=label::not_set;
			subcat_id_buff=-1;

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
				col=functions::count_number_columns(myline);
				my_stream << myline;

				try{
					if(col==5){
						my_stream >> x_buffer >> y_buffer >> cat_id_buff >> score_buff >> bound_buff ;
					}
					else if(col==6){
						my_stream >> x_buffer >> y_buffer >> cat_id_buff >> score_buff >> bound_buff >> name_buff;
					}
					else if(col==7){
						my_stream >> x_buffer >> y_buffer >> cat_id_buff >> score_buff >> bound_buff >> name_buff >>subcat_id_buff;
					}
					else{
						//Error
						Error msg=this->set_error(3);
						throw msg;
					}
				}
				catch(Error msg){
					ostringstream info;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Point id              : "<<counter+1<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}

				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << fname << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Point id              : "<<counter+1<<endl;
					Error msg=this->set_error(4);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				//set the point buffer
				this->point[counter].set_point_coordinate(x_buffer, y_buffer);
				this->point[counter].set_point_name(name_buff);
				this->point[counter].set_number(counter);
				this->point[counter].set_members(cat_id_buff, score_buff, bound_buff, subcat_id_buff);

				counter++;
			}
		}while(counter < this->number_points);

		//check if all is read
		if(counter!=this->number_points && ifile.eof()!=true){
			ostringstream info;
			info << "Filename              : " << fname << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(5);
			msg.make_second_info(info.str());
			throw msg;
		}

		//check for th !END flag
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
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
			info << "Filename " << fname << endl;
			info << "Warning occurs near line: "<< line_counter << endl;
			Warning msg=this->set_warning(0);
			msg.make_second_info(info.str());
			msg.output_msg(4);
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
//Transfer the simple counting point data to a database: the point information
void Dam_Sc_Point_Manager::transfer_input_members2database(QSqlDatabase *ptr_database){
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;

	cout <<"Transfer " << this->number_points <<" points to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id=Dam_Sc_Point::get_max_glob_id_point_table(ptr_database)+1;
	//get the header for the query
	string query_header;
	query_header=Dam_Sc_Point::get_insert_header_point_table(ptr_database);
	ostringstream query_data;
	ostringstream query_total;
	int counter=0;
	string buffer_data;

	for(int i=0; i< this->number_points; i++){
		Dam_Damage_System::check_stop_thread_flag();
		buffer_data =  this->point[i].get_datastring_members2database(glob_id);

		if(buffer_data!=label::not_set){
			query_data <<  buffer_data << " ,";
			//count the global index
			glob_id++;
			counter++;
		}

		if(i%10000==0 && i>0){
			cout << "Transfer simple counting points "<< i <<" to " << i+10000 <<" to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
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
				Warning msg=this->set_warning(1);
				ostringstream info;
				info << "Table Name                : " << Dam_Sc_Point::point_table->get_table_name() << endl;
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
			Warning msg=this->set_warning(1);
			ostringstream info;
			info << "Table Name                : " << Dam_Sc_Point::point_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
}
//Input the simple counting point data per database: point information
void Dam_Sc_Point_Manager::input_perdatabase_point_data(QSqlDatabase *ptr_database, const _sys_system_id id){
	try{
		ostringstream cout;
		cout <<"Load points of simple counting from database..." <<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);

		QSqlQueryModel elem_results;

		int number_p=0;
		number_p=Dam_Sc_Point::count_relevant_points_database(&elem_results, ptr_database, id);
		if(number_p<=0){
			cout << "No simple counting points are found in database"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			return;
		}

		this->number_points=number_p;
		cout <<this->number_points<< " simple counting point(s) are found in database"<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->allocate_points();

		this->set_sys_system_id(id);

		int counter=0;
		int counter2=0;
		//read in points
		for(int i=0; i< this->number_points; i++){
			if(i%10000==0 && i>0){
				Dam_Damage_System::check_stop_thread_flag();
				cout << "Input simple counting points "<< i <<" to " << i+10000 <<"..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
			}
			if(i==counter*constant::max_rows){
				elem_results.clear();
				Dam_Sc_Point::select_relevant_points_database(&elem_results, ptr_database, id ,   i, constant::max_rows);
				counter++;
				counter2=0;
			}
			this->point[i].input_point_perdatabase(&elem_results , counter2);
			counter2++;
		}
	}
	catch(Error msg){
		throw msg;
	}
	ptr_database->close();
	ptr_database->open();
}
//Output the members of geometrical damage raster to display/console
void Dam_Sc_Point_Manager::output_member(void){
	//output of the points
	ostringstream cout;
	cout << "SIMPLE COUNTING POINTS"<<endl;
	cout << " Number points     : " <<W(13) << this->number_points << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(this->number_points>0){
		cout << W(3)<< "No. " << W(8) << "x"<< label::m << W(8) << "y"<< label::m << W(8) << "Cat_Id"<< label::no_unit << W(8) << "Value" << label::score<< W(8) << "Boundary" << label::m << W(8) << "Name" << label::no_unit<<W(8)<< "SubCat_Id"<<label::no_unit<< endl ;
		Sys_Common_Output::output_hyd->output_txt(&cout,true);
		for(int i=0; i< this->number_points; i++){
			cout << W(3)<< i << W(12) << P(2)<< FORMAT_FIXED_REAL<< this->point[i].get_xcoordinate();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL<< this->point[i].get_ycoordinate();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL<< this->point[i].get_cat_id();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL<< this->point[i].get_score_value();
			cout << W(13) << P(2) << FORMAT_FIXED_REAL<< this->point[i].get_boundary_value();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL<< this->point[i].get_point_name();
			cout << W(13) << P(0) << FORMAT_FIXED_REAL<< this->point[i].get_subcat_id()<<endl;

			Sys_Common_Output::output_dam->output_txt(&cout, true);
		}
	}
}
//Output the sum of the damage results to display/console
void Dam_Sc_Point_Manager::output_result_damage(void){
	ostringstream cout;
	cout << " RESULTS PER CATEGORY"<<endl;
	cout << "  1 Public buildings                          : " << P(2)<< FORMAT_FIXED_REAL << this->affected_score_pup_build << label::score<< endl;
	cout << "  2 Ecologic perilous sites or buildings      : " << this->affected_score_eco_build << label::score<< endl;
	cout << "  3 Cultural heritage                         : " << this->affected_score_cult_build << label::score<< endl;
	cout << "  4 Buildings with highly vulnerable person    : " << this->affected_score_person_build << label::score<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Sum up the total damage results from database
void Dam_Sc_Point_Manager::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	try{
		Dam_Sc_Point::set_erg_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
	//reset result members
	this->reset_result_value();

	//Set the query
	QSqlQueryModel results;

	//set query
	ostringstream query;
	query << "SELECT ";
	query << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)<<" , "  ;
	query << Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id)<<"  "  ;
	query <<" FROM "<< Dam_Sc_Point::point_erg_table->get_table_name();
	query << " WHERE ";
	query << Dam_Sc_Point::point_erg_table->get_column_name(label::areastate_id) <<" = " << id.area_state<< " AND ";
	query << Dam_Sc_Point::point_erg_table->get_column_name(label::measure_id) <<" = " << id.measure_nr << " AND ";
	query << Dam_Sc_Point::point_erg_table->get_column_name(hyd_label::sz_bound_id) <<" = " << bound_sz <<" AND ";
	query << Dam_Sc_Point::point_erg_table->get_column_name(risk_label::sz_break_id) <<" = '" << break_sz << "' ";

	//set the query
	results.setQuery(query.str().c_str(), *ptr_database);

	int buff=0;
	//sum up
	for(int i =0; i< results.rowCount(); i++){
		buff=results.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::category_id)).c_str()).toInt();
		if(buff==1){
			this->affected_score_pup_build+=results.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)).c_str()).toDouble();
		}
		else if(buff==2){
			this->affected_score_eco_build+=results.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)).c_str()).toDouble();
		}
		else if(buff==3){
			this->affected_score_cult_build+=results.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)).c_str()).toDouble();
		}else if(buff==4){
			this->affected_score_person_build+=results.record(i).value((Dam_Sc_Point::point_erg_table->get_column_name(dam_label::affected_score)).c_str()).toDouble();
		}
	}
}
//Connect the simple-counting subcategory to the point and calculate the damage per subcategory
double Dam_Sc_Point_Manager::connect_sc_subcategory2points(const int number_categories, Dam_Sc_Subcategory *sc_subcategory){
	//reset them
	for(int i=0; i<number_categories; i++){
		sc_subcategory[i].reset_affected_score();
	}

	bool found=false;
	double sum=0.0;
	//Set new value
	for(int j=0; j<this->number_points; j++){
		found=false;
		if(this->point[j].get_subcat_id()>=0 && this->point[j].get_score_value()>0.0){
			for(int i=0; i<number_categories; i++){
				if(this->point[j].get_subcat_id()==sc_subcategory[i].get_id()){
					found=true;
					sc_subcategory[i].add_affected_score(this->point[j].get_affected_score_value());
					break;
				}
			}
		}
		if(found==false){
			sum=sum+this->point[j].get_score_value();
		}
	}

	return sum;
}
//Set intercepted point to the indizes of the hydraulic floodplain
void Dam_Sc_Point_Manager::set_intercepted_point2floodplain_id(Hyd_Model_Floodplain *fp_model){
	ostringstream cout;
	int counter=0;
	int id_fp_elem=0;
	bool found_flag=false;

	int relevant_hyd_elem[9];
	//Hyd_Floodplainraster_Polygon *raster_elems=NULL;
	int end_counter=1;

	try{
		//intercept the elements with floodplain boundary
		for(int i=0; i< this->number_points; i++){
			found_flag=false;
			if(i%10000==0 && i>0){
				cout << i <<" ("<<this->number_points<<") simple counting points are checked for interception with the hydraulic..."<< endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				Dam_Damage_System::check_stop_thread_flag();
			}
			counter=0;
			//check if the mid_point is inside the floodplain boundary
			if(fp_model->raster.geometrical_bound.check_point_inside_atboundary(&this->point[i])==true){
                if(fp_model->Param_FP.get_geometrical_info().angle==0.0){
                    int col=(int)((this->point[i].get_xcoordinate()-fp_model->Param_FP.get_geometrical_info().origin_global_x)/fp_model->Param_FP.get_geometrical_info().width_x);
                    int row=(int)((this->point[i].get_ycoordinate()-fp_model->Param_FP.get_geometrical_info().origin_global_y)/fp_model->Param_FP.get_geometrical_info().width_y);
                    id_fp_elem=row*fp_model->Param_FP.get_geometrical_info().number_x+col;
                    if(id_fp_elem<0 || id_fp_elem>fp_model->get_number_elements()-1){
                     continue;
                    }

                    if(fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::STANDARD_ELEM ||
                        fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
                        fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::RIVER_ELEM ){
                            this->point[i].set_index_floodplain(fp_model->Param_FP.get_floodplain_number());

                            this->point[i].set_index_floodplain_element(id_fp_elem);

                    }
                }
                else{

                    do{
                        Dam_Damage_System::check_stop_thread_flag();

                        fp_model->set_relevant_elem_indices(id_fp_elem, relevant_hyd_elem);
                        for(int j=0; j<end_counter; j++){
                            if(relevant_hyd_elem[j]<0){
                                continue;
                            }
                            //search for the floodplain element
                            fp_model->raster.set_geometrical_raster_polygon(relevant_hyd_elem[j]);
                            if(fp_model->raster.raster_elem.check_point_outside(&this->point[i])==false){
                                id_fp_elem=relevant_hyd_elem[j];
                                if(fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::STANDARD_ELEM ||
                                    fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::DIKELINE_ELEM ||
                                    fp_model->floodplain_elems[id_fp_elem].get_elem_type()==_hyd_elem_type::RIVER_ELEM ){
                                        this->point[i].set_index_floodplain(fp_model->Param_FP.get_floodplain_number());

                                        this->point[i].set_index_floodplain_element(id_fp_elem);

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
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer point data evaluated by an interception to database: identifier of the floodplain, -floodplain element
void Dam_Sc_Point_Manager::transfer_intercepted_point_data2database(QSqlDatabase *ptr_database){
	//QSqlQuery elem_results(0,*ptr_database);
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	ostringstream cout;
	cout << "Transfer new data of interception of the simple counting points to database..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	ostringstream query_total;
	int counter=0;
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	for(int i=0; i<this->number_points; i++){
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->number_points<<") simple counting points are transfered to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if(this->point[i].get_string_interception_point_data2database(&query_total)==true){
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
				Warning msg=this->set_warning(2);
				ostringstream info;
				info << "Table Name                : " << Dam_Sc_Point::point_table->get_table_name() << endl;
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
			Warning msg=this->set_warning(2);
			ostringstream info;
			info << "Table Name                : " << Dam_Sc_Point::point_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
	query_buff.clear();
	cout << "Transfer new data of interception of the simple counting points to database is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
}
//Check the connection to the hydraulic of the points (static)
bool Dam_Sc_Point_Manager::check_connection2hydraulic(QSqlDatabase *ptr_database,  const _sys_system_id id){
	QSqlQueryModel buffer;
	int number=Dam_Sc_Point::count_relevant_points_database(&buffer, ptr_database, id, false);
	if(number==0){
		return true;
	}
	number=Dam_Sc_Point::count_not_connected_points2hyd(ptr_database, id);

	if(number==0){
		return true;
	}
	else{
		return false;
	}
}
//Get the number of points
int Dam_Sc_Point_Manager::get_number_points(void){
	return this->number_points;
}
//Calculate the damages
void Dam_Sc_Point_Manager::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number){
	int counter=0;
	int counter_fp_id=0;
	Dam_Impact_Values *impact;
	//search for each damage element for the corresponding hydraulic impact element
	for(int i=0; i< this->number_points; i++){
		if(this->point[i].get_index_floodplain()>=0){
			counter=0;
			//search for the floodplain index
			do{
				if(this->point[i].get_index_floodplain()==impact_fp[counter_fp_id].get_index_floodplain()){
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
			if(this->point[i].get_index_floodplain_element()>=0 && this->point[i].get_index_floodplain_element()<impact_fp[counter_fp_id].get_number_element()){
				impact=&(impact_fp[counter_fp_id].impact_values[this->point[i].get_index_floodplain_element()]);
				//calculate the damges
				this->point[i].calculate_damages(impact);
			}
		}
	}
}
//Output the simple counting damages to database
void Dam_Sc_Point_Manager::output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz, bool *was_output, const bool must_output){
	//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout << "Transfer the simple counting damage results to database..."<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//get the global index
	int glob_id=Dam_Sc_Point::get_max_glob_id_point_erg_table(ptr_database)+1;
	//get the header for the query
	string query_header;
	query_header=Dam_Sc_Point::get_insert_header_erg_table(ptr_database);

	ostringstream query_data;
	ostringstream query_total;
	int counter=0;
	string buffer_data;
	bool must_output2=false;

	for(int i=0; i<this->number_points; i++){
		if(i%10000==0 && i>0){
			cout << i <<" ("<<this->number_points<<") results of simple counting are transfered to database..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Damage_System::check_stop_thread_flag();
		}

		if(i==this->number_points-1 && must_output==true && *was_output==false){
			must_output2=true;
		}

		buffer_data = this->point[i].get_datastring_results2database(glob_id , bound_sz, break_sz,  must_output2);
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
				Warning msg=this->set_warning(3);
				ostringstream info;
				info << "Table Name                : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
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
			Warning msg=this->set_warning(3);
			ostringstream info;
			info << "Table Name                : " << Dam_Sc_Point::point_erg_table->get_table_name() << endl;
			info << "Table error info          : " << query_buff.lastError().text().toStdString() << endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
}
//Check if the points are connected to the hydraulic
int Dam_Sc_Point_Manager::check_points_connected2hyd(void){
	int counter=0;

	for(int i=0; i< this->number_points; i++){
		if(this->point[i].get_connected_flag()==true){
			counter++;
		}
	}

	return counter;
}
//Get the affected score for category public buildings
double Dam_Sc_Point_Manager::get_score_pub_build(void){
	return this->affected_score_pup_build;
}
//Get the affected score for category ecologic perilous sites or buildings
double Dam_Sc_Point_Manager::get_score_eco_build(void){
	return this->affected_score_eco_build;
}
//Get the affected score for category cultural heritage
double Dam_Sc_Point_Manager::get_score_cult_build(void){
	return this->affected_score_cult_build;
}
//Get the affected score for category buildings with highly vulnerable person
double Dam_Sc_Point_Manager::get_score_person_build(void){
	return this->affected_score_person_build;
}
//Reset the flag, that the hydraulic connection is set (static)
void Dam_Sc_Point_Manager::reset_hydraulic_connection_flag(QSqlDatabase *ptr_database){
	QSqlQuery elem_results(0,*ptr_database);

	//the table is set (the name and the column names) and allocated
	try{
		Dam_Sc_Point::set_point_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
		//generate the filter
	ostringstream filter;
	bool buffer=false;
	filter << "UPDATE " << Dam_Sc_Point::point_table->get_table_name().c_str();
	filter << " SET ";
	filter << Dam_Sc_Point::point_table->get_column_name(dam_label::raster_connected).c_str() << " = " << functions::convert_boolean2string(buffer) << "  ";

	Data_Base::database_request(&elem_results, filter.str(), ptr_database);
}
//________
//private
//Reset the result value
void Dam_Sc_Point_Manager::reset_result_value(void){
	this->affected_score_pup_build=0.0;
	this->affected_score_eco_build=0.0;
	this->affected_score_cult_build=0.0;
	this->affected_score_person_build=0.0;
}
//Allocate the simple-counting points
void Dam_Sc_Point_Manager::allocate_points(void){
	try{
		this->point=new Dam_Sc_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the simple-counting points
void Dam_Sc_Point_Manager::delete_points(void){
	if(this->point!=NULL){
		delete []this->point;
		this->point=NULL;
	}
}
//Set the system-id to thepoint information
void Dam_Sc_Point_Manager::set_sys_system_id(const _sys_system_id id){
	for(int i=0; i<this->number_points;i++){
		this->point[i].set_systemid(id);
	}
}
//Set error(s)
Error Dam_Sc_Point_Manager::set_error(const int err_type){
	string place="Dam_Sc_Point_Manager::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open file
			place.append("read_points_per_file(string fname)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 2://wrong input sequenze
			place.append("read_points_per_file(string fname)");
			reason="Wrong input by reading the number of points";
			help="Check the file";
			type=5;
		case 3://not all info there
			place.append("read_points_per_file(string fname)");
			reason="Not all relevant point data are found in file";
			help="Check the point data in file";
			type=5;
			break;
		case 4://can not read in the x-ycoordinates properly
			place.append("read_points_per_file(string fname)");
			reason="Can not read in the x-, y-coordinates and data of the simple counting points properly";
			help="Check the coordinates and other data in file";
			type=5;
			break;
		case 5://not all points are found
			place.append("read_points_per_file(string fname)");
			reason="Not all points are found";
			help="Check the number of points and the given point coordinates";
			info << "Number of searched points " << this->number_points << endl;
			type=5;
			break;
		case 6://not all data are found
			place.append("read_points_per_file(string fname)");
			reason="Not all data are found";
			help="Check the file";
			type=5;
			break;
		case 7://to much points
			place.append("read_points_per_file(string fname)");
			reason="To much points are given; Maximum is 1000000 points";
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
Warning Dam_Sc_Point_Manager::set_warning(const int warn_type){
	string place="Dam_Sc_Point_Manager::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://no !END-flag is found in file
			place.append("read_points_per_file(string fname)");
			reason="The point data block is not closed with !END in file";
			help="Check the file and the number of point specification in file";
			type=1;
			break;
		case 1://input datas can not submitted
			place.append("transfer_input_members2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the point data to the database";
			help="Check the database";
			type=2;
			break;
		case 2://input datas can not changed
			place.append("transfer_intercepted_point_data2database(QSqlDatabase *ptr_database)");
			reason="Can not submit the element data of the damage raster to the database";
			help="Check the database";
			type=2;
			break;
		case 3://output datas can not submitted
			place.append("output_results2database(QSqlDatabase *ptr_database,const int bound_sz, const string break_sz)");
			reason="Can not submit the point result data of simple counting to the database";
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
