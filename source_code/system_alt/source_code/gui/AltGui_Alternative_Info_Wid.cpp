#include "Alt_Headers_Precompiled.h"
//#include "AltGui_Alternative_Info_Wid.h"


///Default constructor
AltGui_Alternative_Info_Wid::AltGui_Alternative_Info_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent){

	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	this->ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;

	this->area_state=-1;

	this->ui.lineEdit_alt_id->setReadOnly(true);
	this->ui.lineEdit_detailed->setReadOnly(true);

	this->set_headLabel_text("Measure(s)");

	// SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
	}

	this->set_default_values();


	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Alternative_Info_Wid), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Alternative_Info_Wid::~AltGui_Alternative_Info_Wid(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Alternative_Info_Wid), _sys_system_modules::ALT_SYS);
}
//_________
//public
//Set the members via a widget of the same type
void AltGui_Alternative_Info_Wid::set_member(_Sys_Abstract_Base_Wid *ptr){
	//upcast to derived object
	AltGui_Alternative_Info_Wid*other = (AltGui_Alternative_Info_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}

	this->ui.lineEdit_alt_id->setText(other->ui.lineEdit_alt_id->text());
	this->ui.lineEdit_alt_name->setText(other->ui.lineEdit_alt_name->text());
	this->ui.textEdit_alt_description->setText(other->ui.textEdit_alt_description->toPlainText());
	this->ui.lineEdit_detailed->setText(other->ui.lineEdit_detailed->text());
	this->area_state=other->area_state;

	if(other->ui.radioButton_fpl->isChecked()==true){
		this->ui.radioButton_fpl->setChecked(true);
		this->ui.radioButton_dam->setEnabled(false);
		this->ui.radioButton_hyd->setEnabled(false);
	}
	else if(other->ui.radioButton_dam->isChecked()==true){
		this->ui.radioButton_dam->setChecked(true);
		this->ui.radioButton_fpl->setEnabled(false);
		this->ui.radioButton_hyd->setEnabled(false);
	}
	else if(other->ui.radioButton_hyd->isChecked()==true){
		this->ui.radioButton_hyd->setChecked(true);
		this->ui.radioButton_dam->setEnabled(false);
		this->ui.radioButton_fpl->setEnabled(false);
	}
}
//Changes the editable state for all child widgets in the formular/dialog
void AltGui_Alternative_Info_Wid::set_editable(const bool state){

	this->ui.lineEdit_alt_name->setReadOnly(!state);
	this->ui.textEdit_alt_description->setReadOnly(!state);

}
//Set the member of the widget
void AltGui_Alternative_Info_Wid::set_member(QSqlDatabase *ptr_database, const int id_measure, const int id_areastate){
	
	_alt_general_info data;
	_sys_system_id buffer;
	buffer.area_state=id_areastate;
	buffer.measure_nr=id_measure;

	this->area_state=id_areastate;
	this->ptr_database=ptr_database;

	//set not editable
	this->ui.lineEdit_alt_name->setReadOnly(true);
	this->ui.textEdit_alt_description->setReadOnly(true);


	//set data
	if(Alt_System::check_measure_is_set_database(ptr_database, buffer, &data)==true){
		stringstream buff_id;
		buff_id << data.id.measure_nr;
		this->ui.lineEdit_alt_id->setText(buff_id.str().c_str());
		this->ui.lineEdit_alt_name->setText(data.name.c_str());
		this->ui.textEdit_alt_description->setText(data.description.c_str());
		this->ui.lineEdit_detailed->setText(data.detailed_type.c_str());
		if(data.category==_alt_measure_category::category_fpl){
			this->ui.radioButton_fpl->setChecked(true);
			this->ui.radioButton_dam->setEnabled(false);
			this->ui.radioButton_hyd->setEnabled(false);
		}
		else if(data.category==_alt_measure_category::category_dam){
			this->ui.radioButton_dam->setChecked(true);
			this->ui.radioButton_fpl->setEnabled(false);
			this->ui.radioButton_hyd->setEnabled(false);
		}
		else if(data.category==_alt_measure_category::category_hyd){
			this->ui.radioButton_hyd->setChecked(true);
			this->ui.radioButton_dam->setEnabled(false);
			this->ui.radioButton_fpl->setEnabled(false);
		}

	}
	else{
		this->ui.lineEdit_alt_id->setText("-1");
		this->ui.lineEdit_alt_name->setText("No measure is found!");

	}
}
//Set default values
void AltGui_Alternative_Info_Wid::set_default_values(void){
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");

	this->ui.lineEdit_alt_id->setText("-1");
	this->ui.lineEdit_alt_name->setText("No measure is found!");
}
//_______
//public slots
//Open the dialog to edit values in the widget
void AltGui_Alternative_Info_Wid::show_as_dialog(void){
	Sys_Base_Edit_Dia dialog(NULL, this);
	AltGui_Alternative_Info_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	//accepted
	if(dialog.start_dialog()==true){
		//transfer members to database
		this->transfer_members2database(&inDiaWid);
		
	}
	//rejected
	else{
		return;	
	}
}
//Transfer members to database
void AltGui_Alternative_Info_Wid::transfer_members2database(AltGui_Alternative_Info_Wid *dialog){
//the table is set (the name and the column names) and allocated
	try{
		Alt_System::set_table_measure(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Alt_System::table_measure->get_table_name();
	query_string << " SET " ;
	query_string  << Alt_System::table_measure->get_column_name(alt_label::measure_name) << " = '"<< dialog->ui.lineEdit_alt_name->text().toStdString()<< "' , ";
	query_string  << Alt_System::table_measure->get_column_name(label::description) << " = '"<< dialog->ui.textEdit_alt_description->toPlainText().toStdString()<< "' ";
	query_string  << " WHERE ";
	query_string  << Alt_System::table_measure->get_column_name(label::areastate_id) << " = "  << dialog->area_state;
	query_string  << " AND ";
	query_string  << Alt_System::table_measure->get_column_name(label::measure_id) << " = "  << dialog->ui.lineEdit_alt_id->text().toInt();

	
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Alt_System::table_measure->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	if(dialog->ui.lineEdit_alt_name->text()!=this->ui.lineEdit_alt_name->text()){
		ostringstream buffer_name;
		string name;
		buffer_name<<dialog->ui.lineEdit_alt_id->text().toInt()<<"-"<<dialog->ui.lineEdit_alt_name->text().toStdString();
		name=buffer_name.str();
		if(id_item!=NULL){
			this->id_item->setText(0, name.c_str());
		}
	}

	this->set_member(dialog);

}
//Set the error(s)
Error AltGui_Alternative_Info_Wid::set_error(const int err_type){
	string place="AltGui_Alternative_Info_Wid::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(AltGui_Alternative_Info_Wid *dialog)") ;
			reason="Can not submit the query to the database";
			help="Check the query error";
			type=2;	
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