#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Measure_Dia.h"

///Default constructor
AltGui_Measure_Dia::AltGui_Measure_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	this->measure_state2switch=-1;
		
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));


	this->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	this->listWidget->setToolTip("Id       Name     ");
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Measure_Dia), _sys_system_modules::ALT_SYS);

}
///Default destructor
AltGui_Measure_Dia::~AltGui_Measure_Dia(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Measure_Dia), _sys_system_modules::ALT_SYS);
}
//__________
//public
//Set window title
void AltGui_Measure_Dia::set_window_title(const string txt){
	this->setWindowTitle(txt.c_str());
}
//Set heading
void AltGui_Measure_Dia::set_heading(const string txt){
	this->label_header->setText(txt.c_str());
}
//Set for the dialog the measure identifier
void AltGui_Measure_Dia::set_dialog(const int number, _alt_general_info *measure_list, const int current_measure){

	QListWidgetItem *item;
	ostringstream buffer;
	if(number>0){
		buffer <<"0    base-state";
		item=new QListWidgetItem;
		item->setText(buffer.str().c_str());
		if(current_measure==0){
			item->setBackgroundColor(Qt::lightGray);
			Qt::ItemFlags my_flag;
			my_flag=my_flag|(Qt::ItemIsSelectable);
			item->setFlags(my_flag);
			item->setToolTip("Current measure state");
		}
		this->listWidget->addItem(item);
	}
	else{
		buffer <<"No measures available";
		item=new QListWidgetItem;
		item->setText(buffer.str().c_str());
		Qt::ItemFlags my_flag;
		my_flag=my_flag|(Qt::ItemIsSelectable);
		item->setFlags(my_flag);

		this->okButton->setEnabled(false);

	}

	for(int i=0; i< number; i++){
		buffer.str("");
		buffer<<measure_list[i].id.measure_nr<<"   "<<measure_list[i].name;
		item=new QListWidgetItem;
		item->setText(buffer.str().c_str());
		if(measure_list[i].id.measure_nr==current_measure){
			item->setBackgroundColor(Qt::lightGray);
			Qt::ItemFlags my_flag;
			my_flag=my_flag|(Qt::ItemIsSelectable);
			item->setFlags(my_flag);
			item->setToolTip("Current measure state");
		}
		this->listWidget->addItem(item);
	}


}
//Set for the dialog the measure identifier for a deleting action
void AltGui_Measure_Dia::set_dialog_delete(const int number, _alt_general_info *measure_list, const int current_measure){
	QListWidgetItem *item;
	ostringstream buffer;
	if(number>0){
		if(current_measure==0){
			buffer <<"Base state";
			item=new QListWidgetItem;
			item->setText(buffer.str().c_str());
			buffer.str("");
			item->setBackgroundColor(Qt::lightGray);
			Qt::ItemFlags my_flag;
			//my_flag=my_flag|(Qt::ItemIsSelectable);
			item->setFlags(my_flag);
			item->setToolTip("Current measure state");
			this->listWidget->addItem(item);
		}
		
	}
	else{
		buffer <<"No measures available";
		item=new QListWidgetItem;
		item->setText(buffer.str().c_str());
		buffer.str("");
		Qt::ItemFlags my_flag;
		//my_flag=my_flag|(Qt::ItemIsSelectable);
		item->setFlags(my_flag);

		this->okButton->setEnabled(false);
		this->listWidget->addItem(item);

	}

	for(int i=0; i< number; i++){
		buffer.str("");
		buffer<<measure_list[i].id.measure_nr<<"   "<<measure_list[i].name;
		item=new QListWidgetItem;
		item->setText(buffer.str().c_str());
		if(measure_list[i].id.measure_nr==current_measure){
			item->setBackgroundColor(Qt::lightGray);
			Qt::ItemFlags my_flag;
			//my_flag=my_flag|(Qt::ItemIsSelectable);
			item->setFlags(my_flag);
			item->setToolTip("Current measure state");
		}
		this->listWidget->addItem(item);
	}
}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Measure_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//QList<QListWidgetItem *> selectedItems
		if(this->listWidget->selectedItems().count()==0){
			return false;
		}
		else{

			//set the data
			stringstream buff_str; 
			buff_str << this->listWidget->selectedItems().at(0)->text().toStdString();
			buff_str >> measure_state2switch;
			return true;
		}
	}
}
//Get the selected informations, the measure state to switch 
int AltGui_Measure_Dia::get_selected_information(void){
	return measure_state2switch;
}