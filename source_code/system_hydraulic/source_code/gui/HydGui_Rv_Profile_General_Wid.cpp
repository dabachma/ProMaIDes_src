//#include "HydGui_Rv_Profile_General_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default Constructor
HydGui_Rv_Profile_General_Wid::HydGui_Rv_Profile_General_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->break_params = new HydGui_Rv_Profile_Break_Params_Wid(role, this);
	this->overflow_params = new HydGui_Rv_Profile_Overflow_Params_Wid(role, this);
	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;

	ui.river_geo->setEditTriggers(QAbstractItemView::NoEditTriggers);


	//context menu
	this->close_expand_icon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowDown), QIcon::Normal, QIcon::Off);
	this->close_expand_action=new QAction(this->close_expand_icon, "Close all expanders", &this->context_menu);
	this->open_expand_icon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowUp), QIcon::Normal, QIcon::On);
	this->open_expand_action=new QAction(this->open_expand_icon, "Open all expanders", &this->context_menu);
	this->context_menu.addAction(this->close_expand_action);
	this->context_menu.addAction(this->open_expand_action);
	this->context_menu.insertSeparator(this->close_expand_action);
	QObject::connect(this->open_expand_action, SIGNAL(triggered()), this, SLOT(open_expander_widgets()));
	QObject::connect(this->close_expand_action, SIGNAL(triggered()), this, SLOT(close_expander_widgets()));

	this->set_headLabel_text("River Profile");

	// PROFILE NAME
	ui.profile_name->set_label_text("Profile Name");

	// PROFILE TYPE
	ui.profile_type->set_label_text("Profile Type");
	string pro_types[] = {"RIVER", "WEIR", "BRIDGE"};
	ui.profile_type->set_items(pro_types, 3);
	ui.profile_type->set_tooltip("Type of the profile: first profile has to be a river type");

	// RIVERSTATION
	ui.river_station->set_label_text("Riverstation [m]");
	ui.river_station->set_range(-9999999.0, 999999999.0);
	ui.river_station->set_dataRole(role);

	// DELTA X TABLE
	ui.delta_x_table->set_label_text("Delta x-table [m]");
	ui.delta_x_table->set_tooltip("Value for the dicretisation of the profile table, where the flow-area to the discret local waterlevel and the conveyance to the discret local waterlevel are stored [optional; standard value = 0.1 m]");
	ui.delta_x_table->set_dataRole(role);

	// CONNECTION TYPE
	ui.connection_type->set_label_text("Connection Type");
	string conn_types[] = {"STANDARD", "INFLOW", "OUTFLOW"};
	ui.connection_type->set_items(conn_types, 3);
	ui.connection_type->set_tooltip("Connection type of the profile: first profile has to be 'INFLOW', last profile has to be 'OUTFLOW'");

	// INITIAL CONDITION
	ui.init_condition->set_label_text("Initial Condition [m]");
	ui.init_condition->set_tooltip("Initial condition of the profile; local to the minimal geodetic height (z_min) of the profile points [m]");
	ui.init_condition->set_dataRole(role);

	// OVERFLOW COUPLING LEFT
	this->overflow_params->overflow_coupling_left->set_label_text("Overflow Coupling Left");
	this->overflow_params->overflow_coupling_left->set_tooltip("Flag, if an overflow over the left river bank is applied [optional; default value = false]");
	//// POLENI FAC LEFT
	this->overflow_params->poleni_fac_left->set_label_text("Poleni Factor Left [-]");
	this->overflow_params->poleni_fac_left->set_tooltip("Poleni factor for an overflow over the left river bank [optional; default value = 0.65]");
	this->overflow_params->poleni_fac_left->set_dataRole(role);

	// OVERFLOW COUPLING RIGHT
	this->overflow_params->overflow_coupling_right->set_label_text("Overflow Coupling Right");
	this->overflow_params->overflow_coupling_right->set_tooltip("Flag, if an overflow over the right river bank is applied [optional; default value = false]");
	//// POLENI FAC RIGHT
	this->overflow_params->poleni_fac_right->set_label_text("Poleni Factor Right [-]");
	this->overflow_params->poleni_fac_right->set_tooltip("Poleni factor for an overflow over the right river bank [optional; default value = 0.65]");
	this->overflow_params->poleni_fac_right->set_dataRole(role);

	// RIVER GEOMETRICS
	
	// SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/HydGui_Rv_Profile_General"));
			ui.add_row_button->hide();
			ui.remove_row_button->hide();
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::connect(ui.add_row_button, SIGNAL(clicked()), ui.river_geo, SLOT(add_row()));
			QObject::connect(ui.remove_row_button, SIGNAL(clicked()), ui.river_geo, SLOT(remove_row()));
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
	this->role = role;

	this->boundary_lateral_wids = NULL;
	this->boundary_point_wids = NULL;
	this->expander_wids = NULL;

	ui.break_expander->set_title("Break Parameters");
	ui.break_expander->add_child(this->break_params);
	ui.overflow_expander->set_title("Overflow Parameters");
	ui.overflow_expander->add_child(this->overflow_params);

	ui.boundary_parameters_groupbox->setVisible(false);
	this->number_of_boundary_params = 0;

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Rv_Profile_General_Wid), _sys_system_modules::HYD_SYS);
}

//Default Destructor
HydGui_Rv_Profile_General_Wid::~HydGui_Rv_Profile_General_Wid(void) {
    //this->delete_boundary_params();

	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Rv_Profile_General_Wid), _sys_system_modules::HYD_SYS);
}
//______________
//public methods
//method to set the whole widget editable
void HydGui_Rv_Profile_General_Wid::set_editable(const bool state) {
	ui.connection_type->set_editable(state);
	ui.delta_x_table->set_editable(state);
	ui.init_condition->set_editable(state);
	ui.profile_name->set_editable(state);
	ui.profile_type->set_editable(state);
	ui.river_station->set_editable(state);
	this->overflow_params->set_editable(state);
	this->break_params->set_editable(state);
}

//Set the number of boundary parameters and expander widgets
void HydGui_Rv_Profile_General_Wid::set_number_of_boundary_param_wids(const int number) {
	if (number > 0) {
		this->number_of_boundary_params = number;
		this->allocate_boundary_params();

		for (int i = 0; i < number; i++) {
			this->expander_wids[i] = new Sys_Base_Expand_Wid(ui.boundary_parameters_groupbox);
			this->boundary_lateral_wids[i] = new HydGui_Rv_Profile_Boundary_Lateral_Params_Wid(this->role, this->expander_wids[i]);
			this->boundary_point_wids[i] = new HydGui_Rv_Profile_Boundary_Point_Params_Wid(this->role, this->expander_wids[i]);

			this->expander_wids[i]->add_child(this->boundary_lateral_wids[i]);
			this->expander_wids[i]->add_child(this->boundary_point_wids[i]);
			ui.boundary_parameters_layout->addWidget(this->expander_wids[i]);
		}

		ui.boundary_parameters_groupbox->setVisible(true);
	}
}

//Get the number of boundary parameters and expander widgets
int HydGui_Rv_Profile_General_Wid::get_number_of_boundary_param_wids(void) {
	return this->number_of_boundary_params;
}
//Set main properties of the boundary parameters
void HydGui_Rv_Profile_General_Wid::set_boundary_params_properties(const int index, const string title, const int sc_id) {
	if (index < this->number_of_boundary_params) {
		this->expander_wids[index]->set_title(title);
		this->boundary_lateral_wids[index]->set_sc_id(sc_id);
		this->boundary_point_wids[index]->set_sc_id(sc_id);
	}
}
//Set all members of the widget, similar to a copy constructor
void HydGui_Rv_Profile_General_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	HydGui_Rv_Profile_General_Wid *other = (HydGui_Rv_Profile_General_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general settings
	ui.profile_name->set_text(other->ui.profile_name->get_text());
	ui.profile_type->set_editable(false);
	ui.profile_type->set_current_value(other->ui.profile_type->get_current_index());
	ui.river_station->set_value(other->ui.river_station->get_value());
	ui.delta_x_table->set_value(other->ui.delta_x_table->get_value());
	ui.connection_type->set_current_value(other->ui.connection_type->get_current_index());
	ui.init_condition->set_value(other->ui.init_condition->get_value());
	//overflow settings
	this->overflow_params->set_overflow_coupling_left(other->overflow_params->overflow_coupling_left->get_value());
	this->overflow_params->set_overflow_coupling_right(other->overflow_params->overflow_coupling_right->get_value());
	this->overflow_params->poleni_fac_left->set_value(other->overflow_params->poleni_fac_left->get_value());
	this->overflow_params->poleni_fac_right->set_value(other->overflow_params->poleni_fac_right->get_value());
	//break settings
	this->break_params->set_left_break(other->break_params->break_left_check->isChecked());
	this->break_params->set_right_break(other->break_params->break_right_check->isChecked());
	this->break_params->set_abrupt_breach_left(other->break_params->get_abrupt_breach_left());
	this->break_params->set_abrupt_breach_right(other->break_params->get_abrupt_breach_right());
	this->break_params->wall_resistance_left->set_value(other->break_params->wall_resistance_left->get_value());
	this->break_params->wall_resistance_right->set_value(other->break_params->wall_resistance_right->get_value());
	this->break_params->abrupt_breach_open_left->set_value(other->break_params->abrupt_breach_open_left->get_value());
	this->break_params->abrupt_breach_open_right->set_value(other->break_params->abrupt_breach_open_right->get_value());
	this->break_params->critical_velocity_left->set_value(other->break_params->critical_velocity_left->get_value());
	this->break_params->critical_velocity_right->set_value(other->break_params->critical_velocity_right->get_value());
	//boundary settings
	if (this->boundary_lateral_wids != NULL && this->boundary_point_wids != NULL) {
		if (this->number_of_boundary_params == other->number_of_boundary_params) {
			for (int i = 0; i < this->number_of_boundary_params; i++) {
				this->boundary_lateral_wids[i]->set_condition(other->boundary_lateral_wids[i]->boundary_lateral_condition->get_value());
				this->boundary_lateral_wids[i]->set_stationary(other->boundary_lateral_wids[i]->boundary_lateral_stationary->get_value());
				this->boundary_lateral_wids[i]->boundary_lateral_value->set_value(other->boundary_lateral_wids[i]->boundary_lateral_value->get_value());

				this->boundary_point_wids[i]->set_condition(other->boundary_point_wids[i]->boundary_point_condition->get_value());
				this->boundary_point_wids[i]->set_stationary(other->boundary_point_wids[i]->boundary_point_stationary->get_value());
				this->boundary_point_wids[i]->boundary_point_value->set_value(other->boundary_point_wids[i]->boundary_point_value->get_value());
			}
		}
	}
}

//Set default values
void HydGui_Rv_Profile_General_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general settings
	ui.profile_name->set_text("");
	ui.profile_type->set_current_value(0);
	ui.river_station->set_value(0.0);
	ui.delta_x_table->set_value(0);
	ui.connection_type->set_current_value(0);
	ui.init_condition->set_value(0.0);
	//overflow settings
	this->overflow_params->set_default_values();
	//break settings
	this->break_params->set_default_values();
	//boundary settings
	if (this->boundary_lateral_wids != NULL && this->boundary_point_wids != NULL) {
		for (int i = 0; i < this->number_of_boundary_params; i++) {
			this->boundary_lateral_wids[i]->set_default_values();
			this->boundary_point_wids[i]->set_default_values();
		}
	}
}

//Set the member of the widget per database
void HydGui_Rv_Profile_General_Wid::set_member(QSqlDatabase *ptr_database, const int rv_no, const int glob_prof_no, const _sys_system_id id){
	QSqlQueryModel query_result;
	this->ptr_database=ptr_database;
	int number=0;
	number=_Hyd_River_Profile::select_relevant_profile_in_database(&query_result, this->ptr_database, id, rv_no, glob_prof_no);

	if(number==0){
		ui.profile_name->set_text("Unknown profile");
		return;
	}

	_hyd_connection_types type;

	type=_Hyd_River_Profile::convert_string2connectiontype(query_result.record(0).value((_Hyd_River_Profile::profile_table->get_column_name(hyd_label::profdata_contyp)).c_str()).toString().toStdString());


	if(type==_hyd_connection_types::INFLOW_CONN){
		Hyd_River_Profile_Connection_Inflow buffer;
		buffer.set_systemid(id);
		buffer.input_members_per_database(0, &query_result, this->ptr_database);
		this->set_profile_members2widget(&buffer);
		this->set_profile_boundary2widget(&buffer, id, glob_prof_no);

	}
	else if(type==_hyd_connection_types::STANDARD_CONN){
		Hyd_River_Profile_Connection_Standard buffer;
		buffer.set_systemid(id);
		buffer.input_members_per_database(0, &query_result, this->ptr_database);
		this->set_profile_members2widget(&buffer);
		this->set_profile_boundary2widget(&buffer, id, glob_prof_no);

	}
	else if(type==_hyd_connection_types::OUTFLOW_CONN){
		Hyd_River_Profile_Connection_Outflow buffer;
		buffer.set_systemid(id);
		buffer.input_members_per_database(0, &query_result, this->ptr_database);
		this->set_profile_members2widget(&buffer);
		//this->set_profile_boundary2widget(&buffer, id);
	}
	else{
		ui.profile_name->set_text("Unknown profile connection type");
		return;
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void HydGui_Rv_Profile_General_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_Rv_Profile_General_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_number_of_boundary_param_wids(this->number_of_boundary_params);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	for (int i = 0; i < this->number_of_boundary_params; i++) {
		inDiaWid.set_boundary_params_properties(i, this->expander_wids[i]->get_title(), this->boundary_lateral_wids[i]->get_sc_id());
	}
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
//Open all expander widgets
void HydGui_Rv_Profile_General_Wid::open_expander_widgets(void){

	ui.break_expander->set_expanded(true);
	ui.overflow_expander->set_expanded(true);
	//boundary
	for(int i=0; i< this->number_of_boundary_params; i++){
		this->expander_wids[i]->set_expanded(true);
	}

}
//Close all expander widgets
void HydGui_Rv_Profile_General_Wid::close_expander_widgets(void){
	ui.break_expander->set_expanded(false);
	ui.overflow_expander->set_expanded(false);
	//boundary
	for(int i=0; i< this->number_of_boundary_params; i++){
		this->expander_wids[i]->set_expanded(false);
	}
}
//_______________
//private methods
//Allocate boundary lateral widgets, boundary point widgets and expander wids
void HydGui_Rv_Profile_General_Wid::allocate_boundary_params(void) {
	this->delete_boundary_params();

	//allocate
	this->expander_wids = new Sys_Base_Expand_Wid*[this->number_of_boundary_params];
	this->boundary_lateral_wids = new HydGui_Rv_Profile_Boundary_Lateral_Params_Wid*[this->number_of_boundary_params];
	this->boundary_point_wids = new HydGui_Rv_Profile_Boundary_Point_Params_Wid*[this->number_of_boundary_params];
}
//Delete boundary lateral widgets, boundary point widgets and expander wids
void HydGui_Rv_Profile_General_Wid::delete_boundary_params(void) {

	//delete if necessary
	if (this->boundary_lateral_wids != NULL) {

		for (int i = 0; i < this->number_of_boundary_params; i++) {
		
			if (this->boundary_lateral_wids[i] != NULL) {
				delete [] this->boundary_lateral_wids[i];
				this->boundary_lateral_wids[i] = NULL;
			}
		}

		delete [] this->boundary_lateral_wids;
		this->boundary_lateral_wids = NULL;
	}

	if (this->boundary_point_wids != NULL) {

		for (int i = 0; i < this->number_of_boundary_params; i++) {
		
			if (this->boundary_point_wids[i] != NULL) {
				delete [] this->boundary_point_wids[i];
				this->boundary_point_wids[i] = NULL;
			}
		}

		delete [] this->boundary_point_wids;
		this->boundary_point_wids = NULL;
	}

	if (this->expander_wids != NULL) {

		for (int i = 0; i < this->number_of_boundary_params; i++) {

			if (this->expander_wids[i] != NULL) {
				delete [] this->expander_wids[i];
				this->expander_wids[i] = NULL;
			}
		}

		delete [] this->expander_wids;
		this->expander_wids = NULL;
	}		
}
//Set the general members of a river profile to the display widget
void HydGui_Rv_Profile_General_Wid::set_profile_members2widget(_Hyd_River_Profile *prof){
	//general settings
	ui.profile_name->set_text(prof->name);
	ui.profile_type->set_editable(true);
	ui.profile_type->set_current_value(prof->type);
	//set pixmap
	QPixmap pixmap;
	pixmap.load(":river_icon" );
	if(prof->type== _hyd_profile_types::RIVER_TYPE){
		pixmap.load(":riverprof_icon" );
	}
	else if(prof->type== _hyd_profile_types::WEIR_TYPE){
		pixmap.load(":weir_icon" );
	}
	else if(prof->type== _hyd_profile_types::BRIDGE_TYPE){
		pixmap.load(":bridge_icon" );
	}
	this->set_headPixmap(pixmap);

	ui.profile_type->set_editable(false);
	ui.river_station->set_value(prof->river_station);
	ui.delta_x_table->set_value(prof->delta_x_table);
	ui.connection_type->set_editable(true);
	ui.connection_type->set_current_value(prof->connection);
	ui.connection_type->set_editable(false);
	ui.init_condition->set_value(prof->init_condition);

	//overflow settings
	this->overflow_params->set_values(prof->overflow_left_flag, prof->poleni_factor_left_overflow, prof->overflow_right_flag, prof->poleni_factor_right_overflow);

	//break settings
	this->break_params->set_left_break_params(prof->get_break_flag_left(), prof->get_break_parameter_left(), prof->index_basepoint_left);
	this->break_params->set_right_break_params(prof->get_break_flag_right(), prof->get_break_parameter_right(), prof->index_basepoint_right);

	//points
	ui.river_geo->set_rows(prof->typ_of_profile->get_number_points());

	for(int i=0; i< prof->typ_of_profile->get_number_points() ; i++){
		ui.river_geo->set_point_data(i, prof->typ_of_profile->points[i].get_global_x_y_coordinates().get_xcoordinate(),
			prof->typ_of_profile->points[i].get_global_x_y_coordinates().get_ycoordinate(),prof->typ_of_profile->points[i].get_global_z_coordinate(),
			prof->typ_of_profile->points[i].get_mat_id(), prof->typ_of_profile->points[i].get_xcoordinate(),
			prof->typ_of_profile->points[i].get_channel_type());
	}
	ui.river_geo->resizeColumnsToContents();


	//plot
	if(prof->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->ui.plot_wid->getPlotPtr()->set_number_curves(4);
	}
	else{
		this->ui.plot_wid->getPlotPtr()->set_number_curves(3);
	}
	this->ui.plot_wid->getPlotPtr()->set_axis_title("Local x [m]", "Local y [m]");
	this->ui.plot_wid->getPlotPtr()->set_curve_characteristics(0, "Left bank", QwtPlotCurve::Lines, Qt::green);
	this->ui.plot_wid->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
	this->ui.plot_wid->getPlotPtr()->set_curve_characteristics(1, "Main channel", QwtPlotCurve::Lines, Qt::blue);
	this->ui.plot_wid->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
	this->ui.plot_wid->getPlotPtr()->set_curve_characteristics(2, "Right bank", QwtPlotCurve::Lines, Qt::red);
	this->ui.plot_wid->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::Diamond);
	if(prof->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->ui.plot_wid->getPlotPtr()->set_curve_characteristics(3, "Bridge Body", QwtPlotCurve::Lines, Qt::black);
	}

	this->ui.plot_wid->getPlotPtr()->set_legend(true);

	int no_points=0;
	double *x_array=NULL;
	double *y_array=NULL;

	try{
		//left bank
		no_points=prof->typ_of_profile->get_points_left_bank(&x_array, &y_array);
		if(x_array!=NULL && y_array!=NULL){
			this->ui.plot_wid->getPlotPtr()->set_curve_data(0,no_points, x_array, y_array);
		}
		if(x_array!=NULL){
			delete []x_array;
			x_array=NULL;
		}
		if(y_array!=NULL){
			delete []y_array;
			y_array=NULL;
		}
		//mid channel
		no_points=prof->typ_of_profile->get_points_main_channel(&x_array, &y_array);
		if(x_array!=NULL && y_array!=NULL){
			this->ui.plot_wid->getPlotPtr()->set_curve_data(1,no_points, x_array, y_array);
		}
		if(x_array!=NULL){
			delete []x_array;
			x_array=NULL;
		}
		if(y_array!=NULL){
			delete []y_array;
			y_array=NULL;
		}
		//right bank
		no_points=prof->typ_of_profile->get_points_right_bank(&x_array, &y_array);
		if(x_array!=NULL && y_array!=NULL){
			this->ui.plot_wid->getPlotPtr()->set_curve_data(2,no_points, x_array, y_array);
		}
		if(x_array!=NULL){
			delete []x_array;
			x_array=NULL;
		}
		if(y_array!=NULL){
			delete []y_array;
			y_array=NULL;
		}
		double max_z=0.0;

		if(prof->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
			Hyd_River_Profile_Type_Bridge *buffer=(Hyd_River_Profile_Type_Bridge *)prof->typ_of_profile;
			no_points=buffer->get_bridge_data2plot(&x_array, &y_array);
			if(x_array!=NULL && y_array!=NULL){
				this->ui.plot_wid->getPlotPtr()->set_curve_data(3,no_points, x_array, y_array);
			}
			if(x_array!=NULL){
				delete []x_array;
				x_array=NULL;
			}
			if(y_array!=NULL){
				delete []y_array;
				y_array=NULL;
			}
			max_z=buffer->get_bridge_data().bridgebody_size+buffer->get_bridge_data().local_bridge_height+0.5;
		}
		if(prof->typ_of_profile->get_local_zmax()+0.5>max_z){
			max_z=prof->typ_of_profile->get_local_zmax()+0.5;
		}
		
		this->ui.plot_wid->getPlotPtr()->set_scale_y_axis(0.0, max_z);
		this->ui.plot_wid->getPlotPtr()->show_plot();
	}
	catch(Error msg){
		msg.output_msg(0);
	}

	

}
//Set profile boundary condition members to the widget
void HydGui_Rv_Profile_General_Wid::set_profile_boundary2widget(Hyd_River_Profile_Connection_Standard *prof, const _sys_system_id id, const int glob_prof_id){
	//boundary scenarios
	Hyd_Boundary_Szenario_Management bound_buff;
	bound_buff.set_systemid(id);
	bound_buff.set_szenario_per_db(this->ptr_database);

	int counter_total=0;

	//count them
	for(int i=0; i< bound_buff.get_number_sz();i++){

		prof->hyd_sz.set_members(bound_buff.get_ptr_sz(i));
		prof->input_boundary_data_per_database(this->ptr_database, glob_prof_id);
		if(prof->get_boundary_point_flag()==true || prof->get_boundary_lateral_flag()==true){
			counter_total++;
		}

		prof->reset_boundary_data();

	}
	this->set_number_of_boundary_param_wids(counter_total);

	//set them
	counter_total=0;
	for(int i=0; i< bound_buff.get_number_sz();i++){

		prof->hyd_sz.set_members(bound_buff.get_ptr_sz(i));
		prof->input_boundary_data_per_database(this->ptr_database, glob_prof_id);
		if(prof->get_boundary_point_flag()==true || prof->get_boundary_lateral_flag()==true){

			this->set_boundary_params_properties(counter_total, bound_buff.get_ptr_sz(i)->get_sz_text(), bound_buff.get_ptr_sz(i)->get_id());
			if(prof->get_boundary_lateral_flag()==true){

				//lateral
				if(prof->boundary_lateral_stat_flag==false){
					this->boundary_lateral_wids[counter_total]->set_values(prof->boundary_lateral_stat_flag, *prof->number_boundary_lateral_curve);
				}
				else{
					this->boundary_lateral_wids[counter_total]->set_values(prof->boundary_lateral_stat_flag, *prof->value_boundary_lateral_stat);
				}
			}
			if(prof->get_boundary_point_flag()==true){
				//point
				if(prof->boundary_point_stat_flag==false){
					this->boundary_point_wids[counter_total]->set_values(prof->boundary_point_stat_flag, *prof->number_boundary_point_curve);
				}
				else{
					this->boundary_point_wids[counter_total]->set_values(prof->boundary_point_stat_flag, *prof->value_boundary_point_stat);
				}
			}
				
				
			counter_total++;
		}

		prof->reset_boundary_data();

	}




}
//Transfer members to database
void HydGui_Rv_Profile_General_Wid::transfer_members2database(HydGui_Rv_Profile_General_Wid *){
	////the table is set (the name and the column names) and allocated
	//try{
	//	Hyd_Param_Global::set_table(ptr_database);
	//}
	//catch(Error msg){
	//	throw msg;
	//}

	//QSqlQueryModel query;

	//ostringstream query_string;
	//query_string <<"UPDATE ";
	//query_string << Hyd_Param_Global::global_param_table->get_table_name();
	//query_string << " SET " ;
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstart) << " = "<< dialog->ui.tstart->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstep) << " = "<< dialog->ui.tstep->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tnof) << " = "<< dialog->ui.tnof->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofits) << " = "<< dialog->ui.tnofits->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxnumsteps) << " = "<< dialog->ui.maxnumsteps->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxstepsize) << " = "<< dialog->ui.maxstepsize->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::inistepsize) << " = "<< dialog->ui.initstepsize->get_value()<< " , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::pretype) << " = '"<< dialog->ui.pretype->get_current_value()<< "' , ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::gramschmidt) << " = '"<< dialog->ui.gramschmidt->get_current_value()<< "' ";
	//query_string  << " WHERE ";
	//query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofset) << " = "  << this->set_spinBox->value()-1;
	//
	//Data_Base::database_request(&query, query_string.str(), ptr_database);

	//if(query.lastError().isValid()){
	//	Error msg=set_error(0);
	//	ostringstream info;
	//	info << "Table Name      : " << Hyd_Param_Global::global_param_table->get_table_name() << endl;
	//	info << "Table error info: " << query.lastError().text().toStdString() << endl;
	//	msg.make_second_info(info.str());
	//	msg.output_msg(0);	
	//}

	//this->set_member(dialog);
}
//set the error
Error HydGui_Rv_Profile_General_Wid::set_error(const int err_type){
		string place="HydGui_Rv_Profile_General_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(HydGui_Rv_Profile_General_Wid *dialog)") ;
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
