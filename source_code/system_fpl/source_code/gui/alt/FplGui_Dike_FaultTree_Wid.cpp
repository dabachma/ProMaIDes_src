//#include "FplGui_Dike_FaultTree_Wid.h"
#include "Fpl_Headers_Precompiled.h"

FplGui_Dike_FaultTree_Wid::FplGui_Dike_FaultTree_Wid(DataRole role, QWidget *parent) : FplGui_FaultTree_BaseWid(parent) 
{
	//Sys_Abstract_BaseWid stuff
	//Set the widgets header
	set_headLabel_text("Dike Faulttree");
	set_child(view);
	//FplGui_FaultTree_BaseWid stuff
	setup_items();
	set_item_position_to_defaults();
	connect(arrangeAction, SIGNAL(triggered()), this, SLOT(set_item_position_to_defaults()));

	//Macros defined in _Sys_Abstract_Base_Wid.h
	DATA_ROLE_CONSTRUCTOR_STUFF();
	ADD_MEM(FplGui_Dike_FaultTree_Wid, _sys_system_modules::FPL_SYS);
}
FplGui_Dike_FaultTree_Wid::~FplGui_Dike_FaultTree_Wid(void) 
{
	MIN_MEM(FplGui_Dike_FaultTree_Wid, _sys_system_modules::FPL_SYS);
}
//______
//public
void FplGui_Dike_FaultTree_Wid::set_overflow_active(const bool active) 
{
	if (active == is_overflow_active())
		return;

	if (active) 
	{
		overflowItem->set_active_button(tr("Overflow"));
	}
	else
	{
		overflowItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_overflow_active(void) const 
{
	return overflowItem->get_state();
}
//
void FplGui_Dike_FaultTree_Wid::set_waveOvertopping_active(const bool active) 
{
	if (active == is_waveOvertopping_active())
		return;

	if (active) 
	{
		waveOvertoppingItem->set_active_button(tr("Wave Overtopping"));
	}
	else
	{
		waveOvertoppingItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_waveOvertopping_active(void) const 
{
	return waveOvertoppingItem->get_state();
}
//
void FplGui_Dike_FaultTree_Wid::set_bursting_active(const bool active) 
{
	if (active == is_bursting_active())
		return;

	if (active) 
	{
		burstingItem->set_active_button(tr("Bursting"));
	}
	else
	{
		burstingItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_bursting_active(void) const 
{
	return burstingItem->get_state();
}
//
void FplGui_Dike_FaultTree_Wid::set_piping_sellmeijer_active(const bool active) 
{
	if (active == is_piping_sellmeijer_active())
		return;

	if (active) 
	{
		pipingItem->set_active_button(tr("Piping Sellmeijer"));
	}
	else
	{
		pipingItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_piping_sellmeijer_active(void) 
{
	QString button = pipingItem->get_active_button_text();
	if (button == "Piping Sellmeijer"){
		return true;
	}

	return false;
}
//
void FplGui_Dike_FaultTree_Wid::set_piping_lane_active(const bool active) 
{
	if (active == is_piping_lane_active())
		return;

	if (active) 
	{
		pipingItem->set_active_button("Piping Lane");
	}
	else
	{
		pipingItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_piping_lane_active(void)  
{
	QString button = pipingItem->get_active_button_text();
	if (button == "Piping Lane"){
		return true;
	}

	return false;
}
//
void FplGui_Dike_FaultTree_Wid::set_piping_rename_active(const bool active)
{
	if (active == is_piping_rename_active())
		return;

	if (active) 
	{
		pipingItem->set_active_button("Piping Schmertmann");
	}
	else
	{
		pipingItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_piping_rename_active(void) 
{
	QString button = pipingItem->get_active_button_text();
	if (button == "Piping Schmertmann"){
		return true;
	}

	return false;
}
//
void FplGui_Dike_FaultTree_Wid::set_slope_landside_active(const bool active) 
{
	if (active == is_slope_landside_active())
		return;

	if (active) 
	{
		slopeLandsideItem->set_active_button(tr("Slope Landside"));
	}
	else
	{
		slopeLandsideItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_slope_landside_active(void) const 
{
	return slopeLandsideItem->get_state();
}
//
void FplGui_Dike_FaultTree_Wid::set_slope_waterside_active(const bool active)
{
	if (active == is_slope_waterside_active())
		return;

	if (active) 
	{
		slopeWatersideItem->set_active_button(tr("Slope Waterside"));
	}
	else
	{
		slopeWatersideItem->set_active_button(-1);
	}
}
//
bool FplGui_Dike_FaultTree_Wid::is_slope_waterside_active(void) const
{
	return slopeWatersideItem->get_state();
}
//
void FplGui_Dike_FaultTree_Wid::reset(void)
{
	//set all items to false state
	this->set_bursting_active(false);
	this->set_waveOvertopping_active(false);
	this->set_overflow_active(false);
	this->set_piping_sellmeijer_active(false);
	this->set_piping_lane_active(false);
	this->set_piping_rename_active(false);
	this->set_slope_landside_active(false);
	this->set_slope_waterside_active(false);
}
//Method to set the whole widget editable
void FplGui_Dike_FaultTree_Wid::set_editable(const bool editable) 
{
	this->editable = editable;
	set_items_checkable(editable);
}
//Set all members via database table
void FplGui_Dike_FaultTree_Wid::set_member(QSqlDatabase *ptr_database){
	int number_mecha=0;
	QSqlQueryModel model;

	this->ptr_database=ptr_database;

	//get data from database table
	number_mecha=_Fpl_Section_Type::select_relevant_mechanism(&model, this->ptr_database, _fpl_section_types::dike_sec);

	//read the data out and the the widgets
	//overflow
	string buffer;
	for(int i=0; i< number_mecha; i++) {

		buffer=model.record(i).value((_Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name)).c_str()).toString().toStdString();
		
		if (buffer == fpl_label::mech_overflow)
		{
			this->set_overflow_active(true);
		}
		else if (buffer == fpl_label::mech_wave_runup)
		{
			this->set_waveOvertopping_active(true);
		}
		else if (buffer == fpl_label::mech_bursting_din)
		{
			this->set_bursting_active(true);
		}
		else if (buffer == fpl_label::mech_piping_sell)
		{
			this->set_piping_sellmeijer_active(true);
		}
		else if (buffer == fpl_label::mech_piping_lane)
		{
			this->set_piping_lane_active(true);
		}
		//else if (buffer == fpl_label::mech_piping_rename)
		//{
		//	this->set_piping_rename_active(true);
		//}
		else if (buffer == fpl_label::mech_slope_landside)
		{
			this->set_slope_landside_active(true);
		}
		else if (buffer == fpl_label::mech_slope_waterside)
		{
			this->set_slope_waterside_active(true);
		}
	}
}
//Set all members of the widget, similar to a copy constructor
void FplGui_Dike_FaultTree_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) 
{
	//upcast to derived object
	FplGui_Dike_FaultTree_Wid *other = (FplGui_Dike_FaultTree_Wid *) ptr;
	if (other == NULL) 
	{
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	reset();
	//
	set_overflow_active(other->is_overflow_active());
	set_waveOvertopping_active(other->is_waveOvertopping_active());
	set_bursting_active(other->is_bursting_active());
	set_piping_sellmeijer_active(other->is_piping_sellmeijer_active());
	set_piping_lane_active(other->is_piping_lane_active());
	set_piping_rename_active(other->is_piping_rename_active());
	set_slope_landside_active(other->is_slope_landside_active());
	set_slope_waterside_active(other->is_slope_waterside_active());

	failureItem->set_pos(other->failureItem->get_absolute_center());
	forkItemMid->set_pos(other->forkItemMid->get_absolute_center());
	forkItemLeft->set_pos(other->forkItemLeft->get_absolute_center());
	overflowItem->set_pos(other->overflowItem->get_absolute_center());
	waveOvertoppingItem->set_pos(other->waveOvertoppingItem->get_absolute_center());
	forkItemRight->set_pos(other->forkItemRight->get_absolute_center());
	forkItemRightLeft->set_pos(other->forkItemRightLeft->get_absolute_center());
	burstingItem->set_pos(other->burstingItem->get_absolute_center());
	pipingItem->set_pos(other->pipingItem->get_absolute_center());
	forkItemRightRight->set_pos(other->forkItemRightRight->get_absolute_center());
	slopeLandsideItem->set_pos(other->slopeLandsideItem->get_absolute_center());
	slopeWatersideItem->set_pos(other->slopeWatersideItem->get_absolute_center());
}
//Set default values
void FplGui_Dike_FaultTree_Wid::set_default_values(void) 
{
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//
	reset();
}
//____________
//public slots
//Open the dialog to edit values in the widget
void FplGui_Dike_FaultTree_Wid::show_as_dialog(void) 
{
	Sys_Base_Edit_Dia dialog(NULL, this); 
	FplGui_Dike_FaultTree_Wid inDiaWid(DataRole::EditRole, &dialog); 
	inDiaWid.set_member(this); 
	dialog.add_child(&inDiaWid); 
	dialog.setWindowTitle(this->head_label->text()); 
	QIcon icon; 
	icon.addPixmap(*this->head_pixmap->pixmap()); 
	dialog.setWindowIcon(icon); 

	//accepted
	if(dialog.start_dialog()==true)
	{
		//transfer members to database
		this->transfer_members2database(&inDiaWid);	
	}
	//rejected
	else{
		return;	
	}
}
//
void FplGui_Dike_FaultTree_Wid::set_item_position_to_defaults(void) 
{
	failureItem->set_pos(0, -130);
	forkItemMid->set_pos(0, -30);
	forkItemLeft->set_pos(-150, 0);
	overflowItem->set_pos(-225, 100);
	waveOvertoppingItem->set_pos(-75, 100);
	forkItemRight->set_pos(150, 0);
	forkItemRightLeft->set_pos(75, 100);
	burstingItem->set_pos(10, 200);
	pipingItem->set_pos(140, 200);
	forkItemRightRight->set_pos(250,0);
	slopeLandsideItem->set_pos(225, 100);
	slopeWatersideItem->set_pos(350, 100);
}
//___________
//private
//Transfer members to database
void FplGui_Dike_FaultTree_Wid::transfer_members2database(FplGui_Dike_FaultTree_Wid *dialog) 
{
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
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_overflow_active())<< "  ";
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

	//waves and wind
	//mech wave run up
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_waveOvertopping_active())<< "  ";
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

	//mech wind wave
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_waveOvertopping_active())<< "  ";
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

	//bursting
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_bursting_active())<< "  ";
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

	//piping  sellmeijer
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->is_piping_sellmeijer_active())<< "  ";
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

	//piping  lane
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->is_piping_lane_active())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_piping_lane<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	//piping rename
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied_tree) << " = "<< functions::convert_boolean2string(dialog->is_piping_rename_active())<< "  ";
	query_string  << " WHERE ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_sec_type) << " = '" << _Fpl_Section_Type::convert_section_type2txt(_fpl_section_types::dike_sec)<<"'";
	query_string  << " AND ";
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_name) << " = '"  << fpl_label::mech_piping_schmert<<"'";
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << _Fpl_Section_Type::table_mechanism->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	//check all piping mechanism
	if(dialog->is_piping_lane_active()==false && dialog->is_piping_sellmeijer_active()==false && dialog->is_piping_rename_active()==false){
		query_string.str("");
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_piping_sellmeijer_active())<< "  ";
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
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_piping_lane_active())<< "  ";
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
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_piping_rename_active())<< "  ";
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
	}
	else{

		query_string.str("");
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(true)<< "  ";
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
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(true)<< "  ";
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
		query_string <<"UPDATE ";
		query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
		query_string << " SET " ;
		query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(true)<< "  ";
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




	}

	//slope landside
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_slope_landside_active())<< "  ";
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

	//slope waterside
	query_string.str("");
	query_string <<"UPDATE ";
	query_string << _Fpl_Section_Type::table_mechanism->get_table_name();
	query_string << " SET " ;
	query_string  << _Fpl_Section_Type::table_mechanism->get_column_name(fpl_label::mecha_applied) << " = "<< functions::convert_boolean2string(dialog->is_slope_waterside_active())<< "  ";
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

	this->set_member(dialog);
}
//
void FplGui_Dike_FaultTree_Wid::setup_items(void)
{
	failureItem = new Sys_Elastic_GraphicsJunction(this->scene, this);
	failureItem->set_custom_pixmap(QPixmap(":/failure_icon"));
	failureItem->set_type(Sys_Elastic_GraphicsJunction::Custom);
	failureItem->set_size(102.0, 58.0);

	forkItemMid = new Sys_Elastic_GraphicsJunction(scene, this);
	forkItemMid->set_type(Sys_Elastic_GraphicsJunction::Or);
	
	forkItemLeft = new Sys_Elastic_GraphicsJunction(scene, this);
	forkItemLeft->set_type(Sys_Elastic_GraphicsJunction::Or);

	overflowItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	overflowItem->add_pixmap(tr("Overflow"), QPixmap(":/dike_overflow"));
	overflowItem->set_size(80.0, 80.0);

	waveOvertoppingItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	waveOvertoppingItem->add_pixmap(tr("Wave Overtopping"), QPixmap(":/dike_waveovertopping"));
	waveOvertoppingItem->set_size(80.0, 80.0);

	forkItemRight = new Sys_Elastic_GraphicsJunction(scene, this);
	forkItemRight->set_type(Sys_Elastic_GraphicsJunction::Or);

	forkItemRightLeft = new Sys_Elastic_GraphicsJunction(scene, this);
	forkItemRightLeft->set_type(Sys_Elastic_GraphicsJunction::And);

	burstingItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	burstingItem->add_pixmap(tr("Bursting"), QPixmap(":/dike_bursting"));
	burstingItem->set_size(80.0, 80.0);

	pipingItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	pipingItem->add_pixmap(tr("Piping Sellmeijer"), QPixmap(":/dike_piping_sellmeijer"));
	pipingItem->add_pixmap(tr("Piping Lane"), QPixmap(":/dike_piping_lane"));
	pipingItem->add_pixmap(tr("Piping Schmertmann"), QPixmap(":/dike_piping_rename"));
	pipingItem->set_null_pixmap(QPixmap(":/dike_piping"));
	pipingItem->set_size(80.0, 80.0);

	forkItemRightRight = new Sys_Elastic_GraphicsJunction(scene, this);
	forkItemRightRight->set_type(Sys_Elastic_GraphicsJunction::Or);

	slopeLandsideItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	slopeLandsideItem->add_pixmap(tr("Slope Landside"), QPixmap(":/dike_slope_landside"));
	slopeLandsideItem->set_size(80.0, 80.0);

	slopeWatersideItem = new Sys_Elastic_GraphicsPixmapItem(scene, this);
	slopeWatersideItem->add_pixmap(tr("Slope Waterside"), QPixmap(":/dike_slope_waterside"));
	slopeWatersideItem->set_size(80.0, 80.0);

	failureFork1Edge = new Sys_Elastic_GraphicsEdge(failureItem, forkItemMid);
	forkMidForkLeftEdge = new Sys_Elastic_GraphicsEdge(forkItemMid, forkItemLeft);
	forkLeftOverflowEdge = new Sys_Elastic_GraphicsEdge(forkItemLeft, overflowItem);
	forkLeftWaveOvertoppingEdge = new Sys_Elastic_GraphicsEdge(forkItemLeft, waveOvertoppingItem);
	forkMidforkRightEdge = new Sys_Elastic_GraphicsEdge(forkItemMid, forkItemRight);
	forkRightForkRightLeftEdge = new Sys_Elastic_GraphicsEdge(forkItemRight, forkItemRightLeft);
	forkRightLeftBurstingEdge = new Sys_Elastic_GraphicsEdge(forkItemRightLeft, burstingItem);
	forkRightLeftPipingEdge = new Sys_Elastic_GraphicsEdge(forkItemRightLeft, pipingItem);
	forkRightSlopeLandsideEdge = new Sys_Elastic_GraphicsEdge(forkItemRightRight, slopeLandsideItem);
	forkRightSlopeSeesideEdge = new Sys_Elastic_GraphicsEdge(forkItemRightRight, slopeWatersideItem);
	forkRightforkRightRightEdge = new Sys_Elastic_GraphicsEdge(forkItemRight, forkItemRightRight);

	failureItem->add_bottomLevel_object(forkItemMid);
	forkItemMid->add_bottomLevel_object(forkItemLeft);
	forkItemMid->add_bottomLevel_object(forkItemRight);
	forkItemLeft->add_bottomLevel_object(overflowItem);
	forkItemLeft->add_bottomLevel_object(waveOvertoppingItem);
	forkItemRight->add_bottomLevel_object(forkItemRightLeft);
	forkItemRight->add_bottomLevel_object(forkItemRightRight);
	forkItemRightRight->add_bottomLevel_object(slopeLandsideItem);
	forkItemRightRight->add_bottomLevel_object(slopeWatersideItem);
	forkItemRightLeft->add_bottomLevel_object(burstingItem);
	forkItemRightLeft->add_bottomLevel_object(pipingItem);

	items.append(burstingItem);
	items.append(waveOvertoppingItem);
	items.append(overflowItem);
	items.append(pipingItem);
	items.append(slopeLandsideItem);
	items.append(slopeWatersideItem);
}
//
void FplGui_Dike_FaultTree_Wid::set_items_checkable(const bool checkable) 
{
	overflowItem->set_enabled(checkable);
	waveOvertoppingItem->set_enabled(checkable);
	burstingItem->set_enabled(checkable);
	pipingItem->set_enabled(checkable);
	slopeLandsideItem->set_enabled(checkable);
	slopeWatersideItem->set_enabled(checkable);
}
//set the error
Error FplGui_Dike_FaultTree_Wid::set_error(const int err_type) 
{
	string place="FplGui_Dike_FaultTree_Wid::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type)
	{
		case 0://problems with database query
			place.append("transfer_members2database(FplGui_Dike_FaultTree_Wid *dialog)") ;
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