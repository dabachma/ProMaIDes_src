#include "Sys_Headers_Precompiled.h"
//#include "Geo_Raster.h"

//Default constructor
Geo_Raster::Geo_Raster(void){

	this->number_polygons=0;
	this->number_points=0;
	this->number_segments=0;
	this->no_elems_x=0;
	this->no_elems_y=0;
	this->width_x=0.0;
	this->width_y=0.0;
	this->area=0.0;
	this->x_coor_origin=0.0;
	this->y_coor_origin=0.0;
	this->raster_points=NULL;
	this->raster_segments=NULL;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Raster)-sizeof(Geo_Raster_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);

}
//Default destructor
Geo_Raster::~Geo_Raster(void){
	this->delete_raster_points();
	this->delete_raster_segments();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Raster)-sizeof(Geo_Raster_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);
}
//_______________
//public
//Initialize the raster: the points and segments
void Geo_Raster::init_raster(void){
	this->generate_points_segments();
	this->area=this->width_x*this->width_y;
}
//Clear the raster information: points and segments
void Geo_Raster::clear_raster(void){
	this->delete_raster_points();
	this->delete_raster_segments();
}
//Set a geometrical raster polygon with a given element index
void Geo_Raster::set_geometrical_raster_polygon(const int element_index){
	int nx_counter=(int)(element_index/this->no_elems_x);
	//calculate the index
	int index_y=element_index+(2*this->no_elems_x+1)*(nx_counter+1)-nx_counter*this->no_elems_x;
	int index_x=index_y-this->no_elems_x;
	int index_minus_y=index_y-(2*this->no_elems_x+1);
	int index_minus_x=index_x-1;

	//set the polygon
	try{
		this->raster_elem.set_polygon_segments(&(this->raster_segments[index_y]), &(this->raster_segments[index_x]), &(this->raster_segments[index_minus_y]), &(this->raster_segments[index_minus_x]));
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the number of elements
int Geo_Raster::get_number_elements(void){
	return this->number_polygons;
}
//Get the area of a raster element
double Geo_Raster::get_area(void){
	return this->area;
}
//_______________
//protected
//Generate the raster points and -segments
void Geo_Raster::generate_points_segments(void){
	this->delete_raster_points();
	this->delete_raster_segments();
	//set number of points
	this->number_points=(this->no_elems_x+1)*(this->no_elems_y+1);
	//set number of elements
	this->number_polygons=this->no_elems_x*this->no_elems_y;
	//set the segments
	this->number_segments=(this->no_elems_x+ this->no_elems_y)+(this->no_elems_y*2*this->no_elems_x);

	//allocate the points
	this->allocate_raster_points();
	//allocate the segments
	this->allocate_raster_segments();


	//generate the points
	int counter=0;
	Geo_Point buffer;
	for(int i=0; i<this->no_elems_y+1;i++){
		for(int j=0; j< this->no_elems_x+1;j++){
			this->raster_points[counter].set_point_coordinate((double)j*this->width_x, (double)i*this->width_y);
			buffer=this->raster_points[counter].transform_coordinate_system(0.0,this->x_coor_origin, this->y_coor_origin);
			this->raster_points[counter].set_point(&buffer);
			this->raster_points[counter].set_point_name(label::raster_point);
			counter++;
		}
	}

	//generate the segments
	counter=0;
	int counter_x=0;
	int counter_y=0;
	bool x_set=true;
	bool y_set=true;
	int index_1=0;
	int index_2=0;
	
	for(int j=0; j< this->number_segments ;j++){
		if(counter_y==counter_x){
			index_1=counter-counter_y*this->no_elems_x;
			index_2=index_1+1;
			this->raster_segments[j].set_points(&(this->raster_points[index_1]),&(this->raster_points[index_2]));
		
		}
		else{
			index_1=counter-counter_x*this->no_elems_x;
			index_2=index_1+this->no_elems_x+1;
			this->raster_segments[j].set_points(&(this->raster_points[index_1]),&(this->raster_points[index_2]));
		
		}
		if(counter==(this->no_elems_x-1)+counter_x*this->no_elems_x+counter_y*(this->no_elems_x+1)&& y_set==true){
			counter_x++;
			x_set=true;
			y_set=false;
		}
		if(counter==(this->no_elems_x)+counter_x*this->no_elems_x+counter_y*(this->no_elems_x+1) && x_set==true){
			counter_y++;
			x_set=false;
			y_set=true;
		}
		counter++;
	}
	//this->delete_raster_points();
}
//Generate the geometrical boundary
void Geo_Raster::generate_boundary(void){
	Geo_Point buffer[4];
	//first point
	buffer[0].set_point_coordinate(0.0, 0.0);
	buffer[0]=buffer[0].transform_coordinate_system(0.0, this->x_coor_origin, this->y_coor_origin);
	//second point
	buffer[1].set_point_coordinate(this->width_x*(double)this->no_elems_x,0.0);
	buffer[1]=buffer[1].transform_coordinate_system(0.0, this->x_coor_origin, this->y_coor_origin);
	//third point
	buffer[2].set_point_coordinate(this->width_x*(double)this->no_elems_x,this->width_y*(double)this->no_elems_y);
	buffer[2]=buffer[2].transform_coordinate_system(0.0, this->x_coor_origin, this->y_coor_origin);
	//fourth point
	buffer[3].set_point_coordinate(0.0,this->width_y*(double)this->no_elems_y);
	buffer[3]=buffer[3].transform_coordinate_system(0.0, this->x_coor_origin, this->y_coor_origin);

	//set it to the polygon
	try{
		this->geometrical_bound.set_number_points(4);
		this->geometrical_bound.set_points(buffer);
	}
	catch(Error msg){
		ostringstream info;
		info<< "Try to set-up the geometrical raster boundary" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Allocate the raster points
void Geo_Raster::allocate_raster_points(void){
	
	try{
		if(this->raster_points==NULL){
			this->raster_points=new Geo_Point[this->number_points];	
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		info <<"Number points: "<< this->number_points<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the raster points
void Geo_Raster::delete_raster_points(void){
	if(this->raster_points!=NULL){
		delete []this->raster_points;
		this->raster_points=NULL;

	}
}
//Allocate the raster points
void Geo_Raster::allocate_raster_segments(void){

	try{
		if(this->raster_segments==NULL){
			this->raster_segments=new Geo_Raster_Segment[this->number_segments];
			//count the memory
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		info <<"Number segements: "<< this->number_segments<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the raster points
void Geo_Raster::delete_raster_segments(void){
	if(this->raster_segments!=NULL){
		delete []this->raster_segments;
		this->raster_segments=NULL;
		//count the memory
	}
}
//Check the geometrical information of the raster
void Geo_Raster::check_raster_info(void){
	ostringstream info;
	if(this->no_elems_x<=0){
		Error msg=this->set_error(1);
		info <<"The number of raster elements in x-direction must be greater 0" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->no_elems_y<=0){
		Error msg=this->set_error(1);
		info <<"The number of raster elements in y-direction must be greater 0" << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	if(this->width_x<=0){
		Error msg=this->set_error(1);
		info <<"The width of the raster elements in x-direction must be greater 0"<< label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->width_y<=0){
		Error msg=this->set_error(1);
		info <<"The width of the raster elements in y-direction must be greater 0"<< label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	if(this->no_elems_x*this->no_elems_y>=constant::max_elems){
        Error msg=this->set_error(3);
		throw msg;
	}
}
//_____________
//private
//Set error(s)
Error Geo_Raster::set_error(const int err_type){
		string place="Geo_Raster::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("alloc_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://error in the raster information
			place.append("check_raster_info(void)");
			reason="The raster informations are not properly set";
			help="Check the raster informations";
			type=11;
			break;
		case 2://bad alloc
			place.append("allocate_raster_segments(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 3://to much elements
			place.append("check_raster_info(void)");
			reason="There are to much elements in the raster";
			help="Split the raster or decrease the raster resolution";
			type=33;
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
