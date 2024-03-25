#include "Dam_Headers_Precompiled.h"
//#include "Dam_Ecn_System.h"

//Default constructor
Dam_Ecn_System::Dam_Ecn_System(void){
	this->dam_function=NULL;
	this->no_dam_func=0;
	this->damage_raster=NULL;
	this->no_dam_raster=0;

	this->file_dam_func=label::not_set;
	this->file_raster_immob=label::not_set;
	this->file_raster_mob=label::not_set;
	this->file_raster_immob_stock=label::not_set;
	this->file_raster_mob_stock=label::not_set;

	this->reset_result_members();
	this->del_raster_flag=true;
	this->del_func_flag=true;
	this->del_res_flag=true;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Ecn_System), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Ecn_System::~Dam_Ecn_System(void){
	this->delete_damage_raster();
	this->delete_damage_function();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Ecn_System), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Get the number of raster of the ecnomic damage system
int Dam_Ecn_System::get_number_raster(void){
	return this->no_dam_raster;
}
//Import the damage functions from a file to the database
void Dam_Ecn_System::import_damage_func_file2database(QSqlDatabase *ptr_database){
/**Other already existing damage functions in the database are deleted.
This file should contain the damage function for the immobile as well
as for the mobile damage calculation.
\see Dam_Ecn_Damage_Function
*/
	try{
		this->set_start_warnings_number();
		//first delete all function data
		Dam_Ecn_Damage_Function::delete_data_in_function_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the economical damage function to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_dam_func!=label::not_set){
			//read the data in
			this->read_damage_function_per_file(this->file_dam_func);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_dam_func();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->no_dam_func<<" economical damage function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->no_dam_func; i++){
				this->dam_function[i].transfer_input_members2database(ptr_database);
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
	cout <<"Data-Import of the economical damage function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add the damage function from a file to the database (the existing one are not deleted)
void Dam_Ecn_System::add_damage_func_file2database(QSqlDatabase *ptr_database){
	try{
		this->set_start_warnings_number();

		ostringstream cout;
		cout <<"Add the data of the economical land use categories to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "ADD> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_dam_func!=label::not_set){
			//read the data in
			this->read_damage_function_per_file(this->file_dam_func);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_dam_func();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->no_dam_func<<" economical damage function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->no_dam_func; i++){
				this->dam_function[i].add_input_members2database(ptr_database);
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
	cout <<"Data-Adding of the economical damage function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a economical damage raster from file to database
void Dam_Ecn_System::add_damage_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used){
/**A economical raster consists of two to four rasters:
	- two rasters containing the categories of land-use in each element for the mobile and the
	immobile damage calculation.
	- two rasters for setting an individual stock-value per element for the mobile and the
	immobile damage calculation. The settings of this rasters is optional.
	If they are not set, the default stock-value -declared by the damage functions- are used.
*/
	try{
		if(internaly_used==false){
			this->set_start_warnings_number();
		}
		ostringstream cout;
        cout <<"Add the data of the economical damage raster to the database..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_raster_mob!=label::not_set && this->file_raster_immob!=label::not_set){
			//read the data in
			this->read_damage_raster_per_file(1, &this->file_raster_mob, &this->file_raster_immob, &this->file_raster_mob_stock, &this->file_raster_immob_stock);
			Dam_Damage_System::check_stop_thread_flag();

			//compare with existing damage raster in database

			Dam_Ecn_Raster *raster_buffer=NULL;
			QSqlQueryModel results;
			int number_buff_rast=Dam_Ecn_Raster::select_relevant_raster_database(&results,ptr_database, false);
			if(number_buff_rast>0){
				raster_buffer=new Dam_Ecn_Raster[number_buff_rast];
				_sys_system_id id_buff;
				id_buff.area_state=0;
				id_buff.measure_nr=0;
				//read them in
				for(int i=0; i< number_buff_rast; i++){
					raster_buffer[i].input_raster_perdatabase_general_data(&results, i);
				}
				//check them
				for(int i=0; i< this->no_dam_raster; i++){
					for(int j=0; j< number_buff_rast; j++){
						if(this->damage_raster[i].compare_raster_infos(&raster_buffer[j])==true){
							Error msg=this->set_error(6);
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
			this->output_raster();
			Dam_Damage_System::check_stop_thread_flag();

			//transfer it to database
				for(int i=0; i< this->no_dam_raster; i++){
					try{
						this->damage_raster[i].transfer_input_members2database(ptr_database);
					}
					catch(Error msg){
						Dam_Ecn_Raster::delete_data_in_raster_table(ptr_database,this->damage_raster[i].get_raster_number());
						Dam_Ecn_Element::delete_data_in_elem_table(ptr_database, this->damage_raster[i].get_raster_number());
						throw msg;
					}
				}
		}
		else{
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Filename of immobile damage information : " << this->file_raster_immob <<endl;
			info << "Filename of mobile damage information   : " << this->file_raster_mob <<endl;
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
	cout <<"Data-Adding of the economical damage raster is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(internaly_used==false){
		this->output_error_statistic();
	}
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a economical damage raster from file to database for multiple raster input
void Dam_Ecn_System::add_damage_raster_file2database_multi(QSqlDatabase *ptr_database){
	ostringstream prefix;
	this->set_start_warnings_number();

	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		prefix << "MULTI_"<<i+1<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		ostringstream cout;
		cout <<"Add the data of the economical damage raster "<<i+1<<"..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->file_raster_immob=this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
		this->file_raster_mob=this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();
		this->file_raster_immob_stock=this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->get_file_name();
		this->file_raster_mob_stock=this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->get_file_name();
		this->add_damage_raster_file2database(ptr_database, true);
		cout <<"Data import of the economical damage raster "<<i+1<<" is finished..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_dam->rewind_userprefix();
		this->delete_damage_raster();
	}

	//just for the statistic output
	this->no_dam_raster=this->number_raster_dia->get_number_raster();
	this->output_error_statistic();
}
//Delete all economical data in the database: raster, raster elements, damage function, results
void Dam_Ecn_System::del_damage_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECN> ";

	ostringstream cout;
	cout <<"Delete economical damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	//ecn
	try{
		if(this->del_raster_flag==true){
			cout <<"ECN-raster..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->delete_selected_raster_database(ptr_database);
		}
		if(this->del_func_flag==true){
			cout <<"ECN-damage-function..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Ecn_Damage_Function::delete_data_in_function_table(ptr_database);

		if(this->del_res_flag==true || this->del_func_flag==true){
			cout <<"ECN-results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Ecn_Element::delete_data_in_erg_table(ptr_database);
		}
}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of economical damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Ask for the file of the damage function per dialog
bool Dam_Ecn_System::ask_file_damage_function(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_dam_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":dam_icon");

	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the economic land use categories are stored."<< endl;
	buffer << "This file should contain the damage-function for the land use category"<< endl;
	buffer << "as well as the stock-value." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECN-land use category file");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECN-land use category file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_dam_func=my_dia.get_ptr_file_browser(0)->get_file_name();
		if(this->file_dam_func==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the ECN-land use category!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file of the damage raster per dialog
bool Dam_Ecn_System::ask_file_damage_raster(QWidget *parent){
	/**	If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The results are set to  the members
	file_raster_immob , file_raster_mob , file_raster_immob_stock , file_raster_mob_stock .*/
	Sys_Multi_Filechooser_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":dam_icon");
	my_dia.set_number_file_browser(4,icon);
	stringstream buffer;
	buffer << "Choose the file(s), where the economic damage-raster(s) are stored."<< endl;
	buffer << "The files of the individual stock values are optional. If they are not set"<< endl;
	buffer << "the default stockvalues of the categories is used"<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose ECN-raster file(s)");
	my_dia.get_ptr_file_browser(0)->set_text_label("ECN-damage raster file for the immobile damage land-use categories");
	my_dia.get_ptr_file_browser(0)->set_tooltip("The immobile land-use categories are connected to the elements");

	my_dia.get_ptr_file_browser(1)->set_text_label("ECN-damage raster file for the mobile damage land-use categories");
	my_dia.get_ptr_file_browser(1)->set_tooltip("The mobile land-use categories are connected to the elements");

	my_dia.get_ptr_file_browser(2)->set_text_label("ECN-damage raster file for the individual immobile stock-values [optional]");
	my_dia.get_ptr_file_browser(2)->set_tooltip("If this raster is not set, the default stock-values assigned by the land-use categories is used");

	my_dia.get_ptr_file_browser(3)->set_text_label("ECN-damage raster file for the individual mobile stock-values [optional]");
	my_dia.get_ptr_file_browser(3)->set_tooltip("If this raster is not set, the default stock-values assigned by the land-use categories is used");
	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_raster_immob=my_dia.get_ptr_file_browser(0)->get_file_name();
		this->file_raster_mob=my_dia.get_ptr_file_browser(1)->get_file_name();
		this->file_raster_immob_stock=my_dia.get_ptr_file_browser(2)->get_file_name();
		this->file_raster_mob_stock=my_dia.get_ptr_file_browser(3)->get_file_name();
		return true;
	}
}
//Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
bool Dam_Ecn_System::ask_file_damage_raster_multi(void){
	//ask the number of multiple raster
	this->allocate_number_raster_dia();
	QIcon icon;
	ostringstream txt;
	txt<<"Number of ECN raster...";
	icon.addFile(":dam_icon");
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
		this->mulit_raster_import_dia[i].set_number_file_browser(4,icon);
		stringstream buffer;
		buffer << "Choose the file(s), where the economic damage-raster(s) "<<endl;
		buffer << " (" << i+1 << " of " << this->number_raster_dia->get_number_raster() << ") are stored."<< endl;
		buffer << "The files of the individual stock values are optional. If they are not set"<< endl;
		buffer << "the default stockvalues of the categories is used"<< endl;
		this->mulit_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		this->mulit_raster_import_dia[i].set_window_title("Choose ECN-raster file(s)");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("ECN-damage raster file for the immobile damage land-use categories");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("The immobile land-use categories are connected to the elements");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_text_label("ECN-damage raster file for the mobile damage land-use categories");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_tooltip("The mobile land-use categories are connected to the elements");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->set_text_label("ECN-damage raster file for the individual immobile stock-values [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->set_tooltip("If this raster is not set, the default stock-values assigned by the land-use categories is used");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->set_text_label("ECN-damage raster file for the individual mobile stock-values [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->set_tooltip("If this raster is not set, the default stock-values assigned by the land-use categories is used");
		//start dialog
		if(this->mulit_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	return true;
}
//Ask for raster to handled in a dialog
bool Dam_Ecn_System::ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	DamGui_Raster_Dia my_dia(parent);

	this->read_damage_raster_per_database_general(ptr_database,false);

	if(my_dia.start_dialog(this->damage_raster, this->no_dam_raster)==true){
		return true;
	}
	else{
		return false;
	}
}
//Ask for the flag, what should be deleted (raster, damage functions)
bool Dam_Ecn_System::ask_deleting_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":dam_icon");
	my_dia.set_number_check_boxes(3,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the ECN-system in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting ECN-system");
	my_dia.get_ptr_check_box(0)->setText("Delete ECN-damage raster");
	my_dia.get_ptr_check_box(1)->setText("Delete ECN-damage land use categories");
	my_dia.get_ptr_check_box(2)->setText("Delete ECN-damage results");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->del_raster_flag=my_dia.get_bool_check_box(0);
		this->del_func_flag=my_dia.get_bool_check_box(1);
		this->del_res_flag=my_dia.get_bool_check_box(2);
		return true;
	}
}
//Get a text for deleting
string Dam_Ecn_System::get_deleting_text(void){
	ostringstream buffer;
	if(this->del_raster_flag==true){
		buffer << " The raster(s) will be deleted in database, also the results in the raster!"<<endl;
	}
	if(this->del_func_flag==true){
		buffer << " All damage function(s) will be deleted in database, also all results!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//Read in the economical damage system per database
void Dam_Ecn_System::read_damage_system_per_database(QSqlDatabase *ptr_database){
	this->read_damage_function_per_database(ptr_database);
	this->read_damage_raster_per_database_general(ptr_database);
}
//Read in the economic damage functions from file
void Dam_Ecn_System::read_damage_function_per_file(const string file_damfunc){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	//read in damage function
	//open file
	ifile.open(file_damfunc.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file_damfunc << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_dam->rewind_userprefix();
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
			my_stream >> this->no_dam_func;
			if(my_stream.fail()==true || this->no_dam_func<=0){
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
		info << "Filename              : "<< file_damfunc << endl;
		msg.make_second_info(info.str());
		ifile.close();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	//allocate the damage functions
	try{
		this->allocate_damage_function();
	}
	catch(Error msg){
		ifile.close();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	try{
		//read in the damage functions
		for(int i=0; i< this->no_dam_func; i++){
			this->dam_function[i].read_function(&ifile, &line_counter, i);
		}
		this->check_damage_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Filename              : "<< file_damfunc << endl;
		msg.make_second_info(info.str());
		ifile.close();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	//close file and reset it
	ifile.close();
	ifile.clear();
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the economic damage functions from database
void Dam_Ecn_System::read_damage_function_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the damage function per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->no_dam_func=Dam_Ecn_Damage_Function::select_relevant_functions_database(&results);
		this->allocate_damage_function();
		for(int i=0; i< this->no_dam_func; i++){
			this->dam_function[i].input_function_perdatabase(&results, i);
		}
		this->check_damage_function();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();

		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the economic raster from file
void Dam_Ecn_System::read_damage_raster_per_file(const int no, string *fname_mob, string *fname_immob, string *fname_mob_stock, string *fname_immob_stock){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	try{
		this->no_dam_raster=no;

		//allocate the raster
		this->allocate_damage_raster();

		for(int i=0; i< this->no_dam_raster; i++){

			this->damage_raster[i].set_raster_number(i);
			this->damage_raster[i].read_all_damage_raster_per_file(fname_mob[i], fname_immob[i], fname_mob_stock[i], fname_immob_stock[i] );

		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the general information of the economic raster from database
void Dam_Ecn_System::read_damage_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "ECN> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the general damage raster information per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		QSqlQueryModel results;
		//number of raster
		this->no_dam_raster=Dam_Ecn_Raster::select_relevant_raster_database(&results, ptr_database);
		this->allocate_damage_raster();
		for(int i=0; i< this->no_dam_raster; i++){
			this->damage_raster[i].input_raster_perdatabase_general_data(&results, i);
		}

		//check them
		this->check_raster();
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
//Calculate the damages
void Dam_Ecn_System::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int dam_raster_id){
	ostringstream cout;
	cout << "Calculate damages for economical raster " <<this->damage_raster[dam_raster_id].get_raster_name()<<"..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->damage_raster[dam_raster_id].calculate_damages(impact_fp, number);
}
///Delete the result members for a given system-id and a scenario (boundary-, break-)
void Dam_Ecn_System::delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_Ecn_Element::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the result members to a database table
void Dam_Ecn_System::output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int dam_raster_id, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(dam_raster_id==this->no_dam_raster-1 && *was_output==false){
		must_output=true;
	}
	cout << "Output economical damages to database of "<< this->damage_raster[dam_raster_id].get_raster_name()<<"..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->damage_raster[dam_raster_id].output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Output the members
void Dam_Ecn_System::output_members(void){
	this->output_raster();
	this->output_dam_func();
}
//Output statistic of economical damage system
void Dam_Ecn_System::output_statistic(void){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"DAMAGE RASTER"<<endl;
	cout << " Number             " << this->no_dam_raster << endl;
	cout<<"DAMAGE FUNCTION"<<endl;
	cout << " Number             " << this->no_dam_func << endl;

	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the sum over the raster of the damage results to display/console
void Dam_Ecn_System::output_result_damage(void){
	ostringstream prefix;

	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;

	cout <<"RESULTS PER RASTER"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout,true);
	for(int i=0; i< this->no_dam_raster; i++){
		this->damage_raster[i].output_result_damage();
	}
	cout <<"RESULTS PER LANDUSE"<<endl;
	cout << " Landuse_id ,"<<" Landuse_name ,"<< " Mid "<<label::euro<<", 5%_quantile "<<label::euro<< ", 95%_quantile "<<label::euro<<endl;
	for(int i=0; i< this->no_dam_func; i++){
		this->dam_function[i].output_result_landuse_damage(&cout);
	}

	Sys_Common_Output::output_dam->output_txt(&cout,true);
	Sys_Common_Output::output_dam->reset_prefix_was_outputed();

	cout << "TOTAL RESULTS"<<endl;
	cout << " Immobile (Mid/5%/95%)  : " << P(2)<< FORMAT_FIXED_REAL << this->result_immob_dam.mid_result << "  "<< this->result_immob_dam.quantile_5_result<< "  "<< this->result_immob_dam.quantile_95_result << label::euro << endl;
	cout << " Mobile (Mid/5%/95%)    : " <<  this->result_mob_dam.mid_result << "  "<< this->result_mob_dam.quantile_5_result<< "  "<< this->result_mob_dam.quantile_95_result << label::euro << endl;
	cout << " Total (Mid/5%/95%)     : " <<  this->result_total_dam.mid_result << "  "<< this->result_total_dam.quantile_5_result<< "  "<< this->result_total_dam.quantile_95_result << label::euro << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the results of the immobile damages
_dam_ecn_result Dam_Ecn_System::get_immob_results(void){
	return result_immob_dam;
}
//Get the results of the mobile damages
_dam_ecn_result Dam_Ecn_System::get_mob_results(void){
	return result_mob_dam;
}
//Init damage raster(s)
void Dam_Ecn_System::init_damage_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	this->damage_raster[index].input_raster_perdatabase_element_data(ptr_database, id);
	this->connect_elems2functions(index);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Reduce area of intercepted elements
void Dam_Ecn_System::reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	try{
		cout <<"Check the interception of "<<this->no_dam_raster<<" damage raster(s)..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->no_dam_raster-1; i++){
			cout <<"Intercept economical raster "<<this->damage_raster[i].get_raster_name()<< " with the other raster(s).." <<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//check if there is an interception
			bool intercept_flag=false;
			for(int j=0; j< this->no_dam_raster; j++){
				if(j>i){
					intercept_flag=this->damage_raster[i].check_interception(&this->damage_raster[j]);
					if(intercept_flag==true){
						break;
					}
				}
			}

			if(intercept_flag==true){
				this->damage_raster[i].input_raster_perdatabase_element_data(ptr_database, id);
				prefix <<"RAST_"<<this->damage_raster[i].get_raster_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);
				for(int j=0; j< this->no_dam_raster; j++){
					Dam_Damage_System::check_stop_thread_flag();
					if(j>i){
						this->damage_raster[i].set_intercepted_elem2reduced_area(&this->damage_raster[j], ptr_database, id);
					}
				}

				this->damage_raster[i].transfer_intercepted_elem_data2database(ptr_database);
				Sys_Common_Output::output_dam->rewind_userprefix();
				this->damage_raster[i].clear_raster();
			}
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Intercept damage elements with the hydraulic elements
void Dam_Ecn_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	cout <<"Check the interception of "<<this->no_dam_raster<<" damage raster(s) to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int *rang=NULL;
	cout <<"Sort hydraulic floodplain models..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);

	try{
		for(int i=0; i< this->no_dam_raster; i++){
			this->damage_raster[i].input_raster_perdatabase_element_data(ptr_database, id);

			prefix <<"RAST_"<<this->damage_raster[i].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int j=0; j< number_fp; j++){
				cout <<"Check the interception of economical raster "<<this->damage_raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				prefix <<"FP_"<<fp_models[rang[j]].Param_FP.get_floodplain_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				if(this->damage_raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::complete_outside ||
					this->damage_raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::outside_with_boundary){
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
					cout <<"Generate elements, raster points and segments for the interception of economical raster "<<this->damage_raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].input_elems_database(ptr_database, &bound_result,number_bound, false, true);
				}
				this->damage_raster[i].set_intercepted_elem2floodplain_id(&fp_models[rang[j]]);

				if(delete_again==true){
					cout <<"Delete elements, raster points and segments of the hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].delete_elems_raster_geo();
				}
				Dam_Damage_System::check_stop_thread_flag();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}

			this->damage_raster[i].transfer_intercepted_elem_data2database(ptr_database);
			this->damage_raster[i].clear_raster();
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
}
//Transfer data evaluated by an interception to database: identifier of the floodplain, -floodplain element and the reduced area
void Dam_Ecn_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Transfer the data from the interception of "<<this->no_dam_raster<<" damage raster(s) to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->no_dam_raster; i++){
		this->damage_raster[i].transfer_intercepted_elem_data2database(ptr_database);
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_Ecn_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	this->reset_result_members();
	for(int i=0; i< this->no_dam_raster; i++){
		this->damage_raster[i].sum_total_results(ptr_database, id, bound_sz, break_sz);
		//sum it up
		this->result_immob_dam.mid_result=this->result_immob_dam.mid_result+this->damage_raster[i].get_immob_results().mid_result;
		this->result_mob_dam.mid_result=this->result_mob_dam.mid_result+this->damage_raster[i].get_mob_results().mid_result;

		this->result_immob_dam.quantile_5_result=this->result_immob_dam.quantile_5_result+this->damage_raster[i].get_immob_results().quantile_5_result;
		this->result_mob_dam.quantile_5_result=this->result_mob_dam.quantile_5_result+this->damage_raster[i].get_mob_results().quantile_5_result;

		this->result_immob_dam.quantile_95_result=this->result_immob_dam.quantile_95_result+this->damage_raster[i].get_immob_results().quantile_95_result;
		this->result_mob_dam.quantile_95_result=this->result_mob_dam.quantile_95_result+this->damage_raster[i].get_mob_results().quantile_95_result;
	}

	this->result_total_dam.mid_result=this->result_immob_dam.mid_result+this->result_mob_dam.mid_result;
	this->result_total_dam.quantile_5_result=this->result_immob_dam.quantile_5_result+this->result_mob_dam.quantile_5_result;
	this->result_total_dam.quantile_95_result=this->result_immob_dam.quantile_95_result+this->result_mob_dam.quantile_95_result;
}
//Check the economic damage system
void Dam_Ecn_System::check_system(void){
	if(this->no_dam_func==0 && this->no_dam_raster!=0){
		Warning msg=this->set_warning(0);
		msg.output_msg(4);
	}
	if(this->no_dam_func!=0 && this->no_dam_raster==0){
		Warning msg=this->set_warning(1);
		msg.output_msg(4);
	}
	if(this->no_dam_raster!=0){
		bool flag=true;
		for(int i=0; i<this->no_dam_raster; i++){
			//if raster are connected
			if(this->damage_raster[i].get_connected_flag()==false){
				flag=false;
				break;
			}
		}
		if(flag==false){
			Warning msg=this->set_warning(2);
			msg.output_msg(4);
		}
	}
}
//Intercept the economical damage raster with the given polygons and change the landuse-id
void Dam_Ecn_System::intercept_polygons2landuse_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	bool intercept_flag=false;
	ostringstream prefix;
	ostringstream cout;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	//check if there are any polygon interception with the rasters
	for(int j=0; j< this->no_dam_raster; j++){
		intercept_flag=false;
		for(int i=0; i< number_polys; i++){
			intercept_flag=this->damage_raster[j].check_interception(&polys[i]);
			if(intercept_flag==true){
				break;
			}
		}
		if(intercept_flag==true){
			cout <<"Intercept economical raster "<<this->damage_raster[j].get_raster_name()<< " with the polygon(s)..." <<endl;
			this->damage_raster[j].input_raster_perdatabase_element_data(ptr_database, id);
			Sys_Common_Output::output_dam->output_txt(&cout);
			prefix <<"RAST_"<<this->damage_raster[j].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			for(int i=0; i< number_polys; i++){
				if(this->damage_raster[j].check_interception(&polys[i])==true){
					this->damage_raster[j].set_intercepted_elem2new_landuse(ptr_database, id, &polys[i], current_id, new_id);
				}
			}

			Sys_Common_Output::output_dam->rewind_userprefix();
			this->damage_raster[j].clear_raster();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the flag if the raster are to delete
bool Dam_Ecn_System::get_raster2delete(void){
	return this->del_raster_flag;
}
//Calculate the damage per landuse id by using the calculated results in database
void Dam_Ecn_System::calculate_damage_landuse_id(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//load landuse if not loaded
	if(this->dam_function==NULL){
		this->read_damage_function_per_database(ptr_database);
	}

	for(int i=0; i< this->no_dam_func; i++){
		this->dam_function[i].calculate_damage_landuse_id(ptr_database, id, bound_sz, break_sz);
	}
}
//_________________
//private
//Reset the result members
void Dam_Ecn_System::reset_result_members(void){
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
//Connect the element to their functions
void Dam_Ecn_System::connect_elems2functions(const int index){
	ostringstream cout;
	cout <<"Connect the elements of economical raster " << this->damage_raster[index].get_raster_name() << " to " << this->no_dam_func <<" available damage function(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->damage_raster[index].connect_dam_function2elems(this->no_dam_func, this->dam_function);

	//check them
	cout <<"Check the connection of the economical raster elements to the damage functions..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->damage_raster[index].check_damage_function_connection();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set intercepted elements to no-info
void Dam_Ecn_System::set_intercept_elems2no_info(void){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Check the interception of the damage rasters..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->no_dam_raster; i++){
		for(int j=0; j< this->no_dam_raster; j++){
			if(j>i){
				this->damage_raster[i].set_intercepted_elem2noinfo(&this->damage_raster[j]);
			}
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Allocate the damage functions
void Dam_Ecn_System::allocate_damage_function(void){
	try{
		this->dam_function=new Dam_Ecn_Damage_Function[this->no_dam_func];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the damage functions
void Dam_Ecn_System::delete_damage_function(void){
	if(this->dam_function!=NULL){
		delete []this->dam_function;
		this->dam_function=NULL;
	}
}
//Allocate the geometrical raster for individual stock values
void Dam_Ecn_System::allocate_damage_raster(void){
	try{
		this->damage_raster=new Dam_Ecn_Raster[this->no_dam_raster];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the geometrical raster for individual stock values
void Dam_Ecn_System::delete_damage_raster(void){
	if(this->damage_raster!=NULL){
		delete []this->damage_raster;
		this->damage_raster=NULL;
	}
}
//Check the if there are same raster
void Dam_Ecn_System::check_raster(void){
	for(int i=0; i< this->no_dam_raster; i++){
		for(int j=0; j< this->no_dam_raster; j++){
			if(j>i){
				if(this->damage_raster[i].compare_raster_infos(&this->damage_raster[j])==true){
					Error msg=this->set_error(4);
					ostringstream info;
					info <<"1. Rasterno.  : "<< i << endl;
					info <<"1. Rastername : "<< this->damage_raster[i].get_raster_name() << endl;
					info <<"2. Rasterno.  : "<< j << endl;
					info <<"2. Rastername : "<< this->damage_raster[j].get_raster_name() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
}
//Check the damage function
void Dam_Ecn_System::check_damage_function(void){
	for(int i=0; i< this->no_dam_func; i++){
		for(int j=0; j<this->no_dam_func; j++){
			if(i!=j){
				if(this->dam_function[i].get_land_use_info().id==this->dam_function[j].get_land_use_info().id){
					Error msg=this->set_error(5);
					ostringstream info;
					info <<"Index of the landuse type   : " << this->dam_function[i].get_land_use_info().id << endl;
					info <<"Name of the 1. landuse type : " << this->dam_function[i].get_land_use_info().name << endl;
					info <<"Name of the 2. landuse type : " << this->dam_function[j].get_land_use_info().name << endl;
					throw msg;
				}
			}
		}
	}
}
//Output the damage function to display/console
void Dam_Ecn_System::output_dam_func(void){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;

	if(this->no_dam_func>0){
		cout << "Output " << this->no_dam_func << " economical damage function(s)..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->no_dam_func; i++){
			this->dam_function[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the rasters to display/console
void Dam_Ecn_System::output_raster(void){
	ostringstream prefix;
	prefix << "ECN> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());


	ostringstream cout;

	if(this->no_dam_raster>0){
		cout << "Output "<<this->no_dam_raster<<" raster(s) of economical damages..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->no_dam_raster; i++){
			this->damage_raster[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the error statistic of the system
void Dam_Ecn_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "ECN> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the ECN-system..." << endl ;
	cout << "GENERAL"<< endl;
	if(this->no_dam_func>0){
		cout << " Number land use category :" << W(3) << this->no_dam_func <<endl;
	}
	if(this->no_dam_raster>0){
		cout << " Number raster(s)         :" << W(3) << this->no_dam_raster <<endl;
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
//Delete selected raster in database
void Dam_Ecn_System::delete_selected_raster_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->no_dam_raster ; i++){
		if(this->damage_raster[i].get_is_selected()==true){
			Dam_Ecn_Raster::delete_data_in_raster_table(ptr_database, this->damage_raster[i].get_raster_number());
			Dam_Ecn_Element::delete_data_in_elem_table(ptr_database, this->damage_raster[i].get_raster_number());
			Dam_Ecn_Element::delete_data_in_erg_table_per_raster(ptr_database, this->damage_raster[i].get_raster_number());
		}
	}
}
//Set the warning
Warning Dam_Ecn_System::set_warning(const int warn_type){
	string place="Dam_Ecn_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="Economical damage raster are set, but no damage functions";
			reaction="Can not calculate any economical damages";
			help= "Check the economical damage functions";
			type=15;
			break;
		case 1://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="Economical damage functions are set, but no damage raster";
			reaction="Can not calculate any economical damages";
			help= "Check the economical damage raster";
			type=15;
			break;
		case 2://not all rasters are connected
			place.append("check_system(void)") ;
			reason="Not all economical damage rasters are connected to the hydraulic system";
			help= "Connect the economical damage raster to avoid errors in the damage results";
			type=15;
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
Error Dam_Ecn_System::set_error(const int err_type){
		string place="Dam_Ecn_System::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_damage_raster(void)/allocate_damage_function(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open damage function file
			place.append("read_damage_function_per_file(const string file_damfunc)");
			reason="Can not open the file of the damage functions";
			help="Check the file";
			type=5;
			break;
		case 2://do not find number of damage function in file
			place.append("read_damage_function_per_file(const string file_damfunc)");
			reason="Do not find the keyword for the number of damage functions in file !NO_FUNCTIONS";
			help="The number of damage functions must be declared before the individual damage function data begins";
			type=1;
			break;
		case 3://wrong input
			place.append("read_damage_function_per_file(const string file_damfunc)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage functions in file";
			type=1;
			break;
		case 4://there can't be same raster
			place.append("check_raster(void)");
			reason="There are same raster in the database";
			help="Check the geometrical settings of the economical raster";
			type=21;
			break;
		case 5://there damage functions with the same index
			place.append("check_damage_function(void)");
			reason="There are damage-function (land-use type) with the same index";
			help="Check the economical damage functions";
			type=21;
			break;
		case 6://no adding of identical raster
			place.append("add_damage_raster_file2database(QSqlDatabase *ptr_database)");
			reason="You can not add raster with identical general information like the geometry";
			help="Check the economical raster for adding";
			type=21;
			break;
		case 7://not all required rasters are set
			place.append("add_damage_raster_file2database(QSqlDatabase *ptr_database)") ;
			reason="Not all required economical damage raster file(s) are set. The raster is not transferd to database";
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
