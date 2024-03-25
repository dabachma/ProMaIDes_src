#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Section_Dia.h"

//Default constructor
FplGui_Section_Dia::FplGui_Section_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	ostringstream txt;
	txt <<"Nothing set "<< endl;
	this->label_header->setText(txt.str().c_str());
	this->items=NULL;
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->checkBox_all_select, SIGNAL(clicked(bool )), this, SLOT(all_selection(bool )));

	this->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->listWidget->setToolTip("Id       Name      Start Riverstation [m]      Type    River Conn-Id   Bank"); 

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Section_Dia), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Section_Dia::~FplGui_Section_Dia(void){

	if(this->items!=NULL){
		delete []this->items;
		this->items=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Section_Dia), _sys_system_modules::FPL_SYS);
}
//__________
//public
//Set the dialog, like the database, system_id etc.
void FplGui_Section_Dia::set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id,  const _fpl_thread_type type){
	QSqlQueryModel result;
	int number=0;
	Fpl_Section *my_section=NULL;

	number=Fpl_Section::select_section_database(&result, ptr_database, sys_id);

	try{
		my_section=new Fpl_Section[number];
		
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	try{
		//set the section
		for(int i=0; i< number;i++){
			my_section[i].set_systemid(sys_id);
			my_section[i].input_section_perdatabase(&result, i, ptr_database);
		}

		int number_relevant=0;
		//set the relevant list
		//count
		for(int i=0; i< number; i++){
			if(type==_fpl_thread_type::fpl_frc_by_hand){
				if(my_section[i].get_type_section()==_fpl_section_types::by_hand_sec ){
					number_relevant++;
				}
			}
			else if((type==_fpl_thread_type::fpl_determ_selected || type==_fpl_thread_type::fpl_mc_selected || type==_fpl_thread_type::fpl_frc_selected) ){
				if(my_section[i].get_type_section()!=_fpl_section_types::by_hand_sec){
					number_relevant++;
				}
			}
			else{
				number_relevant++;
			}
		}
		//allocate
		this->allocate_list_item(number_relevant);
		number_relevant=0;
		//set 
		for(int i=0; i< number; i++){
			if(type==_fpl_thread_type::fpl_frc_by_hand){
				if(my_section[i].get_type_section()==_fpl_section_types::by_hand_sec ){
					this->items[number_relevant].setText(my_section[i].get_string_selection().c_str());
					this->listWidget->addItem(&this->items[number_relevant]);
					number_relevant++;
				}
			}
			else if((type==_fpl_thread_type::fpl_determ_selected || type==_fpl_thread_type::fpl_mc_selected || type==_fpl_thread_type::fpl_frc_selected)  ){
				if(my_section[i].get_type_section()!=_fpl_section_types::by_hand_sec){
					this->items[number_relevant].setText(my_section[i].get_string_selection().c_str());
					this->listWidget->addItem(&this->items[number_relevant]);
					number_relevant++;
				}
			}
			else{
				this->items[number_relevant].setText(my_section[i].get_string_selection().c_str());
				this->listWidget->addItem(&this->items[number_relevant]);
				number_relevant++;
			}
		}

		ostringstream txt;
		//set the header

		switch(type){
			case _fpl_thread_type::fpl_determ_selected:
					txt << "Select from " << number_relevant << " FPL-section for deterministic calculation..." << endl;
					break;
			case _fpl_thread_type::fpl_mc_selected:
					txt << "Select from " << number_relevant << " FPL-section for monte-carlo calculation..." << endl;
					break;
			case _fpl_thread_type::fpl_frc_selected:
					txt << "Select from " << number_relevant << " FPL-section for fragility curve calculation..." << endl;
					break;
			case _fpl_thread_type::fpl_frc_by_hand:
					txt << "Select from " << number_relevant << " FPL-section for defining the fragility curve by the user..." << endl;;
					break;
			case _fpl_thread_type::fpl_del_section:
					txt << "Select from " << number_relevant << " FPL-section for deleting all data (input, results etc.) \n in database tables ..." << endl;;
					break;
			default:
				txt << "Select from " << number_relevant << " FPL-section and handle it in an unknown way ..." << endl;;
		}
		this->set_txt(txt.str());
	}
	catch(Error msg){
		if(my_section!=NULL){
			delete []my_section;
			my_section=NULL;
		}
		throw msg;
	}
	
	//delete the sections
	if(my_section!=NULL){
		delete []my_section;
		my_section=NULL;
	}
}
//Start the dialog; it returns true by acception, false by rejection
bool FplGui_Section_Dia::start_dialog(int  **list_section, int *number){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		*number=0;
		*list_section=NULL;
		return false;
	}
	//accepted
	else{

		//count selected
		int number_selected=0;
		for(int i=0; i<this->listWidget->count(); i++){
			if(this->listWidget->item(i)->isSelected()==true){
				number_selected++;
			}
		}
		//allocate the list of section
		if(number_selected==0){
			*number=0;
			*list_section=NULL;
			return false;
		}
		*number=number_selected;
		*list_section=new int[*number];

		//read it out
		number_selected=0;
		for(int i=0; i<this->listWidget->count(); i++){
			if(this->listWidget->item(i)->isSelected()==true){
				int id_buff=0;
				stringstream buff_str; 
				buff_str << this->listWidget->item(i)->text().toStdString();
				buff_str >> id_buff;
				(*list_section)[number_selected]=id_buff;
				number_selected++;
			}
		}
		
		return true;
	}

}
///Set a text to the text label of the dialog
void FplGui_Section_Dia::set_txt(const string txt){
	this->label_header->setText(txt.c_str());
}
//Set the dialog header text
void FplGui_Section_Dia::set_dia_header_txt(const string header_title){
	this->setWindowTitle(header_title.c_str());
}
//_________
//public sots
//Switch the selection between all and clear selection
void FplGui_Section_Dia::all_selection(bool flag){
	if(flag==true){
		this->listWidget->selectAll();
	}
	else{
		this->listWidget->clearSelection();
	}
}
//_________
//private
//Allocate the items of the list widget
void FplGui_Section_Dia::allocate_list_item(const int number){
	this->items=new QListWidgetItem[number];

}
//Set error(s)
Error FplGui_Section_Dia::set_error(const int err_type){
		string place="FplGui_Section_Dia::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_dialog(QSqlDatabase *ptr_database, _sys_system_id sys_id,  const _fpl_thread_type type)");
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