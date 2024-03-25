#include "Dam_Headers_Precompiled.h"
//#include "Dam_Pys_System.h"

//Default constructor
Dam_Pys_System::Dam_Pys_System(void){
	this->category=NULL;
	this->no_category=0;
	this->no_rasters=0;
	this->raster=NULL;

	this->file_density_func=label::not_set;
	this->file_category_func=label::not_set;
	this->file_density_raster=label::not_set;
	this->file_crit_50_59_raster=label::not_set;
	this->file_crit_80_raster=label::not_set;
	this->file_crit_women_raster=label::not_set;
	this->file_crit_home_owner_raster=label::not_set;
	this->file_category_raster=label::not_set;

	this->del_raster_flag=true;
	this->del_densfunc_flag=true;
	this->del_res_flag=true;
	this->del_cat_flag=true;

	this->reset_result_value();

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Pys_System)-sizeof(Dam_Pys_Density_Function), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Pys_System::~Dam_Pys_System(void){
	this->delete_categories();
	this->delete_rasters();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Pys_System)-sizeof(Dam_Pys_Density_Function), _sys_system_modules::DAM_SYS);
}
//____________
//public
//Get the number of raster of the psycho-social system
int Dam_Pys_System::get_number_raster(void){
	return this->no_rasters;
}
//Get the number of categories of the psycho-social system
int Dam_Pys_System::get_number_categories(void){
	return this->no_category;
}
//Import the psycho-social density-function from a file to the database
void Dam_Pys_System::import_density_function_file2database(QSqlDatabase *ptr_database){
/**If a density function already exists in the database, it is deleted.
\see Dam_Pys_Density_Function
*/
	try{
		this->set_start_warnings_number();
		//first delete all function data
		Dam_Pys_Density_Function::delete_data_in_point_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the psycho-social density function to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_density_func!=label::not_set){
			//read the data in
			this->read_density_function_per_file(this->file_density_func);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->density_func.output_member();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of psycho-social density function to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			this->density_func.transfer_input_members2database(ptr_database);
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
	cout <<"Data-Import of the psycho-social density function is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Import the psycho-social categories from a file to the database
void Dam_Pys_System::import_categories_file2database(QSqlDatabase *ptr_database){
/**Other already existing psycho-ocial categories in the database are deleted.
	The category contqains for each criteria one score-function.
	\see Dam_Pys_Category
*/
	try{
		this->set_start_warnings_number();
		//first delete all biotope-types
		Dam_Pys_Category::delete_data_in_table(ptr_database);

		ostringstream cout;
		cout <<"Import the data of the psycho-social categories to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_category_func!=label::not_set){
			//read the data in
			this->read_category_per_file(this->file_category_func);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			for(int i=0; i< this->no_category; i++){
				this->category[i].output_member();
			}
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->no_category<<" psycho-social categories to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			int glob_id;
			glob_id=Dam_Pys_Category::get_max_glob_id_category_table(ptr_database)+1;
			//make a table-model
			QSqlTableModel model(0,*ptr_database);
			for(int i=0; i< this->no_category; i++){
				if(i%10==0 && i > 0){
					cout << "Transfer categories "<< i <<" to database" << endl;
					Sys_Common_Output::output_dam->output_txt(&cout);
				}
				this->category[i].transfer_input_members2database(glob_id,&model,ptr_database);
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
	cout <<"Data-Import of the psyco-social categories is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add the psycho-social categories from a file to the database (the existing one are not deleted)
void Dam_Pys_System::add_categories_file2database(QSqlDatabase *ptr_database){
	try{
		this->set_start_warnings_number();

		ostringstream cout;
		cout <<"Add the data of the psycho-social categories to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "ADD> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_category_func!=label::not_set){
			//read the data in
			this->read_category_per_file(this->file_category_func);
			Dam_Damage_System::check_stop_thread_flag();
			//output the members
			this->output_members();
			Dam_Damage_System::check_stop_thread_flag();
			cout <<"Transfer the data of "<< this->no_category<<" psycho-social categories to the database ..."<< endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//transfer it to database
			for(int i=0; i< this->no_category; i++){
				this->category[i].add_input_members2database(ptr_database);
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
	cout <<"Data-Adding of the psycho-social categorie(s) is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->output_error_statistic();
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a psycho-social damage raster from file to database
void Dam_Pys_System::add_damage_raster_file2database(QSqlDatabase *ptr_database, const bool internaly_used){
/**A psycho-social raster consists of one to six rasters:
	- one raster containing the population density information per element. This raster is optional, if no
	raster is set, the density factor is one.
	- one raster containing the psycho-social category, which provides for each criteria one sore-function (Dam_Pys_Category).
	- four raster containing the information of each criteria. Four criteria are implement: age class 50 to 59 years,
	age class greater 80, female gender and home owner. If more criteria are required the developer has to expand the source code.
	This rasters are also optional. If nothing is set the criteria value per elment is set to zero.
*/
	try{
		if(internaly_used==false){
			this->set_start_warnings_number();
		}
		ostringstream cout;
		cout <<"Add the data of the psycho-social damage raster to the database ..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		//set prefix
		ostringstream prefix;
		prefix << "IMPO> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		if(this->file_category_raster!=label::not_set){
			//read the data in
			this->read_raster_per_file(1, &this->file_density_raster, &this->file_category_raster, &this->file_crit_50_59_raster, &this->file_crit_80_raster, &this->file_crit_women_raster,&this->file_crit_home_owner_raster );
			Dam_Damage_System::check_stop_thread_flag();

			//compare with existing damage raster in database
			Dam_Pys_Raster *raster_buffer=NULL;
			QSqlQueryModel results;
			int number_buff_rast=0;
			number_buff_rast=Dam_Pys_Raster::select_relevant_raster_database(&results,ptr_database, false);
			if(number_buff_rast>0){
				raster_buffer=new Dam_Pys_Raster[number_buff_rast];
				_sys_system_id id_buff;
				id_buff.area_state=0;
				id_buff.measure_nr=0;
				//read them in
				for(int i=0; i< number_buff_rast; i++){
					raster_buffer[i].input_raster_perdatabase_general_data(&results, i);
				}
				//check them
				for(int i=0; i< this->no_rasters; i++){
					for(int j=0; j< number_buff_rast; j++){
						if(this->raster[i].compare_raster_infos(&raster_buffer[j])==true){
							Error msg=this->set_error(2);
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
			for(int i=0; i< this->no_rasters; i++){
				this->raster[i].output_member();
			}
			Dam_Damage_System::check_stop_thread_flag();
			//transfer it to database
			for(int i=0; i< this->no_rasters; i++){
				try{
					this->raster[i].transfer_input_members2database(ptr_database);
				}
				catch(Error msg){
					Dam_Pys_Raster::delete_data_in_raster_table(ptr_database,this->raster[i].get_raster_number());
					Dam_Pys_Element::delete_data_in_elem_table(ptr_database, this->raster[i].get_raster_number());
					throw msg;
				}
			}
		}

		else{
			Error msg=this->set_error(9);
			ostringstream info;
			info << "Filename of category information           : " << this->file_category_raster <<endl;
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
	cout <<"Data-Adding of the psycho-social damage raster is finished"<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	if(internaly_used==false){
		this->output_error_statistic();
	}
	//rewind the prefix
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Add a psycho-social damage raster from file to database for multiple raster input
void Dam_Pys_System::add_damage_raster_file2database_multi(QSqlDatabase *ptr_database){
	ostringstream prefix;
	this->set_start_warnings_number();
	for(int i=0; i<this->number_raster_dia->get_number_raster(); i++){
		prefix << "MULTI_"<<i+1<<"> ";
		Sys_Common_Output::output_dam->set_userprefix(&prefix);
		ostringstream cout;
		cout <<"Add the data of the psycho-social damage raster "<<i+1<<"..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		this->file_density_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->get_file_name();
		this->file_category_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->get_file_name();
		this->file_crit_50_59_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->get_file_name();
		this->file_crit_80_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->get_file_name();
		this->file_crit_women_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(4)->get_file_name();
		this->file_crit_home_owner_raster=this->mulit_raster_import_dia[i].get_ptr_file_browser(5)->get_file_name();

		this->add_damage_raster_file2database(ptr_database, true);
		cout <<"Data import of the psycho-social damage raster "<<i+1<<" is finished..."<< endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		Sys_Common_Output::output_dam->rewind_userprefix();
		this->delete_rasters();
	}
	//just for the statistic output
	this->no_rasters=this->number_raster_dia->get_number_raster();
	this->output_error_statistic();
}
//Delete all psycho-social data in the database: raster, raster elements, categories, score functions, results etc.
void Dam_Pys_System::del_damage_data_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "PYS> ";

	ostringstream cout;
	cout <<"Delete all psycho-social damage information..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	//ecn
	try{
		if(this->del_raster_flag==true){
			cout <<"PYS-damage raster..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Pys_Raster::delete_data_in_raster_table(ptr_database);
			Dam_Pys_Element::delete_data_in_elem_table(ptr_database);
		}
		if(this->del_densfunc_flag==true){
			cout <<"PYS-damage density function..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Pys_Density_Function::delete_data_in_point_table(ptr_database);
		}
		if(this->del_cat_flag==true){
			cout <<"PYS-damage category..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Pys_Category::delete_data_in_table(ptr_database);
		}
		if(this->del_res_flag==true){
			cout <<"PYS-damage results..."<<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			Dam_Pys_Element::delete_data_in_erg_table(ptr_database);
		}
	}
	catch(Error msg){
		msg.output_msg(4);
	}
	cout <<"Deleting of psycho-social damage information is finished!"<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the people-density function functions from file
void Dam_Pys_System::read_density_function_per_file(const string file_func){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;

	//read in density function
	//open file
	ifile.open(file_func.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(3);
		ostringstream info;
		info << "Filename: " << file_func << endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	try{
		this->density_func.read_function(&ifile, &line_counter);
	}
	catch(Error msg){
		ostringstream info;
		info << "Filename              : "<<  file_func << endl;
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
//Read in the people-density function from database
void Dam_Pys_System::read_density_function_per_database(QSqlDatabase *ptr_database){
	this->density_func.input_function_perdatabase(ptr_database);
}
//Read in the raster of the category from file
void Dam_Pys_System::read_raster_per_file(const int no, string *fname_pop, string *fname_cat, string *fname_crit_50_59, string *fname_crit_80, string *fname_crit_female, string *fname_crit_owner){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	try{
		this->no_rasters=no;

		//allocate the raster
		this->allocate_rasters();

		for(int i=0; i< this->no_rasters; i++){
			this->raster[i].set_raster_number(i);
			this->raster[i].read_all_damage_raster_per_file(fname_pop[i], fname_cat[i], fname_crit_50_59[i], fname_crit_80[i], fname_crit_female[i], fname_crit_owner[i]);
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the general information of the psycho-social raster from database
void Dam_Pys_System::read_damage_raster_per_database_general(QSqlDatabase *ptr_database, const bool with_output){
	ostringstream prefix;
	if(with_output==true){
		prefix << "PYS> ";
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());

		ostringstream cout;
		cout << "Read in the general damage raster information per database..." << endl ;
		Sys_Common_Output::output_dam->output_txt(&cout);
	}

	try{
		QSqlQueryModel results;
		//number of raster
		this->no_rasters=Dam_Pys_Raster::select_relevant_raster_database(&results, ptr_database);
		this->allocate_rasters();
		for(int i=0; i< this->no_rasters; i++){
			this->raster[i].input_raster_perdatabase_general_data(&results, i);
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
//Read in the psycho-social category from file
void Dam_Pys_System::read_category_per_file(const string file_category){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ifstream ifile;
	int line_counter=0;
	string myline;
	int pos=-1;

	try{
		//read in biotope-type
		//open file
		ifile.open(file_category.c_str(), ios_base::in);
		if(ifile.is_open()==false){
			Error msg=this->set_error(5);
			ostringstream info;
			info << "Filename: " << file_category << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//search for number of damage function if file
		bool no_func_found=false;
		do{
			getline(ifile, myline,'\n');
			line_counter++;
			functions::clean_string(&myline);

			pos=myline.rfind("!NO_CATEGORY");
			if(pos>=0){
				stringstream my_stream;
				myline=myline.substr(pos+12);
				functions::clean_string(&myline);
				my_stream << myline;
				my_stream >> this->no_category;
				if(my_stream.fail()==true || this->no_category<=0){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(6);
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
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Error occurs near line: "<< line_counter << endl;
			info << "Filename              : "<< file_category << endl;
			msg.make_second_info(info.str());
			ifile.close();
			throw msg;
		}

		//allocate the categories
		try{
			this->allocate_categories();
		}
		catch(Error msg){
			ifile.close();
			throw msg;
		}

		try{
			//read in the categories
			for(int i=0; i< this->no_category; i++){
				this->category[i].read_category_per_file(&ifile, &line_counter);
			}
			this->check_categories();
		}
		catch(Error msg){
			ostringstream info;
			info << "Filename              : "<< file_category << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	catch(Error msg){
		//close file and reset it
		ifile.close();
		ifile.clear();
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	//close file and reset it
	ifile.close();
	ifile.clear();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the psycho-social category from database
void Dam_Pys_System::read_category_per_database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Read in the psycho-social categories per database..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);

	try{
		QSqlTableModel results(0, *ptr_database);
		//number of function
		this->no_category=Dam_Pys_Category::select_relevant_category_database(&results);
		this->allocate_categories();
		for(int i=0; i< this->no_category; i++){
			this->category[i].input_category_perdatabase(&results, i);
		}
		this->check_categories();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Read in the psycho-social damage system per database
void Dam_Pys_System::read_system_per_database(QSqlDatabase *ptr_database){
	this->read_category_per_database(ptr_database);
	this->read_density_function_per_database(ptr_database);
	this->read_damage_raster_per_database_general(ptr_database);
}
//Ask for the file of the density function per dialog
bool Dam_Pys_System::ask_file_density_function(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_density_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":pys_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the psycho-social density-function is stored."<< endl;
	buffer << "If it is not set the density weihgting is not applied."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose PYS-density function file");
	my_dia.get_ptr_file_browser(0)->set_text_label("PYS-density function file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_density_func=my_dia.get_ptr_file_browser(0)->get_file_name();
		if(this->file_density_func==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the PYS-density function!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}
		return true;
	}
}
//Ask for the file of the psycho-social categories per dialog
bool Dam_Pys_System::ask_file_categories(QWidget *parent){
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_category_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":pys_icon");
	my_dia.set_number_file_browser(1,icon);
	stringstream buffer;
	buffer << "Choose the file, where the psycho-social category(ies) are stored."<< endl;
	buffer << "This file contains the score function for each criteria for each category"<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose PYS-category file");
	my_dia.get_ptr_file_browser(0)->set_text_label("PYS-category file");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_category_func=my_dia.get_ptr_file_browser(0)->get_file_name();

		if(this->file_category_func==label::not_set){
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt<<"No file is set for the PYS-category!"<< endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		return true;
	}
}
//Ask for the file(s) of the damage raster(s) per dialog
bool Dam_Pys_System::ask_file_damage_raster(QWidget *parent){
	/**	If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The results are set to  the members
	this->file_density_raster , this->file_category_raster , this->file_crit_50_59_raster , this->file_crit_women_raster,
	this->file_crit_home_owner_raster. If more criteria are required insert here the needed dialogs.*/
	Sys_Multi_Filechooser_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":pys_icon");
	my_dia.set_number_file_browser(6,icon);
	stringstream buffer;
	buffer << "Choose the file(s), where the psycho-social damage-raster(s) are stored."<< endl;
	buffer << "The files, which contains the criteria values and the population density are optional. \n If nothing is set a zero value is applied for every case."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose PYS-raster file(s)");
	my_dia.get_ptr_file_browser(0)->set_text_label("PYS-damage raster file for the population density [optional]");
	my_dia.get_ptr_file_browser(0)->set_tooltip("The population density is important for an weighting of the scores via the population density function");

	my_dia.get_ptr_file_browser(1)->set_text_label("PYS-damage raster file for the categories and their score-functions");
	my_dia.get_ptr_file_browser(1)->set_tooltip("Each category provides a individual score function for each criteria. The indices have to be corresponding to the given categories");

	my_dia.get_ptr_file_browser(2)->set_text_label("PYS-damage raster file for the criteria age class 50-59 years [optional]");
	my_dia.get_ptr_file_browser(2)->set_tooltip("If this raster is not set, the percentage of this age class is set to zero");

	my_dia.get_ptr_file_browser(3)->set_text_label("PYS-damage raster file for the criteria age class >80 years [optional]");
	my_dia.get_ptr_file_browser(3)->set_tooltip("If this raster is not set, the percentage of this age class is set to zero");

	my_dia.get_ptr_file_browser(4)->set_text_label("PYS-damage raster file for the criteria gender female [optional]");
	my_dia.get_ptr_file_browser(4)->set_tooltip("If this raster is not set, the percentage of this criteria is set to zero");

	my_dia.get_ptr_file_browser(5)->set_text_label("PYS-damage raster file for the criteria home owner [optional]");
	my_dia.get_ptr_file_browser(5)->set_tooltip("If this raster is not set, the percentage of this criteria is set to zero");

	//insert more criteria is required

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->file_density_raster=my_dia.get_ptr_file_browser(0)->get_file_name();
		this->file_category_raster=my_dia.get_ptr_file_browser(1)->get_file_name();
		this->file_crit_50_59_raster=my_dia.get_ptr_file_browser(2)->get_file_name();
		this->file_crit_80_raster=my_dia.get_ptr_file_browser(3)->get_file_name();
		this->file_crit_women_raster=my_dia.get_ptr_file_browser(4)->get_file_name();
		this->file_crit_home_owner_raster=my_dia.get_ptr_file_browser(5)->get_file_name();
		return true;
	}
}
//Ask for the file(s) of the damage raster(s) per dialog for multiple raster input
bool Dam_Pys_System::ask_file_damage_raster_multi(void){
	//ask the number of multiple raster
	this->allocate_number_raster_dia();
	QIcon icon;
	ostringstream txt;
	txt<<"Number of PYS raster...";
	icon.addFile(":pys_icon");
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
		this->mulit_raster_import_dia[i].set_number_file_browser(6,icon);
		stringstream buffer;
		buffer << "Choose the file(s), where the psycho-social damage-raster(s)"<<endl;
		buffer << " (" << i+1 << " of " << this->number_raster_dia->get_number_raster() << ") are stored."<< endl;
		buffer << "The files, which contains the criteria values and the population density are optional. \n If nothing is set a zero value is applied for every case."<< endl;
		this->mulit_raster_import_dia[i].set_main_text_label(buffer.str());
		buffer.str("");
		this->mulit_raster_import_dia[i].set_window_title("Choose PYS-raster file(s)");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_text_label("PYS-damage raster file for the population density [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(0)->set_tooltip("The population density is important for an weighting of the scores via the population density function");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_text_label("PYS-damage raster file for the categories and their score-functions");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(1)->set_tooltip("Each category provides a individual score function for each criteria. The indices have to be corresponding to the given categories");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->set_text_label("PYS-damage raster file for the criteria age class 50-59 years [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(2)->set_tooltip("If this raster is not set, the percentage of this age class is set to zero");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->set_text_label("PYS-damage raster file for the criteria age class >80 years [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(3)->set_tooltip("If this raster is not set, the percentage of this age class is set to zero");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(4)->set_text_label("PYS-damage raster file for the criteria gender female [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(4)->set_tooltip("If this raster is not set, the percentage of this criteria is set to zero");

		this->mulit_raster_import_dia[i].get_ptr_file_browser(5)->set_text_label("PYS-damage raster file for the criteria home owner [optional]");
		this->mulit_raster_import_dia[i].get_ptr_file_browser(5)->set_tooltip("If this raster is not set, the percentage of this criteria is set to zero");
		//start dialog
		if(this->mulit_raster_import_dia[i].start_dialog()==false){
			return false;
		}
	}

	return true;
}
//Ask for the flag, what should be deleted (raster, density functions, categories, results)
bool Dam_Pys_System::ask_deleting_flag(QWidget *parent){
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":pys_icon");
	my_dia.set_number_check_boxes(4,icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the PYS-system in the database..."<< endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting PYS-system");
	my_dia.get_ptr_check_box(0)->setText("Delete PYS-damage raster");
	my_dia.get_ptr_check_box(1)->setText("Delete PYS-density function");
	my_dia.get_ptr_check_box(2)->setText("Delete PYS-category");
	my_dia.get_ptr_check_box(3)->setText("Delete PYS-damage results");

	//start dialog
	if(my_dia.start_dialog()==false){
		return false;
	}
	else{
		this->del_raster_flag=my_dia.get_bool_check_box(0);
		this->del_densfunc_flag=my_dia.get_bool_check_box(1);
		this->del_cat_flag=my_dia.get_bool_check_box(2);
		this->del_res_flag=my_dia.get_bool_check_box(3);
		return true;
	}
}
//Ask for raster to handled in a dialog
bool Dam_Pys_System::ask_raster2handled_dialog(QSqlDatabase *ptr_database, QWidget *parent){
	DamGui_Raster_Dia my_dia(parent);

	this->read_damage_raster_per_database_general(ptr_database,false);

	if(my_dia.start_dialog(this->raster, this->no_rasters)==true){
		return true;
	}
	else{
		return false;
	}
}
//Get a text for deleting psycho-social information in database
string Dam_Pys_System::get_deleting_text(void){
	ostringstream buffer;
	if(this->del_raster_flag==true){
		buffer << " The raster(s) will be deleted in database, also the results in the raster!"<<endl;
	}
	if(this->del_densfunc_flag==true){
		buffer << " The density function will be deleted in database, also all results!"<<endl;
	}
	if(this->del_cat_flag==true){
		buffer << " All psycho-social categories will be deleted in database, also all results!"<<endl;
	}
	if(this->del_res_flag==true){
		buffer << " All results will be deleted in database"<<endl;
	}

	return buffer.str();
}
//Output the members
void Dam_Pys_System::output_members(void){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	if(this->no_category>0){
		cout << "Output "<<this->no_category<<" psycho-social category(ies)..." << endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->no_category; i++){
			this->category[i].output_member();
		}
	}

	//density functions
	cout << "Output the given function for the people-density factor..."<<endl;
	this->density_func.output_member();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of psycho-social system
void Dam_Pys_System::output_statistic(void){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"DAMAGE RASTER"<<endl;
	cout << " Number             " << this->no_rasters << endl;
	cout<<"DAMAGE CATEGORIES"<<endl;
	cout << " Number             " << this->no_category << endl;
	cout<<"DENSITY FUNCTION"<<endl;
	cout << " Is set             " << functions::convert_boolean2string(this->density_func.get_function_is_set()) << endl;

	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output the sum over the raster of the damage results to display/console
void Dam_Pys_System::output_result_damage(void){
	ostringstream prefix;

	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	for(int i=0; i< this->no_rasters; i++){
		this->raster[i].output_result_damage();
	}

	ostringstream cout;
	cout << "TOTAL RESULTS"<<endl;
	cout << " WITHOUT DENSITY FACTOR"<<endl;
	cout << "  Age 50-59 a               : " << P(2)<< FORMAT_FIXED_REAL << this->res_crit_age50_59.score_value << label::score<< endl;
	cout << "  Age > 80 a                : " << this->res_crit_age80.score_value << label::score<< endl;
	cout << "  Female gender             : " << this->res_crit_female.score_value << label::score<< endl;
	cout << "  Home owner                : " << this->res_crit_owner.score_value << label::score<< endl;
	cout << " WITH DENSITY FACTOR"<<endl;
	cout << "  Age 50-59 a               : " << this->res_crit_age50_59.score_pop_value << label::score<< endl;
	cout << "  Age > 80 a                : " << this->res_crit_age80.score_pop_value << label::score<< endl;
	cout << "  Female gender             : " << this->res_crit_female.score_pop_value << label::score<< endl;
	cout << "  Home owner                : " << this->res_crit_owner.score_pop_value << label::score<< endl;
	cout << "TOTAL"<<endl;
	cout << " WITHOUT DENSITY FACTOR     : " << this->res_total.score_value<< label::score<< endl;
	cout << " WITH DENSITY FACTOR        : " << this->res_total.score_pop_value<< label::score<< endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Get the results for the psycho-social criteria age class 50 to 59
_dam_pys_result Dam_Pys_System::get_result_crit_age50_59(void){
	return this->res_crit_age50_59;
}
//Get the results for the psycho-social criteria age class greater 80
_dam_pys_result Dam_Pys_System::get_result_crit_age80(void){
	return this->res_crit_age80;
}
//Get the results for the psycho-social criteria female gender
_dam_pys_result Dam_Pys_System::get_result_crit_female(void){
	return this->res_crit_female;
}
//Get the results for the psycho-social criteria home owner
_dam_pys_result Dam_Pys_System::get_result_crit_owner(void){
	return this->res_crit_owner;
}
//Initialize the psycho-social damage system
void Dam_Pys_System::init_damage_raster(const int index, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "INIT> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	this->raster[index].input_raster_perdatabase_element_data(ptr_database, id);
	//the density function has to be set before the connection to the raster
	this->raster[index].set_ptr_density_func(&this->density_func);
	this->connect_elems2categories(index);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Reduce area of intercepted elements of a psycho-social raster
void Dam_Pys_System::reduce_area_intercepted_elems(QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	try{
		cout <<"Check the interception of the psycho-social rasters..."<<endl;
		Sys_Common_Output::output_dam->output_txt(&cout);
		for(int i=0; i< this->no_rasters-1; i++){
			cout <<"Intercept psycho-social raster "<<this->raster[i].get_raster_name()<< " with the other raster(s).." <<endl;
			Sys_Common_Output::output_dam->output_txt(&cout);
			//check if there is an interception
			bool intercept_flag=false;
			for(int j=0; j< this->no_rasters; j++){
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
				for(int j=0; j< this->no_rasters; j++){
					Dam_Damage_System::check_stop_thread_flag();
					if(j>i){
						this->raster[i].set_intercepted_elem2reduced_area(&this->raster[j], ptr_database, id);
					}
				}

				this->raster[i].transfer_intercepted_elem_data2database(ptr_database);
				this->raster[i].clear_raster();
				Sys_Common_Output::output_dam->rewind_userprefix();
			}
		}
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Intercept psycho-social damage elements with the hydraulic elements
void Dam_Pys_System::intercept_hydraulic2damage(Hyd_Model_Floodplain *fp_models, const int number_fp, QSqlDatabase *ptr_database, const _sys_system_id id){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	ostringstream cout;
	cout <<"Check the interception of "<<this->no_rasters<<" psycho-social raster(s) to "<< number_fp <<" hydraulic floodplain(s)..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	int *rang=NULL;
	cout <<"Sort hydraulic floodplain models..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->sort_fp_models_elem_size(&rang, fp_models, number_fp);
	try{
		for(int i=0; i< this->no_rasters; i++){
			this->raster[i].input_raster_perdatabase_element_data(ptr_database, id);
			prefix <<"RAST_"<<this->raster[i].get_raster_number()<<"> ";
			Sys_Common_Output::output_dam->set_userprefix(&prefix);

			for(int j=0; j< number_fp; j++){
				cout <<"Check the interception of psycho-social raster "<<this->raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
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
					cout <<"Generate elements, raster points and segments for the interception of psycho-social raster "<<this->raster[i].get_raster_name()<<" to hydraulic floodplain "<< fp_models[rang[j]].Param_FP.get_floodplain_number() <<"..."<<endl;
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
void Dam_Pys_System::transfer_intercepted_data2database(QSqlDatabase *ptr_database){
	ostringstream prefix;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;
	cout <<"Transfer the data from the interception of "<<this->no_rasters<<" psycho-social raster(s) to database..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	for(int i=0; i< this->no_rasters; i++){
		this->raster[i].transfer_intercepted_elem_data2database(ptr_database);
	}

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Calculate the damages
void Dam_Pys_System::calculate_damages(Dam_Impact_Value_Floodplain *impact_fp, const int number, const int raster_id){
	ostringstream cout;
	cout << "Calculate damages for psycho-social raster " <<this->raster[raster_id].get_raster_name() <<"..."<< endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster[raster_id].calculate_damages(impact_fp, number);
}
//Delete the result members for a given system-id and a scenario (boundary-, break-)
void Dam_Pys_System::delete_result_members_in_database(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	//delete the data in the table
	try{
		Dam_Pys_Element::delete_data_in_erg_table(ptr_database,id, bound_sz, break_sz);
	}
	catch(Error msg){
		throw msg;
	}
}
//Output the result members to a database table
void Dam_Pys_System::output_result_member2database(QSqlDatabase *ptr_database, const int bound_sz, const string break_sz, const int raster_id, bool *was_output){
	ostringstream cout;
	bool must_output=false;
	if(raster_id==this->no_rasters-1 && *was_output==false){
		must_output=true;
	}
	cout << "Output psycho-social damages to database of "<< this->raster[raster_id].get_raster_name() <<"..." << endl ;
	Sys_Common_Output::output_dam->output_txt(&cout);
	this->raster[raster_id].output_results2database(ptr_database, bound_sz, break_sz, was_output, must_output);
}
//Sum up the total damage results for a given system-id and scenario (boundary-, break-) from the database
void Dam_Pys_System::sum_total_results(QSqlDatabase *ptr_database, const _sys_system_id id, const int bound_sz, const string break_sz){
	this->reset_result_value();

	for(int i=0; i< this->no_rasters; i++){
		this->raster[i].sum_total_results(ptr_database, id, bound_sz, break_sz);
		//sum it up
		this->res_crit_age50_59.score_value+=this->raster[i].get_result_crit_age50_59().score_value;
		this->res_crit_age50_59.score_pop_value+=this->raster[i].get_result_crit_age50_59().score_pop_value;
		this->res_crit_age80.score_value+=this->raster[i].get_result_crit_age80().score_value;
		this->res_crit_age80.score_pop_value+=this->raster[i].get_result_crit_age80().score_pop_value;
		this->res_crit_female.score_value+=this->raster[i].get_result_crit_female().score_value;
		this->res_crit_female.score_pop_value+=this->raster[i].get_result_crit_female().score_pop_value;
		this->res_crit_owner.score_value+=this->raster[i].get_result_crit_owner().score_value;
		this->res_crit_owner.score_pop_value+=this->raster[i].get_result_crit_owner().score_pop_value;
	}

	this->res_total.score_value=this->res_crit_age50_59.score_value+this->res_crit_age80.score_value+
		this->res_crit_female.score_value+this->res_crit_owner.score_value;
	this->res_total.score_pop_value=this->res_crit_age50_59.score_pop_value+this->res_crit_age80.score_pop_value+
		this->res_crit_female.score_pop_value+this->res_crit_owner.score_pop_value;
}
//Check the psycho-social damage system
void Dam_Pys_System::check_system(void){
	if(this->no_category==0 && this->no_rasters!=0){
		Warning msg=this->set_warning(0);
		msg.output_msg(4);
	}
	if(this->no_category!=0 && this->no_rasters==0){
		Warning msg=this->set_warning(1);
		msg.output_msg(4);
	}
	if(this->no_rasters!=0){
		bool flag=true;
		for(int i=0; i<this->no_rasters; i++){
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
//Intercept the psycho-social damage raster with the given polygons and change the psycho-social category
void Dam_Pys_System::intercept_polygons2category_change(QSqlDatabase *ptr_database, const _sys_system_id id, const int number_polys, Dam_Polygon *polys, QList <int> current_id, QList <int> new_id){
	bool intercept_flag=false;
	ostringstream prefix;
	ostringstream cout;
	prefix << "PYS> ";
	Sys_Common_Output::output_dam->set_userprefix(&prefix);

	//check if there are any polygon interception with the rasters
	for(int j=0; j< this->no_rasters; j++){
		intercept_flag=false;
		for(int i=0; i< number_polys; i++){
			intercept_flag=this->raster[j].check_interception(&polys[i]);
			if(intercept_flag==true){
				break;
			}
		}
		if(intercept_flag==true){
			cout <<"Intercept psycho-social raster "<<this->raster[j].get_raster_name()<< " with the polygon(s)..." <<endl;
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
bool Dam_Pys_System::get_raster2delete(void){
	return this->del_raster_flag;
}
//__________
//private
//Allocate the psycho-social categories
void Dam_Pys_System::allocate_categories(void){
	try{
		this->category=new Dam_Pys_Category[this->no_category];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the psycho-social categories
void Dam_Pys_System::delete_categories(void){
	if(this->category!=NULL){
		delete []this->category;
		this->category=NULL;
	}
}
///Allocate the score function
void Dam_Pys_System::allocate_rasters(void){
	try{
		this->raster=new Dam_Pys_Raster[this->no_rasters];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
///Delete the score function
void Dam_Pys_System::delete_rasters(void){
	if(this->raster!=NULL){
		delete []this->raster;
		this->raster=NULL;
	}
}
//Check the if there are same raster
void Dam_Pys_System::check_raster(void){
	for(int i=0; i< this->no_rasters; i++){
		for(int j=0; j< this->no_rasters; j++){
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
//Check the psycho-social categories
void Dam_Pys_System::check_categories(void){
	for(int i=0; i< this->no_category; i++){
		for(int j=0; j<this->no_category; j++){
			if(i!=j){
				if(this->category[i].get_index()==this->category[j].get_index()){
					Error msg=this->set_error(8);
					ostringstream info;
					info <<"Index of the category       : " << this->category[i].get_index() << endl;
					info <<"Name of the 1. category     : " << this->category[i].get_name() << endl;
					info <<"Name of the 2. category     : " << this->category[j].get_name() << endl;
					throw msg;
				}
			}
		}
	}
}
//Connect the element to their psycho-social categories
void Dam_Pys_System::connect_elems2categories(const int index){
	ostringstream cout;
	cout <<"Connect the elements of psycho-social raster " << this->raster[index].get_raster_name() << " to " << this->no_category <<" available psycho-social categories..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	this->raster[index].connect_category2elems(this->no_category, this->category);

	//check them
	cout <<"Check the connection of the psycho-social raster elements to the categories..."<<endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	try{
		this->raster[index].check_category_connection();
	}
	catch(Error msg){
		Sys_Common_Output::output_dam->rewind_userprefix();
		throw msg;
	}
}
//Output the error_statistic of the system
void Dam_Pys_System::output_error_statistic(void){
	ostringstream prefix;
	ostringstream cout;
	prefix << "PYS> STA> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	cout << "Statistics of the PYS-system..." << endl ;
	cout << "GENERAL"<< endl;
	if(this->no_category>0){
		cout << " Number categories        :" << W(3) << this->no_category <<endl;
	}
	if(this->no_rasters>0){
		cout << " Number raster(s)         :" << W(3) << this->no_rasters <<endl;
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
//Reset the result value of the system
void Dam_Pys_System::reset_result_value(void){
	//reset result values
	this->res_crit_age50_59.score_pop_value=0.0;
	this->res_crit_age50_59.score_value=0.0;
	this->res_crit_age80.score_pop_value=0.0;
	this->res_crit_age80.score_value=0.0;
	this->res_crit_female.score_pop_value=0.0;
	this->res_crit_female.score_value=0.0;
	this->res_crit_owner.score_pop_value=0.0;
	this->res_crit_owner.score_value=0.0;

	this->res_total.score_pop_value=0.0;
	this->res_total.score_value=0.0;
}
//Delete selected raster in database
void Dam_Pys_System::delete_selected_raster_database(QSqlDatabase *ptr_database){
	for(int i=0; i< this->no_rasters ; i++){
		if(this->raster[i].get_is_selected()==true){
			Dam_Pys_Raster::delete_data_in_raster_table(ptr_database, this->raster[i].get_raster_number());
			Dam_Pys_Element::delete_data_in_elem_table(ptr_database, this->raster[i].get_raster_number());
			Dam_Pys_Element::delete_data_in_erg_table_per_raster(ptr_database, this->raster[i].get_raster_number());
		}
	}
}
//Set the warning
Warning Dam_Pys_System::set_warning(const int warn_type){
	string place="Dam_Pys_System::";
	string help;
	string reason;
	string reaction;
	int type=0;
	Warning msg;
	stringstream info;

	switch (warn_type){
		case 0://the raster are set but no functions
			place.append("check_system(void)") ;
			reason="Psycho-social raster are set, but no category";
			reaction="Can not calculate any psycho-social damages";
			help= "Check the psycho-social categories";
			type=19;
			break;
		case 1://the functions are set but no raster
			place.append("check_system(void)") ;
			reason="Psycho-social categories are set, but no raster";
			reaction="Can not calculate any psycho-social damages";
			help= "Check the psycho-social raster";
			type=19;
			break;
		case 2://not all rasters are connected
			place.append("check_system(void)") ;
			reason="Not all psycho-social rasters are connected to the hydraulic system";
			help= "Connect the psycho-sociale raster to avoid errors in the damage results";
			type=19;
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
Error Dam_Pys_System::set_error(const int err_type){
	string place="Dam_Pys_System::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_categories(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("allocate_rasters(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no adding of identical raster
			place.append("add_damage_raster_file2database(QSqlDatabase *ptr_database)");
			reason="You can not add raster with identical general information like the geometry";
			help="Check the economical raster for adding";
			type=21;
			break;
		case 3://can not open density function file
			place.append("read_density_function_per_file(const string file_func)");
			reason="Can not open the file of the density functions";
			help="Check the file";
			type=5;
			break;
		case 4://there can't be same raster
			place.append("check_raster(void)");
			reason="There are same raster in the database";
			help="Check the geometrical settings of the economical raster";
			type=21;
			break;
		case 5://can not open psycho-social category function file
			place.append("read_category_per_file(const string file_category)");
			reason="Can not open the file of the psycho-social categories";
			help="Check the file";
			type=5;
			break;
		case 6://wrong input
			place.append("read_category_per_file(const string file_category)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the psycho-social categories in file";
			type=1;
			break;
		case 7://do not find number of categories in file
			place.append("read_category_per_file(const string file_category)");
			reason="Do not find the keyword for the number of categories in file !NO_CATEGORY";
			help="The number of categories must be declared before the individual category data begins";
			type=1;
			break;
		case 8://there are categories with the same index
			place.append("check_categories(void");
			reason="There are psycho-social categories with the same index";
			help="Check the psycho-social categories";
			type=23;
			break;
		case 9://not all required rasters are set
			place.append("add_damage_raster_file2database(QSqlDatabase *ptr_database)") ;
			reason="The required psycho-social raster file is not set. The raster is not transferd to database";
			help= "Set the file correctly";
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