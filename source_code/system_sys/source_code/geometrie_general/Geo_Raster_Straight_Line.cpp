#include "Sys_Headers_Precompiled.h"
//#include "Geo_Raster_Straight_Line.h"

//Default constructor
Geo_Raster_Straight_Line::Geo_Raster_Straight_Line(void){
	this->gradient=0.0;
	this->y_interception=0.0;
	this->vertical_flag=false;
	
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Raster_Straight_Line), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Constructor, where the two line points are given
Geo_Raster_Straight_Line::Geo_Raster_Straight_Line(Geo_Point *point1, Geo_Point *point2){
	this->point1=point1;
	this->point2=point2;
	this->vertical_flag=false;
	//set the gradient
	this->calc_gradient();
	//set the y_interception
	this->calc_y_interception();
	//set the name of the first point
	this->name=*(this->point1->get_ptr_point_name());

	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Raster_Straight_Line), _sys_system_modules::GEOSYS_SYS);//count the memory
}

//Default destructor
Geo_Raster_Straight_Line::~Geo_Raster_Straight_Line(void){

	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Raster_Straight_Line), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//_______________________
//public
//Set the points of a line
void Geo_Raster_Straight_Line::set_points(Geo_Point *point1, Geo_Point *point2){
	this->point1=point1;
	this->point2=point2;
	//set the gradient
	this->calc_gradient();
	//set the y_interception
	this->calc_y_interception();
	//set the name of the first point
	this->name=*(this->point1->get_ptr_point_name());
}
//Get the gradient
double Geo_Raster_Straight_Line::get_gradient(void){
	return this->gradient;
}
//Get the y-axis interception value
double Geo_Raster_Straight_Line::get_y_interception(void){
	return this->y_interception;
}
//Get the name of the line 
string Geo_Raster_Straight_Line::get_name(void){
	return this->name;
}
//Get the pointer to name of the line 
string* Geo_Raster_Straight_Line::get_ptr_name(void){
	return &this->name;
}
//Get flag if the line is vertical
bool Geo_Raster_Straight_Line::get_vertical_flag(void){
	return this->vertical_flag;
}
//Check if the given point is on the line
bool Geo_Raster_Straight_Line::calc_point_on_line(Geo_Point *check_point){
	bool online_flag=false;
	double x_koor=check_point->get_xcoordinate();
	double y_koor=check_point->get_ycoordinate();

	if(this->vertical_flag==false){
		double result=this->gradient*x_koor+this->y_interception;
		if(abs(result-y_koor)<constant::meter_epsilon){
			online_flag=true;
		}
	}
	else{
		if(abs(x_koor-this->point1->get_xcoordinate())< constant::meter_epsilon && abs(x_koor-this->point2->get_xcoordinate())<constant::meter_epsilon){
			online_flag=true;
		}
	}
	return online_flag;
}
//Calculate and retrieve an interception point with the given line
void Geo_Raster_Straight_Line::calc_interception(Geo_Raster_Straight_Line *interception_line, _geo_interception_point *my_interception){
	my_interception->name_1=this->name;
	my_interception->name_2=interception_line->name;
	my_interception->indefinite_flag=false;
	my_interception->interception_flag=false;

	//exception: check vertical line
	if(this->vertical_flag==true && interception_line->vertical_flag==false){
		double x_value=this->point1->get_xcoordinate();
		double y_value=interception_line->gradient*(x_value)+interception_line->y_interception;
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point(x_value, y_value, label::interception_point);
		return;
	}
	else if(this->vertical_flag==false && interception_line->vertical_flag==true){
		double x_value=interception_line->point1->get_xcoordinate();
		double y_value=this->gradient*(x_value)+this->y_interception;
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point(x_value, y_value, label::interception_point);
		return;
	}
	else if(this->vertical_flag==true && interception_line->vertical_flag==true){
		my_interception->interception_flag=false;
		if(this->point1->get_xcoordinate()==interception_line->point1->get_xcoordinate()){
			my_interception->indefinite_flag=true;	
		}
		else{
			my_interception->indefinite_flag=true;
		}
		return;
	}

	//calculate the interception point
	double delta_m=this->get_gradient()-interception_line->get_gradient();
	double delta_b=interception_line->get_y_interception()-this->get_y_interception();
	if(delta_m==0.0){//no one point interception
		if(delta_b==0.0){//indefinite interception points
			my_interception->indefinite_flag=true;
			my_interception->interception_flag=false;
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
	}
	else{
		my_interception->indefinite_flag=false;
		my_interception->interception_flag=true;
		double x_value=delta_b/delta_m;
		double y_value=this->get_gradient()*x_value+this->get_y_interception();
		my_interception->interception_point.set_point(x_value, y_value, label::interception_point);
	}

	return;
}
//Output the members to display/console
void Geo_Raster_Straight_Line::output_members(ostringstream *cout){
	*cout <<" Geo_Raster_Straight_Line "<<this->name<<endl;
	*cout <<"  Name point 2     : " << this->point2->get_point_name() <<endl;
	if(this->vertical_flag==false){
		*cout <<"  Gradient          : " << this->gradient <<label::no_unit<< endl;
		*cout <<"  y-interception    : " << this->y_interception << endl;
	}
	else{
		*cout <<"  vertical line" << endl;
	}
}
//Copy operator 
Geo_Raster_Straight_Line& Geo_Raster_Straight_Line::operator=(const Geo_Raster_Straight_Line& object){
	this->gradient=object.gradient;
	this->name=object.name;
	this->point1=object.point1;
	this->point2=object.point2;
	this->vertical_flag=object.vertical_flag;
	this->y_interception=object.y_interception;
	return *this;
}
//_____________________
//protected
//Calculate the gradient of the line
void Geo_Raster_Straight_Line::calc_gradient(void){
	double deltax=this->point2->get_xcoordinate()-this->point1->get_xcoordinate();
	if(abs(deltax)<constant::meter_epsilon){//vertical line
		this->vertical_flag=true;
		this->gradient=1e20;//numerical steep gradient
	}
	else{
		this->vertical_flag=false;
		this->gradient=(this->point2->get_ycoordinate()-this->point1->get_ycoordinate())/deltax;
		
	}
}
//Calculate the y-axis interception value
void Geo_Raster_Straight_Line::calc_y_interception(void){
	this->y_interception=this->point1->get_ycoordinate()-this->gradient*this->point1->get_xcoordinate();
}