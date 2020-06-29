//#include "Fpl_Cub_Dike_Waterside.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Cub_Dike_Waterside::Fpl_Cub_Dike_Waterside(void){
	//points
	this->ptr_base_point=NULL;
	this->ptr_crest_point=NULL;
	this->number_points=0;

	//segments
	this->ptr_segments=NULL;
	this->number_segments=0;
	this->ptr_ideal_segments=NULL;
	this->number_ideal_segments=0;
	//interception with segments
	this->interception.indefinite_flag=false;
	this->interception.interception_flag=false;
	this->interception.interception_point.set_point_name(label::interception_point);

	this->mid_gamma_material=1.0;

	//berms and foreshores
	this->gamma_berm=1.0;
	this->with_berm_flag=false;
	this->line_influence_berm=NULL;
	this->number_horizontal_berms=0;
	this->part_contraction=NULL;

	

	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Waterside), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Cub_Dike_Waterside::~Fpl_Cub_Dike_Waterside(void){
	this->delete_segment_real();
	this->delete_segment_ideal();
	this->delete_berm_influence_line();
	this->delete_contraction_factor();

	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Dike_Waterside), _sys_system_modules::FPL_SYS);//count the memory
}
//__________
//public
//Set the geometrie of the waterside cubature per database table
void Fpl_Cub_Dike_Waterside::set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	//output
	ostringstream cout;
	cout <<"Set the waterside cubature of the dike... "  << endl ;
	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	this->set_base_crest_point(base_point, crest_point);

	QSqlQueryModel results;

	Fpl_Section_Points::select_different_zone_points(&results, id, section_id, fpl_label::cub_waterside , ptr_database, fpl_label::region_cubature, fpl_label::sec_dike);

	this->number_points=results.rowCount()+2;
	this->number_segments=this->number_points-1;
	try{
		this->allocate_segment_real();
	}
	catch(Error msg){
		throw msg;
	}

	//set the segments
	try{
		if(this->number_segments==1){
			//just one segment
			//begin with the base point water end with the crest point; no points between
			this->ptr_segments[0].set_points(this->ptr_base_point,this->ptr_crest_point);
			this->ptr_segments[0].set_zone_number(this->ptr_crest_point->get_zone_number());
		}
		else{
			//more than one segment
			Fpl_Section_Points waterside_next(fpl_label::cub_waterside);
			Fpl_Section_Points waterside_before(fpl_label::cub_waterside);
			for(int i=0; i< this->number_segments; i++){
				if(i==0){//begin with the base point water then the waterside number 0
					waterside_next.set_point_per_database(&results,i);
					this->ptr_segments[i].set_points(this->ptr_base_point,&waterside_next);
					this->ptr_segments[i].set_zone_number(waterside_next.get_zone_number());
					waterside_before=waterside_next;
				}
				else if(i==this->number_segments-1){//end with the crest point
					this->ptr_segments[i].set_points(&waterside_before,this->ptr_crest_point);
					this->ptr_segments[i].set_zone_number(this->ptr_crest_point->get_zone_number());
				}
				else{
					waterside_next.set_point_per_database(&results,i);
					this->ptr_segments[i].set_points(&waterside_before,&waterside_next);
					this->ptr_segments[i].set_zone_number(waterside_next.get_zone_number());
					waterside_before=waterside_next;
				}
				this->ptr_segments[i].set_gradient_reduction();
			}
		}
		//check the cubature
		this->check_cubature();
		//idealize the cubature (just the geometrie; gamma_material is calculated with the real cubature)
		this->idealizise_cubature();
	}
	catch(Error msg){
		throw msg;
	}
	this->calc_total_length();
}
//Set the geometrie of the waterside cubature per file
void Fpl_Cub_Dike_Waterside::set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter){
	//read in
	string myline;
	QString qmyline;

	this->set_base_crest_point(base_point, crest_point);

	//find number of points
	do{
		qmyline=ifile->readLine();
		myline=qmyline.toStdString();
		(*line_counter)++;
		functions::clean_string(&myline);
		 if(myline==fpl_label::end_waterside_cub){
			break;
		}
		if(myline.empty()==false){
			this->find_key_values_file(myline);
		}
		if(this->number_points>0){
			break;
		}
	}
	while(ifile->atEnd()==false);

	this->number_points=this->number_points+2;
	this->number_segments=this->number_points-1;
	try{
		this->allocate_segment_real();
	}
	catch(Error msg){
		throw msg;
	}

	//set the segments
	int counter_point=0;
	try{
		if(this->number_segments==1){
			//just one segment
			//begin with the base point water end with the crest point; no points between
			this->ptr_segments[0].set_points(this->ptr_base_point,this->ptr_crest_point);
			this->ptr_segments[0].set_zone_number(this->ptr_crest_point->get_zone_number());
		}
		else{
			//more than one segment
			Fpl_Section_Points waterside_next(fpl_label::cub_waterside);
			Fpl_Section_Points waterside_before(fpl_label::cub_waterside);
			do{
				qmyline=ifile->readLine();
				myline=qmyline.toStdString();
				(*line_counter)++;
				functions::clean_string(&myline);
				if(myline==fpl_label::end_waterside_cub){
					break;
				}
				else if(myline.empty()==false){
					double buff_x=0.0;
					double buff_y=0.0;
					int buff_zone=0;
					stringstream point_buff;
					point_buff << myline;
					point_buff >> buff_x >> buff_y>> buff_zone;

					if(point_buff.fail()==true){
						Error msg=this->set_error(15);
						ostringstream info;
						info << "Read line : " << myline << endl;
						msg.make_second_info(info.str());
						throw msg;
					}
					waterside_next.set_point_coordinate(buff_x, buff_y);
					waterside_next.set_zone_number(buff_zone);

					if(counter_point==0){
						this->ptr_segments[counter_point].set_points(this->ptr_base_point,&waterside_next);
						this->ptr_segments[counter_point].set_zone_number(waterside_next.get_zone_number());
						waterside_before=waterside_next;
					}
					else{
						this->ptr_segments[counter_point].set_points(&waterside_before,&waterside_next);
						this->ptr_segments[counter_point].set_zone_number(waterside_next.get_zone_number());
						waterside_before=waterside_next;
					}
					counter_point++;
					this->ptr_segments[counter_point].set_gradient_reduction();
				}
			}
			while(ifile->atEnd()==false && counter_point<this->number_points-2);

			this->ptr_segments[this->number_segments-1].set_points(&waterside_before,this->ptr_crest_point);
			this->ptr_segments[this->number_segments-1].set_zone_number(this->ptr_crest_point->get_zone_number());
		}

		if(counter_point!=this->number_points-2){
			Error msg=this->set_error(19);
			ostringstream info;
			info<<"Number points found in file : "<<counter_point<<endl;
			info<<"Number points required      : "<<this->number_points-2<<endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//check the cubature
		this->check_cubature();
		//idealize the cubature (just the geometrie; gamma_material is calculated with the real cubature)
		this->idealizise_cubature();
	}
	catch(Error msg){
		throw msg;
	}
	this->calc_total_length();
}
//Set the random variables of reduction factors of the wave runup mechanism (gamma berm)
void Fpl_Cub_Dike_Waterside::set_gamma_berm(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->gamma_berm_total.set_input(id, section_id, fpl_label::gamma_berms, true, ptr_database, fpl_label::mech_wave_runup, fpl_label::sec_dike, 0, false);
		this->gamma_berm_total.set_distribution_types(ptr_database);
	}
	catch(Error msg){
		throw msg;
	}
}
//Set the random variables of reduction factors of the wave runup mechanism (gamma berm) per file
void Fpl_Cub_Dike_Waterside::set_gamma_berm(QFile *ifile, int *line_counter){
	try{
		this->gamma_berm_total.set_input(ifile, line_counter, fpl_label::gamma_berms, true, fpl_label::end_mech_wave_runup);
		this->gamma_berm_total.check_given_boundaries(1.0,0.0);
	}
	catch(Error msg){
		throw msg;
	}
}
//Transfer the geomtrical data of the waterside dike cubature to database
void Fpl_Cub_Dike_Waterside::transfer_geometry2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	Fpl_Section_Points buffer;
	for(int i=0; i<this->number_segments-1; i++){
		buffer.set_point(this->ptr_segments[i].point2);
		buffer.set_zone_number(this->ptr_segments[i].get_zone_number());
		buffer.set_point_name(fpl_label::cub_waterside);
		buffer.transfer_point2database(id, section_id, ptr_database,  fpl_label::region_cubature, fpl_label::sec_dike);
	}
}
//Output the geometrie of the waterside to disply/console
void Fpl_Cub_Dike_Waterside::output_geometrie(void){
	ostringstream cout;
	cout<<"WATERSIDE CUBATURE GEOMETRY"<< endl;
	cout <<" Number points (segments)      : " << this->number_points <<" ("<<this->number_segments<<")"<< endl;
	cout <<" Horizontal distance           : " << this->total_horizontal << label::m <<endl;
	cout <<" Vertical distance             : " << this->total_vertical << label::m << endl;
	cout <<" Distance                      : " << this->total_length << label::m << endl;
	cout <<" Number ideal horizontal berms : " << this->number_horizontal_berms << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	cout<<"REAL SEGMENTS:"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	//output the segments
	for(int i=0;i<this->number_segments;i++){
		this->ptr_segments[i].output_members();
	}
	cout<<"IDEAL SEGMENTS:"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	//output the segments
	for(int i=0;i<this->number_ideal_segments;i++){
		this->ptr_ideal_segments[i].output_members();
	}
}
//Transfer the random variables of the waterside dike cubature to database
void Fpl_Cub_Dike_Waterside::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	this->gamma_berm_total.transfer_rand2database(ptr_database, id, section_id, fpl_label::mech_wave_runup, fpl_label::sec_dike);
}
//Output the members to display/console
void Fpl_Cub_Dike_Waterside::output_members(void){
	ostringstream cout;
	cout<<"WATERSIDE CUBATURE VARIABLES"<< endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->gamma_berm_total.output_member();
}
//Get the total distance of the waterside cubature
double Fpl_Cub_Dike_Waterside::get_total_distance(void){
	return this->total_length;
}
//Get the horizontal distance of the waterside cubature
double Fpl_Cub_Dike_Waterside::get_horizontal_distance(void){
	return this->total_horizontal;
}
//Get the vertical distance of the waterside cubature
double Fpl_Cub_Dike_Waterside::get_vertical_distance(void){
	return this->total_vertical;
}
//Get the dike height
double Fpl_Cub_Dike_Waterside::get_dike_height(void){
	return this->ptr_segments[this->number_segments-1].point2.get_ycoordinate();
}
//Get if there are berms at the waterside
bool Fpl_Cub_Dike_Waterside::get_with_berm_flag(void){
	return this->with_berm_flag;
}
//Get the total berm reduction factor of the wav runup mechanism (gamma berm) randomly distributed
double Fpl_Cub_Dike_Waterside::get_total_gamma_berm(const bool random_calculation){
	if(this->gamma_berm==1.0){//no calculation of the random varialbes because berm is out of influence or is a foreshore etc.
		return this->gamma_berm;
	}
	else{
		//set the total gamma_berm factore
		this->gamma_berm_total.set_new_middle(this->gamma_berm);
		this->gamma_berm=this->gamma_berm_total.calculate_variables(random_calculation).end_result;
		return this->gamma_berm;
	}
}
//Get the number of segments
int Fpl_Cub_Dike_Waterside::get_number_segments(void){
	return this->number_segments;
}
//Get a pointer to a segments specified with an index
Fpl_Cub_Dike_Segment_Waterside *Fpl_Cub_Dike_Waterside::get_segment(const int index){
	if(index<0 || index>this->number_segments-1){
		return NULL;
	}
	else{
		return &this->ptr_segments[index];
	}
}
//Get a pointer to the last point (:= crest waterside)
Geo_Point *Fpl_Cub_Dike_Waterside::get_ptr_last_point(void){
	return &this->ptr_segments[this->number_segments-1].point2;
}
//Reset the flags if the variables where randomly generated
void Fpl_Cub_Dike_Waterside::reset_random_flag(void){
	this->gamma_berm_total.reset_random_generated();
}
//Calculate the interception of a given line (e.g. waterlevel) and the cubature to get the relevant cubature; the search is upwards
void Fpl_Cub_Dike_Waterside::calculate_intercept_cubature_firstup(Geo_Straight_Line *ptr_relv_waterlevel, int *no_segment_intercept){
	bool interception_flag=false;
	//reset the interception point
	this->interception.interception_flag=false;
	this->interception.indefinite_flag=false;

	//first check if the line can intercept the cubature
	//it is under the base point (whole cubature is taken into account)
	if(ptr_relv_waterlevel->point1.get_ycoordinate()<= this->ptr_base_point->get_ycoordinate()){
		*no_segment_intercept=0;
		this->first_segment=this->ptr_segments[0];
		return;
	}//it is above the crest point (last segment is taken into accout)
	if(ptr_relv_waterlevel->point1.get_ycoordinate()> this->ptr_crest_point->get_ycoordinate()){
		*no_segment_intercept=this->number_segments;
		this->first_segment=this->ptr_segments[this->number_segments];
		return;
	}

	//search for the interception beginning from the given segment first upward
	for(int i=*no_segment_intercept; i< this->number_segments;i++){
		this->ptr_segments[i].calc_interception(ptr_relv_waterlevel, &this->interception);
		//interception is found
		if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){
			*no_segment_intercept=i;
			//set the first shorten segment
			first_segment=this->ptr_segments[i];
			//set the new beginning
			try{
				first_segment.set_points(&this->interception.interception_point, &this->ptr_segments[i].point2);
			}
			catch(Error msg){
				//change error to warning
				Warning msg2;
				msg2.error_to_warning(&msg, 9);
				msg2.set_reaction("Whole segment is taken");
				ostringstream info;
				info << "Interception between waterside cubature and horizontal lines like waterlevel etc. "<<endl;
				info << "Points are not correct " << endl;
				info << "Segmentnumber " << i << endl;
				msg2.make_second_info(info.str());
				msg2.output_msg(1);
				//reaction
				first_segment=this->ptr_segments[i];
			}
			interception_flag=true;
			break;
		}
		else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){
			*no_segment_intercept=i;
			//set the first shorten segment
			first_segment=this->ptr_segments[i];
			interception_flag=true;
			break;
		}
	}
	//if nothing is found: search for the interception beginning from the given segment downward
	if(interception_flag==false){
		for(int i=*no_segment_intercept; i>=0 ;i--){
			this->ptr_segments[i].calc_interception(ptr_relv_waterlevel, &this->interception);
			//interception is found
			if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){
				*no_segment_intercept=i;
				//set the first shorten segment
				first_segment=this->ptr_segments[i];
				//set the new beginning
				try{
					first_segment.set_points(&this->interception.interception_point, &this->ptr_segments[i].point2);
				}
				catch(Error msg){
					//change error to warning
					Warning msg2;
					msg2.error_to_warning(&msg, 9);
					msg2.set_reaction("Whole segment is taken");
					ostringstream info;
					info << "Interception between waterside cubature and horizontal lines like waterlevel etc. "<<endl;
					info << "Points are not correct " << endl;
					info << "Segmentnumber " << i << endl;
					msg2.make_second_info(info.str());
					msg2.set_during_calculation_flag(false);
					msg2.output_msg(1);
					//reaction
					first_segment=this->ptr_segments[i];
				}
				interception_flag=true;
				break;
			}
			else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){
				*no_segment_intercept=i;
				//set the first shorten segment
				first_segment=this->ptr_segments[i];
				interception_flag=true;
				break;
			}
		}
	}
	//still nothing is found
	if(interception_flag==false){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Waterlevel -1.5 *wave height = " << ptr_relv_waterlevel->point1.get_ycoordinate() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		//reaction
		first_segment=this->ptr_segments[0];
		*no_segment_intercept=0;
	}
}
//Calculate the interception of a given line (e.g. waterlevel) and the cubatureto get the relevant cubature; the search is downwards
void Fpl_Cub_Dike_Waterside::calculate_intercept_idealcubature_firstdown(Geo_Straight_Line *ptr_relv_waterlevel, int *no_segment_intercept){
	bool interception_flag=false;
	//reset the interception point
	this->interception.interception_flag=false;
	this->interception.indefinite_flag=false;

	//first check if the line can intercept the cubature
	//it is under the base point (whole cubature is taken into account)
	if(ptr_relv_waterlevel->point1.get_ycoordinate()<= this->ptr_base_point->get_ycoordinate()){
		*no_segment_intercept=0;
		this->first_segment=this->ptr_ideal_segments[0];
		return;
	}//it is above the crest point (last segment is taken into accout)
	if(ptr_relv_waterlevel->point1.get_ycoordinate()> this->ptr_crest_point->get_ycoordinate()){
		*no_segment_intercept=this->number_ideal_segments;
		this->first_segment=this->ptr_ideal_segments[this->number_ideal_segments];
		return;
	}

	//search for the interception beginning from the given segment first downward
	for(int i=*no_segment_intercept; i>=0; i--){
		this->ptr_ideal_segments[i].calc_interception(ptr_relv_waterlevel, &this->interception);
		//interception is found
		if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){
			*no_segment_intercept=i;
			//set the first shorten segment
			first_segment=this->ptr_ideal_segments[i];
			//set the new beginning
			try{
				first_segment.set_points(&this->interception.interception_point, &this->ptr_ideal_segments[i].point2);
			}
			catch(Error msg){
				//change error to warning
				Warning msg2;
				msg2.error_to_warning(&msg, 9);
				msg2.set_reaction("Whole segment is taken");
				ostringstream info;
				info << "Interception between waterside cubature and horizontal lines like waterlevel etc. "<<endl;
				info << "Points are not correct " << endl;
				info << "Segmentnumber " << i << endl;
				msg2.make_second_info(info.str());
				msg2.set_during_calculation_flag(false);
				msg2.output_msg(1);
				//reaction
				first_segment=this->ptr_ideal_segments[i];
			}
			interception_flag=true;
			break;
		}
		else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){
			*no_segment_intercept=i;
			//set the first shorten segment
			first_segment=this->ptr_segments[i];
			interception_flag=true;
			break;
		}
	}
	//if nothing is found search from the given segment upward
	if(interception_flag==false){
		for(int i=*no_segment_intercept; i< this->number_ideal_segments;i++){
			this->ptr_ideal_segments[i].calc_interception(ptr_relv_waterlevel, &this->interception);
			//interception is found
			if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){
				*no_segment_intercept=i;
				//set the first shorten segment
				first_segment=this->ptr_ideal_segments[i];
				//set the new beginning
				try{
					first_segment.set_points(&this->interception.interception_point, &this->ptr_ideal_segments[i].point2);
				}
				catch(Error msg){
					//change error to warning
					Warning msg2;
					msg2.error_to_warning(&msg, 9);
					msg2.set_reaction("Whole segment is taken");
					ostringstream info;
					info << "Interception between waterside cubature and horizontal lines like waterlevel etc. "<<endl;
					info << "Points are not correct " << endl;
					info << "Segmentnumber " << i << endl;
					msg2.make_second_info(info.str());
					msg2.set_during_calculation_flag(false);
					msg2.output_msg(1);
					//reaction
					first_segment=this->ptr_ideal_segments[i];
				}
				interception_flag=true;
				break;
			}
			else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){
				*no_segment_intercept=i;
				//set the first shorten segment
				first_segment=this->ptr_ideal_segments[i];
				interception_flag=true;
				break;
			}
		}
	}
	//still nothing is found
	if(interception_flag==false){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Waterlevel -relevant line = " << ptr_relv_waterlevel->point1.get_ycoordinate() << endl;
		msg.make_second_info(info.str());
		msg.output_msg(1);
		//reaction
		first_segment=this->ptr_ideal_segments[0];
		*no_segment_intercept=0;
	}
}
//Calculate gamma material factor at the real cubature for the mechanism of wave runup
double Fpl_Cub_Dike_Waterside::calculate_gamma_material(const double random_calculation, const double wave_height, const int no_segment_intercept){
	//reset the gamma value
	this->mid_gamma_material=1.0;

	//calculate the gamma material
	double sum_length=0.0;
	double sum_gamma_length=0.0;
	double determ_gamma=0.0;
	//first new shorten segment
	sum_length=this->first_segment.get_distance();
	//grass is directly calculated from the wave height (TAW Wave runup and wave overtopping, 2002)
	/*if(this->first_segment.ptr_gamma_f->get_default_name()==fpl_label::material_roughness_grass){
		determ_gamma=1.15*pow(wave_height,0.5);
		if(determ_gamma>1.0){
			determ_gamma=1.0
		}
		this->first_segment.ptr_gamma_f->set_new_middle(determ_gamma);
	}*/
	determ_gamma=this->first_segment.ptr_mat_zone->roughness.get_result().end_result;
	sum_gamma_length=this->first_segment.get_distance()*determ_gamma;

	//rest of the segments
	for(int i=no_segment_intercept+1; i<this->number_segments;i++){
		sum_length=sum_length+this->ptr_segments[i].get_distance();
		//grass is directly calculated from the wave height (TAW Wave runup and wave overtopping, 2002)
		/*if(this->ptr_segments[i].ptr_gamma_f->get_default_name()==fpl_label::material_roughness_grass){
			determ_gamma=1.15*pow(wave_height,0.5);
			if(determ_gamma>1.0){
				determ_gamma=1.0
			}
			this->ptr_segments[i].ptr_gamma_f->set_new_middle(determ_gamma);
		}*/
		//make it random
		determ_gamma=this->ptr_segments[i].ptr_mat_zone->roughness.get_result().end_result;
		//sum it up
		sum_gamma_length=sum_gamma_length+this->ptr_segments[i].get_distance()*determ_gamma;
	}
	this->mid_gamma_material=sum_gamma_length/sum_length;

	return this->mid_gamma_material;
}
//Calculate the middle of the gradient at the real cubature for the mechanism of wave runup
double Fpl_Cub_Dike_Waterside::calculate_mid_relevant_gradient(const int no_segment_intercept){
	double dist_horizontal=0.0;
	double dist_vertical=0.0;
	double mid_gradient=0.0;

	//the first shorten segment
	dist_horizontal=this->first_segment.get_relevant_horizontal();
	dist_vertical=this->first_segment.get_relevant_vertical();
	//loop over the real relevant segments
	for(int i=no_segment_intercept+1; i<this->number_segments;i++){
		//sum it up over the segments; the values are already reduced for H/V<1/8
		dist_horizontal=dist_horizontal+this->ptr_segments[i].get_relevant_horizontal();
		dist_vertical=dist_vertical+this->ptr_segments[i].get_relevant_vertical();
	}
	//calculate mid gradient
	if(dist_horizontal!=0){
		mid_gradient=dist_vertical/dist_horizontal;
	}
	else{//quasi vertical (normaly not possible; see check_cubature)
		mid_gradient=10e10;
	}
	return mid_gradient;
}
//Calculate the middle of the gradient at the waterside cubature
double Fpl_Cub_Dike_Waterside::calculate_mid_gradient(void){
	double dist_horizontal=0.0;
	double dist_vertical=0.0;
	double mid_gradient=0.0;

	//loop over the real relevant segments
	for(int i=0; i<this->number_segments;i++){
		//sum it up over the segments
		dist_horizontal=dist_horizontal+this->ptr_segments[i].get_horizontal_dist();
		dist_vertical=dist_vertical+this->ptr_segments[i].get_vertical_dist();
	}
	//calculate mid gradient
	if(dist_horizontal!=0){
		mid_gradient=dist_vertical/dist_horizontal;
	}
	else{//quasi vertical (normaly not possible; see check_cubature)
		mid_gradient=10e10;
	}
	return mid_gradient;
}
//Calculate the gamma berm and the waveheight reduction factor for a berm or a foreshore at the idealisized cubature for the mechanism of wave runup
void Fpl_Cub_Dike_Waterside::calculate_gammaberm_waveheightreduction(const double wave_length, double *wave_height, const double waterlevel){
	this->gamma_berm=1.0;

	for(int i=0;i<this->number_ideal_segments;i++){
		if(this->ptr_ideal_segments[i].get_is_horizontal_berm()==true){
			//decide what the segment is:foreshore or berm or in between
			this->ptr_ideal_segments[i].set_foreshore_flag(wave_length);
			//set the distance between waterlevel and berm/foreshore
			this->ptr_ideal_segments[i].set_vertical_water2berm(waterlevel);

			//it is a foreshore so no influence lenght needs to be calculated
			if(this->ptr_ideal_segments[i].get_berm_data()->foreshore==true && this->ptr_ideal_segments[i].get_berm_data()->foreshore_berm==false){
				this->ptr_ideal_segments[i].calculate_waveheight_reduction(*wave_height);
				//reduce the wave height
				*wave_height=(*wave_height)*this->ptr_ideal_segments[i].get_waveheight_reduction();
			}
			//is between a berm and a foreshore
			else if(this->ptr_ideal_segments[i].get_berm_data()->foreshore==false && this->ptr_ideal_segments[i].get_berm_data()->foreshore_berm==true){
				this->ptr_ideal_segments[i].calculate_waveheight_reduction(*wave_height);
				this->ptr_ideal_segments[i].interpolate_waveheight_reduction(wave_length);
				//reduce the wave height
				*wave_height=(*wave_height)*this->ptr_ideal_segments[i].get_waveheight_reduction();

				//calculate the influence length
				this->calculate_influence_length_berm(*wave_height , i);
				this->ptr_ideal_segments[i].calculate_gamma_berm(*wave_height, this->ptr_crest_point->get_ycoordinate(), waterlevel);
				this->ptr_ideal_segments[i].interpolate_gamma_berm(wave_length);
			}
			//it is a berm
			else if(this->ptr_ideal_segments[i].get_berm_data()->foreshore==false && this->ptr_ideal_segments[i].get_berm_data()->foreshore_berm==false){
				//calculate the influence length of berm
				this->calculate_influence_length_berm(*wave_height , i);
				this->ptr_ideal_segments[i].calculate_gamma_berm(*wave_height, this->ptr_crest_point->get_ycoordinate(), waterlevel);
			}
			//multiplicate the gamma berms to a total gamm berm
			this->gamma_berm=this->gamma_berm*this->ptr_ideal_segments[i].get_gamma_berm();
		}
	}//end of loop over the ideal segments
}
//Calculate the contration height for the overflow mechanism
void Fpl_Cub_Dike_Waterside::calculate_contraction_height(const double delta_h, double *contraction_height){
	// make the interpolation
	/*double b=0.0;
	double m=0.0;*/
	//reset the interpolated heights and the ground flags of the ground
	this->part_contraction[0].interpolated_height=this->part_contraction[0].height;
	this->part_contraction[0].ground_flag=true;
	// set the interpolated height of the rest to the real height
	for(int i=1;i<this->number_horizontal_berms+1;i++){
		this->part_contraction[i].interpolated_height=this->part_contraction[i].height;
		this->part_contraction[i].ground_flag=false;
	}

	//claculate per interpolation
	for(int j=this->number_horizontal_berms; j>0;j--){
		//check if the bermwidth is wide enough
		if(this->part_contraction[j].effective_width/(delta_h) > 2.5){
			this->part_contraction[j].ground_flag=true;
			break;
		}
		else{
			//interpolated if berm are not wide enough
			/*for(int i=0; i< j; i++){
				m=(this->part_contraction[j].interpolated_height-this->part_contraction[i].interpolated_height)/(2.5-0.0);
				b=this->part_contraction[i].interpolated_height;
				this->part_contraction[i].interpolated_height=m*this->part_contraction[j].effective_width/(delta_h)+b;
			}*/
		}
	}

	//evaluate the result
	for(int j=0; j< this->number_horizontal_berms+1; j++){
		if(this->part_contraction[j].ground_flag==true){
			*contraction_height=this->part_contraction[j].interpolated_height;
		}
	}
}
//Calculate the relevant slope for the overflow mechanism at the ideal cubature
void Fpl_Cub_Dike_Waterside::calculate_weir_slope(double *weir_slope, const int no_segment_intercept){
	double dist_horizontal=0.0;
	double dist_vertical=0.0;
	double reduction_factor=1.0;
	int counter_berm=1;
	//make the calculation beginning from the crest
	for(int i=this->number_ideal_segments-1; i>=no_segment_intercept; i-- ){
		//sum it up over the segments; the values are already reduced for H/V<1/8
		dist_horizontal=dist_horizontal+this->ptr_ideal_segments[i].get_relevant_horizontal()*reduction_factor;
		dist_vertical=dist_vertical+this->ptr_ideal_segments[i].get_relevant_vertical()*reduction_factor;
		//set the reduction factor for the slope
		if(this->ptr_ideal_segments[i].get_is_horizontal_berm()==true){
			//the following slopes not more taken into account
			if(this->part_contraction[this->number_horizontal_berms+1-counter_berm].ground_flag==true){
				break;
			}
			else{//the following slopes are reduced
				reduction_factor=this->part_contraction[this->number_horizontal_berms-counter_berm].interpolated_height/this->part_contraction[this->number_horizontal_berms-counter_berm].height;
				counter_berm++;
			}
		}
	}

	//calculate mid gradient
	if(dist_horizontal!=0){
		*weir_slope=dist_vertical/dist_horizontal;
	}
	else{//quasi vertical (normaly not possible; see check_cubature)
		*weir_slope=10e10;
	}
}
//Check the statistic of the random generation process
void Fpl_Cub_Dike_Waterside::check_statistic(void){
	this->gamma_berm_total.check_statistic();
}
//Reset the statistic important values
void Fpl_Cub_Dike_Waterside::reset_statistics(void){
	this->gamma_berm_total.reset_statistics();
}
//Output the statistics of the random generation process to display/console
void Fpl_Cub_Dike_Waterside::output_statistic(void){
	this->gamma_berm_total.output_statistic();
}
//Output the deterministic results to display/console
void Fpl_Cub_Dike_Waterside::output_determ_results(void){
	ostringstream cout;
	cout << "Mid of gamma_material " << this->mid_gamma_material << label::no_unit <<endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(),false);
}

//__________
//private
//Allocate the segments of the real cubature
void Fpl_Cub_Dike_Waterside::allocate_segment_real(void){
	try{
		this->ptr_segments=new Fpl_Cub_Dike_Segment_Waterside[this->number_segments];
	}
	catch(bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the segments of the real cubature
void Fpl_Cub_Dike_Waterside::delete_segment_real(void){
	if(this->ptr_segments!=NULL){
		delete []this->ptr_segments;
		this->ptr_segments=NULL;
	}
}
//Allocate the segments of the ideal cubature
void Fpl_Cub_Dike_Waterside::allocate_segment_ideal(void){
	try{
		this->ptr_ideal_segments=new Fpl_Cub_Dike_Segment_Waterside[this->number_ideal_segments];
	}
	catch(bad_alloc &t){
		Error msg=set_error(4);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the segments of the ideal cubature
void Fpl_Cub_Dike_Waterside::delete_segment_ideal(void){
	if(this->ptr_ideal_segments!=NULL){
		delete []this->ptr_ideal_segments;
		this->ptr_ideal_segments=NULL;
	}
}
//Allocate the line for calculation of the berm influence
void Fpl_Cub_Dike_Waterside::allocate_berm_influence_line(void){
	try{
		this->line_influence_berm=new Geo_Straight_Line;
	}
	catch(bad_alloc &t){
		Error msg=set_error(10);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the line for calculation of the berm influence
void Fpl_Cub_Dike_Waterside::delete_berm_influence_line(void){
	if(this->line_influence_berm!=NULL){
		delete this->line_influence_berm;
		this->line_influence_berm=NULL;
	}
}
//Allocate the line for calculation of the contraction factor
void Fpl_Cub_Dike_Waterside::allocate_contraction_factor(void){
	try{
		this->part_contraction=new _fpl_contraction_height[this->number_horizontal_berms+1];
		Sys_Memory_Count::self()->add_mem(sizeof(_fpl_contraction_height)*this->number_horizontal_berms+1,_sys_system_modules::FPL_SYS);//count the memory
	}
	catch(bad_alloc &t){
		Error msg=set_error(12);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the line for calculation of the contraction factor
void Fpl_Cub_Dike_Waterside::delete_contraction_factor(void){
	if(this->part_contraction!=NULL){
		delete []this->part_contraction;
		Sys_Memory_Count::self()->minus_mem(sizeof(_fpl_contraction_height)*this->number_horizontal_berms+1,_sys_system_modules::FPL_SYS);//count the memory
		this->part_contraction=NULL;
	}
}
//Check the cubature segments
void Fpl_Cub_Dike_Waterside::check_cubature(void){
	for(int i=0; i< this->number_segments; i++){
		if(this->ptr_segments[i].get_gradient()>1.0){//gradient greater than 1:1
			Error msg=this->set_error(6);
			ostringstream info;
			info << "Segmentnumber " << i << " with a gradient of " << this->ptr_segments[i].get_gradient() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		if(this->ptr_segments[i].get_gradient()<0.0){//negative gradient
			Error msg=this->set_error(7);
			ostringstream info;
			info << "Segmentnumber " << i << " with a gradient of " << this->ptr_segments[i].get_gradient() << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}//first segment have to be >=1/8
	if(this->ptr_segments[0].get_gradient()<1.0/8.0){
			Error msg=this->set_error(8);
			ostringstream info;
			info << "The gradient is " << this->ptr_segments[0].get_gradient() << endl;
			msg.make_second_info(info.str());
			throw msg;
	}//last segment have to be >=1/8
	if(this->ptr_segments[this->number_segments-1].get_gradient()<1.0/8.0){
			Error msg=this->set_error(9);
			ostringstream info;
			info << "The gradient is " << this->ptr_segments[this->number_segments-1].get_gradient() << endl;
			msg.make_second_info(info.str());
			throw msg;
	}
}
//Idealizied the cubature (ideal cubature: berms are combined and turned to horizontal)
void Fpl_Cub_Dike_Waterside::idealizise_cubature(void){
	int counter=0;
	//reset the ideal cubature
	this->delete_segment_ideal();
	this->number_ideal_segments=0;

	///set the new ideal cubature
	//all segments where the gradient is smaller than 1/8 and which are connected are combined to one

	//first count it how much new ideal segments
	for(int i=1;i< this->number_segments-1; i++){
		if(this->ptr_segments[i].get_gradient()>=1.0/8.0){//slopes
			counter++;
		}
		else{//combine the connected berm segments
			while(this->ptr_segments[i+1].get_gradient()<1.0/8.0 && i<this->number_segments-1){
				i++;
			}
			counter++;
		}
	}
	//count 1 for the first and last segment
	counter=counter+2;
	this->number_ideal_segments=counter;

	//second allocate a new ideal cubature
	try{//create a new ideal cubature
		this->allocate_segment_ideal();
	}
	catch(Error msg){
		throw msg;
	}
	//third set the ideal segments
	//the first segment is the same
	this->ptr_ideal_segments[0]=this->ptr_segments[0];
	counter=1;
	for(int i=1;i< this->number_segments-1; i++){
		if(this->ptr_segments[i].get_gradient()>=1.0/8.0){//slopes
			this->ptr_ideal_segments[counter]=this->ptr_segments[i];
			counter++;
		}
		else{//the first point of the new ideal segment is set
			Geo_Point begin_point;
			Geo_Point end_point;
			begin_point=this->ptr_segments[i].point1;
			//search for the last point
			while(this->ptr_segments[i+1].get_gradient()<1.0/8.0 && i<this->number_segments-1){
				i++;
			}
			//set the last point of the ideal segment
			end_point=this->ptr_segments[i].point2;
			this->ptr_ideal_segments[counter].set_points(&begin_point,&end_point);
			counter++;
		}
	}
	//the last segment is the same
	this->ptr_ideal_segments[this->number_ideal_segments-1]=this->ptr_segments[this->number_segments-1];

	//fourth
	//store the real horizontal and vertical distances before the berm are turned
	for (int i=0;i< this->number_ideal_segments; i++){
		this->ptr_ideal_segments[i].set_gradient_reduction();
	}
	//turn segments which are berm segments
	for (int i=1;i< this->number_ideal_segments-1; i++){
		try{
			this->ptr_ideal_segments[i].turn_berm_segment(&this->ptr_ideal_segments[i-1], &this->ptr_ideal_segments[i+1]);
			//set the with berm_flag and count the berms
			if(this->ptr_ideal_segments[i].get_is_horizontal_berm()==true){
				this->with_berm_flag=true;
				this->number_horizontal_berms++;
			}
		}
		catch(Error msg){
			throw msg;
		}
	}
	//allocate a line for the calculation of the influence width of the berm; it is needed for the calculation of influence length
	if(this->with_berm_flag==true){
		try{
			this->allocate_berm_influence_line();
		}
		catch(Error msg){
			throw msg;
		}
	}
	//prepare the contraction heights of berms for the overflow calculation
	this->prepare_contraction_heights();
}
//Calculate the total length as well as the vertical and horizontal distance
void Fpl_Cub_Dike_Waterside::calc_total_length(void){
	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;

	for(int i=0;i<this->number_segments;i++){
		//set the points to a segment
		this->total_length=this->total_length+this->ptr_segments[i].get_distance();
		this->total_horizontal=this->total_horizontal+this->ptr_segments[i].get_horizontal_dist();
		this->total_vertical=this->total_vertical+this->ptr_segments[i].get_vertical_dist();
	}
}
//Set the pointer to the base point and the crest point
void Fpl_Cub_Dike_Waterside::set_base_crest_point(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point){
	this->ptr_base_point=base_point;
	this->ptr_crest_point=crest_point;
}
//Calculate the influence length of a berm for the mechanism wave-runup
void Fpl_Cub_Dike_Waterside::calculate_influence_length_berm(const double waveheight, const int index_berm){
	//reset the interception point
	this->interception.interception_flag=false;
	this->interception.indefinite_flag=false;
	double y_influence=0.0;

	//search for the interception point to calculate the influence length of the berm
	//first downward the berm=> left influnce length
	y_influence=this->ptr_ideal_segments[index_berm].point1.get_ycoordinate()-waveheight;
	if(y_influence<=this->ptr_base_point->get_ycoordinate()){//is below the base point=> x-base point is taken
		this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_left=abs(this->ptr_base_point->get_xcoordinate()-this->ptr_ideal_segments[index_berm].point1.get_xcoordinate());
	}
	else{//search for the interception point downward; is on the left side of the berm
		//make a horizontal line with the y_influence height
		this->line_influence_berm->set_coordinates(this->ptr_base_point->get_xcoordinate(),y_influence);
		for(int j=index_berm;j>=0;j--){
			this->ptr_ideal_segments[j].calc_interception(this->line_influence_berm, &this->interception);
			//interception have to be found because the line is between the base point and the berm
			if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){//normal interception
				this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_left=abs(this->interception.interception_point.get_xcoordinate()-this->ptr_ideal_segments[index_berm].point1.get_xcoordinate());
			}
			else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){//interception at another horizontal berm (end of berm is taken)!downward
				this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_left=abs(this->ptr_ideal_segments[j].point2.get_xcoordinate()-this->ptr_ideal_segments[index_berm].point1.get_xcoordinate());
			}
		}
	}
		//second upward search the berm=> right influence length
	y_influence=this->ptr_ideal_segments[index_berm].point1.get_ycoordinate()+waveheight;
	if(y_influence>=this->ptr_crest_point->get_ycoordinate()){//is above the crest point=> x-crest point is taken
		this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_right=abs(this->ptr_crest_point->get_xcoordinate()-this->ptr_ideal_segments[index_berm].point2.get_xcoordinate());
	}
	else{//search for the interception point upward => right influence length
		this->line_influence_berm->set_coordinates(this->ptr_base_point->get_xcoordinate(),y_influence);
		for(int j=index_berm;j<this->number_ideal_segments;j++){
			this->ptr_ideal_segments[j].calc_interception(this->line_influence_berm, &this->interception);
			//interception have to be found because the line is between the berm and the crest point
			if(this->interception.interception_flag==true && this->interception.indefinite_flag==false){//normal interception
				this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_right=abs(this->interception.interception_point.get_xcoordinate()-this->ptr_ideal_segments[index_berm].point2.get_xcoordinate());
			}
			else if(this->interception.interception_flag==false && this->interception.indefinite_flag==true){//interception at another horizontal berm (begin of berm is taken)!upward
				this->ptr_ideal_segments[index_berm].get_berm_data()->x_influence_right=abs(this->ptr_ideal_segments[j].point1.get_xcoordinate()-this->ptr_ideal_segments[index_berm].point2.get_xcoordinate());
			}
		}
	}
}
//Prepare the calculation of a partly contraction height for the overflow mechanism
void Fpl_Cub_Dike_Waterside::prepare_contraction_heights(void){
	//allocate the part_contraction_heights; needed for overflow calculation
	try{
		this->allocate_contraction_factor();
	}
	catch(Error msg){
		throw msg;
	}
	//first the for contraction height to the ground
	this->part_contraction[0].height=this->ptr_crest_point->get_ycoordinate();
	this->part_contraction[0].effective_width=1e20;
	this->part_contraction[0].interpolated_height=this->part_contraction[0].height;
	this->part_contraction[0].ground_flag=true;
	int counter=1;
	//seond search for other horizontal berms
	for (int i=1; i < this->number_ideal_segments-1; i++){
		if(this->ptr_ideal_segments[i].get_is_horizontal_berm()==true){
			this->part_contraction[counter].height=this->ptr_crest_point->get_ycoordinate()-this->ptr_ideal_segments[i].point1.get_ycoordinate();
			//effective witdh is *1 if the real gradient == <1/15 and between >1/15 and < 1/8 it is interpolated
			this->part_contraction[counter].effective_width=this->ptr_ideal_segments[i].get_horizontal_dist()*(1.0-this->ptr_ideal_segments[i].get_gradient_reduction());
			this->part_contraction[counter].interpolated_height=this->part_contraction[counter].height;
			this->part_contraction[counter].ground_flag=false;
			counter++;
		}
	}
}
//Find the key-values for the file input of the geometrical points and the material zone variables
void Fpl_Cub_Dike_Waterside::find_key_values_file(const string myline){
	int pos=-1;
	string buffer;
	stringstream buffer1;
	bool wrong_input=false;

	pos=myline.find(fpl_label::key_no_points);
	if(pos>=0 && wrong_input==false){
		buffer=myline.substr(fpl_label::key_no_points.length());
		functions::clean_string(&buffer);
		buffer1 << buffer;
		buffer1 >> this->number_points;
		if(buffer1.fail()==true){
			wrong_input=true;
		}
		else{
			return;
		}
	}

	if(wrong_input==true){
		ostringstream info;
		info << "Wrong input sequenze  : " << buffer1.str() << endl;
		Error msg=this->set_error(14);
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Fpl_Cub_Dike_Waterside::set_error(const int err_type){
		string place="Fpl_Cub_Dike_Waterside::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("allocate_segment_real(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://no results found (non fatal error)
			place.append("set_zone_material(QSqlDatabase *ptr_database, const _section_id id)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://the fetches are not correctly ordered
			place.append("connect_zone2geometry");
			reason="The waterside segment can not be connected to the material zones";
			help="Check the waterside cubature and the random variables of the material zones";
			type=11;
			break;
		case 4://bad_alloc
			place.append("allocate_segment_ideal(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 5://bad_alloc
			place.append("calculate_relevant_ideal_cubature(Geo_Straight_Line *ptr_relv_waterlevel, const double wave_height)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 6://do not find any interception point
			place.append("check_cubature(void)");
			reason="The gradient of the segments is greater than 1; it is not corresponding to wave_run up of a dike section";
			help="Check the waterside cubature";
			type=11;
			break;
		case 7://do not find any interception point
			place.append("check_cubature(void)");
			reason="The gradient of the segments is smaller than 0; it is not corresponding to wave_run up of a dike section";
			help="Check the waterside cubature";
			type=11;
			break;
		case 8://do not find any interception point
			place.append("check_cubature(void)");
			reason="The gradient of the first segments is smaller than 1/8; it is not corresponding to wave_run up of a dike section";
			help="Check the waterside cubature; first segmenst have to be a slope >1/8 and < 1/1";
			type=11;
			break;
		case 9://do not find any interception point
			place.append("check_cubature(void)");
			reason="The gradient of the last segments is smaller than 1/8; it is not corresponding to wave_run up of a dike section";
			help="Check the waterside cubature; last segmenst have to be a slope >1/8 and < 1/1";
			type=11;
			break;
		case 10://bad_alloc
			place.append("allocate_berm_influence_line(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 11://bad_alloc
			place.append("allocate_zone_material(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 12://bad_alloc
			place.append("allocate_contraction_factor(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 14://wrong input
			place.append("find_key_values_file(const string myline)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the information of the waterside cubature/mechanism wave-runup in file";
			type=1;
			break;
		case 15://wrong input
			place.append("set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter)");
			reason="There is a problem with the input; wrong sign is read in";
			help="Check the points of the waterside cubature in file";
			type=1;
			break;
		case 16://do not find the begin of the general section information in file
			place.append("set_zone_material(QFile *ifile, int *line_counter)");
			reason="Can not find the begin of the material zone information in file";
			help="The material zone information has to start with !$BEGIN_ROUGHNESS";
			type=1;
			break;
		case 17://do not find the number of material zone
			place.append("set_zone_material(QFile *ifile, int *line_counter)");
			reason="Can not find the number of material zones in file";
			help="The number of material zones has to defined before the material information with !no_material";
			type=1;
			break;
		case 18://not all material zones found
			place.append("set_zone_material(QFile *ifile, int *line_counter)");
			reason="Not all required material zones are found";
			help="Check the material zones of the wave-runup generation mechanism in file";
			type=1;
			break;
		case 19://not all points found in file
			place.append("set_segments(Fpl_Section_Points *base_point, Fpl_Section_Points *crest_point, QFile *ifile, int *line_counter)");
			reason="Not all points are found in file";
			help="Check the information of the waterside cubature geometry in file";
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
//Set warning(s)
Warning Fpl_Cub_Dike_Waterside::set_warning(const int warn_type){
		string place="Fpl_Cub_Dike_Waterside::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://do not find any interception point
			place.append("calculate_intercept_...cubature...(Geo_Straight_Line *ptr_relv_waterlevel)");
			reason="Do not find any interception point with the waterside cubature";
			help="Check the waterside cubature and the relevant line";
			reaction="The whole cubature is taken";
			msg.set_during_calculation_flag(false);
			type=9;
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
};