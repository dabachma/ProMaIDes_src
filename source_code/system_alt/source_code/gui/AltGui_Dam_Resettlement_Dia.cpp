#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Dam_Resettlement_Dia.h"


//Default constructor
AltGui_Dam_Resettlement_Dia::AltGui_Dam_Resettlement_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));

		//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Dam_Resettlement_Dia), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Dam_Resettlement_Dia::~AltGui_Dam_Resettlement_Dia(void){


	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Dam_Resettlement_Dia), _sys_system_modules::ALT_SYS);
}
//________
//public
//Set for the dialog the function/categories, which should be switched
void AltGui_Dam_Resettlement_Dia::set_dialog(QSqlDatabase *database){

	QSqlTableModel result(NULL, *database);
	int number=0;
	try{
		number=Dam_Eco_Btype::select_relevant_biotope_types_database(&result, false);
	}
	catch(Error msg){
		ostringstream info;
		info <<"Try to set up the DAM-resettlement dialog for measure's implementation"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	this->comboBox_btype->clear();
	
	
	QStringList list;
	ostringstream buff_txt;
	buff_txt << "-1   no biotope-type";
	list.append(buff_txt.str().c_str());

	for(int i=0; i< number; i++){
		buff_txt.str("");
		buff_txt << result.record(i).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_id)).c_str()).toInt() << "   ";
		buff_txt << result.record(i).value((Dam_Eco_Btype::table->get_column_name(dam_label::biotope_type_name)).c_str()).toString().toStdString();
		list.append(buff_txt.str().c_str());
	}

	this->comboBox_btype->addItems(list);
	this->comboBox_btype->insertSeparator(1);


}
//Get the selected informations 
void AltGui_Dam_Resettlement_Dia::get_selected_information(QStringList *file_list, QList<int> *id_list){
	
	QString buffer;
	QStringList split_list;
	
	//file
	buffer=widget_file->fileName();
	file_list->append(buffer);

	//id existing
	buffer=this->comboBox_btype->currentText();
	split_list=buffer.split(" ");
	buffer=split_list.at(0);
	id_list->append(buffer.toInt());

}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Dam_Resettlement_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//set the data

		return true;
	}
}