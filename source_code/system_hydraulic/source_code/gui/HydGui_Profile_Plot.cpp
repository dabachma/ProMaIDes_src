//#include "HydGui_Profile_Plot.h"
#include "Hyd_Headers_Precompiled.h"

//constructur
HydGui_Profile_Plot::HydGui_Profile_Plot(QWidget *parent){
	this->number_curves=0;
	this->curves=NULL;
	this->symbols=NULL;

	this->legend=new QwtLegend(this);
	this->insertLegend(this->legend);
	this->x_values=NULL;
	this->y_values=NULL;
	//set the parent Widget (window)
	this->setParent(parent);
}
//destructor
HydGui_Profile_Plot::~HydGui_Profile_Plot(void){
	if(this->legend!=NULL){
		delete this->legend;
		this->legend=NULL;
	}
	if(this->curves!=NULL){
		delete []this->curves;
		this->curves=NULL;
	}
	if(this->symbols!=NULL){
		delete []this->symbols;
		this->symbols=NULL;
	}
	if(this->x_values!=NULL){
		delete []this->x_values;
		this->x_values=NULL;
	}
	if(this->y_values!=NULL){
		delete []this->y_values;
		this->y_values=NULL;
	}

}

//__________
//public
//set titles (plot, x-axis, y-axis
void HydGui_Profile_Plot::set_titles(const string plot, const string x_axis, const string y_axis){
	this->setTitle(plot.c_str());
	this->setAxisTitle(xBottom, x_axis.c_str());
	this->setAxisTitle(yLeft, y_axis.c_str());
}
	
//set the number of curves, which should be plotted
void HydGui_Profile_Plot::set_number_curves(const int number_of_curves){
	this->number_curves=number_of_curves;
	this->allocate_curves();
}

//set the curves
void HydGui_Profile_Plot::set_curves(const int curve_number, const string title, const int number_segments, QColor my_color, Hyd_Profile_Segment *segment2show){
	//allocate the datapoints and set the curve
	this->allocate_data_points(number_segments+1);
	this->curves[curve_number].setStyle(QwtPlotCurve::CurveStyle::Lines);
	this->curves[curve_number].setTitle(title.c_str());
	this->curves[curve_number].setPen(my_color);
	//curve is given to plot
	this->curves[curve_number].attach(this);
	//set the data points
	for(int i=0; i<number_segments; i++){
		this->x_values[i]=segment2show[i].point1.get_xcoordinate();
		this->y_values[i]=segment2show[i].point1.get_ycoordinate();
	}
	//last point is set
	this->x_values[number_segments]=segment2show[number_segments-1].point2.get_xcoordinate();
	this->y_values[number_segments]=segment2show[number_segments-1].point2.get_ycoordinate();
	//point are given to curve
    this->curves[curve_number].setSamples(this->x_values, this->y_values, number_segments+1);
	
}
///set symbols
void HydGui_Profile_Plot::set_symbols(const int curve_number, QwtSymbol::Style my_style){

	
	//set the symbols
	this->symbols[curve_number].setStyle(my_style);
	//take the color of the curve
	//this->symbols[curve_number].setPen(this->curves[curve_number].pen());
	this->symbols[curve_number].setPen(QColor(Qt::black));
	this->symbols[curve_number].setSize(7);
	//symbols to cruve
    this->curves[curve_number].setSymbol(&this->symbols[curve_number]);

}
//show the Plot
void HydGui_Profile_Plot::show_plot(void){
	this->resize(600,400);
	this->show();
	if(this->parentWidget()==NULL){
		QApplication::exec();
	}
}
//_________________
//private
//allocate curves
void HydGui_Profile_Plot::allocate_curves(void){

    this->detachItems();
	if (this->curves!=NULL){
		delete [] this->curves;
	}
	if (this->symbols!=NULL){
		delete [] this->symbols;
	}
	this->curves = new QwtPlotCurve [this->number_curves];
	this->symbols = new QwtSymbol [this->number_curves];

}
//allocate values
void HydGui_Profile_Plot::allocate_data_points(const int number_of_points){
	if (this->x_values!=NULL){
		delete [] this->x_values;
		this->x_values=NULL;
	}
	if (this->y_values!=NULL){
		delete [] this->y_values;
		this->y_values=NULL;
	}
	this->x_values = new double [number_of_points];
	this->y_values = new double [number_of_points];
	
}
