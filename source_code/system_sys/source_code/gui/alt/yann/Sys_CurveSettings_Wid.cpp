#include "source_code\Sys_Headers_Precompiled.h"
//#include "Sys_CurveSettings_Wid.h"


//Default constructor
Sys_CurveSettings_Wid::Sys_CurveSettings_Wid(QWidget *parent)
{
	this->ui.setupUi(this);
	this->setParent(parent);

	this->curveStyleList << "NoCurve" << "Lines" << "Sticks" << "Steps" << "Dots";
	this->ui.curveStyleComboBox->addItems(curveStyleList);
	this->symbolStyleList << "NoSymbol" << "Ellipse" << "Rect" << "Diamond" << "Triangle" << "DTriangle" << "UTriangle" <<"LTriangle" << "RTriangle" 
		<< "Cross" << "XCross" << "HLine" << "VLine" << "Star1" << "Star2" << "Hexagon";
	this->ui.symbolStyleComboBox->addItems(symbolStyleList);

	Sys_Memory_Count::self()->add_mem(sizeof(Sys_CurveSettings_Wid),_sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_CurveSettings_Wid::~Sys_CurveSettings_Wid(void) 
{	
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_CurveSettings_Wid),_sys_system_modules::SYS_SYS);
}

//_________
//public

//Get the title of the curve
QwtText Sys_CurveSettings_Wid::getCurveTitle(void)
{
	QwtText curveTitle(this->ui.curveTitleLineEdit->text());
	return curveTitle;
}
//Get the style of the curve. Possible curve styles are Lines, Sticks, Steps and Dots. Default is set to Lines.
QwtPlotCurve::CurveStyle Sys_CurveSettings_Wid::getCurveStyle(void)
{
	QString curveStyleType(this->ui.curveStyleComboBox->currentText());
	if (curveStyleType == "NoCurve") {
		return QwtPlotCurve::NoCurve;
	} else if (curveStyleType == "Lines") {
		return QwtPlotCurve::Lines;
	} else if (curveStyleType == "Sticks") {
		return QwtPlotCurve::Sticks;
	} else if (curveStyleType == "Steps") {
		return QwtPlotCurve::Steps;
	} else if (curveStyleType == "Dots") {
		return QwtPlotCurve::Dots;
	} else {
		return QwtPlotCurve::Lines;
	}
}
//Get the line width of the curve
int Sys_CurveSettings_Wid::getCurveWidth(void)
{
	return this->ui.curveWidthSpinBox->value();
}
//Get the style of the symbol. Possible symbol styles are Ellipse, Rect, Diamond, Triangle, Cross, XCross, HLine, VLine, Star1, Star2 and Hexagon. Default is set to Rect.
QwtSymbol::Style Sys_CurveSettings_Wid::getSymbolStyle(void)
{
	QString curveSymbolType(this->ui.symbolStyleComboBox->currentText());
	if (curveSymbolType == "NoSymbol") {
		return QwtSymbol::NoSymbol;
	} else if (curveSymbolType == "Ellipse") {
		return QwtSymbol::Ellipse;
	} else if (curveSymbolType == "Rect") {
		return QwtSymbol::Rect;
	} else if (curveSymbolType == "Diamond") {
		return QwtSymbol::Diamond;
	} else if (curveSymbolType == "Triangle") {
		return QwtSymbol::Triangle;
	} else if (curveSymbolType == "DTriangle") {
		return QwtSymbol::DTriangle;
	} else if (curveSymbolType == "Uriangle") {
		return QwtSymbol::UTriangle;
	} else if (curveSymbolType == "Lriangle") {
		return QwtSymbol::LTriangle;
	}  else if (curveSymbolType == "RTriangle") {
		return QwtSymbol::RTriangle;
	} else if (curveSymbolType == "Cross") {
		return QwtSymbol::Cross;
	} else if (curveSymbolType == "XCross") {
		return QwtSymbol::XCross;
	} else if (curveSymbolType == "HLine") {
		return QwtSymbol::HLine;
	} else if (curveSymbolType == "VLine") {
		return QwtSymbol::VLine;
	} else if (curveSymbolType == "Star1") {
		return QwtSymbol::Star1;
	} else if (curveSymbolType == "Star2") {
		return QwtSymbol::Star2;
	} else if (curveSymbolType == "Hexagon") {
		return QwtSymbol::Hexagon;
	} else {
		return QwtSymbol::Rect;
	}
}
//Get the symbol size
int Sys_CurveSettings_Wid::getSymbolSize(void)
{
	return this->ui.symbolSizeSpinBox->value();
}
//Get the curve color
QColor Sys_CurveSettings_Wid::getCurveColor(void)
{
	return this->curveColor;
}
//Get the symbol color
QColor Sys_CurveSettings_Wid::getSymbolColor(void)
{
	return this->symbolColor;
}

//____________
//private slots

void Sys_CurveSettings_Wid::on_curveColorPushButton_clicked()
{
	QColor color = QColorDialog::getColor(Qt::black, this, "Curve Color",  QColorDialog::DontUseNativeDialog);
	if(color.isValid()) 
	{
		this->curveColor = color;
	}
}
	
void Sys_CurveSettings_Wid::on_symbolColorPushButton_clicked()
{
	QColor color = QColorDialog::getColor(Qt::black, this, "Symbol Color",  QColorDialog::DontUseNativeDialog);
	if(color.isValid()) 
	{
		this->symbolColor = color;
	}
}