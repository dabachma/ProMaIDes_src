//#include "Hyd_River_Profile_Type_Standard.h"
#include "Hyd_Headers_Precompiled.h"

//constructor
Hyd_River_Profile_Type_Standard::Hyd_River_Profile_Type_Standard(void){
	
	//tables
	this->c_left_table.set_table_type(hyd_label::Conveyance_table_left);
	this->c_right_table.set_table_type(hyd_label::Conveyance_table_right);
	this->c_mid_table.set_table_type(hyd_label::Conveyance_table_mid);
	this->h_table.set_table_type(hyd_label::Waterlevel_table);
	this->width_table.set_table_type(hyd_label::Width_table);
	this->area_table.set_table_type(hyd_label::Area_table);
	

	this->no_mid_points=0;
	this->no_mid_segments=0;
	this->no_left_bank_points=0;
	this->no_left_bank_segments=0;
	this->no_right_bank_points=0;
	this->no_right_bank_segments=0;
	this->total_segment_number=0;

	//segments for the table calculation
	this->mid_profile=NULL;
	this->left_bank_profile=NULL;
	this->right_bank_profile=NULL;

	//result values per time step
	this->c_right_value=0.0;
	this->c_mid_value=0.0;
	this->c_left_value=0.0;

	this->left_bank_flag=false;
	this->right_bank_flag=false;

	this->material_type=_hyd_mat_coefficient_types::manning_coefficient;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Type_Standard)-sizeof(Hyd_Tables)*5, _sys_system_modules::HYD_SYS);


}

//destructor
Hyd_River_Profile_Type_Standard::~Hyd_River_Profile_Type_Standard(void){

	this->delete_area_segments();

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Type_Standard)-sizeof(Hyd_Tables)*5, _sys_system_modules::HYD_SYS);

}
//_________________________
//public
//calculate and allocate the area values (x-value) and the tables (y-value)
void Hyd_River_Profile_Type_Standard::calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table){
	//check which bank exists and count the number of points
	try{
		
		this->count_area_points();
	}
	catch(Error msg){
		throw msg;
	}


	//claculate number of tablespoints
	this->number_table_points=(int)(((this->global_z_max+this->increment_max_height)-this->global_z_min)/delta_x)+2;

	//allocate the depending values of the table
	try{
		this->area_table.allocate_dependent_values(number_table_points);
		this->width_table.allocate_dependent_values(number_table_points);
		this->h_table.allocate_dependent_values(number_table_points);
		this->c_mid_table.allocate_dependent_values(number_table_points);
		this->c_left_table.allocate_dependent_values(number_table_points);
		this->c_right_table.allocate_dependent_values(number_table_points);

		this->area_table.set_independent_values(&this->h_table);
		this->width_table.set_independent_values(&this->area_table);
		this->h_table.set_independent_values(&this->area_table);
		this->c_mid_table.set_independent_values(&this->area_table);
		this->c_left_table.set_independent_values(&this->area_table);
		this->c_right_table.set_independent_values(&this->area_table);
	}
	catch(Error msg){
		throw msg;
	}

	//generate the river segments for calculation of tables
	try{
		this->generate_area_segments(material_table);
		this->calculate_table_values(delta_x);
		//this->output_tables();
	}
	catch(Error msg){
		throw msg;
	}

}
//plot generation
void Hyd_River_Profile_Type_Standard::generate_plot(QWidget *parent){
	_Hyd_River_Profile_Type::generate_plot(parent);

	this->plot->set_titles("Standard River Profile", "x_axis [m]", "y_axis [m]");
	this->plot->set_number_curves(3);
	this->plot->set_curves(0, "Left Bank", this->no_left_bank_segments, Qt::red, this->left_bank_profile);
	this->plot->set_curves(1, "Main Channel", this->no_mid_segments, Qt::blue, this->mid_profile);
	this->plot->set_curves(2, "Right Bank", this->no_right_bank_segments, Qt::green, this->right_bank_profile);

}
//output the tables
void Hyd_River_Profile_Type_Standard::output_tables(void){
	ostringstream cout;
	this->area_table.output_table(&cout);
	this->width_table.output_table(&cout);
	this->c_left_table.output_table(&cout);
	this->c_mid_table.output_table(&cout);
	this->c_right_table.output_table(&cout);
	this->h_table.output_table(&cout);
	
}
//Get the convenyance factor for the mid channel
double Hyd_River_Profile_Type_Standard::get_c_mid_channel(void){
	return this->c_mid_value;
}
//Get the convenyance factor total
double Hyd_River_Profile_Type_Standard::get_c_value(void){
	return this->c_mid_value+this->c_left_value+this->c_right_value;
}
//output the area segments
void Hyd_River_Profile_Type_Standard::output_area_segments(void){
	ostringstream cout;
	ostringstream prefix;

	//leftbank
	cout << "Left bank" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	prefix <<"LB>";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	prefix.str("");

	for(int i=0; i<this->no_left_bank_segments; i++){
		cout << " No " << i << endl;
		this->left_bank_profile[i].output_members(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();

	//main channel
	cout << "Main channel" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	prefix <<"MC>";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	prefix.str("");

	for(int i=0; i<this->no_mid_segments; i++){
		cout << " No " << i << endl;
		this->mid_profile[i].output_members(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();

	//right bank
	cout << "Right bank" << endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);
	prefix <<"RB>";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	prefix.str("");
	for(int i=0; i<this->no_right_bank_segments; i++){
		cout << " No " << i << endl;
		this->right_bank_profile[i].output_members(&cout);
		Sys_Common_Output::output_hyd->output_txt(&cout, true);
	}
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Set the actuel flow specific values with a given area from tables
void Hyd_River_Profile_Type_Standard::set_actuel_profilevalues_by_table(const double area){
	this->zero_outflow_reduction_flag=false;
	try{
		this->solved_area=area;
		if(this->solved_area<0.00){
			this->area2calc=0.000;
		}
		else{
			this->area2calc=this->solved_area;
		}

		this->set_actual_waterlevel_by_table(this->area2calc);

		//this->check_unphysical_waterlevels();
		//check for outflow reduction
		if(this->h_value<=constant::dry_hyd_epsilon){
			this->zero_outflow_reduction_flag=true;
		}

		this->set_actual_conveyance_by_table(this->area2calc);
		
	}
	catch(Error msg){
		throw msg;
	}
}
//Get the waterlevel by a given area from table 
double Hyd_River_Profile_Type_Standard::get_waterlevel_from_table(const double area){
	double buffer=0.0;
	buffer=this->h_table.get_interpolated_values(area);
	buffer=buffer-this->global_z_min;
		if(buffer <= constant::flow_epsilon){
			buffer=0.0;
		}
	return buffer;
}
//Get the width by a given area from table 
double Hyd_River_Profile_Type_Standard::get_width_from_table(const double area) {
	double buffer = 0.0;
	buffer = this->width_table.get_interpolated_values(area);

	return buffer;
}
///Set the actuel flow specific values with a given waterlevel from tables
void Hyd_River_Profile_Type_Standard::set_actuel_profilevalues_by_waterlevel(const double global_waterlevel){
	this->zero_outflow_reduction_flag=false;
	try{
		this->set_waterlevel_direct(global_waterlevel);
		this->check_unphysical_waterlevels();
		//check for outflow reduction
		if(this->h_value<=constant::dry_hyd_epsilon*10.0){
			this->zero_outflow_reduction_flag=true;
		}
		this->solved_area=this->area_table.get_interpolated_values(this->s_value);
		if(this->solved_area<0.0){
			this->area2calc=0.000;
		}
		else{
			this->area2calc=this->solved_area;
		}
		this->set_actual_conveyance_by_table(this->area2calc);
		this->width = this->width_table.get_interpolated_values(this->area2calc);
	}
	catch(Error msg){
		throw msg;	
	}
}
//Calculate the actual discharge through this profile by a given upstream and downstream profile
double Hyd_River_Profile_Type_Standard::calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream){
	double discharge=0.0;
	double v_area=0.0;
	double fr_h=0.0;


	//calculate the gradient
	this->calculate_gradient(distance_upstream, upstream_profile, downstream_profile);
	//same waterlevels => noflow
	if(abs(this->gradient)<constant::flow_epsilon){
		discharge=0.0;
		v_area=0.0;
		fr_h=0.0;
	}
	//flow upstream to downstream
	else if(this->gradient>0.0){
		//no discharge if there is no upstream water level 
		if(upstream_profile->typ_of_profile->get_actual_local_waterlevel_h()<constant::dry_hyd_epsilon){
			discharge=0.0;
			v_area=0.0;
			fr_h=0.0;
		}
		//steep gradient flow=> (c-value of upstream profile profiles)
		else if(this->s_value <= upstream_profile->typ_of_profile->get_global_z_min()){
				discharge=(upstream_profile->typ_of_profile->get_c_value())*this->gradient;
				v_area=upstream_profile->typ_of_profile->get_area2calc();
				fr_h=upstream_profile->typ_of_profile->get_actual_local_waterlevel_h();
		}
		//transition zone from normal flow to steep gradient flow; transition from mid of c-value to upstream c-value)
		else if(this->s_value > (upstream_profile->typ_of_profile->get_global_z_min()) && this->s_value <= (upstream_profile->typ_of_profile->get_global_z_min()+0.4) && this->global_z_min<=upstream_profile->typ_of_profile->get_global_z_min()){
			double delta_h=this->s_value-(upstream_profile->typ_of_profile->get_global_z_min());
			double mid_fac=(0.4-delta_h)/0.8;
			discharge=((0.5+mid_fac)*upstream_profile->typ_of_profile->get_c_value()+(0.5-mid_fac)*this->get_c_value())*gradient;
			v_area=(0.5+mid_fac)*upstream_profile->typ_of_profile->get_area2calc()+(0.5-mid_fac)*this->area2calc;
			fr_h=(0.5+mid_fac)*upstream_profile->typ_of_profile->get_actual_local_waterlevel_h()+(0.5-mid_fac)*this->h_value;
		}
		//starting discharge for a hole before
		else if(upstream_profile->get_actual_global_waterlevel()>(this->global_z_min+constant::dry_hyd_epsilon)&& this->h_value<=constant::dry_hyd_epsilon){
			discharge=(this->get_starting_value()+upstream_profile->typ_of_profile->get_c_value())*0.5*this->gradient;
			v_area=0.0;
			fr_h=upstream_profile->typ_of_profile->get_actual_local_waterlevel_h();

		}	
		//normal flow (mid of c-value of both profiles)
		else{
			discharge=(this->get_c_value()+upstream_profile->typ_of_profile->get_c_value())*0.5*this->gradient;
			v_area=(0.5)*upstream_profile->typ_of_profile->get_area2calc()+(0.5)*this->area2calc;
			fr_h=(0.5)*upstream_profile->typ_of_profile->get_actual_local_waterlevel_h()+(0.5)*this->h_value;
		}
	}
	//flow downstream to upstream
	else if(this->gradient<0.0){
		//no discharge if there is no water level here
		if(this->h_value<constant::dry_hyd_epsilon){
			discharge=0.0;
			v_area=0.0;
			fr_h=0.0;
		}
		//steep gradient flow=> (c-value of upstream profile profiles)
		else if(upstream_profile->typ_of_profile->get_actual_global_waterlevel() <= this->global_z_min){
				discharge=(this->get_c_value())*this->gradient;
				v_area=this->area2calc;
				fr_h=this->h_value;
		}
		//transition zone from normal flow to steep gradient flow; transition from mid of c-value to upstream c-value)
		else if(upstream_profile->typ_of_profile->get_actual_global_waterlevel() > this->global_z_min && upstream_profile->typ_of_profile->get_actual_global_waterlevel() <= (this->global_z_min+0.4) && upstream_profile->typ_of_profile->get_global_z_min()<=this->global_z_min){
			double delta_h=this->s_value-(upstream_profile->typ_of_profile->get_global_z_min());
			double mid_fac=(0.4-delta_h)/0.8;
			discharge=((0.5+mid_fac)*this->get_c_value()+(0.5-mid_fac)*upstream_profile->typ_of_profile->get_c_value())*gradient;
			v_area=(0.5-mid_fac)*upstream_profile->typ_of_profile->get_area2calc()+(0.5+mid_fac)*this->area2calc;
			fr_h=(0.5-mid_fac)*upstream_profile->typ_of_profile->get_actual_local_waterlevel_h()+(0.5+mid_fac)*this->h_value;

		}
		//normal flow
		else{
			discharge=(this->get_c_value()+upstream_profile->typ_of_profile->get_c_value())*0.5*this->gradient;
			v_area=(0.5)*upstream_profile->typ_of_profile->get_area2calc()+(0.5)*this->area2calc;
			fr_h=(0.5)*upstream_profile->typ_of_profile->get_actual_local_waterlevel_h()+(0.5)*this->h_value;
		}
	}


	//calculate the velocity
	if(v_area<=constant::area_epsilon){
		this->v_value=0.0;
		this->froude_no=0.0;
	}
	else{
		this->v_value=discharge/v_area;
		this->froude_no=this->v_value/pow((constant::Cgg*fr_h),0.5);
	}

	//this->calculate_velocity_energy_head(timepoint);

	return discharge;
}
//Get a starting value in cause of: a hole before, this profile is dry, the waterlevel before is greater than this global z_min value
double Hyd_River_Profile_Type_Standard::get_starting_value(void){
	double start_value=0.0;
	try{
		start_value=this->area_table.get_interpolated_values(this->global_z_min+constant::dry_hyd_epsilon*2.0);
		start_value=this->c_mid_table.get_interpolated_values(start_value);
	}
	catch(Error msg){
		throw msg;
	}

	return start_value;
}
//Clone profile type
void Hyd_River_Profile_Type_Standard::clone_profile_type(_Hyd_River_Profile_Type *prof_type){
	_Hyd_River_Profile_Type::clone_profile_type(prof_type);

	Hyd_River_Profile_Type_Standard *buffer=(Hyd_River_Profile_Type_Standard *)prof_type;

	this->no_mid_points=buffer->no_mid_points;
	this->no_mid_segments=buffer->no_mid_segments;
	this->no_left_bank_points=buffer->no_left_bank_points;
	this->no_left_bank_segments=buffer->no_left_bank_segments;
	this->no_right_bank_points=buffer->no_right_bank_points;
	this->no_right_bank_segments=buffer->no_right_bank_segments;
	this->total_segment_number=buffer->total_segment_number;

	this->allocate_area_segments();

	for(int i =0; i< this->no_left_bank_segments; i++){
		this->left_bank_profile[i]=buffer->left_bank_profile[i];
	}
	for(int i =0; i< this->no_mid_segments; i++){
		this->mid_profile[i]=buffer->mid_profile[i];
	}
	for(int i =0; i< this->no_right_bank_segments; i++){
		this->right_bank_profile[i]=buffer->right_bank_profile[i];
	}

	this->left_bank_flag=buffer->left_bank_flag;
	this->right_bank_flag=buffer->right_bank_flag;

	//copy tables
	this->c_left_table.clone_tables(&buffer->c_left_table);
	this->c_mid_table.clone_tables(&buffer->c_mid_table);
	this->c_right_table.clone_tables(&buffer->c_right_table);
	this->c_mid_table.set_independent_values(&this->area_table);
	this->c_left_table.set_independent_values(&this->area_table);
	this->c_right_table.set_independent_values(&this->area_table);


}
//________________
//protected
//Set actual conveyance with a given area
void Hyd_River_Profile_Type_Standard::set_actual_conveyance_by_table(const double area){
	try{
		this->c_left_value=this->c_left_table.get_interpolated_values(area);
		this->c_mid_value=this->c_mid_table.get_interpolated_values(area);
		this->c_right_value=this->c_right_table.get_interpolated_values(area);
	}
	catch(Error msg){
		throw msg;
	}
}
//______________________
//private
//Calculate the gradient
void Hyd_River_Profile_Type_Standard::calculate_gradient(const double distance, _Hyd_River_Profile *upstream, _Hyd_River_Profile *downstream_profile){
	
	//calculate the gradient
	if(this->user_setting.explict_v_is_used==true && (upstream->get_profile_type()==_hyd_profile_types::RIVER_TYPE || upstream->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE) && downstream_profile!=NULL){
		this->gradient=((upstream->get_actual_global_waterlevel()+upstream->typ_of_profile->get_velocity_energy_head())-(this->s_value+this->get_velocity_energy_head()));
	}
	else{
		this->gradient=(upstream->get_actual_global_waterlevel()-this->s_value);
	}

	//same waterlevels => no gradient
	if(abs(this->gradient)<=constant::flow_epsilon/1000.0){
		gradient=0.0;
	}
	//almost same waterlevels: replace the gradient by arctan-function
	/*else if(abs(this->gradient)<=0.005078){
		this->gradient=0.10449968880528*atan(159.877741951379*(this->gradient))/pow(distance,0.5);
	}*/
	else if(abs(this->gradient)<=0.000246925){
		this->gradient=0.02304416717797*atan(3287.68191373281*(this->gradient))/pow(distance,0.5);
	}
	//take the square root function
	else{
		this->gradient=this->gradient/(pow(distance,0.5)*pow(abs(this->gradient),0.5));
	}
}
//check if there is a right bank and a left bank in the profile
void Hyd_River_Profile_Type_Standard::count_area_points(void){
	for(int i=0; i< this->number_points; i++){
		if(this->points[i].identity==1){
			this->no_left_bank_points++;
		}
		if(this->points[i].identity==2){
			this->no_mid_points++;
		}
		if(this->points[i].identity==3){
			this->no_right_bank_points++;
		}
	}
	if(this->no_mid_points==0){
		Error msg=this->set_error(2);
		throw msg;
	}
	if (this->no_left_bank_points==0){
		Error msg=this->set_error(3);
		throw msg;
	}
}
//generate for each area (left bank, mid ,right bank) a poly line of river segments
void Hyd_River_Profile_Type_Standard::generate_area_segments(Hyd_Param_Material *material_table){
	//first set the number of needed segments per area
	this->calculate_number_segments();
	//second allocate the area
	try{
		this->allocate_area_segments();
		//third set the segments
		this->set_points2areasegments(material_table);
	}
	catch(Error msg){
		throw msg;
	}

}
//calculate the number of segments for each area
void Hyd_River_Profile_Type_Standard::calculate_number_segments(void){
		
	//left bank
	this->no_left_bank_segments=this->no_left_bank_points+1;
	if(this->no_left_bank_points!=1){
		this->left_bank_flag=true;
	}
	
	//main channel
	this->no_mid_segments=this->no_mid_points+2;

	//right bank
	this->no_right_bank_segments=this->no_right_bank_points+2;
	if(this->no_right_bank_points!=0){
		this->right_bank_flag=true;
	}

	this->total_segment_number=this->no_left_bank_segments+this->no_mid_segments+this->no_right_bank_segments;
}
//allocate the segments for each area
void Hyd_River_Profile_Type_Standard::allocate_area_segments(void){
	this->delete_area_segments();
	try{
		//left bank
		//number of segment is number of points, because of the deviding segment between left bank and mid of river
		this->left_bank_profile=new Hyd_Profile_Segment [this->no_left_bank_segments];

		//main channel
		//number of segment is number of points +1, because of the deviding segment between left bank and mid of river nad the right bank and the mid river
		this->mid_profile=new Hyd_Profile_Segment [this->no_mid_segments];

		//right channel
		//number of segment is number of points +1, because of the deviding segment between left bank and mid of river nad the right bank and the mid river
		this->right_bank_profile=new Hyd_Profile_Segment [this->no_right_bank_segments];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(1);
		throw msg;
	}


}
//Delete the Hyd_Profile_Segment for each area (left, right bank, mid channel)
void Hyd_River_Profile_Type_Standard::delete_area_segments(void){
	if(this->mid_profile!=NULL){
		delete []this->mid_profile;
		this->mid_profile=NULL;
	}
	if(this->left_bank_profile!=NULL){
		delete []this->left_bank_profile;
		this->left_bank_profile=NULL;
	}
	if(this->right_bank_profile!=NULL){
		delete []this->right_bank_profile;
		this->right_bank_profile=NULL;
	}
}
//set the area segments with the given points
void Hyd_River_Profile_Type_Standard::set_points2areasegments(Hyd_Param_Material *material_table){
	int point_counter=0;
	double manning_buffer=0.0;
	//set the poly segments
	//left bank
	//generate the first vertical line
	this->left_bank_profile[0].set_coordinates(this->points[point_counter].get_xcoordinate(), this->points[this->zmax_marker].get_ycoordinate()+this->increment_max_height,this->points[point_counter].get_xcoordinate(), this->points[point_counter].get_ycoordinate());
	point_counter++;
	//the begin of the profile is continued with 2.0 m vertical with the material values of the profile segment after
	try{
		if(this->left_bank_flag==false && this->material_type==_hyd_mat_coefficient_types::poleni_coefficient){
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, _hyd_mat_coefficient_types::poleni_coefficient);
		}
		else{
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, _hyd_mat_coefficient_types::manning_coefficient);
		}
	}
	catch(Error msg){
		throw msg;
	}
	this->left_bank_profile[0].set_profilespecific_members(manning_buffer,1);
	//generate the segments between 
	for(int j=0 ;j<this->no_left_bank_segments-2;j++){
		this->left_bank_profile[j+1].set_points(&this->left_bank_profile[j].point2, &this->points[point_counter]);
		try{
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, _hyd_mat_coefficient_types::manning_coefficient);
		}
		catch(Error msg){
			throw msg;
		}
		this->left_bank_profile[j+1].set_profilespecific_members(manning_buffer,1);
		point_counter++;
	}
	//generate the last vertical line
	this->left_bank_profile[this->no_left_bank_segments-1].set_coordinates(this->left_bank_profile[this->no_left_bank_segments-2].point2.get_xcoordinate(), this->left_bank_profile[this->no_left_bank_segments-2].point2.get_ycoordinate(), this->left_bank_profile[this->no_left_bank_segments-2].point2.get_xcoordinate(), this->left_bank_profile[0].point1.get_ycoordinate()); 
	this->left_bank_profile[this->no_left_bank_segments-1].set_profilespecific_members(1.0/1000.0,0);

	//mid channel
	//generate the first vertical line
	this->mid_profile[0]=this->left_bank_profile[this->no_left_bank_segments-1];
	this->mid_profile[0].switch_points();

	if(this->left_bank_flag==true){
		//80 is the kst value (1/80 =n value) for the interface between water areas; lu is effective 1 
		//this->mid_profile[0].set_profilespecific_members(1.0/80.0,1);
		if(this->user_setting.use_interface==false){
			//without any interface applied in Hec-Ras; lu is not effective 0
			this->mid_profile[0].set_profilespecific_members(0.1,0);
		}
		else{
			//the n-value of the first mid-channel profile (after Bollrich)
			try{
				manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, this->material_type);
			}
			catch(Error msg){
				throw msg;
			}
			this->mid_profile[0].set_profilespecific_members(manning_buffer,1);
		}
	}
	else{
		//there is no left bank: begin is continued with the material values of the segment after
		try{
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, this->material_type);
		}
		catch(Error msg){
			throw msg;
		}
		this->mid_profile[0].set_profilespecific_members(manning_buffer,1);

	}
	//generate segments between
	for(int j=0; j<this->no_mid_segments-2; j++){
		this->mid_profile[j+1].set_points(&this->mid_profile[j].point2,&this->points[point_counter]);
		try{
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, this->material_type);
		}
		catch(Error msg){
			throw msg;
		}
		this->mid_profile[j+1].set_profilespecific_members(manning_buffer,1);
		point_counter++;
	}
	//generate the last vertical line
	this->mid_profile[this->no_mid_segments-1].set_coordinates(this->mid_profile[this->no_mid_segments-2].point2.get_xcoordinate(), this->mid_profile[this->no_mid_segments-2].point2.get_ycoordinate(), this->mid_profile[this->no_mid_segments-2].point2.get_xcoordinate(), this->mid_profile[0].point1.get_ycoordinate()); 
	if(this->right_bank_flag==true){
		//80 is the kst value for the interface between water areas; lu is effective 1 
		//this->mid_profile[this->no_mid_segments-1].set_profilespecific_members(1.0/80.0,1);
		if(this->user_setting.use_interface==false){
			//without any interface applied in Hec-Ras; lu is not effective 0
			this->mid_profile[this->no_mid_segments-1].set_profilespecific_members(0.1,0);
		}
		else{
			// the n-value of the last mid-channel profile (after Bollrich)
			try{
				manning_buffer=material_table->get_mat_value(this->points[point_counter-1].mat_type, this->material_type);
			}
			catch(Error msg){
				throw msg;
			}
			this->mid_profile[this->no_mid_segments-1].set_profilespecific_members(manning_buffer,1);
		}	
	}
	else{

		//there is no right bank: end is continued with the material values of the segment before
		try{
			manning_buffer=material_table->get_mat_value(this->points[point_counter-1].mat_type, this->material_type);
		}
		catch(Error msg){
			throw msg;
		}
		this->mid_profile[this->no_mid_segments-1].set_profilespecific_members(manning_buffer,1);

	}

	//right bank
	this->right_bank_profile[0]=this->mid_profile[this->no_mid_segments-1];
	this->right_bank_profile[0].switch_points();
	//is the interface between water and air: numerical simplification: very smooth surface; lu is not effective 0
	this->right_bank_profile[0].set_profilespecific_members(1.0/1000.0,0);
	//generate segments between
	for(int j=0; j<this->no_right_bank_segments-2; j++){
		this->right_bank_profile[j+1].set_points(&this->right_bank_profile[j].point2,&this->points[point_counter]);
		try{
			manning_buffer=material_table->get_mat_value(this->points[point_counter].mat_type, _hyd_mat_coefficient_types::manning_coefficient);
		}
		catch(Error msg){
			throw msg;
		}
		this->right_bank_profile[j+1].set_profilespecific_members(manning_buffer,1);
		point_counter++;
	}
	//generate the last point
	this->right_bank_profile[this->no_right_bank_segments-1].set_coordinates(this->right_bank_profile[this->no_right_bank_segments-2].point2.get_xcoordinate(), this->right_bank_profile[this->no_right_bank_segments-2].point2.get_ycoordinate(),this->right_bank_profile[this->no_right_bank_segments-2].point2.get_xcoordinate(), this->right_bank_profile[0].point1.get_ycoordinate());
	//the end of the profile is continued with 2.0 m vertical with the material values of the profile segment before
	try{
		if(this->right_bank_flag==false && this->material_type==_hyd_mat_coefficient_types::poleni_coefficient){
			manning_buffer=material_table->get_mat_value(this->points[point_counter-1].mat_type, this->material_type);
		}
		else{
			manning_buffer=material_table->get_mat_value(this->points[point_counter-1].mat_type, _hyd_mat_coefficient_types::manning_coefficient);
		}
	}
	catch(Error msg){
		throw msg;
	}
	
	this->right_bank_profile[this->no_right_bank_segments-1].set_profilespecific_members(manning_buffer,1);
	
	//check if all points are used (!= number_points-1 ??)
	if(point_counter!=this->number_points){
		Error msg =this->set_error(4);
		throw msg;
	}
}
//generate intersected profile with the waterlevel
void Hyd_River_Profile_Type_Standard::generate_intersected_area_segments(const int no_real_segments, Hyd_Profile_Segment *real_segments, int *no_intersect_segments, Hyd_Profile_Segment *(*intersect_segments)){
	//create an interception point
	_geo_interception_point my_interception;
	my_interception.interception_point.set_point_name(label::interception_point);

	int full_segment_counter=0;
	int intercept_segment_counter=0;
	bool wet_flag=false;
	bool new_interception=false;

	//reset the values
	*no_intersect_segments=0;
	if(*intersect_segments!= NULL){
		delete [](*intersect_segments);
		*intersect_segments=NULL;
	}

	//count the necessary segments
	for(int i=0;i < no_real_segments;i++){
		//make the interception with the waterlevel
		real_segments[i].calc_interception(this->waterlevel_line, &my_interception);

		//check if there is no interception
		if(my_interception.interception_flag==false && my_interception.indefinite_flag==false){
			if(wet_flag==false){
				//nothing is counted, segment is completly in the dry zone
			}
			else{//the segment is completly in the wet zone
				full_segment_counter++;
			}
		}

		//check if there is an interception
		if(my_interception.interception_flag==true ){
			//check if full interception or an interception at end points
			if(my_interception.interception_point!=real_segments[i].point1 && my_interception.interception_point!=real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=true;
					intercept_segment_counter++;
					new_interception=true;
				}
				else if(wet_flag==true && my_interception.interception_point.check_below_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=false;
					intercept_segment_counter++;
					new_interception=true;
				}
			}//the first point is intercepted
			else if(my_interception.interception_point==real_segments[i].point1 && my_interception.interception_point!=real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=true;
					intercept_segment_counter++;
					new_interception=true;
				}
				else if(wet_flag==true && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//staying wet
					full_segment_counter++;
				}
			}//the second point is intercepted
			else if(my_interception.interception_point!=real_segments[i].point1 && my_interception.interception_point==real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false){
					//nothing happen a first point will intercepted in the next step;
				}
				else if(wet_flag==true){
					if(i<no_real_segments-1){
						if( my_interception.interception_point.check_below_of(&real_segments[i+1].point2)){
							//change from dry to wet
							wet_flag=false;
							intercept_segment_counter++;
							new_interception=true;
						}
						else if(wet_flag==true && my_interception.interception_point.check_above_of(&real_segments[i+1].point2)){
							//staying wet
							full_segment_counter++;
						}
						else if(wet_flag==true){//horizontal
							//staying wet
							full_segment_counter++;
						}
					}
					else{//the last segment is intercepted by the second point
						//change from dry to wet
						wet_flag=false;
						intercept_segment_counter++;
						new_interception=true;
					}
				}
			}
		}
		//horizontal interception
		else if(my_interception.interception_flag==false && my_interception.indefinite_flag==true){
			if(wet_flag==true){
				if(i<no_real_segments-1){
					if(real_segments[i].point2.check_below_of(&real_segments[i+1].point2)){
						//change from dry to wet
						wet_flag=false;
						intercept_segment_counter++;
						new_interception=true;
					}
					else if(real_segments[i].point2.check_above_of(&real_segments[i+1].point2)){
						//staying wet
						full_segment_counter++;	
					}
					else{//next is also horizontal
						//staying wet
						full_segment_counter++;	
					}
				}
				else{
					//change from dry to wet
					wet_flag=false;
					intercept_segment_counter++;
					new_interception=true;
				}
			}
			else{//dry
				//nothing happen
			}

		}

		//count earth segment
		if(new_interception==true){
			//set waterlevel segment and earth segment
			if(intercept_segment_counter%2!=0 && intercept_segment_counter>1){
				full_segment_counter++;
			}
		}
		new_interception=false;
	}//end segment counting

	//number of needed segments
	////allocate the new intercepted segments
	if(intercept_segment_counter>0){
		*no_intersect_segments=(intercept_segment_counter-1)*2+full_segment_counter+1;
	}
	else{
		*no_intersect_segments=0;
		return;
	}
	try{
		*intersect_segments=new Hyd_Profile_Segment[*no_intersect_segments];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(5);
		throw msg;
	}

	//start segment setting
	int segment_set_counter=0;
	int segment_waterline_set_counter=*no_intersect_segments-1;
	Geo_Point dummy1;
	Geo_Point dummy2;
	new_interception=false;
	//first reset 
	full_segment_counter=0;
	intercept_segment_counter=0;
	wet_flag=false;

	for(int i=0;i < no_real_segments;i++){
		//make the interception with the waterlevel
		real_segments[i].calc_interception(this->waterlevel_line, &my_interception);

		//check if there is no interception
		if(my_interception.interception_flag==false && my_interception.indefinite_flag==false){
			if(wet_flag==false){
				//nothing is counted, segment is completly in the dry zone
			}
			else{//the segment is completly in the wet zone
				(*intersect_segments)[segment_set_counter]=real_segments[i];
				segment_set_counter++;
				full_segment_counter++;
			}
		}

		//check if there is an interception
		if(my_interception.interception_flag==true ){
			//check if full interception or an interception at end points
			if(my_interception.interception_point!=real_segments[i].point1 && my_interception.interception_point!=real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=true;
					(*intersect_segments)[segment_set_counter]=real_segments[i];
					(*intersect_segments)[segment_set_counter].set_points(&my_interception.interception_point,&((*intersect_segments)[segment_set_counter].point2));
					(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
					dummy1=my_interception.interception_point;
					segment_set_counter++;
					intercept_segment_counter++;
					new_interception=true;
				}
				else if(wet_flag==true && my_interception.interception_point.check_below_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=false;
					(*intersect_segments)[segment_set_counter]=real_segments[i];
					(*intersect_segments)[segment_set_counter].set_points(&((*intersect_segments)[segment_set_counter].point1), &my_interception.interception_point);
					(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
					dummy2=my_interception.interception_point;
					segment_set_counter++;
					intercept_segment_counter++;
					new_interception=true;
				}
			}//the first point is intercepted
			else if(my_interception.interception_point==real_segments[i].point1 && my_interception.interception_point!=real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//change from dry to wet
					wet_flag=true;
					(*intersect_segments)[segment_set_counter]=real_segments[i];
					(*intersect_segments)[segment_set_counter].set_points(&my_interception.interception_point,&((*intersect_segments)[segment_set_counter].point2));
					(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
					dummy1=my_interception.interception_point;
					segment_set_counter++;
					intercept_segment_counter++;
					new_interception=true;
				}
				else if(wet_flag==true && my_interception.interception_point.check_above_of(&real_segments[i].point2)){
					//staying wet
					(*intersect_segments)[segment_set_counter]=real_segments[i];
					segment_set_counter++;
					full_segment_counter++;
				}
			}//the second point is intercepted
			else if(my_interception.interception_point!=real_segments[i].point1 && my_interception.interception_point==real_segments[i].point2){
				//check which zone (dry/wet) and the relativ position of the second point to the interception point
				if(wet_flag==false){
					//nothing happen a first point will intercepted in the next step;
				}
				else if(wet_flag==true){
					if(i<no_real_segments-1){
						if( my_interception.interception_point.check_below_of(&real_segments[i+1].point2)){
							//change from dry to wet
							wet_flag=false;
							(*intersect_segments)[segment_set_counter]=real_segments[i];
							(*intersect_segments)[segment_set_counter].set_points(&((*intersect_segments)[segment_set_counter].point1), &my_interception.interception_point);
							(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
							dummy2=my_interception.interception_point;
							segment_set_counter++;
							intercept_segment_counter++;
							new_interception=true;
						}
						else if(wet_flag==true && my_interception.interception_point.check_above_of(&real_segments[i+1].point2)){
							//staying wet
							(*intersect_segments)[segment_set_counter]=real_segments[i];
							segment_set_counter++;
							full_segment_counter++;
						}
						else if(wet_flag==true){//horizontal
							//staying wet
							(*intersect_segments)[segment_set_counter]=real_segments[i];
							segment_set_counter++;
							full_segment_counter++;
						}
					}
					else{//the last segment is intercepted by the second point
						//change from dry to wet
						wet_flag=false;
						(*intersect_segments)[segment_set_counter]=real_segments[i];
						(*intersect_segments)[segment_set_counter].set_points(&((*intersect_segments)[segment_set_counter].point1), &my_interception.interception_point);
						(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
						dummy2=my_interception.interception_point;
						segment_set_counter++;
						intercept_segment_counter++;
						new_interception=true;
					}
				}
			}
		}
		//horizontal interception
		else if(my_interception.interception_flag==false && my_interception.indefinite_flag==true){
			if(wet_flag==true){
				if(i<no_real_segments-1){
					if(real_segments[i].point2.check_below_of(&real_segments[i+1].point2)){
						//change from dry to wet
						wet_flag=false;
						(*intersect_segments)[segment_set_counter]=real_segments[i];
						(*intersect_segments)[segment_set_counter].set_profilespecific_members(0.0,0);
						(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
						dummy2=my_interception.interception_point;
						segment_set_counter++;
						intercept_segment_counter++;
						new_interception=true;
					}
					else if(real_segments[i].point2.check_above_of(&real_segments[i+1].point2)){
						//staying wet
						(*intersect_segments)[segment_set_counter]=real_segments[i];
						(*intersect_segments)[segment_set_counter].set_profilespecific_members(0.0,0);
						(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
						segment_set_counter++;
						full_segment_counter++;	
					}
					else{//next is also horizontal
						//staying wet
						(*intersect_segments)[segment_set_counter]=real_segments[i];
						(*intersect_segments)[segment_set_counter].set_profilespecific_members(0.0,0);
						(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
						segment_set_counter++;
						full_segment_counter++;	
					}
				}
				else{
					//change from dry to wet
					wet_flag=false;
					(*intersect_segments)[segment_set_counter]=real_segments[i];
					(*intersect_segments)[segment_set_counter].set_profilespecific_members(0.0,0);
					(*intersect_segments)[segment_set_counter].recalc_profilespecific_members();
					dummy2=my_interception.interception_point;
					segment_set_counter++;
					intercept_segment_counter++;
					new_interception=true;
				}
			}
			else{//dry
				//nothing happen
			}

		}

		//set the waterline segment
		if(new_interception==true){
			//set waterlevel segment and earth segment
			if(intercept_segment_counter%2!=0 && intercept_segment_counter>1){
				Hyd_Profile_Segment buffer;
				buffer=(*intersect_segments)[segment_set_counter-1];
				(*intersect_segments)[segment_set_counter-1].set_points(&dummy2,&dummy1);
				(*intersect_segments)[segment_set_counter-1].set_profilespecific_members(0.0,0);
				(*intersect_segments)[segment_set_counter-1].recalc_profilespecific_members();
				(*intersect_segments)[segment_set_counter]=buffer;
				segment_set_counter++;
				(*intersect_segments)[segment_waterline_set_counter].set_points(&dummy1, &dummy2);
				(*intersect_segments)[segment_waterline_set_counter].set_profilespecific_members(0.0,0);
				(*intersect_segments)[segment_waterline_set_counter].recalc_profilespecific_members();
				segment_waterline_set_counter--;
			}
			else if(intercept_segment_counter%2==0 && intercept_segment_counter>1){
				(*intersect_segments)[segment_waterline_set_counter].set_points(&dummy2, &dummy1);
				(*intersect_segments)[segment_waterline_set_counter].set_profilespecific_members(0.0,0);
				(*intersect_segments)[segment_waterline_set_counter].recalc_profilespecific_members();
				segment_waterline_set_counter--;
			}
		}
	
		new_interception=false;
	}//end segment setting
} 
//calculate the table values
void Hyd_River_Profile_Type_Standard::calculate_table_values(const double delta_x){
	//set first values
	this->area_table.set_values(0,0.0);
	this->width_table.set_values(0, 0.0);
	this->c_left_table.set_values(0,0.0);
	this->c_mid_table.set_values(0,0.0);
	this->c_right_table.set_values(0,0.0);
	this->h_table.set_values(0,0.0+this->global_z_min);
	//allocate the waterlevel
	try{
		this->waterlevel_line=new Geo_Straight_Line;
	}
	catch(bad_alloc &){
		Error msg=this->set_error(1);
		throw msg;
	}

	//intersected segments
	int no_intersected_segments=0;
	Hyd_Profile_Segment *intersected_segments=NULL;
	double area_left=0.0;
	double area_mid=0.0;
	double area_right=0.0;
	double width = 0.0;

	//this->generate_plot(NULL);

	ostringstream cout;//test
	for(int i=1; i<this->number_table_points-1;i++){//loop over waterlevel
		//generate the waterlevel line
		this->waterlevel_line->set_coordinates(0.0,(double)i*delta_x);
		//set the waterlevel to table
		this->h_table.set_values(i,(double)i*delta_x+this->global_z_min);

	
		

		//left bank
		if(this->left_bank_profile[0]!=this->left_bank_profile[1]){
			this->generate_intersected_area_segments(this->no_left_bank_segments, this->left_bank_profile, &no_intersected_segments, &intersected_segments);
		}
		else{
			no_intersected_segments=0;
		}
		//calculate the area and width
		if(no_intersected_segments!=0){
			area_left=this->calculate_area2table(no_intersected_segments, intersected_segments);
			this->c_left_table.set_values(i, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_left));
			width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
		}

		//main channel
		if(this->mid_profile[0]!=this->mid_profile[1]){
			this->generate_intersected_area_segments(this->no_mid_segments, this->mid_profile, &no_intersected_segments, &intersected_segments);
		}
		else{
			no_intersected_segments=0;
		}
		//calculate the area and width
		if(no_intersected_segments!=0){
			area_mid=this->calculate_area2table(no_intersected_segments, intersected_segments);
			this->c_mid_table.set_values(i, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_mid));
			width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
		}
		//for develepment and check the calculated area with a plot
		/*HydGui_Profile_Plot test;
		test.set_number_curves(1);
		test.set_curves(0, "mid_intersect" , no_intersected_segments, Qt::blue, intersected_segments); 
		test.show_plot();*/


		//right bank
		if(this->right_bank_profile[0]!=this->right_bank_profile[1]){
			this->generate_intersected_area_segments(this->no_right_bank_segments, this->right_bank_profile, &no_intersected_segments, &intersected_segments);
		}
		else{
			no_intersected_segments=0;
		}
		//calculate the area and width
		if(no_intersected_segments!=0){
			area_right=this->calculate_area2table(no_intersected_segments, intersected_segments);
			this->c_right_table.set_values(i, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_right));
			width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
		}

		//area to table
		this->area_table.set_values(i, area_left+area_mid+area_right);
		//reset local area
		area_left=0.0;
		area_mid=0.0;
		area_right=0.0;
		//width to table
		this->width_table.set_values(i, width);
		//reset width
		width = 0.0;

	

	}

	

	//make the last calculation for the last table point
	//generate the waterlevel line
	this->waterlevel_line->set_coordinates(0.0,this->mid_profile[0].point1.get_ycoordinate()-constant::meter_epsilon*2.0);
	//set the waterlevel to table
	this->h_table.set_values(this->number_table_points-1,(this->mid_profile[0].point1.get_ycoordinate()-constant::meter_epsilon)+this->global_z_min);
	
	//left bank
	if(this->left_bank_profile[0]!=this->left_bank_profile[1]){
		this->generate_intersected_area_segments(this->no_left_bank_segments, this->left_bank_profile, &no_intersected_segments, &intersected_segments);
	}
	else{
		no_intersected_segments=0;
	}
	if(no_intersected_segments!=0){
		area_left=this->calculate_area2table(no_intersected_segments, intersected_segments);
		this->c_left_table.set_values(this->number_table_points-1, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_left));
		width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
	}

	//main channel
	if(this->mid_profile[0]!=this->mid_profile[1]){
		this->generate_intersected_area_segments(this->no_mid_segments, this->mid_profile, &no_intersected_segments, &intersected_segments);
	}
	else{
		no_intersected_segments=0;
	}
	if(no_intersected_segments!=0){
		area_mid=this->calculate_area2table(no_intersected_segments, intersected_segments);
		this->c_mid_table.set_values(this->number_table_points-1, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_mid));
		width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
	}

	//right bank
	if(this->right_bank_profile[0]!=this->right_bank_profile[1]){
		this->generate_intersected_area_segments(this->no_right_bank_segments, this->right_bank_profile, &no_intersected_segments, &intersected_segments);
	}
	else{
		no_intersected_segments=0;
	}
	if(no_intersected_segments!=0){
		area_right=this->calculate_area2table(no_intersected_segments, intersected_segments);
		this->c_right_table.set_values(this->number_table_points-1, this->calculate_conveyance2table(no_intersected_segments, intersected_segments, area_right));
		width = width + this->calculate_width2table(no_intersected_segments, intersected_segments);
	}

	//set the area
	this->area_table.set_values(this->number_table_points-1, area_left+area_mid+area_right);

	this->width_table.set_values(this->number_table_points - 1, width);
	

	//delete the waterlevel
	if(this->waterlevel_line!=NULL){
		delete this->waterlevel_line;
		this->waterlevel_line=NULL;	
	}
	if(intersected_segments!=NULL){
		delete []intersected_segments;
		intersected_segments=NULL;	
	}

}
//calculate the area with the generated Hyd_Profile_Segment
double Hyd_River_Profile_Type_Standard::calculate_area2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments){
	double area=0.0;

	for(int i=0; i<no_intersect_segments-1; i++){
		area=area+(intersect_segments[i].point1.get_xcoordinate()-intersect_segments[i+1].point1.get_xcoordinate())*(intersect_segments[i].point1.get_ycoordinate()+intersect_segments[i+1].point1.get_ycoordinate());
	}
	area = area + (intersect_segments[no_intersect_segments-1].point1.get_xcoordinate()-intersect_segments[0].point1.get_xcoordinate())*(intersect_segments[no_intersect_segments-1].point1.get_ycoordinate()+intersect_segments[0].point1.get_ycoordinate());


	return area/2.0;
}
//Calculate the width with the generated Hyd_Profile_Segment for the table generation
double Hyd_River_Profile_Type_Standard::calculate_width2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments) {
	double width = 0.0;
	if (no_intersect_segments > 1) {
		width = intersect_segments[no_intersect_segments - 1].point1.get_xcoordinate() - intersect_segments[0].point1.get_xcoordinate();
	}
	
	return width;


}
//calculate hydraulic_radius
double Hyd_River_Profile_Type_Standard::calculate_hydraulic_radius2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments){
	double lu=0.0;
	for(int i=0; i<no_intersect_segments; i++){
		lu=lu+intersect_segments[i].get_lu();
	}
	return lu;
}
//calculate conveyance with the
double Hyd_River_Profile_Type_Standard::calculate_conveyance2table(const int no_intersect_segments, Hyd_Profile_Segment *intersect_segments, const double area){
	double conveyance=0.0;
	//calculate the wetted perimeter
	double lu = this->calculate_hydraulic_radius2table(no_intersect_segments, intersect_segments);
	//calculate the mean of manning value (formula after Einstein)
	double mean_n=0.0;
	for(int i=0; i<no_intersect_segments; i++){
		mean_n=mean_n+intersect_segments[i].get_luXn_value();
	}
	mean_n=1.0/pow((lu/mean_n),(2.0/3.0));
	//calculate conveyance:= 1/manning*area*(hydraulic radius )^(2/3); hydraulic radius:= area/wetted perimeter;
	double hyd_radius=area/lu;
	conveyance=1.0/mean_n*area*pow(hyd_radius,(2.0/3.0));

	return conveyance;
}
//set the error
Error Hyd_River_Profile_Type_Standard::set_error(const int err_type){
		string place="Hyd_River_Profile_Type_Standard::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("calc_alloc_tables(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad alloc
			place.append("generate_poly_river_segments(void)/calculate_table_values(const double delta_x)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://no mid profile is found
			place.append("count_area_points(void)");
			reason="No main channel point(ident=2) is found";
			help="Check the profile";
			type=11;
			break;
		case 3://no left bank point found - needed as starting point
			place.append("count_area_points(void)");
			reason="No left bank point (ident = 1) is found - Needed as starting point!";
			help="Check the profile";
			type=11;
			break;
		case 4://not all points are used to set profile segments
			place.append("set_points2areasegments(void)");
			reason="Not all points are used to set profile segments";
			help="Check the profile";
			type=11;
			break;
		case 5://bad alloc
			place.append("generate_intersected_area_segments(const int no_real_segments, Hyd_Profile_Segment *real_segments, int *no_intersect_segments, Hyd_Profile_Segment *intersect_segments)");
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