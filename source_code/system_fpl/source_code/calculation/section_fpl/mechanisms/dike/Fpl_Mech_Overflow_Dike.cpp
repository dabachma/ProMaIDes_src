//#include "Fpl_Mech_Overflow_Dike.h"
#include "Fpl_Headers_Precompiled.h"

///Default constructor
Fpl_Mech_Overflow_Dike::Fpl_Mech_Overflow_Dike(void){
	this->waterside_cubature=NULL;
	this->crest_cubature=NULL;
	this->landside_cubature=NULL;
	this->reset_buffer_variables();
	this->no_segment_interception=0;
	this->q_overflow.set_unit(label::qm_per_secm);

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Mech_Overflow_Dike)-6*sizeof(Fpl_Random_Variables)-sizeof(Geo_Straight_Line), _sys_system_modules::FPL_SYS);//count the memory
}
///Default destructor
Fpl_Mech_Overflow_Dike::~Fpl_Mech_Overflow_Dike(void){
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Mech_Overflow_Dike)-6*sizeof(Fpl_Random_Variables)-sizeof(Geo_Straight_Line), _sys_system_modules::FPL_SYS);//count the memory
}
//______________
//public
//Set the pointer to the waterside cubature, which is allocated in the dike-section class
void Fpl_Mech_Overflow_Dike::set_waterside_cubature(Fpl_Cub_Dike_Waterside *waterside_cubature){
	this->waterside_cubature=waterside_cubature;
}
//Set the pointer to the landside cubature, which is allocated in the dike-section class
void Fpl_Mech_Overflow_Dike::set_landside_cubature(Fpl_Cub_Dike_Landside *landside_cubature){
	this->landside_cubature=landside_cubature;
}
//Set the pointer to the crest cubature, which is allocated in the dike-section class
void Fpl_Mech_Overflow_Dike::set_crest_cubature(Fpl_Cub_Dike_Crest *crest_cubature){
	this->crest_cubature=crest_cubature;
}
//Set the pointer to the crest cubature, which is allocated in the dike-section class
//void Fpl_Mech_Overflow_Dike::set_lanside_cubature(Fpl_Cub_Dike_Landsie *landside_cubature){
//
//}
//Transfer the overflow-mechanism data to database, e.g. geometry, random variables etc.
void Fpl_Mech_Overflow_Dike::transfer_data2database(const int section_id, const _sys_system_id id, QSqlDatabase *ptr_database){
	ostringstream prefix;
	ostringstream cout;
	prefix << "OVERFLOW> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	cout << "Transfer variable members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->mue_factor.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);
	this->factor_crestwidth.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);
	this->factor_flowcontraction.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);
	this->factor_watersideslope.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);
	this->factor_landsideslope.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);

	//result members
	cout << "Transfer result members to database..."<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	this->q_overflow.transfer_rand2database(ptr_database,id, section_id, fpl_label::mech_overflow, fpl_label::sec_dike);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Set the random variables of the results for the overflow mechanism per database table
void Fpl_Mech_Overflow_Dike::set_result_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->q_overflow.set_input(id,section_id,fpl_label::q_overflow, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->q_overflow.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per database table
void Fpl_Mech_Overflow_Dike::set_variables(QSqlDatabase *ptr_database, const _sys_system_id id, const int section_id){
	try{
		this->mue_factor.set_input(id, section_id,fpl_label::mue_factor, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->mue_factor.set_distribution_types(ptr_database);
		this->factor_crestwidth.set_input(id, section_id,fpl_label::factor_crestwidth, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->factor_crestwidth.set_distribution_types(ptr_database);
		this->factor_flowcontraction.set_input(id, section_id,fpl_label::factor_flowcontraction, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->factor_flowcontraction.set_distribution_types(ptr_database);
		this->factor_watersideslope.set_input(id, section_id,fpl_label::factor_watersideslope, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->factor_watersideslope.set_distribution_types(ptr_database);
		this->factor_landsideslope.set_input(id, section_id,fpl_label::factor_landsideslope, true, ptr_database, fpl_label::mech_overflow, fpl_label::sec_dike, 0, false);
		this->factor_landsideslope.set_distribution_types(ptr_database);
	}catch(Error msg){
		throw msg;
	}
}
//Set the random input variables per file
void Fpl_Mech_Overflow_Dike::set_variables(QFile *ifile, int *line_counter, const qint64 pos_file, const int line_counter_start){
	ostringstream prefix;
	prefix << "OVERFLOW> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Input variable members per file..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	
	try{
		this->mue_factor.set_input(ifile, line_counter,fpl_label::mue_factor,true, fpl_label::end_mech_overflow);
		this->mue_factor.check_given_boundaries(1.2,0.2);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
		this->factor_crestwidth.set_input(ifile, line_counter,fpl_label::factor_crestwidth,true, fpl_label::end_mech_overflow);
		this->factor_crestwidth.check_given_boundaries(2.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
		this->factor_flowcontraction.set_input(ifile, line_counter,fpl_label::factor_flowcontraction,true, fpl_label::end_mech_overflow);
		this->factor_flowcontraction.check_given_boundaries(2.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
		this->factor_watersideslope.set_input(ifile, line_counter,fpl_label::factor_watersideslope,true, fpl_label::end_mech_overflow);
		this->factor_watersideslope.check_given_boundaries(2.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
		this->factor_landsideslope.set_input(ifile, line_counter,fpl_label::factor_landsideslope,true, fpl_label::end_mech_overflow);
		this->factor_landsideslope.check_given_boundaries(2.0,0.0);
		ifile->seek(pos_file);
		*line_counter=line_counter_start;

		//result variable
		this->q_overflow.set_input(ifile, line_counter,fpl_label::q_overflow,true, fpl_label::end_mech_overflow);
		this->q_overflow.check_positiv_zero_variable();
		ifile->seek(pos_file);
		*line_counter=line_counter_start;
	}
	catch(Error msg){
		Sys_Common_Output::output_fpl->rewind_userprefix();
		throw msg;
	}

	cout << "Input of variable members per file is finsihed" << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output members to display/console
void Fpl_Mech_Overflow_Dike::output_members(void){
	ostringstream prefix;
	prefix << "OVERFLOW> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Variable members mechanism overflow..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->mue_factor.output_member();
	this->factor_crestwidth.output_member();
	this->factor_flowcontraction.output_member();
	this->factor_watersideslope.output_member();
	this->factor_landsideslope.output_member();

	cout << "Result members mechanism overflow..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->q_overflow.output_member();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Calculate the mechanism; main input parameter is the waterlevel before the dike section
double Fpl_Mech_Overflow_Dike::calculate_mechanism(const bool random_calculation, const double waterlevel){
	
	
	this->reset_buffer_variables();
	//reset the random flags of the variables
	this->reset_random_flag();

	//calculate delta_h
	this->delta_h=waterlevel-this->waterside_cubature->get_dike_height();

	//check if  overflow is possible
	if(this->delta_h<=0.0){
		return this->q_overflow_buff;
	}


	//calculate delta_h2width
	 this->delta_h2width=this->delta_h/this->crest_cubature->get_crest_width();

	//calculate contraction height buffer
	this->waterside_cubature->calculate_contraction_height(this->delta_h,&this->contraction_height);

	//make the relevant line
	this->make_relevant_interception_line();
	//intercept with ideal cubature
	this->waterside_cubature->calculate_intercept_idealcubature_firstdown(&this->crest_minus_2delta_h, &this->no_segment_interception);
	//calculate slope waterside
	this->slope_water_side=this->waterside_cubature->calculate_mid_gradient();
	//calculate slope land side
	this->slope_land_side=this->landside_cubature->calculate_mid_gradient();

	//decide and calculate the mue factor
	this->decide_mue_calculation(random_calculation);

	//calculate the variable buffer
	this->calculate_q_overflow();
	this->q_overflow.set_new_middle(this->q_overflow_buff);
	this->q_overflow.calculate_variables(random_calculation);

	//return the end result
	return this->q_overflow.get_result().end_result;
}
//Check the statistic of the random variables
void Fpl_Mech_Overflow_Dike::check_statistic(void){
	//variables
	this->mue_factor.check_statistic();
	this->factor_crestwidth.check_statistic();
	this->factor_flowcontraction.check_statistic();
	this->factor_watersideslope.check_statistic();
	this->factor_landsideslope.check_statistic();

	//resultvariables
	this->q_overflow.check_statistic();
}
//Reset the statistic important values
void Fpl_Mech_Overflow_Dike::reset_statistics(void){
	//variables
	this->mue_factor.reset_statistics();
	this->factor_crestwidth.reset_statistics();
	this->factor_flowcontraction.reset_statistics();
	this->factor_watersideslope.reset_statistics();
	this->factor_landsideslope.reset_statistics();

	//resultvariables
	this->q_overflow.reset_statistics();

	this->reset_counter_prob();
}
//Output the statistics of the random variables to display/console
void Fpl_Mech_Overflow_Dike::output_statistic(void){
	ostringstream prefix;
	prefix << "OVERFLOW> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());

	ostringstream cout;
	cout << "Variable members mechanism overflow..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->mue_factor.output_statistic();
	this->factor_crestwidth.output_statistic();
	this->factor_flowcontraction.output_statistic();
	this->factor_watersideslope.output_statistic();
	this->factor_landsideslope.output_statistic();

	cout << "Result members mechanism overflow..." << endl;
	Sys_Common_Output::output_fpl->output_txt(&cout,true);
	this->q_overflow.output_statistic();

	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//Output the deterministic results to display/console in case of an determistic calculation
void Fpl_Mech_Overflow_Dike::output_determ_results(void){
	ostringstream prefix;
	prefix << "OVERFLOW> ";
	Sys_Common_Output::output_fpl->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "Deterministic results mechanism overflow..."<< endl;
	cout << " Delta h                 : "<< this->delta_h << label::m << endl;
	cout << " Overflow discharge      : " << this->q_overflow.get_result().end_result << label::qm_per_secm << endl;
	cout << " Mue factor              : " << this->mue_factor.get_result().end_result << label::no_unit << endl; 
	cout << " Factor crestwidth       : " << this->factor_crestwidth.get_result().end_result << label::no_unit << endl; 
	cout << " Factor flowcontraction  : " << this->factor_flowcontraction.get_result().end_result << label::no_unit << endl; 
	cout << " Factor watersideslope   : " << this->factor_watersideslope.get_result().end_result << label::no_unit << endl;
	cout << " Factor landsideslope    : " << this->factor_landsideslope.get_result().end_result << label::no_unit << endl;
	cout << " Contraction height      : " << this->contraction_height << label::m << endl;
	cout << " Slope waterside         : " << this->slope_water_side << label::no_unit << endl;
	cout << " Slope landside          : " << this->slope_land_side << label::no_unit << endl;


	Sys_Common_Output::output_fpl->output_txt(&cout,false);
	Sys_Common_Output::output_fpl->rewind_userprefix();
}
//_______________
//private
//Reset the random generated flags of the random variables
void Fpl_Mech_Overflow_Dike::reset_random_flag(void){
	//reset the random generated flags 
	this->q_overflow.reset_random_generated();
	this->mue_factor.reset_random_generated();
	this->factor_crestwidth.reset_random_generated();
	this->factor_flowcontraction.reset_random_generated();
	this->factor_watersideslope.reset_random_generated();
	this->factor_landsideslope.reset_random_generated();
}
//Reset the buffer variables
void Fpl_Mech_Overflow_Dike::reset_buffer_variables(void){
	this->delta_h=0.0;
	this->q_overflow_buff=0.0;
	this->mue_buff=0.0;
	this->factor_crestwidth_buff=1.0;
	this->factor_flowcontraction_buff=0.0;
	this->factor_watersideslope_buff=1.0;
	this->factor_landsideslope_buff=1.0;
	this->contraction_height=0.0;
	this->slope_water_side=0.0;
	this->slope_land_side=0.0;

	this->delta_h2width=0.0;
}
//Generate the line of the relevant waterlevel (dike_crest-2*delta_h
void Fpl_Mech_Overflow_Dike::make_relevant_interception_line(void){
	double height=this->crest_cubature->get_crest_height()-2.0*this->delta_h;
	if(height<0.0){
		height=0.0;
	}
	this->crest_minus_2delta_h.set_coordinates(0.0,height);
}

//Decide the poleni-factor (mue) calculation for the ccorrespodnig weir type
void Fpl_Mech_Overflow_Dike::decide_mue_calculation(const bool random_calculation){
/** Detailed information to the weir-types are given in:
	-	"Ueberfaelle und Wehre" (Guenther; vieweg 2005)
	-	"Combined Wave Overtopping and Overflow of Dikes and Seawalls" (Bleck, Schüttrumpf; 2000) [poster].
*/
	//sharp crested
	if(this->delta_h2width > 1.8){
		//calculate the variable mue_buffer
		this->calculate_sharp_weir_mue();
	}//short weir
	else if(this->delta_h2width <= 1.8 && this->delta_h2width >= 0.5){
		//calculate the variable mue_buffer
		this->calculate_short_weir_mue();
	}//interpolate between short and broad weir
	else if( this->delta_h2width  <0.5 &&  this->delta_h2width  > 0.35){
		this->calculate_broad_weir_factors(random_calculation);
		//calculate the variable mue_buffer
		this->interpolate_weir_types_mue();
	}//broad weir
	else if( this->delta_h2width <=0.35){
		this->calculate_broad_weir_factors(random_calculation);
		//calculate the variable mue_buffer
		this->calculate_broad_weir_mue();
	}
	//set the variables buffer to the random variables
	this->mue_factor.set_new_middle(this->mue_buff);
	this->mue_buff=this->mue_factor.calculate_variables(random_calculation).end_result;
}
//Calculate the poleni-factor (mue) for a sharp-crested weir
void Fpl_Mech_Overflow_Dike::calculate_sharp_weir_mue(void){
/** Detailed information to the sharp-crested weir type in:  "Ueberfaelle und Wehre" G., Guenther; vieweg 2005
*/
	this->mue_buff=0.6035+0.0813*this->delta_h/this->contraction_height;
	//set a maximum by 0.7
	if(this->mue_buff>0.7){
		this->mue_buff=0.7;
	}
	if(this->mue_buff<0.49){
		this->mue_buff=0.49;
	}
}
//Calculate the poleni-factor (mue) for a short-crested weir
double Fpl_Mech_Overflow_Dike::calculate_short_weir_mue(void){
/** Detailed information to the short-crested weir type in:  "Ueberfaelle und Wehre" G., Guenther; vieweg 2005
*/
	this->mue_buff=(0.6035+0.0813*this->delta_h/this->contraction_height);

	this->mue_buff=this->mue_buff*(1.0-0.2*exp(-0.6*pow(this->delta_h2width,3.06)));

	//set a maximum by 0.7
	if(this->mue_buff>0.7){
		this->mue_buff=0.7;
	}
	if(this->mue_buff<0.49){
		this->mue_buff=0.49;
	}

	return this->mue_buff;
}
//Calculate the poleni-factor (mue) for a broad-crested weir
double Fpl_Mech_Overflow_Dike::calculate_broad_weir_mue(void){
/** Detailed information to the broad-crested weir type in:  "Combined Wave Overtopping and Overflow of Dikes and Seawalls" (Bleck, Schüttrumpf; 2000) [poster].
*/

	this->mue_buff=1.44*(1.0+this->factor_flowcontraction_buff)*this->factor_crestwidth_buff*this->factor_watersideslope_buff*this->factor_landsideslope_buff;
	this->mue_buff=this->mue_buff/constant::Cfacweir;
	if(this->mue_buff>0.7){
		this->mue_buff=0.7;
	}
	if(this->mue_buff<0.49){
		this->mue_buff=0.49;
	}
	return this->mue_buff;
}
///Calculate additional factors for a broad weir formula 
void Fpl_Mech_Overflow_Dike::calculate_broad_weir_factors(const bool random_calculation){
/** Detailed information to factors for the broad-crested weir type overflow
calculation in:  "Combined Wave Overtopping and Overflow of Dikes and Seawalls" (Bleck, Schüttrumpf; 2000) [poster].
*/
	//factor crest width
	this->factor_crestwidth_buff=1.0/(1.0-(2.0/9.0))*(1.0-(2.0/9.0)/(1.0+pow(this->delta_h2width,4)));
	this->factor_crestwidth.set_new_middle(this->factor_crestwidth_buff);
	this->factor_crestwidth_buff=this->factor_crestwidth.calculate_variables(random_calculation).end_result;
	//factor flow contraction
	if(this->delta_h/(this->delta_h+this->contraction_height)>(1.0/3.0)){
		this->factor_flowcontraction_buff=(1.0/3.0)*pow((this->delta_h/(this->delta_h+this->contraction_height)-(1.0/3.0)),(3/4));
		this->factor_flowcontraction.set_new_middle(this->factor_flowcontraction_buff);
		this->factor_flowcontraction_buff=this->factor_flowcontraction.calculate_variables(random_calculation).end_result;
	}
	else{
		this->factor_flowcontraction_buff=0.0;
		this->factor_flowcontraction.set_new_middle(this->factor_flowcontraction_buff);
		this->factor_flowcontraction.calculate_variables(random_calculation).end_result;
	}

	//factor waterside slope (1.0 is used instead of 0.9+..is used 
	this->factor_watersideslope_buff=(0.9+1.0/8.0*pow(this->slope_water_side,0.5));
	this->factor_watersideslope_buff=this->factor_watersideslope_buff*((1.0+1.38*pow(this->delta_h2width,4)/this->factor_watersideslope_buff)/(1.0+pow(this->delta_h2width,4)));
	this->factor_watersideslope.set_new_middle(this->factor_watersideslope_buff);
	this->factor_watersideslope_buff=this->factor_watersideslope.calculate_variables(random_calculation).end_result;
	//factor landside slope
	this->factor_landsideslope_buff=this->slope_land_side*constant::Cpi*0.5;
	this->factor_landsideslope_buff=sin(this->factor_landsideslope_buff);
	this->factor_landsideslope_buff=1.0+0.5*pow((this->factor_landsideslope_buff),0.5);
	this->factor_landsideslope_buff=(1.0+this->factor_landsideslope_buff*pow(this->delta_h2width,4))/(1.0+pow(this->delta_h2width,4));
	this->factor_landsideslope.set_new_middle(this->factor_landsideslope_buff);
	this->factor_landsideslope_buff=this->factor_landsideslope.calculate_variables(random_calculation).end_result;
}
//Interpolate the poleni-factor (mue) between the weir types (short and broad crested weir)
void Fpl_Mech_Overflow_Dike::interpolate_weir_types_mue(void){
	//init the values
	double mue_broad=0.0;
	double mue_short=0.0;
	double m=0.0;
	double b=0.0;

	//calculate each
	mue_broad=this->calculate_broad_weir_mue();
	mue_short=this->calculate_short_weir_mue();

	m=(mue_broad-mue_short)/(0.35-0.5);
	b=mue_short-m*0.5;
	//interpolate
	this->mue_buff=m*this->delta_h2width+b;
}
//Calculate the overflow discharge
void Fpl_Mech_Overflow_Dike::calculate_q_overflow(void){
	//poleni formula
	this->q_overflow_buff=constant::Cfacweir*this->mue_buff*pow(this->delta_h,3.0/2.0);
}