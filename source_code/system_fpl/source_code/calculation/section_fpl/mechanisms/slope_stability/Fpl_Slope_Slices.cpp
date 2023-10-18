#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Slope_Slices.h"

//Default constructor
Fpl_Slope_Slices::Fpl_Slope_Slices(void){

	this->total_area=0.0;
	this->total_slip_length=0.0;

	this->number_connect_zone_area=0;
	this->connect_zone_area=NULL;
	this->number_connect_zone_slip_surface=0;
	this->connect_zone_slip_surface=NULL;

	this->angle_of_slice=0.0;
	this->sin_angle_slice=0.0;
	this->cos_angle_slice=0.0;

	this->index_seepage_point=-1;

	this->slice_width=0.0;

	
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Slope_Slices)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Slope_Slices::~Fpl_Slope_Slices(void){

	this->delete_area_material_zone();
	this->delete_slip_surface_material_zone();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Slope_Slices)-sizeof(Geo_Point), _sys_system_modules::FPL_SYS);//count the memory
}
//_________
//public
//Initialize the slice of the slope circle (here müssen noch die Zonen übergeben werden)
bool Fpl_Slope_Slices::init_slice(Geo_Simple_Polygon *poly_cub, Geo_Simple_Polygon *poly_slice, Geo_Simple_Polygon *poly_wet_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, Fpl_Slip_Circle *ptr_circle, const double delta_h, const double h_start){

	bool relevant=true;
	//the slip surface is given by the second segment
	this->total_slip_length=poly_slice->get_segment(2)->get_distance();
	//interception with the zones for the slip surface
	relevant=this->mat_zone_interception_slip_surface(poly_slice, number_mat_zones, mat_zones, buffer_all_zone);
	if(relevant==false){
		return relevant;
	}
	//set the x-coordinate of the midpoint
	this->mid_point=poly_slice->get_segment(2)->calc_mid_point();
	//set the angle of the slice
	this->set_slice_angle(ptr_circle);
	//set the total area
	this->total_area=poly_cub->polygon_interception(poly_slice);

	
	//interception with the dry zones
	relevant=this->mat_zone_interception_dry_area(poly_slice, number_mat_zones, mat_zones, buffer_all_zone);
	if(relevant==false){
		return relevant;
	}
	//interception with the wet zones
	relevant=this->mat_zone_interception_wet_area(poly_slice, poly_wet_slice, delta_h, buffer_all_zone, poly_cub, h_start);
	if(relevant==false){
		return relevant;
	}
	return relevant;
}
//Get the total slice area
double Fpl_Slope_Slices::get_total_slice_area(void){
	return this->total_area;
}
//Get the total slip lenght
double Fpl_Slope_Slices::get_total_slip_length(void){
	return this->total_slip_length;
}
//Get the mid point of the slice at the slip surface
Geo_Point* Fpl_Slope_Slices::get_mid_point(void){
	return &this->mid_point;
}
//Get the mid weight
double Fpl_Slope_Slices::get_mid_weight(Fpl_Seepage_Line_Point_List *seepage_list, bool *wet_flag, double *water_area, int *number_comp_wet){
	double sum=0.0;
	double h_water=0.0;
	*wet_flag=false;
	//set current waterlevel

	h_water=seepage_list->get_list_point(this->index_seepage_point)->get_waterlevel()-this->mid_point.get_ycoordinate();
	if(h_water<0.0){
		h_water=0.0;
	}
	else{
		*wet_flag=true;
	}
	
	//get the wet table point (interpolated)
	Fpl_Dike_Geo_Wet_Materialzone_Table_Point *buffer;
	buffer=this->wet_area_table.interpolate_table_values(h_water);
	*water_area=buffer->get_perc_area()*this->get_total_slice_area();
	if(buffer->get_perc_area()>=1.0-constant::percentage_epsilon){
		(*number_comp_wet)++;
	}

	//calculate the weight
	for(int i=0; i<this->number_connect_zone_area; i++){
		sum=sum+(buffer->get_perc_area()*buffer->get_ptr_connected_mat_zones(i)->get_percentage_mat_zone()*this->connect_zone_area[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_uplift_density()+
			(this->connect_zone_area[i].get_percentage_mat_zone()-buffer->get_perc_area()*buffer->get_ptr_connected_mat_zones(i)->get_percentage_mat_zone())*this->connect_zone_area[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_wet_density());
	}
	sum=sum*this->get_total_slice_area();
	return sum;
}
//Get the mid tangent of the angle of friction
double Fpl_Slope_Slices::get_mid_tan_friction(void){
	double sum=0.0;

	if(this->number_connect_zone_slip_surface>1){
		for(int i=0; i<this->number_connect_zone_slip_surface; i++){
			sum=sum+this->connect_zone_slip_surface[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->angle_friction.get_result().end_result*
				this->connect_zone_slip_surface[i].get_percentage_mat_zone();
		}
		sum=tan(_Geo_Geometrie::grad_to_rad(sum));
	}
	else{
		sum=this->connect_zone_slip_surface[0].get_ptr_mat_zone()->get_ptr_relevant_variables()->get_tan_friction_angle();
	}


	return sum;
}
//Get the mit of cohesion
double Fpl_Slope_Slices::get_mid_cohesion(void){
	double sum=0.0;
	for(int i=0; i<this->number_connect_zone_slip_surface; i++){
		sum=sum+this->connect_zone_slip_surface[i].get_ptr_mat_zone()->get_ptr_relevant_variables()->cohesion.get_result().end_result*
			this->connect_zone_slip_surface[i].get_percentage_mat_zone();
	}
	return sum;
}
//Get the number of the connected material zones of the slice area
int Fpl_Slope_Slices::get_number_connect_zone_area(void){
	return this->number_connect_zone_area;
}
//Get the pointer of the connected material zones of the slice area
Fpl_Dike_Geo_Materialzone_Connector *Fpl_Slope_Slices::get_ptr_connect_zone_area(void){
	return this->connect_zone_area;
}
//Get the number of the connected material zones of the slip surface
int Fpl_Slope_Slices::get_number_connect_zone_slip_surface(void){
	return this->number_connect_zone_slip_surface;
}
//Get the pointer of the connected material zones of the slip surface
Fpl_Dike_Geo_Materialzone_Connector *Fpl_Slope_Slices::get_ptr_connect_zone_slip_surface(void){
	return this->connect_zone_slip_surface;
}
//Output the members to display/console
void Fpl_Slope_Slices::output_members(const int no){
	ostringstream cout;
	cout<<"No. "<<W(12)<<" Mid_x "<<label::m<<W(12)<< " Mid_y "<<label::m<<W(12)<< " Area " << label::squaremeter <<W(12);
	cout<< " Slip_length "<<label::m<<W(12)<<" No_area_zone "<<label::no_unit<<W(12)<< " No_slip_zone "<< label::no_unit << W(12)<< " Angle "<<label::degree<<endl;
	cout <<" "<<no<<" " <<W(12)<<this->mid_point.get_xcoordinate()<<" " << W(19)<<this->mid_point.get_ycoordinate()<<" "<<W(14);
	cout <<this->total_area<<" "<<W(12)<<this->total_slip_length<<" "<<W(12)<<this->number_connect_zone_area<<" " <<W(12);
	cout <<this->number_connect_zone_slip_surface<<W(12)<<_Geo_Geometrie::rad_to_grad(this->angle_of_slice)<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
}
//Get the angle of the slice
double Fpl_Slope_Slices::get_slice_angle(void){
	return _Geo_Geometrie::rad_to_grad(this->angle_of_slice);
}
//Get ths slice angle (rad)
double Fpl_Slope_Slices::get_slice_angle_rad(void){
	return this->angle_of_slice;
}
//Get the sinus of the slices angle
double Fpl_Slope_Slices::get_sin_angle_slice(void){
	return this->sin_angle_slice;
}
//Get the cosinus of the slice angle
double Fpl_Slope_Slices::get_cos_angle_slice(void){
	return this->cos_angle_slice;
}
//Set the index of the point of the seepage line
void Fpl_Slope_Slices::set_index_point_seepage_line(const int index){
	this->index_seepage_point=index;
}
//Get the index of the point of the seepage line
int Fpl_Slope_Slices::get_index_point_seepage_line(void){
	return this->index_seepage_point;
}
//Get the slice width
double Fpl_Slope_Slices::get_slice_width(void){
	return this->slice_width;
}
//Set the slice width
void Fpl_Slope_Slices::set_slice_width(const double width){
	this->slice_width=width;
}
//_________
//private
//Allocate the connected material zones for the area in the slice
void Fpl_Slope_Slices::allocate_area_material_zone(void){
	this->delete_area_material_zone();
	try{
		this->connect_zone_area=new Fpl_Dike_Geo_Materialzone_Connector[this->number_connect_zone_area];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the connected material zones for the area in the slice
void Fpl_Slope_Slices::delete_area_material_zone(void){
	if(this->connect_zone_area!=NULL){
		delete []this->connect_zone_area;
		this->connect_zone_area=NULL;
	}
}
//Allocate the connected material zones for the slip surface
void Fpl_Slope_Slices::allocate_slip_surface_material_zone(void){
	this->delete_slip_surface_material_zone();
	try{
		this->connect_zone_slip_surface=new Fpl_Dike_Geo_Materialzone_Connector[this->number_connect_zone_slip_surface];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the connected material zones for the slip surface
void Fpl_Slope_Slices::delete_slip_surface_material_zone(void){
	if(this->connect_zone_slip_surface!=NULL){
		delete []this->connect_zone_slip_surface;
		this->connect_zone_slip_surface=NULL;
	}
}
//Calculate and set the slice angle
void Fpl_Slope_Slices::set_slice_angle(Fpl_Slip_Circle *ptr_circle){
	if(this->mid_point.check_left_of(ptr_circle->get_mid_point())==false && this->mid_point.check_right_of(ptr_circle->get_mid_point())==false){
		this->angle_of_slice=0.0;
	}
	else{
		double dist=this->mid_point.horizontal_distance(ptr_circle->get_mid_point());
		this->angle_of_slice=asin(dist/ptr_circle->get_radius());
		//landside slope
		if(ptr_circle->get_landside_slip_circle()==true){
			if(this->mid_point.check_left_of(ptr_circle->get_mid_point())==false){
				this->angle_of_slice=this->angle_of_slice*(-1.0);	
			}
		}
		//waterside slope
		else{
			if(this->mid_point.check_left_of(ptr_circle->get_mid_point())==true){
				this->angle_of_slice=this->angle_of_slice*(-1.0);	
			}
		}
	}
	this->sin_angle_slice=sin(this->angle_of_slice);
	this->cos_angle_slice=cos(this->angle_of_slice);
}
//Make the geometrical interception with material zones for the dry area calculation
bool Fpl_Slope_Slices::mat_zone_interception_dry_area(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone){
	//reset the buffer
	for(int i=0; i< number_mat_zones; i++){
		buffer_all_zone[i].reset_connector();
	}

	//make interception of slice with zone for the area
	int counter_found=0;
	double area=0.0;
	double total_per=0.0;
	for(int i=0; i< number_mat_zones; i++){
		Fpl_Calculation::check_stop_thread_flag();
		area=0.0;
		area=mat_zones[i].polygon_interception(poly_slice);
		if(area>0.0){
			counter_found++;
			area=area/this->total_area;
			if(area>1.0+constant::percentage_epsilon){
				return false;
			}
			total_per=total_per+area;
			buffer_all_zone[counter_found-1].set_percentage_mat_zone(area);
			buffer_all_zone[counter_found-1].set_ptr_mat_zone(&mat_zones[i]);
			if(total_per>1.0-constant::percentage_epsilon/10.0){
				break;
			}	
		}
	}


	if(total_per<1.0-constant::percentage_epsilon || total_per>1.0+constant::percentage_epsilon){
		return false;
	}

	Fpl_Calculation::check_stop_thread_flag();
	//allocate the area zone connector
	this->number_connect_zone_area=counter_found;
	this->allocate_area_material_zone();
	//copy them
	for(int i=0; i< this->number_connect_zone_area; i++){
		this->connect_zone_area[i].set_percentage_mat_zone(buffer_all_zone[i].get_percentage_mat_zone());
		this->connect_zone_area[i].set_ptr_mat_zone(buffer_all_zone[i].get_ptr_mat_zone());
	}
	return true;
}
//Make the geometrical interception with material zones for the slip surface calculation
bool Fpl_Slope_Slices::mat_zone_interception_slip_surface(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone){
	int counter_found=0;
	double area=0.0;
	double total_per=0.0;

	//reset the buffer
	for(int i=0; i< number_mat_zones; i++){
		buffer_all_zone[i].reset_connector();
	}

	Geo_Interception_Point_List buff_list;
	//make interception of slice slip segment with zone for the slip length
	for(int i=0; i< number_mat_zones; i++){
		Fpl_Calculation::check_stop_thread_flag();
		area=0.0;
		area=mat_zones[i].segment_interception(poly_slice->get_segment(2), &buff_list);
		if(area>0.0){
			counter_found++;
			area=area/this->total_slip_length;
			if(area>1.0+constant::percentage_epsilon){
				//Error
				Error msg=this->set_error(3);
				throw msg;
			}
			total_per=total_per+area;
			buffer_all_zone[counter_found-1].set_percentage_mat_zone(area);
			buffer_all_zone[counter_found-1].set_ptr_mat_zone(&mat_zones[i]);
			if(total_per>=1.0-constant::percentage_epsilon){
				break;
			}	
		}
	}

		if(total_per<=1.0-constant::percentage_epsilon || total_per>1.0+constant::percentage_epsilon){
			return false;
		}



	//allocate the area zone connector
	this->number_connect_zone_slip_surface=counter_found;
	this->allocate_slip_surface_material_zone();
	//copy them
	for(int i=0; i< this->number_connect_zone_slip_surface; i++){
		this->connect_zone_slip_surface[i].set_percentage_mat_zone(buffer_all_zone[i].get_percentage_mat_zone());
		this->connect_zone_slip_surface[i].set_ptr_mat_zone(buffer_all_zone[i].get_ptr_mat_zone());
	}

	return true;
}
//Make the geometrical interception with material zones for the wet area calculation
bool Fpl_Slope_Slices::mat_zone_interception_wet_area(Geo_Simple_Polygon *poly_slice, Geo_Simple_Polygon *poly_wet_slice, const double delta_h, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, Geo_Simple_Polygon *poly_cub, const double h_start){
	
	this->wet_area_table.set_number_connected_mat_zones(this->number_connect_zone_area);

	for(int i=0; i< this->number_connect_zone_area; i++){
		buffer_all_zone[i].reset_connector();
		buffer_all_zone[i].set_ptr_mat_zone(this->connect_zone_area[i].get_ptr_mat_zone());
	}

	//add the zero point
	this->wet_area_table.add_table_point(0.0,0.0,buffer_all_zone);

	//the rest points	
	Geo_Point *buff1;
	Geo_Point *buff2;
	double buff_h=0.0;
	double area_wet=0.0;
	double area_zone=0.0;
	//calculate the wet slice zone connector

	//make first wet polygon
	//set the wet slice polygon
	buff1=&poly_slice->get_segment(2)->point1;
	buff2=&poly_slice->get_segment(2)->point2;
	poly_wet_slice->get_segment(0)->set_points(buff1,buff2);
	if(buff1->check_below_of(buff2)==true){
		poly_wet_slice->get_segment(1)->set_coordinates(buff2->get_xcoordinate(),buff2->get_ycoordinate(), buff2->get_xcoordinate(), buff2->get_ycoordinate()+0.01);
		buff_h=buff2->get_ycoordinate()+0.01-this->mid_point.get_ycoordinate();
		poly_wet_slice->get_segment(2)->set_coordinates(buff2->get_xcoordinate(), buff2->get_ycoordinate()+0.01, buff1->get_xcoordinate(), buff2->get_ycoordinate()+0.01);
		poly_wet_slice->get_segment(3)->set_coordinates(buff1->get_xcoordinate(), buff2->get_ycoordinate()+0.01, buff1->get_xcoordinate(), buff1->get_ycoordinate());
	}
	else if(buff1->check_above_of(buff2)==true){
		poly_wet_slice->get_segment(1)->set_coordinates(buff2->get_xcoordinate(),buff2->get_ycoordinate(), buff2->get_xcoordinate(), buff1->get_ycoordinate()+0.01);
		buff_h=buff1->get_ycoordinate()+0.01-this->mid_point.get_ycoordinate();
		poly_wet_slice->get_segment(2)->set_coordinates(buff2->get_xcoordinate(), buff1->get_ycoordinate()+0.01, buff1->get_xcoordinate(), buff1->get_ycoordinate()+0.01);
		poly_wet_slice->get_segment(3)->set_coordinates(buff1->get_xcoordinate(), buff1->get_ycoordinate()+0.01, buff1->get_xcoordinate(), buff1->get_ycoordinate());
	}
	else{
		poly_wet_slice->get_segment(1)->set_coordinates(buff2->get_xcoordinate(),buff2->get_ycoordinate(), buff2->get_xcoordinate(), buff2->get_ycoordinate()+delta_h);
		buff_h=delta_h;
		poly_wet_slice->get_segment(2)->set_coordinates(buff2->get_xcoordinate(), buff2->get_ycoordinate()+delta_h, buff1->get_xcoordinate(), buff1->get_ycoordinate()+delta_h);
		poly_wet_slice->get_segment(3)->set_coordinates(buff1->get_xcoordinate(), buff1->get_ycoordinate()+delta_h, buff1->get_xcoordinate(), buff1->get_ycoordinate());
	}
	//calculate the total wet area
	area_wet=poly_cub->polygon_interception(poly_wet_slice);

	//ostringstream cout;
	//cout <<"Wet"<<endl;
	//for(int i=0; i< poly_wet_slice->get_number_segments(); i++){
	//	poly_wet_slice->get_segment(i)->output_members(&cout);
	//}
	//Sys_Common_Output::output_fpl->output_txt(&cout);
	//cout <<"Dry"<<endl;
	//for(int i=0; i< poly_slice->get_number_segments(); i++){
	//	poly_slice->get_segment(i)->output_members(&cout);
	//}
	//Sys_Common_Output::output_fpl->output_txt(&cout);


	if(area_wet>=this->total_area-constant::area_epsilon){
		//all is wet; no zone interception is required
		this->wet_area_table.add_table_point(buff_h,1.0,this->connect_zone_area);
		return true;
	}
	else{
		//zone interception
		for(int i=0; i<this->number_connect_zone_area; i++){
			area_zone=poly_wet_slice->polygon_interception(this->connect_zone_area[i].get_ptr_mat_zone());
			buffer_all_zone[i].set_percentage_mat_zone(area_zone/area_wet);
			buffer_all_zone[i].set_ptr_mat_zone(this->connect_zone_area[i].get_ptr_mat_zone());

		}
		//add the new point to table
		this->wet_area_table.add_table_point(buff_h,area_wet/this->total_area,buffer_all_zone);
	}

	int counter=0;
	double new_h=0.0;
	//do the steps of delta h

	do{
		area_wet=0.0;
		counter++;
		//increase the delta h
		new_h=buff_h+counter*delta_h;

		//set the wet slice polygon
		poly_wet_slice->get_segment(1)->point2.add_values2coordinate(0.0,delta_h);
		poly_wet_slice->get_segment(2)->point1.add_values2coordinate(0.0,delta_h);
		poly_wet_slice->get_segment(2)->point2.add_values2coordinate(0.0,delta_h);
		poly_wet_slice->get_segment(3)->point1.add_values2coordinate(0.0,delta_h);

		if((new_h+this->mid_point.get_ycoordinate())>=(h_start-2.0*delta_h)){
				


			for(int i=0; i< poly_wet_slice->get_number_segments(); i++){
				poly_wet_slice->get_segment(i)->refresh_distances();
			}
			double sum_per=0.0;


			//calculate the total wet area
			area_wet=poly_cub->polygon_interception(poly_wet_slice);
			if(area_wet>this->total_area){
				area_wet=this->total_area;
			}
			//zone interception
			for(int i=0; i<this->number_connect_zone_area; i++){
				area_zone=poly_wet_slice->polygon_interception(this->connect_zone_area[i].get_ptr_mat_zone());
				buffer_all_zone[i].set_percentage_mat_zone(area_zone/area_wet);
				sum_per=sum_per+area_zone/area_wet;
				buffer_all_zone[i].set_ptr_mat_zone(this->connect_zone_area[i].get_ptr_mat_zone());
			}
	
			if(sum_per< 1.0-constant::percentage_epsilon || sum_per>1.0+constant::percentage_epsilon){
				return false;
			}

			//add the new point to table
			this->wet_area_table.add_table_point(new_h,area_wet/this->total_area,buffer_all_zone);

		}		
		
	}
	while(area_wet<=this->total_area-constant::area_epsilon);

	Fpl_Calculation::check_stop_thread_flag();

	return true;

	/*cout << " count "<< counter << " h " << new_h <<" y "<< this->mid_point.get_ycoordinate()<<" ";
	cout <<" x "<< this->mid_point.get_xcoordinate()<<" "<<new_h+ this->mid_point.get_ycoordinate()<<" " << this->number_connect_zone_area << endl;*/

}
//set the error
Error Fpl_Slope_Slices::set_error(const int err_type){
		string place="Fpl_Slope_Slices::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_area_material_zone(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad_alloc
			place.append("allocate_slip_surface_material_zone(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://found area is greater than total area
			place.append("mat_zone_interception_dry_area(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone)");
			reason="Found area in zones is greater than the total slice area";
			help="Check the code";
			type=6;
			break;
		case 3://found slip length is greater than total slip length
			place.append("mat_zone_interception_slip_surface(Geo_Simple_Polygon *poly_slice, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone)");
			reason="Found slip length in zones is greater than the total slip length";
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

//Set warning(s)
Warning Fpl_Slope_Slices::set_warning(const int warn_type){
	string place="Fpl_Slope_Slices::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){

		default:
			place.append("set_warning(const int warn_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=5;
	}
	msg.set_msg(place,reason,help,reaction,type);
	msg.make_second_info(info.str());
	return msg;

}