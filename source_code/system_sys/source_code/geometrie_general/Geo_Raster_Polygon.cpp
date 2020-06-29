#include "Sys_Headers_Precompiled.h"
//#include "Geo_Raster_Polygon.h"

//constructor
Geo_Raster_Polygon::Geo_Raster_Polygon(void){
	this->number_segments=4;
	//allocate the needed segments
	try{
		this->allocate_segments();
	}
	catch(Error msg){
		throw msg;
	}
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Raster_Polygon), _sys_system_modules::GEOSYS_SYS);

}
//destructor
Geo_Raster_Polygon::~Geo_Raster_Polygon(void){
	this->delete_segments();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Raster_Polygon), _sys_system_modules::GEOSYS_SYS);

}
//_____________
//public
//set the points with a given mid_point, x-width, y-width and angle of the coordinate system
void Geo_Raster_Polygon::set_points(Geo_Point *mid_point, const double x_width, const double y_width, const double angle, const double x_origin, const double y_origin){
	if(x_width <=0.0 || y_width<=0.0){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "x_width = " << x_width << label::m << "y_width " << y_width << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	bool transform_required=false;
	if(abs(angle)>constant::angle_epsilon){
		transform_required=true;
	}
	Geo_Point local_mid;
	local_mid=*mid_point;
	if(transform_required==true){
		local_mid.set_point_coordinate(local_mid.get_xcoordinate()-x_origin, local_mid.get_ycoordinate()-y_origin);
		local_mid=local_mid.transform_coordinate_system(-1.0*angle);
	}
	

	Geo_Point buff1;
	Geo_Point buff2;
	//direction clockwise
	//first left, above
	buff1.set_point_coordinate(local_mid.get_xcoordinate()-x_width/2.0,local_mid.get_ycoordinate()+y_width/2.0);
	if(transform_required==true){
		buff1=buff1.transform_coordinate_system(angle, x_origin, y_origin);
	}
	buff2.set_point_coordinate(local_mid.get_xcoordinate()+x_width/2.0,local_mid.get_ycoordinate()+y_width/2.0);
	if(transform_required==true){
		buff2=buff2.transform_coordinate_system(angle, x_origin, y_origin);
	}
	this->segments[0].set_points(&buff1, &buff2);
	//second right, above
	buff1.set_point_coordinate(local_mid.get_xcoordinate()+x_width/2.0,local_mid.get_ycoordinate()-y_width/2.0);
	if(transform_required==true){
		buff1=buff1.transform_coordinate_system(angle, x_origin, y_origin);
	}
	this->segments[1].set_points(&buff2, &buff1);
	//third right, down
	buff2.set_point_coordinate(local_mid.get_xcoordinate()-x_width/2.0,local_mid.get_ycoordinate()-y_width/2.0);
	if(transform_required==true){
		buff2=buff2.transform_coordinate_system(angle, x_origin, y_origin);
	}
	this->segments[2].set_points(&buff1, &buff2);
	//fourth left , down
	buff1.set_point_coordinate(local_mid.get_xcoordinate()-x_width/2.0,local_mid.get_ycoordinate()+y_width/2.0);
	if(transform_required==true){
		buff1=buff1.transform_coordinate_system(angle, x_origin, y_origin);
	}
	this->segments[3].set_points(&buff2, &buff1);

}
//Set the polygon's segments
void Geo_Raster_Polygon::set_polygon_segments(Geo_Segment *segment_y, Geo_Segment *segment_x, Geo_Segment *segment_minus_y, Geo_Segment *segment_minus_x){
	this->segments[0]=(*segment_y);
	this->segments[1]=(*segment_x);
	this->segments[2]=(*segment_minus_y);
	this->segments[3]=(*segment_minus_x);

	this->sort_points();
}
//Set the polygon's segments
void Geo_Raster_Polygon::set_polygon_segments(Geo_Raster_Segment *segment_y, Geo_Raster_Segment *segment_x, Geo_Raster_Segment *segment_minus_y, Geo_Raster_Segment *segment_minus_x){
	this->segments[0].set_segment(segment_y);
	this->segments[1].set_segment(segment_x);
	this->segments[2].set_segment(segment_minus_y);
	this->segments[3].set_segment(segment_minus_x);

	this->sort_points();
}
//Get the midpoint of the element
Geo_Point Geo_Raster_Polygon::get_mid_point(void){

	Geo_Point buffer;
	double mid_x=0.0;
	double mid_y=0.0;

	for(int i=0; i< 4; i++){
		mid_x=mid_x+this->segments[i].point1.get_xcoordinate();
		mid_y=mid_y+this->segments[i].point1.get_ycoordinate();

	}

	mid_x=mid_x/4.0;
	mid_y=mid_y/4.0;
	buffer.set_point_coordinate(mid_x, mid_y);
	return buffer;

}
//Get a segment of the polygon with the given index
Geo_Segment* Geo_Raster_Polygon::get_segment(const int index){
	if(index>=this->number_segments){
		Error msg=this->set_error(0);
		throw msg;

		return NULL;
	}
	else{
		return &(this->segments[index]);
	}
}
//Get a point of the polygon with the given index
Geo_Point* Geo_Raster_Polygon::get_point(const int index){
	if(index>=this->number_segments){
		Error msg=this->set_error(0);
		throw msg;
		return NULL;
	}
	else{
		return &(this->segments[index].point1);
	}
}
//__________________
//private
//Sort the points
void Geo_Raster_Polygon::sort_points(void){
	for(int i=1; i<this->number_segments;i++){
		if(this->segments[i-1].point2!=this->segments[i].point1){
			this->segments[i].switch_points();
		}
	}
}
//set the error
Error Geo_Raster_Polygon::set_error(const int err_type){
	string place="Geo_Raster_Polygon::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://unknow index
			place.append("get_point(const int index)/get_segment(const int index)");
			reason="The index of the polygon point/segment is not know";
			help="Check the code";
			type=6;
			break;
		case 1://x-width or y-width is <=0.0
			place.append("set_points(Geo_Point *mid_point, const double x_width, const double y_width, const double angle)");
			reason="Can not make a raster polygon; one width is <=0.0";
			help="Check the width";
			type=1;
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