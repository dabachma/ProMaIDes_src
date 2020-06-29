//#include "Geo_Ray.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Geo_Ray::Geo_Ray(void){

	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Ray), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//Default destructor
Geo_Ray::~Geo_Ray(void){
		//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Ray), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//________________
//public
//Calculate if a given interception point is on the ray
bool Geo_Ray::calc_interception_point_on_ray(Geo_Point *check_point){
	bool online_flag=false;

	if(*check_point==this->point1){
		return true;
	}
	//check if interception point is on the segment
	if(this->vertical_flag==false){//not vertical
		if(this->point1.get_xcoordinate()<this->point2.get_xcoordinate()){//find min x of the to segment points
			if(check_point->get_xcoordinate()>=this->point1.get_xcoordinate()){
				online_flag=true;//check point x is between the to x coordinates
			}
			else{
				online_flag=false;
			}
		}
		else{
			if(check_point->get_xcoordinate()<=this->point1.get_xcoordinate()){
				online_flag=true;//check point x is between the to x coordinates (other direction)
			}
			else{
				online_flag=false;
			}
		}
	}
	else{//vertical
		if(this->point1.get_ycoordinate()<this->point2.get_ycoordinate()){
			if(check_point->get_ycoordinate()>=this->point1.get_ycoordinate()){
				online_flag=true;
			}
			else{
				online_flag=false;
			}
		}
		else{
			if(check_point->get_ycoordinate()<=this->point1.get_ycoordinate()){
				online_flag=true;
			}
			else{
				online_flag=false;
			}
		}
	}
	return online_flag;
}
//Calculate if point is on the ray	
bool Geo_Ray::calc_point_on_ray(Geo_Point *check_point){
	bool online_flag=false;

	if(this->calc_point_on_line(check_point)==false){
		return false;
	}
	online_flag=this->calc_interception_point_on_ray(check_point);
	return online_flag;
}
//Get and calculate an interceptionpoint with a given segment
void Geo_Ray::calc_interception(Geo_Segment *interception_segment,  _geo_interception_point *my_interception){
	my_interception->name_1=this->name;
	my_interception->name_2=*(interception_segment->get_ptr_name());
	my_interception->indefinite_flag=false;
	my_interception->interception_flag=false;
	
	//exception: check vertical line
	if(this->vertical_flag==true && interception_segment->get_vertical_flag()==false){//check if it is vertic
		double x_value=this->point1.get_xcoordinate();
		double y_value=interception_segment->get_gradient()*(x_value)+interception_segment->get_y_interception();
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);

		//check if interception point is on the segment
		if(this->calc_interception_point_on_ray(&my_interception->interception_point)==true && interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on segment both segments
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	else if(this->vertical_flag==false && interception_segment->get_vertical_flag()==true){
		double x_value=interception_segment->point1.get_xcoordinate();
		double y_value=this->gradient*(x_value)+this->y_interception;
		my_interception->interception_flag=true;
		my_interception->indefinite_flag=false;
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_ray(&my_interception->interception_point)==true&& interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on both segments
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
	//both lines are vertical
	else if(this->vertical_flag==true && interception_segment->get_vertical_flag()==true){
		if(this->calc_interception_point_on_ray(&interception_segment->point1)==true || this->calc_interception_point_on_ray(&interception_segment->point2)==true ){
			if(this->point1==interception_segment->point1 || this->point1==interception_segment->point2){
				my_interception->indefinite_flag=false;
				my_interception->interception_flag=true;
				my_interception->interception_point=this->point1;
			}
			else{
				my_interception->indefinite_flag=true;
				my_interception->interception_flag=false;
			}
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
		return;
	}


	//calculate the interception point (no vertical segment
	double delta_m=this->get_gradient()-interception_segment->get_gradient();
	double delta_b=interception_segment->get_y_interception()-this->get_y_interception();
	if(delta_m==0.0){//no one point interception
		if(delta_b==0.0){//indefinite interception points
			if(this->calc_interception_point_on_ray(&interception_segment->point1)==true || this->calc_interception_point_on_ray(&interception_segment->point2)==true){
				if(this->point1==interception_segment->point1 || this->point1==interception_segment->point2){
					my_interception->indefinite_flag=false;
					my_interception->interception_flag=true;
					my_interception->interception_point=this->point1;
				}
				else{
					my_interception->indefinite_flag=true;
					my_interception->interception_flag=false;
				}
			}
			else{
				my_interception->indefinite_flag=false;
				my_interception->interception_flag=false;
			}
		}
		else{
			my_interception->indefinite_flag=false;
			my_interception->interception_flag=false;
		}
		return;
	}
	else{
		my_interception->indefinite_flag=false;
		my_interception->interception_flag=true;
		double x_value=delta_b/delta_m;
		double y_value=this->get_gradient()*x_value+this->get_y_interception();
		my_interception->interception_point.set_point_coordinate(x_value, y_value);
		//check if interception point is on the segment
		if(this->calc_interception_point_on_ray(&my_interception->interception_point)==true && interception_segment->calc_interception_point_on_segment(&my_interception->interception_point)==true){//is on both segment
			return;
		}
		else{//not on segment
			my_interception->interception_flag=false;
			my_interception->indefinite_flag=false;
			return;
		}
	}
}