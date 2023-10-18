//#include "DamGui_Pys_Categories_Wid.h"
#include "Dam_Headers_Precompiled.h"

//Default constructor
DamGui_Pys_Categories_Wid::DamGui_Pys_Categories_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
	QWidget *widget = new QWidget(parent);
	ui.setupUi(widget);
	this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;
	this->set_headLabel_text("Psychosocial Category");

	//gui
	ui.id->set_label_text("Id");
	ui.id->set_tooltip("Id of the psychosocial category");
	ui.id->set_range(0,999999);
	ui.name->set_label_text("Name");
	ui.name->set_tooltip("Name of the psychosocial category");

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
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Pys_Categories_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Pys_Categories_Wid::~DamGui_Pys_Categories_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Pys_Categories_Wid), _sys_system_modules::DAM_SYS);
}
//______________
//public methods
//method to set the whole widget editable
void DamGui_Pys_Categories_Wid::set_editable(const bool state) {
	ui.id->set_editable(state);
	ui.name->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void DamGui_Pys_Categories_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	DamGui_Pys_Categories_Wid *other = (DamGui_Pys_Categories_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	ui.id->set_value(other->ui.id->get_value());
	ui.name->set_text(other->ui.name->get_text());
}

//Set default values
void DamGui_Pys_Categories_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general
	ui.id->set_value(0);
	ui.name->set_text("-");
}
//Set the member of the widget per database
void DamGui_Pys_Categories_Wid::set_member(QSqlDatabase *ptr_database, const int cat_no){
	try{	
		QSqlTableModel results(0, *ptr_database);
		this->ptr_database=ptr_database;

		if(Dam_Pys_Category::select_relevant_category_database(&results, cat_no)==0){
			this->set_headLabel_text("Unknown psychosocial category");
			this->setEnabled(false);
		}

		Dam_Pys_Category buffer;
		buffer.input_category_perdatabase(&results, 0);
		//general parameters
		ui.id->set_value(buffer.index);
		ui.name->set_text(buffer.name);


		//plot
		buffer.set_score_function2plot_widget(this->ui.widget_plot_50_59->getPlotPtr(), _dam_pys_criteria::crit_age_50_59);
		buffer.set_score_function2plot_widget(this->ui.widget_plot_80->getPlotPtr(), _dam_pys_criteria::crit_age_80);
		buffer.set_score_function2plot_widget(this->ui.widget_plot_female->getPlotPtr(), _dam_pys_criteria::crit_female);
		buffer.set_score_function2plot_widget(this->ui.widget_plot_owner->getPlotPtr(), _dam_pys_criteria::crit_owner);


		//set density function
		if(Dam_Pys_Density_Function::select_relevant_function_points_database(&results, false)==0){
			ui.groupBox_6->setEnabled(false);
		}
		else{
			Dam_Pys_Density_Function buff_dens;
			buff_dens.input_function_perdatabase(this->ptr_database);

			this->ui.widget_plot_dens->getPlotPtr()->set_number_curves(1);
			this->ui.widget_plot_dens->getPlotPtr()->set_axis_title("Population density [P/m²]", "Multiplier [-]");
			this->ui.widget_plot_dens->getPlotPtr()->set_curve_characteristics(0, "Impact", QwtPlotCurve::Lines, Qt::black);

			buff_dens.set_members2plot_wid(this->ui.widget_plot_dens->getPlotPtr());
			this->ui.widget_plot_dens->getPlotPtr()->show_plot();

		}

		//set icon
		QPixmap icon;
		icon.load(":pys_icon" );
		this->set_headPixmap(icon);
	}
	catch(Error msg){
		msg.output_msg(0);
	}

}
//____________
//public slots
//Open the dialog to edit values in the widget
void DamGui_Pys_Categories_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	DamGui_Pys_Categories_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}
