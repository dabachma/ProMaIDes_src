//#include "source_code\Sys_Headers_Precompiled.h"
#include "Sys_PlotSettings_Dia.h"


//Default constructor
Sys_PlotSettings_Dia::Sys_PlotSettings_Dia(QWidget *parent) : QDialog(parent) 
{	
	this->ui.setupUi(this);
	//slot connection

	//main slot connection

    this->setWindowTitle(tr("Plot Settings"));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}

//Default destructor
Sys_PlotSettings_Dia::~Sys_PlotSettings_Dia(void) 
{
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}

//___________
//public

//Start the dialog
bool Sys_PlotSettings_Dia::start_dialog(void) 
{
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

const QwtText Sys_PlotSettings_Dia::getPlotTitle(void) 
{
	QwtText text(this->ui.plotTitleLineEdit->text());
	return text;
}

const QwtText Sys_PlotSettings_Dia::getXAxisTitle(void)
{
	QwtText text(this->ui.horizontalAxisLineEdit->text());
	return text;
}

const QwtText Sys_PlotSettings_Dia::getYAxisTitle(void)
{
	QwtText text(this->ui.verticalAxisLineEdit->text());
	return text;
}

bool Sys_PlotSettings_Dia::isVerticalGridlinesEnabled(void)
{
	return this->ui.verticalAxisGridCheckBox->isChecked();
}

bool Sys_PlotSettings_Dia::isHorizontalGridlinesEnabled(void)
{
	return this->ui.horizontalAxisGridCheckBox->isChecked();
}

QwtPlot::LegendPosition Sys_PlotSettings_Dia::getLegendPosition(void)
{
	if (this->ui.leftLegendRadioButton->isChecked()) {
		return QwtPlot::LeftLegend;
	} else if (this->ui.rightLegendRadioButton->isChecked()) {
		return QwtPlot::RightLegend;
	} else if (this->ui.bottomLegendRadioButton->isChecked()) {
		return QwtPlot::BottomLegend;
	} else if (this->ui.topLegendRadioButton->isChecked()) {
		return QwtPlot::TopLegend;
	} else {
		return QwtPlot::RightLegend;
	}
}

const QFont Sys_PlotSettings_Dia::getPlotTitleFont(void)
{
	return this->plotTitleFont;
}

const QFont Sys_PlotSettings_Dia::getHorizontalAxisTitleFont(void)
{
	return this->horizontalAxisTitleFont;
}

const QFont Sys_PlotSettings_Dia::getVerticalAxisTitleFont(void)
{
	return this->verticalAxisTitleFont;
}

const QColor Sys_PlotSettings_Dia::getPlotTitleColor(void)
{
	return this->plotTitleColor;
}

const QColor Sys_PlotSettings_Dia::getHorizontalAxisTitleColor(void)
{
	return this->horizontalAxisTitleColor;
}

const QColor Sys_PlotSettings_Dia::getVerticalAxisTitleColor(void)
{
	return this->verticalAxisTitleColor;
}

//____________
//private


//____________
//private slots
void Sys_PlotSettings_Dia::on_plotTitleFontPushButton_clicked()
{
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->plotTitleFont);
	if (fontDialog.exec()) {
		this->plotTitleFont = fontDialog.currentFont();
	} 
}

void Sys_PlotSettings_Dia::on_plotTitleColorPushButton_clicked()
{
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->plotTitleColor);
	if (colorDialog.exec()) {
		this->plotTitleColor = colorDialog.currentColor();
	}
}

void Sys_PlotSettings_Dia::on_horizontalAxisFontPushButton_clicked()
{
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->horizontalAxisTitleFont);
	if (fontDialog.exec()) {
		this->horizontalAxisTitleFont = fontDialog.currentFont();
	}
}

void Sys_PlotSettings_Dia::on_horizontalAxisColorPushButton_clicked()
{
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->horizontalAxisTitleColor);
	if (colorDialog.exec()) {
		this->horizontalAxisTitleColor = colorDialog.currentColor();
	}
}

void Sys_PlotSettings_Dia::on_verticalAxisFontPushButton_clicked()
{
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->verticalAxisTitleFont);
	if (fontDialog.exec()) {
		this->verticalAxisTitleFont = fontDialog.currentFont();
	} 
}

void Sys_PlotSettings_Dia::on_verticalAxisColorPushButton_clicked()
{
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->verticalAxisTitleColor);
	if (colorDialog.exec()) {
		this->verticalAxisTitleColor = colorDialog.currentColor();
	}
}
