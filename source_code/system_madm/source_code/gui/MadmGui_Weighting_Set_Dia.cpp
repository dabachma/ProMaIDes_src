#include "Madm_Headers_Precompiled.h"
//#include "MadmGui_Weighting_Set_Dia.h"

//Default constructor
MadmGui_Weighting_Set_Dia::MadmGui_Weighting_Set_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	ostringstream txt;
	txt <<"Select the weighting set "<< endl;
	this->label_header->setText(txt.str().c_str());
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->checkBox_all_select, SIGNAL(clicked(bool )), this, SLOT(all_selection(bool )));

	this->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->listWidget->setToolTip("Set-id      Name  "); 
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(MadmGui_Weighting_Set_Dia), _sys_system_modules::MADM_SYS);
}
//Default destructor
MadmGui_Weighting_Set_Dia::~MadmGui_Weighting_Set_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(MadmGui_Weighting_Set_Dia), _sys_system_modules::MADM_SYS);
}
//__________
//public
//Set the dialog, like the database, system_id etc.
int MadmGui_Weighting_Set_Dia::set_dialog(QSqlDatabase *ptr_database){
	QSqlQueryModel result;
	int number=0;
	try{

		QSqlQueryModel model;
		number=Madm_System::select_data_in_settable(&model, ptr_database);

		QListWidgetItem *item;
		item=NULL;
		ostringstream buffer;
		for(int i=0; i< number; i++){
			buffer.str("");
			buffer << model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_id)).c_str()).toString().toStdString()<<"   ";
			buffer << model.record(i).value((Madm_System::table_sets->get_column_name(madm_label::set_name)).c_str()).toString().toStdString();
			item=new QListWidgetItem;
			item->setText(buffer.str().c_str());
			this->listWidget->addItem(item);
		}



	}
	catch(Error msg){
		throw msg;
	}
	return number;
}
//Start the dialog; it returns true by acception, false by rejection
bool MadmGui_Weighting_Set_Dia::start_dialog(int  **list_sets, int *number){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		*number=0;
		*list_sets=NULL;
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
			*list_sets=NULL;
			return false;
		}
		*number=number_selected;
		*list_sets=new int[*number];

		//read it out
		number_selected=0;
		for(int i=0; i<this->listWidget->count(); i++){
			if(this->listWidget->item(i)->isSelected()==true){
				int id_buff=0;
				stringstream buff_str; 
				buff_str << this->listWidget->item(i)->text().toStdString();
				buff_str >> id_buff;
				(*list_sets)[number_selected]=id_buff;
				number_selected++;
			}
		}
		
		return true;
	}

}
///Set a text to the text label of the dialog
void MadmGui_Weighting_Set_Dia::set_txt(const string txt){
	this->label_header->setText(txt.c_str());
}
//Set the dialog header text
void MadmGui_Weighting_Set_Dia::set_dia_header_txt(const string header_title){
	this->setWindowTitle(header_title.c_str());
}
//_________
//public sots
//Switch the selection between all and clear selection
void MadmGui_Weighting_Set_Dia::all_selection(bool flag){
	if(flag==true){
		this->listWidget->selectAll();
	}
	else{
		this->listWidget->clearSelection();
	}
}