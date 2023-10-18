#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Slip_Circle.h"


//Default constructor
Fpl_Slip_Circle::Fpl_Slip_Circle(void): minimum_number_slices(5){

	this->number_slices=0;
	this->slices=NULL;
	this->slices_width=0.5;
	this->last_slice_width=0.0;
	this->relevant_circle=false;
	this->landside_flag=true;
	this->relevant_macro_circle=false;
	this->seepage_inside=false;
	this->intercept_area=0.0;

	this->cub_segments=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Slip_Circle), _sys_system_modules::FPL_SYS);//count the memory


}
//Default destructor
Fpl_Slip_Circle::~Fpl_Slip_Circle(void){

	this->delete_slices();
	this->delete_cub_segments();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Slip_Circle), _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
//Get number of interceptions with the structure cubatur
int Fpl_Slip_Circle::get_number_interception(void){
	return this->points_cubature.get_number_points();
}
//Get the number of slices
int Fpl_Slip_Circle::get_number_slices(void){
	return this->number_slices;
}
//Set the slices width
void Fpl_Slip_Circle::set_slices_width(const double width_slices){
	this->slices_width=width_slices;
}
//Get the flag, if it is a relevant slip circle, who intercepts the cubature
bool Fpl_Slip_Circle::get_relevant_circle(void){
	return this->relevant_circle;
}
//Set the flag, if it is a relevant slip circle
void Fpl_Slip_Circle::set_relevant_circle(const bool relevant){
	this->relevant_circle=relevant;
}
//Check the interception with the cubature
void Fpl_Slip_Circle::check_interception2structure(Geo_Polysegment *cubature, Geo_Point *crest_mid, int *number_no, int *number_one, int *number_more_two, int *number_y_coor, Geo_Simple_Polygon *outer_poly, int *number_outer){

	this->points_cubature.delete_list();
	this->check_interception2polysegment(&this->points_cubature, cubature);
	if(this->points_cubature.get_number_points()==0 ){
		this->relevant_circle=false;
		(*number_no)++;
		return;
	}
	if (this->points_cubature.get_number_points() == 1) {
		this->relevant_circle = false;
		(*number_one)++;
		return;
	}
	this->points_cubature.sort_list_x_coordinates(this->landside_flag);
	if(this->points_cubature.get_number_points()>2){
		(*number_more_two)++;
		//no warning just count
		/*Warning msg=this->set_warning(1);
		ostringstream info;
		info<<"Radius of the slip circle        : "<<this->radius << label::m<<endl;
		info<<"x-coordiante of the slip circle  : "<<this->mid_point.get_xcoordinate()<<label::m<<endl;
		info<<"y-coordiante of the slip circle  : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
		info<<"Number interception              : "<<this->points_cubature.get_number_points()<<endl;
		this->points_cubature.output_members(&info);
		msg.make_second_info(info.str());
		msg.output_msg(1);*/

	}
	Geo_Point_List buff_list;
	this->check_interception2polygon(&buff_list,outer_poly);
	if(buff_list.get_number_points()>this->points_cubature.get_number_points()){
		this->relevant_circle=false;
		(*number_outer)++;
		return;
	}
	
	


	if(this->points_cubature.get_number_points()>1){
		
		
		if(this->landside_flag==true){
			if(this->points_cubature.get_ptr_point(0)->get_ycoordinate()>this->points_cubature.get_ptr_point(1)->get_ycoordinate()){
				this->relevant_circle=true;
			}
			else{
				(*number_y_coor)++;
				return;
			}
		}
		if(this->landside_flag==false){
			if(this->points_cubature.get_ptr_point(0)->get_ycoordinate()>this->points_cubature.get_ptr_point(1)->get_ycoordinate()){
				this->relevant_circle=true;
			}
			else{
				(*number_y_coor)++;
				return;
			}

		}
	}

	if(this->landside_flag==true){
		if(this->points_cubature.get_ptr_point(0)->check_right_of(crest_mid)==false){
			this->relevant_macro_circle=true;
		}
	}
	else{
		if(this->points_cubature.get_ptr_point(0)->check_left_of(crest_mid)==false){
			this->relevant_macro_circle=true;
		}

	}

}
//Initialize the slices of each slip circle
bool Fpl_Slip_Circle::init_slices(Geo_Simple_Polygon *cubature, const double y_max, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, const double delta_h,  int *number_slice_reduced, const double y_min, int *number_slice2small){

	if(this->relevant_circle==false){
		return true;
	}

	double dist_hori=0.0;
	//take the first and second interception points as slipe circle points
	dist_hori=this->points_cubature.get_ptr_point(0)->horizontal_distance(this->points_cubature.get_ptr_point(1));

	//claculate number of slices
	this->number_slices=(int)(dist_hori/this->slices_width);
	this->last_slice_width=this->slices_width;
	if(dist_hori-(double)this->number_slices*this->slices_width>constant::meter_epsilon*100.0){
		this->last_slice_width=dist_hori-(double)this->number_slices*this->slices_width;
		this->number_slices++;
	}

	//check number of slices
	if(this->number_slices<this->minimum_number_slices){
		(*number_slice_reduced)++;
		this->number_slices=this->minimum_number_slices;
		this->slices_width=(double)(dist_hori/this->number_slices);
		this->last_slice_width=this->slices_width;
		if(this->slices_width<constant::meter_epsilon*100.0){
			(*number_slice_reduced)--;
			(*number_slice2small)++;
			this->relevant_circle=false;
			return true;

		}
		//just count and make; no warning
		//Warning msg=this->set_warning(0);
		//ostringstream info;
		//info<<"Radius of the slip circle        : "<<this->radius << label::m<<endl;
		//info<<"x-coordiante of the slip circle  : "<<this->mid_point.get_xcoordinate()<<label::m<<endl;
		//info<<"y-coordiante of the slip circle  : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
		//info<<"New slice width                  : "<<this->slices_width<<label::m<<endl;
		//msg.make_second_info(info.str());
		//msg.output_msg(1);
	}

	Fpl_Calculation::check_stop_thread_flag();
	//allocate the number of slices
	try{
		this->allocate_slices();
	

		/*ostringstream cout;
		cout <<"Number the slices of "<<this->number_slices  << endl ;
		Sys_Common_Output::output_fpl->output_txt(&cout);*/

		//init the slices
		this->relevant_circle=this->make_geometrical_slice_interception(cubature, y_max, number_mat_zones, mat_zones, buffer_all_zone, delta_h, y_min);
		if(this->relevant_circle==false){
			return false;
		}
	}
	catch(Error msg){
		ostringstream info;
		info<< "Midpoint x : "<<P(10) << FORMAT_FIXED_REAL<<this->mid_point.get_xcoordinate()<<label::m<<endl;
		info<< "Midpoint y : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
		info<< "Radius     : "<<this->get_radius()<<label::m<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	return true;
}
//Set the relevant points of the seepage line to the given list
void Fpl_Slip_Circle::add_seepage_line_points(Fpl_Seepage_Line_Point_List *seepage_list, Geo_Polysegment *cubature){
	if(this->relevant_circle==false){
		return;
	}
	int index=0;
	for(int i=0; i< this->number_slices; i++){
		index=seepage_list->add_new_point(this->slices[i].get_mid_point()->get_xcoordinate(),0.0, false, cubature);
		this->slices[i].set_index_point_seepage_line(index);
	}
}
//Output the members to display/console
void Fpl_Slip_Circle::output_members(const int no){
	if(this->relevant_circle==false){
		return;
	}

	ostringstream prefix;
	prefix << "CRICLE_"<<no<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	ostringstream cout;
	cout<<"SLIP CIRCLE "<< endl;
	cout <<" Midpoint x-coordinate            : " << this->mid_point.get_xcoordinate()<<label::m<< endl;
	cout <<" Midpoint y-coordinate            : " << this->mid_point.get_ycoordinate()<<label::m<< endl;
	cout <<" Radius                           : " << this->radius<<label::m<< endl;
	cout <<" Number interception to cubature  : " << this->points_cubature.get_number_points() << label::no_unit << endl;
	cout <<" Relevant slip circle             : " << functions::convert_boolean2string(this->relevant_circle) << endl;
	cout <<" Number slices                    : " << this->number_slices<<label::no_unit << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	if(this->number_slices>0){
		cout<<" SLICES:"<< endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		//output the slices
		cout << "Start point : "<< this->points_cubature.get_ptr_point(0)->get_xcoordinate()<<"/"<<this->points_cubature.get_ptr_point(0)->get_ycoordinate()<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
		for(int i=0;i<this->number_slices;i++){
			this->slices[i].output_members(i);
		}
		cout << "End point   : "<< this->points_cubature.get_ptr_point(1)->get_xcoordinate()<<"/"<<this->points_cubature.get_ptr_point(1)->get_ycoordinate()<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout,true);
	}
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the stability of the slip circle
void Fpl_Slip_Circle::calculate_stability(Fpl_Seepage_Line_Point_List *seepage_list, double *macro, double *micro, const double water_density){
	
	*macro=-1.0;
	*micro=-1.0;

	if(this->relevant_circle==false){
		return;
	}
	

	double self_weight=0.0;
	double area_water=0.0;
	double tan_friction=0.0;
	double resistance=0.0;
	double stress=0.0;
	bool wet_flag=false;
	this->seepage_inside=false;
	int number_com_wet=0;
	bool complet_wet=false;

	
	for(int i=0; i<this->number_slices; i++){
		self_weight=constant::Cgg*this->slices[i].get_mid_weight(seepage_list, &wet_flag, &area_water, &number_com_wet);
		if(this->seepage_inside==false && wet_flag==true ){
			this->seepage_inside=true;
		}
		tan_friction=this->slices[i].get_mid_tan_friction();
		//reststance
		if((this->slices[i].get_sin_angle_slice()*tan_friction+this->slices[i].get_cos_angle_slice())>0.0){
			resistance=resistance+(self_weight*tan_friction+this->slices[i].get_mid_cohesion()*this->slices[i].get_slice_width())
				/(this->slices[i].get_sin_angle_slice()*tan_friction+this->slices[i].get_cos_angle_slice());
		}
		//stress
		stress=stress+((self_weight+area_water*water_density*constant::Cgg)*this->slices[i].get_sin_angle_slice());
		
	}
	if(number_com_wet==this->number_slices){
		complet_wet=true;
	}
	if(stress<=0.0){
		stress=0.1;
	}

	double add_moment=0.0;
	if(this->landside_flag==false){
		add_moment=this->calc_additional_moment_waterbody(seepage_list->get_list_point(1)->get_waterlevel(), water_density);
	}

	

	if(this->relevant_macro_circle){
		*macro=(resistance*this->radius+add_moment)/(stress*this->radius);
	}
	else{
		if(this->seepage_inside==true && complet_wet==false){
			*micro=(resistance*this->radius+add_moment)/(stress*this->radius);
		}
	}
}
//Set if it is a landside or a waterside slip circle
void Fpl_Slip_Circle::set_waterside_slip_circle(const bool flag){
	this->landside_flag=!flag;
}
//Get if it is a landside slip circle
bool Fpl_Slip_Circle::get_landside_slip_circle(void){
	return this->landside_flag;
}
//Get if it is a relevant slip circle for the macro stability
bool Fpl_Slip_Circle::get_relevant_macro_circle(void){
	return this->relevant_macro_circle;
}
//Get if the seepage line is intercepting the slip circle
bool Fpl_Slip_Circle::get_seepage_inside_circle(void){
	return this->seepage_inside;
}
//Check the area of the section which is intercept by thze slip circle
void Fpl_Slip_Circle::check_area_intercept(const double area_cubature, int *number_small_area){
	if(this->relevant_circle==false){
		return;
	}

	if(this->intercept_area/area_cubature<0.01){
		this->relevant_circle=false;
		(*number_small_area)++;
		//just count no warning
		//Warning msg=this->set_warning(2);
		//ostringstream info;
		//info<<"Radius of the slip circle        : "<<this->radius << label::m<<endl;
		//info<<"x-coordiante of the slip circle  : "<<this->mid_point.get_xcoordinate()<<label::m<<endl;
		//info<<"y-coordiante of the slip circle  : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
		//info<<"Percentage of intercepted area   : "<<this->intercept_area/area_cubature<<label::percentage<<endl;
		//msg.make_second_info(info.str());
		//msg.output_msg(1);
	}

}
//Output the slices to tecplot
void Fpl_Slip_Circle::output_slices2tecplot(ofstream *output, const bool landside){
	if(this->relevant_circle==false){
		return;
	}

	if(landside==true){
		*output<<" " <<W(12)<< this->points_cubature.get_ptr_point(0)->get_xcoordinate()<<" " <<W(12)<< this->points_cubature.get_ptr_point(0)->get_ycoordinate()<<endl;
	}
	else{
		*output<<" " <<W(12)<< this->points_cubature.get_ptr_point(1)->get_xcoordinate()<<" " <<W(12)<< this->points_cubature.get_ptr_point(1)->get_ycoordinate()<<endl;

	}
	for(int i=0; i< this->number_slices; i++){
		*output<<" " <<W(12)<< this->slices[i].get_mid_point()->get_xcoordinate()<<" " <<W(12)<< this->slices[i].get_mid_point()->get_ycoordinate()<<endl;
	}
	if(landside==true){
		*output<<" " <<W(12)<< this->points_cubature.get_ptr_point(1)->get_xcoordinate()<<" " <<W(12)<< this->points_cubature.get_ptr_point(1)->get_ycoordinate()<<endl;
	}
	else{
		*output<<" " <<W(12)<< this->points_cubature.get_ptr_point(0)->get_xcoordinate()<<" " <<W(12)<< this->points_cubature.get_ptr_point(0)->get_ycoordinate()<<endl;

	}
	output->flush();

}
//Output the slices to paraview
void Fpl_Slip_Circle::output_slices2paraview(ofstream *output, const bool landside, const int before, const int after) {
	if (this->relevant_circle == false) {
		return;
	}

	if (landside == true) {
		*output << this->points_cubature.get_ptr_point(0)->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, before);
		*output << this->points_cubature.get_ptr_point(0)->get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, after);
		*output << endl;
	}
	else {
		*output << this->points_cubature.get_ptr_point(1)->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, before);
		*output << this->points_cubature.get_ptr_point(1)->get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, after);
		*output << endl;

	}



	for (int i = 0; i < this->number_slices; i++) {
		*output << this->slices[i].get_mid_point()->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, before);
		*output	<< this->slices[i].get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, after);
		*output << endl;
	}


	if (landside == true) {
		*output << this->points_cubature.get_ptr_point(1)->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, before);
		*output << this->points_cubature.get_ptr_point(1)->get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, after);
		*output << endl;
	}
	else {
		*output << this->points_cubature.get_ptr_point(0)->get_xcoordinate() << ",";
		functions::add_seperator_csv("NAN,", output, before);
		*output << this->points_cubature.get_ptr_point(0)->get_ycoordinate();
		functions::add_seperator_csv(",NAN", output, after);
		*output << endl;

	}
	output->flush();



}
//Output the slices to excel
void Fpl_Slip_Circle::output_slices2excel(ofstream *output, const bool landside, const int before, const int after) {
	if (this->relevant_circle == false) {
		return;
	}

	if (landside == true) {
		*output << this->points_cubature.get_ptr_point(0)->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, before);
		*output << this->points_cubature.get_ptr_point(0)->get_ycoordinate();
		functions::add_seperator_csv(";", output, after);
		*output << endl;
	}
	else {
		*output << this->points_cubature.get_ptr_point(1)->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, before);
		*output << this->points_cubature.get_ptr_point(1)->get_ycoordinate();
		functions::add_seperator_csv(";", output, after);
		*output << endl;

	}



	for (int i = 0; i < this->number_slices; i++) {
		*output << this->slices[i].get_mid_point()->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, before);
		*output << this->slices[i].get_mid_point()->get_ycoordinate();
		functions::add_seperator_csv(";", output, after);
		*output << endl;
	}


	if (landside == true) {
		*output << this->points_cubature.get_ptr_point(1)->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, before);
		*output << this->points_cubature.get_ptr_point(1)->get_ycoordinate();
		functions::add_seperator_csv(";", output, after);
		*output << endl;
	}
	else {
		*output << this->points_cubature.get_ptr_point(0)->get_xcoordinate() << ";";
		functions::add_seperator_csv(";", output, before);
		*output << this->points_cubature.get_ptr_point(0)->get_ycoordinate();
		functions::add_seperator_csv(";", output, after);
		*output << endl;

	}
	output->flush();

}
//Set the relevant segments of the cubature, which are involved in the slip cricle
void Fpl_Slip_Circle::set_involved_cubature_segment(Geo_Polysegment *cubature){

	if(this->relevant_circle==false){
		return;
	}

	Geo_Point start;
	Geo_Point end;
	Geo_Point_List buffer;


	if(this->landside_flag==true){
		start.set_point_coordinate(this->points_cubature.get_ptr_point(0)->get_xcoordinate(),this->points_cubature.get_ptr_point(0)->get_ycoordinate());
		end.set_point_coordinate(this->points_cubature.get_ptr_point(1)->get_xcoordinate(),this->points_cubature.get_ptr_point(1)->get_ycoordinate());
	}
	else{
		start.set_point_coordinate(this->points_cubature.get_ptr_point(1)->get_xcoordinate(),this->points_cubature.get_ptr_point(1)->get_ycoordinate());
		end.set_point_coordinate(this->points_cubature.get_ptr_point(0)->get_xcoordinate(),this->points_cubature.get_ptr_point(0)->get_ycoordinate());
	}

	buffer.set_new_point(&start);

	for(int i=0; i< cubature->get_number_segments(); i++){
		if(cubature->get_segment(i)->point1.check_right_of(&start)==true && cubature->get_segment(i)->point1.check_left_of(&end)==true){
			buffer.set_new_point(&cubature->get_segment(i)->point1);
		}
	}
	buffer.set_new_point(&end);

	this->allocate_cub_segments();

	this->cub_segments->set_segments(buffer.get_number_points(), buffer.get_total_list());
	this->cub_segments->set_lever_arm(&this->mid_point, this->landside_flag);

}
//___________
//private
//Allocate the slices
void Fpl_Slip_Circle::allocate_slices(void){
	this->delete_slices();
	try{
		this->slices=new Fpl_Slope_Slices[this->number_slices];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	for(int i=0; i< this->number_slices-1; i++){
		this->slices[i].set_slice_width(this->slices_width);
	}

	if(this->number_slices>0){
		this->slices[this->number_slices-1].set_slice_width(this->last_slice_width);
	}
}
//Delete the slices
void Fpl_Slip_Circle::delete_slices(void){
	if(this->slices!=NULL){
		delete []this->slices;
		this->slices=NULL;
	}
}
//Allocate the segments of the cubature, which are involved in the slip circle
void Fpl_Slip_Circle::allocate_cub_segments(void){
	this->delete_cub_segments();
	try{
		this->cub_segments=new Fpl_Slope_External_Force;
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(2);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the segments of the cubature, which are involved in the slip circle
void Fpl_Slip_Circle::delete_cub_segments(void){
	if(this->cub_segments!=NULL){
		delete this->cub_segments;
		this->cub_segments=NULL;
	}
}
//Intercept each slices and calculate the relevant area
bool Fpl_Slip_Circle::make_geometrical_slice_interception(Geo_Simple_Polygon *cubature, const double y_max, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, const double delta_h, const double y_min){

	this->intercept_area=0.0;
	Geo_Simple_Polygon slice;
	slice.set_number_points(4);

	Geo_Simple_Polygon wet_slice;
	wet_slice.set_number_points(4);

	double x_start=0.0;
	double y_coor1=0.0;
	double y_coor2=0.0;

	if(this->landside_flag==true){
		x_start=this->points_cubature.get_ptr_point(0)->get_xcoordinate();
		y_coor1=this->points_cubature.get_ptr_point(0)->get_ycoordinate();
	}
	else{
		x_start=this->points_cubature.get_ptr_point(1)->get_xcoordinate();
		y_coor1=this->points_cubature.get_ptr_point(1)->get_ycoordinate();
	}

	bool relevant=true;

	for(int i=0; i< this->number_slices; i++){

		Fpl_Calculation::check_stop_thread_flag();
		//set the slice polygon clockwise
		if(i<this->number_slices-1){
			if(i>0){
				x_start=x_start+this->slices_width;
			}
			slice.get_segment(0)->set_coordinates(x_start,y_max+0.1,x_start+this->slices_width,y_max+0.1);
			//calculate the y-coordinate 2
			if(this->get_y_coordinate_point_at_circle(x_start+this->slices_width, &y_coor2)==false){
				//Error
				Error msg=this->set_error(1);
				ostringstream info;
				info<<"Radius of the slip circle        : "<<this->radius << label::m<<endl;
				info<<"x-coordinate of the slip circle  : "<<this->mid_point.get_xcoordinate()<<label::m<<endl;
				info<<"y-coordinate of the slip circle  : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			slice.get_segment(1)->set_coordinates(x_start+this->slices_width,y_max+0.1,x_start+this->slices_width,y_coor2);
			slice.get_segment(2)->set_coordinates(x_start+this->slices_width,y_coor2,x_start,y_coor1);
			slice.get_segment(3)->set_coordinates(x_start,y_coor1,x_start,y_max+0.1);
			y_coor1=y_coor2;
		}
		else{
			x_start=x_start+this->slices_width;
			slice.get_segment(0)->set_coordinates(x_start,y_max+0.1,x_start+this->last_slice_width,y_max+0.1);
			//calculate the y-coordinate 2
			if(this->get_y_coordinate_point_at_circle(x_start+this->last_slice_width, &y_coor2)==false){
				//Error
				Error msg=this->set_error(1);
				ostringstream info;
				info<<"Radius of the slip circle        : "<<this->radius << label::m<<endl;
				info<<"x-coordinate of the slip circle  : "<<this->mid_point.get_xcoordinate()<<label::m<<endl;
				info<<"y-coordinate of the slip circle  : "<<this->mid_point.get_ycoordinate()<<label::m<<endl;
				msg.make_second_info(info.str());
				throw msg;
			}
			slice.get_segment(1)->set_coordinates(x_start+this->last_slice_width,y_max+0.1,x_start+this->last_slice_width,y_coor2);
			slice.get_segment(2)->set_coordinates(x_start+this->last_slice_width,y_coor2,x_start,y_coor1);
			slice.get_segment(3)->set_coordinates(x_start,y_coor1,x_start,y_max+0.1);
		}
		//if(i>5){
		///*ostringstream cout;
		//cout <<" i "<<i  << endl ;
		//Sys_Common_Output::output_fpl->output_txt(&cout);*/
		//ostringstream cout;
		//cout <<"slice"<<endl;
		//for(int j=0; j< slice.get_number_segments(); j++){
		//	
		//	slice.get_segment(j)->point1.output_members(&cout);
		//	slice.get_segment(j)->point2.output_members(&cout);
		//	Sys_Common_Output::output_fpl->output_txt(&cout);
		//}
		//
		//
		//}
		//set the area of the slices
		relevant=this->slices[i].init_slice(cubature, &slice, &wet_slice, number_mat_zones, mat_zones, buffer_all_zone, this, delta_h, y_min);
		if(relevant==false){
			break;
		}
		this->intercept_area=this->intercept_area+this->slices[i].get_total_slice_area();
	}
	return relevant;

}
//Calculate additional moment of the waterbody
double Fpl_Slip_Circle::calc_additional_moment_waterbody(const double waterlevel, const double water_density){

	return this->cub_segments->calc_external_water_forces(waterlevel,water_density);
}
//set the error
Error Fpl_Slip_Circle::set_error(const int err_type){
		string place="Fpl_Slip_Circle::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_slip_circle(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no point at circle is found
			place.append("make_geometrical_slice_interception(Geo_Polysegment *cubature, const double y_max)");
			reason="No y_coordinate is found at the slip circle for the given x-coordinate";
			help="Check the code";
			type=6;
			break;
		case 2://bad_alloc
			place.append("allocate_cub_segments(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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
Warning Fpl_Slip_Circle::set_warning(const int warn_type){
	string place="Fpl_Slip_Circle::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://slice width is greater than circle width
			place.append("init_slices(Geo_Simple_Polygon *cubature, const double y_max, const int number_mat_zones, Fpl_Dike_Geo_Materialzone *mat_zones, Fpl_Dike_Geo_Materialzone_Connector *buffer_all_zone, const double delta_h)");
			reason="The slice width of the slip circle is greater than the relevant circle width";
			reaction="The slice width is reduced, until a minimum number of slices is reached";
			type=26;
			break;
		case 1://more than three interception
			place.append("check_interception2structure(Geo_Polysegment *cubature, Geo_Point *crest_mid)");
			reason="There are more than three interception of the slip circle with the section's cubature";
			reaction="The two first found interception points are taken into account";
			type=26;
			break;
		case 2://more than three interception
			place.append("check_area_intercept(const double area_cubature)");
			reason="The total area of interception of the slip circle is smaller than 5% of the total section cuabture area";
			reaction="The slip circle is not raken into account";
			type=26;
			break;
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