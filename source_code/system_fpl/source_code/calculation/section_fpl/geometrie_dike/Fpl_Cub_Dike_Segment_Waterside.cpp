//#include "Fpl_Cub_Dike_Segment_Waterside.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
Fpl_Cub_Dike_Segment_Waterside::Fpl_Cub_Dike_Segment_Waterside(void){
	this->berm_flag=false;
	this->berm_mid_gradient_flag=false;
	this->berm_horizontal_flag=false;
	this->ptr_mat_zone=NULL;
	this->zone_number=0;
	this->gradient_reduction=1.0;
	this->gamma_berm=1.0;
	this->waveheight_reduction=1.0;
	this->hori_berm_data=NULL;
	this->sin_alpha=0.0;
	this->erosion_start=false;
	this->erosion_failed=false;
	this->wave_impact_failed=false;
	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Segment_Waterside)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//Copy constructor
Fpl_Cub_Dike_Segment_Waterside::Fpl_Cub_Dike_Segment_Waterside(Fpl_Cub_Dike_Segment_Waterside &object){
	Geo_Segment::operator =(object);
    this->hori_berm_data=NULL;
	this->berm_flag=object.berm_flag;
	this->berm_mid_gradient_flag=object.berm_mid_gradient_flag;
	this->berm_horizontal_flag=object.berm_horizontal_flag;
	this->ptr_mat_zone=object.ptr_mat_zone;
	this->zone_number=object.zone_number;
	this->gradient_reduction=object.gradient_reduction;
	this->sin_alpha=object.sin_alpha;
	this->erosion_start=object.erosion_start;
	this->erosion_failed=object.erosion_failed;
	this->wave_impact_failed=object.wave_impact_failed;
	
	if(object.hori_berm_data!=NULL){
		this->allocate_berm_data();
		this->hori_berm_data=object.hori_berm_data;
	}
	else{
		this->hori_berm_data=NULL;
	}

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Cub_Dike_Segment_Waterside)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//Default destructor
Fpl_Cub_Dike_Segment_Waterside::~Fpl_Cub_Dike_Segment_Waterside(void){
	if(this->hori_berm_data!=NULL){
		delete this->hori_berm_data;
		this->hori_berm_data=NULL;	
	}
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Cub_Dike_Segment_Waterside)-sizeof(Geo_Segment), _sys_system_modules::FPL_SYS);//count the memory
}
//___________________
//public
//Set the points of the segment
void Fpl_Cub_Dike_Segment_Waterside::set_points(Geo_Point *point1, Geo_Point *point2){
	Geo_Segment::set_points(point1, point2);
	if(this->point1.check_left_of(&this->point2)==false){
		
		this->switch_points();
		
		/*else{
			ostringstream info;
			info<<"x-coordinate point 1 : "<< this->point1.get_xcoordinate() <<endl;
			info<<"x-coordinate point 2 : "<< this->point2.get_xcoordinate() <<endl;
			
			Error msg=this->set_error(0);
			msg.make_second_info(info.str());
			throw msg;
		}*/
	}
	if(this->point1.get_ycoordinate()!=this->point2.get_ycoordinate() && this->point1.check_below_of(&this->point2)==false){
		Error msg=this->set_error(1);
		throw msg;
	}
	try{
		this->check_segment_for_berm();
	}
	catch(Error msg){
		throw msg;
	}

	this->sin_alpha=this->vertical_dist/this->distance;
}
//Set the zone number of the segment
void Fpl_Cub_Dike_Segment_Waterside::set_zone_number(const int zone_number){
	this->zone_number=zone_number;
}
//Set the gradient reduction before the berm is turned to horizontal
void Fpl_Cub_Dike_Segment_Waterside::set_gradient_reduction(void){
	//segment is a horizontal berm m almost 0.0
	if(this->get_gradient()<=(1.0/15.0)){
		this->gradient_reduction=0.0;
	}
	//interpolated gradient reduction => segment is in the transition zone between a slope (m>=1/8) and a berm (m<=1/15)
	else if(this->get_gradient()>(1.0/15.0) && this->get_gradient()<(1.0/8.0)){
		double b=1.0/(1.0-15.0/8.0);
		double m=-b*15.0;
		this->gradient_reduction=m*this->get_gradient()+b;
	}
	//segment is a slope; gradient is not reduced
	else{
		this->gradient_reduction=1.0;
	}
	//check negative values and correct it
	if(this->gradient_reduction<0.0){
		this->gradient_reduction=0.0;
	}
}
//Turn the berm segment to horizontal
void Fpl_Cub_Dike_Segment_Waterside::turn_berm_segment(Fpl_Cub_Dike_Segment_Waterside *segment_before, Fpl_Cub_Dike_Segment_Waterside *segment_after){
	double grad_before_turning=0.0;
	//go out if it is no berm segment
	if(this->berm_flag==false){
		return;
	}
	else{
		//set the gradient before the turning
		grad_before_turning=this->get_gradient();
		//first segment before
		Geo_Point mid_point;
		_geo_interception_point interception_before;
		mid_point=this->calc_mid_point();
		Geo_Straight_Line slope;
		slope.set_points(&segment_before->point1 , &segment_before->point2);
		//set the berm and turn it (horizontal at the mid point)
		Geo_Straight_Line berm;
		berm.set_points(&mid_point);

		//calculate the new interception with the segment which is before
		berm.calc_interception(&slope, &interception_before);
		//no interception found
		if(interception_before.interception_flag!=true){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Interception with the Segment before the berm with name " << segment_before->get_name();
			msg.make_second_info(info.str());
			throw msg;
		}
		//set the second point of the before berm segment to the interception point
		segment_before->set_points(&segment_before->point1,&interception_before.interception_point);

		//segment after
		//set the straight line to the segment after
		slope.set_points(&segment_after->point1, &segment_after->point2);
		//calculate the interception with the segment after
		_geo_interception_point interception_after;
		berm.calc_interception(&slope, &interception_after);
		//no interception found
		if(interception_after.interception_flag!=true){
			Error msg=this->set_error(3);
			ostringstream info;
			info << "Interception with the Segment before the berm with name " << segment_after->get_name();
			msg.make_second_info(info.str());
			throw msg;
		}
		//set the first point of the after berm segment to the interception point
		segment_after->set_points(&interception_after.interception_point, &segment_after->point2);

		//set the berm segment
		this->set_points(&interception_before.interception_point, &interception_after.interception_point);
		//check if it is horizontal
		if(this->berm_horizontal_flag==true){
			this->hori_berm_data->real_gradient=grad_before_turning;
		}
	}
}
//Get the zone number
int Fpl_Cub_Dike_Segment_Waterside::get_zone_number(void){
	return this->zone_number;
}
//Get the gradient reduction
double Fpl_Cub_Dike_Segment_Waterside::get_gradient_reduction(void){
	return this->gradient_reduction;
}
//Get the relevant vertical distance before the berms was turned
double Fpl_Cub_Dike_Segment_Waterside::get_relevant_vertical(void){
	return this->vertical_dist*this->gradient_reduction;
}
//Get the relevant horizontal distance before the berms was turned
double Fpl_Cub_Dike_Segment_Waterside::get_relevant_horizontal(void){
	return this->horizontal_dist*this->gradient_reduction;
}
//Get if this is a berm segment
bool Fpl_Cub_Dike_Segment_Waterside::get_is_berm(void){
	if(this->berm_flag==true || this->berm_mid_gradient_flag==true){
		return true;
	}
	else{
		return false;
	}
}
//Get if it is an horizontal berm
bool Fpl_Cub_Dike_Segment_Waterside::get_is_horizontal_berm(void){
	return this->berm_horizontal_flag;
}
//Get the pointer to the berm data
_fpl_horizontal_berm_data* Fpl_Cub_Dike_Segment_Waterside::get_berm_data(void){
	return this->hori_berm_data;
}
//Set the foreshore flag
void Fpl_Cub_Dike_Segment_Waterside::set_foreshore_flag(const double wave_length){
	if(this->berm_horizontal_flag==true){
		//it is a forshore
		if(this->get_horizontal_dist()/wave_length>=1.0){//it is a foreshore

			this->hori_berm_data->foreshore=true;
			this->hori_berm_data->foreshore_berm=false;	
		}
		//it is in the transition zone between berm and foreshore
		else if(this->get_horizontal_dist()/wave_length<1.0 && this->get_horizontal_dist()/wave_length>=0.25){//between berm and foreshore
			//calculate the berm
			this->hori_berm_data->foreshore=false;
			this->hori_berm_data->foreshore_berm=true;
		}
		//it is a just berm
		else{
			this->hori_berm_data->foreshore=false;
			this->hori_berm_data->foreshore_berm=false;
		}
	}
}
//Set the vertical distance between berm/foreshore and waterlevel
void Fpl_Cub_Dike_Segment_Waterside::set_vertical_water2berm(const double waterlevel){
	//set the vertical distance between waterlevel and berm 
	this->hori_berm_data->distance_water2berm=waterlevel-this->point1.get_ycoordinate();
}
//Calculate berm reduction factor (gamma berm)
void Fpl_Cub_Dike_Segment_Waterside::calculate_gamma_berm(const double wave_height, const double crest_height, const double waterlevel){
	
	//set the vertical distance between waterlevel and crest height
	this->hori_berm_data->max_runup=crest_height-waterlevel;
	
	//set r_b=b_berm/Lberm; (Lberm=influence lenght)
	this->hori_berm_data->influence_lenght_berm=(this->get_horizontal_dist()+this->hori_berm_data->x_influence_left+this->hori_berm_data->x_influence_right);
	this->hori_berm_data->r_b=this->get_horizontal_dist()/this->hori_berm_data->influence_lenght_berm;
	
	//set r_db
	if(this->hori_berm_data->distance_water2berm<=0.0){//berm is above the still water line
		//crest height is taken as the wave_runup
		this->hori_berm_data->r_db=0.5-0.5*cos(constant::Cpi*this->hori_berm_data->distance_water2berm/(this->hori_berm_data->max_runup));	
	}
	else{//berm is below the still waterline
		if(this->hori_berm_data->distance_water2berm<=2.0*wave_height){
			this->hori_berm_data->r_db=0.5-0.5*cos(constant::Cpi*this->hori_berm_data->distance_water2berm/(2.0*wave_height));

		}
		else{//out of influence
			this->hori_berm_data->r_db=1.0;
		}
	}
	//calculate the gamma berm
	this->gamma_berm=1.0-this->hori_berm_data->r_b*(1.0-this->hori_berm_data->r_db);
	//interpolate for a berm which is in reality not horizontal
	if(this->hori_berm_data->real_gradient>=(1.0/8.0)){//it is a slope, normally not possible because whole method is not used in this case
		this->gamma_berm=1.0;
	}
	else if(this->hori_berm_data->real_gradient<(1.0/8.0) && this->hori_berm_data->real_gradient>(1.0/15.0)){//interpolation
		double b=0.0;
		double m=0.0;
		m=(1.0-this->gamma_berm)/((1.0/8.0)-(1.0/15.0));
		b=this->gamma_berm-m*(1.0/15.0);
		this->gamma_berm=m*(this->hori_berm_data->real_gradient)+b;

	}//else no interpolation is necessary

}
//Calculate the wave height reduction 
void Fpl_Cub_Dike_Segment_Waterside::calculate_waveheight_reduction(const double wave_height){

	if(this->hori_berm_data->distance_water2berm>0.0){//forshore is below the still waterline
		if(wave_height>this->hori_berm_data->distance_water2berm*0.5){//2*waveheight is bigger than the waterlevel above the foreshore
			this->waveheight_reduction=0.5*this->hori_berm_data->distance_water2berm/wave_height;
		}
		else{//no wave height reduction
			this->waveheight_reduction=1.0;
		}
	}
	else{//forshore is above the still waterline => just wave run up until the foreshore
		this->waveheight_reduction=0.0;
	}
	//interpolate for a foreshore which is in reality not horizontal
	if(this->hori_berm_data->real_gradient>=(1.0/8.0)){//it is a slope, normally not possible because whole method is not used in this case
		this->waveheight_reduction=1.0;
	}
	else if(this->hori_berm_data->real_gradient<(1.0/8.0) && this->hori_berm_data->real_gradient>(1.0/15.0)){//interpolation	
		double b=0.0;
		double m=0.0;
		m=(1.0-this->waveheight_reduction)/((1.0/8.0)-(1.0/15.0));
		b=this->waveheight_reduction-m*(1.0/15.0);
		this->waveheight_reduction=m*(this->hori_berm_data->real_gradient)+b;
	}//else no interpolation is necessary

}
//Interpolate the berm reduction factor between berm and foreshore
void Fpl_Cub_Dike_Segment_Waterside::interpolate_gamma_berm(const double wave_length){
	double quotient=this->get_horizontal_dist()/wave_length;
	if(quotient<=1.0 && quotient >0.25){//just interpolation in this case
		//interpolate
		double b=0.0;
		double m=0.0;
		//gamma_berm interpolation
		m=(1.0-this->gamma_berm)/(1.0-0.25);
		b=this->gamma_berm-m*(0.25);
		this->gamma_berm=m*(quotient)+b;
	}

}
//Interpolate the wave height reduction factor between berm and foreshore
void Fpl_Cub_Dike_Segment_Waterside::interpolate_waveheight_reduction(const double wave_length){
	double quotient=this->get_horizontal_dist()/wave_length;
	if(quotient<=1.0 && quotient >0.25){//just interpolation in this case
		//interpolate
		double b=0.0;
		double m=0.0;
		//wave_height reduction interpolation
		m=(1.0-this->waveheight_reduction)/(0.25-1.0);
		b=this->waveheight_reduction-m*(1.0);
		this->waveheight_reduction=m*(quotient)+b;
	}
}
//Get the wave height reduction factor
double Fpl_Cub_Dike_Segment_Waterside::get_waveheight_reduction(void){
	return this->waveheight_reduction;
}
//Get the berm reduction factor (gamma berm)
double Fpl_Cub_Dike_Segment_Waterside::get_gamma_berm(void){
	return this->gamma_berm;
}
//Output the members to display/console
void Fpl_Cub_Dike_Segment_Waterside::output_members(const bool outflag){
	ostringstream cout;
	ostringstream prefix;
	prefix << "WATERSIDE> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout <<" Name first point : " << this->point1.get_point_name() <<endl;
	cout <<" Name second point: " << this->point2.get_point_name() <<endl;
	cout <<" x                : " << this->point1.get_xcoordinate()<< label::m<<endl;
	cout <<" y                : " << this->point1.get_ycoordinate()<<label::m <<endl;
	cout <<" Zoneno.          : " << this->zone_number<< endl;
	cout <<" Distance         : " << this->distance << label::m <<endl;
	cout <<"  Horizontal      : " << this->horizontal_dist << label::m << endl;
	cout <<"  Vertical        : " << this->vertical_dist << label::m << endl;
	cout <<" Gradientreduction: " << this->gradient_reduction << label::no_unit << endl;
	if(this->berm_flag==false){
		cout << " Type             : slope "<< endl;
	}
	else if(this->berm_flag==true && this->berm_mid_gradient_flag==false){
		cout << " Type             : berm "<< endl;
	}
	else if(this->berm_flag==true && this->berm_mid_gradient_flag==true){
		cout << " Type             : berm/slope "<< endl;
	}
	if(this->vertical_flag==false){
		cout <<" Gradient         : " << this->gradient << label::no_unit << endl;
	}
	else{
		cout <<" Gradient infinite " << endl;
	}

	
	Sys_Common_Output::output_fpl->output_txt(cout.str(),outflag);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the deterministic results
void Fpl_Cub_Dike_Segment_Waterside::output_determ_result(const bool outflag){
	ostringstream cout;
	ostringstream prefix;
	prefix << "G_SE_"<<this->name<<"> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	cout <<" Name of first point " << this->point1.get_point_name() << " x " << this->point1.get_xcoordinate();
	cout << " y " << this->point1.get_ycoordinate()<<endl;
	cout <<" Zonenr. " << this->zone_number<< endl;
	cout <<" Distance " << this->distance << label::m <<endl;
	cout <<" Horizontal " << this->horizontal_dist << label::m << endl;
	cout <<" Vertical  " << this->vertical_dist << label::m << endl;
	cout <<" Gradientreduction " << this->gradient_reduction << label::no_unit << endl;

	if(this->berm_horizontal_flag==false){
		cout << " Slope "<< endl;
	}
	else{
		if(this->hori_berm_data->foreshore==true){
			cout << " Foreshore " << endl;
			cout << "  wave_height reduction "<< this->waveheight_reduction << endl;
			cout << "  real gradient " << this->hori_berm_data->real_gradient << endl;
			cout << "  distance waterlevel->berm " << this->hori_berm_data->distance_water2berm<< label::m <<endl;
		}
		else if(this->hori_berm_data->foreshore==false && this->hori_berm_data->foreshore_berm==true){
			cout << " Between Foreshore and berm " <<endl;
			cout << "  wave_height reduction "<< this->waveheight_reduction << endl;
			cout << "  gamma_berm " << this->gamma_berm << endl;
			cout << " Berm parameters " << endl;
			cout << "  real gradient " << this->hori_berm_data->real_gradient << endl;
			cout << "  distance waterlevel->berm " << this->hori_berm_data->distance_water2berm<< label::m << endl;
			cout << "  influence length berm " << this->hori_berm_data->influence_lenght_berm<< label::m  << endl;
			cout << "  max wave runup " << this->hori_berm_data->max_runup<< label::m  << endl;
		}
		else if(this->hori_berm_data->foreshore==false && this->hori_berm_data->foreshore_berm==false){
			cout << " Berm " <<endl;
			cout << "  gamma_berm " << this->gamma_berm << endl;
			cout << " Berm parameters " << endl;
			cout << "  real gradient " << this->hori_berm_data->real_gradient << endl;
			cout << "  distance waterlevel->berm " << this->hori_berm_data->distance_water2berm<< label::m << endl;
			cout << "  influence length berm " << this->hori_berm_data->influence_lenght_berm<< label::m  << endl;
			cout << "  max wave runup " << this->hori_berm_data->max_runup<< label::m  << endl;
		}
	}
	Sys_Common_Output::output_fpl->output_txt(cout.str(),outflag);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Get the sinus of the slope angle
double Fpl_Cub_Dike_Segment_Waterside::get_sin_alpha(void){
	return this->sin_alpha;
}
//Set the erosion start flag
void Fpl_Cub_Dike_Segment_Waterside::set_erosion_start(const bool flag){
	this->erosion_start=flag;
}
//Get the erosion start flag
bool Fpl_Cub_Dike_Segment_Waterside::get_erosion_start(void){
	return this->erosion_start;
}
//Set the erosion failed flag
void Fpl_Cub_Dike_Segment_Waterside::set_erosion_failed(const bool flag){
	this->erosion_failed=flag;
}
//Get the erosion failed flag
bool Fpl_Cub_Dike_Segment_Waterside::get_erosion_failed(void){
	return this->erosion_failed;
}
//Set the erosion failed flag
void Fpl_Cub_Dike_Segment_Waterside::set_wave_impact_failed(const bool flag){
	this->wave_impact_failed=flag;
}
//Get the erosion failed flag
bool Fpl_Cub_Dike_Segment_Waterside::get_wave_impact_failed(void){
	return this->wave_impact_failed;
}
//Copy operator
Fpl_Cub_Dike_Segment_Waterside& Fpl_Cub_Dike_Segment_Waterside::operator=(const Fpl_Cub_Dike_Segment_Waterside& object){
	Geo_Segment::operator =(object);
	this->berm_flag=object.berm_flag;
	this->berm_mid_gradient_flag=object.berm_mid_gradient_flag;
	this->berm_horizontal_flag=object.berm_horizontal_flag;
	this->ptr_mat_zone=object.ptr_mat_zone;
	this->zone_number=object.zone_number;
	this->gradient_reduction=object.gradient_reduction;
	this->sin_alpha=object.sin_alpha;
	this->erosion_start=object.erosion_start;
	this->erosion_failed=object.erosion_failed;
	this->wave_impact_failed=object.wave_impact_failed;
	
	if(object.hori_berm_data!=NULL){
		this->allocate_berm_data();
		this->hori_berm_data=object.hori_berm_data;
	}
	else{
		this->hori_berm_data=NULL;
	}
	
	return *this;
}
//______________
//private
//Check if the segement have the gradient for a horizontal berm (gradient =0.0), berm (0.0 < gradient <1/15), slope (gradient >1/8), transition zone between slope and berm (1/15 < gradient <1/8)
void Fpl_Cub_Dike_Segment_Waterside::check_segment_for_berm(void){
	//horizontal berm
	if(this->get_gradient()==0.0){
		this->berm_horizontal_flag=true;
		this->berm_flag=true;
		//allocate the berm data
		try{
			this->allocate_berm_data();
		}
		catch(Error msg){
			throw msg;
		}
	}
	//berm
	else if(this->get_gradient()> 0.0 && this->get_gradient()<=(1.0/15.0)){
		this->berm_flag=true;
	}
	//transition zone
	else if(this->get_gradient()>(1.0/15.0) && this->get_gradient()<(1.0/8.0)){
		this->berm_flag=true;
		this->berm_mid_gradient_flag=true;
	}
	//slope
	else{
		this->berm_flag=false;
		this->berm_mid_gradient_flag=false;
		this->berm_horizontal_flag=false;
	}
}
//Allocate the berm_data
void Fpl_Cub_Dike_Segment_Waterside::allocate_berm_data(void){
	if(this->hori_berm_data==NULL){
		try{
			this->hori_berm_data=new _fpl_horizontal_berm_data;
		}
		catch(bad_alloc&){
			Error msg=this->set_error(3);
			throw msg;
		}	
		//init
		this->hori_berm_data->distance_water2berm=0.0;
		this->hori_berm_data->influence_lenght_berm=0.0;
		this->hori_berm_data->r_b=0.0;
		this->hori_berm_data->r_db=1.0;
		this->hori_berm_data->x_influence_left=0.0;
		this->hori_berm_data->x_influence_right=0.0;
		this->hori_berm_data->max_runup=0.0;
		this->hori_berm_data->foreshore=false;
		this->hori_berm_data->foreshore_berm=false;
	}
}
//Set error(s)
Error Fpl_Cub_Dike_Segment_Waterside::set_error(const int err_type){
		string place="Fpl_Cub_Dike_Segment_Waterside::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://the points are not in the right direction: first point x have to be smaller as the second point x
			place.append("set_points(Geo_Point *point1, Geo_Point *point2)");
			reason="Points are not in the right direction: x of first have to be smaller or the same as x of second point";
			help="Check the waterside cubature";
			info << "First point name    :"<< this->point1.get_point_name()<<endl;
			info << "First point number  :" << this->point1.get_number()<< endl;
			info << "Second point name   :" <<this->point2.get_point_name()<<endl;
			info<<  "Second point number :" << this->point2.get_number()<< endl;
			type=11;
			break;
		case 1://the points are not in the right direction: first point x have to be smaller as the second point x
			place.append("set_points(Geo_Point *point1, Geo_Point *point2)");
			reason="Points are not in the right direction: y of first have to be smaller or the same as y of second point";
			help="Check the waterside cubature";
			info << "First point name    :"<< this->point1.get_point_name()<<endl;
			info << "First point number  :" << this->point1.get_number()<< endl;
			info << "Second point name   :" <<this->point2.get_point_name()<<endl;
			info<<  "Second point number :" << this->point2.get_number()<< endl;	
			type=11;
			break;
		case 2://the points are not in the right direction: first point x have to be smaller as the second point x
			place.append("turn_berm_segment(Fpl_Cub_Dike_Segment_Waterside *segment_before, Fpl_Cub_Dike_Segment_Waterside *segment_after)");
			reason="Can not find an interception between the slope and the berm";
			help="Check the waterside cubature";
			info << "First point name    :"<< this->point1.get_point_name()<<endl;
			info << "First point number  :" << this->point1.get_number()<< endl;
			info << "Second point name   :" <<this->point2.get_point_name()<<endl;
			info<<  "Second point number :" << this->point2.get_number()<< endl;	type=11;
			break;
		case 3://bad allocation
			place.append("allocate_berm_data(void)");
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
