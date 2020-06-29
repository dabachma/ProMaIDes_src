#include "source_code\Sys_Headers_Precompiled.h"
//#include "Sys_PlotSettings_Dia.h"


//Default constructor
Sys_PlotSettings_Dia::Sys_PlotSettings_Dia(QWidget *parent) : QDialog(parent){
	

	this->ui.setupUi(this);
	//slot connection
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	//main slot connection
	//font
	QObject::connect(ui.pushButton_font_title, SIGNAL(clicked()), this, SLOT(start_font_dia_title()));
	QObject::connect(ui.pushButton_font_xaxis, SIGNAL(clicked()), this, SLOT(start_font_dia_x_axis()));
	QObject::connect(ui.pushButton_font_yaxis, SIGNAL(clicked()), this, SLOT(start_font_dia_y_axis()));
	//color
	QObject::connect(ui.pushButton_color_title, SIGNAL(clicked()), this, SLOT(start_color_dia_title()));
	QObject::connect(ui.pushButton_color_xaxis, SIGNAL(clicked()), this, SLOT(start_color_dia_x_axis()));
	QObject::connect(ui.pushButton_color_yaxis, SIGNAL(clicked()), this, SLOT(start_color_dia_y_axis()));

    this->setWindowTitle(tr("Plot Settings"));

	this->number_add_tab=0;

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}

//Default destructor
Sys_PlotSettings_Dia::~Sys_PlotSettings_Dia(void){

	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}
//___________
//public
//Start the dialog
bool Sys_PlotSettings_Dia::start_dialog(void){

	int decision = this->exec();
	//rejected
	if(decision == 0){
		return false;
	}
	//accepted; 
	else{
		return true;
	}
}
//Set the title, x-axis name and y-axis name
void Sys_PlotSettings_Dia::set_title_x_y_axis(const string title, const string x_axis, const string y_axis){
	this->ui.lineEdit_title->setText(title.c_str());
	this->ui.lineEdit_xaxis->setText(x_axis.c_str());
	this->ui.lineEdit_yaxis->setText(y_axis.c_str());

}
//Get the title
string Sys_PlotSettings_Dia::get_title(void){
	return this->ui.lineEdit_title->text().toStdString();
}
//Get the x-axis name
string Sys_PlotSettings_Dia::get_x_axis_name(void){
	return this->ui.lineEdit_xaxis->text().toStdString();
}
//Get the y-axis name
string Sys_PlotSettings_Dia::get_y_axis_name(void){
	return this->ui.lineEdit_yaxis->text().toStdString();
}
//Set the font of the title, x-axis name and y-axis name
void Sys_PlotSettings_Dia::set_font_title_x_y_axis(const QFont title, const QFont x_axis, const QFont y_axis){
	this->f_title=title;
	this->f_x_axis=x_axis;
	this->f_y_axis=y_axis;
}
//Get the font of the title
QFont Sys_PlotSettings_Dia::get_font_title(void){
	return this->f_title;
}
//Get the font of the x-axis name
QFont Sys_PlotSettings_Dia::get_font_x_axis_name(void){
	return this->f_x_axis;
}
//Get the font of the y-axis name
QFont Sys_PlotSettings_Dia::get_font_y_axis_name(void){
	return this->f_y_axis;
}
//Set the color of the title, x-axis name and y-axis name
void Sys_PlotSettings_Dia::set_color_title_x_y_axis(const QColor title, const QColor x_axis, const QColor y_axis){
	this->c_title=title;
	this->c_x_axis=x_axis;
	this->c_y_axis=y_axis;
}
//Get the color of the title
QColor Sys_PlotSettings_Dia::get_color_title(void){
	return this->c_title;
}
//Get the color of the x-axis name
QColor Sys_PlotSettings_Dia::get_color_x_axis_name(void){
	return this->c_x_axis;
}
//Get the color of the y-axis name
QColor Sys_PlotSettings_Dia::get_color_y_axis_name(void){
	return this->c_y_axis;
}

//Get number additional tab-widgets (number of curves)
int Sys_PlotSettings_Dia::get_number_additional_tab(void){
	return this->number_add_tab;
}
//Set curve setting to tab widgets
void Sys_PlotSettings_Dia::set_curve_setting2tabwidget(const string title){
	
		Sys_CurveSettings_Wid *tab=NULL;
		tab=new Sys_CurveSettings_Wid;
		this->ui.tabWidget->addTab(tab,title.c_str());

}
//____________
//private slots
//Start font dialog of the title
void Sys_PlotSettings_Dia::start_font_dia_title(void){
	QFontDialog dia(this);

	dia.setCurrentFont(this->f_title);

	int decision = dia.exec();

	if(decision == 1){
		this->f_title=dia.currentFont();
	}


}
//Start font dialog of the x-axis
void Sys_PlotSettings_Dia::start_font_dia_x_axis(void){
	QFontDialog dia(this);

	dia.setCurrentFont(this->f_x_axis);

	int decision = dia.exec();

	if(decision == 1){
		this->f_x_axis=dia.currentFont();
	}
}
//Start font dialog of the y-axis
void Sys_PlotSettings_Dia::start_font_dia_y_axis(void){
	QFontDialog dia(this);

	dia.setCurrentFont(this->f_y_axis);

	int decision = dia.exec();

	if(decision == 1){
		this->f_y_axis=dia.currentFont();
	}
}
//Start color dialog of the title
void Sys_PlotSettings_Dia::start_color_dia_title(void){
	QColorDialog dia(this);

	dia.setCurrentColor(this->c_title);

	int decision = dia.exec();

	if(decision == 1){
		this->c_title=dia.currentColor();
	}
}
//Start color dialog of the x-axis
void Sys_PlotSettings_Dia::start_color_dia_x_axis(void){
	QColorDialog dia(this);

	dia.setCurrentColor(this->c_x_axis);

	int decision = dia.exec();

	if(decision == 1){
		this->c_x_axis=dia.currentColor();
	}
}
//Start color dialog of the y-axis
void Sys_PlotSettings_Dia::start_color_dia_y_axis(void){
	QColorDialog dia(this);

	dia.setCurrentColor(this->c_y_axis);

	int decision = dia.exec();

	if(decision == 1){
		this->c_y_axis=dia.currentColor();
	}
}