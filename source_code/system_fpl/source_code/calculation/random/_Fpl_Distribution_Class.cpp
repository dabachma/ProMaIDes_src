//#include "_Fpl_Distribution_Class.h"
#include "Fpl_Headers_Precompiled.h"

//Default constructor
_Fpl_Distribution_Class::_Fpl_Distribution_Class(void){
	this->random_result.end_result=0.0;
	this->random_result.inter_result=0.0;
	//for the calculation of the statistics
	this->reset_statistic();
	this->boundaries.max=1e20;
	this->boundaries.min=-1e20;
	this->description=label::not_set;
	this->random_generator=NULL;
	this->allocate_random_generator();
}
//Default destructor
_Fpl_Distribution_Class::~_Fpl_Distribution_Class(void){
	this->delete_random_generator();
}
//____________
//public
//Initialize the random generator; the seed value is always the same (static)
 void _Fpl_Distribution_Class::reinit_random(void){
 /**This initialization method is required for a fragility curve calculation. Each step have
 to start with the same seed value.
 */
	 if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_zikurat){
		Fpl_Random_Generator_Zikurat::reinit_random();
	}
	else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_MofAll){
		Fpl_Random_Generator_MofAll::reinit_random();
	}
	else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_sfmt){
		Fpl_Random_Generator_Sfmt::reinit_generator();
	}
	else{//ranmar is used
		Fpl_Random_Generator_Ranmar::reinit_random();
	}
}
//Initialize the random generator with a given seed value
 void _Fpl_Distribution_Class::reinit_random(const long int seed){
	 if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_zikurat){
		Fpl_Random_Generator_Zikurat::reinit_random(seed);
	}
	 else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_MofAll){
		Fpl_Random_Generator_MofAll::reinit_random(seed);
	}
	 else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_sfmt){
		Fpl_Random_Generator_Sfmt::reinit_generator();
	}
	else{//ranmar is used
		Fpl_Random_Generator_Ranmar::reinit_random(seed);
	}
}
//Set the boundaries for the result of the variable
void _Fpl_Distribution_Class::set_boundaries(const double min, const double max){
	this->boundaries.max=max;
	this->boundaries.min=min;
 }
//Get the calculated mean value
double _Fpl_Distribution_Class::get_calculated_mean(void){
	return this->mean_calc;
}
//Get mean distribution type as text
string _Fpl_Distribution_Class::get_mean_distribution_type(void){
	return "";
}
//Set the input of the distribution type via dialog for checking the random generator and the transformation
bool _Fpl_Distribution_Class::set_input_check(void){
/**There are not for all child classes dialogs available. Moreover, there is no checking availabe
for the combination classes of distribution types like Fpl_Distribution_Class_Mean_Mean or Fpl_Distribution_Class_Discret_Mean.
For the discrete distribution class there is not yet an check dialog availabe.
Thus, the checking is interupted.
*/
	return false;
}
//Check the statistic of the random generation process
void _Fpl_Distribution_Class::check_statistic(void){
	//counting the numbers of generation steps
	this->counter_calc++;
	//meanvalue updating
	this->mean_calc=this->mean_calc_old+((this->random_result.end_result)-this->mean_calc_old)/(double)this->counter_calc;
	//varianz updating first by 2 generation steps
	if(counter_calc>2){
		this->varianz_calc=this->varianz_calc*((double)(this->counter_calc-2)/(double)(this->counter_calc-1))+(double)this->counter_calc*pow((this->mean_calc-this->mean_calc_old),2);
		this->std_dev_calc=pow(this->varianz_calc,0.5);
	}
	//storing of the actuell values to the old ones
	this->mean_calc_old=this->mean_calc;

	//init the min and max values
	if(counter_calc==1){
		this->max_calc=this->random_result.end_result;
		this->min_calc=this->random_result.end_result;
	}
	//find max
	if(this->random_result.end_result > max_calc){
		this->max_calc=this->random_result.end_result;
	}
	//find min
	if(this->random_result.end_result < min_calc){
		this->min_calc=this->random_result.end_result;
	}
}
//Reset the values for checking the statistics
void _Fpl_Distribution_Class::reset_statistic(void){
	//reset this value for the calculation of the statistics
	this->mean_calc=0.0;
	this->std_dev_calc=0.0;
	this->min_calc=1e30;
	this->max_calc=-1e30;
	this->counter_calc=0;
	this->mean_calc_old=0.0;
	this->varianz_calc=0.0;
	this->mid_calc=0.0;
	this->counter_set_mid=0;
}
//Return the last generated result without any generation
_fpl_result _Fpl_Distribution_Class::get_result(void){
	return this->random_result;
}
//Set the random result
void _Fpl_Distribution_Class::set_result(const _fpl_result new_res){
	this->random_result=new_res;
}
//Get calculated exact probability for the distribution for the random generator check
double _Fpl_Distribution_Class::get_calulculated_exact_prob(const double , const double ){
	return 0.0;
}
//Convert the distribution type (_fpl_distribution_type) to a string (static)
string _Fpl_Distribution_Class::convert_dist_type2txt(_fpl_distribution_type type){
	string buffer;
	switch(type){
		case _fpl_distribution_type::mean_type:
			buffer=fpl_label::dist_mean;
			break;
		case _fpl_distribution_type::triangle_type:
			buffer=fpl_label::dist_triangle;
			break;
		case _fpl_distribution_type::discret_type:
			buffer=fpl_label::dist_discret;
			break;
		case _fpl_distribution_type::mean2mean_type:
			buffer=fpl_label::dist_mean2mean;
			break;
		case _fpl_distribution_type::discrete2mean_type:
			buffer=fpl_label::dist_discret2mean;
			break;
		default:
			buffer=label::unknown_type;
	}
	return buffer;
}
//Convert a string to the distribution type (_fpl_distribution_type) (static)
_fpl_distribution_type _Fpl_Distribution_Class::convert_txt2dist_type(const string txt){
	_fpl_distribution_type type;
	string buffer=txt;

	functions::clean_string(&buffer);
	functions::convert_string2lower_case(&buffer);

	if(buffer==fpl_label::dist_mean){
		type=_fpl_distribution_type::mean_type;
	}
	else if(buffer==fpl_label::dist_triangle){
		type=_fpl_distribution_type::triangle_type;
	}
	else if(buffer==fpl_label::dist_discret){
		type=_fpl_distribution_type::discret_type;
	}
	else if(buffer==fpl_label::dist_mean2mean){
		type=_fpl_distribution_type::mean2mean_type;
	}
	else if(buffer==fpl_label::dist_discret2mean){
		type=_fpl_distribution_type::discrete2mean_type;
	}
	else{
		Error msg;
		msg.set_msg("_Fpl_Distribution_Class::convert_txt2dist_type(const string txt)", "Can not convert this distribution type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert section type: " << txt << endl;
		info << "Possible types: "<< endl;
		info << " "<<fpl_label::dist_mean  << endl;
		info << " "<<fpl_label::dist_triangle  << endl;
		info << " "<<fpl_label::dist_discret << endl;
		info << " "<<fpl_label::dist_mean2mean  << endl;
		info << " "<<fpl_label::dist_discret2mean  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	return type;
}
//Convert an integer to the distribution type (_fpl_distribution_type) (static)
_fpl_distribution_type _Fpl_Distribution_Class::convert_int2dist_type(const int type){
	_fpl_distribution_type buffer;
	switch(type){
		case(0):
			buffer=_fpl_distribution_type::mean_type;
			break;
		case(1):
			buffer=_fpl_distribution_type::triangle_type;
			break;
		case(2):
			buffer=_fpl_distribution_type::discret_type;
			break;
		case(3):
			buffer=_fpl_distribution_type::mean2mean_type;
			break;
		case(4):
			buffer=_fpl_distribution_type::discrete2mean_type;
			break;
		default:
			buffer=_fpl_distribution_type::unknown_dist_type;
	}

	return buffer;
}
//Get the index of the result (relevant by discret and discret2mean distribution type)
int _Fpl_Distribution_Class::get_result_index(void){
	return 0;
}
//Get the number of points (relevant by discret and discret2mean distribution type)
int _Fpl_Distribution_Class::get_number_point(void){
	return 0;
}
//Get the value per given point index
double _Fpl_Distribution_Class::get_point_value(const int index){
	return 0.0;
}
//_____________
//protected
//Generation of uniformly-distributed numbers between [0...1]
double _Fpl_Distribution_Class::ran1(void){
	return this->random_generator->run_generator();
}
//___________
//private
//Allocate the random generator
void _Fpl_Distribution_Class::allocate_random_generator(void){
	this->delete_random_generator();
	try{
		if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_zikurat){
			this->random_generator=new Fpl_Random_Generator_Zikurat;
		}
		else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_MofAll){
			this->random_generator=new Fpl_Random_Generator_MofAll;
		}
		else if(Fpl_Mc_Sim::get_generator_type()==_fpl_random_generator_type::gen_type_sfmt){
			this->random_generator=new Fpl_Random_Generator_Sfmt;
		}
		else{//ranmar is used
			this->random_generator=new Fpl_Random_Generator_Ranmar;
		}
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the random generator
void _Fpl_Distribution_Class::delete_random_generator(void){
	if(this->random_generator!=NULL){
		delete this->random_generator;
		this->random_generator=NULL;
	}
}

//Set error(s)
Error _Fpl_Distribution_Class::set_error(const int err_type){
	string place="_Fpl_Distribution_Class::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_random_generator(void)");
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