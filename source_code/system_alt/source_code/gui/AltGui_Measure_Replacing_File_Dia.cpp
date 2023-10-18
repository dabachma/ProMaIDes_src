#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Measure_Replacing_File_Dia.h"

//Default constructor
AltGui_Measure_Replacing_File_Dia::AltGui_Measure_Replacing_File_Dia(QWidget *parent): QDialog(parent){
	this->setupUi(this);

	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->spinBox, SIGNAL(valueChanged(int)), this, SLOT(recieve_number_spin_box(int )));

	this->ptr_database=NULL;
	this->system_id.measure_nr=0;
	this->system_id.area_state=0;

	this->tableWidget->setColumnCount(1);
	//this->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
	this->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Select replacing files"));
	this->tableWidget->setAlternatingRowColors(true);


	
	this->old_spinbox_value=0;
	this->spinBox->setEnabled(false);


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Measure_Replacing_File_Dia), _sys_system_modules::ALT_SYS);
}
//Default destructo
AltGui_Measure_Replacing_File_Dia::~AltGui_Measure_Replacing_File_Dia(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Measure_Replacing_File_Dia), _sys_system_modules::ALT_SYS);
}
//_______
//public
//Set for the dialog the measure identifier
void AltGui_Measure_Replacing_File_Dia::set_dialog(const _alt_general_info data){
	this->measure_info=data;
	this->system_id.area_state=data.id.area_state;
	this->system_id.measure_nr=data.id.measure_nr;
	this->set_measure_type2dia();
}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Measure_Replacing_File_Dia::start_dialog(void){
	this->tableWidget->resizeColumnsToContents();
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
//Get the selected informations 
void AltGui_Measure_Replacing_File_Dia::get_selected_information(QStringList *file_list, QList<int> *id_list){
	AltGui_File_Replacing_Wid* input_widget=NULL;
	QString buffer;
	QStringList split_list;
	for (int i = 0; i < this->tableWidget->rowCount(); i++){

		input_widget = qobject_cast<AltGui_File_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
		buffer=input_widget->get_file_name();
		file_list->append(buffer);
		buffer=input_widget->get_ptr_combo_box()->currentText();
		split_list=buffer.split(" ");
		buffer=split_list.at(0);
		id_list->append(buffer.toInt());



	}


}
//Set the pointer to the database
void AltGui_Measure_Replacing_File_Dia::set_ptr_database(QSqlDatabase *database){
	this->ptr_database=database;
}
//______
//private slots
//Check the value of the spinbox
void AltGui_Measure_Replacing_File_Dia::recieve_number_spin_box(int number){
	if(abs(number-this->old_spinbox_value)<=1){
		this->tableWidget->setRowCount(number);
		
		if(number>this->old_spinbox_value){
			this->set_row_item(true);
		}
		else{
			this->set_row_item(false);
		}
		this->old_spinbox_value=number;
	}
	else{
		this->spinBox->setValue(this->old_spinbox_value);
	}
}
//_______
//private
//Set the measure specific data for the dialog
void AltGui_Measure_Replacing_File_Dia::set_measure_type2dia(void){

	//set combo list
	_sys_system_id buff_id;
	buff_id.measure_nr=0;
	buff_id.area_state=this->system_id.area_state;
	ostringstream text;

	//hyd replacing
	if(this->measure_info.detailed_type==alt_replace_hyd::boundary_sc){
		
		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the replacing HYD-boundary scenario(s)"<<endl;
		text << "and specifiy the existing HYD-boundary scenario, which should"<<endl;
		text << "be replaced!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);
		//set tool tip
		text.str("");
		text<<"Id       Name      Annuality [a]     Eventprobability [-]";
		tooltip_combo=text.str().c_str();

		//set hyd boundaries
		Hyd_Boundary_Szenario_Management sc_buff;
		sc_buff.set_systemid(buff_id);
		sc_buff.set_existing_szenario_per_db(this->ptr_database);

		for(int i=0; i< sc_buff.get_number_sz(); i++){
			this->combo_list.append(sc_buff.get_ptr_sz(i)->get_sz_text().c_str());
		}
		this->spinBox->setRange(0,sc_buff.get_number_sz());
		
	}
	else if(this->measure_info.detailed_type==alt_replace_hyd::dike_lines){
		
		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the replacing HYD-2D-dikeline(s) (just 1 dikeline with index 0)"<<endl;
		text << "and specifiy the existing HYD-2D-dikeline, which should"<<endl;
		text << "be replaced!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);
		//set tool tip
		text.str("");
		text<<"Id      Name       FP-No.     ";
		tooltip_combo=text.str().c_str();

		//set hyd-dikeline to combo-box
		this->set_hyd_dikeline2list(buff_id);
		
	}
	else if(this->measure_info.detailed_type==alt_replace_hyd::river_profiles){
		
		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file of the replacing HYD-river profile(s)"<<endl;
		text << "and specifiy the existing HYD-River Model, where the profile(s) should"<<endl;
		text << "be replaced!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);
		
		
		//set tool tip
		text.str("");
		text<<"Id      Name     RV-No.     ";
		tooltip_combo=text.str().c_str();

		//set hyd-river to combo-box
		this->set_hyd_rv_model2list(buff_id);
		
	}
	//hyd_add
	else if(this->measure_info.detailed_type==alt_add_hyd::dike_lines){
		
		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the new HYD-2D-dikeline(s) (just 1 dikeline with index 0)!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);
		//set tool tip
		text.str("");
		text<<"Global-Id       FP-No.     ";
		tooltip_combo=text.str().c_str();

		//set spin box
		this->spinBox->setEnabled(true);

		this->combo_list.append(alt_label::adding_item.c_str());
		
	}


	//fpl combi
	else if(this->measure_info.detailed_type==alt_combi_fpl::fpl_line){

		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the new/replacing FPL-section(s)"<<endl;
		text << "and specifiy -in case of a replacement- the existing FPL-section, "<<endl;
		text << "which should be replaced!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);

		//set tool tip
		text.str("");
		text<<"Id      Name     Start Riverstation [m]     Type    Bank";
		tooltip_combo=text.str().c_str();

		//set fpl-section
		this->combo_list.append(alt_label::adding_item.c_str());
		this->set_fpl_section2list(buff_id);


	}


	//fpl replace
	else if(this->measure_info.detailed_type==alt_replace_fpl::section){

		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the replacing FPL-section(s)"<<endl;
		text << "and specifiy the existing FPL-section, which should"<<endl;
		text << "be replaced!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);

		//set tool tip
		text.str("");
		text<<"Id      Name     Start Riverstation [m]     Type    Bank";
		tooltip_combo=text.str().c_str();

		//set fpl-section
		this->set_fpl_section2list(buff_id);


	}
	//replace fpl-section parameter
	else if(this->measure_info.detailed_type==alt_replace_fpl::section_param){

		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file(s) of the replacing FPL-section parameters"<<endl;
		text << "and specifiy the existing FPL-section, which parameters should"<<endl;
		text << "be replaced!"<<endl;
		text << "The height of the section and the location are not affected!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);

		//set tool tip
		text.str("");
		text<<"Id      Name     Start Riverstation [m]     Type    Bank";
		tooltip_combo=text.str().c_str();

		//change the measure state to the bosed_on measure state
		buff_id.measure_nr=this->measure_info.measurestate_based_on;

		//set fpl-section
		this->set_fpl_section2list(buff_id);


	}
	//fpl add
	else if(this->measure_info.detailed_type==alt_add_fpl::section){

		text << "Set for measure "<< this->measure_info.name << " ("<<this->measure_info.detailed_type<<")"<<endl;
		text << "the file of the new FPL-section(s)!"<<endl;
		this->head_label->setText(text.str().c_str());
		//set spin box
		this->spinBox->setEnabled(true);

		this->combo_list.append(alt_label::adding_item.c_str());

	}
	

}
//Set the widget for every rows
void AltGui_Measure_Replacing_File_Dia::set_row_item(const bool add){
	
	AltGui_File_Replacing_Wid* input_widget=NULL;
	for (int i = 0; i < this->tableWidget->rowCount(); i++){
		if(add==true){
			AltGui_File_Replacing_Wid *rowItem = new AltGui_File_Replacing_Wid(this);

			//! Set the FileChooser widget at every rows of the table
			this->tableWidget->setCellWidget(this->tableWidget->rowCount()-1, 0, rowItem);
			this->tableWidget->setCurrentCell(this->tableWidget->rowCount(), 0);
			this->tableWidget->setRowHeight(i, 60);
			if(i<this->tableWidget->rowCount()-1){
				input_widget = qobject_cast<AltGui_File_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
				input_widget->get_ptr_combo_box()->setEnabled(false);
			}	
		}
		else{
			if(i==this->tableWidget->rowCount()-1){
				input_widget = qobject_cast<AltGui_File_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
				input_widget->get_ptr_combo_box()->setEnabled(true);
			}
		}
		
	}
	if(this->tableWidget->rowCount()>0){
		this->set_combox_items(this->tableWidget->rowCount()-1);
	}
	this->tableWidget->resizeColumnsToContents();
	
}
//Set the items of the combo box after the generated string list
void AltGui_Measure_Replacing_File_Dia::set_combox_items(const int index){

	QStringList buffer;
	buffer=this->combo_list;
	AltGui_File_Replacing_Wid* input_widget=NULL;
	QString delete_str;

	for (int i = 0; i < this->tableWidget->rowCount()-1; i++){
		input_widget = qobject_cast<AltGui_File_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
		delete_str=input_widget->get_ptr_combo_box()->currentText();
		if(delete_str!=alt_label::adding_item.c_str()){
			buffer.removeAt(buffer.indexOf(delete_str));
		}

	}

	input_widget = qobject_cast<AltGui_File_Replacing_Wid*>(this->tableWidget->cellWidget(index,0));
	input_widget->set_combobox_items(buffer);
	input_widget->get_ptr_combo_box()->setToolTip(this->tooltip_combo);
}
//Set the fpl-sections to the combo-box list
void AltGui_Measure_Replacing_File_Dia::set_fpl_section2list(_sys_system_id id){

	QSqlQueryModel result;
	int number=0;
	Fpl_Section *my_section=NULL;

	if(id.measure_nr>0){
		number=Fpl_Section::select_relevant_section_database(&result, this->ptr_database, id, false);
		if(this->measure_info.detailed_type==alt_replace_fpl::section || this->measure_info.detailed_type==alt_replace_fpl::section_param){
			this->spinBox->setRange(0,number);
		}
	}
	else{
		id.measure_nr=0;
		number=Fpl_Section::select_section_database(&result, this->ptr_database, id);
		if(this->measure_info.detailed_type==alt_replace_fpl::section || this->measure_info.detailed_type==alt_replace_fpl::section_param){
			this->spinBox->setRange(0,number);
		}
	}

	try{
		my_section=new Fpl_Section[number];
		for(int i=0; i< number; i++){
			my_section[i].set_systemid(id);
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	
	//set the section
	for(int i=0; i< number;i++){
		if(id.measure_nr>0){
			my_section[i].input_section_perdatabase(&result, i, this->ptr_database);
		}
		else{
			my_section[i].input_section_perdatabase(&result, i, this->ptr_database, false);
		}
		this->combo_list.append(my_section[i].get_string_selection().c_str());
	}

	if(my_section!=NULL){
		delete []my_section;
		my_section=NULL;
	}
}
//Set the hyd-2d-dikeline to the combo-box list
void AltGui_Measure_Replacing_File_Dia::set_hyd_dikeline2list(const _sys_system_id id){

	QSqlQueryModel result;
	int number=0;
	number=Hyd_Floodplain_Polysegment::select_polysegments_database(&result, this->ptr_database, id, false, true);

	//set the section
	for(int i=0; i< number;i++){
		ostringstream info;
		info << result.record(i).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_glob_id)).c_str()).toInt();
		info <<"   "<<result.record(i).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::polyseg_name)).c_str()).toString().toStdString();
		info <<"   "<< result.record(i).value((Hyd_Floodplain_Polysegment::polysegment_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();
		this->combo_list.append(info.str().c_str());
	}
	if(this->measure_info.detailed_type==alt_replace_hyd::dike_lines){
		this->spinBox->setRange(0,number);
	}
}
//Set the hyd-river models to the combo-box list
void AltGui_Measure_Replacing_File_Dia::set_hyd_rv_model2list(const _sys_system_id id){

	QSqlTableModel result(0, *this->ptr_database);
	int number=0;
	number=Hyd_Model_River::select_relevant_model_database(&result, id, false);

	//set the section
	for(int i=0; i< number;i++){
		ostringstream info;
		info << i << "    ";
		info <<"   "<<result.record(i).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString();
		info <<"   "<< result.record(i).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id)).c_str()).toInt();
		this->combo_list.append(info.str().c_str());
	}
	if(this->measure_info.detailed_type==alt_replace_hyd::river_profiles){
		this->spinBox->setRange(0,number);
	}


}
//Set error(s)
Error AltGui_Measure_Replacing_File_Dia::set_error(const int err_type){
		string place="AltGui_Measure_Replacing_File_Dia::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_fpl_section2list(const _sys_system_id id)");
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