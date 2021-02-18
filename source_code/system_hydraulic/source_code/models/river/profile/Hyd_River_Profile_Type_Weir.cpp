#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_River_Profile_Type_Weir.h"

//constructor
Hyd_River_Profile_Type_Weir::Hyd_River_Profile_Type_Weir(void){
	this->downstream_waterlevel_local=0.0;
	this->downstream_waterlevel_global=0.0;
	this->upstream_waterlevel_local=0.0;
	this->upstream_waterlevel_global=0.0;
	this->weir_discharge_table=NULL;
	//reset the table type
	this->c_left_table.set_table_type(hyd_label::Conveyance_table_left_weir);
	this->c_right_table.set_table_type(hyd_label::Conveyance_table_right_weir);
	this->material_type=_hyd_mat_coefficient_types::poleni_coefficient;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_River_Profile_Type_Weir)-sizeof(Hyd_River_Profile_Type_Standard), _sys_system_modules::HYD_SYS);

}

//destructor
Hyd_River_Profile_Type_Weir::~Hyd_River_Profile_Type_Weir(void){
	if(this->weir_discharge_table!=NULL){
		delete [] this->weir_discharge_table;
		this->weir_discharge_table=NULL;
	}
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_River_Profile_Type_Weir)-sizeof(Hyd_River_Profile_Type_Standard), _sys_system_modules::HYD_SYS);
}
//_______________
//public
//Calculate and allocate the tables (Hyd_Tables): area, waterlevel, convenyance for left bank, right bank and mid channel
void Hyd_River_Profile_Type_Weir::calc_alloc_tables(const double delta_x, Hyd_Param_Material *material_table){
	try{
		Hyd_River_Profile_Type_Standard::calc_alloc_tables(delta_x, material_table);
		//set the waterlevl as the independent value of the convenyance table (left bank and right bank)
		this->c_left_table.set_independent_values(&this->h_table);
		this->c_right_table.set_independent_values(&this->h_table);

		this->check_weir_segments();
	}
	catch(Error msg){
		throw msg;
	}
	try{
		this->weir_discharge_table = new Hyd_Tables[this->no_mid_segments];
		for (int i=0; i< this->no_mid_segments; i++){
			this->weir_discharge_table[i].allocate_dependent_values(this->number_table_points);
			this->weir_discharge_table[i].set_independent_values(&this->h_table);
			this->weir_discharge_table[i].set_table_type(hyd_label::Weir_discharge_table);
		}
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}

	this->calculate_weir_tables();
	//this->output_tables();
	
}
///Set the actuel flow specific values with a given waterlevel from tables
void Hyd_River_Profile_Type_Weir::set_actuel_profilevalues_by_waterlevel(const double global_waterlevel){
	try{
		this->set_waterlevel_direct(global_waterlevel);
		this->solved_area=this->area_table.get_interpolated_values(global_waterlevel);
		if(this->solved_area<0.0){
			this->area2calc=0.000;	
		}
		else{
			this->area2calc=this->solved_area;
		}
		this->set_actual_conveyance_by_table(this->s_value);

		this->width = this->width_table.get_interpolated_values(this->area2calc);
	}
	catch(Error msg){
		throw msg;	
	}
}
//output the tables
void Hyd_River_Profile_Type_Weir::output_tables(void){
	ostringstream cout;
	this->c_left_table.output_table(&cout);
	this->c_right_table.output_table(&cout);
	this->h_table.output_table(&cout);
	this->width_table.output_table(&cout);
	for (int i=0; i< this->no_mid_segments; i++){
		cout << "Table for Segmentno. " << i << endl;
		this->weir_discharge_table[i].output_table(&cout);
	}
}
//Calculate the actual discharge through this profile by a given upstream and downstream profile
double Hyd_River_Profile_Type_Weir::calculate_actual_discharge(_Hyd_River_Profile *upstream_profile,  _Hyd_River_Profile *downstream_profile, const double distance_upstream){
	double discharge=0.0;
	double buff_discharge=0.0;

	
	//set the waterlevel from the upstream/downstream profile
	if(upstream_profile->get_actual_global_waterlevel()>=downstream_profile->get_actual_global_waterlevel()){
		this->s_value=upstream_profile->get_actual_global_waterlevel();
	}
	else if(upstream_profile->get_actual_global_waterlevel()<downstream_profile->get_actual_global_waterlevel()){
		this->s_value=downstream_profile->get_actual_global_waterlevel();
	}	
	
	//weir is not overflowed
	if((upstream_profile->get_actual_global_waterlevel()-this->global_z_min)<constant::dry_hyd_epsilon && (downstream_profile->get_actual_global_waterlevel()-this->global_z_min)<constant::dry_hyd_epsilon){
		this->solved_area=0.0;
		this->s_value=this->get_global_z_min();
		this->h_value=0.0;
		this->area2calc=0.0;
		discharge=0.0;
		this->v_value=0.0;
		return discharge; 
	}

	this->h_value=this->s_value-this->get_global_z_min();

	//waterlevel upstream defines also the area of the weir
	try{
		this->solved_area=this->area_table.get_interpolated_values(this->s_value);
		this->area2calc=this->solved_area;
		this->width = this->width_table.get_interpolated_values(this->area2calc);

	}
	catch(Error msg){
		ostringstream info;
		info << "Weir is not overflowed; Check the weir " << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//set the waterlevels (upstream, downstream)
	this->downstream_waterlevel_global=downstream_profile->get_actual_global_waterlevel();
	this->downstream_waterlevel_local=this->downstream_waterlevel_global-this->global_z_min;
	this->upstream_waterlevel_global=upstream_profile->get_actual_global_waterlevel();
	this->upstream_waterlevel_local=this->upstream_waterlevel_global-this->global_z_min;;


	
	//no flow (same waterlevels)
	if(abs(this->downstream_waterlevel_global-this->upstream_waterlevel_global)<constant::flow_epsilon){
		discharge=0.0;
	}
	//no submerged due to downstream waterlevel (flow downstream)
	else if(this->downstream_waterlevel_local<= 0.0 && this->upstream_waterlevel_local>0.0){
		for(int i=0; i< this->no_mid_segments; i++){
			discharge=discharge+this->weir_discharge_table[i].get_interpolated_values(this->s_value);
		}

	}//no submerged due to upstream waterlevel (flow upnstream)
	else if(this->downstream_waterlevel_local> 0.0 && this->upstream_waterlevel_local<=0.0){
		for(int i=0; i< this->no_mid_segments; i++){
			discharge=discharge-this->weir_discharge_table[i].get_interpolated_values(this->s_value);
		}

	}
	//weirflow with submerged reduction due to the downstream waterlevel (flow downstream)
	else if(this->upstream_waterlevel_global > this->downstream_waterlevel_global){
		double buff_overfall_height_upstream=0.0;
		double buff_reduction_height_downstream=0.0;
		for(int i=0; i< this->no_mid_segments; i++){		
			buff_discharge=this->weir_discharge_table[i].get_interpolated_values(this->s_value);
			if(buff_discharge==0.0){
				discharge=discharge+buff_discharge;
			}
			else{
				buff_overfall_height_upstream=this->s_value-(this->mid_profile[i].get_minimal_y_coordinate()+this->get_global_z_min());
				buff_reduction_height_downstream=this->downstream_waterlevel_global-(this->mid_profile[i].get_minimal_y_coordinate()+this->get_global_z_min());
				if(buff_reduction_height_downstream<=0.0){
					discharge=discharge+buff_discharge;
				}
				else{
					//reduction of the discharge
					double reduction_term=(1.0-buff_reduction_height_downstream/buff_overfall_height_upstream);
					//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
					/*if(reduction_term<=0.000463529){
						discharge=discharge+buff_discharge*0.057965266895*atan(8984.365582471040*reduction_term);
					}*/
					if(reduction_term<=0.01677822){
						discharge=discharge+buff_discharge*0.1878318106*atan(282.613262681108*reduction_term);
					}
					else{
						discharge=discharge+buff_discharge*pow(reduction_term,(1.0/3.0));
					}
				}
			}		
		}
	}
	//weirflow with submerged reduction due to the upstream waterlevel (flow upstream)
	else if(this->downstream_waterlevel_global>this->upstream_waterlevel_global){
		double buff_reduction_height_upstream=0.0;
		double buff_overfall_height_downstream=0.0;
		for(int i=0; i< this->no_mid_segments; i++){
			buff_discharge=this->weir_discharge_table[i].get_interpolated_values(this->s_value);
			if(buff_discharge==0.0){
				discharge=discharge-buff_discharge;
			}
			else{
				buff_reduction_height_upstream=this->upstream_waterlevel_global-(this->mid_profile[i].get_minimal_y_coordinate()+this->get_global_z_min());
				buff_overfall_height_downstream=this->s_value-(this->mid_profile[i].get_minimal_y_coordinate()+this->get_global_z_min());
				if(buff_reduction_height_upstream<=0.0){
					discharge=discharge-buff_discharge;
				}
				else{
					//reduction of the discharge
					double reduction_term=(1.0-buff_reduction_height_upstream/buff_overfall_height_downstream);
					//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
					if(reduction_term<=0.000463529){
						discharge=discharge-buff_discharge*0.057965266895*atan(8984.365582471040*reduction_term);
					}
					else{
						discharge=discharge-buff_discharge*pow(reduction_term,(1.0/3.0));
					}
				}
			}		
		}
	}


	//calculate discharge via the left and right bank
	//calculate the gradient
	double gradient=0.0;
	gradient=(this->upstream_waterlevel_global-this->downstream_waterlevel_global);
	//same waterlevels => no gradient
	if(abs(gradient)<=constant::flow_epsilon){
		gradient=0.0;
	}
	//almost same waterlevels: replace the gradient by arctan-function
	else if(abs(gradient)<=0.0152){
		gradient=0.10449968880528*atan(159.877741951379*(gradient))/pow(distance_upstream,0.5);
	}
	//take the square root function
	else{
		gradient=gradient/(pow(distance_upstream,0.5)*pow(abs(gradient),0.5));
	}

	//set the value of convenyance (left and right bank)
	this->c_left_value=this->c_left_table.get_interpolated_values(this->s_value);
	this->c_right_value=this->c_right_table.get_interpolated_values(this->s_value);

	buff_discharge=(this->c_left_value+this->c_right_value)*gradient;	
	discharge=discharge+buff_discharge;


	//calculate the velocity
	if(this->area2calc<=constant::area_epsilon){
		this->v_value=0.0;
		this->froude_no=0.0;
	}
	else{
		this->v_value=discharge/this->area2calc;
		this->froude_no=1.0;
	}

	return discharge;
}
//Set the actuel flow specific values with a given area from tables
void Hyd_River_Profile_Type_Weir::set_actuel_profilevalues_by_table(const double area){
	//is in calculate_actual_discharge() overwriten
	this->solved_area=area;
}
//Reset the maximum values and the result
void Hyd_River_Profile_Type_Weir::reset_result_max_values(void){
	_Hyd_River_Profile_Type::reset_result_max_values();
	this->downstream_waterlevel_local=0.0;
	this->downstream_waterlevel_global=0.0;
	this->upstream_waterlevel_local=0.0;
	this->upstream_waterlevel_global=0.0;
}
//Clone river profile
void Hyd_River_Profile_Type_Weir::clone_profile_type(_Hyd_River_Profile_Type *prof_type){
	Hyd_River_Profile_Type_Standard::clone_profile_type(prof_type);

	this->c_left_table.set_independent_values(&this->h_table);
	this->c_right_table.set_independent_values(&this->h_table);

	Hyd_River_Profile_Type_Weir *buffer=(Hyd_River_Profile_Type_Weir *)prof_type;


	this->downstream_waterlevel_local=buffer->downstream_waterlevel_local;
	this->downstream_waterlevel_global=buffer->downstream_waterlevel_global;
	this->upstream_waterlevel_local=buffer->upstream_waterlevel_local;
	this->upstream_waterlevel_global=buffer->upstream_waterlevel_global;
	try{
		this->weir_discharge_table = new Hyd_Tables[this->no_mid_segments];
		for (int i=0; i< this->no_mid_segments; i++){
			this->weir_discharge_table[i].clone_tables(&(buffer->weir_discharge_table[i]));
			this->weir_discharge_table[i].set_independent_values(&this->h_table);
		}
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//_______________
//private
//check the segments of the main channel (main channel = weir)
void Hyd_River_Profile_Type_Weir::check_weir_segments(void){
	for (int i=0; i<this->no_mid_segments; i++){
		if ( this->mid_profile[i].point1.check_right_of(&this->mid_profile[i].point2)==true){
			Error msg;
			msg = this->set_error(1);
			throw msg;
		}
	}
}
//calculate the weir tables
void Hyd_River_Profile_Type_Weir::calculate_weir_tables(void){
	//set the first values
	for (int i=0; i<this->no_mid_segments; i++){
		this->weir_discharge_table[i].set_values(0,0.0);
	}
	double buffer_height=0.0;
	Hyd_Profile_Segment buff_intercept_segment;
	_geo_interception_point buff_interception_point;
	buff_interception_point.interception_point.set_point_name(label::interception_point);


	//allocate the waterlevel
	try{
		this->waterlevel_line=new Geo_Straight_Line;
	}
	catch(bad_alloc &){
		Error msg=this->set_error(0);
		throw msg;
	}
	for (int i=0; i<this->no_mid_segments; i++){
		
		for (int j=0; j<this->number_table_points; j++){
			//vertical segments have no overflow
			if(this->mid_profile[i].get_vertical_flag()==true){
				this->weir_discharge_table[i].set_values(j, 0.0);
			}
			else{
				buffer_height =	this->weir_discharge_table[i].get_values(j).independentvalue-this->global_z_min;
				//generate the waterlevel line
				this->waterlevel_line->set_coordinates(0.0,buffer_height);
				//check if segment is wet
				if(this->mid_profile[i].point1.check_below_of(&this->waterlevel_line->point1)==true &&
					this->mid_profile[i].point2.check_below_of(&this->waterlevel_line->point1)==true){
						//waterlevel completly above segment
						this->weir_discharge_table[i].set_values(j,this->calculate_segment_overflow_discharge2table(&this->mid_profile[i]));
				}
				else if(this->mid_profile[i].point1.check_above_of(&this->waterlevel_line->point1)==true &&
					this->mid_profile[i].point2.check_above_of(&this->waterlevel_line->point1)==true){
						//waterlevel below segments
						this->weir_discharge_table[i].set_values(j,0.0);
				}
				else{
					//waterlevel intercept segment
					this->mid_profile[i].calc_interception(this->waterlevel_line, &buff_interception_point);
					if(buff_interception_point.indefinite_flag==true){
						//both are horizontal => 0.0 discharge
						this->weir_discharge_table[i].set_values(j,0.0);
					}
					else if(this->mid_profile[i].point1.check_above_of(&this->mid_profile[i].point2)==true){
						//make new buffer segment
						buff_intercept_segment=this->mid_profile[i];
						buff_intercept_segment.set_points(&buff_interception_point.interception_point, &this->mid_profile[i].point2);
						buff_intercept_segment.recalc_profilespecific_members();
						//calculate the discharge
						this->weir_discharge_table[i].set_values(j,this->calculate_segment_overflow_discharge2table(&buff_intercept_segment));

					}
					else if(this->mid_profile[i].point2.check_above_of(&this->mid_profile[i].point1)==true){
						//make new buffer segment
						buff_intercept_segment=this->mid_profile[i];
						buff_intercept_segment.set_points(&this->mid_profile[i].point1, &buff_interception_point.interception_point);
						buff_intercept_segment.recalc_profilespecific_members();
						//calculate the discharge
						this->weir_discharge_table[i].set_values(j,this->calculate_segment_overflow_discharge2table(&buff_intercept_segment));

					}


				}
			}
		}
	}
	//delete the waterlevel
	if(this->waterlevel_line!=NULL){
		delete this->waterlevel_line;
		this->waterlevel_line=NULL;	
	}
}
//calculate the overflow discharge for each segment for generating the table
double Hyd_River_Profile_Type_Weir::calculate_segment_overflow_discharge2table(Hyd_Profile_Segment *segment){
	
	double discharge = 0.0;
	if(segment->get_vertical_flag()==true){
		return discharge;
	}
	double b=segment->get_horizontal_dist();
	double h_over_total=0.0;
	double m=segment->get_gradient();
	double c_fac=2.0/3.0*pow(2.0*constant::Cgg,0.5)*segment->get_mat_value();
	
	h_over_total=this->waterlevel_line->point1.vertical_distance(&(segment->point1));
	if(h_over_total<0.0){
		h_over_total=0.0;
	}
	//if segment is horizontal the heights can directly be calculated
	if (segment->get_vertical_dist()==0.0){
		discharge=c_fac*b*pow(h_over_total,3.0/2.0);
	}
	//if segment is inclined...
	else{
		

		double hu=h_over_total-m*b;
		if(hu<0.0){
			hu=0.0;
		}
		discharge=-(2.0/(5.0*m))*c_fac*(pow((hu),5.0/2.0)-pow(h_over_total,5.0/2.0));
	}
	if(discharge<=constant::area_epsilon){
		discharge=0.0;
	};
	return discharge;
}

//Set actual conveyance with a given global waterlevel
void Hyd_River_Profile_Type_Weir::set_actual_conveyance_by_table(const double waterlevel){

	try{
		this->c_left_value=this->c_left_table.get_interpolated_values(waterlevel);
		this->c_right_value=this->c_right_table.get_interpolated_values(waterlevel);
	}
	catch(Error msg){
		throw msg;
	}

}
//set the error
Error Hyd_River_Profile_Type_Weir::set_error(const int err_type){
		string place="Hyd_River_Profile_Type_Weir::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("calc_alloc_tables()/calculate_weir_tables()/clone_profile()");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no overhang!
			place.append("check_weir_segments(void)");
			reason="No overhang in weir profiles allowed";
			help="Check the weir profile";
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