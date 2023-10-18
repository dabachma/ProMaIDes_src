#include "Dam_Headers_Precompiled.h"
//#include "Dam_Sc_System.h"

//Default constructor
Dam_Sc_System::Dam_Sc_System(void){
	this->subcategories=NULL;
	this->number_subcategories=0;

	this->file_subcat=label::not_set;
	this->file_points=label::not_set;

	this->del_subcat_flag=true;
	this->del_points_flag=true;
	this->del_res_flag=true;

	this->score_not_con2subcat=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Sc_System)-sizeof(Dam_Sc_Point_Manager), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Sc_System::~Dam_Sc_System(void){
	this->delete_subcategories();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Sc_System)-sizeof(Dam_Sc_Point_Manager), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Import the simple counting subcategories from a file to the database
void Dam_Sc_System::sc_subcategory_file2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	this->set_start_warnings_number();

	ostringstream cout;
	try{
		cout <<"Import the data of the simple counting subcategories to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		//read the data in
		this->read_sc_subcategory_per_file(this->file_subcat);
		Dam_Sc_Subcategory::delete_data_in_table(ptr_database);
		this->transfer_subcat2database(ptr_database);
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		Dam_Sc_Subcategory::delete_data_in_table(ptr_database);
		msg.output_msg(4);
	}

	this->set_warning_number();

	cout <<"Importing the data of the simple counting subcategories is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->output_error_statistic();
}
//Read in the simple counting subcategories from database
void Dam_Sc_System::read_sc_subcategory_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		try{
		ostringstream cout;
		cout <<"Load subcategories of simple counting from database..." <<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);

		QSqlQueryModel elem_results;

		int number_p=0;
		number_p=Dam_Sc_Subcategory::select_relevant_subcategory_database(&elem_results, ptr_database);
		if(number_p<=0){
			cout << "No simple counting subcategories are found in database"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			return;
		}

		this->number_subcategories=number_p;
		cout <<this->number_subcategories<< " simple counting subcategories are found in database"<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->allocate_subcategories();

		//read in subcategories
		for(int i=0; i< this->number_subcategories; i++){
			this->subcategories[i].input_subcategory_perdatabase(&elem_results , i);
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a simple counting point data from file to database
void Dam_Sc_System::add_damage_points_file2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	this->set_start_warnings_number();

		ostringstream cout;
	try{
		cout <<"Import the data of the simple counting points to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_points!=label::not_set){
			//read the data in
			this->point_manager.read_points_per_file(this->file_points);
			Dam_Sc_Point::delete_data_in_point_table(ptr_database);
			this->point_manager.transfer_input_members2database(ptr_database);
		}
		else{
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Filename of point information           : " << this->file_points <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		Dam_Sc_Point::delete_data_in_point_table(ptr_database);
		msg.output_msg(4);
	}

	this->set_warning_number();

	cout <<"Importing the data of the simple counting points is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
	this->output_error_statistic();
}
//Read in the simple counting points from database
void Dam_Sc_System::read_sc_points_per_database(QSqlDatabase *ptr_database, const _sys_system_id id, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "SC> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the simple counting point information per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		this->point_manager.input_perdatabase_point_data(ptr_database, id);
	}
	catch(Error msg){
		if(with_output==true){
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}
	if(with_output==true){
		Sys_Common_Output::output_dam->rewind_userprefix();
	}
}
//Read in the simple counting damage system per database
void Dam_Sc_System::read_system_per_database(QSqlDatabase *ptr_database, const _sys_system_id id){
	this->read_sc_points_per_database(ptr_database, id);
	this->read_sc_subcategory_per_database(ptr_database);
}
//Delete all simple counting data in the database: points, subcategories, results etc.
void Dam_Sc_System::del_damage_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "SC> ";

	ostringstream cout;
	cout <<"Delete all simple counting damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	//ecn
	try{
		if(this->del_points_flag==true){
			cout <<"SC-damage points..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Sc_Point::delete_data_in_point_table(ptr_database);
		}
		if(this->del_subcat_flag==true){
			cout <<"SC-damage subcategory..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Sc_Subcategory::delete_data_in_table(ptr_database);
		}
		if(this->del_res_flag==true){
			cout <<"SC-damage results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Sc_Point::delete_data_in_erg_table(ptr_database);
		}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of simple counting damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Ask for the file of the simple counting subcategories per dialog
bool Dam_Sc_System::ask_file_subcategories(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_category_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":sc_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the simple counting subcategory(ies) are stored."<< endl;
	buffer << "This file contains the definition of the subcategories"<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose SC-subcategory file");
	my_dia.get_ptr_file_browser(0)->set_text_label("SC-subcategory file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_subcat=my_dia.get_ptr_file_browser(0)->get_file_name();

		if(this->file_subcat==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the SC-subcategory!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		return true;
	}
}
//Ask for the file of the simple counting damage points per dialog
bool Dam_Sc_System::ask_file_damage_points(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_category_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":sc_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the simple counting point(s) are stored."<< endl;
	buffer << "This file contains the definition of the points, their score value, \ntheir boundary value, their category definition"<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose SC-point file");
	my_dia.get_ptr_file_browser(0)->set_text_label("SC-point file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_points=my_dia.get_ptr_file_browser(0)->get_file_name();

		if(this->file_points==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the SC-points!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		return true;
	}
}
//Ask for the flag, what should be deleted (points, subcategories)
bool Dam_Sc_System::ask_deleting_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":sc_icon");
	my_dia.set_number_check_boxes(3,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the SC-system in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting SC-system");
	my_dia.get_ptr_check_box(0)->setText("Delete SC-damage points");
	my_dia.get_ptr_check_box(1)->setText("Delete SC-subcategories");
	my_dia.get_ptr_check_box(2)->setText("Delete SC-results");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->del_points_flag=my_dia.get_bool_check_box(0);
		this->del_subcat_flag=my_dia.get_bool_check_box(1);
		this->del_res_flag=my_dia.get_bool_check_box(2);
		return true;
	}
}
//Output the members
void Dam_Sc_System::output_members(void){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;

	if(this->number_subcategories>0){
		cout << "Output "<<this->number_subcategories<<" subcategory(s) of the simple counting damage..." << endl;
		cout << " Id "<< "     " << "Name " << "   "<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_subcategories; i++){
			this->subcategories[i].output_members(&cout);
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}

	cout << "Output the points of the simple counting damage..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->point_manager.output_member();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of simple counting system
void Dam_Sc_System::output_statistic(void){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"DAMAGE POINTS"<<endl;
	cout << " Number             " << this->point_manager.get_number_points() << endl;
	cout<<"DAMAGE SUBCATEGORIES"<<endl;
	cout << " Number             " << this->number_subcategories << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the damage results to display/console
void Dam_Sc_System::output_result_damage(void){
	ostringstream prefix;

	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "TOTAL RESULTS"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->point_manager.output_result_damage();
	if(this->number_subcategories>0){
		cout << " RESULTS PER SUBCATEGORY"<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_subcategories; i++){
			cout << "   "<<this->subcategories[i].get_id() << "   " <<this->subcategories[i].get_subcategory_name()<< "  " << P(2)<< FORMAT_FIXED_REAL<<this->subcategories[i].get_affected_score()<<label::score <<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
		cout << "   "<<"No subcategory" << "   " << "  " <<P(2)<< FORMAT_FIXED_REAL<<this->score_not_con2subcat <<label::score <<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
///Intercept simple counting points with the hydraulic elements
void Dam_Sc_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	cout <<"Check the interception of simple counting points to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int *rang=NULL;
	cout <<"Sort hydraulic floodplain models..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);
	try{
		this->point_manager.input_perdatabase_point_data(ptr_database, id);
        if(this->point_manager.get_number_points()>0){

            for(int j=0; j< number_fp; j++){
                cout <<"Check the interception of simple counting points to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
                Sys_Common_Output::output_dam->output_txt(&cout);
                prefix <<"FP_"<<fp_models[rang[j]].Param_FP.get_floodplain_number()<<"> ";
                Sys_Common_Output::output_dam->set_userprefix(&prefix);

                bool delete_again=false;
                if(fp_models[rang[j]].floodplain_elems==NULL || fp_models[rang[j]].raster.get_ptr_raster_points()==NULL || fp_models[rang[j]].raster.get_ptr_raster_segments()==NULL){
                    delete_again=true;
                    QSqlQueryModel bound_result;
                    int number_bound=0;
                    cout <<"Generate elements, raster points and segments for the interception of simple counting points to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
                    Sys_Common_Output::output_dam->output_txt(&cout);
                    fp_models[rang[j]].input_elems_database(ptr_database, &bound_result,number_bound, false, true);
                }
                this->point_manager.set_intercepted_point2floodplain_id(&fp_models[rang[j]]);

                if(delete_again==true){
                    cout <<"Delete elements, raster points and segments of the hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
                    Sys_Common_Output::output_dam->output_txt(&cout);
                    fp_models[rang[j]].delete_elems_raster_geo();
                }
                Dam_Damage_System::check_stop_thread_flag();
                Sys_Common_Output::output_dam->rewind_userprefix();
            }

            this->point_manager.transfer_intercepted_point_data2database(ptr_database);
        }
        }
        catch(Error msg){
            if(rang!=NULL){
                delete []rang;
            }
            throw msg;
        }

	if(rang!=NULL){
		delete []rang;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element
void Dam_Sc_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "SC> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Transfer the data from the interception of the simple counting points to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->point_manager.transfer_intercepted_point_data2database(ptr_database);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Calculate the damages
void Dam_Sc_System::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number){
	ostringstream cout;
	cout << "Calculate damages for the simple counting system..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->point_manager.calculate_damages(impact_fp, number);
}
//Delete the result members for a given system-id and a scenario (boundary-, break-)
void Dam_Sc_System::delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_Sc_Point::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the result members to a database table
void Dam_Sc_System::output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(*was_output==false){
		must_output=true;
	}
	cout << "Output simple counting damages to database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->point_manager.output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_Sc_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	this->point_manager.sum_total_results(ptr_database, id, bound_sz, break_sz);
	this->score_not_con2subcat=this->point_manager.connect_sc_subcategory2points(this->number_subcategories, this->subcategories);
}
//Check the damage system
void Dam_Sc_System::check_system(void){
	if(this->point_manager.get_number_points()!=0){
		int counter=this->point_manager.check_points_connected2hyd();
		if(counter<this->point_manager.get_number_points()){
			Warning msg=this->set_warning(1);
			ostringstream info;
			info << "Points connected           : " << counter <<endl;
			info << "Total number points        : " << this->point_manager.get_number_points() <<endl;
			msg.make_second_info(info.str());
			msg.output_msg(4);
		}
	}
}
//Get a text for deleting simple counting information in database
string Dam_Sc_System::get_deleting_text(void){
	ostringstream buffer;
	if(this->del_points_flag==true){
		buffer << " The point(s) will be deleted in database, also the results!"<<endl;
	}
	if(this->del_subcat_flag==true){
		buffer << " All simple counting categories will be deleted in database!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//_____________
//private
//Read in the simple counting subcategories from file
void Dam_Sc_System::read_sc_subcategory_per_file(const string file){
	ifstream ifile;
	//open file
	ifile.open(file.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(2);
		ostringstream info;
		info << "Filename: " << file << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter=0;
	string myline;
	int pos=-1;
	stringstream my_stream;
	int found_cat=0;

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
			info << "Filename              : " << file << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(3);
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
			info << "Filename              : " << file << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(3);
			msg.make_second_info(info.str());
			throw msg;
		}

		//read it in
		my_stream << myline;
		my_stream >> found_cat;
		if(my_stream.fail()==true){
			ostringstream info;
			info << "Wrong input sequenze  : " << my_stream.str() << endl;
			info << "Filename              : " << file << endl;
			info << "Input must be         :  number_subcategories"<< endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(4);
			msg.make_second_info(info.str());
			throw msg;
		}
		my_stream.clear();
		my_stream.str("");

		this->number_subcategories=found_cat;

		int counter=0;
		int col=0;
		int subcat_id_buff=-1;
		string name_buff=label::not_set;

		this->allocate_subcategories();

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
					if(col==2){
						my_stream >>  subcat_id_buff >> name_buff ;
					}
					else{
						//Error
						Error msg=this->set_error(5);
						throw msg;
					}
				}
				catch(Error msg){
					ostringstream info;
					info << "Filename              : " << file<< endl;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Subcategory Id        : "<<counter+1<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}

				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << file<< endl;
					info << "Error occurs near line: "<< line_counter << endl;
					info << "Subcategory Id        : "<<counter+1<<endl;
					Error msg=this->set_error(6);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				//set the point buffer
				this->subcategories[counter].set_members(subcat_id_buff, name_buff);

				counter++;
			}
		}
		while(counter<this->number_subcategories);

		//check if all is read
		if(counter!=this->number_subcategories && ifile.eof()!=true){
			ostringstream info;
			info << "Filename              : " << file << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(7);
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
			info << "Filename " << file << endl;
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
//Transfer the data of the simple counting subcategories to database
void Dam_Sc_System::transfer_subcat2database(QSqlDatabase *ptr_database){
		//Set the query
	QSqlQuery query_buff(*ptr_database);

	ostringstream cout;
	cout <<"Transfer " << this->number_subcategories <<" subcategories to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	for(int i=0; i< this->number_subcategories; i++){
		Dam_Damage_System::check_stop_thread_flag();
		this->subcategories[i].transfer_input_members2database(ptr_database);
	}
}
//Allocate the simple counting subcategories
void Dam_Sc_System::allocate_subcategories(void){
	try{
		this->subcategories=new Dam_Sc_Subcategory[this->number_subcategories];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the simple counting subcategories
void Dam_Sc_System::delete_subcategories(void){
	if(this->subcategories!=NULL){
		delete []this->subcategories;
		this->subcategories=NULL;
	}
}
//Output the error_statistic of the system
void Dam_Sc_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "SC> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the SC-system..." << endl ;
	cout << "GENERAL"<< endl;
	if(this->number_subcategories>0){
		cout << " Number subcategories     :" << W(3) << this->number_subcategories <<endl;
	}
	if(this->point_manager.get_number_points()>0){
		cout << " Number point(s)          :" << W(3) << this->point_manager.get_number_points() <<endl;
	}
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                 :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)               :" << W(3) << this->get_occured_warning() <<endl;
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Set error(s)
Error Dam_Sc_System::set_error(const int err_type){
	string place="Dam_Sc_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_subcategories(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://not all files are set
			place.append("add_damage_points_file2database(QSqlDatabase *ptr_database)") ;
			reason="The required simpe counting point file is not set. The points are not transferd to database";
			help= "Set the file correctly";
			type=1;
			break;
		case 2://can not open file
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 3://not all data are found
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Not all data are found";
			help="Check the file";
			type=5;
			break;
		case 4://wrong input sequenze
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Wrong input by reading the number of subcategories";
			help="Check the file";
			type=5;
		case 5://not all info there
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Not all relevant subcategory data are found in file";
			help="Check the point data in file";
			type=5;
			break;
		case 6://can not read in the x-ycoordinates properly
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Can not read in the id and name of the subcategory properly";
			help="Check the coordinates and other data in file";
			type=5;
			break;
		case 7://not all points are found
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="Not all subcategories are found";
			help="Check the number of subcategories and the given subcategories";
			info << "Number of searched subcategories : " << this->number_subcategories << endl;
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
Warning Dam_Sc_System::set_warning(const int warn_type){
	string place="Dam_Sc_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://no !END-flag is found in file
			place.append("read_sc_subcategory_per_file(const string file)");
			reason="The subcategory data block is not closed with !END in file";
			help="Check the file and the number of subcategory specification in file";
			type=1;
			break;
		case 1://not all points are connected
			place.append("check_system(void)");
			reason="Not all points are connected to the hydraulic system";
			help="Reconnect the simple counting system";
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
}
