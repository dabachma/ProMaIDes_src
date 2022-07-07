#include "Sys_Headers_Precompiled.h"
//#include "Sys_Output_Division.h"

//constructor
#ifdef development
	Sys_Output_Division::Sys_Output_Division(void):gui_flag(true), console_flag(true){
#else
	Sys_Output_Division::Sys_Output_Division(void):gui_flag(true), console_flag(false){
#endif
	this->name_file="logfile.dat";
	this->guioutput=NULL;
	this->parent_display=NULL;
	this->detail_display=false;
	this->module_type=_sys_system_modules::SYS_SYS;
	this->prefix_was_output=false;
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Output_Division),_sys_system_modules::SYS_SYS);//count the memory
}
//Constructor with a given module type
#ifdef development
	Sys_Output_Division::Sys_Output_Division(const _sys_system_modules type):gui_flag(true), console_flag(true){
#else
	Sys_Output_Division::Sys_Output_Division(const _sys_system_modules type):gui_flag(true), console_flag(false){
#endif

	this->name_file="logfile.dat";
	this->guioutput=NULL;
	this->parent_display=NULL;
	this->detail_display=false;
	this->module_type=type;
	this->prefix_was_output=false;
	
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Output_Division), _sys_system_modules::SYS_SYS);//count the memory
}
//destructor
Sys_Output_Division::~Sys_Output_Division(void){
	//close the logfile
	this->close_file();
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Output_Division)), _sys_system_modules::SYS_SYS);//count the memory
}
//_________________________
//public
//Set the type of module for the output
void Sys_Output_Division::set_module_type(const _sys_system_modules type){
	this->module_type=type;
}
//open the output file and set the file name
void Sys_Output_Division::set_file(const string filename){
	this->name_file=filename;
	this->name_file.append(".dat");
}
//set an new logfile
void Sys_Output_Division::open_new_file(const string filename){

	////check if it is open
	if(this->fileoutput.is_open()==true){
		this->save_logfile2archiv();
		//close the file
		this->close_file();
	}
	//set a new logfile name
	this->name_file=filename;
	this->name_file.append(".dat");
	//open the file
	this->open_file();
}
//Set a new logfile per gui
void Sys_Output_Division::set_new_logfile_gui(QWidget *parent){
	Sys_Output_Logfile_Dia dialog(parent);
	dialog.set_txt_modul_type(this->module_type);

	bool flag=false;
	string buffer;
	//start the dialog
	flag=dialog.setOutputLogfile(&buffer, this->name_file);

	//open new file
	if(flag==true){
		this->open_new_file(buffer);
	}

}
//close the logfile
void Sys_Output_Division::close_file(void){
	//check if it is open
	if(this->fileoutput.is_open()==true){
		//close the file
		this->fileoutput.close();
		if(fileoutput.is_open()==true){
			this->error(1);
		}
	}
}
//remove the logfile
void Sys_Output_Division::remove_file(void){
	//check if it is open
	if(this->fileoutput.is_open()==false){
		//remove the file
		remove(this->name_file.c_str());	
	}
}
//set the gui output display
void Sys_Output_Division::set_gui_texteditor(QTextEdit *ptr_output_display=NULL){
	this->guioutput=ptr_output_display;
	this->welcome_to_gui();
}
//set detail flags per console
void Sys_Output_Division::set_detailflag_console(void){
	int buffer;
	bool flag=false;
	cout << "=======================================================================" <<endl;
	cout << "Set new output flags for" << endl;
	switch(this->module_type){
			//output for the module system
			case _sys_system_modules::SYS_SYS:
				cout << " SYS output " << endl;				
				break;
			//output for the module fpl
			case _sys_system_modules::FPL_SYS:
				cout << " FPL output " << endl;
				break;
			//output for the module hyd
			case _sys_system_modules::HYD_SYS:
				cout << " HYD output " << endl;
				break;
			//output for the module madm
			case _sys_system_modules::MADM_SYS:
				cout << " MADM output " << endl;
				break;
			//output for the module dam
			case _sys_system_modules::DAM_SYS:
				cout << " DAM output " << endl;
				break;
			default:
				cout << " UNKNOWN output " << endl;
				break;
		}
	cout << " Actuel flag:" << endl;
	cout << " Console Output " << ((this->detail_display==true)?("true"):("false")) << endl;
	//new console output
	cout << "New Console Output (0) true (1) false " << endl;
	do{	
		cin >> buffer;
		if((flag=(cin.fail()))){//check if the input is valif
				cout << "Wrong input! Do it again" << endl;
				cin.clear();//clear the error flags
				cin.ignore();//clear cin
		}
		else if(buffer < 0 || buffer>1){
			cout << "Wrong Selection! Do it again! " << endl;
		}
	}
	while(flag || buffer<0 || buffer>1);
	if(buffer==0){
		this->detail_display=true;
	}
	else{
		this->detail_display=false;
	}

	cout << "New flag:" << endl;
	cout << " Console Output " << ((this->detail_display==true)?("true"):("false")) << endl;
}
//Set the output detail flag per GUI
void Sys_Output_Division::set_detailflag_gui(QWidget *parent){

	Sys_Output_Flag_Dia dialog(parent);

	dialog.set_current_flag(this->detail_display);
	dialog.set_txt_modul_type(this->module_type);

	this->detail_display=dialog.get_new_detailed_flag();

}
//Set the falg for the output directly (true => all to console and to file; false => all to file and some things to console)
void Sys_Output_Division::set_detailflag(const bool detail){
	this->detail_display=detail;
}
//Get the flag for the output
bool Sys_Output_Division::get_detailflag(void){
	return this->detail_display;
}
//output the text
void Sys_Output_Division::output_txt(const string txt, const bool detail_output, const bool file_output){
	//if(Sys_Project::get_project_name()==label::not_set){
	//	return;
	//}

	ostringstream total_gui;
	ostringstream total_file;
	string buffer1=txt;
	string buffer2;
	int pos=0;
	int counter=0;
	int prefix_length=0;
	prefix_length=strlen(this->standard_prefix().c_str())+strlen(this->user_prefix.c_str());
	string whitespace_gui(prefix_length,' ');
	string whitespace_file(prefix_length,' ');
	if(this->user_prefix.length()==0){
		this->prefix_was_output=false;
	}
	
	//split the txt string by endl;
	do{
		pos=buffer1.find("\n");//find position of \n
		buffer2=buffer1.substr(0,pos);//cut by \n
		buffer1.erase(0,pos+1);
		if(pos> 0 && counter==0 && buffer2.empty()!=true && this->prefix_was_output==false){
			total_gui << this->standard_prefix() << this->user_prefix << buffer2 <<endl;
			total_file << this->standard_prefix() << this->user_prefix << buffer2 <<endl;
			this->prefix_was_output=true;
		}
		else if(pos> 0 && counter==0 && buffer2.empty()!=true && this->prefix_was_output==true){
			total_gui << whitespace_gui << buffer2 << endl;
			total_file << whitespace_file << buffer2 << endl;
		}
		else if(pos> 0 && counter !=0 && buffer2.empty()!=true){
			total_gui << whitespace_gui << buffer2 << endl;
			total_file << whitespace_file << buffer2 << endl;
		}
		else if(pos==-1 && counter !=0 && buffer2.empty()!=true){//no "endl" found
			total_gui << whitespace_gui << buffer2;
			total_file << whitespace_file << buffer2;
			buffer2="";
		}
		else if(pos==-1 && counter ==0 && buffer2.empty()!=true){//no "endl" found
			total_gui  << buffer2;
			total_file  << buffer2;
			buffer2="";
		}
		else if(pos==0){//just one "endl"
			total_gui << endl;
			total_file << endl;
		}
		counter++;
	}
	while(buffer2.empty()!=true);

	//ouput to gui
	if(gui_flag==true){
		if(this->guioutput!=NULL){
			QString buffer(total_gui.str().c_str());
			if(detail_output==false){//always output		
				emit gui_text(buffer);//emit the signal
			}
			else if(detail_output==true && this->detail_display==true){
				emit gui_text(buffer);//emit the signal
			}
		
		}
	}
	//output to console
	if(console_flag==true){
		if(detail_output==false){//always output
			cout << total_file.str() ;
		}
		else if(detail_output==true && this->detail_display==true){
			cout << total_file.str() ;
		}//no output in the other cases
	}

	//output to file
	if(file_output==true){
		if(Sys_Project::get_project_name()!=label::not_set){
			this->open_file();
			if(this->fileoutput.is_open()==true){
				this->fileoutput << total_file.str();
				this->fileoutput.flush();
			}
		}
	}
}
//output text with deleting the stream
void Sys_Output_Division::output_txt(ostringstream *stream, const bool detail_output, const bool file_output){
	string buffer;
	buffer=stream->str();
	stream->str("");
	stream->clear();
	this->output_txt(buffer, detail_output, file_output);
}
//insert a separator to the output media
void Sys_Output_Division::insert_separator(const int type,  const bool detail_output){
	//open the file (for the first time)
	this->open_file();
	ostringstream separator;
	switch (type){
		case 0:
			separator << "----------" << endl ;
			this->output_txt(separator.str(), detail_output);
			break;
		case 2:
			separator << "--------------------" << endl;
			this->output_txt(separator.str(), detail_output);
		break;
		case 3:
			separator << "====================" << endl;
			this->output_txt(separator.str(), detail_output);
		break;
		case 4:
			separator << "-----------------------------------------------------------------------" << endl ;
			this->output_txt(separator.str(), detail_output);
			break;
		default:
			separator << "----------" << endl;
			this->output_txt(separator.str(), detail_output);
	}
}
//get a separator
string Sys_Output_Division::get_separator(const int type){
		ostringstream separator;
	switch (type){
		case 0:
			separator << "----------" << endl ;
			return separator.str();
		case 2:
			separator << "--------------------" << endl;
			return separator.str();
		case 3:
			separator << "====================" << endl;
			return separator.str();
		case 4:
			separator << " ---------------------------------------------------------" << endl ;
			return separator.str();
		default:
			separator << "----------" << endl;
		return separator.str();
	}
}
//set the user prefix
void Sys_Output_Division::set_userprefix(const string userprefix){
	this->user_prefix.append(userprefix);
	this->prefix_was_output=false;
}
//Add one part to the user-defined prefix and delete the stringstream
void Sys_Output_Division::set_userprefix(ostringstream *prefix){
	string buffer;
	buffer=prefix->str();
	prefix->str("");
	prefix->clear();
	this->set_userprefix(buffer);
}
//reset userprefix
void Sys_Output_Division::reset_userprefix(void){
	this->user_prefix="";
	this->prefix_was_output=false;
	//make one endl
	if(gui_flag==true){
		emit gui_text("\n");//emit the signal
	}
	if(console_flag==true){
		cout << endl;
	}
	if(this->fileoutput.is_open()==true){
		this->fileoutput << endl;
	}
} 
//Reset the flag that the prefix was outputed; the prefix will be newly outputed
void Sys_Output_Division::reset_prefix_was_outputed(void){
	this->prefix_was_output=false;
}
//rewind one stage of the userprefix
void Sys_Output_Division::rewind_userprefix(void){
	string buffer1;
	int pos=0;
	buffer1=this->user_prefix;
	pos=buffer1.rfind(">");//first >
	buffer1=buffer1.substr(0,pos);
	pos=buffer1.rfind(">");//second >
	buffer1=buffer1.substr(0,pos+1);
	if(pos>=0){
		this->user_prefix=buffer1.append(" ");
	}
	else{
		this->user_prefix=buffer1;
	}
	this->prefix_was_output=false;
}
//get the user prefix
string Sys_Output_Division::get_totalprefix(void){
	ostringstream buffer;
	buffer<< this->standard_prefix() << this->user_prefix;
	return buffer.str();
}

//set welcome text
void Sys_Output_Division::welcome_to_gui(void){
	ostringstream cout;
	//to gui
	if(gui_flag==true){
		if(this->guioutput!=NULL){
			switch(this->module_type){
				//output for the module system
				case _sys_system_modules::SYS_SYS:	
                    cout << this->standard_prefix()<<"----- This is the output for the module SYS ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the fpl
				case _sys_system_modules::FPL_SYS:
                    cout <<this->standard_prefix()<< "----- This is the output for the module FPL ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the exception
				case _sys_system_modules::EXC_SYS:
                    cout <<this->standard_prefix()<< "----- This is the Exception output ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the hydraulic
				case _sys_system_modules::HYD_SYS:
                    cout <<this->standard_prefix()<< "----- This is the output for the module HYD ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the madm-system
				case _sys_system_modules::MADM_SYS:
                    cout <<this->standard_prefix()<< "----- This is the output for the module MADM ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
					break;
				//output for the damage-system
				case _sys_system_modules::DAM_SYS:
                    cout <<this->standard_prefix()<< "----- This is the output for the module DAM ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the risk-system
				case _sys_system_modules::RISK_SYS:
                    cout <<this->standard_prefix()<< "----- This is the output for the module RISK ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
					break;
				//output for the alt system
				case _sys_system_modules::ALT_SYS:	
                    cout << this->standard_prefix()<<"----- This is the output for the module ALT ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
					break;
				//output for the cost system
				case _sys_system_modules::COST_SYS:	
                    cout << this->standard_prefix()<<"----- This is the output for the module COST ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
					break;
				//output for the hydrol system
				case _sys_system_modules::HYDROL_SYS:
					cout << this->standard_prefix() << "----- This is the output for the module HYDROL (" << Sys_Project::get_version_number() << " from " << Sys_Project::get_version_date() << ")----" << endl;
					break;
				default:
                cout << this->standard_prefix()<<"----- This is the output for a not know module type ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			}
			cout <<"          "<< "Starttime "<< Sys_Output_Division::set_time();
			cout  << endl;
			this->guioutput->insertPlainText(cout.str().c_str());


		}
	}
}

//Switch the logfile to the given path
void Sys_Output_Division::switch_logfile_path(const string path, const bool del_flag){
	if(this->fileoutput.is_open()==true){
		this->fileoutput.close();
		if(del_flag==true){
			this->remove_file();
		}
	}
	ostringstream buffer;
	buffer << path <<"/"<< this->get_logfile_name()<<".dat";
	this->name_file=buffer.str();
}
//set the time  (static)
string Sys_Output_Division::set_time(void){
	//fot the time
	ostringstream occ_time;
	time_t log_time;
	struct tm *timeinfo;
	//get time
	time(&log_time);
	timeinfo=localtime(&log_time);
	occ_time << asctime(timeinfo);
	string buff;
	buff=occ_time.str();
	buff=buff.substr(0, buff.length()-1);
	return buff;
}
//Save the current logfile to the logfile archiv
void Sys_Output_Division::save_logfile2archiv(void){
	if(this->fileoutput.is_open()==false){
		return;
	}
	if(Sys_Project::get_logfile_save_flag()==false){
		return;
	}
	QDir my_dir;
	ostringstream buff;
	string buffer;

	buff <<Sys_Project::get_main_path()<<sys_label::folder_logfile_archiv;
	buffer=buff.str();
	if(my_dir.exists(buffer.c_str())==false){
		//create folder
		if(my_dir.mkdir(buffer.c_str())==false){
			this->error(2);
		}
	}
	//close the file
	buff.str("");
	string pur_filename;
	pur_filename=functions::get_file_name(this->name_file);
	pur_filename=functions::remove_suffix_file_name(pur_filename);

	int counter=0;
	this->fileoutput.close();
	QFile myfile;
	myfile.setFileName(this->name_file.c_str());
	bool ok_flag=false;
	do{
		buff.str("");
		buff << buffer<<"/"<<pur_filename<<"_"<<this->get_current_date()<<"_"<<counter<<".dat";
		if(myfile.copy(buff.str().c_str())==true){
			ok_flag=true;
		}
		else{
			myfile.unsetError();
			counter++;
		}
	}
	while(ok_flag==false);

	//remove it
	this->remove_file();

}
//Set the focus on the display, where the output comes
void Sys_Output_Division::set_focus_on_display(void){
	if(this->guioutput!=NULL && this->guioutput->parentWidget()!=NULL && this->parent_display!=NULL){
		this->parent_display->setCurrentWidget(this->guioutput->parentWidget());
	}
}
//Set the pointer to the parent tabwidget of the display output widget
void Sys_Output_Division::set_ptr_parent_display_output(QTabWidget *parent){
	this->parent_display=parent;
}
//________________
//private
//open the file 
void Sys_Output_Division::open_file(void){
	//check if it is open
	if(this->fileoutput.is_open()==false){
		//open the file
		this->fileoutput.open(this->name_file.c_str());
		if(!this->fileoutput){//can not open
			this->error(0);
		}
		else{
			this->welcome_to_file();
		}
	}
}

//set welcome text
void Sys_Output_Division::welcome_to_file(void){
	ostringstream cout;
	if(this->fileoutput.is_open()==true){
		switch(this->module_type){
			//output for the module system
			case _sys_system_modules::SYS_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module SYS ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			//output for the fpl system
			case _sys_system_modules::FPL_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module FPL ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			//output for the EXC system
			case _sys_system_modules::EXC_SYS:	
                cout << this->standard_prefix()<<"----- This is the Exception output ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;;
				break;
			//output for the hyd system
			case _sys_system_modules::HYD_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module HYD ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			//output for the madm system
			case _sys_system_modules::MADM_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module MADM ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
				break;
			//output for the dam system
			case _sys_system_modules::DAM_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module DAM ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			//output for the risk system
			case _sys_system_modules::RISK_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module RISK ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
				break;
			//output for the alt system
			case _sys_system_modules::ALT_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module ALT ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;
			//output for the cost system
			case _sys_system_modules::COST_SYS:	
                cout << this->standard_prefix()<<"----- This is the output for the module COST ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")----" << endl;
				break;
			//output for the hydrol system
			case _sys_system_modules::HYDROL_SYS:
				cout << this->standard_prefix() << "----- This is the output for the module HYDROL (" << Sys_Project::get_version_number() << " from " << Sys_Project::get_version_date() << ")----" << endl;
				break;
			default:
                cout << this->standard_prefix()<<"----- This is the output for a not know module type ("<<Sys_Project::get_version_number()<<" from "<<Sys_Project::get_version_date() <<")-----" << endl;
				break;

		}
		cout << "          "<< "Generationtime   : "<< Sys_Output_Division::set_time();
		cout << "          "<< "Projectname      : "<< Sys_Project::get_project_name() << endl;
		cout << endl;
		this->fileoutput <<cout.str();
	}
}
//give the standard prefix
string Sys_Output_Division::standard_prefix(void){
	ostringstream std_prefix;
	switch(this->module_type){
			//output for the module system
			case _sys_system_modules::SYS_SYS:
				std_prefix << "SYS> " ;
				break;
			//output for the module fpl
			case _sys_system_modules::FPL_SYS:
				std_prefix << "FPL> " ;
				break;
			//output for the module EXC
			case _sys_system_modules::EXC_SYS:
				std_prefix << "EXCEP> " ;
				break;
			//output for the module hyd
			case _sys_system_modules::HYD_SYS:
				std_prefix << "HYD> " ;
				break;
			//output for the module madm
			case _sys_system_modules::MADM_SYS: 
				std_prefix << "MADM> " ;
				break;
			//output for the module dam
			case _sys_system_modules::DAM_SYS: 
				std_prefix << "DAM> " ;
				break;
			//output for the module dam
			case _sys_system_modules::RISK_SYS: 
				std_prefix << "RISK> " ;
				break;
			//output for the module alt
			case _sys_system_modules::ALT_SYS: 
				std_prefix << "ALT> " ;
				break;
			//output for the module cost
			case _sys_system_modules::COST_SYS: 
				std_prefix << "COST> " ;
				break;
				//output for the module hydrol
			case _sys_system_modules::HYDROL_SYS:
				std_prefix << "HYDROL> ";
				break;
			default:
				std_prefix << "NOT_KNOWN> " ;
	}
	return std_prefix.str();
}
//Get the logfile name depending to the module type (_sys_system_modules)
string Sys_Output_Division::get_logfile_name(void){
	string buffer;

	switch(this->module_type){
		case _sys_system_modules::DAM_SYS:
			buffer="logfile_DAM";
			break;
		case _sys_system_modules::EXC_SYS:
			buffer="exception";
			break;
		case _sys_system_modules::FPL_SYS:
			buffer="logfile_FPL";
			break;
		case _sys_system_modules::HYD_SYS:
			buffer="logfile_HYD";
			break;
		case _sys_system_modules::MADM_SYS:
			buffer="logfile_MADM";
			break;
		case _sys_system_modules::SYS_SYS:
			buffer="logfile_SYS";
			break;
		case _sys_system_modules::RISK_SYS:
			buffer="logfile_RISK";
			break;
		case _sys_system_modules::ALT_SYS:
			buffer="logfile_ALT";
			break;
		case _sys_system_modules::COST_SYS:
			buffer="logfile_COST";
			break;
		case _sys_system_modules::HYDROL_SYS:
			buffer = "logfile_HYDROL";
			break;
		default:
			buffer="logfile_UNKNOWN";
	}
	return buffer;
}
//Get the date as string for saving the logfiles
string Sys_Output_Division::get_current_date(void){
	ostringstream buffer;
	time_t mytime;
	struct tm * timeinfo;
	time(&mytime);
	timeinfo = localtime (&mytime);

	buffer <<1900+timeinfo->tm_year<<"-"<<timeinfo->tm_mon+1<<"-"<<timeinfo->tm_mday;
	return buffer.str();;
}
//set the error
void Sys_Output_Division::error(const int err_type){
		string place="Sys_Output_Division::";
		string help;
		string reason;
        bool fatal;
        fatal=false;
		stringstream info;
	switch (err_type){
		case 0://can not open logfile
			place="open_file(const string filename)";
			reason="Could not open the logfile";
			help="Check the file name";
			info << "Filename: " << this->name_file<< endl;
			fatal=true;
			break;
		case 1://can not close logfile
			place="close_file(void)";
			reason="Could not close the logfile";
			help="Check the file name";
			info << "Filename: " << this->name_file<< endl;
			fatal=true;
			break;
		case 2://can not create directories
			place.append("save_logfile2archiv(void)");
			reason="Can not create the directories";
			help="Check the directory name";
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
	}
	//put the output together
	ostringstream cout1;
	cout1 << "################################################" << endl;
	cout1 << "Reason of error: "<< reason << endl;
	cout1 << "Place of error in Code: "<< place << endl;
	cout1 << "Help for error: " << help << endl;
	cout1 << "Secondary informations: " ;
	if(info.str().empty()){
		cout1 << "No secondary information available " << endl;
	}
	else{
		cout1 << info.str() << endl;
	}
	cout1 << "################################################" << endl;
	//final output
	QString buffer(cout1.str().c_str());
	//ouput to gui
	if(gui_flag==true){
		if(this->guioutput!=NULL){	
			emit gui_text(buffer);//emit the signal
		}
	}
	//output to console
	if(console_flag==true){
			cout << cout1.str();
	}
}
