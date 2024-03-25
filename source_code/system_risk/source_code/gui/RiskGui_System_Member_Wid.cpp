#include "Risk_Headers_Precompiled.h"
//#include "RiskGui_System_Member_Wid.h"

//Default constructor
RiskGui_System_Member_Wid::RiskGui_System_Member_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	//Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->area2print=ui.scrollAreaWidgetContents;

	this->set_headLabel_text("Control Parameters for the catchment based risk calculation");

	//main outflow river
	this->ui.main_outflow->set_label_text("Number main outflow river");
	this->ui.main_outflow->set_tooltip("The id of the river, which outflow discharge is relevant for the downstream riparian");
	this->ui.main_outflow->set_range(-1, 100);
	this->ui.main_outflow->set_increment(1);
	//max hydraulic calc
	this->ui.max_hyd->set_label_text("Maximum hydraulic calculation");
	this->ui.max_hyd->set_tooltip("Maximum of hydraulic calculation for the catchment risk calculation");
	this->ui.max_hyd->set_range(10, 10000);
	this->ui.max_hyd->set_increment(10);
	//use predicted values
	this->ui.use_prediction->set_label_text("Use predicted values for an optimisation");
	this->ui.use_prediction->set_tooltip("Flag if the predicted values are integrated in the catchment area risk approach");
	
	
	//max mc runs
	this->ui.mc_max->set_label_text("Maximum number of MC-runs");
	this->ui.mc_max->set_tooltip("Maximal number of a monte-carlo simulations");
	this->ui.mc_max->set_range(100, 10000000);
	this->ui.mc_max->set_increment(10000);
	
	//number of intervals for the frc-curve
	this->ui.num_intervals->set_label_text("Maximum number of intervals for dividing the Frc-curve");
	this->ui.num_intervals->set_tooltip("Number of intervals a frc curve is divided for the generation of the catchment area risk [min. 2]");
	this->ui.num_intervals->set_range(2, 100);
	this->ui.num_intervals->set_increment(1);

	//probability boundary
	this->ui.bound_prob->set_label_text("Probability boundary [-]");
	this->ui.bound_prob->set_tooltip("Boundary of sum up probability of catchment area risk scenarios, which should be calculated (maximum is 1.0)");
	this->ui.bound_prob->set_range(0.0, 1.0);

	//mc_random_generator
	this->ui.mc_random_generator->set_label_text("Random generator");
	this->ui.mc_random_generator->set_tooltip("The type of random generator, which is used (RANMAR, MOTHER_OF_ALL, SFMT)");
	string types[] = {"RANMAR", "MOTHER_OF_ALL", "SFMT"};
	this->ui.mc_random_generator->set_items(types, 3);


	
	switch(role) {
		case 0: //DisplayRole
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/config"));
			//this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EditRole
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
	//count the memory
	ADD_MEM(RiskGui_System_Member_Wid, _sys_system_modules::RISK_SYS);
}
//Default destructor
RiskGui_System_Member_Wid::~RiskGui_System_Member_Wid(void) {
	MIN_MEM(RiskGui_System_Member_Wid, _sys_system_modules::RISK_SYS);
}
//________
//public
//Decides wether the widget is just to view or to edit the shown data
void RiskGui_System_Member_Wid::set_editable(const bool state) {
	this->editable = state;
	//general
	this->ui.main_outflow->set_editable(state);
	this->ui.max_hyd->set_editable(state);
	this->ui.use_prediction->set_editable(state);
	//risk state prediction
	this->ui.mc_max->set_editable(state);
	this->ui.num_intervals->set_editable(state);
	this->ui.bound_prob->set_editable(state);
	this->ui.mc_random_generator->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void RiskGui_System_Member_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	RiskGui_System_Member_Wid *other = (RiskGui_System_Member_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general
	this->ui.main_outflow->set_value(other->ui.main_outflow->get_value());
	this->ui.max_hyd->set_value(other->ui.max_hyd->get_value());
	this->ui.use_prediction->set_value(other->ui.use_prediction->get_value());
	//risk state prediction
	this->ui.mc_max->set_value(other->ui.mc_max->get_value());
	this->ui.num_intervals->set_value(other->ui.num_intervals->get_value());
	this->ui.bound_prob->set_value(other->ui.bound_prob->get_value());
	this->ui.mc_random_generator->set_editable(true);
	this->ui.mc_random_generator->set_current_value(other->ui.mc_random_generator->get_current_index());
	this->ui.mc_random_generator->set_editable(false);
}
//Set default values
void RiskGui_System_Member_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");

	//general
	this->ui.main_outflow->set_value(-1);
	this->ui.max_hyd->set_value(risk_control_param::max_hydraulic_def);
	this->ui.use_prediction->set_value(true);
	//risk state prediction
	this->ui.mc_max->set_value(risk_control_param::no_mc_def);
	this->ui.num_intervals->set_value(2);
	this->ui.bound_prob->set_value(risk_control_param::boundary_prob_def);
	this->ui.mc_random_generator->set_current_value(0);
}
//Set the member of the widget
void RiskGui_System_Member_Wid::set_member(QSqlDatabase *ptr_database){
	this->ptr_database=ptr_database;
	Risk_System buffer;
	try{
		buffer.set_input(ptr_database,false);
	


		//general
		this->ui.main_outflow->set_value(buffer.get_number_main_outflow_river());
		this->ui.max_hyd->set_value(buffer.get_ca_control_parameter().max_hyd_calc);
		this->ui.use_prediction->set_value(buffer.get_ca_control_parameter().use_predict_values);
		//risk state prediction
		this->ui.mc_max->set_value(buffer.get_ca_control_parameter().number_mc);
		this->ui.num_intervals->set_value(buffer.get_ca_control_parameter().number_intervals);
		this->ui.bound_prob->set_value(buffer.get_ca_control_parameter().prob_bound);
		this->ui.mc_random_generator->set_current_value(buffer.get_ca_control_parameter().gen_type);

	}
	catch(Error msg){
		msg.output_msg(0);
	}

}
//____________
//public slots
//Open the dialog to edit values in the widget
void RiskGui_System_Member_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	RiskGui_System_Member_Wid inDiaWid(DataRole::EditRole, &dialog);
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
//________
//private
//Transfer members to database
void RiskGui_System_Member_Wid::transfer_members2database(RiskGui_System_Member_Wid *dialog){
	//the table is set (the name and the column names) and allocated
	try{
		Risk_System::set_table_config(this->ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;
	ostringstream query_string;

	//general
	//main outflow river
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< dialog->ui.main_outflow->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::main_outflow_rv <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//max hydraulic calc
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< dialog->ui.max_hyd->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::max_hydraulic <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");


	//use predicted values
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< functions::convert_boolean2string(dialog->ui.use_prediction->get_value())<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::use_prediction <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");


	//risk stae generation via mc-analysis
	//max mc
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< dialog->ui.mc_max->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::number_mc_sim <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//number frc intervals
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< dialog->ui.num_intervals->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::number_intervals <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//probability boundary
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(risk_label::value_param) << " = "<< dialog->ui.bound_prob->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(risk_label::name_param) << " = '"  << risk_control_param::boundary_prob <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//random generator
	query_string <<"UPDATE ";
	query_string << Risk_System::table_config->get_table_name();
	query_string << " SET " ;
	query_string  << Risk_System::table_config->get_column_name(fpl_label::control_value) << " = '"<< dialog->ui.mc_random_generator->get_current_value()<< "'  ";
	query_string  << " WHERE ";
	query_string  << Risk_System::table_config->get_column_name(fpl_label::control_name) << " = '"  << risk_control_param::rand_generator <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Risk_System::table_config->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");


	this->set_member(dialog);
}
//set the error
Error RiskGui_System_Member_Wid::set_error(const int err_type){
		string place="RiskGui_System_Member_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(RiskGui_System_Member_Wid *dialog)") ;
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