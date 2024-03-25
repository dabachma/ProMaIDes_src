#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Faulttree_Dike_Wid.h"

//Default constructor
FplGui_Faulttree_Dike_Wid::FplGui_Faulttree_Dike_Wid(DataRole role, QWidget *parent): _Sys_Abstract_Base_Wid(parent){
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;

	this->set_headLabel_text("Faulttree dike section");



	this->set_default_values();

	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/faulttree"));

			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
	QObject::connect(this->ui.checkBox_wind_wave, SIGNAL(clicked(bool )), SLOT(check_wind_wave(bool )));
	QObject::connect(this->ui.checkBox_ero_seaward_start, SIGNAL(clicked(bool )), SLOT(check_ero_waterside_start(bool )));
	QObject::connect(this->ui.checkBox_heave, SIGNAL(clicked(bool )), SLOT(check_heave(bool )));
	QObject::connect(this->ui.checkBox_pip_lane, SIGNAL(clicked(bool )), SLOT(check_pip_lane(bool )));
	QObject::connect(this->ui.checkBox_pip_sell, SIGNAL(clicked(bool )), SLOT(check_pip_sell(bool )));
	QObject::connect(this->ui.checkBox_pip_schmert, SIGNAL(clicked(bool )), SLOT(check_pip_schmert(bool )));


	ADD_MEM(FplGui_Faulttree_Dike_Wid, _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Faulttree_Dike_Wid::~FplGui_Faulttree_Dike_Wid(void){


	MIN_MEM(FplGui_Faulttree_Dike_Wid, _sys_system_modules::FPL_SYS);
}
//________
//public
//Set the member, which are displayed in the widget
void FplGui_Faulttree_Dike_Wid::set_member(_Sys_Abstract_Base_Wid *ptr){
		//upcast to derived object
	FplGui_Faulttree_Dike_Wid *other = (FplGui_Faulttree_Dike_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}

	this->ui.checkBox_ero_landward->setChecked(other->ui.checkBox_ero_landward->isChecked());
	this->ui.checkBox_ero_seaward->setChecked(other->ui.checkBox_ero_seaward->isChecked());
	this->ui.checkBox_ero_seaward_start->setChecked(other->ui.checkBox_ero_seaward_start->isChecked());
	this->ui.checkBox_heave->setChecked(other->ui.checkBox_heave->isChecked());
	this->ui.checkBox_overflow->setChecked(other->ui.checkBox_overflow->isChecked());
	this->ui.checkBox_overtopping->setChecked(other->ui.checkBox_overtopping->isChecked());
	this->ui.checkBox_pip_lane->setChecked(other->ui.checkBox_pip_lane->isChecked());
	this->ui.checkBox_pip_sell->setChecked(other->ui.checkBox_pip_sell->isChecked());

	this->ui.radioButton_sell->setChecked(other->ui.radioButton_sell->isChecked());
	this->ui.radioButton_lane->setChecked(other->ui.radioButton_lane->isChecked());
	this->ui.radioButton_schmert->setChecked(other->ui.radioButton_schmert->isChecked());

	this->ui.checkBox_pip_schmert->setChecked(other->ui.checkBox_pip_schmert->isChecked());
	this->ui.checkBox_slope_landward->setChecked(other->ui.checkBox_slope_landward->isChecked());
	this->ui.checkBox_slope_water->setChecked(other->ui.checkBox_slope_water->isChecked());
	this->ui.checkBox_wave_impact->setChecked(other->ui.checkBox_wave_impact->isChecked());
	this->ui.checkBox_wind_wave->setChecked(other->ui.checkBox_wind_wave->isChecked());

}
//Changes the editable state for all child widgets in the formular/dialog
void FplGui_Faulttree_Dike_Wid::set_editable(const bool state){
	this->editable = state;

	this->ui.checkBox_ero_landward->setEnabled(false);
	this->ui.checkBox_ero_seaward->setEnabled(state);
	this->ui.checkBox_ero_seaward_start->setEnabled(state);
	this->ui.checkBox_heave->setEnabled(state);
	this->ui.checkBox_overflow->setEnabled(false);
	this->ui.checkBox_overtopping->setEnabled(state);
	this->ui.checkBox_pip_lane->setEnabled(state);
	this->ui.radioButton_lane->setEnabled(state);
	this->ui.checkBox_pip_sell->setEnabled(state);
	this->ui.radioButton_sell->setEnabled(state);
	this->ui.checkBox_pip_schmert->setEnabled(state);
	this->ui.radioButton_schmert->setEnabled(state);
	this->ui.checkBox_slope_landward->setEnabled(state);
	this->ui.checkBox_slope_water->setEnabled(state);
	this->ui.checkBox_wave_impact->setEnabled(state);
	this->ui.checkBox_wind_wave->setEnabled(state);


	
	if(this->ui.checkBox_heave->isChecked()==false){
		this->ui.checkBox_pip_lane->setEnabled(false);
		this->ui.radioButton_lane->setEnabled(false);
		this->ui.checkBox_pip_sell->setEnabled(false);
		this->ui.radioButton_sell->setEnabled(false);
		this->ui.checkBox_pip_schmert->setEnabled(false);
		this->ui.radioButton_schmert->setEnabled(false);

		this->ui.checkBox_pip_lane->setChecked(false);
		this->ui.checkBox_pip_sell->setChecked(false);
		this->ui.radioButton_sell->setChecked(false);
		this->ui.checkBox_pip_schmert->setChecked(false);

	}

	if(this->ui.checkBox_wind_wave->isChecked()==false){
		this->ui.checkBox_ero_seaward->setEnabled(false);
		this->ui.checkBox_overtopping->setEnabled(false);
		this->ui.checkBox_wave_impact->setEnabled(false);
		this->ui.checkBox_overtopping->setChecked(false);
		this->ui.checkBox_ero_seaward->setChecked(false);
		this->ui.checkBox_wave_impact->setChecked(false);
	}
}
//Set default values
void FplGui_Faulttree_Dike_Wid::set_default_values(void){
	this->ui.checkBox_ero_landward->setChecked(true);
	this->ui.checkBox_ero_seaward->setChecked(true);
	this->ui.checkBox_ero_seaward_start->setChecked(true);
	this->ui.checkBox_heave->setChecked(true);
	this->ui.checkBox_overflow->setChecked(true);
	this->ui.checkBox_overtopping->setChecked(true);
	this->ui.checkBox_pip_lane->setChecked(true);
	this->ui.checkBox_pip_sell->setChecked(true);
	this->ui.radioButton_sell->setChecked(true);
	this->ui.checkBox_pip_schmert->setChecked(true);
	this->ui.checkBox_slope_landward->setChecked(true);
	this->ui.checkBox_slope_water->setChecked(true);
	this->ui.checkBox_wave_impact->setChecked(true);
	this->ui.checkBox_wind_wave->setChecked(true);
}
//Set the member of the widget
void FplGui_Faulttree_Dike_Wid::set_member(QSqlDatabase *ptr_database){
	int number_mecha=0;
	QSqlQueryModel model;

	this->ptr_database=ptr_database;

	//get data from database table
	try{
		number_mecha=_Fpl_Section_Type::select_mechanism(&model, this->ptr_database, _fpl_section_types::dike_sec);
	}
	catch(Error msg){
		msg.output_msg(0);
	}
	//read the data out and the the widgets
	//overflow
	string buffer;
	bool applied=false;
	bool in_tree=false;
	for(int i=0; i< number_mecha; i++) {

		buffer=model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name)).c_str()).toString().toStdString();
		applied=functions::convert_string2boolean(model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied)).c_str()).toString().toStdString());
		in_tree=functions::convert_string2boolean(model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree)).c_str()).toString().toStdString());
	
		if (buffer == fpl_label::mech_overflow)
		{
			this->ui.checkBox_overflow->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_wave_runup)
		{
			this->ui.checkBox_overtopping->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_bursting_din)
		{
			this->ui.checkBox_heave->setChecked(applied);
			if(applied==false){
				this->ui.checkBox_pip_lane->setEnabled(false);
				this->ui.radioButton_lane->setEnabled(false);
				this->ui.checkBox_pip_sell->setEnabled(false);
				this->ui.radioButton_sell->setEnabled(false);
				this->ui.checkBox_pip_schmert->setEnabled(false);
				this->ui.radioButton_schmert->setEnabled(false);

				this->ui.checkBox_pip_lane->setChecked(false);
				this->ui.checkBox_pip_sell->setChecked(false);
				this->ui.radioButton_sell->setChecked(false);
				this->ui.checkBox_pip_schmert->setChecked(false);

			}


		}
		else if (buffer == fpl_label::mech_piping_sell)
		{
			this->ui.checkBox_pip_sell->setChecked(applied);
			this->ui.radioButton_sell->setChecked(in_tree);
		}
		else if (buffer == fpl_label::mech_piping_lane)
		{
			this->ui.checkBox_pip_lane->setChecked(applied);
			this->ui.radioButton_lane->setChecked(in_tree);
		}
		else if (buffer == fpl_label::mech_piping_schmert)
		{
			this->ui.checkBox_pip_schmert->setChecked(applied);
			this->ui.radioButton_schmert->setChecked(in_tree);
		}
		else if (buffer == fpl_label::mech_slope_landside)
		{
			this->ui.checkBox_slope_landward->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_slope_waterside)
		{
			this->ui.checkBox_slope_water->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_erosion_landside)
		{
			this->ui.checkBox_ero_landward->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_erosion_waterside)
		{
			this->ui.checkBox_ero_seaward->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_wave_impact)
		{
			this->ui.checkBox_wave_impact->setChecked(applied);
		}
		else if (buffer == fpl_label::mech_erosion_waterside_start)
		{
			this->ui.checkBox_ero_seaward_start->setChecked(applied);
			if(applied==false){
				this->ui.checkBox_ero_seaward->setEnabled(false);
				this->ui.checkBox_ero_seaward->setChecked(false);
			}
		}
		
		else if (buffer == fpl_label::mech_wind_wave)
		{
			this->ui.checkBox_wind_wave->setChecked(applied);

			if(applied==false){
				this->ui.checkBox_ero_seaward->setEnabled(false);
				this->ui.checkBox_ero_seaward_start->setEnabled(false);
				this->ui.checkBox_overtopping->setEnabled(false);
				this->ui.checkBox_wave_impact->setEnabled(false);
				this->ui.checkBox_overtopping->setChecked(false);
				this->ui.checkBox_ero_seaward->setChecked(false);
				this->ui.checkBox_ero_seaward_start->setChecked(false);
				this->ui.checkBox_wave_impact->setChecked(false);
			}
		}
	}
}
//________
//public slots
//Open the dialog to edit values in the widget
void FplGui_Faulttree_Dike_Wid::show_as_dialog(void){
	Sys_Base_Edit_Dia dialog(NULL, this);
	FplGui_Faulttree_Dike_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	inDiaWid.set_editable(true);
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
//Check checkbox wind wave mechanism
void FplGui_Faulttree_Dike_Wid::check_wind_wave(bool clicked){

	this->ui.checkBox_ero_seaward->setEnabled(clicked);
	this->ui.checkBox_ero_seaward_start->setEnabled(clicked);
	this->ui.checkBox_overtopping->setEnabled(clicked);
	this->ui.checkBox_wave_impact->setEnabled(clicked);
	this->ui.checkBox_overtopping->setChecked(clicked);
	this->ui.checkBox_ero_seaward->setChecked(clicked);
	this->ui.checkBox_ero_seaward_start->setChecked(clicked);
	this->ui.checkBox_wave_impact->setChecked(clicked);
}
//Check checkbox start erosion waterside mechanism
void FplGui_Faulttree_Dike_Wid::check_ero_waterside_start(bool clicked){
	this->ui.checkBox_ero_seaward->setEnabled(clicked);
	this->ui.checkBox_ero_seaward->setChecked(clicked);
}
//Check checkbox heave mechanism
void FplGui_Faulttree_Dike_Wid::check_heave(bool clicked){
	this->ui.checkBox_pip_lane->setEnabled(clicked);
	this->ui.radioButton_lane->setEnabled(clicked);
	this->ui.checkBox_pip_sell->setEnabled(clicked);
	this->ui.radioButton_sell->setEnabled(clicked);
	this->ui.checkBox_pip_schmert->setEnabled(clicked);
	this->ui.radioButton_schmert->setEnabled(clicked);

	this->ui.checkBox_pip_lane->setChecked(clicked);
	this->ui.checkBox_pip_sell->setChecked(clicked);
	this->ui.radioButton_sell->setChecked(clicked);
	this->ui.checkBox_pip_schmert->setChecked(clicked);


}
//Check checkbox piping Sellmeijer mechanism
void FplGui_Faulttree_Dike_Wid::check_pip_sell(bool clicked){
	if(clicked==false && this->ui.radioButton_sell->isChecked()==true){
		this->ui.radioButton_sell->setChecked(false);
		if(this->ui.checkBox_pip_lane->isChecked()==true){
			this->ui.radioButton_lane->setChecked(true);
		}
		else if(this->ui.checkBox_pip_schmert->isChecked()==true){
			this->ui.radioButton_schmert->setChecked(true);
		}
	}
	if(clicked==true && this->ui.checkBox_pip_lane->isChecked()==false && this->ui.checkBox_pip_schmert->isChecked()==false){
		this->ui.radioButton_sell->setChecked(true);
	}

	
}
//Check checkbox piping Lane mechanism
void FplGui_Faulttree_Dike_Wid::check_pip_lane(bool clicked){
	if(clicked==false && this->ui.radioButton_lane->isChecked()==true){
		this->ui.radioButton_lane->setChecked(false);
		if(this->ui.checkBox_pip_sell->isChecked()==true){
			this->ui.radioButton_sell->setChecked(true);
		}
		else if(this->ui.checkBox_pip_schmert->isChecked()==true){
			this->ui.radioButton_schmert->setChecked(true);
		}
	}
	if(clicked==true && this->ui.checkBox_pip_sell->isChecked()==false && this->ui.checkBox_pip_schmert->isChecked()==false){
		this->ui.radioButton_lane->setChecked(true);
	}
}
//Check checkbox piping Schmertmann mechanism
void FplGui_Faulttree_Dike_Wid::check_pip_schmert(bool clicked){
	if(clicked==false && this->ui.radioButton_schmert->isChecked()==true){
		this->ui.radioButton_schmert->setChecked(false);
		if(this->ui.checkBox_pip_sell->isChecked()==true){
			this->ui.radioButton_sell->setChecked(true);
		}
		else if(this->ui.checkBox_pip_lane->isChecked()==true){
			this->ui.radioButton_lane->setChecked(true);
		}
		
	}
	if(clicked==true && this->ui.checkBox_pip_lane->isChecked()==false && this->ui.checkBox_pip_sell->isChecked()==false){
		this->ui.radioButton_schmert->setChecked(true);
	}
}
//________
//private
//Transfer members to database
void FplGui_Faulttree_Dike_Wid::transfer_members2database(FplGui_Faulttree_Dike_Wid *dialog){
	
	//the table is set (the name and the column names) and allocated
	try 
	{
		_Fpl_Section_Type::set_table_mechanism(this->ptr_database);
	}
	catch(Error msg) 
	{
		throw msg;
	}

	QSqlQueryModel query;
	ostringstream query_string;
	//set the new data to database
	//overflow mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_overflow->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_overflow<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//overtopping mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_overtopping->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_wave_runup<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//ero land mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_ero_landward->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_erosion_landside<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//ero water mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_ero_seaward->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_erosion_waterside<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//ero water mechanism start
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_ero_seaward_start->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_erosion_waterside_start<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//wind wave mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_wind_wave->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_wind_wave<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//heave mechanism 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_heave->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_bursting_din<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//slope land 
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_slope_landward->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_slope_landside<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//slope water
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_slope_water->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_slope_waterside<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//wave impact
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_wave_impact->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_wave_impact<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//piping sell
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_pip_sell->isChecked())<< " , ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->ui.radioButton_sell->isChecked())<< "  ";

	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_piping_sell<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//piping lane
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_pip_lane->isChecked())<< " , ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->ui.radioButton_lane->isChecked())<< "  ";

	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_piping_lane<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//piping schmertmann
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->ui.checkBox_pip_schmert->isChecked())<< " , ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->ui.radioButton_schmert->isChecked())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_piping_schmert<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid())
	{
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	this->set_member(dialog);
}
//set the error
Error FplGui_Faulttree_Dike_Wid::set_error(const int err_type){
		string place="FplGui_Faulttree_Dike_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(FplGui_Faulttree_Dike_Wid *dialog)") ;
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