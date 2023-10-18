#include "Dam_Headers_Precompiled.h"
//#include "Dam_Eco_System.h"

//Default constructor
Dam_Eco_System::Dam_Eco_System(void){
	this->number_btype=0;
	this->number_raster_btype=0;
	this->b_types=NULL;
	this->raster_btypes=NULL;

	this->number_soil_ero_raster=0;
	this->number_soil_ero_function=0;
	this->number_soil_cost_function=0;
	this->soil_raster=NULL;
	this->soil_ero_function=NULL;
	this->soil_cost_function=NULL;

	this->file_b_type=label::not_set;
	this->file_raster_b_type=label::not_set;
	this->file_soil_cost=label::not_set;
	this->file_soil_erosion=label::not_set;
	this->file_raster_soil_cost=label::not_set;
	this->file_raster_soil_erosion=label::not_set;
	this->result_biotope_dam=0.0;
	this->result_soil_ero_dam=0.0;

	this->del_raster_flag=true;
	this->del_btype_flag=true;
	this->del_res_flag=true;
	this->del_soil_ero_flag=true;
	this->del_soil_cost_flag=true;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Eco_System), _sys_system_modules::DAM_SYS);
}
//Default constructor
Dam_Eco_System::~Dam_Eco_System(void){
	this->delete_btypes();
	this->delete_btypes_raster();
	this->delete_soil_cost_function();
	this->delete_soil_ero_function();
	this->delete_soil_ero_raster();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Eco_System), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Get the number of biotope-type raster
int Dam_Eco_System::get_number_biotope_raster(void){
	return this->number_raster_btype;
}
//Get the number of soil-type raster
int Dam_Eco_System::get_number_soil_raster(void){
	return this->number_soil_ero_raster;
}
//Import the biotope types from a file to the database
void Dam_Eco_System::import_biotope_types_file2database(QSqlDatabase *ptr_database){
/**Other already existing biotope-types in the database are deleted.
	This file should contain the information about the biotope-types including the damage-functions for
	the impact-values waterlevel, duration and flow velocity.
	\see Dam_Eco_Btype
*/
	try{
		this->set_start_warnings_number();
		//first delete all biotope-types
		Dam_Eco_Btype::delete_data_in_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the ecological biotope-types to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_b_type!=label::not_set){
			//read the data in
			this->read_biotope_types_per_file(this->file_b_type);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_btype<<" ecological biotope-types to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			int glob_id;
			glob_id=Dam_Eco_Btype::get_max_glob_id_biotope_table(ptr_database)+1;
			//make a table-model
			QSqlTableModel model(0,*ptr_database);
			for(int i=0; i< this->number_btype; i++){
				if(i%10==0 && i > 0){
					cout << "Transfer biotope-type "<< i <<" to database" << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
				}
				this->b_types[i].transfer_input_members2database(glob_id,&model,ptr_database);
				//counter global index
				glob_id++;
			}
		}
		else{
			cout <<"No file is set for the import"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Import of the ecological biotope-types is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add the biotope types from a file to the database (the existing one are not deleted)
void Dam_Eco_System::add_biotope_types_file2database(QSqlDatabase *ptr_database){
	try{
		this->set_start_warnings_number();

		ostringstream cout;
		cout <<"Add the data of the ecological biotope type(s) to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "ADD> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_b_type!=label::not_set){
			//read the data in
			this->read_biotope_types_per_file(this->file_b_type);
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_btype<<" ecological biotope-type(s) to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_btype; i++){
				this->b_types[i].add_input_members2database(ptr_database);
			}
		}
		else{
			cout <<"No file is set for the import"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Adding of the ecological biotope-type(s) is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a biotope type raster from file to database
void Dam_Eco_System::add_biotope_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used){
/**A ecological biotope-type raster consists of one raster: the raster includes the biotope-type
indices.
*/
	try{
		if(internaly_used==false){
			this->set_start_warnings_number();
		}
		ostringstream cout;
		cout <<"Add the data of the ecological biotope-type raster to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_raster_b_type!=label::not_set){
			//read the data in
			this->read_biotope_raster_per_file(1, &this->file_raster_b_type);
			Dam_Damage_System::check_stop_thread_flag();
			//compare with existing damage raster in database
			Dam_Eco_Btype_Raster *raster_buffer=NULL;
			QSqlTableModel results(0, *ptr_database);
			int number_buff_rast=Dam_Eco_Btype_Raster::select_relevant_raster_database(&results,ptr_database,false);
			if(number_buff_rast>0){
				raster_buffer=new Dam_Eco_Btype_Raster[number_buff_rast];
				_sys_system_id id_buff;
				id_buff.area_state=0;
				id_buff.measure_nr=0;
				//read them in
				for(int i=0; i< number_buff_rast; i++){
					raster_buffer[i].input_raster_perdatabase_general_data(&results, i);
				}
				//check them
				for(int i=0; i< this->number_raster_btype; i++){
					for(int j=0; j< number_buff_rast; j++){
						if(this->raster_btypes[i].compare_raster_infos(&raster_buffer[j])==true){
							Error msg=this->set_error(9);
							ostringstream info;
							info <<"Rastername in database: "<< raster_buffer[j].get_raster_name() << endl;
							msg.make_second_info(info.str());
							delete []raster_buffer;
							throw msg;
						}
					}
				}
				delete []raster_buffer;
			}
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			//transfer it to database
			for(int i=0; i< this->number_raster_btype; i++){
				try{
					this->raster_btypes[i].transfer_input_members2database(ptr_database);
				}
				catch(Error msg){
					Dam_Eco_Btype_Raster::delete_data_in_raster_table(ptr_database,this->raster_btypes[i].get_raster_number());
					Dam_Eco_Btype_Element::delete_data_in_elem_table(ptr_database, this->raster_btypes[i].get_raster_number());
					throw msg;
				}
			}
		}
		else{
			Error msg=this->set_error(11);
			ostringstream info;
			info << "Filename of biotope-type damage information : " << this->file_raster_b_type <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Adding of the ecological biotope-type raster is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(internaly_used==false){
		this->output_error_statistic();
	}
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a biotope type raster from file to database for multiple raster input
void Dam_Eco_System::add_biotope_raster_file2database_multi(QSqlDatabase *ptr_database){
	ostringstream prefix;
	this->set_start_warnings_number();
	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		prefix << "MULTI_"<<i+1<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		ostringstream cout;
		cout <<"Add the data of the ecological biotope-type raster "<<i+1<<"..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->file_raster_b_type=this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
		this->add_biotope_raster_file2database(ptr_database, true);
		cout <<"Data import of the ecological biotope-type raster "<<i+1<<" is finished..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_dam->rewind_userprefix();
		this->delete_btypes_raster();
	}
	//just for the statistic output
	this->number_raster_btype=this->number_raster_dia->get_number_raster();
	this->output_error_statistic();
}
//Delete all biotope-type data in the database: raster, raster elements, biotope-types, results
void Dam_Eco_System::del_biotope_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> B_TYPE> ";

	ostringstream cout;
	cout <<"Delete all ecological biotope-type damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	try{
		if(this->del_raster_flag==true){
			cout <<"ECO-damage biotope-type raster..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->delete_selected_biotope_raster_database(ptr_database);
		}
		if(this->del_btype_flag==true){
			cout <<"ECO-damage biotope-types..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Eco_Btype::delete_data_in_table(ptr_database);
		}
		if(this->del_res_flag==true || this->del_btype_flag==true){
			cout <<"ECO-damage biotope-type results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Eco_Btype_Element::delete_data_in_erg_table(ptr_database);
		}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of ecological biotope-type damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Ask for the file of the biotope types per dialog
bool Dam_Eco_System::ask_file_biotope_types(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_b_type */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":eco_btype_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the ecological biotope-types are stored."<< endl;
	buffer << "This file should contain the damage-functions for the calculation of the"<< endl;
	buffer << "ecological damages to a bitope-type." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-biotope type file");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECO-biotope type file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_b_type=my_dia.get_ptr_file_browser(0)->get_file_name();
		if(this->file_b_type==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the ECO-biotope type!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file of the biotope raster per dialog
bool Dam_Eco_System::ask_file_biotope_raster(QWidget *parent){
/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_raster_b_type
*/
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
		QIcon icon;
	icon.addFile(":eco_btype_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the raster of the ecological biotope-types is stored."<< endl;
	buffer << "This file should contain the geometrical raster of the biotope-types." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-raster of biotope-types file");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECO-raster of biotope-types file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_raster_b_type=my_dia.get_ptr_file_browser(0)->get_file_name();
		return true;
	}
}
//Ask for the file(s) of the biotope raster(s) per dialog for multiple raster input
bool Dam_Eco_System::ask_file_biotope_raster_multi(void){
	//ask the number of multiple raster
	this->allocate_number_raster_dia();
	QIcon icon;
	ostringstream txt;
	txt<<"Number of ECO biotope-type raster...";
	icon.addFile(":eco_btype_icon");
	this->number_raster_dia->change_window_title(txt.str(),icon);

	if(this->number_raster_dia->start_dialog()==false){
		return false;
	}
	if(this->number_raster_dia->get_number_raster()==0){
		return false;
	}
	//ask the multiple raster
	this->allocate_multi_raster_path_dia();

	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		this->mulit_raster_import_dia[i].set_number_file_browser(1,icon);
		stringstream buffer;
		buffer << "Choose the file, where the raster of the ecological biotope-types "<<endl;
		buffer << " (" << i+1 << " of " << this->number_raster_dia->get_number_raster() << ") are stored."<< endl;
		buffer << "This file should contain the geometrical raster of the biotope-types." << endl;
		this->mulit_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		this->mulit_raster_import_dia[i].set_window_title("Choose ECO-raster of biotope-types file");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("Choose ECO-raster of biotope-types file");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("ECO-raster of biotope-types file");

		//start dialog
		if(this->mulit_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	return true;
}
//Ask for the flag, what should be deleted (raster, biotope-type, results)
bool Dam_Eco_System::ask_deleting_biotope_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":eco_btype_icon");
	my_dia.set_number_check_boxes(3,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the ECO-system for biotope-types in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting ECO-system for biotope-types");
	my_dia.get_ptr_check_box(0)->setText("Delete ECO-biotope raster");
	my_dia.get_ptr_check_box(1)->setText("Delete ECO-biotope types");
	my_dia.get_ptr_check_box(2)->setText("Delete ECO-biotope damage results");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->del_raster_flag=my_dia.get_bool_check_box(0);
		this->del_btype_flag=my_dia.get_bool_check_box(1);
		this->del_res_flag=my_dia.get_bool_check_box(2);
		return true;
	}
}
//Ask for biotope raster to handled in a dialog
bool Dam_Eco_System::ask_biotope_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	DamGui_Raster_Dia my_dia(parent);

	this->read_biotope_raster_per_database_general(ptr_database,false);

	if(my_dia.start_dialog(this->raster_btypes, this->number_raster_btype)==true){
		return true;
	}
	else{
		return false;
	}
}
//Get a text for deleting biotope information in database
string Dam_Eco_System::get_biotope_deleting_text(void){
	ostringstream buffer;
	if(this->del_raster_flag==true){
		buffer << " The raster(s) will be deleted in database, also the results in the raster!"<<endl;
	}
	if(this->del_btype_flag==true){
		buffer << " All biotope types will be deleted in database, also all results!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//Read in the ecologic biotope-types from file
void Dam_Eco_System::read_biotope_types_per_file(const string file_biotype){
	ostringstream prefix;
	prefix << "ECO> B_TYPE> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	try{
		//read in biotope-type
		//open file
		ifile.open(file_biotype.c_str(), ios_base::in);
		if(ifile.is_open()==false){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Filename: " << file_biotype << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//search for number of damage function if file
		bool no_func_found=false;
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			functions::clean_string(&myline);

			pos=myline.rfind("!NO_BIO_TYPE");
			if(pos>=0){
				stringstream my_stream;
				myline=myline.substr(pos+12);
				functions::clean_string(&myline);
				my_stream << myline;
				my_stream >> this->number_btype;
				if(my_stream.fail()==true || this->number_btype<=0){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				no_func_found=true;
				break;
			}
			pos=myline.rfind("!BEGIN");
			if(pos>=0){
				no_func_found=false;
				break;
			}
			pos=myline.rfind("!END");
			if(pos>=0){
				no_func_found=false;
				break;
			}
		}
		while(ifile.eof()!=true);

		if(no_func_found==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file_biotype << endl;
			msg.make_second_info(info.str());
			ifile.close();
			throw msg;
		}

		//allocate the biotope-types
		try{
			this->allocate_btypes();
		}
		catch(Error msg){
			ifile.close();
			throw msg;
		}

		try{
			//read in the biotope-types
			for(int i=0; i< this->number_btype; i++){
				this->b_types[i].read_biotope_type_per_file(&ifile, &line_counter);
			}
			this->check_biotope_type();
		}
		catch(Error msg){
			ostringstream info;
			info << "Filename              : "<< file_biotype << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		//close file and reset it
		ifile.close();
		ifile.clear();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();

		throw msg;
	}

	//close file and reset it
	ifile.close();
	ifile.clear();

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecologic biotope-types from database
void Dam_Eco_System::read_biotope_types_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> B_TYPE> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the biotope-type per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->number_btype=Dam_Eco_Btype::select_relevant_biotope_types_database(&results);
		this->allocate_btypes();
		for(int i=0; i< this->number_btype; i++){
			this->b_types[i].input_biotope_type_perdatabase(&results, i);
		}
		this->check_biotope_type();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecologic biotope raster from file
void Dam_Eco_System::read_biotope_raster_per_file(const int no, string *fname){
	ostringstream prefix;
	prefix << "ECO> B_TYPE> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	try{
		this->number_raster_btype=no;
		this->allocate_btypes_raster();
		for(int i=0; i< this->number_raster_btype; i++){
			this->raster_btypes[i].set_raster_number(i);
			this->raster_btypes[i].read_btype_raster_per_file(fname[i]);
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the general information of the ecologic biotope raster from database
void Dam_Eco_System::read_biotope_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "ECO> B_TYPE> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the general biotope-type raster information per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		QSqlQueryModel results;
		//number of function
		this->number_raster_btype=Dam_Eco_Btype_Raster::select_relevant_raster_database(&results, ptr_database);
		this->allocate_btypes_raster();
		for(int i=0; i< this->number_raster_btype; i++){
			this->raster_btypes[i].input_raster_perdatabase_general_data(&results, i);
		}
		this->check_b_type_raster();
	}
	catch(Error msg){
		if(with_output==true){
			Sys_Common_Output::output_dam->rewind_userprefix();
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}
	if(with_output==true){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
	}
}
//Get the flag if the biotope raster are to delete
bool Dam_Eco_System::get_biotope_raster2delete(void){
	return this->del_raster_flag;
}
//Import the ecological soil types and their cost function from a file to the database
void Dam_Eco_System::import_soil_type_cost_function_file2database(QSqlDatabase *ptr_database){
/**Other already existing soil-cost-functions in the database are deleted.
	This file should contain the information about the soil-types and their costs depending to the erosion grade.
	\see Dam_Eco_Soil_Cost_Function
*/
	try{
		this->set_start_warnings_number();
		//first delete all biotope-types
		Dam_Eco_Soil_Cost_Function::delete_data_in_function_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the ecological soil-type cost-function to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_soil_cost!=label::not_set){
			//read the data in
			this->read_soil_type_cost_function(this->file_soil_cost);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_soil_cost_function<<" ecological soil-cost function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_soil_cost_function; i++){
				this->soil_cost_function[i].transfer_input_members2database(ptr_database);
			}
		}
		else{
			cout <<"No file is set for the import"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Import of the ecological soil-type cost-function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add the ecological soil types and their cost function from a file to the database (the existing one are not deleted)
void Dam_Eco_System::add_soil_type_cost_function_file2database(QSqlDatabase *ptr_database){
	try{
		this->set_start_warnings_number();

		ostringstream cout;
		cout <<"Add the data of the ecological soil type(s) to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "ADD> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_soil_cost!=label::not_set){
			//read the data in
			this->read_soil_type_cost_function(this->file_soil_cost);
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_soil_cost_function<<" ecological soil type(s) to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_soil_cost_function; i++){
				this->soil_cost_function[i].add_input_members2database(ptr_database);
			}
		}
		else{
			cout <<"No file is set for the import"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Adding of the ecological soil type(s) is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Import ecological soil erosion types and their function from a file to the database
void Dam_Eco_System::import_soil_type_erosion_function_file2database(QSqlDatabase *ptr_database){
/**Other already existing soil-erosion-functions in the database are deleted.
	This file should contain the information about the erosion-types and their erosion grades depending to the
	flow velocity as the impact value.
\see Dam_Eco_Soil_Erosion_Function
*/
	try{
		this->set_start_warnings_number();
		//first delete all biotope-types
		Dam_Eco_Soil_Erosion_Function::delete_data_in_function_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the ecological soil-erosion-function to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_soil_erosion!=label::not_set){
			//read the data in
			this->read_soil_type_erosion_function(this->file_soil_erosion);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_soil_ero_function<<" ecological soil-erosion function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_soil_ero_function; i++){
				this->soil_ero_function[i].transfer_input_members2database(ptr_database);
			}
		}
		else{
			cout <<"No file is set for the import"<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Import of the ecological soil-erosion-function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a ecological soil erosion raster from file to database
void Dam_Eco_System::add_soil_type_erosion_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used){
/**A ecological soil-erosion raster consists of two raster:
	- the first raster consists of the soil-types, which is connected to the soil-cost-functions.
	- the second raster includes the erosion-types, which are connected to soil-erosion-functions.

Both rasters are required.
*/
	try{
		if(internaly_used==false){
			this->set_start_warnings_number();
		}
		ostringstream cout;
		cout <<"Add the data of the ecological soil-erosion raster to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_raster_soil_cost!=label::not_set && this->file_raster_soil_erosion!=label::not_set ){
			//read the data in
			this->read_soil_type_erosion_rasters(1, &this->file_raster_soil_cost, &this->file_raster_soil_erosion);
			Dam_Damage_System::check_stop_thread_flag();
			//compare with existing damage raster in database
			Dam_Eco_Soil_Raster *raster_buffer=NULL;
			QSqlTableModel results(0, *ptr_database);
			int number_buff_rast=Dam_Eco_Soil_Raster::select_relevant_raster_database(&results,ptr_database,false);
			if(number_buff_rast>0){
				raster_buffer=new Dam_Eco_Soil_Raster[number_buff_rast];
				_sys_system_id id_buff;
				id_buff.area_state=0;
				id_buff.measure_nr=0;
				//read them in
				for(int i=0; i< number_buff_rast; i++){
					raster_buffer[i].input_raster_perdatabase_general_data(&results, i);
				}
				//check them
				for(int i=0; i< this->number_soil_ero_raster; i++){
					for(int j=0; j< number_buff_rast; j++){
						if(this->soil_raster[i].compare_raster_infos(&raster_buffer[j])==true){
							Error msg=this->set_error(10);
							ostringstream info;
							info <<"Rastername in database: "<< raster_buffer[j].get_raster_name() << endl;
							msg.make_second_info(info.str());
							delete []raster_buffer;
							throw msg;
						}
					}
				}
				delete []raster_buffer;
			}
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			//transfer it to database
			for(int i=0; i< this->number_soil_ero_raster; i++){
				try{
					this->soil_raster[i].transfer_input_members2database(ptr_database);
				}
					catch(Error msg){
						Dam_Eco_Soil_Raster::delete_data_in_raster_table(ptr_database,this->soil_raster[i].get_raster_number());
						Dam_Eco_Soil_Element::delete_data_in_elem_table(ptr_database, this->soil_raster[i].get_raster_number());
						throw msg;
					}
			}
		}
		else{
			Error msg=this->set_error(12);
			ostringstream info;
			info << "Filename of soil cost information    : " << this->file_raster_soil_cost <<endl;
			info << "Filename of soil erosion information : " << this->file_raster_soil_erosion <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		if(msg.get_user_aborted_exception()==false){
			this->number_error++;
		}
		msg.output_msg(4);
	}
	this->set_warning_number();
	ostringstream cout;
	cout <<"Data-Adding of the ecological soil-erosion raster is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(internaly_used==false){
		this->output_error_statistic();
	}
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a ecological soil erosion raster from file to database for multiple raster input
void Dam_Eco_System::add_soil_type_erosion_raster_file2database_multi(QSqlDatabase *ptr_database){
	ostringstream prefix;
	this->set_start_warnings_number();
	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		prefix << "MULTI_"<<i+1<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		ostringstream cout;
		cout <<"Add the data of the ecological soil-erosion raster "<<i+1<<"..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->file_raster_soil_cost=this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
		this->file_raster_soil_erosion=this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();

		this->add_soil_type_erosion_raster_file2database(ptr_database, true);
		cout <<"Data import of the ecological soil-erosion raster "<<i+1<<" is finished..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_dam->rewind_userprefix();
		this->delete_soil_ero_raster();
	}

	//just for the statistic output
	this->number_soil_ero_raster=this->number_raster_dia->get_number_raster();
	this->output_error_statistic();
}
//Delete all soil-erosion data in the database: raster, raster elements, cost-function, erosion-function, results
void Dam_Eco_System::del_soil_erosion_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";

	ostringstream cout;
	cout <<"Delete all ecological soil-erosion damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	try{
		if(this->del_raster_flag==true){
			cout <<"ECO-damage soil raster..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->delete_selected_soil_erosion_raster_database(ptr_database);
		}
		if(this->del_soil_cost_flag==true){
			cout <<"ECO-damage soil-cost function..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Eco_Soil_Cost_Function::delete_data_in_function_table(ptr_database);
		}
		if(this->del_soil_ero_flag==true){
			cout <<"ECO-damage soil-erosion function..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Eco_Soil_Erosion_Function::delete_data_in_function_table(ptr_database);
		}

		if(this->del_res_flag==true || this->del_soil_ero_flag==true || this->del_soil_cost_flag==true){
			cout <<"ECO-damage soil-erosion results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Eco_Soil_Element::delete_data_in_erg_table(ptr_database);
		}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of ecological soil-erosion damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Ask for the file of the ecological soil types and their cost function per dialog
bool Dam_Eco_System::ask_file_soil_type_cost_function(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_soil_cost */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":eco_soil_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the ecological soil-cost function(s) are stored."<< endl;
	buffer << "This file should contain the cost-functions of different soil-types for the calculation of the"<< endl;
	buffer << "ecological damages due to soil erosion." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-soil-cost function file");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECO-soil-cost function file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_soil_cost=my_dia.get_ptr_file_browser(0)->get_file_name();
		if(this->file_soil_cost==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the ECO-soil-cost function!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file of the ecological soil erosion types and their function per dialog
bool Dam_Eco_System::ask_file_soil_type_erosion_function(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_soil_erosion */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":eco_soil_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the ecological soil-erosion function(s) are stored."<< endl;
	buffer << "This file should contain the erosion-functions of different erosion categories for the calculation of the"<< endl;
	buffer << "ecological damages due to soil erosion." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-soil-erosion function file");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECO-soil-erosion function file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_soil_erosion=my_dia.get_ptr_file_browser(0)->get_file_name();
		if(this->file_soil_erosion==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the ECO-soil-erosion function!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file of the ecological soil erosion raster per dialog
bool Dam_Eco_System::ask_file_soil_type_erosion_raster(QWidget *parent){
	/**	If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The results are set to  the members
	file_raster_soil_cost , file_raster_soil_erosion .*/
	Sys_Multi_Filechooser_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":eco_soil_icon");
	my_dia.set_number_file_browser(2,icon);
	stringstream buffer;
	buffer << "Choose the file(s), where the ecologic soil-erosion-rasters are stored. Both rasters are required."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-raster of soil-erosion file(s)");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECO-raster file for the soil-cost-types");
	my_dia.get_ptr_file_browser(0)->set_tooltip("The soil-types are connected to the soil-cost functions");

	my_dia.get_ptr_file_browser(1)->set_text_label("ECO-raster file for the soil-erosion-types");
	my_dia.get_ptr_file_browser(1)->set_tooltip("The soil-types are connected to the soil-erosion functions");
	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_raster_soil_cost=my_dia.get_ptr_file_browser(0)->get_file_name();
		this->file_raster_soil_erosion=my_dia.get_ptr_file_browser(1)->get_file_name();
		return true;
	}
}
//Ask for the file(s) of the ecological soil erosion raster(s) per dialog for multiple raster input
bool Dam_Eco_System::ask_file_soil_type_erosion_raster_multi(void){
//ask the number of multiple raster
	this->allocate_number_raster_dia();
	QIcon icon;
	ostringstream txt;
	txt<<"Number of ECO soil-erosion raster...";
	icon.addFile(":eco_soil_icon");
	this->number_raster_dia->change_window_title(txt.str(),icon);

	if(this->number_raster_dia->start_dialog()==false){
		return false;
	}
	if(this->number_raster_dia->get_number_raster()==0){
		return false;
	}
	//ask the multiple raster
	this->allocate_multi_raster_path_dia();

	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		this->mulit_raster_import_dia[i].set_number_file_browser(2,icon);
		stringstream buffer;
		buffer << "Choose the file(s), where the ecologic soil-erosion-rasters "<<endl;
		buffer << " (" << i+1 << " of " << this->number_raster_dia->get_number_raster() << ") are stored."<< endl;
		buffer << "Both rasters are required." << endl;
		this->mulit_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		this->mulit_raster_import_dia[i].set_window_title("Choose ECO-raster of soil-erosion file(s)");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("ECO-raster file for the soil-cost-types");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("The soil-types are connected to the soil-cost functions");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_text_label("ECO-raster file for the soil-erosion-types");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_tooltip("The soil-types are connected to the soil-erosion functions");

		//start dialog
		if(this->mulit_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	return true;
}
//Ask for the flag, what should be deleted (raster, cost-function, erosion function, results)
bool Dam_Eco_System::ask_deleting_soil_erosion_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":eco_soil_icon");
	my_dia.set_number_check_boxes(4,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the ECO-system for soil-erosion in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECO-soil-erosion type file");
	my_dia.get_ptr_check_box(0)->setText("Delete ECO-soil-erosion raster");
	my_dia.get_ptr_check_box(1)->setText("Delete ECO soil-cost function");
	my_dia.get_ptr_check_box(2)->setText("Delete ECO soil-erosion function");
	my_dia.get_ptr_check_box(3)->setText("Delete ECO-soil-erosion damage results");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->del_raster_flag=my_dia.get_bool_check_box(0);
		this->del_soil_cost_flag=my_dia.get_bool_check_box(1);
		this->del_soil_ero_flag=my_dia.get_bool_check_box(2);
		this->del_res_flag=my_dia.get_bool_check_box(3);
		return true;
	}
}
//Ask for soil erosion raster to handled in a dialog
bool Dam_Eco_System::ask_soil_erosion_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	DamGui_Raster_Dia my_dia(parent);

	this->read_soil_type_erosion_raster_per_database_general(ptr_database,false);

	if(my_dia.start_dialog(this->soil_raster, this->number_soil_ero_raster)==true){
		return true;
	}
	else{
		return false;
	}
}
//Get a text for deleting soil erosion information in database
string Dam_Eco_System::get_soil_erosion_deleting_text(void){
	ostringstream buffer;
	if(this->del_raster_flag==true){
		buffer << " The raster(s) will be deleted in database, also the results in the raster!"<<endl;
	}
	if(this->del_soil_cost_flag==true){
		buffer << " All soil cost function(s) will be deleted in database, also all results!"<<endl;
	}
	if(this->del_soil_ero_flag==true){
		buffer << " All soil erosion function(s) will be deleted in database, also all results!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//Read in the ecological soil types and their cost function from file
void Dam_Eco_System::read_soil_type_cost_function(const string file){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	try{
		//read in biotope-type
		//open file
		ifile.open(file.c_str(), ios_base::in);
		if(ifile.is_open()==false){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Filename: " << file << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//search for number of damage function if file
		bool no_func_found=false;
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			functions::clean_string(&myline);

			pos=myline.rfind("!NO_FUNCTIONS");
			if(pos>=0){
				stringstream my_stream;
				myline=myline.substr(pos+13);
				functions::clean_string(&myline);
				my_stream << myline;
				my_stream >> this->number_soil_cost_function;
				if(my_stream.fail()==true || this->number_soil_cost_function<=0){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				no_func_found=true;
				break;
			}
			pos=myline.rfind("!BEGIN");
			if(pos>=0){
				no_func_found=false;
				break;
			}
			pos=myline.rfind("!END");
			if(pos>=0){
				no_func_found=false;
				break;
			}
		}
		while(ifile.eof()!=true);

		if(no_func_found==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file << endl;
			msg.make_second_info(info.str());
			ifile.close();
			throw msg;
		}

		//allocate the soil-cost-functions
		try{
			this->allocate_soil_cost_function();
		}
		catch(Error msg){
			ifile.close();
			throw msg;
		}

		try{
			//read in the biotope-types
			for(int i=0; i< this->number_soil_cost_function; i++){
				this->soil_cost_function[i].read_function(&ifile, &line_counter);
			}
			this->check_soil_cost_function();
		}
		catch(Error msg){
			ostringstream info;
			info << "Filename              : "<< file << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		//close file and reset it
		ifile.close();
		ifile.clear();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecological soil-cost types and their cost function from database
void Dam_Eco_System::read_soil_type_cost_function_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the soil-cost function per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->number_soil_cost_function=Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&results);
		this->allocate_soil_cost_function();
		for(int i=0; i< this->number_soil_cost_function; i++){
			this->soil_cost_function[i].input_function_perdatabase(&results, i);
		}
		this->check_soil_cost_function();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecological soil erosion types and their function from file
void Dam_Eco_System::read_soil_type_erosion_function(const string file){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	try{
		//read in biotope-type
		//open file
		ifile.open(file.c_str(), ios_base::in);
		if(ifile.is_open()==false){
			Error msg=this->set_error(1);
			ostringstream info;
			info << "Filename: " << file << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//search for number of damage function if file
		bool no_func_found=false;
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			functions::clean_string(&myline);

			pos=myline.rfind("!NO_FUNCTIONS");
			if(pos>=0){
				stringstream my_stream;
				myline=myline.substr(pos+13);
				functions::clean_string(&myline);
				my_stream << myline;
				my_stream >> this->number_soil_ero_function;
				if(my_stream.fail()==true || this->number_soil_ero_function<=0){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				no_func_found=true;
				break;
			}
			pos=myline.rfind("!BEGIN");
			if(pos>=0){
				no_func_found=false;
				break;
			}
			pos=myline.rfind("!END");
			if(pos>=0){
				no_func_found=false;
				break;
			}
		}
		while(ifile.eof()!=true);

		if(no_func_found==false){
			Error msg=this->set_error(2);
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file << endl;
			msg.make_second_info(info.str());
			ifile.close();
			throw msg;
		}

		//allocate the soil-cost-functions
		try{
			this->allocate_soil_ero_function();
		}
		catch(Error msg){
			ifile.close();
			throw msg;
		}

		try{
			//read in the biotope-types
			for(int i=0; i< this->number_soil_ero_function; i++){
				this->soil_ero_function[i].read_function(&ifile, &line_counter);
			}
			this->check_soil_erosion_function();
		}
		catch(Error msg){
			ostringstream info;
			info << "Filename              : "<< file << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		//close file and reset it
		ifile.close();
		ifile.clear();
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecological soil-erosion types and their function from database
void Dam_Eco_System::read_soil_type_erosion_function_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the soil-erosion function per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->number_soil_ero_function=Dam_Eco_Soil_Erosion_Function::select_relevant_functions_database(&results);
		this->allocate_soil_ero_function();
		for(int i=0; i< this->number_soil_ero_function; i++){
			this->soil_ero_function[i].input_function_perdatabase(&results, i);
		}
		this->check_soil_erosion_function();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the ecological soil erosion raster from file
void Dam_Eco_System::read_soil_type_erosion_rasters(const int no, string *fname_type, string *fname_erosion){
	ostringstream prefix;
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	try{
		//soil-type raster
		this->number_soil_ero_raster=no;
		this->allocate_soil_ero_raster();

		for(int i=0; i< this->number_soil_ero_raster;i++){
			this->soil_raster[i].read_all_soil_erosion_raster_per_file( fname_erosion[i], fname_type[i]);
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the general information of the ecological soil-erosion raster from database
void Dam_Eco_System::read_soil_type_erosion_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "ECO> SOIL> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the general soil-erosion raster information per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		QSqlQueryModel results;
		//number of function
		this->number_soil_ero_raster=Dam_Eco_Soil_Raster::select_relevant_raster_database(&results, ptr_database);
		this->allocate_soil_ero_raster();
		for(int i=0; i< this->number_soil_ero_raster; i++){
			this->soil_raster[i].input_raster_perdatabase_general_data(&results, i);
		}
		this->check_soil_erosion_raster();
	}
	catch(Error msg){
		if(with_output==true){
			Sys_Common_Output::output_dam->rewind_userprefix();
			Sys_Common_Output::output_dam->rewind_userprefix();
		}
		throw msg;
	}
	if(with_output==true){
		Sys_Common_Output::output_dam->rewind_userprefix();
		Sys_Common_Output::output_dam->rewind_userprefix();
	}
}
//Read inf the ecological system per database
void Dam_Eco_System::read_system_per_database(QSqlDatabase *ptr_database){
	//biotope-types
	this->read_biotope_raster_per_database_general(ptr_database);
	this->read_biotope_types_per_database(ptr_database);
	//soil-erosion
	this->read_soil_type_erosion_raster_per_database_general(ptr_database);
	this->read_soil_type_cost_function_per_database(ptr_database);
	this->read_soil_type_erosion_function_per_database(ptr_database);
}
//Calculate the damages for the soil-erosion types
void Dam_Eco_System::calculate_soil_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id){
	ostringstream cout;
	cout << "Calculate damages for ecological soil-erosion raster " <<this->soil_raster[raster_id].get_raster_name()<<"..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->soil_raster[raster_id].calculate_damages(impact_fp, number);
}
//Calculate the damages for the biotope types
void Dam_Eco_System::calculate_biotope_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id){
	ostringstream cout;
	cout << "Calculate damages for ecological biotope-type raster " <<this->raster_btypes[raster_id].get_raster_name() <<"..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster_btypes[raster_id].calculate_damages(impact_fp, number);
}
//Delete the soil-erosion result members for a given system-id and a scenario (boundary-, break-)
void Dam_Eco_System::delete_soil_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_Eco_Soil_Element::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Delete the biotope result members for a given system-id and a scenario (boundary-, break-)
void Dam_Eco_System::delete_biotope_result_members_in_database(QSqlDatabase *ptr_database,const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_Eco_Btype_Element::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the soil-erosion result members to a database table
void Dam_Eco_System::output_soil_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(raster_id==this->number_soil_ero_raster-1 && *was_output==false){
		must_output=true;
	}
	cout << "Output ecological soil-erosion damages to database of " << this->soil_raster[raster_id].get_raster_name() <<"..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->soil_raster[raster_id].output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Output the biotope result members to a database table
void Dam_Eco_System::output_biotope_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(raster_id==this->number_raster_btype-1 && *was_output==false){
		must_output=true;
	}
	cout << "Output ecological biotope-type damages to database of " << this->raster_btypes[raster_id].get_raster_name() <<"..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster_btypes[raster_id].output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Get the flag if the soil erosion raster are to delete
bool Dam_Eco_System::get_soil_erosion_raster2delete(void){
	return this->del_raster_flag;
}
//Output the members
void Dam_Eco_System::output_members(void){
	ostringstream prefix;
	prefix << "ECO> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	//biotope-types
	prefix.str("");
	prefix << "B_TYP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->number_raster_btype>0){
		cout << "Output "<<this->number_raster_btype<<" raster(s) of biotope-types..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_raster_btype; i++){
			this->raster_btypes[i].output_member();
		}
	}

	if(this->number_btype>0){
		cout << "Output " << this->number_btype << " biotope-type(s)..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_btype; i++){
			this->b_types[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();

	//soil
	prefix.str("");
	prefix << "SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	if(this->number_soil_ero_raster>0){
		cout << "Output "<<this->number_soil_ero_raster<<" raster(s) of soil-erosion..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_soil_ero_raster; i++){
			this->soil_raster[i].output_member();
		}
	}

	if(this->number_soil_ero_function>0){
		cout << "Output "<<this->number_soil_ero_function<<" of soil-erosion function(s)..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_soil_ero_function; i++){
			this->soil_ero_function[i].output_member();
		}
	}

	if(this->number_soil_cost_function>0){
		cout << "Output "<<this->number_soil_cost_function<<" of soil-cost function(s)..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_soil_cost_function; i++){
			this->soil_cost_function[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of ecological damage system
void Dam_Eco_System::output_statistic(void){
	ostringstream prefix;
	prefix << "EC0> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	//biotope-types
	prefix.str("");
	prefix << "B_TYP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout<<"BIOTOPE-TYPE RASTER"<<endl;
	cout << " Number             " << this->number_raster_btype << endl;
	cout<<"BIOTOPE-TYPE"<<endl;
	cout << " Number             " << this->number_btype << endl;

	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();

	//biotope-types
	prefix.str("");
	prefix << "SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	cout<<"SOIL-EROSION RASTER"<<endl;
	cout << " Number                  " << this->number_soil_ero_raster << endl;
	cout<<"SOILEROSION-COST-TYPE"<<endl;
	cout << " Number cost-function    " << this->number_soil_cost_function << endl;
	cout << " Number erosion-function " << this->number_soil_ero_function << endl;

	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the sum over the raster of the damage results to display/console
void Dam_Eco_System::output_result_damage(void){
	ostringstream prefix;

	prefix << "ECO> B_TYPE> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	for(int i=0; i< this->number_raster_btype; i++){
		this->raster_btypes[i].output_result_damage();
	}

	ostringstream cout;
	cout << "TOTAL RESULT"<<endl;
	cout << " Biotope-type  : " << P(2)<< FORMAT_FIXED_REAL << this->result_biotope_dam << label::euro << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();

	prefix.str("");
	prefix << "ECO> SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	for(int i=0; i< this->number_soil_ero_raster; i++){
		this->soil_raster[i].output_result_damage();
	}

	cout << "TOTAL RESULT"<<endl;
	cout << " Soil-erosion  : " << P(2)<< FORMAT_FIXED_REAL << this->result_soil_ero_dam << label::euro << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the results of the biotope-tape damages
double Dam_Eco_System::get_biotope_type_damages(void){
	return this->result_biotope_dam;
}
//Get the results of the biotope-tape damages
double Dam_Eco_System::get_soil_erosion_damages(void){
	return this->result_soil_ero_dam;
}
//Initialize the soil damage raster(s)
void Dam_Eco_System::init_soil_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	this->soil_raster[index].input_raster_perdatabase_element_data(ptr_database, id);
	this->connect_soil_elems2functions(index);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Initialize the biotope damage raster(s)
void Dam_Eco_System::init_biotope_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	this->raster_btypes[index].input_raster_perdatabase_element_data(ptr_database, id);
	this->connect_biotope_elems2functions(index);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Reduce area of intercepted elements
void Dam_Eco_System::reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "ECO> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	try{
		cout <<"Check the interception of the biotope-type rasters..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		try{
			prefix << "B_TYPE> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int i=0; i< this->number_raster_btype-1; i++){
				cout <<"Intercept biotope-type raster "<<this->raster_btypes[i].get_raster_name()<< " with the other raster(s).." <<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				//check if there is an interception
				bool intercept_flag=false;
				for(int j=0; j< this->number_raster_btype; j++){
					if(j>i){
						intercept_flag=this->raster_btypes[i].check_interception(&this->raster_btypes[j]);
						if(intercept_flag==true){
							break;
						}
					}
				}

				if(intercept_flag==true){
					this->raster_btypes[i].input_raster_perdatabase_element_data(ptr_database, id);
					prefix <<"RAST_"<<this->raster_btypes[i].get_raster_number()<<"> ";
					Sys_Common_Output::output_dam->set_userprefix(&prefix);
					for(int j=0; j< this->number_raster_btype; j++){
						Dam_Damage_System::check_stop_thread_flag();
						if(j>i){
							this->raster_btypes[i].set_intercepted_elem2reduced_area(&this->raster_btypes[j], ptr_database, id);
						}
					}

					this->raster_btypes[i].transfer_intercepted_elem_data2database(ptr_database);
					Sys_Common_Output::output_dam->rewind_userprefix();
					this->raster_btypes[i].clear_raster();
				}
			}
		}
		catch(Error msg){
			Sys_Common_Output::output_dam->rewind_userprefix();
			throw msg;
		}
		Sys_Common_Output::output_dam->rewind_userprefix();

		cout <<"Check the interception of the soil-type rasters..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		try{
			prefix << "SOIL> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			for(int i=0; i< this->number_soil_ero_raster-1; i++){
				cout <<"Intercept soil-erosion raster "<<this->soil_raster[i].get_raster_name()<< " with the other raster(s).." <<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				//check if there is an interception
				bool intercept_flag=false;
				for(int j=0; j< this->number_soil_ero_raster; j++){
					if(j>i){
						intercept_flag=this->soil_raster[i].check_interception(&this->soil_raster[j]);
						if(intercept_flag==true){
							break;
						}
					}
				}

				if(intercept_flag==true){
					this->soil_raster[i].input_raster_perdatabase_element_data(ptr_database, id);
					prefix <<"RAST_"<<this->soil_raster[i].get_raster_number()<<"> ";
					Sys_Common_Output::output_dam->set_userprefix(&prefix);

					for(int j=0; j< this->number_soil_ero_raster; j++){
						Dam_Damage_System::check_stop_thread_flag();
						if(j>i){
							this->soil_raster[i].set_intercepted_elem2reduced_area(&this->soil_raster[j], ptr_database, id);
						}
					}

					this->soil_raster[i].transfer_intercepted_elem_data2database(ptr_database);
					Sys_Common_Output::output_dam->rewind_userprefix();
					this->soil_raster[i].clear_raster();
				}
			}
		}
		catch(Error msg){
			Sys_Common_Output::output_dam->rewind_userprefix();
			throw msg;
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Intercept damage elements with the hydraulic elements
void Dam_Eco_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "ECO> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	int *rang=NULL;
	cout <<"Sort hydraulic floodplain models..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);

	//biotope-types
	prefix << "B_TYP> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	try{
		cout <<"Check the interception of "<<this->number_raster_btype<<" biotope-type raster(s) to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_raster_btype; i++){
			this->raster_btypes[i].input_raster_perdatabase_element_data(ptr_database, id);
			prefix <<"RAST_"<<this->raster_btypes[i].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int j=0; j< number_fp; j++){
				cout <<"Check the interception of biotope-type raster "<<this->raster_btypes[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);

				prefix <<"FP_"<<fp_models[rang[j]].Param_FP.get_floodplain_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				if(this->raster_btypes[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::complete_outside ||
					this->raster_btypes[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::outside_with_boundary){
					Sys_Common_Output::output_dam->rewind_userprefix();
					cout <<"No interception..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					continue;
				}

				bool delete_again=false;
				if(fp_models[rang[j]].floodplain_elems==NULL || fp_models[rang[j]].raster.get_ptr_raster_points()==NULL || fp_models[rang[j]].raster.get_ptr_raster_segments()==NULL){
					delete_again=true;
					QSqlQueryModel bound_result;
					int number_bound=0;
					cout <<"Generate elements, raster points and segments for the interception of biotope-type raster "<<this->raster_btypes[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].input_elems_database(ptr_database, &bound_result,number_bound, false, true);
				}

				this->raster_btypes[i].set_intercepted_elem2floodplain_id(&fp_models[rang[j]]);
				if(delete_again==true){
					cout <<"Delete elements, raster points and segments of the hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].delete_elems_raster_geo();
				}
				Dam_Damage_System::check_stop_thread_flag();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
			this->raster_btypes[i].transfer_intercepted_elem_data2database(ptr_database);
			this->raster_btypes[i].clear_raster();
			Sys_Common_Output::output_dam->rewind_userprefix();
		}

		Sys_Common_Output::output_dam->rewind_userprefix();

		//soil

		prefix << "SOIL> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		cout <<"Check the interception of "<<this->number_soil_ero_raster<<" soil-erosion raster(s) to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_soil_ero_raster; i++){
			this->soil_raster[i].input_raster_perdatabase_element_data(ptr_database, id);

			prefix <<"RAST_"<<this->soil_raster[i].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int j=0; j< number_fp; j++){
				cout <<"Check the interception of soil-erosion raster "<<this->soil_raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);

				prefix <<"FP_"<<fp_models[rang[j]].Param_FP.get_floodplain_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);
				if(this->soil_raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::complete_outside ||
					this->soil_raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::outside_with_boundary){
					Sys_Common_Output::output_dam->rewind_userprefix();
					cout <<"No interception..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					continue;
				}

				bool delete_again=false;
				if(fp_models[rang[j]].floodplain_elems==NULL || fp_models[rang[j]].raster.get_ptr_raster_points()==NULL || fp_models[rang[j]].raster.get_ptr_raster_segments()==NULL){
					delete_again=true;
					QSqlQueryModel bound_result;
					int number_bound=0;
					cout <<"Generate elements, raster points and segments for the interception of soil-erosion raster "<<this->soil_raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].input_elems_database(ptr_database, &bound_result,number_bound, false, true);
				}

				this->soil_raster[i].set_intercepted_elem2floodplain_id(&fp_models[rang[j]]);

				if(delete_again==true){
					cout <<"Delete elements, raster points and segments of the hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].delete_elems_raster_geo();
				}
				Dam_Damage_System::check_stop_thread_flag();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
			this->soil_raster[i].transfer_intercepted_elem_data2database(ptr_database);
			this->soil_raster[i].clear_raster();
			Sys_Common_Output::output_dam->rewind_userprefix();
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
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
void Dam_Eco_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECO> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	//biotope-types
	prefix.str("");
	prefix << "B_TYP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Transfer the data from the interception of "<<this->number_raster_btype<<" biotope-type raster(s) to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->number_raster_btype; i++){
		this->raster_btypes[i].transfer_intercepted_elem_data2database(ptr_database);
	}

	Sys_Common_Output::output_dam->rewind_userprefix();

	//soil
	prefix.str("");
	prefix << "SOIL> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout <<"Transfer the data from the interception of "<<this->number_soil_ero_raster<<" soil-erosion raster(s) to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->number_soil_ero_raster; i++){
		this->soil_raster[i].transfer_intercepted_elem_data2database(ptr_database);
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_Eco_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	this->result_biotope_dam=0.0;
	this->result_soil_ero_dam=0.0;

	//btype
	for(int i=0; i< this->number_raster_btype; i++){
		this->raster_btypes[i].sum_total_results(ptr_database, id, bound_sz, break_sz);
		//sum it up
		this->result_biotope_dam=this->result_biotope_dam+this->raster_btypes[i].get_biotope_damage_results();
	}
	//soil
	for(int i=0; i< this->number_soil_ero_raster; i++){
		this->soil_raster[i].sum_total_results(ptr_database, id, bound_sz, break_sz);
		//sum it up
		this->result_soil_ero_dam=this->result_soil_ero_dam+this->soil_raster[i].get_soil_ero_damage_results();
	}
}
//Check the ecologic damage system
void Dam_Eco_System::check_system(void){
	if(this->number_btype==0 && this->number_raster_btype!=0){
		Warning msg=this->set_warning(0);
		msg.output_msg(4);
	}
	if(this->number_btype!=0 && this->number_raster_btype==0){
		Warning msg=this->set_warning(1);
		msg.output_msg(4);
	}
	if(this->number_raster_btype!=0){
		bool flag=true;
		for(int i=0; i<this->number_raster_btype; i++){
			//if raster are connected
			if(this->raster_btypes[i].get_connected_flag()==false){
				flag=false;
				break;
			}
		}
		if(flag==false){
			Warning msg=this->set_warning(2);
			msg.output_msg(4);
		}
	}

	if(this->number_soil_cost_function==0 && this->number_soil_ero_raster!=0){
		Warning msg=this->set_warning(4);
		msg.output_msg(4);
	}
	if(this->number_soil_ero_function==0 && this->number_soil_ero_raster!=0){
		Warning msg=this->set_warning(5);
		msg.output_msg(5);
	}
	if(this->number_soil_cost_function!=0 && this->number_soil_ero_raster==0){
		Warning msg=this->set_warning(6);
		msg.output_msg(4);
	}
	if(this->number_soil_ero_function!=0 && this->number_soil_ero_raster==0){
		Warning msg=this->set_warning(7);
		msg.output_msg(4);
	}

	if(this->number_soil_ero_raster!=0){
		bool flag=true;
		for(int i=0; i<this->number_soil_ero_raster; i++){
			//if raster are connected
			if(this->soil_raster[i].get_connected_flag()==false){
				flag=false;
				break;
			}
		}
		if(flag==false){
			Warning msg=this->set_warning(3);
			msg.output_msg(4);
		}
	}
}
//Intercept the biotope-type damage raster with the given polygons and change the biotope-type-id
void Dam_Eco_System::intercept_polygons2biotope_type_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	bool intercept_flag=false;
	ostringstream prefix;
	ostringstream cout;
	prefix << "ECO> BTYPE>";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	//check if there are any polygon interception with the rasters
	for(int j=0; j< this->number_raster_btype; j++){
		intercept_flag=false;
		for(int i=0; i< number_polys; i++){
			intercept_flag=this->raster_btypes[j].check_interception(&polys[i]);
			if(intercept_flag==true){
				break;
			}
		}
		if(intercept_flag==true){
			cout <<"Intercept ecological biotope-type raster "<<this->raster_btypes[j].get_raster_name()<< " with the polygon(s)..." <<endl;
			this->raster_btypes[j].input_raster_perdatabase_element_data(ptr_database, id);
			Sys_Common_Output::output_dam->output_txt(&cout);
			prefix <<"RAST_"<<this->raster_btypes[j].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			for(int i=0; i< number_polys; i++){
				if(this->raster_btypes[j].check_interception(&polys[i])==true){
					this->raster_btypes[j].set_intercepted_elem2new_biotope_type(ptr_database, id, &polys[i], current_id, new_id);
				}
			}

			Sys_Common_Output::output_dam->rewind_userprefix();
			this->raster_btypes[j].clear_raster();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Intercept the soil-erosion damage raster with the given polygons and change the soil-type-id
void Dam_Eco_System::intercept_polygons2soil_type_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	bool intercept_flag=false;
	ostringstream prefix;
	ostringstream cout;
	prefix << "ECO> SOIL>";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	//check if there are any polygon interception with the rasters
	for(int j=0; j< this->number_soil_ero_raster; j++){
		intercept_flag=false;
		for(int i=0; i< number_polys; i++){
			intercept_flag=this->soil_raster[j].check_interception(&polys[i]);
			if(intercept_flag==true){
				break;
			}
		}
		if(intercept_flag==true){
			cout <<"Intercept ecological soil-erosion raster "<<this->soil_raster[j].get_raster_name()<< " with the polygon(s)..." <<endl;
			this->soil_raster[j].input_raster_perdatabase_element_data(ptr_database, id);
			Sys_Common_Output::output_dam->output_txt(&cout);
			prefix <<"RAST_"<<this->soil_raster[j].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			for(int i=0; i< number_polys; i++){
				if(this->soil_raster[j].check_interception(&polys[i])==true){
					this->soil_raster[j].set_intercepted_elem2new_soil_type(ptr_database, id, &polys[i], current_id, new_id);
				}
			}

			Sys_Common_Output::output_dam->rewind_userprefix();
			this->soil_raster[j].clear_raster();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//_________
//private
//Connect the soil-erosion elements to the functions
void Dam_Eco_System::connect_soil_elems2functions(const int index){
	ostringstream cout;
	cout <<"Connect the elements of soil-type raster " << this->soil_raster[index].get_raster_name() << " to " ;
	cout << this->number_soil_cost_function <<" available soil-cost function(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->soil_raster[index].connect_soil_type2elems(this->number_soil_cost_function, this->soil_cost_function);

	//check them
	cout <<"Check the connection of the soil raster elements to the soil-cost functions..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->soil_raster[index].check_soil_cost_function_connection();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	cout <<"Connect the elements of soil-type raster " << this->soil_raster[index].get_raster_name() << " to " ;
	cout << this->number_soil_cost_function <<" available soil-erosion function(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->soil_raster[index].connect_soil_erosion2elems(this->number_soil_ero_function, this->soil_ero_function);

	//check them
	cout <<"Check the connection of the soil raster elements to the soil-erosion functions..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->soil_raster[index].check_soil_cost_function_connection();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
}
//Connect the biotope elements to the functions
void Dam_Eco_System::connect_biotope_elems2functions(const int index){
	ostringstream cout;
	cout <<"Connect the elemnts of biotope-type raster " << this->raster_btypes[index].get_raster_name() << " to " << this->number_btype <<" available biotope type(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster_btypes[index].connect_biotype2elems(this->number_btype, this->b_types);

	//check them
	cout <<"Check the connection of the biotope-type raster elements to the biotope-types..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->raster_btypes[index].check_biotype_connection();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
}
//Set intercepted elements to no-info
void Dam_Eco_System::set_intercept_elems2no_info(void){
	ostringstream prefix;
	prefix << "ECO> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Check the interception of the biotope-type rasters..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->number_raster_btype; i++){
		for(int j=0; j< this->number_raster_btype; j++){
			if(j>i){
				this->raster_btypes[i].set_intercepted_elem2noinfo(&this->raster_btypes[j]);
			}
		}
	}
	cout <<"Check the interception of the soil-type rasters..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->number_soil_ero_raster; i++){
		for(int j=0; j< this->number_soil_ero_raster; j++){
			if(j>i){
				this->soil_raster[i].set_intercepted_elem2noinfo(&this->soil_raster[j]);
			}
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Allocate the biotope-types
void Dam_Eco_System::allocate_btypes(void){
	try{
		this->b_types=new Dam_Eco_Btype[this->number_btype];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the biotope-types
void Dam_Eco_System::delete_btypes(void){
	if(this->b_types!=NULL){
		delete []this->b_types;
		this->b_types=NULL;
	}
}
//Allocate the raster of the biotope-types
void Dam_Eco_System::allocate_btypes_raster(void){
	try{
		this->raster_btypes=new Dam_Eco_Btype_Raster[this->number_raster_btype];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the raster of thebiotope-types
void Dam_Eco_System::delete_btypes_raster(void){
	if(this->raster_btypes!=NULL){
		delete []this->raster_btypes;
		this->raster_btypes=NULL;
	}
}
//Allocate the raster of the soil-erosion
void Dam_Eco_System::allocate_soil_ero_raster(void){
	try{
		this->soil_raster=new Dam_Eco_Soil_Raster[this->number_soil_ero_raster];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the raster of the soil-erosion
void Dam_Eco_System::delete_soil_ero_raster(void){
	if(this->soil_raster!=NULL){
		delete []this->soil_raster;
		this->soil_raster=NULL;
	}
}
//Allocate the functions of the soil-erosion
void Dam_Eco_System::allocate_soil_ero_function(void){
	try{
		this->soil_ero_function=new Dam_Eco_Soil_Erosion_Function[this->number_soil_ero_function];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the functions of the soil-erosion
void Dam_Eco_System::delete_soil_ero_function(void){
	if(this->soil_ero_function!=NULL){
		delete []this->soil_ero_function;
		this->soil_ero_function=NULL;
	}
}
//Allocate the functions of the soil-cost
void Dam_Eco_System::allocate_soil_cost_function(void){
	try{
		this->soil_cost_function=new Dam_Eco_Soil_Cost_Function[this->number_soil_cost_function];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the functions of the soil-cost
void Dam_Eco_System::delete_soil_cost_function(void){
	if(this->soil_cost_function!=NULL){
		delete []this->soil_cost_function;
		this->soil_cost_function=NULL;
	}
}
//Check the if there are same soil-erosion raster
void Dam_Eco_System::check_soil_erosion_raster(void){
	for(int i=0; i< this->number_soil_ero_raster; i++){
		for(int j=0; j< this->number_soil_ero_raster; j++){
			if(j>i){
				if(this->soil_raster[i].compare_raster_infos(&this->soil_raster[j])==true){
					Error msg=this->set_error(5);
					ostringstream info;
					info <<"1. Rasterno.  : "<< i << endl;
					info <<"1. Rastername : "<< this->soil_raster[i].get_raster_name() << endl;
					info <<"2. Rasterno.  : "<< j << endl;
					info <<"2. Rastername : "<< this->soil_raster[j].get_raster_name() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
}
//Check the soil-erosion functions
void Dam_Eco_System::check_soil_erosion_function(void){
	for(int i=0; i< this->number_soil_ero_function; i++){
		for(int j=0; j<this->number_soil_ero_function; j++){
			if(i!=j){
				if(this->soil_ero_function[i].get_index()==this->soil_ero_function[j].get_index()){
					Error msg=this->set_error(7);
					ostringstream info;
					info <<"Index of the soil-cost function   : " << this->soil_ero_function[i].get_index() << endl;
					info <<"Name of the 1. soil-cost function : " << this->soil_ero_function[i].get_name() << endl;
					info <<"Name of the 2. soil-cost function : " << this->soil_ero_function[j].get_name() << endl;
					throw msg;
				}
			}
		}
	}
}
//Check the soil-cost functions
void Dam_Eco_System::check_soil_cost_function(void){
	for(int i=0; i< this->number_soil_cost_function; i++){
		for(int j=0; j<this->number_soil_cost_function; j++){
			if(i!=j){
				if(this->soil_cost_function[i].get_index()==this->soil_cost_function[j].get_index()){
					Error msg=this->set_error(8);
					ostringstream info;
					info <<"Index of the soil-cost function   : " << this->soil_cost_function[i].get_index() << endl;
					info <<"Name of the 1. soil-cost function : " << this->soil_cost_function[i].get_name() << endl;
					info <<"Name of the 2. soil-cost function : " << this->soil_cost_function[j].get_name() << endl;
					throw msg;
				}
			}
		}
	}
}
//Check the if there are same biotope-type raster
void Dam_Eco_System::check_b_type_raster(void){
	for(int i=0; i< this->number_raster_btype; i++){
		for(int j=0; j< this->number_raster_btype; j++){
			if(j>i){
				if(this->raster_btypes[i].compare_raster_infos(&this->raster_btypes[j])==true){
					Error msg=this->set_error(4);
					ostringstream info;
					info <<"1. Rasterno.  : "<< i << endl;
					info <<"1. Rastername : "<< this->raster_btypes[i].get_raster_name() << endl;
					info <<"2. Rasterno.  : "<< j << endl;
					info <<"2. Rastername : "<< this->raster_btypes[j].get_raster_name() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
}
//Check the biotope-types
void Dam_Eco_System::check_biotope_type(void){
	for(int i=0; i< this->number_btype; i++){
		for(int j=0; j<this->number_btype; j++){
			if(i!=j){
				if(this->b_types[i].get_index()==this->b_types[j].get_index()){
					Error msg=this->set_error(6);
					ostringstream info;
					info <<"Index of the biotope type   : " << this->b_types[i].get_index() << endl;
					info <<"Name of the 1. biotope type : " << this->b_types[i].get_name() << endl;
					info <<"Name of the 2. biotope type : " << this->b_types[j].get_name() << endl;
					throw msg;
				}
			}
		}
	}
}
//Output the error-statistic of the system
void Dam_Eco_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "ECO> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the ECO-system..." << endl ;
	if(this->number_btype>0 || this->number_raster_btype>0){
		cout << "BIOTOPE TYPES"<< endl;
		if(this->number_btype>0){
			cout << " Number biope type()s            :" << W(3) << this->number_btype <<endl;
		}
		if(this->number_raster_btype>0){
			cout << " Number biotope type raster(s)   :" << W(3) << this->number_raster_btype <<endl;
		}
	}
	if(this->number_soil_cost_function>0 || this->number_soil_ero_function>0 || this->number_soil_ero_raster>0){
		cout << "SOIL"<< endl;
		if(this->number_soil_cost_function>0){
			cout << " Number soil cost function(s)    :" << W(3) << this->number_soil_cost_function <<endl;
		}
		if(this->number_soil_ero_function>0){
			cout << " Number soil erosion function(s) :" << W(3) << this->number_soil_ero_function <<endl;
		}
		if(this->number_soil_ero_raster>0){
			cout << " Number soil erosion raster(s)   :" << W(3) << this->number_soil_ero_raster <<endl;
		}
	}
	cout << "TOTAL ERROR-/WARNING-NUMBER"<< endl;
	cout << " Error(s)                        :" << W(3) << this->number_error <<endl;
	cout << " Warning(s)                      :" << W(3) << this->get_occured_warning() <<endl;
	if(Dam_Damage_System::get_stop_thread_flag()==true){
		cout << " User has aborted the calculation " << endl;
	}
	Sys_Common_Output::output_dam->output_txt(&cout);
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Delete selected biotope raster in database
void Dam_Eco_System::delete_selected_biotope_raster_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->number_raster_btype ; i++){
		if(this->raster_btypes[i].get_is_selected()==true){
			Dam_Eco_Btype_Raster::delete_data_in_raster_table(ptr_database, this->raster_btypes[i].get_raster_number());
			Dam_Eco_Btype_Element::delete_data_in_elem_table(ptr_database, this->raster_btypes[i].get_raster_number());
			Dam_Eco_Btype_Element::delete_data_in_erg_table_per_raster(ptr_database, this->raster_btypes[i].get_raster_number());
		}
	}
}
//Delete selected soil erosion raster in database
void Dam_Eco_System::delete_selected_soil_erosion_raster_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->number_soil_ero_raster ; i++){
		if(this->soil_raster[i].get_is_selected()==true){
			Dam_Eco_Soil_Raster::delete_data_in_raster_table(ptr_database, this->soil_raster[i].get_raster_number());
			Dam_Eco_Soil_Element::delete_data_in_elem_table(ptr_database, this->soil_raster[i].get_raster_number());
			Dam_Eco_Soil_Element::delete_data_in_erg_table_per_raster(ptr_database, this->soil_raster[i].get_raster_number());
		}
	}
}
//Set the warning
Warning Dam_Eco_System::set_warning(const int warn_type){
	string place="Dam_Eco_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="Ecological biotope type damage raster are set, but no biotope types";
			reaction="Can not calculate any ecological biotope type damages";
			help= "Check the ecological biotpe types";
			type=17;
			break;
		case 1://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="Ecological biotope types are set, but no biotope type raster";
			reaction="Can not calculate any ecological biotope type damages";
			help= "Check the ecological biotope type damage raster";
			type=17;
			break;
		case 2://not all rasters are connected
			place.append("check_system(void)") ;
			reason="Not all ecological biotope type damage rasters are connected to the hydraulic system";
			help= "Connect the ecological damage raster to avoid errors in the damage results";
			type=17;
			break;
		case 3://not all rasters are connected
			place.append("check_system(void)") ;
			reason="Not all ecological soil-erosion damage rasters are connected to the hydraulic system";
			help= "Connect the ecological damage raster to avoid errors in the damage results";
			type=17;
			break;
		case 4://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="Ecological soil-cost functions are set, but no soil-erosion raster";
			reaction="Can not calculate any ecological soil-erosion damages";
			help= "Check the ecological soil-erosion damage raster";
			type=17;
			break;
		case 5://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="Ecological soil-erosion functions are set, but no soil-erosion raster";
			reaction="Can not calculate any ecological soil-erosion damages";
			help= "Check the ecological soil-erosion damage raster";
			type=17;
			break;
		case 6://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="Ecological soil-erosion damage raster are set, but no soil-cost function";
			reaction="Can not calculate any ecological soil-erosion damages";
			help= "Check the ecological soil-cost function";
			type=17;
			break;
		case 7://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="Ecological soil-erosion damage raster are set, but no soil-erosion function";
			reaction="Can not calculate any ecological soil-erosion damages";
			help= "Check the ecological soil-erosion function";
			type=17;
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
//Set error(s)
Error Dam_Eco_System::set_error(const int err_type){
	string place="Dam_Eco_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_...(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open biotope-types file
			place.append("read_biotope_types_per_file(const string file_biotype)");
			reason="Can not open the file of the biotope-types";
			help="Check the file";
			type=5;
			break;
		case 2://do not find number of biotope-types in file
			place.append("read_biotope_types_per_file(const string file_biotype)");
			reason="Do not find the keyword for the number of biotope-types in file !NO_BIO_TYPE";
			help="The number of biotope-types must be declared before the individual biotope-types data begins";
			type=1;
			break;
		case 3://wrong input
			place.append("read_biotope_types_per_file(const string file_biotype)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the biotope-types in file";
			type=1;
			break;
		case 4://there can't be same raster
			place.append("check_b_type_raster(void)");
			reason="There are same raster in the database";
			help="Check the geometrical settings of the biotope-type raster";
			type=23;
			break;
		case 5://there can't be same raster
			place.append("check_soil_erosion_raster");
			reason="There are same raster in the database";
			help="Check the geometrical settings of the soil-erosion raster";
			type=23;
			break;
		case 6://there are biotope-types with the same index
			place.append("check_biotope_type(void)");
			reason="There are biotope-type with the same index";
			help="Check the biotope-types";
			type=23;
			break;
		case 7://there soil-erosion function with the same index
			place.append("check_soil_erosion_function(void)");
			reason="There are soil-erosion function with the same index";
			help="Check the soil-erosion functions";
			type=23;
			break;
		case 8://there soil-cost function with the same index
			place.append("check_soil_cost_function(void)");
			reason="There are soil-cost function with the same index";
			help="Check the soil-cost functions";
			type=23;
			break;
		case 9://no adding of identical biotope-type raster
			place.append("add_biotope_raster_file2database(QSqlDatabase *ptr_database)");
			reason="You can not add raster with identical general information like the geometry";
			help="Check the biotope-type raster for adding";
			type=23;
			break;
		case 10://no adding of identical soil-erosion raster
			place.append("add_soil_type_erosion_raster_file2database(QSqlDatabase *ptr_database");
			reason="You can not add raster with identical general information like the geometry";
			help="Check the soil-erosion raster for adding";
			type=23;
			break;
		case 11://not all required rasters are set
			place.append("add_biotope_raster_file2database(QSqlDatabase *ptr_database)") ;
			reason="The required ecological biotope-type damage raster file is not set. The raster is not transferd to database";
			help= "Set the file correctly";
			type=1;
			break;
		case 12://not all required rasters are set
			place.append("add_soil_type_erosion_raster_file2database(QSqlDatabase *ptr_database)") ;
			reason="Not all required ecological soil damage raster file(s) are set. The raster is not transferd to database";
			help= "Set all files correctly";
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