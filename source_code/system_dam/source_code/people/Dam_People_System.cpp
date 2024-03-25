#include "Dam_Headers_Precompiled.h"
//#include "Dam_People_System.h"

//Default constructor
Dam_People_System::Dam_People_System(void){
	this->number_dam_function=0;
	this->number_raster=0;
	this->dam_function=NULL;
	this->raster=NULL;

	this->file_dam_func=label::not_set;
	this->file_raster_type=label::not_set;
	this->file_raster_pop_dens=label::not_set;

	this->result_affected_pop=0.0;
	this->result_endangered_pop=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_People_System), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_People_System::~Dam_People_System(void){
	this->delete_damage_function();
	this->delete_raster();

	this->del_raster_flag=true;
	this->del_func_flag=true;
	this->del_res_flag=true;

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_People_System), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Get the number of raster of the people2risk damage system
int Dam_People_System::get_number_raster(void){
	return this->number_raster;
}
//Import the damage function for the people2risk calculation from a file to the database
void Dam_People_System::import_damage_func_file2database(QSqlDatabase *ptr_database){
/**Other already existing people2risk damage functions in the database are deleted.
	This file should contain the damage function with three values for each zone.
	\see Dam_People_Damage_Function
*/
	try{
		this->set_start_warnings_number();
		//first delete all function data
		Dam_People_Damage_Function::delete_data_in_function_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the people2risk damage function to the database ..."<< endl;
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
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of " << this->number_dam_function << " people2risk damage function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_dam_function; i++){
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
	cout <<"Data-Import of the people2risk damage function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add the damage function for the people2risk calculation from a file to the database (the existing one are not deleted)
void Dam_People_System::add_damage_func_file2database(QSqlDatabase *ptr_database){
	try{
		this->set_start_warnings_number();

		ostringstream cout;
		cout <<"Add the data of the people2risk damage function to the database ..."<< endl;
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
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->number_dam_function<<" people2risk damage function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->number_dam_function; i++){
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
	cout <<"Data-Adding of the people2risk damage function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a people2risk raster from file to database
void Dam_People_System::add_people2risk_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used){
/**A people2risk raster consists of two rasters:
	- a raster containing the categories of people2risk damage types.
	- a raster containing the population density of the element, representing the stock value.

	Both rasters are required.
*/
	try{
		if(internaly_used==false){
			this->set_start_warnings_number();
		}
		ostringstream cout;
		cout <<"Add the data of the people2risk damage raster to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_raster_type!=label::not_set && this->file_raster_pop_dens!=label::not_set){
			//read the data in
			this->read_people2risk_raster_per_file(1, &this-> file_raster_type, &file_raster_pop_dens);
			Dam_Damage_System::check_stop_thread_flag();
			//compare with existing damage raster in databse
			Dam_People_Raster *raster_buffer=NULL;
			QSqlTableModel results(0, *ptr_database);
			int number_buff_rast=Dam_People_Raster::select_relevant_raster_database(&results, ptr_database, false);
			if(number_buff_rast>0){
				raster_buffer=new Dam_People_Raster[number_buff_rast];
				_sys_system_id id_buff;
				id_buff.area_state=0;
				id_buff.measure_nr=0;
				//read them in
				for(int i=0; i< number_buff_rast; i++){
					raster_buffer[i].input_raster_perdatabase_general_data(&results, i);
				}
				//check them
				for(int i=0; i< this->number_raster; i++){
					for(int j=0; j< number_buff_rast; j++){
						if(this->raster[i].compare_raster_infos(&raster_buffer[j])==true){
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
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			//transfer it to database
			for(int i=0; i< this->number_raster; i++){
				try{
					this->raster[i].transfer_input_members2database(ptr_database);
				}
				catch(Error msg){
					Dam_People_Raster::delete_data_in_raster_table(ptr_database,this->raster[i].get_raster_number());
					Dam_People_Element::delete_data_in_elem_table(ptr_database, this->raster[i].get_raster_number());
					throw msg;
				}
			}
		}
		else{
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Filename of category information           : " << this->file_raster_type <<endl;
			info << "Filename of population density information : " << this->file_raster_pop_dens <<endl;
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
	cout <<"Data-Adding of the people2risk damage raster is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(internaly_used==false){
		this->output_error_statistic();
	}
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a people2risk damage raster from file to database for multiple raster input
void Dam_People_System::add_people2risk_raster_file2database_multi(QSqlDatabase *ptr_database){
	ostringstream prefix;
	this->set_start_warnings_number();
	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		prefix << "MULTI_"<<i+1<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		ostringstream cout;
		cout <<"Add the data of the people2risk damage raster "<<i+1<<"..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->file_raster_type=this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
		this->file_raster_pop_dens=this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();
		this->add_people2risk_raster_file2database(ptr_database, true);
		cout <<"Data import of the people2risk damage raster "<<i+1<<" is finished..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_dam->rewind_userprefix();
		this->delete_raster();
	}
	//just for the statistic output
	this->number_raster=this->number_raster_dia->get_number_raster();
	this->output_error_statistic();
}
//Delete all people2risk data in the database: raster, raster elements, damage function, results
void Dam_People_System::del_damage_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "POP> ";

	ostringstream cout;
	cout <<"Delete all people2risk damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	try{
		if(this->del_raster_flag==true){
			cout <<"POP-damage raster..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			this->delete_selected_raster_database(ptr_database);
		}
		if(this->del_func_flag==true){
			cout <<"POP-damage function..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_People_Damage_Function::delete_data_in_function_table(ptr_database);
		}
		if(this->del_res_flag==true || this->del_func_flag==true){
			cout <<"POP-damage results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_People_Element::delete_data_in_erg_table(ptr_database);
		}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of people2risk damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Ask for the file of the damage function for the people2risk calculation per dialog
bool Dam_People_System::ask_file_damage_function(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_dam_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":pop_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the poeple2risk vulnerability categories are stored."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose POP-vulnerability category file");
	my_dia.get_ptr_file_browser(0)->set_text_label("POP-vulnerability category file");

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
			txt<<"No file is set for the POP-vulnerability category!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file(s) of the people2risk raster(s) per dialog
bool Dam_People_System::ask_file_people2risk_raster(QWidget *parent){
	/**	If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The results are set to  the members
	file_raster_type , file_raster_pop_dens .*/
	Sys_Multi_Filechooser_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":pop_icon");
	my_dia.set_number_file_browser(2,icon);
	stringstream buffer;
	buffer << "Choose the file(s), where the people2risk damage-raster(s) are stored. Both rasters are required."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose POP-raster file(s)");
	my_dia.get_ptr_file_browser(0)->set_text_label("POP-damage raster file for the people2risk vulnerability category");
	my_dia.get_ptr_file_browser(0)->set_tooltip("The vulnerability category are connected to the elements");

	my_dia.get_ptr_file_browser(1)->set_text_label("POP-damage raster file for the population density");
	my_dia.get_ptr_file_browser(1)->set_tooltip("The population density represents the stock value of the element");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_raster_type=my_dia.get_ptr_file_browser(0)->get_file_name();
		this->file_raster_pop_dens=my_dia.get_ptr_file_browser(1)->get_file_name();
		return true;
	}
}
//Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
bool Dam_People_System::ask_file_people2risk_raster_multi(void){
	//ask the number of multiple raster
	this->allocate_number_raster_dia();
	QIcon icon;
	ostringstream txt;
	txt<<"Number of POP raster...";
	icon.addFile(":pop_icon");
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
		buffer << "Choose the file(s), where the people2risk damage-raster(s) "<<endl;
		buffer << " (" << i+1 << " of " << this->number_raster_dia->get_number_raster() << ") are stored."<< endl;
		buffer << "Both rasters are required."<< endl;
		this->mulit_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		this->mulit_raster_import_dia[i].set_window_title("Choose POP-raster file(s)");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("POP-damage raster file for the people2risk vulnerability category");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("The vulnerability category are connected to the elements");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_text_label("POP-damage raster file for the population density");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_tooltip("The population density represents the stock value of the element");

		//start dialog
		if(this->mulit_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	return true;
}
//Ask for the flag, what should be deleted (raster, damage functions, results)
bool Dam_People_System::ask_deleting_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":pop_icon");
	my_dia.set_number_check_boxes(3,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the POP-system in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting POP-system");
	my_dia.get_ptr_check_box(0)->setText("Delete POP-damage raster");
	my_dia.get_ptr_check_box(1)->setText("Delete POP-damage vulnerability categories");
	my_dia.get_ptr_check_box(2)->setText("Delete POP-damage results");

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
//Read in the people2risk damage system per database
void Dam_People_System::read_people2risk_system_per_database(QSqlDatabase *ptr_database){
	this->read_damage_function_per_database(ptr_database);
	this->read_people2risk_raster_per_database_general(ptr_database);
}
//Calculate the damages
void Dam_People_System::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id){
	ostringstream cout;
	cout << "Calculate damages for people2risk raster " <<this->raster[raster_id].get_raster_name() <<"..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster[raster_id].calculate_damages(impact_fp, number);
}
//Delete the result members for a given system-id and a scenario (boundary-, break-)
void Dam_People_System::delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_People_Element::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the result members to a database table
void Dam_People_System::output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(raster_id==this->number_raster-1 && *was_output==false){
		must_output=true;
	}
	cout << "Output people2risk damages to database of "<< this->raster[raster_id].get_raster_name() <<"..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster[raster_id].output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Read in the damage funbction for the people2risk calculation per file
void Dam_People_System::read_damage_function_per_file(const string file_dam_func){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	//read in damage function
	//open file
	ifile.open(file_dam_func.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Filename: " << file_dam_func << endl;
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
			my_stream >> this->number_dam_function;
			if(my_stream.fail()==true || this->number_dam_function<=0){
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
		info << "Filename              : "<< file_dam_func << endl;
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
		for(int i=0; i< this->number_dam_function; i++){
			this->dam_function[i].read_function(&ifile, &line_counter);
		}
		this->check_damage_function();
	}
	catch(Error msg){
		ostringstream info;
		info << "Filename              : "<< file_dam_func << endl;
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
//Read in the people2risk damage functions from database
void Dam_People_System::read_damage_function_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the people2risk damage function per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->number_dam_function=Dam_People_Damage_Function::select_relevant_functions_database(&results);
		this->allocate_damage_function();
		for(int i=0; i< this->number_dam_function; i++){
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
//Read in the people2risk raster from file
void Dam_People_System::read_people2risk_raster_per_file(const int no, string *fname_type, string *fname_people){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	try{
		this->number_raster=no;

		//allocate the raster
		this->allocate_raster();

		for(int i=0; i< this->number_raster; i++){
			this->raster[i].set_raster_number(i);
			this->raster[i].read_all_damage_raster_per_file(fname_type[i], fname_people[i]);
		}
		this->check_raster();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the general people2risk raster from database
void Dam_People_System::read_people2risk_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "POP> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the general people2risk damage raster per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		QSqlQueryModel results;
		//number of function
		this->number_raster=Dam_People_Raster::select_relevant_raster_database(&results, ptr_database);
		this->allocate_raster();
		for(int i=0; i< this->number_raster; i++){
			this->raster[i].input_raster_perdatabase_general_data(&results, i);
		}
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
//Ask for raster to handled in a dialog
bool Dam_People_System::ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	DamGui_Raster_Dia my_dia(parent);

	this->read_people2risk_raster_per_database_general(ptr_database,false);

	if(my_dia.start_dialog(this->raster, this->number_raster)==true){
		return true;
	}
	else{
		return false;
	}
}
//Get a text for deleting people2risk information in database
string Dam_People_System::get_deleting_text(void){
	ostringstream buffer;
	if(this->del_raster_flag==true){
		buffer << " The raster(s) will be deleted in database, also the results in the raster!"<<endl;
	}
	if(this->del_func_flag==true){
		buffer << " All people2risk categories will be deleted in database, also all results!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//Output the members
void Dam_People_System::output_members(void){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->number_raster>0){
		cout << "Output "<<this->number_raster<<" raster(s) of people2risk..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_raster; i++){
			this->raster[i].output_member();
		}
	}

	if(this->number_dam_function>0){
		cout << "Output " << this->number_dam_function << " people2risk damage functions..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_dam_function; i++){
			this->dam_function[i].output_member();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of people2risk damage system
void Dam_People_System::output_statistic(void){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"PEOPLE2RISK RASTER"<<endl;
	cout << " Number             " << this->number_raster << endl;
	cout<<"PEOPLE2RISK DAMAGE FUNCTION"<<endl;
	cout << " Number             " << this->number_dam_function << endl;

	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the sum over the raster of the damage results to display/console
void Dam_People_System::output_result_damage(void){
	ostringstream prefix;

	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	for(int i=0; i< this->number_raster; i++){
		this->raster[i].output_result_damage();
	}

	ostringstream cout;
	cout << "TOTAL RESULTS"<<endl;
	cout << " Affected               : " << P(2)<< FORMAT_FIXED_REAL << this->result_affected_pop << label::person<< endl;
	cout << " Endangered             : " <<  this->result_endangered_pop  << label::person << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the results of the affected people
double Dam_People_System::get_affected_pop_results(void){
	return this->result_affected_pop;
	}
//Get the results of the endangered people
double Dam_People_System::get_endangered_pop_results(void){
	return this->result_endangered_pop;
}
//Initialize the people2risk raster(s)
void Dam_People_System::init_damage_rasters(const int index, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	this->raster[index].input_raster_perdatabase_element_data(ptr_database, id);
	this->connect_elems2functions(index);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Reduce area of intercepted elements
void Dam_People_System::reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	try{
		cout <<"Check the interception of the risk2people rasters..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->number_raster-1; i++){
			cout <<"Intercept people2risk raster "<<this->raster[i].get_raster_name()<< " with the other raster(s).." <<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//check if there is an interception
			bool intercept_flag=false;
			for(int j=0; j< this->number_raster; j++){
				if(j>i){
					intercept_flag=this->raster[i].check_interception(&this->raster[j]);
					if(intercept_flag==true){
						break;
					}
				}
			}

			if(intercept_flag==true){
				this->raster[i].input_raster_perdatabase_element_data(ptr_database, id);
				prefix <<"RAST_"<<this->raster[i].get_raster_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);
				for(int j=0; j< this->number_raster; j++){
					Dam_Damage_System::check_stop_thread_flag();
					if(j>i){
						this->raster[i].set_intercepted_elem2reduced_area(&this->raster[j], ptr_database, id);
					}
				}

				this->raster[i].transfer_intercepted_elem_data2database(ptr_database);
				Sys_Common_Output::output_dam->rewind_userprefix();
				this->raster[i].clear_raster();
			}
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Intercept people2risk damage elements with the hydraulic elements
void Dam_People_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	cout <<"Check the interception of "<<this->number_raster<<" people2risk raster(s) to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int *rang=NULL;
	cout <<"Sort hydraulic floodplain models..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);

	try{
		for(int i=0; i< this->number_raster; i++){
			this->raster[i].input_raster_perdatabase_element_data(ptr_database, id);
			prefix <<"RAST_"<<this->raster[i].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int j=0; j< number_fp; j++){
				cout <<"Check the interception of people2risk raster "<<this->raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
				Sys_Common_Output::output_dam->output_txt(&cout);
				prefix <<"FP_"<<fp_models[rang[j]].Param_FP.get_floodplain_number()<<"> ";
				Sys_Common_Output::output_dam->set_userprefix(&prefix);

				if(this->raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::complete_outside ||
					this->raster[i].geometrical_bound.check_polygon_interception(&(fp_models[rang[j]].raster.geometrical_bound))==_polygon_intercept::outside_with_boundary){
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
					cout <<"Generate elements, raster points and segments for the interception of people2risk raster "<<this->raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].input_elems_database(ptr_database, &bound_result,number_bound, false, true);
				}

				this->raster[i].set_intercepted_elem2floodplain_id(&fp_models[rang[j]]);

				if(delete_again==true){
					cout <<"Delete elements, raster points and segments of the hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
					fp_models[rang[j]].delete_elems_raster_geo();
				}
				Dam_Damage_System::check_stop_thread_flag();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
			this->raster[i].transfer_intercepted_elem_data2database(ptr_database);
			this->raster[i].clear_raster();
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
void Dam_People_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Transfer the data from the interception of "<<this->number_raster<<" people2risk raster(s) to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->number_raster; i++){
		this->raster[i].transfer_intercepted_elem_data2database(ptr_database);
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_People_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	this->result_affected_pop=0.0;
	this->result_endangered_pop=0.0;

	for(int i=0; i< this->number_raster; i++){
		this->raster[i].sum_total_results(ptr_database, id, bound_sz, break_sz);
		//sum it up
		this->result_affected_pop=this->result_affected_pop+this->raster[i].get_affected_pop_results();
		this->result_endangered_pop=this->result_endangered_pop+this->raster[i].get_endangered_pop_results();
	}
}
//Check the people2risk damage system
void Dam_People_System::check_system(void){
	if(this->number_dam_function==0 && this->number_raster!=0){
		Warning msg=this->set_warning(0);
		msg.output_msg(4);
	}
	if(this->number_dam_function!=0 && this->number_raster==0){
		Warning msg=this->set_warning(1);
		msg.output_msg(4);
	}
	if(this->number_raster!=0){
		bool flag=true;
		for(int i=0; i<this->number_raster; i++){
			//if raster are connected
			if(this->raster[i].get_connected_flag()==false){
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
//Intercept the people2risk damage raster with the given polygons and change the people2risk category
void Dam_People_System::intercept_polygons2category_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	bool intercept_flag=false;
	ostringstream prefix;
	ostringstream cout;
	prefix << "POP> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	//check if there are any polygon interception with the rasters
	for(int j=0; j< this->number_raster; j++){
		intercept_flag=false;
		for(int i=0; i< number_polys; i++){
			intercept_flag=this->raster[j].check_interception(&polys[i]);
			if(intercept_flag==true){
				break;
			}
		}
		if(intercept_flag==true){
			cout <<"Intercept people2risk raster "<<this->raster[j].get_raster_name()<< " with the polygon(s)..." <<endl;
			this->raster[j].input_raster_perdatabase_element_data(ptr_database, id);
			Sys_Common_Output::output_dam->output_txt(&cout);
			prefix <<"RAST_"<<this->raster[j].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);
			for(int i=0; i< number_polys; i++){
				if(this->raster[j].check_interception(&polys[i])==true){
					this->raster[j].set_intercepted_elem2new_category(ptr_database, id, &polys[i], current_id, new_id);
				}
			}

			Sys_Common_Output::output_dam->rewind_userprefix();
			this->raster[j].clear_raster();
		}
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the flag if the raster are to delete
bool Dam_People_System::get_raster2delete(void){
	return this->del_raster_flag;
}
//__________
//private
//Connect the element an their functions
void Dam_People_System::connect_elems2functions(const int index){
	ostringstream cout;
	cout <<"Connect the elements of people2risk raster " << this->raster[index].get_raster_name() << " to " << this->number_dam_function <<" available risk2people damage function(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->raster[index].connect_dam_function2elems(this->number_dam_function, this->dam_function);

	//check them
	cout <<"Check the connection of the risk2people raster elements to the damage-functions..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->raster[index].check_damage_function_connection();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
}
//Allocate the people2risk damage function
void Dam_People_System::allocate_damage_function(void){
	try{
		this->dam_function=new Dam_People_Damage_Function[this->number_dam_function];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the people2risk damage function
void Dam_People_System::delete_damage_function(void){
	if(this->dam_function!=NULL){
		delete []this->dam_function;
		this->dam_function=NULL;
	}
}
//Allocate the people2risk raster
void Dam_People_System::allocate_raster(void){
	try{
		this->raster=new Dam_People_Raster[this->number_raster];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the people2risk raster
void Dam_People_System::delete_raster(void){
	if(this->raster!=NULL){
		delete []this->raster;
		this->raster=NULL;
	}
}
//Check the if there are same raster
void Dam_People_System::check_raster(void){
	for(int i=0; i< this->number_raster; i++){
		for(int j=0; j< this->number_raster; j++){
			if(j>i){
				if(this->raster[i].compare_raster_infos(&this->raster[j])==true){
					Error msg=this->set_error(4);
					ostringstream info;
					info <<"1. Rasterno.  : "<< i << endl;
					info <<"1. Rastername : "<< this->raster[i].get_raster_name() << endl;
					info <<"2. Rasterno.  : "<< j << endl;
					info <<"2. Rastername : "<< this->raster[j].get_raster_name() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
}
//Check the damage function
void Dam_People_System::check_damage_function(void){
	for(int i=0; i< this->number_dam_function; i++){
		for(int j=0; j<this->number_dam_function; j++){
			if(i!=j){
				if(this->dam_function[i].get_index()==this->dam_function[j].get_index()){
					Error msg=this->set_error(5);
					ostringstream info;
					info <<"Index of the landuse type   : " << this->dam_function[i].get_index() << endl;
					info <<"Name of the 1. landuse type : " << this->dam_function[i].get_name() << endl;
					info <<"Name of the 2. landuse type : " << this->dam_function[j].get_name() << endl;
					throw msg;
				}
			}
		}
	}
}
//Output the error-statistic of the system
void Dam_People_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "POP> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the POP-system..." << endl ;
	cout << "GENERAL"<< endl;
	if(this->number_dam_function>0){
		cout << " Number damage function(s):" << W(3) << this->number_dam_function <<endl;
	}
	if(this->number_raster>0){
		cout << " Number raster(s)         :" << W(3) << this->number_raster <<endl;
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
void Dam_People_System::delete_selected_raster_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->number_raster ; i++){
		if(this->raster[i].get_is_selected()==true){
			Dam_People_Raster::delete_data_in_raster_table(ptr_database, this->raster[i].get_raster_number());
			Dam_People_Element::delete_data_in_elem_table(ptr_database, this->raster[i].get_raster_number());
			Dam_People_Element::delete_data_in_erg_table_per_raster(ptr_database, this->raster[i].get_raster_number());
		}
	}
}
//Set the warning
Warning Dam_People_System::set_warning(const int warn_type){
	string place="Dam_People_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="People2risk damage raster are set, but no damage functions";
			reaction="Can not calculate any people2risk damages";
			help= "Check the people2risk damage functions";
			type=16;
			break;
		case 1://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="People2risk damage functions are set, but no damage raster";
			reaction="Can not calculate any people2risk damages";
			help= "Check the people2risk damage raster";
			type=16;
			break;
		case 2://not all rasters are connected
			place.append("check_system(void)") ;
			reason="Not all people2risk damage rasters are connected to the hydraulic system";
			help= "Connect the people2risk damage raster to avoid errors in the damage results";
			type=16;
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
Error Dam_People_System::set_error(const int err_type){
	string place="Dam_People_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_raster(void)/allocate_damage_function(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://can not open damage function file
			place.append("read_damage_function_per_file(const string file_dam_func)");
			reason="Can not open the file of the damage functions";
			help="Check the file";
			type=5;
			break;
		case 2://do not find number of damage function in file
			place.append("read_damage_function_per_file(const string file_dam_func)");
			reason="Do not find the keyword for the number of damage functions in file !NO_FUNCTIONS";
			help="The number of damage functions must be declared before the individual damage function data begins";
			type=1;
			break;
		case 3://wrong input
			place.append("read_damage_function_per_file(const string file_dam_func)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the damage functions in file";
			type=1;
			break;
		case 4://there can't be same raster
			place.append("check_raster(void)");
			reason="There are same raster in the database";
			help="Check the geometrical settings of the people2risk raster";
			type=22;
			break;
		case 5://there damage functions with the same index
			place.append("check_damage_function(void)");
			reason="There are damage-function (land-use type) with the same index";
			help="Check the people2risk damage functions";
			type=22;
			break;
		case 6://no adding of identical raster
			place.append("add_damage_raster_file2database(QSqlDatabase *ptr_database)");
			reason="You can not add raster with identical general information like the geometry";
			help="Check the raster for adding";
			type=22;
			break;
		case 7://not all required rasters are set
			place.append("add_people2risk_raster_file2database(QSqlDatabase *ptr_database)") ;
			reason="Not all required people2risk damage raster file(s) are set. The raster is not transferd to database";
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