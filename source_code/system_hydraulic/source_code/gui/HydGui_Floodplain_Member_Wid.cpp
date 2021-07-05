//#include "HydGui_Floodplain_Member_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//default constructor
HydGui_Floodplain_Member_Wid::HydGui_Floodplain_Member_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;

	this->set_headLabel_text("Floodplain Model Parameter");

	// FLOODPLAIN_NAME
	ui.floodplain_name->set_label_text("Floodplain name");
	ui.floodplain_name->set_tooltip("");

	// FLOODPLAIN_NUMBER
	ui.floodplain_number->set_label_text("Floodplain number");
	ui.floodplain_number->set_tooltip("");
	ui.floodplain_number->set_range(0, 100);

	// NX
	ui.nx->set_label_text("Number of elements in x-direction [-]");
	ui.nx->set_tooltip("Number of elements in x-direction [-]");
	ui.nx->set_range(0, 10000);

	// NY
	ui.ny->set_label_text("Number of elements in y-direction [-]");
	ui.ny->set_tooltip("Number of elements in y-direction [-]");
	ui.ny->set_range(0, 10000);

	// ELEMENTS_X_WIDTH
	ui.elements_x->set_label_text("Width of the elements in x-direction [m]");
	ui.elements_x->set_tooltip("Width of the element in x-direction [m]");

	// ELEMENTS_Y_WIDTH
	ui.elements_y->set_label_text("Width of the elements in y-direction [m]");
	ui.elements_y->set_tooltip("Width of the elements in y-direction [m]");

	// ANGLE
	ui.angle->set_label_text("Angle of the coordinate system [°]");
	ui.angle->set_tooltip("Angle of the coordinate system related to the normal x-direction (--->) [°]");
	ui.angle->set_dataRole(role);
	ui.angle->set_range(-360.0, 360.0);

	// LOW_LEFT_X
	ui.low_left_x->set_label_text("Low left x coordinate [m]");
	ui.low_left_x->set_tooltip("Low left x coordinate, location of the origin [m]");
	ui.low_left_x->set_dataRole(role);
	ui.low_left_x->set_range(-999999999.0, 999999999.0);

	// LOW_LEFT_Y
	ui.low_left_y->set_label_text("Low left y coordinate [m]");
	ui.low_left_y->set_tooltip("Low left y coordinate, location of the origin [m]");
	ui.low_left_y->set_dataRole(role);
	ui.low_left_y->set_range(-999999999.0, 999999999.0);

	// NOINFO_VALUE
	ui.noinfo_value->set_label_text("No info value");
	ui.noinfo_value->set_tooltip("Value for a element with no information");
	ui.noinfo_value->set_value(-9999.0);
	ui.noinfo_value->set_dataRole(role);
	ui.noinfo_value->set_range(-999999999.0, 999999999.0);
	// TWOD_OUTPUT
	ui.twod_output->set_label_text("Full path to 2D Output File");
	ui.twod_output->set_tooltip("Name of file, where the 2d output is printed in tecplot-format (geometrical as well as result output) [optional; if not set, no output will be performed]");
	this->ui.groupBox_2->hide();
	// REL_TOL
	ui.rel_tol->set_label_text("Relative tolerance");
	ui.rel_tol->set_tooltip("Defines the relative tolerances (related to the results) for solving the system [optional; standard value = 1e-8]");
	ui.rel_tol->set_precision(2);
	ui.rel_tol->set_range(-20, 20);
	ui.rel_tol->set_value(0.0000005);
	ui.rel_tol->set_dataRole(role);

	// ABS_TOL
	ui.abs_tol->set_label_text("Absolute tolerance");
	ui.abs_tol->set_tooltip("Defines the absolute tolerances (related to the results) for solving the system [optional; standard value = 1e-5]");
	ui.abs_tol->set_precision(2);
	ui.abs_tol->set_range(-20, 20);
	ui.abs_tol->set_value(0.0000005);
	ui.abs_tol->set_dataRole(role);

	// WET_TOL
	ui.wet_tol->set_label_text("Tolerance, whether an element is regarded as wet or dry [m]");
	ui.wet_tol->set_tooltip("Defines if an element is regarded as wet or dry [m] [optional; standard value = 1e-3]");
	ui.wet_tol->set_precision(2);
	ui.wet_tol->set_range(-20, 20);
	ui.wet_tol->set_value(0.01);
	ui.wet_tol->set_dataRole(role);

	//SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			//this->head_pixmap->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogInfoView));
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));
	}
	//set icon
	QPixmap fp_icon;
	fp_icon.load(":raster_icon" );
	this->set_headPixmap(fp_icon);

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_Floodplain_Member_Wid), _sys_system_modules::HYD_SYS);
}

//default destructor
HydGui_Floodplain_Member_Wid::~HydGui_Floodplain_Member_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_Floodplain_Member_Wid), _sys_system_modules::HYD_SYS);
}
//________
//public
//method to set the whole widget editable
void HydGui_Floodplain_Member_Wid::set_editable(const bool state) {
	this->editable = state;

	ui.floodplain_name->set_editable(state);
	ui.floodplain_number->set_editable(state);
	ui.nx->set_editable(state);
	ui.ny->set_editable(state);
	ui.elements_x->set_editable(state);
	ui.elements_y->set_editable(state);
	ui.angle->set_editable(state);
	ui.low_left_x->set_editable(state);
	ui.low_left_y->set_editable(state);
	ui.noinfo_value->set_editable(state);
	ui.twod_output->set_editable(state);
	ui.rel_tol->set_editable(state);
	ui.abs_tol->set_editable(state);
	ui.wet_tol->set_editable(state);
}

//Set all members of the widget, similar to a copy constructor
void HydGui_Floodplain_Member_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	HydGui_Floodplain_Member_Wid *other = (HydGui_Floodplain_Member_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general settings
	ui.floodplain_name->set_text(other->ui.floodplain_name->get_text());
	ui.floodplain_number->set_value(other->ui.floodplain_number->get_value());
	ui.floodplain_number->set_editable(false);
	ui.nx->set_value(other->ui.nx->get_value());
	ui.nx->set_editable(false);
	ui.ny->set_value(other->ui.ny->get_value());
	ui.ny->set_editable(false);
	ui.elements_x->set_value(other->ui.elements_x->get_value());
	ui.elements_x->set_editable(false);
	ui.elements_y->set_value(other->ui.elements_y->get_value());
	ui.elements_y->set_editable(false);
	ui.angle->set_value(other->ui.angle->get_value());
	ui.angle->set_editable(false);
	ui.low_left_x->set_value(other->ui.low_left_x->get_value());
	ui.low_left_x->set_editable(false);
	ui.low_left_y->set_value(other->ui.low_left_y->get_value());
	ui.low_left_y->set_editable(false);
	ui.noinfo_value->set_value(other->ui.noinfo_value->get_value());
	ui.noinfo_value->set_editable(false);
	//file settings
	ui.twod_output->set_text(other->ui.twod_output->get_text());
	ui.twod_output->set_editable(false);
	//limit settings
	ui.rel_tol->set_value(other->ui.rel_tol->get_value());
    ui.rel_tol->set_range(-10.0,-3.0);
	ui.abs_tol->set_value(other->ui.abs_tol->get_value());
    ui.abs_tol->set_range(-6.0,-3.0);
	ui.wet_tol->set_value(other->ui.wet_tol->get_value());
	ui.wet_tol->set_range(-6.0,-1.0);
}

//Set default values
void HydGui_Floodplain_Member_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general settings
	ui.floodplain_name->set_text("");
	ui.floodplain_number->set_value(0);
	ui.nx->set_value(0);
	ui.ny->set_value(0);
	ui.elements_x->set_value(0);
	ui.elements_y->set_value(0);
	ui.angle->set_value(0);
	ui.low_left_x->set_value(0);
	ui.low_left_y->set_value(0);
	ui.noinfo_value->set_value(0);
	//file settings
	ui.twod_output->set_text("");
	//limit settings
	ui.rel_tol->set_value(0);
	ui.abs_tol->set_value(0);
	ui.wet_tol->set_value(0);
}

//Set the member of the widget per database
void HydGui_Floodplain_Member_Wid::set_member(QSqlDatabase *ptr_database, const int fp_no){
	QSqlQueryModel results;
	this->ptr_database=ptr_database;
	int number=0;
	try{
		number=Hyd_Model_Floodplain::select_relevant_model_database(&results, ptr_database, fp_no);
	}
	catch(Error msg){
		throw msg;
	}


	this->ui.floodplain_number->set_value(fp_no);

	if(number==0){
		this->ui.floodplain_name->set_text("Unknown FP-No.");
		return;
	}
	else{

		this->ui.abs_tol->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::atol)).c_str()).toDouble());
		this->ui.rel_tol->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::rtol)).c_str()).toDouble());
		//name
		this->ui.floodplain_name->set_text(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString());
		//gemetrical settings
		this->ui.nx->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::nx)).c_str()).toInt());
		this->ui.ny->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::ny)).c_str()).toInt());
		this->ui.elements_x->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_x)).c_str()).toDouble());
		this->ui.elements_y->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::elemwidth_y)).c_str()).toDouble());
		this->ui.low_left_x->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowleftx)).c_str()).toDouble());
		this->ui.low_left_y->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::lowlefty)).c_str()).toDouble());
		this->ui.angle->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::angle)).c_str()).toDouble());
		this->ui.noinfo_value->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::noinfovalue)).c_str()).toDouble());
		this->ui.wet_tol->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::wet)).c_str()).toDouble());
		this->ui.twod_output->set_text(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::d2output)).c_str()).toString().toStdString());
		//description
		//this->ui.wet_tol->set_value(results.record(0).value((Hyd_Model_Floodplain::general_param_table->get_column_name(label::description)).c_str()).toString().toStdString());
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void HydGui_Floodplain_Member_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_Floodplain_Member_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	if(dialog.start_dialog()==true){
		//transfer members to database
		this->transfer_members2database(&inDiaWid);
		
	}
	//rejected
	else{
		return;	
	}
}
//___________
//private
//Transfer members to database
void HydGui_Floodplain_Member_Wid::transfer_members2database(HydGui_Floodplain_Member_Wid *dialog){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Model_Floodplain::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Model_Floodplain::general_param_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::atol) << " = "<< dialog->ui.abs_tol->get_value()<< " , ";
	query_string  << Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::rtol) << " = "<< dialog->ui.rel_tol->get_value()<< " , ";
	query_string  << Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_name) << " = '"<< dialog->ui.floodplain_name->get_text()<< "' , ";
	query_string  << Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::wet) << " = "<< dialog->ui.wet_tol->get_value()<< "  ";
	query_string  << " WHERE ";
	query_string  << Hyd_Model_Floodplain::general_param_table->get_column_name(hyd_label::genmod_id) << " = "  << dialog->ui.floodplain_number->get_value();
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_Floodplain::general_param_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	this->set_member(dialog);
}
//set the error
Error HydGui_Floodplain_Member_Wid::set_error(const int err_type){
		string place="HydGui_Floodplain_Member_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(HydGui_Floodplain_Member_Wid *dialog)") ;
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
