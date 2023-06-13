//#include "HydGui_System_Member_Wid.h"
#include "Hyd_Headers_Precompiled.h"

//default constructor
HydGui_System_Member_Wid::HydGui_System_Member_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);
	this->area2print=ui.scrollAreaWidgetContents;

	this->set_headLabel_text("System Parameter");


	// TSTART
	ui.tstart->set_label_text("Start timestep [s]");
	ui.tstart->set_tooltip("Starting time of the simulation [s] [optional; standard value =0.0]");
	ui.tstart->set_range(0, 10000000000);
	ui.tstart->set_increment(86400.0);
	ui.tstart->set_dataRole(role);

	// TSTEP
	ui.tstep->set_label_text("Size of a output timestep [s]");
	ui.tstep->set_tooltip("Size of output timestep times number of output timesteps = total real time [s]");
	ui.tstep->set_range(0, 100000.0);
	ui.tstep->set_increment(1000.0);
	ui.tstep->set_dataRole(role);

	// TNOF
	ui.tnof->set_label_text("Number of output timesteps [-]");
	ui.tnof->set_tooltip("Size of output timestep times number of output timesteps = total real time [s]");
	ui.tnof->set_range(0, 12000000);
	ui.tnof->set_increment(10);
	
	// NOFITS
	ui.tnofits->set_label_text("Number of internal timesteps in one output timestep [-]");
	ui.tnofits->set_tooltip("The syncronisation of the models is performed at this timepoints");
	ui.tnofits->set_range(0, 20000);
	ui.tnofits->set_increment(10);

	// MAXNUMSTEPS
	ui.maxnumsteps->set_label_text("Maximum number of solver timesteps to reach the next internal timestep [-]");
	ui.maxnumsteps->set_tooltip("[optional; standard value = 40000]");
	ui.maxnumsteps->set_range(0, 100000);
	ui.maxnumsteps->set_increment(1000);

	// MAXSTEPSIZE
	ui.maxstepsize->set_label_text("Maximum solver stepsize [s]");
	ui.maxstepsize->set_tooltip("Maximum solver stepsize  for the solver timesteps [s] [optional; standard value = 3000.0]");
	ui.maxstepsize->set_range(0, 5000000.0);
	ui.maxstepsize->set_increment(1000.0);
	ui.maxstepsize->set_dataRole(role);

	// INITSTEPSIZE
	ui.initstepsize->set_label_text("Initial solver stepsize [s]");
	ui.initstepsize->set_tooltip("Initial stepsize for the solver [s] [optional; standard value = 0.0]");
	ui.initstepsize->set_range(0, 2000.0);
	ui.initstepsize->set_increment(1000.0);
	ui.initstepsize->set_dataRole(role);

	// PRETYPE
	ui.pretype->set_label_text("Preconditioner type");
	ui.pretype->set_tooltip("Preconditioner types: PREC_LEFT or PREC_RIGHT  [optional; standard value = PREC_LEFT]");
	string types[] = {"PREC_LEFT", "PREC_RIGHT"};
	ui.pretype->set_items(types, 2);

	// GRAMSCHMIDT
	ui.gramschmidt->set_label_text("Graham Schmidt Scheme");
	ui.gramschmidt->set_tooltip("Graham Schmidt schemes: CLASSICAL_GS or MODIFIED_GS [optional; standard value = MODIFIED_GS]");
	string schemes[] = { "MODIFIED_GS", "CLASSICAL_GS"};
	ui.gramschmidt->set_items(schemes, 2);

	// maximum waterlevel change in fp 
	ui.max_h_fp->set_label_text("Maximum of h change in floodplain element [m]");
	ui.max_h_fp->set_tooltip("Maximum of waterlevel change per syncronisation timestep in a floodplain element [optional, standard value = 0.1 m]");
	ui.max_h_fp->set_range(0.000001, 10000.0);
	ui.max_h_fp->set_increment(0.01);
	ui.max_h_fp->set_dataRole(role);

	// maximum waterlevel change in rv 
	ui.max_h_rv->set_label_text("Maximum of h change in river element [m]");
	ui.max_h_rv->set_tooltip("Maximum of waterlevel change per syncronisation timestep in a river element [optional, standard value = 0.1 m]");
	ui.max_h_rv->set_range(0.000001, 10000.0);
	ui.max_h_rv->set_increment(0.01);
	ui.max_h_rv->set_dataRole(role);


	// maximum explicit v-head change in rv 
	ui.max_v_rv->set_label_text("Maximum of explicit velocity head in river model [m] [under construction!]");
	ui.max_v_rv->set_tooltip("Maximum change of explicit velocity head per syncronisation timestep [optional, standard value = 0.1 m]");
	ui.max_v_rv->set_range(0.000001, 10000.0);
	ui.max_v_rv->set_increment(0.01);
	ui.max_v_rv->set_dataRole(role);

	// minimum stepsize of internal syncronisation step
	ui.min_intstep->set_label_text("Minimum step size for the syncronisation step [s]");
	ui.min_intstep->set_tooltip("Minimum of internal timstep per syncronisation step [optional, standard value = 10.0s]");
	ui.min_intstep->set_range(1.0, 10000.0);
	ui.min_intstep->set_increment(1.0);
	ui.min_intstep->set_dataRole(role);

	// output settings
	ui.output_1d_tecplot->set_label_text("Tecplot 1d output required");
	ui.output_1d_tecplot->set_tooltip("Please check it, if a 1d output for Tecplot is required");
	//ui.output_1d_tecplot->set_dataRole(role);

	ui.output_2d_tecplot->set_label_text("Tecplot 2d output required");
	ui.output_2d_tecplot->set_tooltip("Please check it, if a 2d output for Tecplot is required");
	//ui.output_2d_tecplot->set_dataRole(role);

	ui.output_1d_paraview->set_label_text("ParaView / Excel (csv) 1d output required");
	ui.output_1d_paraview->set_tooltip("Please check it, if a 1d output for ParaView /Excel is required");
	//ui.output_1d_paraview->set_dataRole(role);

	ui.output_2d_paraview->set_label_text("ParaView 2d output required");
	ui.output_2d_paraview->set_tooltip("Please check it, if a 2d output for ParaView is required");
	//ui.output_1d_paraview->set_dataRole(role);

	ui.output_2d_bluekenue->set_label_text("BlueKenue 2d output required");
	ui.output_2d_bluekenue->set_tooltip("Please check it, if a 2d output for BlueKenue is required");
	//ui.output_2d_bluekenue->set_dataRole(role);

	ui.output_instat_dbase->set_label_text("Instationary DataBase output required");
	ui.output_instat_dbase->set_tooltip("Please check it, if a instationary database output is required");
	//ui.output_instat_dbase->set_dataRole(role);

	//SELF
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

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_System_Member_Wid), _sys_system_modules::HYD_SYS);
}

//default destrucotr
HydGui_System_Member_Wid::~HydGui_System_Member_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_System_Member_Wid), _sys_system_modules::HYD_SYS);
}
//________
//public
//Decides wether the widget is just to view or to edit the shown data
void HydGui_System_Member_Wid::set_editable(const bool state) {
	this->editable = state;

	ui.tstart->set_editable(state);
	ui.tstep->set_editable(state);
	ui.tnof->set_editable(state);
	ui.tnofits->set_editable(state);
	ui.maxnumsteps->set_editable(state);
	ui.maxstepsize->set_editable(state);
	ui.initstepsize->set_editable(state);
	ui.pretype->set_editable(state);
	ui.gramschmidt->set_editable(state);

	ui.max_h_fp->set_editable(state);
	ui.max_h_rv->set_editable(state);
	ui.max_v_rv->set_editable(state);
	ui.min_intstep->set_editable(state);
	ui.output_1d_tecplot->set_editable(state);
	ui.output_2d_tecplot->set_editable(state);
	ui.output_1d_paraview->set_editable(state);
	ui.output_2d_paraview->set_editable(state);
	ui.output_2d_bluekenue->set_editable(state);
	ui.output_instat_dbase->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void HydGui_System_Member_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//casting to derived object
	HydGui_System_Member_Wid *other = (HydGui_System_Member_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "dynamic_cast error: could not cast object\n";
		return;
	}
	//time settings
	ui.tstart->set_value(other->ui.tstart->get_value());
	ui.tstep->set_value(other->ui.tstep->get_value());
	ui.tnof->set_value(other->ui.tnof->get_value());
	ui.tnofits->set_value(other->ui.tnofits->get_value());
	//solver settings
	ui.maxnumsteps->set_value(other->ui.maxnumsteps->get_value());
	ui.maxstepsize->set_value(other->ui.maxstepsize->get_value());
	ui.initstepsize->set_value(other->ui.initstepsize->get_value());
	//preconditioner settings
	ui.pretype->set_editable(true);
	ui.pretype->set_current_value(other->ui.pretype->get_current_index());
	ui.pretype->set_editable(false);
	ui.gramschmidt->set_editable(true);
	ui.gramschmidt->set_current_value(other->ui.gramschmidt->get_current_index());
	ui.gramschmidt->set_editable(false);
	//syncron settings
	ui.max_h_fp->set_value(other->ui.max_h_fp->get_value());
	ui.max_h_rv->set_value(other->ui.max_h_rv->get_value());
	ui.max_v_rv->set_value(other->ui.max_v_rv->get_value());
	ui.min_intstep->set_value(other->ui.min_intstep->get_value());

	//output settings
	ui.output_1d_tecplot->set_value(other->ui.output_1d_tecplot->get_value());
	ui.output_2d_tecplot->set_value(other->ui.output_2d_tecplot->get_value());
	ui.output_1d_paraview->set_value(other->ui.output_1d_paraview->get_value());
	ui.output_2d_paraview->set_value(other->ui.output_2d_paraview->get_value());
	ui.output_2d_bluekenue->set_value(other->ui.output_2d_bluekenue->get_value());
	ui.output_instat_dbase->set_value(other->ui.output_instat_dbase->get_value());

}

//Set default values
void HydGui_System_Member_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//time settings
	ui.tstart->set_value(0.0);
	ui.tstep->set_value(0.0);
	ui.tnof->set_value(0);
	ui.tnofits->set_value(0);
	//solver settings
	ui.maxnumsteps->set_value(0);
	ui.maxstepsize->set_value(0.0);
	ui.initstepsize->set_value(0.0);
	//preconditioner settings
	ui.pretype->set_current_value(0);
	ui.gramschmidt->set_current_value(0);

	//syncron settings
	ui.max_h_fp->set_value(0.01);
	ui.max_h_rv->set_value(0.01);
	ui.max_v_rv->set_value(0.1);
	ui.min_intstep->set_value(10.0);

	//output settings
	ui.output_1d_tecplot->set_value(false);
	ui.output_2d_tecplot->set_value(false);
	ui.output_1d_paraview->set_value(true);
	ui.output_2d_paraview->set_value(true);
	ui.output_2d_bluekenue->set_value(false);
	ui.output_instat_dbase->set_value(true);
}
//Set the member of the widget per database
void HydGui_System_Member_Wid::set_member(QSqlDatabase *ptr_database){
	Hyd_Param_Global buffer;
	this->ptr_database=ptr_database;
	try{
		buffer.globals_per_database(this->ptr_database, false);
	}
	catch(Error msg){
		throw msg;
	}

	ui.tstart->set_value(buffer.get_startime());
		// TSTEP

	ui.tstep->set_value(buffer.get_stepsize());

	// TNOF

	ui.tnof->set_value(buffer.get_number_timesteps());
	
	// NOFITS

	ui.tnofits->set_value(buffer.GlobNofITS);

	// MAXNUMSTEPS

	ui.maxnumsteps->set_value(buffer.GlobMaxNumSteps);

	// MAXSTEPSIZE

	ui.maxstepsize->set_value(buffer.GlobMaxStepSize);

	// INITSTEPSIZE

	ui.initstepsize->set_value(buffer.GlobInitStepSize);

	// PRETYPE
	ui.pretype->set_editable(true);
	ui.pretype->set_current_value(buffer.GlobPreType-1);
	ui.pretype->set_editable(false);
	// GRAMSCHMIDT
	ui.gramschmidt->set_editable(true);
	ui.gramschmidt->set_current_value(buffer.GlobGramSchmidt-1);
	ui.gramschmidt->set_editable(false);

	//syncron settings
	ui.max_h_fp->set_value(buffer.max_h_change_fp);
	ui.max_h_rv->set_value(buffer.max_h_change_rv);
	ui.max_v_rv->set_value(buffer.max_v_change_rv);
	ui.min_intstep->set_value(buffer.min_internal_step);

	//output settings
	ui.output_1d_tecplot->set_value(buffer.output_flags.tecplot_1d_required);
	ui.output_2d_tecplot->set_value(buffer.output_flags.tecplot_2d_required);
	ui.output_1d_paraview->set_value(buffer.output_flags.paraview_1d_required);
	ui.output_2d_paraview->set_value(buffer.output_flags.paraview_2d_required);
	ui.output_2d_bluekenue->set_value(buffer.output_flags.bluekenue_2d_required);
	ui.output_instat_dbase->set_value(buffer.output_flags.database_instat_required);
}
//____________
//public slots
//Open the dialog to edit values in the widget
void HydGui_System_Member_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	HydGui_System_Member_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	inDiaWid.set_editable(true);
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
//________
//private
//Transfer members to database
void HydGui_System_Member_Wid::transfer_members2database(HydGui_System_Member_Wid *dialog){
	//the table is set (the name and the column names) and allocated
	try{
		Hyd_Param_Global::set_table(this->ptr_database);
	}
	catch(Error msg){
		throw msg;
	}

	QSqlQueryModel query;

	ostringstream query_string;
	query_string <<"UPDATE ";
	query_string << Hyd_Param_Global::global_param_table->get_table_name();
	query_string << " SET " ;
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstart) << " = "<< dialog->ui.tstart->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tstep) << " = "<< dialog->ui.tstep->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::tnof) << " = "<< dialog->ui.tnof->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofits) << " = "<< dialog->ui.tnofits->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxnumsteps) << " = "<< dialog->ui.maxnumsteps->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::maxstepsize) << " = "<< dialog->ui.maxstepsize->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::inistepsize) << " = "<< dialog->ui.initstepsize->get_value()<< " , ";
	
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_fp) << " = "<< dialog->ui.max_h_fp->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_h_rv) << " = "<< dialog->ui.max_h_rv->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_maxchange_v_rv) << " = "<< dialog->ui.max_v_rv->get_value()<< " , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::syn_min_int_tstep) << " = "<< dialog->ui.min_intstep->get_value()<< " , ";

	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::pretype) << " = '"<< dialog->ui.pretype->get_current_value()<< "' , ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::gramschmidt) << " = '"<< dialog->ui.gramschmidt->get_current_value()<< "', ";
	

	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_1d) << " = '" << functions::convert_boolean2string(dialog->ui.output_1d_tecplot->get_value()) << "', ";
	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_tecplot_2d) << " = '" << functions::convert_boolean2string(dialog->ui.output_2d_tecplot->get_value()) << "', ";
	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_1d) << " = '" << functions::convert_boolean2string(dialog->ui.output_1d_paraview->get_value()) << "', ";
	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_paraview_2d) << " = '" << functions::convert_boolean2string(dialog->ui.output_2d_paraview->get_value()) << "', ";
	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_bluekenue_2d) << " = '" << functions::convert_boolean2string(dialog->ui.output_2d_bluekenue->get_value()) << "', ";
	query_string << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::output_instat_db) << " = '" << functions::convert_boolean2string(dialog->ui.output_instat_dbase->get_value()) << "' ";



	
	query_string  << " WHERE ";
	query_string  << Hyd_Param_Global::global_param_table->get_column_name(hyd_label::nofset) << " = "  << this->set_spinBox->value()-1;
	
	Data_Base::database_request(&query, query_string.str(), this->ptr_database);

	if(query.lastError().isValid()){
		Error msg=set_error(0);
		ostringstream info;
		info << "Table Name      : " << Hyd_Param_Global::global_param_table->get_table_name() << endl;
		info << "Table error info: " << query.lastError().text().toStdString() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(0);	
	}

	this->set_member(dialog);
}
//set the error
Error HydGui_System_Member_Wid::set_error(const int err_type){
	string place="HydGui_System_Member_Wid::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://problems with database query
			place.append("transfer_members2database(HydGui_System_Member_Wid *dialog)") ;
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