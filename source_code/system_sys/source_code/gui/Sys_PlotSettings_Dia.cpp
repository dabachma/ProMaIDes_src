#include "Sys_Headers_Precompiled.h"
//#include "Sys_PlotSettings_Dia.h"


//Default constructor
Sys_PlotSettings_Dia::Sys_PlotSettings_Dia(QWidget *parent) : QDialog(parent){	
	this->ui.setupUi(this);

	this->number_curves=0;
	this->curves=NULL;

    this->setWindowTitle(tr("Plot Settings"));

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_PlotSettings_Dia::~Sys_PlotSettings_Dia(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_PlotSettings_Dia),_sys_system_modules::SYS_SYS);
}
//___________
//public
//Start the dialog
bool Sys_PlotSettings_Dia::start_dialog(void) {
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
//Get the plot title
const QwtText Sys_PlotSettings_Dia::getPlotTitle(void) 
{
	QwtText text(this->ui.plotTitleLineEdit->text());
	return text;
}
//Get the title of the horizontal axis
const QwtText Sys_PlotSettings_Dia::getXAxisTitle(void)
{
	QwtText text(this->ui.horizontalAxisLineEdit->text());
	return text;
}
//Get the title of the vertical axis
const QwtText Sys_PlotSettings_Dia::getYAxisTitle(void)
{
	QwtText text(this->ui.verticalAxisLineEdit->text());
	return text;
}
//Check whether x axis gridlines are enabled or not
bool Sys_PlotSettings_Dia::isVerticalGridlinesEnabled(void)
{
	return this->ui.verticalAxisGridCheckBox->isChecked();
}
//Check whether y axis gridlines are enabled or not
bool Sys_PlotSettings_Dia::isHorizontalGridlinesEnabled(void)
{
	return this->ui.horizontalAxisGridCheckBox->isChecked();
}
//Get the legend position
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
//Get the plot title font
const QFont Sys_PlotSettings_Dia::getPlotTitleFont(void)
{
	return this->plotTitleFont;
}
//Get the x-axis title font
const QFont Sys_PlotSettings_Dia::getHorizontalAxisTitleFont(void)
{
	return this->horizontalAxisTitleFont;
}
//Get the y-axis title font
const QFont Sys_PlotSettings_Dia::getVerticalAxisTitleFont(void)
{
	return this->verticalAxisTitleFont;
}
//Get the plot title color
const QColor Sys_PlotSettings_Dia::getPlotTitleColor(void)
{
	return this->plotTitleColor;
}
//Get the x-axis title color
const QColor Sys_PlotSettings_Dia::getHorizontalAxisTitleColor(void)
{
	return this->horizontalAxisTitleColor;
}
//Get the y-axis title color
const QColor Sys_PlotSettings_Dia::getVerticalAxisTitleColor(void)
{
	return this->verticalAxisTitleColor;
}
//Set number of curves to handle and a pointer to the curve
void Sys_PlotSettings_Dia::set_curves(const int number, QwtPlotCurve *curve){
	this->number_curves=number;
	this->curves=curve;


	Sys_CurveSettings_Wid *curveSettings;
	for(int i=0; i<this->number_curves; i++){
		curveSettings = new Sys_CurveSettings_Wid();
	
		this->ui.tabWidget->addTab(curveSettings, this->curves[i].title().text());

		//set charcteristics of the curve
		curveSettings->ui.curveTitleLineEdit->setText(this->curves[i].title().text());
		curveSettings->ui.curveStyleComboBox->setCurrentIndex(this->curves[i].pen().style());
		int width=this->curves[i].pen().width();
		curveSettings->ui.curveWidthSpinBox->setValue(width+1);
		QColor lastCurveColor = this->curves[i].pen().color();
		curveSettings->set_curve_col(lastCurveColor);
		//set charcteristics of the symbols
        if(this->curves[i].symbol()!=NULL){
            QColor lastSymbolColor = this->curves[i].symbol()->pen().color();
            curveSettings->ui.symbolSizeSpinBox->setValue(this->curves[i].symbol()->size().width());
            curveSettings->ui.symbolStyleComboBox->setCurrentIndex(this->curves[i].symbol()->style() + 1);
            curveSettings->set_sym_col(lastSymbolColor);
        }
		
		bool visi=this->curves[i].isVisible();
		curveSettings->setEnabled(visi);


	}



}
//____________
//private


//____________
//private slots
//Button of title font is clicked
void Sys_PlotSettings_Dia::on_plotTitleFontPushButton_clicked(void){
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->plotTitleFont);
	if (fontDialog.exec()) {
		this->plotTitleFont = fontDialog.currentFont();
	} 
}
//Button of title color is clicked
void Sys_PlotSettings_Dia::on_plotTitleColorPushButton_clicked(void){
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->plotTitleColor);
	if (colorDialog.exec()) {
		this->plotTitleColor = colorDialog.currentColor();
	}
}
//Button of horizontal axis font is clicked
void Sys_PlotSettings_Dia::on_horizontalAxisFontPushButton_clicked(void){
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->horizontalAxisTitleFont);
	if (fontDialog.exec()) {
		this->horizontalAxisTitleFont = fontDialog.currentFont();
	}
}
//Button of horizontal axis color is clicked
void Sys_PlotSettings_Dia::on_horizontalAxisColorPushButton_clicked(void){
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->horizontalAxisTitleColor);
	if (colorDialog.exec()) {
		this->horizontalAxisTitleColor = colorDialog.currentColor();
	}
}
//Button of vertical axis font is clicked
void Sys_PlotSettings_Dia::on_verticalAxisFontPushButton_clicked(void){
	QFontDialog fontDialog(this);
	fontDialog.setCurrentFont(this->verticalAxisTitleFont);
	if (fontDialog.exec()) {
		this->verticalAxisTitleFont = fontDialog.currentFont();
	} 
}
//Button of vertical axis color is clicked
void Sys_PlotSettings_Dia::on_verticalAxisColorPushButton_clicked(void){
	QColorDialog colorDialog(this);
	colorDialog.setCurrentColor(this->verticalAxisTitleColor);
	if (colorDialog.exec()) {
		this->verticalAxisTitleColor = colorDialog.currentColor();
	}
}
