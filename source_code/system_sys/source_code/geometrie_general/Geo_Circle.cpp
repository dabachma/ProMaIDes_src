//#include "Geo_Circle.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Geo_Circle::Geo_Circle(void){
	this->radius=0.0;
	this->mid_point.set_point_name("Midpoint");
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Circle), _sys_system_modules::GEOSYS_SYS);
}
//Default destructor
Geo_Circle::~Geo_Circle(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Circle), _sys_system_modules::GEOSYS_SYS);
}
//____________
//public
//Set the radius
void Geo_Circle::set_radius(const double rad){
	this->radius=rad;
}
//Get the radius
double Geo_Circle::get_radius(void){
	return this->radius;
}
//Set the midpoint
void Geo_Circle::set_midpoint(Geo_Point *point){
	this->mid_point=*point;
}
//Set the midpoint
void Geo_Circle::set_midpoint(const double x_coor, const double y_coor){
	this->mid_point.set_point_coordinate(x_coor, y_coor);
}
//Get the midpoint
Geo_Point *Geo_Circle::get_mid_point(void){
	return &this->mid_point;
}
//Get the area of the circle
double Geo_Circle::get_area(void){
	return constant::Cpi*0.5*pow(this->radius,0.5);
}
//Get the perimeter of the circle
double Geo_Circle::get_perimeter(void){
	return 2.0*constant::Cpi*this->radius;
}
//Check if a point is inside a circle
bool Geo_Circle::check_point_inside(Geo_Point *point){

	double diff_x;
	double diff_y;
	diff_x=point->get_xcoordinate()-this->mid_point.get_xcoordinate();
	diff_y=point->get_ycoordinate()-this->mid_point.get_ycoordinate();
	if(pow(diff_x,2.0)+pow(diff_y,2.0)<this->radius){
		return true;
	}
	else{
		return false;
	}

}
//Check if a point is on a circle
bool Geo_Circle::check_point_atboundary(Geo_Point *point){
	double diff_x;
	double diff_y;
	diff_x=point->get_xcoordinate()-this->mid_point.get_xcoordinate();
	diff_y=point->get_ycoordinate()-this->mid_point.get_ycoordinate();
	if(pow(diff_x,2.0)+pow(diff_y,2.0)-this->radius==constant::meter_epsilon){
		return true;
	}
	else{
		return false;
	}

}
//Check an interception between a straight line and a circle; tangent points are not taken into account
void Geo_Circle::check_interception2line(Geo_Point_List *list, Geo_Straight_Line *line){

	Geo_Point buffer;
	list->delete_list();

	double root=0.0;
	double root1=0.0;
	double x=0.0;
	double y=0.0;

	if(line->get_vertical_flag()==false){
		root=(pow(this->radius,2.0)-pow(this->mid_point.get_xcoordinate(),2.0)-pow(this->mid_point.get_ycoordinate(),2.0)-pow(line->get_y_interception(),2.0)+2.0*this->mid_point.get_ycoordinate()*line->get_y_interception());
		root=root/(1.0+pow(line->get_gradient(),2.0));
		
		root1=(line->get_gradient()*line->get_y_interception()-this->mid_point.get_xcoordinate()-line->get_gradient()*this->mid_point.get_ycoordinate());
		root1=root1/(1.0+pow(line->get_gradient(),2.0));
		root=root+pow(root1,2.0);
		if(root<0.0){
			//no interception;
			return;
		}
		else if(root==0.0){
			//one interception (tangente)
			x=-1.0*root1;
			y=line->get_gradient()*x+line->get_y_interception();
			buffer.set_point(x, y, "Interception");
			//exlcue tangent point
			//list->set_new_point(&buffer);
		}
		else{
			//two interception
			x=-1.0*root1 + pow(root,0.5);
			y=line->get_gradient()*x+line->get_y_interception();
			buffer.set_point(x, y, "Interception");
			list->set_new_point(&buffer);

			x=-1.0*root1 - pow(root,0.5);
			y=line->get_gradient()*x+line->get_y_interception();
			buffer.set_point(x, y, "Interception");
			list->set_new_point(&buffer);
		}
	}
	else{
		//vertical
		root=pow(this->radius,2.0)-pow((line->point1.get_xcoordinate()-this->mid_point.get_xcoordinate()),2.0);
		if(root<0.0){
			//no interception
			return;
		}
		else if(root==0.0){
			//one interception tangent
			x=line->point1.get_xcoordinate();
			y=this->mid_point.get_ycoordinate();
			buffer.set_point(x, y, "Interception");
			//list->set_new_point(&buffer);
		}
		else{
			//two interceptions
			x=line->point1.get_xcoordinate();
			y=this->mid_point.get_ycoordinate()+pow(root,0.5);
			buffer.set_point(x, y, "Interception");
			list->set_new_point(&buffer);

			x=line->point1.get_xcoordinate();
			y=this->mid_point.get_ycoordinate()-pow(root,0.5);
			buffer.set_point(x, y, "Interception");
			list->set_new_point(&buffer);
		}
	}
}
//Check an interception between a segment and a circle; tangent points are not taken into account
void Geo_Circle::check_interception2segment(Geo_Point_List *list, Geo_Segment *segment){

	Geo_Point buffer;
	Geo_Point_List list_buff;


	this->check_interception2line(&list_buff, segment);
	
	//check if points are at the segement
	for(int i=0; i <list_buff.get_number_points(); i++){
		if(segment->calc_point_on_segment(list_buff.get_ptr_point(i))==true){
			//set them to the list
			list->set_new_point(list_buff.get_ptr_point(i));
		}
	}
}
//Check an interception between a polysegment and a circle; tangent points are not taken into account
void Geo_Circle::check_interception2polysegment(Geo_Point_List *list, Geo_Polysegment *poly_segment){
	for(int i=0; i <poly_segment->get_number_segments(); i++){
		this->check_interception2segment(list, poly_segment->get_segment(i));
	}
}
///Check an interception between a polygon and a circle; tangent points are not taken into account
void Geo_Circle::check_interception2polygon(Geo_Point_List *list, Geo_Simple_Polygon *poly){

	for(int i=0; i <poly->get_number_segments(); i++){
		this->check_interception2segment(list, poly->get_segment(i));
	}

}
//Get the minimum y-coordinate of a point of the circle by a given x-coordinate; if no coordinate is found false is returned
bool Geo_Circle::get_y_coordinate_point_at_circle(const double x_coor, double *y_coor){
	
	

	double root=pow(this->radius,2.0)-pow((x_coor-this->mid_point.get_xcoordinate()),2.0);

	//two points are possible; return the minimum
	if(root>0.0){
		*y_coor=this->mid_point.get_ycoordinate()-pow(root,0.5);
		return true;
	}
	//one point
	else if(root<=0.0 && root>=(-constant::meter_epsilon)){
		*y_coor=this->mid_point.get_ycoordinate();
		return true;
	}

	//no point
	else if(root<(-constant::meter_epsilon)){
		return false;
	}
	
	
	
	return false;
}
