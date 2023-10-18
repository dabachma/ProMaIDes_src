#include "Dam_Headers_Precompiled.h"
//#include "DamGui_Raster_Dia.h"


//Default constructor
DamGui_Raster_Dia::DamGui_Raster_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);
	ostringstream txt;
	txt <<"Nothing set "<< endl;
	this->label_header->setText(txt.str().c_str());
	this->items=NULL;
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->checkBox_all_select, SIGNAL(clicked(bool )), this, SLOT(all_selection(bool )));

	this->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	this->listWidget->setToolTip("Id             Name      "); 

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Raster_Dia), _sys_system_modules::DAM_SYS);

}
//Default destructor
DamGui_Raster_Dia::~DamGui_Raster_Dia(void){

	if(this->items!=NULL){
		delete []this->items;
		this->items=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Raster_Dia), _sys_system_modules::DAM_SYS);

}
//__________
//public
//Start the dialog and set the current available economical raster as a selection; it returns true by acception, false by rejection
bool DamGui_Raster_Dia::start_dialog(Dam_Ecn_Raster *raster, const int number){



	ostringstream buffer;
	buffer<<"Number of available raster(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		raster[i].set_raster_is_selected(false);
		itemtxt << raster[i].get_raster_number() << "   "<< raster[i].get_raster_name();

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
					if(id_buff==raster[j].get_raster_number()){
						raster[j].set_raster_is_selected(true);
					}
				}
			}
		}
		return true;
	}
}
//Start the dialog and set the current available ecological biotope raster as a selection; it returns true by acception, false by rejection
bool DamGui_Raster_Dia::start_dialog(Dam_Eco_Btype_Raster *raster, const int number){
	ostringstream buffer;
	buffer<<"Number of available raster(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	QIcon icon;
	icon.addFile(":eco_btype_icon");
	this->setWindowIcon(icon);

	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		raster[i].set_raster_is_selected(false);
		itemtxt << raster[i].get_raster_number() << "   "<< raster[i].get_raster_name();

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
					if(id_buff==raster[j].get_raster_number()){
						raster[j].set_raster_is_selected(true);
					}
				}
			}
		}
		return true;
	}
}
//Start the dialog and set the current available ecological soil erosion raster as a selection; it returns true by acception, false by rejection
bool DamGui_Raster_Dia::start_dialog(Dam_Eco_Soil_Raster *raster, const int number){
	ostringstream buffer;
	buffer<<"Number of available raster(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	QIcon icon;
	icon.addFile(":eco_soil_icon");
	this->setWindowIcon(icon);

	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		raster[i].set_raster_is_selected(false);
		itemtxt << raster[i].get_raster_number() << "   "<< raster[i].get_raster_name();

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
					if(id_buff==raster[j].get_raster_number()){
						raster[j].set_raster_is_selected(true);
					}
				}
			}
		}
		return true;
	}
}
//Start the dialog and set the current available people2risk raster as a selection; it returns true by acception, false by rejection
bool DamGui_Raster_Dia::start_dialog(Dam_People_Raster *raster, const int number){
	ostringstream buffer;
	buffer<<"Number of available raster(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	QIcon icon;
	icon.addFile(":pop_icon");
	this->setWindowIcon(icon);

	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		raster[i].set_raster_is_selected(false);
		itemtxt << raster[i].get_raster_number() << "   "<< raster[i].get_raster_name();

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
					if(id_buff==raster[j].get_raster_number()){
						raster[j].set_raster_is_selected(true);
					}
				}
			}
		}
		return true;
	}
}
//Start the dialog and set the current available psycho-social raster as a selection; it returns true by acception, false by rejection
bool DamGui_Raster_Dia::start_dialog(Dam_Pys_Raster *raster, const int number){
	ostringstream buffer;
	buffer<<"Number of available raster(s): " <<number << endl;
	this->header_txt.append(buffer.str());
	this->label_header->setText(this->header_txt.c_str());

	QIcon icon;
	icon.addFile(":pys_icon");
	this->setWindowIcon(icon);


	this->allocate_list_item(number);

	for(int i=0; i< number; i++){
		ostringstream itemtxt;
		raster[i].set_raster_is_selected(false);
		itemtxt << raster[i].get_raster_number() << "   "<< raster[i].get_raster_name();

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
					if(id_buff==raster[j].get_raster_number()){
						raster[j].set_raster_is_selected(true);
					}
				}
			}
		}
		return true;
	}
}
//Set a text to the text label of the dialog
void DamGui_Raster_Dia::set_txt(const string txt){
	this->header_txt=txt;
}
//_________
//public sots
//Switch the selection between all and clear selection
void DamGui_Raster_Dia::all_selection(bool flag){
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
void DamGui_Raster_Dia::allocate_list_item(const int number){
	this->items=new QListWidgetItem[number];

}