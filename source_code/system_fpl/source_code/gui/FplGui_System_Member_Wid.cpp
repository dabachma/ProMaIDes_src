//#include "FplGui_System_Member_Wid.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
FplGui_System_Member_Wid::FplGui_System_Member_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	//Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->area2print=ui.scrollAreaWidgetContents;

	this->set_headLabel_text("Probabilistic Control Parameters");

	//mc_max
	this->ui.mc_max->set_label_text("Maximum number of MC-runs");
	this->ui.mc_max->set_tooltip("Maximal number of a monte-carlo simulations");
	this->ui.mc_max->set_range(100, 100000000);
	this->ui.mc_max->set_increment(10000);	
	//mc_min
	this->ui.mc_min->set_label_text("Minimum number of MC-runs");
	this->ui.mc_min->set_tooltip("Minimal number of a monte-carlo simulations before the abort criterion is checked");
	this->ui.mc_min->set_range(0, 100000000);
	this->ui.mc_min->set_increment(10000);

	//mc_error_abs
	this->ui.mc_error_abs->set_label_text("Absolute accuracy-criteria for the abort criterion");
	this->ui.mc_error_abs->set_tooltip("Absolute accuracy-criteria for the abort criterion of a monte-carlo simulations");
	this->ui.mc_error_abs->set_range(0.0001, 0.3);
	//mc_error_rel
	this->ui.mc_error_rel->set_label_text("Relative accuracy-criteria for the abort criterion");
	this->ui.mc_error_rel->set_tooltip("Relative accuracy-criteria for the abort criterion of a monte-carlo simulations ");
	this->ui.mc_error_rel->set_range(0.001, 0.4);
	//mc_confindent_int
	this->ui.mc_confident_int->set_label_text("Confidence interval for the abort criterion");
	this->ui.mc_confident_int->set_tooltip("The confidence interval for the abort criterion of a monte-carlo simulations ");
	this->ui.mc_confident_int->set_range(0.5, 0.999);
	//mc_random_generator
	this->ui.mc_random_generator->set_label_text("Random generator");
	this->ui.mc_random_generator->set_tooltip("The type of random generator, which is used (RANMAR, MOTHER_OF_ALL, SFMT)");
	string types[] = {"RANMAR", "MOTHER_OF_ALL", "SFMT"};
	this->ui.mc_random_generator->set_items(types, 3);

	//frc_max
	this->ui.frc_max->set_label_text("Maximum number of fragility curve steps");
	this->ui.frc_max->set_tooltip("Maximal number of fragility curve steps or maximal number of waterlevels, which are calculated");
	this->ui.frc_max->set_range(10, 100000000);
	this->ui.frc_max->set_increment(10000);
	//frc_step_max
	this->ui.frc_step_max->set_label_text("Maximal stepsize [m]");
	this->ui.frc_step_max->set_tooltip("Maximal stepsize [m] of fragility curve calculation");
	this->ui.frc_step_max->set_range(0.005, 0.5);
	//frc_step_min
	this->ui.frc_step_min->set_label_text("Minimal stepsize [m]");
	this->ui.frc_step_min->set_tooltip("Minimal stepsize [m] of fragility curve calculation");
	this->ui.frc_step_min->set_range(0.005, 0.5);
	//frc_step_start
	this->ui.frc_step_start->set_label_text("Starting stepsize [m]");
	this->ui.frc_step_start->set_tooltip("Starting stepsize [m] of fragility curve calculation");
	//frc_start_height
	this->ui.frc_start_height->set_label_text("Starting waterlevel [m]");
	this->ui.frc_start_height->set_tooltip("Starting waterlevel [m] of fragility curve calculation");
	//frc_step_reduce
	this->ui.frc_step_reduce->set_label_text("Boundary value for stepsize reduction");
	this->ui.frc_step_reduce->set_tooltip("Boundary value for a stepsize reduction for fragility curve simulations");
	this->ui.frc_step_reduce->set_range(1.0, 5.0);
	//frc_step_expand
	this->ui.frc_step_expand->set_label_text("Boundary value for stepsize expansion");
	this->ui.frc_step_expand->set_tooltip("Boundary value for a stepsize expansion for fragility curve simulations");
	this->ui.frc_step_expand->set_range(1.0, 5.0);
	//frc_step_factor
	this->ui.frc_step_factor->set_label_text("Stepsize factor");
	this->ui.frc_step_factor->set_tooltip("Step-size factor for fragility curve simulations");
	this->ui.frc_step_expand->set_range(1.0, 5.0);

	//seepage calculation
	this->ui.seepage_max_waterlevel->set_label_text("Method for the maximum waterlevel");
	this->ui.seepage_max_waterlevel->set_tooltip("The type of the calculation method for the maximum waterlevel");
	string types_2[] = {fpl_label::seepage_max_base_land, fpl_label::seepage_max_one_third, fpl_label::seepage_max_kozeny};
	this->ui.seepage_max_waterlevel->set_items(types_2, 3);

	this->ui.seepage_min_waterlevel->set_label_text("Method for the minimum waterlevel");
	this->ui.seepage_min_waterlevel->set_tooltip("The type of the calculation method for the minimum waterlevel");
	string types_3[] = {fpl_label::seepage_min_base_land, fpl_label::seepage_min_one_third};
	this->ui.seepage_min_waterlevel->set_items(types_3, 2);

	//output control
	this->ui.output_tecplot->set_label_text("Tecplot");
	this->ui.output_tecplot->set_tooltip("Activate it for generation of tecplot output files (deterministic)");
	this->ui.output_tecplot->set_value(false);

	this->ui.output_paraview->set_label_text("ParaView");
	this->ui.output_paraview->set_tooltip("Activate it for generation of ParaView output files (deterministic)");
	this->ui.output_paraview->set_value(true);

	this->ui.output_excel->set_label_text("Excel");
	this->ui.output_excel->set_tooltip("Activate it for generation of Excel output files (deterministic)");
	this->ui.output_excel->set_value(true);
	


	DATA_ROLE_CONSTRUCTOR_STUFF();
	//count the memory
	ADD_MEM(FplGui_System_Member_Wid, _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_System_Member_Wid::~FplGui_System_Member_Wid(void) {
	MIN_MEM(FplGui_System_Member_Wid, _sys_system_modules::FPL_SYS);
}
//________
//public
//Decides wether the widget is just to view or to edit the shown data
void FplGui_System_Member_Wid::set_editable(const bool state) {
	this->editable = state;
	//monte carlo
	this->ui.mc_max->set_editable(state);
	this->ui.mc_min->set_editable(state);
	this->ui.mc_error_abs->set_editable(state);
	this->ui.mc_error_rel->set_editable(state);
	this->ui.mc_confident_int->set_editable(state);
	this->ui.mc_random_generator->set_editable(state);
	//fragility curve
	this->ui.frc_max->set_editable(state);
	this->ui.frc_step_max->set_editable(state);
	this->ui.frc_step_min->set_editable(state);
	this->ui.frc_step_start->set_editable(state);
	this->ui.frc_start_height->set_editable(state);
	this->ui.frc_step_reduce->set_editable(state);
	this->ui.frc_step_expand->set_editable(state);
	this->ui.frc_step_factor->set_editable(state);

	//seepage
	this->ui.seepage_max_waterlevel->set_editable(state);
	this->ui.seepage_min_waterlevel->set_editable(state);

	//output
	this->ui.output_tecplot->set_editable(state);
	this->ui.output_paraview->set_editable(state);
	this->ui.output_excel->set_editable(state);

}
//Set all members of the widget, similar to a copy constructor
void FplGui_System_Member_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	FplGui_System_Member_Wid *other = (FplGui_System_Member_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//monte carlo
	this->ui.mc_max->set_value(other->ui.mc_max->get_value());
	this->ui.mc_min->set_value(other->ui.mc_min->get_value());
	this->ui.mc_error_abs->set_value(other->ui.mc_error_abs->get_value());
	this->ui.mc_error_rel->set_value(other->ui.mc_error_rel->get_value());
	this->ui.mc_confident_int->set_value(other->ui.mc_confident_int->get_value());
	this->ui.mc_random_generator->set_editable(true);
	this->ui.mc_random_generator->set_current_value(other->ui.mc_random_generator->get_current_index());
	this->ui.mc_random_generator->set_editable(false);
	//fragility curve
	this->ui.frc_max->set_value(other->ui.frc_max->get_value());
	this->ui.frc_step_max->set_value(other->ui.frc_step_max->get_value());
	this->ui.frc_step_min->set_value(other->ui.frc_step_min->get_value());
	this->ui.frc_step_start->set_value(other->ui.frc_step_start->get_value());
	this->ui.frc_start_height->set_value(other->ui.frc_start_height->get_value());
	this->ui.frc_step_reduce->set_value(other->ui.frc_step_reduce->get_value());
	this->ui.frc_step_expand->set_value(other->ui.frc_step_expand->get_value());
	this->ui.frc_step_factor->set_value(other->ui.frc_step_factor->get_value());
	//seepage
	this->ui.seepage_max_waterlevel->set_editable(true);
	this->ui.seepage_max_waterlevel->set_current_value(other->ui.seepage_max_waterlevel->get_current_index());
	this->ui.seepage_max_waterlevel->set_editable(false);
	this->ui.seepage_min_waterlevel->set_editable(true);
	this->ui.seepage_min_waterlevel->set_current_value(other->ui.seepage_min_waterlevel->get_current_index());
	this->ui.seepage_min_waterlevel->set_editable(false);

	//output
	this->ui.output_tecplot->set_editable(true);
	this->ui.output_tecplot->set_value(other->ui.output_tecplot->get_value());
	this->ui.output_tecplot->set_editable(false);

	this->ui.output_paraview->set_editable(true);
	this->ui.output_paraview->set_value(other->ui.output_paraview->get_value());
	this->ui.output_paraview->set_editable(false);

	this->ui.output_excel->set_editable(true);
	this->ui.output_excel->set_value(other->ui.output_excel->get_value());
	this->ui.output_excel->set_editable(false);
			
}
//Set default values
void FplGui_System_Member_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//monte carlo
	this->ui.mc_max->set_value(0);
	this->ui.mc_min->set_value(0);
	this->ui.mc_error_abs->set_value(0);
	this->ui.mc_error_rel->set_value(0);
	this->ui.mc_confident_int->set_value(0);
	this->ui.mc_random_generator->set_current_value(0);
	//fragility curve
	this->ui.frc_max->set_value(0);
	this->ui.frc_step_max->set_value(0);
	this->ui.frc_step_min->set_value(0);
	this->ui.frc_step_start->set_value(0);
	this->ui.frc_start_height->set_value(0);
	this->ui.frc_step_reduce->set_value(0);
	this->ui.frc_step_expand->set_value(0);
	this->ui.frc_step_factor->set_value(0);
	//seepage
	this->ui.seepage_max_waterlevel->set_current_value(0);
	this->ui.seepage_min_waterlevel->set_current_value(0);
	//output
	this->ui.output_tecplot->set_value(false);
	this->ui.output_paraview->set_value(true);
	this->ui.output_excel->set_value(true);
}
//Set the member of the widget
void FplGui_System_Member_Wid::set_member(QSqlDatabase *ptr_database){
	this->ptr_database=ptr_database;
	Fpl_Frc_Sim buffer;
	Fpl_Seepage_Calculator_Dike buffer_2;

	try{
		buffer.init_frc_calculation();
		buffer.set_input(ptr_database, false);
		//monte carlo
		this->ui.mc_max->set_value(buffer.mc_sim_frc->mc_max);
		this->ui.mc_min->set_value(buffer.mc_sim_frc->mc_min);
		this->ui.mc_error_abs->set_value(buffer.mc_sim_frc->epsilon_absolut);
		this->ui.mc_error_rel->set_value(buffer.mc_sim_frc->epsilon_relativ);
		this->ui.mc_confident_int->set_value(buffer.mc_sim_frc->confidence);
		this->ui.mc_random_generator->set_editable(true);
		this->ui.mc_random_generator->set_current_value(buffer.mc_sim_frc->generator);
		this->ui.mc_random_generator->set_editable(false);
		//fragility curve
		this->ui.frc_max->set_value(buffer.max_calculation);
		this->ui.frc_step_max->set_value(buffer.stepsize_max);
		this->ui.frc_step_min->set_value(buffer.stepsize_min);
		this->ui.frc_step_start->set_value(buffer.starting_stepsize);
		this->ui.frc_start_height->set_value(buffer.start_value);
		this->ui.frc_step_reduce->set_value(buffer.quotient_boundary_reduce);
		this->ui.frc_step_expand->set_value(buffer.quotient_boundary_expand);
		this->ui.frc_step_factor->set_value(buffer.stepsize_factor);

		//seepage
		buffer_2.set_input(ptr_database, false, false);
		this->ui.seepage_max_waterlevel->set_editable(true);
		this->ui.seepage_max_waterlevel->set_current_value(buffer_2.seepage_calc_max);
		this->ui.seepage_max_waterlevel->set_editable(false);
		this->ui.seepage_min_waterlevel->set_editable(true);
		this->ui.seepage_min_waterlevel->set_current_value(buffer_2.seepage_calc_min);
		this->ui.seepage_min_waterlevel->set_editable(false);

		//output
		output_control buffer3;
		Fpl_Mc_Sim::get_output_control_from_table(ptr_database, &buffer3);

		this->ui.output_tecplot->set_value(buffer3.tec_output);
		this->ui.output_paraview->set_value(buffer3.para_output);
		this->ui.output_excel->set_value(buffer3.excel_output);

	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void FplGui_System_Member_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	FplGui_System_Member_Wid inDiaWid(DataRole::EditRole, &dialog);
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
void FplGui_System_Member_Wid::transfer_members2database(FplGui_System_Member_Wid *dialog){
	//the table is set (the name and the column names) and allocated
	try{
		Fpl_Mc_Sim::set_table(this->ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;
	ostringstream query_string;

	//mc-values
	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.mc_max->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_max <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.mc_min->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_min <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.mc_error_abs->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_error_abs <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.mc_error_rel->get_value()*100.0<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_error_rel <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.mc_confident_int->get_value()*100.0<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_confident <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '"<< dialog->ui.mc_random_generator->get_current_value()<< "'  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::mc_rand_generator <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//fragility curve
	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_max->get_value()<< " ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_max <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_max->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_max <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_min->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_min <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_start->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_start <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_start_height->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_height_start <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_reduce->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_reduce <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_expand->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_expand <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = "<< dialog->ui.frc_step_factor->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_control_param::frc_step_factor <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	//seepage
	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '"<< dialog->ui.seepage_max_waterlevel->get_current_value()<< "'  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_seepage_method::seepage_maximum <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");

	query_string <<"UPDATE ";
	query_string << Fpl_Mc_Sim::table->get_table_name();
	query_string << " SET " ;
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_value) << " = '"<< dialog->ui.seepage_min_waterlevel->get_current_value()<< "'  ";
	query_string  << " WHERE ";
	query_string  << Fpl_Mc_Sim::table->get_column_name(fpl_label::control_name) << " = '"  << fpl_seepage_method::seepage_minimum <<"'";
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);
	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Fpl_Mc_Sim::table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}
	query_string.str("");
	
	//output
	output_control buffer3;
	buffer3.excel_output = dialog->ui.output_excel->get_value();
	buffer3.para_output = dialog->ui.output_paraview->get_value();
	buffer3.tec_output = dialog->ui.output_tecplot->get_value();
	Fpl_Mc_Sim::set_output_control2table(ptr_database, buffer3);




	this->set_member(dialog);
}
//set the error
Error FplGui_System_Member_Wid::set_error(const int err_type){
		string place="FplGui_System_Member_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(FplGui_System_Member_Wid *dialog)") ;
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