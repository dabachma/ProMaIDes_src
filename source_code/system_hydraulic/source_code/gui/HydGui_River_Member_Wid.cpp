//#include "HydGui_River_Member_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//Default Constructor
HydGui_River_Member_Wid::HydGui_River_Member_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;
	this->set_headLabel_text("River Model Parameter");

	

	// SELF
	// RIVER_NAME
	this->ui.river_name->set_label_text("River Name");
	this->ui.river_name->set_tooltip("");

	// RIVER_NUMBER
	this->ui.river_number->set_label_text("River Number");
	this->ui.river_number->set_tooltip("");
	this->ui.river_number->set_range(0, 9999);

	// ONED_OUTPUT
	this->ui.oned_output->set_label_text("Full path to 1D Output File");
	this->ui.oned_output->set_tooltip("Name of file, where the 1d output is printed in tecplot-format (geometrical as well as result output) [optional; if not set, no output will be performed]");

	// TWOD_OUTPUT
	this->ui.twod_output->set_label_text("Full path to 2D Output File");
	this->ui.twod_output->set_tooltip("Name of file, where the 2d output is printed in tecplot-format (geometrical as well as result output) [optional; if not set, no output will be performed]");

	this->ui.groupBox_2->hide();

	// REL_TOL
	this->ui.rel_tol->set_label_text("Relative Tolerance");
	this->ui.rel_tol->set_tooltip("Defines the relative tolerances (related to the results) for solving the system [optional; standard value = 1e-7]");
	this->ui.rel_tol->set_suffix("[-]");
	this->ui.rel_tol->set_range(-15, 10);
	this->ui.rel_tol->set_dataRole(role);

	// ABS_TOL
	this->ui.abs_tol->set_label_text("Absolute Tolerance");
	this->ui.abs_tol->set_tooltip("Defines the absolute tolerances (related to the results) for solving the system [optional; standard value = 1e-8]");
	this->ui.abs_tol->set_suffix("[-]");
	this->ui.abs_tol->set_range(-15, 10);
	this->ui.abs_tol->set_dataRole(role);

	// v_explicit
	this->ui.v_explicit->set_label_text("Use velocity head explicit");
	this->ui.v_explicit->set_tooltip("Flag if the explicit velocity-approach for the calculation of the gradient ist used");

	// v_explicit
	this->ui.prof_interface->set_label_text("Use profile interface from main channel");
	this->ui.prof_interface->set_tooltip("Flag for a interface approach for composed profiles: false:= no interface is taken between right/left bank and main channel; true:=interface is taken from the main channel");



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
	QPixmap rv_icon;
	rv_icon.load(":river_icon" );
	this->set_headPixmap(rv_icon);

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_River_Member_Wid), _sys_system_modules::HYD_SYS);
}

//Default Destructor
HydGui_River_Member_Wid::~HydGui_River_Member_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_River_Member_Wid), _sys_system_modules::HYD_SYS);
}

//______
//public
//Decides wether the widget is just to view or to edit the shown data
void HydGui_River_Member_Wid::set_editable(const bool state) {
	this->editable = state;

	this->ui.river_name->set_editable(state);
	this->ui.river_number->set_editable(state);
	this->ui.twod_output->set_editable(state);
	this->ui.oned_output->set_editable(state);
	this->ui.rel_tol->set_editable(state);
	this->ui.abs_tol->set_editable(state);

	this->ui.v_explicit->set_editable(state);
	this->ui.prof_interface->set_editable(state);
}

//Set all members of the widget, similar to a copy constructor
void HydGui_River_Member_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	HydGui_River_Member_Wid *other = (HydGui_River_Member_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	//general settings
	this->ui.river_name->set_text(other->ui.river_name->get_text());
	this->ui.river_number->set_value(other->ui.river_number->get_value());
	this->ui.river_number->set_editable(false);
	//file settings
	this->ui.oned_output->set_text(other->ui.oned_output->get_text());
	this->ui.twod_output->set_text(other->ui.twod_output->get_text());
	//limit settings
	this->ui.rel_tol->set_value(other->ui.rel_tol->get_value());
	this->ui.rel_tol->set_range(-14.0,-3.0);
	this->ui.abs_tol->set_value(other->ui.abs_tol->get_value());
	this->ui.abs_tol->set_range(-14.0,-3.0);

	this->ui.v_explicit->set_value(other->ui.v_explicit->get_value());
	this->ui.prof_interface->set_value(other->ui.prof_interface->get_value());

}
//Set default values
void HydGui_River_Member_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general settings
	this->ui.river_name->set_text("");
	this->ui.river_number->set_value(0);
	//file settings
	this->ui.oned_output->set_text("");
	this->ui.twod_output->set_text("");
	//limit settings
	this->ui.rel_tol->set_value(0);
	this->ui.abs_tol->set_value(0);

	this->ui.v_explicit->set_value(false);
	this->ui.prof_interface->set_value(false);
}

//Set the member of the widget per database
void HydGui_River_Member_Wid::set_member(QSqlDatabase *ptr_database, const int rv_no){
	QSqlQueryModel results;
	this->ptr_database=ptr_database;
	int number=0;
	try{
		number=Hyd_Model_River::select_relevant_model_database(&results, ptr_database, rv_no);
	}
	catch(Error msg){
		throw msg;
	}


	this->ui.river_number->set_value(rv_no);
	ui.river_number->set_editable(false);

	if(number==0){
		this->ui.river_name->set_text("Unknown RV-No.");
		return;
	}
	else{



		this->ui.abs_tol->set_value(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::atol)).c_str()).toDouble());
		this->ui.rel_tol->set_value(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::rtol)).c_str()).toDouble());
		this->ui.v_explicit->set_value(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::v_explicit)).c_str()).toBool());
		this->ui.prof_interface->set_value(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::prof_interface)).c_str()).toBool());
		//name
		this->ui.river_name->set_text(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name)).c_str()).toString().toStdString());

		this->ui.oned_output->set_text(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::d1output)).c_str()).toString().toStdString());
		this->ui.twod_output->set_text(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(hyd_label::d2output)).c_str()).toString().toStdString());
		
		
		//description
		//this->ui.wet_tol->set_value(results.record(0).value((Hyd_Model_River::general_param_table->get_column_name(label::description)).c_str()).toString().toStdString());


	}

}
//____________
//public slots
//Open the dialog to edit values in the widget
void HydGui_River_Member_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_River_Member_Wid inDiaWid(DataRole::EditRole, &dialog);
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
//________
//private
//Transfer members to database
void HydGui_River_Member_Wid::transfer_members2database(HydGui_River_Member_Wid *dialog){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Model_River::set_table(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Model_River::general_param_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::atol) << " = "<< dialog->ui.abs_tol->get_value()<< " , ";
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::rtol) << " = "<< dialog->ui.rel_tol->get_value()<< " , ";
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_name) << " = '"<< dialog->ui.river_name->get_text()<< "' , ";
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::v_explicit) << " = "<< functions::convert_boolean2string(dialog->ui.v_explicit->get_value())<< " , ";
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::prof_interface) << " = "<< functions::convert_boolean2string(dialog->ui.prof_interface->get_value())<< "  ";

	query_string  << " WHERE ";
	query_string  << Hyd_Model_River::general_param_table->get_column_name(hyd_label::genmod_id) << " = "  << dialog->ui.river_number->get_value();
	
	Data_Base::database_request(&query, query_string.str(), ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Hyd_Model_River::general_param_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	this->set_member(dialog);
}
//set the error
Error HydGui_River_Member_Wid::set_error(const int err_type){
		string place="HydGui_River_Member_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(HydGui_River_Member_Wid *dialog)") ;
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
