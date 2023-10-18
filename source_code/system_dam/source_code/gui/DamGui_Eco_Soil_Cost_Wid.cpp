//#include "DamGui_Eco_Soil_Cost_Wid.h"
#include "Dam_Headers_Precompiled.h"

//Default constructor
DamGui_Eco_Soil_Cost_Wid::DamGui_Eco_Soil_Cost_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;
	this->set_headLabel_text("Soil Cost Function");

	//gui
	ui.id->set_label_text("Id");
	ui.id->set_tooltip("Id of the soil type");
	ui.id->set_range(0,999999);
	ui.name->set_label_text("Name");
	ui.name->set_tooltip("Name of the soil type");
	ui.stepwise_flag->set_label_text("Stepwise flag");
	ui.stepwise_flag->set_tooltip("Stepwise or continuous function");

	//SELF
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
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Eco_Soil_Cost_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Eco_Soil_Cost_Wid::~DamGui_Eco_Soil_Cost_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Eco_Soil_Cost_Wid), _sys_system_modules::DAM_SYS);
}
//______________
//public methods
//method to set the whole widget editable
void DamGui_Eco_Soil_Cost_Wid::set_editable(bool state) {
	ui.id->set_editable(state);
	ui.name->set_editable(state);
	ui.stepwise_flag->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void DamGui_Eco_Soil_Cost_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	DamGui_Eco_Soil_Cost_Wid *other = (DamGui_Eco_Soil_Cost_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	ui.id->set_value(other->ui.id->get_value());
	ui.name->set_text(other->ui.name->get_text());
	ui.stepwise_flag->set_value(other->ui.stepwise_flag->get_value());
}
//Set default values
void DamGui_Eco_Soil_Cost_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general
	ui.id->set_value(0);
	ui.name->set_text("-");
	ui.stepwise_flag->set_value(false);
}
//Set the member of the widget per database
void DamGui_Eco_Soil_Cost_Wid::set_member(QSqlDatabase *ptr_database, const int func_no){
	try{	
		QSqlTableModel results(0, *ptr_database);
		this->ptr_database=ptr_database;

		if(Dam_Eco_Soil_Cost_Function::select_relevant_functions_database(&results, func_no)==0){
			this->set_headLabel_text("Unknown Soil-Cost Function");
			this->setEnabled(false);
		}

		Dam_Eco_Soil_Cost_Function buffer;
		buffer.input_function_perdatabase(&results, 0);
		//general parameters
		ui.id->set_value(buffer.index);
		ui.name->set_text(buffer.name);
		ui.stepwise_flag->set_value(buffer.stepwise_flag);


		//plot
		this->ui.widget_plot->getPlotPtr()->set_number_curves(1);
		this->ui.widget_plot->getPlotPtr()->set_axis_title("Soil erosion factor [-]", "Cost [monetary/m²]");
		this->ui.widget_plot->getPlotPtr()->set_curve_characteristics(0, "Impact", QwtPlotCurve::Lines, Qt::black);

		buffer.set_members2plot_wid(this->ui.widget_plot->getPlotPtr());

		this->ui.widget_plot->getPlotPtr()->show_plot();

		//set icon
		QPixmap icon;
		icon.load(":eco_soil_icon" );
		this->set_headPixmap(icon);
	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void DamGui_Eco_Soil_Cost_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	DamGui_Eco_Soil_Cost_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}