#include "Hyd_Headers_Precompiled.h"
//#include "HydGui_Boundary_Scenario_Dia.h"

//Default constructor
HydGui_Boundary_Scenario_Dia::HydGui_Boundary_Scenario_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	ostringstream txt;
	txt <<"Nothing set "<< endl;
	this->label_header->setText(txt.str().c_str());
	this->items=NULL;
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->checkBox_all_select, SIGNAL(clicked(bool )), this, SLOT(all_selection(bool )));

	this->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	this->listWidget->setToolTip("Id       Name      Annuality [a]     Eventprobability [-]"); 

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Boundary_Scenario_Dia), _sys_system_modules::HYD_SYS);

}
//Default destructor
HydGui_Boundary_Scenario_Dia::~HydGui_Boundary_Scenario_Dia(void){

	if(this->items!=NULL){
		delete []this->items;
		this->items=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Boundary_Scenario_Dia), _sys_system_modules::HYD_SYS);
}
//__________
//public
//Start the dialog; it returns true by acception, false by rejection
bool HydGui_Boundary_Scenario_Dia::start_dialog(Hyd_Boundary_Szenario *ptr_scenarios, const int number){

	ostringstream buffer;
	buffer<<"Number of available scenario(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		ptr_scenarios[i].set_is_selected(false);
		itemtxt << ptr_scenarios[i].get_sz_text();

		this->items[i].setText(itemtxt.str().c_str());
		this->listWidget->addItem(&this->items[i]);

	}


	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		for(int i=0; i<this->listWidget->count(); i++){
			QListWidgetItem *buff;

			buff=this->listWidget->item(i);

			if(buff->isSelected()==true){
				int id_buff=0;
				stringstream buff_str; 
				buff_str << buff->text().toStdString();
				buff_str >> id_buff;
				//find the scneario id and set it as selected
				for(int j=0; j< number; j++){
					if(id_buff==ptr_scenarios[j].get_id()){
						ptr_scenarios[j].set_is_selected(true);
					}

				}
				
			}
		}
		
		return true;
	}

}
///Set a text to the text label of the dialog
void HydGui_Boundary_Scenario_Dia::set_txt(const string txt){
	this->header_txt=txt;
}
//_________
//public sots
//Switch the selection between all and clear selection
void HydGui_Boundary_Scenario_Dia::all_selection(bool flag){
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
void HydGui_Boundary_Scenario_Dia::allocate_list_item(const int number){
	this->items=new QListWidgetItem[number];

}