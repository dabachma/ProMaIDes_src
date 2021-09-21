//#include "Geo_Point.h"
#include "Sys_Headers_Precompiled.h"


//constructor
Geo_Point::Geo_Point(void){
	this->x_coordinate=0.0;
	this->y_coordinate=0.0;
	this->number=0;
	
	//common
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);


}
//copy constructor
Geo_Point::Geo_Point(const Geo_Point& object){
	this->name=object.name;
	this->x_coordinate=object.x_coordinate;
	this->y_coordinate=object.y_coordinate;
	this->number=object.number;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);
	
}
//with all infos
Geo_Point::Geo_Point(const double x_coor, const double y_coor, const string name){
	this->x_coordinate=x_coor;
	this->y_coordinate=y_coor;
	this->name=name;
	this->number=0;
		//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);
	
}
//just with name
Geo_Point::Geo_Point(const string name){
	this->x_coordinate=0.0;
	this->y_coordinate=0.0;
	this->name=name;
	this->number=0;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);
	
}
//destructor
Geo_Point::~Geo_Point(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Point), _sys_system_modules::GEOSYS_SYS);
	
}
//___________________
//public
//set the name of the point
void Geo_Point::set_point_name(const string name){
	this->name=name;
}
//get the point name
string Geo_Point::get_point_name(void){
	return this->name;
}
//Get the pointer to the point name
string* Geo_Point::get_ptr_point_name(void){
	return &this->name;
}
//set the coordinates of the point
void Geo_Point::set_point_coordinate(const double x_coor, const double y_coor){
	this->x_coordinate=x_coor;
	this->y_coordinate=y_coor;
}
//Set the x-coordinate of the point
void Geo_Point::set_point_x_coordinate(const double x_coor){
	this->x_coordinate=x_coor;
}
//Set the y-coordinate of the point
void Geo_Point::set_point_y_coordinate(const double y_coor){
	this->y_coordinate=y_coor;
}
//get the x_coordinate
double Geo_Point::get_xcoordinate(void){
	return this->x_coordinate;
}
//get the y_coordinate
double Geo_Point::get_ycoordinate(void){
	return this->y_coordinate;
}
//Get the point number
int Geo_Point::get_number(void){
	return this->number;
}
//Set the point number
void Geo_Point::set_number(const int id){
	this->number=id;
}
//Add values to the point coordinates
void Geo_Point::add_values2coordinate(const double x, const double y){
	this->x_coordinate=this->x_coordinate+x;
	this->y_coordinate=this->y_coordinate+y;
}
//set the infos of the point
void Geo_Point::set_point(const double x_coor, const double y_coor, const string name){
	this->x_coordinate=x_coor;
	this->y_coordinate=y_coor;
	this->name=name;
}
//set the infos of the point
void Geo_Point::set_point(Geo_Point *point){
	this->x_coordinate=point->get_xcoordinate();
	this->y_coordinate=point->get_ycoordinate();
	this->name=point->name;
}
//set the infos of the point
void Geo_Point::set_point(Geo_Point point){
	this->x_coordinate=point.get_xcoordinate();
	this->y_coordinate=point.get_ycoordinate();
	this->name=point.name;
}
//calculate the horizontal distance between points
double Geo_Point::horizontal_distance(Geo_Point *point){
	double delta_x=0.0;
	delta_x=abs(this->x_coordinate-point->x_coordinate);
	return delta_x;
}
//calculate the horizontal distance between points
double Geo_Point::horizontal_distance(Geo_Point point){
	double delta_x=0.0;
	delta_x=abs(this->x_coordinate-point.x_coordinate);
	return delta_x;
}
//calculate the vertical distance between points
double Geo_Point::vertical_distance(Geo_Point *point){
	double delta_y=0.0;
	delta_y=abs(this->y_coordinate-point->y_coordinate);
	return delta_y;
}
//calculate the vertical distance between points
double Geo_Point::vertical_distance(Geo_Point point){
	double delta_y=0.0;
	delta_y=abs(this->y_coordinate-point.y_coordinate);
	return delta_y;
}
//calculate the direct distance between points
double Geo_Point::distance(Geo_Point *point){
	double distance=0.0;
	distance=pow((pow((this->y_coordinate-point->y_coordinate),2)+pow((this->x_coordinate-point->x_coordinate),2)),0.5);
	return distance;
}
//Get the nearest point of two given points
int Geo_Point::get_nearest_point(Geo_Point *point_1, Geo_Point *point_2){
		double dist_1=0.0;
		double dist_2=0.0;
		


		dist_1=this->distance(point_1);
		dist_2=this->distance(point_2);

		if(dist_1< dist_2){
			return 1;
		}
		else if(dist_2<dist_1){
			return 2;
		}
		else{
			return 1;
		}
}
//calculate the angle to y-axis between two points
double Geo_Point::direction_yaxsis(Geo_Point *point){
	double a=0.0;
	double c=0.0;
	double angle=0.0;
	a=this->horizontal_distance(point);
	c=this->distance(point);
	angle=acos(a/c);
	angle=_Geo_Geometrie::rad_to_grad(angle);
	if(abs(a-c)<=constant::meter_epsilon){
		if(this->check_left_of(point)==true){
			angle=90.0;
			return angle;
		}
		else{
			angle=270.0;
			return angle;
		}	
	}
	else if(abs(a)<=constant::meter_epsilon){
		if(this->check_below_of(point)==true){
			angle=180.0;
			return angle;
		}
		else{
			angle=0.0;
			return angle;
		}
	}
	
	//quadrant 1
	if(this->check_left_of(point)==true && this->check_below_of(point)==true){
		angle=angle+90.0;
	}
	//quadrant 2
	else if(this->check_right_of(point)==true && this->check_below_of(point)==true){
		angle=(90.0-angle)+180.0;
	}
	//quadrant 3
	else if(this->check_right_of(point)==true && this->check_above_of(point)==true){
		angle=angle+270.0;
	}
	//quadrant 4
	else if(this->check_left_of(point)==true && this->check_above_of(point)==true){
		angle=(90.0-angle);
	}
	return angle; 
}
//calculate the angle to x-axis between two points
double Geo_Point::direction_xaxsis(Geo_Point *point){
	double angle=0.0;
	angle=this->direction_yaxsis(point)-90.0;
	angle=_Geo_Geometrie::angle_to_360(angle);
	return angle;
}

//Transform a point with a given angle of the coordinate system (counterclockwise is positive angle)
Geo_Point Geo_Point::transform_coordinate_system(const double angle){
	double rad_angle=this->grad_to_rad(angle);
	double transform_x=0.0;
	double transform_y=0.0;
	Geo_Point my_point;
	my_point=*(this);

	if(angle!=0.0){
		transform_x=this->x_coordinate*cos(rad_angle)+this->y_coordinate*sin(rad_angle);
		transform_y=this->x_coordinate*(-1.0)*sin(rad_angle)+this->y_coordinate*cos(rad_angle);

		my_point.set_point_coordinate(transform_x, transform_y);
	}

	return my_point;
}
//Transform a point with a given angle, x and y origin of the coordinate system (counterclockwise is positive angle)
Geo_Point Geo_Point::transform_coordinate_system(const double angle, const double x_origin, const double y_origin){
	Geo_Point my_point;
	my_point=this->transform_coordinate_system(angle);
	my_point.set_point_coordinate(my_point.get_xcoordinate()+x_origin, my_point.get_ycoordinate()+y_origin);
	return my_point;
}
//check if this point is left of the given
bool Geo_Point::check_left_of(Geo_Point *point){
	bool check=false;
	if(this->x_coordinate<point->x_coordinate){
		return check=true;
	}
	else{
		return check;
	}
}
//check if this point is right of the given
bool Geo_Point::check_right_of(Geo_Point *point){
	bool check=false;
	if(this->x_coordinate>point->x_coordinate){
		return check=true;
	}
	else{
		return check;
	}
}
//check if this point is below the given one
bool Geo_Point::check_below_of(Geo_Point *point){
	bool check=false;
	if(this->y_coordinate<point->y_coordinate){
		return check=true;
	}
	else{
		return check;
	}
}
//check if this point is above the given one
bool Geo_Point::check_above_of(Geo_Point *point){
	bool check=false;
	if(this->y_coordinate>point->y_coordinate){
		return check=true;
	}
	else{
		return check;
	}
}
//Move the point for a given distance in x-direction (positiv distance is to the right)
void Geo_Point::move_to_x_direction(const double distance){

	this->x_coordinate=this->x_coordinate+distance;

}
//Move the point for a given distance in y-direction (positiv distance is upwards)
void Geo_Point::move_to_y_direction(const double distance){
	this->y_coordinate=this->y_coordinate+distance;
}
//output the point
void Geo_Point::output_members(ostringstream *cout){
	*cout<<" GEO_POINT_"<<this->name<< " x,y "<< this->x_coordinate <<", " << this->y_coordinate <<endl;
}
//Output the coordinate members to display/console
void Geo_Point::output_coor_members(ostringstream *cout){
	*cout<< FORMAT_FIXED_REAL << P(3) << this->x_coordinate <<", " << this->y_coordinate <<endl;
}
//Get a string for inserting a point to a sql-database (static)
string Geo_Point::get_point2sql_string(const double x_coor, const double y_coor){
	ostringstream buff;
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('POINT( ";
	buff << x_coor << "  " << y_coor;
	buff << ")') ";
	return buff.str();
}
//copy operator
Geo_Point& Geo_Point::operator=(const Geo_Point& object){
	this->name=object.name;
	this->x_coordinate=object.x_coordinate;
	this->y_coordinate=object.y_coordinate;
	this->number=object.number;
	return *this;
}
//Check same point
bool Geo_Point::check_same_point(Geo_Point *point){
	bool flag=false;
	//define epsilon-tolerance
	if(abs(this->x_coordinate-point->x_coordinate)<constant::meter_epsilon  && abs(this->y_coordinate-point->y_coordinate)<constant::meter_epsilon){
		flag=true;
	}
	return flag;
}
//compare operator ==
bool Geo_Point::operator==(const Geo_Point& object){
	bool flag=false;
	//define epsilon-tolerance
	if(abs(this->x_coordinate-object.x_coordinate)<constant::meter_epsilon  && abs(this->y_coordinate-object.y_coordinate)<constant::meter_epsilon){
		flag=true;
	}
	return flag;
}
//compare operator !=
bool Geo_Point::operator!=(const Geo_Point& object){
	bool flag=(*this==object);
	//if(abs(this->x_coordinate-object.x_coordinate)>constant::point_epsilon  || abs(this->y_coordinate-object.y_coordinate)>constant::point_epsilon){
	//	flag=true;
	//}
	return !flag;
}
//__________________________________
//private
//set the error
Error Geo_Point::set_error(const int err_type){
		string place="Geo_Point::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
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
//set warning
Warning Geo_Point::set_warning(const int warn_type){
		string place="Geo_Point::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no unique result found
		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;
};


