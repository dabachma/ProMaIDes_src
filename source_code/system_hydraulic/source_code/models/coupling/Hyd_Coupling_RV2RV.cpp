#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_RV2RV.h"

//constructor
Hyd_Coupling_RV2RV::Hyd_Coupling_RV2RV(void){		
	this->inflow_rv_model=NULL;
	this->outflow_rv_model=NULL;
	this->inflow_profile_up=NULL;
	this->inflow_profile_down=NULL;

	this->index_inflow_profile_down=-1;
	this->index_inflow_profile_upwards=-1;

	this->total_distance_inflow=0.0;
	this->distance_inflow_downwards=0.0;
	this->distance2river_begin_inflow=0.0;
	this->coupling_flag_inflow=false;
	this->lateral_flag_inflow=true;
	this->left_flag_inflow=false;
	this->horizontal_backwater_flag_inflow=false;
	this->horizontal_backwater_flag_inflow_upstream=false;
	this->mid_fac_down_inflow=0.0;
	this->mid_fac_up_inflow=0.0;

	this->mid_height=0.0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_RV2RV), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_RV2RV::~Hyd_Coupling_RV2RV(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_RV2RV), _sys_system_modules::HYD_SYS);
}
//__________________
//public
//The pointer of the models for coupling are setted
void Hyd_Coupling_RV2RV::set_ptr_coupling(Hyd_Model_River *inflow_river, Hyd_Model_River *outflow_river){
	this->inflow_rv_model=inflow_river;
	this->outflow_rv_model=outflow_river;
}
//The couplings are initialized
void Hyd_Coupling_RV2RV::init_coupling(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "RV_RV> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow) ";
	cout << " and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)..." << endl ;
	Sys_Common_Output::output_hyd->output_txt(&cout);
	try{
		//first chek if the there is already a h-boundary is given
		if(this->outflow_rv_model->outflow_river_profile.get_h_outflow_is_given()==true){
			this->coupling_flag_inflow=false;
			Warning msg=this->set_warning(0);
			msg.output_msg(2);
			return;
		}
		int river_section_index=-2;
		//set the inflow profile index
		river_section_index=this->inflow_rv_model->find_river_section_index_inside(false, this->outflow_rv_model->get_river_midline_last_section());
		if(river_section_index==0){
			//check for the first river section which profile is the nearest point (0 inflow profile or 1:=lateral)
			river_section_index=this->inflow_rv_model->river_midline.get_nearest_point_index(&(this->outflow_rv_model->outflow_river_profile.typ_of_profile->mid_point_global_x_y));
			if(river_section_index==0){
				this->lateral_flag_inflow=false;
			}
			else{
				this->lateral_flag_inflow=true;
				river_section_index=0;
			}

		}
		else{
			this->lateral_flag_inflow=true;
		}
		
		
		if(river_section_index>=0 && river_section_index<this->inflow_rv_model->Param_RV.get_number_profiles()-1){
			this->coupling_flag_inflow=true;
			//set the pointer and indices
			if(this->lateral_flag_inflow==false){
				this->inflow_profile_up=&this->inflow_rv_model->inflow_river_profile;
				this->index_inflow_profile_upwards=0;
				this->inflow_profile_down=NULL;
				this->index_inflow_profile_down=-1;
				this->inflow_rv_model->inflow_river_profile.set_q_inflow_is_given();
			}
			else{
				if(river_section_index==0 && this->inflow_rv_model->Param_RV.get_number_profiles()>2){
					this->inflow_profile_up=&this->inflow_rv_model->inflow_river_profile;
					this->index_inflow_profile_upwards=0;
					this->inflow_profile_down=&(this->inflow_rv_model->river_profiles[0]);
					this->index_inflow_profile_down=1;
				}
				else if(river_section_index==0 && this->inflow_rv_model->Param_RV.get_number_profiles()==2){
					this->inflow_profile_up=&this->inflow_rv_model->inflow_river_profile;
					this->index_inflow_profile_upwards=0;
					this->inflow_profile_down=&this->inflow_rv_model->outflow_river_profile;
					this->index_inflow_profile_down=1;
				}
				else if(river_section_index>0 && river_section_index==this->inflow_rv_model->Param_RV.get_number_profiles()-2){
					this->inflow_profile_up=&(this->inflow_rv_model->river_profiles[this->inflow_rv_model->Param_RV.get_number_profiles()-3]);	
					this->index_inflow_profile_upwards=river_section_index;
					this->inflow_profile_down=&this->inflow_rv_model->outflow_river_profile;
					this->index_inflow_profile_down=river_section_index+1;
				}
				else{
					this->inflow_profile_up=&(this->inflow_rv_model->river_profiles[river_section_index-1]);	
					this->index_inflow_profile_upwards=river_section_index;
					this->inflow_profile_down=&(this->inflow_rv_model->river_profiles[river_section_index]);
					this->index_inflow_profile_down=river_section_index+1;
				}

			}


			//outflow river model
			this->outflow_rv_model->outflow_river_profile.set_h_outflow_is_given();
			//calculate the distances
			this->calculate_distances_inflow(this->inflow_rv_model->get_river_section_polygon(river_section_index), this->outflow_rv_model->get_river_midline_last_section());
			
			//calculate the factors
			this->calculate_mid_factors();
			//check the connection
			this->check_connection();
		}

	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow) ";
		info << "and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)" << endl ;
		msg.make_second_info(info.str());
		throw msg;
	}
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Synchronise the models
void Hyd_Coupling_RV2RV::synchronise_models(void){
	if(this->coupling_flag_inflow==true){
		try{
			double value=0.0;

			value=this->calculate_mid_waterlevel();

			//set h to the outflow profile
			this->outflow_rv_model->outflow_river_profile.set_h_outflow_global(value);
			
			//get the inflow q
			value=this->outflow_rv_model->outflow_river_profile.get_actual_river_discharge();
			//set the inflow q
			if(this->lateral_flag_inflow==true){
				if(this->horizontal_backwater_flag_inflow==false && this->horizontal_backwater_flag_inflow_upstream==false){
					this->inflow_profile_up->add_coupling_discharge_1d(value*this->mid_fac_up_inflow);
					this->inflow_profile_down->add_coupling_discharge_1d(value*this->mid_fac_down_inflow);
				}
				else if(this->horizontal_backwater_flag_inflow==true){
					this->inflow_profile_down->add_coupling_discharge_1d(value);
				}
				else if(this->horizontal_backwater_flag_inflow_upstream==true){
					this->inflow_profile_up->add_coupling_discharge_1d(value);
				}
			}
			else{
				this->inflow_rv_model->inflow_river_profile.add_q_inflow_coupling(value);
			}
		}
		catch(Error msg){
			ostringstream info;
			info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow) ";
			info << "and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)" << endl ;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Reset the coupling discharges
void Hyd_Coupling_RV2RV::reset_coupling_discharges(void){
	if(this->coupling_flag_inflow==false){
		return;
	}
	if(this->lateral_flag_inflow==true ){
		this->inflow_profile_up->reset_coupling_discharge_1d();
		this->inflow_profile_down->reset_coupling_discharge_1d();
	}
	else{
		this->inflow_rv_model->inflow_river_profile.reset_q_inflow_coupling();
	}
}
//Output the header for the coupled model indizes (static)
void Hyd_Coupling_RV2RV::output_header_coupled_indices(ostringstream *cout){
	*cout << "River to river..." << endl ;
	*cout << "No." << W(12) << "Id_RVout" << W(12) << "Id_RVin" << W(14) << "Id_RVprofile" << W(14);
	*cout << "Lateral" << W(14) << "z_mid" <<label::m << W(15);
	*cout << "Left_bank_out"<< W(17) << "Dist_begin_out" << label::m << endl;
	Sys_Common_Output::output_hyd->output_txt(cout);

}
//Output the indexes of the coupled model
void Hyd_Coupling_RV2RV::output_index_coupled_models(ostringstream *cout, const int number){
	*cout << number << W(10);
	*cout << this->outflow_rv_model->Param_RV.get_river_number() << W(12);
	*cout << this->inflow_rv_model->Param_RV.get_river_number()<< W(14);
	*cout << this->index_inflow_profile_upwards <<W(16);
	*cout << functions::convert_boolean2string(this->lateral_flag_inflow)<< W(17);
	*cout << P(2) << FORMAT_FIXED_REAL<<  this->mid_height<< W(15);
	if(this->lateral_flag_inflow==true){
		*cout << functions::convert_boolean2string(this->left_flag_inflow) << W(15);
		*cout << this->distance2river_begin_inflow  <<endl;
	}
	else{
		*cout << endl;
	}
	Sys_Common_Output::output_hyd->output_txt(cout);
}
//Get if the lateral junction is on the left or right bank (flow direction inflow river)
bool Hyd_Coupling_RV2RV::get_left_bank_flag_inflow(void){
	return this->left_flag_inflow;
}
//Get if the junction is lateral
bool Hyd_Coupling_RV2RV::get_lateral_flag_inflow(void){
	return this->lateral_flag_inflow;
}
//Insert the junction to a list of RV2FP-coupling points
void Hyd_Coupling_RV2RV::insert_junction_inflow_point2RV2FPlist(Hyd_Coupling_Point_RV2FP_List *list, const int river_index){
	if(this->lateral_flag_inflow==false){
		return;
	}
	if(this->coupling_flag_inflow==false){
		return;
	}
	if(list->get_leftriver_bank_line()!=this->left_flag_inflow){
		return;
	}
	if(river_index<0){
		return;
	}
	if(river_index!=this->inflow_rv_model->Param_RV.get_river_number()){
		return;
	}
	try{
		//set the coupling point
		if (list->find_coupling_point(this->distance2river_begin_inflow, &this->inflow_point) == NULL) {
			return;
		};
		this->inflow_point.set_total_distance_along_polysegment(this->distance2river_begin_inflow);
		//ostringstream cout;
		//
		//this->inflow_point.output_coor_members(&cout);
		//Sys_Common_Output::output_hyd->output_txt(&cout);
		//it is the right list=> insert the junction
		
		//list->output_setted_members(&cout);
		//Sys_Common_Output::output_hyd->output_txt(&cout);
		list->insert_constricting_coupling_points(this->outflow_rv_model->outflow_river_profile.typ_of_profile->get_width_end_start_profile(), &this->inflow_point);

	}
	catch(Error msg){
		ostringstream info;
		info << "Coupling between river model " << this->inflow_rv_model->Param_RV.get_river_number() << " (inflow) ";
		info << "and river model " <<this->outflow_rv_model->Param_RV.get_river_number() <<" (outflow)" << endl ;
		info << "Try to insert the coupling point of the ouflow profile to the rv2fp-list of the inflow river" << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Clone the river to river couplings
void Hyd_Coupling_RV2RV::clone_couplings(Hyd_Coupling_RV2RV *coupling, Hyd_Hydraulic_System *system){
	
	this->index_inflow_profile_down=coupling->index_inflow_profile_down;
	this->index_inflow_profile_upwards=coupling->index_inflow_profile_upwards;

	this->total_distance_inflow=coupling->total_distance_inflow;
	this->distance_inflow_downwards=coupling->distance_inflow_downwards;
	this->distance2river_begin_inflow=coupling->distance2river_begin_inflow;
	this->coupling_flag_inflow=coupling->coupling_flag_inflow;
	this->lateral_flag_inflow=coupling->lateral_flag_inflow;
	this->left_flag_inflow=coupling->left_flag_inflow;
	this->horizontal_backwater_flag_inflow=coupling->horizontal_backwater_flag_inflow;
	this->horizontal_backwater_flag_inflow_upstream=coupling->horizontal_backwater_flag_inflow_upstream;
	this->mid_fac_down_inflow=coupling->mid_fac_down_inflow;
	this->mid_fac_up_inflow=coupling->mid_fac_up_inflow;

	this->mid_height=coupling->mid_height;

	if(coupling->inflow_rv_model!=NULL){
		this->inflow_rv_model=system->get_ptr_river_model(coupling->inflow_rv_model->Param_RV.get_river_number());
		this->inflow_profile_up=this->inflow_rv_model->get_ptr_river_profile(this->index_inflow_profile_upwards);
		this->inflow_profile_down=this->inflow_rv_model->get_ptr_river_profile(this->index_inflow_profile_down);
	}
	if(coupling->outflow_rv_model!=NULL){
		this->outflow_rv_model=system->get_ptr_river_model(coupling->outflow_rv_model->Param_RV.get_river_number());
	}
	this->inflow_point=coupling->inflow_point;

}
//______________
//protected
//Calculate the distance to the inflow profiles via an interception of a midline segment of one river section and a river section polygon
void Hyd_Coupling_RV2RV::calculate_distances_inflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline){

	if(this->lateral_flag_inflow==false){
		return;
	}
	
	//ostringstream cout;
	//Sys_Common_Output::output_hyd->output_txt(&cout);
	//river_section_polygon->output_point_members(&cout);
	//river_section_midline->output_point_members(&cout);
	//Sys_Common_Output::output_hyd->output_txt(&cout);

	Geo_Interception_Point_List list;
	//there must be one or two interception point
	river_section_polygon->calculate_segment_interception(&list, river_section_midline);
	list.sort_distance();
	_geo_multisegment_interception key_point=list.get_first_point();
	

	//point is intercepted
	//upwards profile
	if(key_point.index_point_is_intercepted==0 || key_point.index_point_is_intercepted==3){
		if(key_point.index_point_is_intercepted==0){
			this->total_distance_inflow=river_section_polygon->get_segment(0)->get_distance();
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_inflow=true;
		}
		else{
			this->total_distance_inflow=river_section_polygon->get_segment(2)->get_distance();
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->left_flag_inflow=false;

		}
		this->distance_inflow_downwards=this->total_distance_inflow;
	}
	//downwards profile
	else if(key_point.index_point_is_intercepted==1 || key_point.index_point_is_intercepted==2){
		
		if(key_point.index_point_is_intercepted==1){
			this->left_flag_inflow=true;
			this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_inflow=river_section_polygon->get_segment(0)->get_distance();
		}
		else{
			this->left_flag_inflow=false;
			this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
			this->total_distance_inflow=river_section_polygon->get_segment(2)->get_distance();
		}
		this->distance_inflow_downwards=0.0;
	}

	//segment is intercepted
	//upstream
	else if(key_point.index_is_intercept==0){
		//Error
		Error msg = this->set_error(1);
		ostringstream info;
		info << "River midline is intercepting the upstream profile of the inflow river"<< endl;
		info << "Check the river polygon points in QGIS of river " << this->inflow_rv_model->Param_RV.get_river_name() <<endl;
		river_section_polygon->output_point_members(&info);
		info << "Check the river midline points in QGIS of river " << this->outflow_rv_model->Param_RV.get_river_name()<< endl;
		river_section_midline->output_point_members(&info);
		msg.make_second_info(info.str());
		throw msg;


		//decide which side
		//Geo_Interception_Point_List list1;
		//_geo_interception_point point1;
		////left side
		//point1=this->inflow_rv_model->river_leftline.calc_last_interception(river_section_midline);
		//if(point1.interception_flag==true){
		//	this->left_flag_inflow=true;
		//	this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
		//	this->distance_inflow_downwards=this->total_distance_inflow;	
		//	this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		//}
		//else{
		//	//right side
		//	point1=this->inflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
		//	this->left_flag_inflow=false;
		//	this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
		//	this->distance_inflow_downwards=this->total_distance_inflow;	
		//	this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		//}
	}
	//downstream
	else if(key_point.index_is_intercept==2){
		//Error
		Error msg = this->set_error(1);
		ostringstream info;
		info << "River midline is intercepting the downstream profile of the inflow river" << endl;
		info << "Check the river polygon points in QGIS of river " << this->inflow_rv_model->Param_RV.get_river_name() << endl;
		river_section_polygon->output_point_members(&info);
		info << "Check the river midline points in QGIS of river " << this->outflow_rv_model->Param_RV.get_river_name() << endl;
		river_section_midline->output_point_members(&info);
		msg.make_second_info(info.str());
		throw msg;


		////decide which side
		//Geo_Interception_Point_List list1;
		//_geo_interception_point point1;
		////left side
		//point1=this->inflow_rv_model->river_leftline.calc_last_interception(river_section_midline);
		//if(point1.interception_flag==true){
		//	this->left_flag_inflow=true;
		//	this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
		//	this->distance_inflow_downwards=0.0;	
		//	this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&point1.interception_point);
		//}
		//else{
		//	//right side
		//	point1=this->inflow_rv_model->river_rightline.calc_last_interception(river_section_midline);
		//	this->left_flag_inflow=false;
		//	this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
		//	this->distance_inflow_downwards=0.0;	
		//	this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&point1.interception_point);
		//}
	}
	//right bank
	else if(key_point.index_is_intercept==1){
		this->left_flag_inflow=false;
		this->distance2river_begin_inflow=this->inflow_rv_model->river_rightline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_inflow=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].get_distance();
		this->distance_inflow_downwards=this->inflow_rv_model->river_rightline.my_segment[this->index_inflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));	
	}
	//left bank
	else if(key_point.index_is_intercept==3){
		this->left_flag_inflow=true;
		this->distance2river_begin_inflow=this->inflow_rv_model->river_leftline.get_distance_along_polysegment(&(key_point.line_interception.interception_point));
		this->total_distance_inflow=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].get_distance();
		this->distance_inflow_downwards=this->inflow_rv_model->river_leftline.my_segment[this->index_inflow_profile_upwards].point2.distance(&(key_point.line_interception.interception_point));
	}

	//check if the upwards profile is a weir/bridge profile
	if(this->inflow_profile_up->get_profile_type()==_hyd_profile_types::WEIR_TYPE || this->inflow_profile_up->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->total_distance_inflow=1.0;
		this->distance_inflow_downwards=0.0;
	}
	if(this->inflow_profile_down->get_profile_type()==_hyd_profile_types::WEIR_TYPE || this->inflow_profile_down->get_profile_type()==_hyd_profile_types::BRIDGE_TYPE){
		this->total_distance_inflow=1.0;
		this->distance_inflow_downwards=1.0;
	}
}
//Check the connection of the rivers
void Hyd_Coupling_RV2RV::check_connection(void){
	//the global z min coordinate of the outflow profile have to be >= than the inflow profile z-min coordinate
	if(this->lateral_flag_inflow==true){
		this->mid_height=this->inflow_profile_down->typ_of_profile->get_global_z_min()*this->mid_fac_down_inflow+
			this->inflow_profile_up->typ_of_profile->get_global_z_min()*this->mid_fac_up_inflow;
		//check it
		if(this->outflow_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()< this->mid_height ){
			Error msg=this->set_error(0);
			ostringstream info;
			info << "Global z_min outflow profile     : " <<this->outflow_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()<< label::m <<endl;
			info << "Global z_min_mid inflow profile  : " <<this->mid_height<< label::m <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
	else{
		this->mid_height=this->inflow_rv_model->inflow_river_profile.typ_of_profile->get_global_z_min();
		if(this->outflow_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()< this->inflow_rv_model->inflow_river_profile.typ_of_profile->get_global_z_min()){
			Error msg=this->set_error(0);
			ostringstream info;
			info << "Global z_min outflow profile     : " <<this->outflow_rv_model->outflow_river_profile.typ_of_profile->get_global_z_min()<< label::m <<endl;
			info << "Global z_min_mid inflow profile  : " <<this->inflow_rv_model->inflow_river_profile.typ_of_profile->get_global_z_min()<< label::m <<endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//Calculate the current mid-waterlevel via interpolation
double Hyd_Coupling_RV2RV::calculate_mid_waterlevel(void){

	if(this->coupling_flag_inflow==false){
		return 0.0;
	}
	double value=0.0;
	this->horizontal_backwater_flag_inflow=false;
	this->horizontal_backwater_flag_inflow_upstream=false;

	if(this->lateral_flag_inflow==true){
		if(this->inflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->inflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			value=this->inflow_profile_down->get_actual_global_waterlevel()*this->mid_fac_down_inflow+
				this->inflow_profile_up->get_actual_global_waterlevel()*this->mid_fac_up_inflow;
		}
		else if(this->inflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->inflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon){
			value=this->inflow_profile_up->get_actual_global_waterlevel();
			this->horizontal_backwater_flag_inflow_upstream=true;
		}
		else if(this->inflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()>constant::dry_hyd_epsilon && this->inflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			value=this->inflow_profile_down->get_actual_global_waterlevel();
			this->horizontal_backwater_flag_inflow=true;
		}
		else if(this->inflow_profile_down->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon && this->inflow_profile_up->typ_of_profile->get_actual_local_waterlevel_h()<=constant::dry_hyd_epsilon){
			value=this->inflow_profile_down->typ_of_profile->get_global_z_min()*this->mid_fac_down_inflow+
				this->inflow_profile_up->typ_of_profile->get_global_z_min()*this->mid_fac_up_inflow;
		}
	}
	else{
		value=this->inflow_rv_model->inflow_river_profile.get_actual_global_waterlevel();
	}
	return value;
}
//Calculate the mid factors for the interpolation of the waterlevel
void Hyd_Coupling_RV2RV::calculate_mid_factors(void){
	if(this->lateral_flag_inflow==true){
		this->mid_fac_down_inflow=(1.0-this->distance_inflow_downwards/this->total_distance_inflow);
		this->mid_fac_up_inflow=(this->distance_inflow_downwards/this->total_distance_inflow);
	}
}
//Set error(s)
Error Hyd_Coupling_RV2RV::set_error(const int err_type){
string place="Hyd_Coupling_RV2RV::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://outflow zmin < inflow zmin
			place.append("check_connection(void)");
			reason="The global z-min coordinate of the outflow profile have to be >= than the inflow profile z-min coordinate";
			help="Check the coupling profiles";
			info << "Index river inflow profile " << this->index_inflow_profile_upwards << endl;
			type=16;
			break;
		case 1://midline is intercepting profile
			place.append("calculate_distances_inflow(Hyd_Floodplain_Polygon *river_section_polygon, Geo_Segment *river_section_midline)");
			reason = "The river midline of the outflow river is intercepting a profile of the inflow river";
			help = "Check the river midline ot the outflow river and the profiles of the inflow river";
			type = 16;
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
//Set the warning
Warning Hyd_Coupling_RV2RV::set_warning(const int warn_type){
	string place="Hyd_Coupling_RV2RV::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://Type of material value does not match
			place.append("init_coupling(void)") ;
			reason="There is a second outflow coupling for the river to another river";
			reaction="This coupling is ignored";
			help= "The mid point of the outflow profile should not be in different river polygons";
			info << "Index outflow river : " <<this->outflow_rv_model->Param_RV.get_river_number() << endl;
			info << "Index inflow river : " <<this->inflow_rv_model->Param_RV.get_river_number() << endl;
			type=10;	
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