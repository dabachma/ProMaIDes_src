//#include "DamGui_Ecn_Func_Wid.h"
#include "Dam_Headers_Precompiled.h"

//Default constructor
DamGui_Ecn_Func_Wid::DamGui_Ecn_Func_Wid(DataRole role, QWidget *parent) : _Sys_Abstract_Base_Wid(parent) {
	// Qt Stuff
    QWidget *widget = new QWidget(parent);
    ui.setupUi(widget);
    this->set_child(widget);

	this->area2print=ui.scrollAreaWidgetContents;
	this->scroll_area=ui.scrollArea;
	this->set_headLabel_text("ECN Land use category");

	//landuse_category
	ui.landuse_category->set_label_text("Landuse category");
	ui.landuse_category->set_tooltip("Identifier of the landuse category");
	ui.landuse_category->set_range(0,9999999);

	//landuse_name
	ui.landuse_name->set_label_text("Landuse name");
	ui.landuse_name->set_tooltip("The name of the landuse category");

	//impact_type
	ui.impact_type->set_label_text("Impact type");
	ui.impact_type->set_tooltip("The keyword for the typ of the impact value of the function (x-value)");
	string impact_types[] = {"IMPACT_H", "IMPACT_V", "IMPACT_D", "IMPACT_VH"}; 
	ui.impact_type->set_items(impact_types, 4);

	//stockvalue
	ui.stockvalue->set_label_text("Default stockvalue [Monetary/m²]");
	ui.stockvalue->set_tooltip("The default stock value for the landuse category [Monetary/m²]");
	ui.stockvalue->set_dataRole(role);

	//quantile_5
	ui.quantile_5->set_label_text("5% Quantile");
	ui.quantile_5->set_tooltip("The 5% quantile of default stock value for the landuse category [Monetary/m²] [optional]");
	ui.quantile_5->set_dataRole(role);

	//quantile_95
	ui.quantile_95->set_label_text("95% Quantile");
	ui.quantile_95->set_tooltip("The 95% quantile of default stock value for the landuse category [Monetary/m²] [optional]");
	ui.quantile_95->set_dataRole(role);

	//SELF
	switch(role) {
		case 0: //DISPLAY_ROLE
			this->set_editable(false);
			this->head_pixmap->setPixmap(QPixmap(":/32x32/DamGui_Ecn_Func"));
			break;
		case 1: //EDIT_ROLE
			this->set_editable(true);
			this->headWidget->hide();
			QObject::disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu_request(QPoint)));
	}

	this->set_default_values();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(DamGui_Ecn_Func_Wid), _sys_system_modules::DAM_SYS);
}
//Default destructor
DamGui_Ecn_Func_Wid::~DamGui_Ecn_Func_Wid(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(DamGui_Ecn_Func_Wid), _sys_system_modules::DAM_SYS);
}
//______________
//public methods
//method to set the whole widget editable
void DamGui_Ecn_Func_Wid::set_editable(bool state) {
	this->editable = state;

	ui.landuse_category->set_editable(state);
	ui.landuse_name->set_editable(state);
	ui.impact_type->set_editable(state);
	ui.stockvalue->set_editable(state);
	ui.quantile_5->set_editable(state);
	ui.quantile_95->set_editable(state);
}
//Set all members of the widget, similar to a copy constructor
void DamGui_Ecn_Func_Wid::set_member(_Sys_Abstract_Base_Wid *ptr) {
	//upcast to derived object
	DamGui_Ecn_Func_Wid *other = (DamGui_Ecn_Func_Wid *) ptr;
	if (other == NULL) {
		std::cerr << "reinterpret_cast error: could not cast object\n";
		return;
	}
	ui.landuse_category->set_value(other->ui.landuse_category->get_value());
	ui.landuse_name->set_text(other->ui.landuse_name->get_text());
	ui.impact_type->set_current_value(other->ui.impact_type->get_current_index());
	ui.stockvalue->set_value(other->ui.stockvalue->get_value());
	ui.quantile_5->set_value(other->ui.quantile_5->get_value());
	ui.quantile_95->set_value(other->ui.quantile_95->get_value());
}
//Set default values
void DamGui_Ecn_Func_Wid::set_default_values(void) {
	//head widget
	this->set_head_spinBox_range(0);
	this->set_head_spinBox_text("Set Nr.:");
	//general parameters
	ui.landuse_category->set_value(0);
	ui.landuse_name->set_text("");
	ui.impact_type->set_current_value(0);
	//stockvalue parameters
	ui.stockvalue->set_value(0);
	ui.quantile_5->set_value(0);
	ui.quantile_95->set_value(0);
}
//Set the member of the widget per database
void DamGui_Ecn_Func_Wid::set_member(QSqlDatabase *ptr_database, const int func_no){
	try{	
		QSqlTableModel results(0, *ptr_database);
		this->ptr_database=ptr_database;

		if(Dam_Ecn_Damage_Function::select_relevant_functions_database(&results, func_no)==0){
			this->set_headLabel_text("Unknown Damage Function");
			this->setEnabled(false);
		}
		ui.impact_type->set_editable(true);

		Dam_Ecn_Damage_Function buffer;
		buffer.input_function_perdatabase(&results, 0);
		//general parameters
		ui.landuse_category->set_value(buffer.landuse_info.id);
		ui.landuse_name->set_text(buffer.landuse_info.name);
		ui.impact_type->set_current_value(buffer.impact_type);
		//stockvalue parameters
		ui.stockvalue->set_value(buffer.landuse_info.default_stockvalue);
		ui.quantile_5->set_value(buffer.landuse_info.def_stock_5);
		ui.quantile_95->set_value(buffer.landuse_info.def_stock_95);

		ui.impact_type->set_editable(false);

		this->set_function2plot(&buffer);

	}
	catch(Error msg){
		msg.output_msg(0);
	}
}
//____________
//public slots
//Open the dialog to edit values in the widget
void DamGui_Ecn_Func_Wid::show_as_dialog(void) {
	Sys_Base_Edit_Dia dialog(NULL, this);
	DamGui_Ecn_Func_Wid inDiaWid(DataRole::EditRole, &dialog);
	inDiaWid.set_member(this);
	dialog.add_child(&inDiaWid);
	dialog.setWindowTitle(this->head_label->text());
	QIcon icon;
	icon.addPixmap(*this->head_pixmap->pixmap());
	dialog.setWindowIcon(icon);
	dialog.exec();
}
//___________
//private
//Set the damage function points to a plot widget
void DamGui_Ecn_Func_Wid::set_function2plot(Dam_Ecn_Damage_Function *func){
	//plot
	
   ui.plot_wid->getPlotPtr()->set_number_curves(3);
   ui.plot_wid->set_groupbox_title("Damage function");
	
    ui.plot_wid->getPlotPtr()->set_axis_title("Waterlevel [m]", "Relative damage [%]");
    ui.plot_wid->getPlotPtr()->set_curve_characteristics(0, "Mid", QwtPlotCurve::Lines, Qt::green);
    ui.plot_wid->getPlotPtr()->set_curve_symbols(0, 5 ,QwtSymbol::Diamond);
    ui.plot_wid->getPlotPtr()->set_curve_characteristics(1, "5% Quantile", QwtPlotCurve::Lines, Qt::blue);
    ui.plot_wid->getPlotPtr()->set_curve_symbols(1, 5 ,QwtSymbol::Cross);
    ui.plot_wid->getPlotPtr()->set_curve_characteristics(2, "95% Quantile", QwtPlotCurve::Lines, Qt::red);
    ui.plot_wid->getPlotPtr()->set_curve_symbols(2, 5 ,QwtSymbol::Diamond);

    ui.plot_wid->getPlotPtr()->set_legend(true);


    int no_points=func->get_number_points();
    double *x_array=NULL;
    double *y_array=NULL;

    try{
        x_array=new double[no_points];
        y_array=new double[no_points];
    }
    catch(bad_alloc &t){
        Error msg=this->set_error(0);
        ostringstream info;
        info<< "Info bad alloc: " << t.what() << endl;
        msg.make_second_info(info.str());
        throw msg;
    }


    //mid
    for(int i=0; i<no_points; i++){
        x_array[i]=func->get_ptr_function_points(i)->impact_value;
        y_array[i]=func->get_ptr_function_points(i)->rel_damage;
    }
    ui.plot_wid->getPlotPtr()->set_curve_data(0,no_points, x_array, y_array);

    //5% quantile
    for(int i=0; i<no_points; i++){
        y_array[i]=func->get_ptr_function_points(i)->quant_5_rel_damage;
    }
    ui.plot_wid->getPlotPtr()->set_curve_data(1,no_points, x_array, y_array);
    //95% quantile
    for(int i=0; i<no_points; i++){
        y_array[i]=func->get_ptr_function_points(i)->quant_95_rel_damage;
    }
    ui.plot_wid->getPlotPtr()->set_curve_data(2,no_points, x_array, y_array);

    if(x_array!=NULL){
        delete []x_array;
        x_array=NULL;
    }
    if(y_array!=NULL){
        delete []y_array;
        y_array=NULL;
    }

    ui.plot_wid->getPlotPtr()->show_plot();

}
//Set error(s)
Error DamGui_Ecn_Func_Wid::set_error(const int err_type){
		string place="DamGui_Ecn_Func_Wid::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("set_function2plot(Dam_Ecn_Damage_Function *func)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
