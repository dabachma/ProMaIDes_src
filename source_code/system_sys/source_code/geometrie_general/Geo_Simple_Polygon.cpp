//#include "Geo_Simple_Polygon.h"
#include "Sys_Headers_Precompiled.h"

//constructor
Geo_Simple_Polygon::Geo_Simple_Polygon(void){
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//destructor
Geo_Simple_Polygon::~Geo_Simple_Polygon(void){
	this->delete_segments();
	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//_____________________________
//public
//Set the number of points of the polygon
void Geo_Simple_Polygon::set_number_points(const int number){
	if(number>2){
		this->number_segments=number;
	}
	else{
		Error msg=this->set_error(0);
		throw msg;
	}
	//allocate the needed segments
	try{
		this->allocate_segments();
	}
	catch(Error msg){
		throw msg;
	}
}
//Set Geo_Points of the polygon
void Geo_Simple_Polygon::set_points(Geo_Point *points, const bool check){
	//check the points
	try{
		this->check_points(points);
	}
	catch(Error msg){
		throw msg;
	}

	//set the segments
	for (int i=0; i<this->number_segments-1; i++){
		this->segments[i].set_points(&(points[i]),&(points[i+1]));
	}
	//last segment is connect to the first point=> polygon is closed
	this->segments[this->number_segments-1].set_points(&(points[this->number_segments-1]),&(points[0]));

	//check segments
	try{
		if (check == true) {
			this->check_simple_polygon();
		}
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the points of the polygon
void Geo_Simple_Polygon::set_points(Geo_Point_List *point_list){
	if(point_list->get_number_points()<3){
		return;
	}

	try{
		this->set_number_points(point_list->get_number_points());
		this->set_points(point_list->get_total_list());
	}
	catch(Error msg){
		throw msg;
	}
}
//Get a segment of the polygon with the given index
Geo_Segment* Geo_Simple_Polygon::get_segment(const int index){
	if(index>=this->number_segments){
		Error msg=this->set_error(2);
		throw msg;
		return NULL;
	}
	else{
		return &(this->segments[index]);
	}
}
//Get a point of the polygon with the given index
Geo_Point* Geo_Simple_Polygon::get_point(const int index){
	if(index>=this->number_segments){
		Error msg=this->set_error(2);
		throw msg;

		return NULL;
	}
	else{
		return &(this->segments[index].point1);
	}
}
//Check if points are clockwise (true) or counter clockwise (false) sorted
bool Geo_Simple_Polygon::check_clockwise(void) {
	bool counterclockwise = false;
	double buff = 0.0;
	for (int i = 0; i < this->number_segments; i++) {
		buff = buff + (this->segments[i].point2.get_xcoordinate() - this->segments[i].point1.get_xcoordinate())*(this->segments[i].point1.get_ycoordinate() + this->segments[i].point2.get_ycoordinate());

	}
	if (buff > 0) {
		counterclockwise = true;
	}

	return counterclockwise;
}
//Get mid-point of the polygon
//https://www.biancahoegel.de/geometrie/schwerpunkt_geometrie.html
//https://qastack.com.de/programming/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
Geo_Point Geo_Simple_Polygon::get_mid_point(void) {
	Geo_Point mid;
	double buff_area = this->calculate_area();
	bool clockwise = this->check_clockwise();

	
	double buff_x = 0.0;
	double buff_y = 0.0;
	for (int i = 0; i < this->number_segments; i++) {
		//counterclockwise
		if (clockwise == false) {
			buff_x = buff_x + (this->segments[i].point1.get_xcoordinate() + this->segments[i].point2.get_xcoordinate()) * (this->segments[i].point1.get_xcoordinate()*this->segments[i].point2.get_ycoordinate() - this->segments[i].point2.get_xcoordinate()*this->segments[i].point1.get_ycoordinate());
			buff_y = buff_y + (this->segments[i].point1.get_ycoordinate() + this->segments[i].point2.get_ycoordinate()) * (this->segments[i].point1.get_xcoordinate()*this->segments[i].point2.get_ycoordinate() - this->segments[i].point2.get_xcoordinate()*this->segments[i].point1.get_ycoordinate());
		}
		else {
			//clockwise
			buff_x = buff_x + (this->segments[i].point1.get_xcoordinate() + this->segments[i].point2.get_xcoordinate()) * (this->segments[i].point2.get_xcoordinate()*this->segments[i].point1.get_ycoordinate() - this->segments[i].point1.get_xcoordinate()*this->segments[i].point2.get_ycoordinate());
			buff_y = buff_y + (this->segments[i].point1.get_ycoordinate() + this->segments[i].point2.get_ycoordinate()) * (this->segments[i].point2.get_xcoordinate()*this->segments[i].point1.get_ycoordinate() - this->segments[i].point1.get_xcoordinate()*this->segments[i].point2.get_ycoordinate());
		}

	}
	if (this->number_segments != 0) {
		buff_x = buff_x / (6.0*buff_area);
		buff_y = buff_y / (6.0*buff_area);
		mid.set_point_coordinate(buff_x, buff_y);
	}
	return mid;
}
//Change point coordinates by a given point index
void Geo_Simple_Polygon::change_point_coordinates(const int index, const double x, const double y){
	if(index>=this->number_segments){
		Error msg=this->set_error(2);
		throw msg;
	}
	else{
		if(index==0){
			this->segments[index].point1.set_point_coordinate(x,y);
			this->segments[index].refresh_distances();
			this->segments[this->number_segments-1].point2.set_point_coordinate(x,y);
			this->segments[this->number_segments-1].refresh_distances();
		}
		else{
			this->segments[index-1].point2.set_point_coordinate(x,y);
			this->segments[index-1].refresh_distances();
			this->segments[index].point1.set_point_coordinate(x,y);
			this->segments[index].refresh_distances();
		}

	}
}
//Set a simple polygon with a given polysegment
void Geo_Simple_Polygon::set_polygon(Geo_Polysegment *poly_seg){
	this->set_number_points(poly_seg->get_number_segments()+1);

	for(int i=0; i< this->number_segments; i++){
		if(i<this->number_segments-1){
			this->segments[i].set_points(&poly_seg->get_segment(i)->point1,&poly_seg->get_segment(i)->point2);
		}
		else{
			this->segments[i].set_points(&poly_seg->get_segment(i-1)->point2,&poly_seg->get_segment(0)->point1);
		}
	}

	try{
		this->check_simple_polygon();
	}
	catch(Error msg){
		throw msg;
	}
}
//Move the polygon for a given distance in x-direction (positiv distance is to the right)
void Geo_Simple_Polygon::move_to_x_direction(const double distance){
	for(int i=0; i< this->number_segments; i++){
		this->segments[i].move_to_x_direction(distance);
	}
}
//Move the polygon for a given distance in y-direction (positiv distance is upwards)
void Geo_Simple_Polygon::move_to_y_direction(const double distance){
	for(int i=0; i< this->number_segments; i++){
		this->segments[i].move_to_y_direction(distance);
	}
}
//check if the polygon is a simple one=> no interception of the segments
void Geo_Simple_Polygon::check_simple_polygon(void){
	_geo_interception_point intercept_point;
	intercept_point.interception_point.set_point_name(label::interception_point);

	for (int i=1; i<this->get_number_segments()-1;i++){
		for (int j=0; j<this->get_number_segments();j++){
			if(j!=i && j!=i+1 && j!=i-1){
				this->segments[i].calc_interception(&(this->segments[j]),&intercept_point);
				if(intercept_point.interception_flag==true){
					Error msg=this->set_error(1);
					ostringstream info;
					info <<"Interception between S.No " << i << " and S.No. " << j << endl;
					info << P(10) <<"x " << intercept_point.interception_point.get_xcoordinate()<<endl;
					info <<"y " << intercept_point.interception_point.get_ycoordinate()<<endl;
					info <<"Segment x(1, "<<i<<") "<<this->segments[i].point1.get_xcoordinate()<<" y(1, "<<i<<") "<<this->segments[i].point1.get_ycoordinate();
					info <<" x(2, "<<i<<") "<<this->segments[i].point2.get_xcoordinate()<<" y(2, "<<i<<") "<<this->segments[i].point2.get_ycoordinate()<<endl;
					info <<"Segment x(1, "<<j<<") "<<this->segments[j].point1.get_xcoordinate()<<" y(1, "<<j<<") "<<this->segments[j].point1.get_ycoordinate();
					info <<" x(2, "<<j<<") "<<this->segments[j].point2.get_xcoordinate()<<" y(2, "<<j<<") "<<this->segments[j].point2.get_ycoordinate()<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			else if(j==i+1 || j==i-1){
				this->segments[i].calc_interception(&(this->segments[j]),&intercept_point);
				if(intercept_point.indefinite_flag==true){
					Error msg=this->set_error(1);
					ostringstream info;
					info <<"Interception between S.No " << i << " and S.No. " << j << endl;
					info <<"Indefinite"<<endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}
	//first segment
	for (int j=0; j<this->get_number_segments();j++){
		if(j!=0 && j!=1 && j!=this->number_segments-1){
			this->segments[0].calc_interception(&(this->segments[j]),&intercept_point);
			if(intercept_point.interception_flag==true){
				Error msg=this->set_error(1);
				ostringstream info;
				info <<"Interception between first segment and S.No. " << j << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else if(j==1 || j==this->number_segments-1){
			this->segments[0].calc_interception(&(this->segments[j]),&intercept_point);
			if(intercept_point.indefinite_flag==true){
				Error msg=this->set_error(1);
				ostringstream info;
				info <<"Interception between first segment and S.No. " << j << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
	//last segment
	for (int j=0; j<this->get_number_segments();j++){
		if(j!=this->number_segments-1 && j!=0 && j!=this->number_segments-2){
			this->segments[this->number_segments-1].calc_interception(&(this->segments[j]),&intercept_point);
			if(intercept_point.interception_flag==true){
				Error msg=this->set_error(1);
				ostringstream info;
				info <<"Interception between last segment and S.No. " << j << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
		else if(j==0 || j==this->number_segments-2){
			this->segments[this->number_segments-1].calc_interception(&(this->segments[j]),&intercept_point);
			if(intercept_point.indefinite_flag==true){
				Error msg=this->set_error(1);
				ostringstream info;
				info <<"Interception between last segment and S.No. " << j << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
}
//_____________
//private
//set the error
Error Geo_Simple_Polygon::set_error(const int err_type){
	string place="Geo_Simple_Polygon::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://minum numbers of points is 3
			place.append("set_number_points(const int number)");
			reason="There have to be a minimum of three points in a polygon";
			help="Check the number of points";
			type=11;
			break;
		case 1://no simple polygon
			place.append("check_simple_polygon(void)");
			reason="The segments of the polygon are intercepting each other => no simple polygon";
			help="Check the polygon";
			type=11;
			break;
		case 2://unknow index
			place.append("get_point(const int index)/get_segment(const int index)/change_point_coordinates(const int index, const double x, const double y)");
			reason="The index of the polygon point/segment is not know";
			help="Check the code";
			type=6;
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