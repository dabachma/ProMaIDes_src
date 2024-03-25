#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_River_Profile_Type_Bridge.h"

//constructor
Hyd_River_Profile_Type_Bridge::Hyd_River_Profile_Type_Bridge(void){
	this->bridge_data.bridgebody_size=0.0;
	this->bridge_data.local_bridge_height=0.0;
	this->global_bridge_height=0.0;
	this->pressure_flow_area=0.0;
	this->flag_pressure_flow=false;
	this->weir_flow_area=0.0;
	this->upper_bridge_area=0.0;
	this->flow_v_under=0.0;
	this->q_under=0.0;
	this->flow_v_over=0.0;
	this->q_over=0.0;
	this->press_flow_data=NULL;
	//this->fixed_normal_q=1.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Type_Bridge)-sizeof(Hyd_River_Profile_Type_Standard), _sys_system_modules::HYD_SYS);

}
//destructor
Hyd_River_Profile_Type_Bridge::~Hyd_River_Profile_Type_Bridge(void){
	if(this->press_flow_data!=NULL){
		delete []this->press_flow_data;
		this->press_flow_data=NULL;
	}
		//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Type_Bridge)-sizeof(Hyd_River_Profile_Type_Standard), _sys_system_modules::HYD_SYS);
}
//__________
//public
//Set the vertical size of the bridge body (here the profile specific value)
void Hyd_River_Profile_Type_Bridge::set_profile_specific_value(const _hyd_bridge_values data){
	this->bridge_data=data;
}
//Get the convenyance factor total; it is limited by the bridge height
double Hyd_River_Profile_Type_Bridge::get_c_value(void){

	if(this->flag_pressure_flow==false){
		return this->c_mid_value+this->c_left_value+this->c_right_value;
	}
	else{

		return (this->c_mid_table.get_interpolated_values(this->pressure_flow_area)+this->c_left_table.get_interpolated_values(this->pressure_flow_area)+
			this->c_right_table.get_interpolated_values(this->pressure_flow_area));
	}
}
//Get the bridge data specific values
_hyd_bridge_values Hyd_River_Profile_Type_Bridge::get_bridge_data(void){
	return this->bridge_data;
}
//Get the bridge data as a geometrie for a plot
int Hyd_River_Profile_Type_Bridge::get_bridge_data2plot(double **x, double **z){
	int no=5;
	try{
		(*x)=new double[no];
		(*z)=new double[no];

	}
	catch(bad_alloc &t){
		Error msg=this->set_error(8);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	(*x)[0]=this->get_first_point()->get_xcoordinate();
	(*z)[0]=this->bridge_data.local_bridge_height;
	(*x)[1]=this->get_last_point()->get_xcoordinate();
	(*z)[1]=this->bridge_data.local_bridge_height;
	(*x)[2]=this->get_last_point()->get_xcoordinate();
	(*z)[2]=this->bridge_data.local_bridge_height+this->bridge_data.bridgebody_size;
	(*x)[3]=this->get_first_point()->get_xcoordinate();
	(*z)[3]=this->bridge_data.local_bridge_height+this->bridge_data.bridgebody_size;
	(*x)[4]=this->get_first_point()->get_xcoordinate();
	(*z)[4]=this->bridge_data.local_bridge_height;

	return no;
}
//out the profile type members
void Hyd_River_Profile_Type_Bridge::output_members(ostringstream *cout){
	
	*cout << " Number of points " << this->number_points << endl;
	*cout << " Globals: zmin_z " << P(2) << FORMAT_FIXED_REAL << this->global_z_min << label::m;
	*cout << "; zmin_x " << P(2) << FORMAT_FIXED_REAL <<this->global_x_zmin <<label::m; 
	*cout << "; zmin_y " << P(2) << FORMAT_FIXED_REAL << this->global_y_zmin << label::m << endl;
	*cout << " Globals: zmax_z " << P(2) << FORMAT_FIXED_REAL << this->global_z_max <<label::m;
	*cout << "; zmax_x " << P(2) << FORMAT_FIXED_REAL << this->global_x_zmax <<label::m;
	*cout << "; zmax_y " << P(2) << FORMAT_FIXED_REAL << this->global_y_zmax << label::m << endl;	
	*cout << " Bridge: hlocal " << this->bridge_data.local_bridge_height << label::m << "; body size " << this->bridge_data.bridgebody_size << label::m << endl;
	*cout << " The points are... " << endl;
	*cout << W(8) << "No." << W(12)<<"x(glob)" << label::m<< W(12) <<"y(glob)" << label::m;
	*cout << W(12) <<"z(glob)" << label::m <<W(10) <<"x(loc)" <<label::m;
	*cout <<W(10) <<"z(loc)" <<label::m <<W(10) <<"mattype" <<label::no_unit<< W(10) <<"ident"<<label::no_unit<< endl;
	for(int i=0 ; i< this->number_points ;i++){
		this->points[i].output_members(cout);
	}
}
//Calculate and allocate the tables (Hyd_Tables): area, waterlevel, convenyance for left bank, right bank and mid channel; set the bridge height and width
void Hyd_River_Profile_Type_Bridge::calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table){
	//calculate the standard tables
	try{
		Hyd_River_Profile_Type_Standard::calc_alloc_tables(delta_x, material_table);
		//check for overhanging segments
		this->check_bridge_segments();
	}
	catch(Error msg){
		throw msg;
	}
	//calculate the global bridge height
	this->global_bridge_height=this->global_z_min+this->bridge_data.local_bridge_height;
	
	//check if the bridge intercepts the profile at each side
	if(this->global_bridge_height-this->get_first_point()->get_global_z_coordinate()>=constant::meter_epsilon || this->global_bridge_height-this->get_last_point()->get_global_z_coordinate()>=constant::meter_epsilon){
		Error msg=this->set_error(2);
		throw msg;
	}

	//calculate the pressure flow area
	this->pressure_flow_area=this->area_table.get_interpolated_values(this->global_bridge_height);
	//calculate the area of the profile to the upper bridge brink
	this->upper_bridge_area=this->area_table.get_interpolated_values(this->global_bridge_height+this->bridge_data.bridgebody_size);
	//set a first gues for the fixed flow
	//this->fixed_normal_q=this->pressure_flow_area;

	try{
		//allocate and calculate the pressure flow data
		this->alloc_calc_pressure_flow_data();
	}
	catch(Error msg){
		throw msg;
	}
}
//Calculate the actual discharge through this profile by a given upstream and downstream profile
double Hyd_River_Profile_Type_Bridge::calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream){
	double discharge=0.0;
	this->q_over=0.0;
	this->flow_v_over=0.0;
	//reset the weir flow area
	this->weir_flow_area=0.0;

	double h_upstream=upstream_profile->get_actual_global_waterlevel();
	double delta1=h_upstream-this->global_bridge_height;
	double delta2=this->s_value-this->global_bridge_height;

	//no flow
	if(abs(h_upstream-this->s_value)<constant::flow_epsilon){
		discharge=0.0;
		this->q_under=0.0;
		this->flow_v_under=0.0;
		this->v_value=0.0;
		this->flag_pressure_flow=false;
	}
	//normal manning flow
	else if(delta1<=0.0 && delta2<=0.0){
		this->q_under=Hyd_River_Profile_Type_Standard::calculate_actual_discharge(upstream_profile, downstream_profile, distance_upstream);
		discharge=this->q_under;
		this->flow_v_under=this->v_value;
		this->flag_pressure_flow=false;
		//this->fixed_normal_q=discharge;
	}
	//flow to downstream as pressure flow
	else if(h_upstream>this->s_value){

		//transition zone between manning flow and pressure flow
		if(delta1>0.0 && delta1<=constant::min_seize_bridge){
			//this->q_under=(1.0-(delta1/constant::min_seize_bridge))*this->fixed_normal_q;	
			this->q_under=(1.0-(delta1/constant::min_seize_bridge))*Hyd_River_Profile_Type_Standard::calculate_actual_discharge(upstream_profile, downstream_profile, distance_upstream);
			
			this->q_under=this->q_under+(delta1/constant::min_seize_bridge)*this->calculate_pressure_flow(h_upstream, this->s_value);
			discharge=this->q_under;
			this->flow_v_under=(this->q_under/this->pressure_flow_area);
			this->area2calc=this->pressure_flow_area;
		}
		//total pressure flow
		else if(delta1>0.0 && delta1>constant::min_seize_bridge){
			this->q_under=this->calculate_pressure_flow(h_upstream, this->s_value);
			discharge=this->q_under;
			this->flow_v_under=(this->q_under/this->pressure_flow_area);
			this->area2calc=this->pressure_flow_area;
		}
		

		//add weir discharge to pressure flow
		if(h_upstream>this->global_bridge_height+this->bridge_data.bridgebody_size){
			//add a weir discharge to the pressure discharge
			this->q_over=this->calculate_weir_flow(h_upstream, this->s_value);
			discharge=discharge+this->q_over;
			this->flow_v_over=(this->q_over/this->weir_flow_area);
		}
		//no weir flow
		else{
			this->q_over=0.0;
			this->flow_v_over=0.0;
		}

		//calculate the mid flow velocity
		this->v_value=(discharge/(this->pressure_flow_area));
		this->flag_pressure_flow=true;

	}
	//flow to upstream as pressure flow
	else if(h_upstream<this->s_value){
		//transition zone between manning flow and pressure flow
		if(delta2>0.0 && delta2<=constant::min_seize_bridge){
			//this->q_under=(1.0-(delta2/constant::min_seize_bridge))*this->fixed_normal_q;
			this->q_under=(1.0-(delta2/constant::min_seize_bridge))*Hyd_River_Profile_Type_Standard::calculate_actual_discharge(upstream_profile, downstream_profile, distance_upstream);
			
			this->q_under=this->q_under+(delta2/constant::min_seize_bridge)*(-1.0)*this->calculate_pressure_flow(this->s_value, h_upstream);
			//this->q_under=(-1.0)*this->q_under;
			discharge=this->q_under;
			this->flow_v_under=(this->q_under/this->pressure_flow_area);
			this->area2calc=this->pressure_flow_area;	
		}
		//total pressure flow
		else if(delta2>0.0 && delta2>constant::min_seize_bridge){
			this->q_under=(-1.0)*this->calculate_pressure_flow(this->s_value, h_upstream);
			discharge=this->q_under;
			this->flow_v_under=(this->q_under/this->pressure_flow_area);
			this->area2calc=this->pressure_flow_area;
		}
		

		//add weir discharge to pressure flow
		if(this->s_value>this->global_bridge_height+this->bridge_data.bridgebody_size){
			this->q_over=(-1.0)*this->calculate_weir_flow( this->s_value, h_upstream);
			discharge=discharge+this->q_over;
			this->flow_v_over=(this->q_over/this->weir_flow_area);
		}
		//no weir flow
		else{
			this->q_over=0.0;
			this->flow_v_over=0.0;
		}

		//calculate the mid flow velocity
		this->v_value=(discharge/(this->pressure_flow_area));
		this->flag_pressure_flow=true;

	}

	
	//set froude-number to one in case of pressure flow
	if(this->flag_pressure_flow==true){
		this->froude_no=1.0;
	}


	
	return discharge;
}
//Reset the maximum values and the result
void Hyd_River_Profile_Type_Bridge::reset_result_max_values(void){
	_Hyd_River_Profile_Type::reset_result_max_values();
	this->flag_pressure_flow=false;
	this->weir_flow_area=0.0;
	this->flow_v_under=0.0;
	this->q_under=0.0;
	this->flow_v_over=0.0;
	this->q_over=0.0;
	//this->fixed_normal_q=this->pressure_flow_area;

}
//Clone river profile type
void Hyd_River_Profile_Type_Bridge::clone_profile_type(_Hyd_River_Profile_Type *prof_type){
	Hyd_River_Profile_Type_Standard::clone_profile_type(prof_type);

	Hyd_River_Profile_Type_Bridge *buffer=(Hyd_River_Profile_Type_Bridge *)prof_type;

	this->bridge_data=buffer->bridge_data;
	this->global_bridge_height=buffer->global_bridge_height;
	this->pressure_flow_area=buffer->pressure_flow_area;
	this->upper_bridge_area=buffer->upper_bridge_area;
	//this->fixed_normal_q=this->pressure_flow_area;
	
	try{
		this->press_flow_data = new _hyd_pressure_flow_param[this->total_segment_number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->total_segment_number; i++){
		this->press_flow_data[i]=buffer->press_flow_data[i];
	}
}
//____________
//private
//Check the points and the areatypes of consistency and check the bridge specific data
void Hyd_River_Profile_Type_Bridge::check_points(const string name, const int number){
	try{
		_Hyd_River_Profile_Type::check_points(name, number);
	}
	catch(Error msg){
		throw msg;
	}
	//check if the local bridge height is below the minum local bridge height
	if(this->bridge_data.local_bridge_height<constant::min_height_bridge){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Given local bridge height " << this->bridge_data.local_bridge_height << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;

	}
	//check if the bridge body size below the minimal bridge seize
	if(this->bridge_data.bridgebody_size<constant::min_seize_bridge){
		Error msg=this->set_error(1);
		ostringstream info;
		info << "Given bridgebody size " << this->bridge_data.bridgebody_size << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}	
}
//check the segments of the profile
void Hyd_River_Profile_Type_Bridge::check_bridge_segments(void){


	//left bank
	if(this->left_bank_flag==true){
		for (int i=0; i<this->no_left_bank_segments; i++){
			if ( this->left_bank_profile[i].point1.check_right_of(&this->left_bank_profile[i].point2)==true){
				Error msg;
				msg = this->set_error(4);
				ostringstream info;
				info<<"Overhang is at the left bank" << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}

	//mid channel
	for (int i=0; i<this->no_mid_segments; i++){
		if ( this->mid_profile[i].point1.check_right_of(&this->mid_profile[i].point2)==true){
			Error msg;
			msg = this->set_error(4);
			ostringstream info;
			info<<"Overhang is at the mid channel" << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	//right bank
	if(this->right_bank_flag==true){
		for (int i=0; i<this->no_right_bank_segments; i++){
			if ( this->right_bank_profile[i].point1.check_right_of(&this->right_bank_profile[i].point2)==true){
				Error msg;
				msg = this->set_error(4);
				ostringstream info;
				info<<"Overhang is at the right bank" << endl;
				msg.make_second_info(info.str());
				throw msg;
			}
		}
	}
}
//Allocate and calculate the pressure flow data for each river profile segment
void Hyd_River_Profile_Type_Bridge::alloc_calc_pressure_flow_data(void){
	//allocate them
	try{

		this->press_flow_data = new _hyd_pressure_flow_param[this->total_segment_number];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(3);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//init them
	for(int i=0; i< this->total_segment_number; i++){
		this->press_flow_data[i].opening_height=0.0;
		this->press_flow_data[i].opening_width=0.0;
		this->press_flow_data[i].sill_height=0.0;
		this->press_flow_data[i].applied_flag=false;

	}
	//calculate them
	int counter=0;
	//left bank
	for(int i=0; i< this->no_left_bank_segments; i++){
		if(this->left_bank_profile[i].get_vertical_flag()==false){
			//segment is completly above
			if(this->left_bank_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->left_bank_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=false;
			}
			//segement is completly under the bridge
			else if(this->left_bank_profile[i].point1.get_ycoordinate()<this->bridge_data.local_bridge_height && this->left_bank_profile[i].point2.get_ycoordinate()<this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->left_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->left_bank_profile[i].point1.get_ycoordinate()+0.5*this->left_bank_profile[i].point2.get_ycoordinate();
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->left_bank_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->left_bank_profile[i].point2.get_ycoordinate()<=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->left_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->left_bank_profile[i].point2.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->left_bank_profile[i].point1.get_ycoordinate()<=this->bridge_data.local_bridge_height && this->left_bank_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->left_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->left_bank_profile[i].point1.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;	
			}
		}
		counter++;
	}
	//mid channel
	for(int i=0; i< this->no_mid_segments; i++){
		if(this->mid_profile[i].get_vertical_flag()==false){
			//segment is completly above
			if(this->mid_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->mid_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=false;
			}
			//segement is completly under the bridge
			else if(this->mid_profile[i].point1.get_ycoordinate()<this->bridge_data.local_bridge_height && this->mid_profile[i].point2.get_ycoordinate()<this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->mid_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->mid_profile[i].point1.get_ycoordinate()+0.5*this->mid_profile[i].point2.get_ycoordinate();
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->mid_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->mid_profile[i].point2.get_ycoordinate()<=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->mid_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->mid_profile[i].point2.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->mid_profile[i].point1.get_ycoordinate()<=this->bridge_data.local_bridge_height && this->mid_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->mid_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->mid_profile[i].point1.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
		}
		counter++;
	}
	//right bank
	for(int i=0; i< this->no_right_bank_segments; i++){
		if(this->right_bank_profile[i].get_vertical_flag()==false){
			
			//segment is completly above
			if(this->right_bank_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->right_bank_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=false;
			}
			//segement is completly under the bridge
			else if(this->right_bank_profile[i].point1.get_ycoordinate()<this->bridge_data.local_bridge_height && this->right_bank_profile[i].point2.get_ycoordinate()<this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->right_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->right_bank_profile[i].point1.get_ycoordinate()+0.5*this->right_bank_profile[i].point2.get_ycoordinate();
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->right_bank_profile[i].point1.get_ycoordinate()>=this->bridge_data.local_bridge_height && this->right_bank_profile[i].point2.get_ycoordinate()<=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->right_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->right_bank_profile[i].point2.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
			//intercepted by the bridge
			else if(this->right_bank_profile[i].point1.get_ycoordinate()<=this->bridge_data.local_bridge_height && this->right_bank_profile[i].point2.get_ycoordinate()>=this->bridge_data.local_bridge_height){
				this->press_flow_data[counter].applied_flag=true;
				this->press_flow_data[counter].opening_width=this->right_bank_profile[i].get_horizontal_dist();
				this->press_flow_data[counter].sill_height=0.5*this->right_bank_profile[i].point1.get_ycoordinate()+0.5*this->bridge_data.local_bridge_height;
				this->press_flow_data[counter].sill_height=this->press_flow_data[counter].sill_height+this->global_z_min;
				this->press_flow_data[counter].opening_height=this->global_bridge_height-this->press_flow_data[counter].sill_height;
			}
		}
		counter++;
	}
}
//Calculate the discharge via pressure flow (gate formula)
double Hyd_River_Profile_Type_Bridge::calculate_pressure_flow(const double flow_h, const double reducing_h){
	double q_buff=0.0;
	double discharge=0.0;
	double contraction_coeff=0.0;
	double flow_coeff=0.0;
	double reduction_fac=0.0;
	double h_local_before=0.0;
	double h_local_after=0.0;

	//ostringstream cout;

	for(int i=0; i<this->total_segment_number; i++){
		q_buff=0.0;
		if(this->press_flow_data[i].applied_flag==true){
			h_local_before=flow_h-this->press_flow_data[i].sill_height;
			if(h_local_before>0.0 && this->press_flow_data[i].opening_height>0.0 ){
				
				flow_coeff=this->calculate_flow_coefficient(h_local_before,this->press_flow_data[i].opening_height);
				q_buff=this->press_flow_data[i].opening_height*this->press_flow_data[i].opening_width*flow_coeff*pow((2.0*constant::Cgg*h_local_before),0.5);
				contraction_coeff=this->calculate_contraction_factor(h_local_before, this->press_flow_data[i].opening_height, flow_coeff);
				//check if a reduction is needed
				h_local_after=reducing_h-this->press_flow_data[i].sill_height;
				if(h_local_after>0.0){
					//reduce the flow
					reduction_fac=this->calculate_reduction_factor(h_local_before, h_local_after, contraction_coeff, this->press_flow_data[i].opening_height);
					q_buff=q_buff*reduction_fac;
				}
				/*cout <<" i " << i << " flcoef " << flow_coeff << " open h " << this->press_flow_data[i].opening_height << " open width " << this->press_flow_data[i].opening_width << " hlocal " << h_local_before;
				cout <<" con coef " << contraction_coeff << " h_l_after " << h_local_after << " reduc fac" << reduction_fac << " q " << q_buff << endl;
				Sys_Common_Output::output_hyd->output_txt(&cout);*/
			}
		}
		discharge=discharge+q_buff;
	}
	return discharge;
}
//Calculation of the flow coefficient for a gate flow (after "Ueberfaelle und Wehre", 2005, G. Peter)
double Hyd_River_Profile_Type_Bridge::calculate_flow_coefficient(const double h_local_before, const double opening_h){
	double coeff=0.0;
	double quotient=h_local_before/opening_h;
	double angle=90.0;

	coeff=0.7341169-0.04261387*log(quotient)-0.01410859*angle+0.00016111*pow(angle,2.0)-0.00000040072*pow(angle,3.0);
	coeff=coeff/(1.0-0.18920573*log(quotient)+0.02434395*pow(log(quotient),2.0)-0.01236335*angle+0.00012978*pow(angle,2.0));

	//set boundaries
	if(coeff>0.62){
		coeff=0.62;
	}
	else if(coeff<0.0){
		coeff=0.0;
	}
	return coeff;
}
//Calculation of the contraction factor for a gate of 90°-inclination (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
double Hyd_River_Profile_Type_Bridge::calculate_contraction_factor(const double h_local_before, const double opening_h, const double flow_coeff){
	double coeff=0.0;
	double quotient=h_local_before/opening_h;
	coeff=pow(flow_coeff,2.0)/(2.0*quotient)+flow_coeff*pow((1.0/(4.0*pow(quotient,2.0))+1.0),0.5);
	return coeff;
}
//Calculation of the reduction factor for backwater effect
double Hyd_River_Profile_Type_Bridge::calculate_reduction_factor(const double h_local_before, const double h_local_after, const double contraction_fac, const double opening_h){
	double coeff=0.0;
	double coeff1=0.0;
	double a1=contraction_fac*opening_h/h_local_before;
	double a2=contraction_fac*opening_h/h_local_after;

	coeff1=(pow((1.0-2.0*a1*(1.0-a2)),2.0)-1.0+pow((h_local_after/h_local_before),2.0));
	//no backwaterreduction
	if(coeff1<0.0){
		coeff=(1.0+a1)*((1.0-2.0*a1*(1.0-a2)));
	}
	else{
		coeff=(1.0+a1)*((1.0-2.0*a1*(1.0-a2))-pow(coeff1,0.5));

	}
	//replace the square-root function by a tangens- function by a given boundary; this functions is the best fit to the square-root
	//functions between 0.001  and 0.02 ; the boundary is set, where the functions (arctan/square root) are identically)
	if(abs(coeff)<=0.0152){
		coeff=0.10449968880528*atan(159.877741951379*coeff); //0.0152
	}
	else{
		coeff=pow(coeff,0.5);
	}

	//set boundaries
	if(coeff >1.0){
		coeff=1.0;
	}
	else if(coeff<0.0){
		coeff=0.0;
	}

	return coeff;
}
//Calculate an additional discharge via weir flow (weir formula)
double Hyd_River_Profile_Type_Bridge::calculate_weir_flow(const double flow_h, const double reducing_h){
	double delta_h_1=0.0;
	double delta_h_2=0.0;
	double q_buff=0.0;

	delta_h_1=flow_h-(this->global_bridge_height+this->bridge_data.bridgebody_size);
	delta_h_2=reducing_h-(this->global_bridge_height+this->bridge_data.bridgebody_size);

	//calculate the area
	this->weir_flow_area=this->area_table.get_interpolated_values(flow_h)-this->upper_bridge_area;

	//same waterlevels=> no flow
	if(abs(delta_h_1-delta_h_2)<constant::flow_epsilon){
		q_buff=0.0;
	}
	//both waterlevel are under the weir sill=> no flow
	else if(delta_h_1<=0.0 && delta_h_2<=0.0){
		q_buff=0.0;
	}
	//flow without submerged weirflow reductiom
	else if(delta_h_1> 0.0 && delta_h_2 <= 0.0){
		q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_1,(1.0/2.0))*this->weir_flow_area;

	}
	//flow with submerged weirflow reductiom
	else if(delta_h_1 > 0.0 && delta_h_2 > 0.0){
		//flow out of 1 into 2 
		q_buff=constant::Cfacweir*constant::poleni_const*pow(delta_h_1,(1.0/2.0))*this->weir_flow_area;
		//reduction of the discharge (submerged weirflow)
		double reduction_term=(1.0-delta_h_2/delta_h_1);
		//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
		if(reduction_term<=0.000463529){
			q_buff=q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
		}
		else{
			q_buff=q_buff*pow(reduction_term,(1.0/3.0));
		}		
	}
	return q_buff;


}
//set the error
Error Hyd_River_Profile_Type_Bridge::set_error(const int err_type){
		string place="Hyd_River_Profile_Type_Bridge::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://the local bridge height below the minimum local bridge height
			place.append("check_points(void)");
			reason="The local bridge height is below the minimal local bridge heigth";
			info<< "Minimal local bridge height: "<< constant::min_height_bridge << label::m << endl;
			help="Check the bridge profile";
			type=11;
			break;
		case 1:////check if the bridge body size is below the minimal bridge body size
			place.append("check_points(void)");
			reason="The bridge body size is below the minimal size" ;
			info<< "Minimal bridge body size: "<< constant::min_seize_bridge << label::m << endl;
			help="Check the bridge profile";
			type=11;
			break;
		case 2://the two outer profile points have to be above the bridge height
			place.append("calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table)");
			reason="The two outer profile points (left/right) have to be above the global bridge height";
			help="Check the bridge profile";
			type=11;
			break;
		case 3://bad alloc
			place.append("alloc_calc_pressure_flow_data(void)/clone_profile(_Hyd_River_Profile_Type *prof_type)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 4://no overhang!
			place.append("check_bridge_segments(void)");
			reason="No overhang in bridge profiles allowed";
			help="Check the bridge profile";
			type=14;
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