#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Structure_Gate.h"

//constructor
Hyd_Coupling_Structure_Gate::Hyd_Coupling_Structure_Gate(void):poleni_fac(0.577){
	this->local_opening_h=0.0;
	this->reduction_backwater=1.0;
	this->flow_coefficient=0.55;
	this->contraction_factor=0.6;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Structure_Gate), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Structure_Gate::~Hyd_Coupling_Structure_Gate(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Structure_Gate), _sys_system_modules::HYD_SYS);
}
//________________
//public
//Get and calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2)
double Hyd_Coupling_Structure_Gate::get_discharge(const double global_h_1, const double global_h_2){
	double delta_h_1=0.0;
	double delta_h_2=0.0;
	double q_buff=0.0;
	//1 is the main river |, by flow 1->2 sideweir reduction is taken; 2 is the floodplain or the diversion river |-, by flow 2->1 no sideweir reduction is taken


	delta_h_1=global_h_1-this->global_h_sill;
	delta_h_2=global_h_2-this->global_h_sill;

	//same waterlevels=> no flow
	if(abs(delta_h_1-delta_h_2)<constant::flow_epsilon){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the sill=> no flow
	else if(delta_h_1<=0.0 && delta_h_2<=0.0){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//weir flow after poleni
	else if(delta_h_1<this->local_opening_h && delta_h_2<this->local_opening_h){
		this->calculate_poleni_discharge(global_h_1, global_h_2);
		return this->discharge;
	}
	//flow von 1 to 2
	else if(delta_h_1 > delta_h_2){
	
		this->calculate_flow_coefficient(delta_h_1);
		this->calculate_contraction_factor(delta_h_1);

		q_buff=-1.0*this->local_opening_h*this->width*this->flow_coefficient*pow((delta_h_1*2.0*constant::Cgg),0.5);
		//with backwater reduction
		if(delta_h_2>0.0){
			this->calculate_reduction_factor(delta_h_1, delta_h_2);
			q_buff=q_buff*this->reduction_backwater;
		}
		this->coupling_v=q_buff/(this->local_opening_h*this->width);

	}
	//flow von 2 to 1
	else if(delta_h_1 < delta_h_2){
		this->calculate_flow_coefficient(delta_h_2);
		this->calculate_contraction_factor(delta_h_2);
		q_buff=this->local_opening_h*this->width*this->flow_coefficient*pow((delta_h_2*2.0*constant::Cgg),0.5);

		//with backwater reduction
		if(delta_h_1>0.0){
			this->calculate_reduction_factor(delta_h_2, delta_h_1);
			q_buff=q_buff*this->reduction_backwater;
		}
		this->coupling_v=q_buff/(this->local_opening_h*this->width);

	}
	//set a boundary to the discharge
	if(abs(q_buff)<=this->discharge_boundary){
		discharge=0.0;
		this->coupling_v=0.0;
	}
	this->discharge=q_buff;
	//calculate maximum values
	//this->discharge=this->smooth_coupling_discharge(q_buff);

	return this->discharge;
}
//Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
void Hyd_Coupling_Structure_Gate::set_coupling_structure_data(_hyd_coupling_structure_data data){
	_Hyd_Coupling_Structure::set_coupling_structure_data(data);
	this->local_h_sill=data.uncontrollable_param;

	if(this->controlled_flag==false){
		this->local_opening_h=data.controlled_param;
		this->control_curve_is_found=true;
	}
	else{
		int index_curve=(int)data.controlled_param;
		this->allocate_control_function(index_curve);
	}
}
//Get the coupling structure data
_hyd_coupling_structure_data Hyd_Coupling_Structure_Gate::get_coupling_structure_data(void){

	_hyd_coupling_structure_data buffer;
	buffer.controlled_flag=this->controlled_flag;
	buffer.controlled_param=this->local_opening_h;
	buffer.uncontrollable_param=this->local_h_sill;
	buffer.width=this->width;
	return buffer;
}
//Output the members
void Hyd_Coupling_Structure_Gate::output_members(ostringstream *cout){
	*cout << P(2)<< FORMAT_FIXED_REAL;
	*cout <<" Width                                       : " << W(7) << this->width<<label::m<<endl;
	*cout <<" Local sill height                           : " << W(7) << this->local_h_sill<<label::m<<endl;
	*cout <<" Controlled                                  : " << W(7) << functions::convert_boolean2string(this->controlled_flag)<<endl;
	if(this->controlled_flag==false){
		*cout <<" Local opening height                        : " << W(7) << this->local_opening_h << label::m<<endl;
	}
	else{
		//*cout <<" Curve number for local opening height       : " << W(7) << this-><<endl;	
	}
}
//Clone the gate structure
void Hyd_Coupling_Structure_Gate::clone_coupling_structure(_Hyd_Coupling_Structure *structure){
	_Hyd_Coupling_Structure::clone_coupling_structure(structure);
	//cast
	Hyd_Coupling_Structure_Gate *buffer=(Hyd_Coupling_Structure_Gate *)structure;
	
	this->local_opening_h=buffer->local_opening_h;
	this->reduction_backwater=buffer->reduction_backwater;
	this->flow_coefficient=buffer->flow_coefficient;
	this->contraction_factor=buffer->contraction_factor;

}
//__________
//private
//Calculation of the flow coefficient for a gate flow (after "Ueberfaelle und Wehre", 2005, G. Peter)
void Hyd_Coupling_Structure_Gate::calculate_flow_coefficient(const double h_local_before){
	double quotient=h_local_before/this->local_opening_h;
	double angle=90.0;

	this->flow_coefficient=0.7341169-0.04261387*log(quotient)-0.01410859*angle+0.00016111*pow(angle,2.0)-0.00000040072*pow(angle,3.0);
	this->flow_coefficient=this->flow_coefficient/(1.0-0.18920573*log(quotient)+0.02434395*pow(log(quotient),2.0)-0.01236335*angle+0.00012978*pow(angle,2.0));

	//set boundaries
	if(this->flow_coefficient>0.62){
		this->flow_coefficient=0.62; 
	}
	else if(this->flow_coefficient<0.0){
		this->flow_coefficient=0.0;
	}	
}
//Calculation of the contraction factor for a gate of 90°-inclination (after "Ueberfaelle und Wehre", 2005, G. Peter, vieweg)
void Hyd_Coupling_Structure_Gate::calculate_contraction_factor(const double h_local_before){
	double quotient=h_local_before/this->local_opening_h;
	this->contraction_factor=pow(this->flow_coefficient,2.0)/(2.0*quotient)+this->flow_coefficient*pow((1.0/(4.0*pow(quotient,2.0))+1.0),0.5);
}
//Calculation of the reduction factor for backwater effect
void Hyd_Coupling_Structure_Gate::calculate_reduction_factor(const double h_local_before, const double h_local_after){
	double a1=this->contraction_factor*this->local_opening_h/h_local_before;
	double a2=this->contraction_factor*this->local_opening_h/h_local_after;
	double coeff1=0.0;

	coeff1=(pow((1.0-2.0*a1*(1.0-a2)),2.0)-1.0+pow((h_local_after/h_local_before),2.0));

	//no backwaterreduction
	if(coeff1<0.0){
		this->reduction_backwater=(1.0+a1)*((1.0-2.0*a1*(1.0-a2)));
	}
	else{
		this->reduction_backwater=(1.0+a1)*((1.0-2.0*a1*(1.0-a2))-pow(coeff1,0.5));

	}	
	//replace the square-root function by a tangens- function by a given boundary; this functions is the best fit to the square-root
	//functions between 0.001  and 0.02 ; the boundary is set, where the functions (arctan/square root) are identically)
	if(abs(this->reduction_backwater)<=0.0152){
		this->reduction_backwater=0.10449968880528*atan(159.877741951379*this->reduction_backwater); //0.0152
	}
	else{
		this->reduction_backwater=pow(this->reduction_backwater,0.5);
	}

	//set boundaries
	if(this->reduction_backwater >1.0){
		this->reduction_backwater=1.0;
	}
	else if(this->reduction_backwater<0.0){
		this->reduction_backwater=0.0;
	}
}
//Calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2) for a weir (waterlevel below the gate opening) 
void Hyd_Coupling_Structure_Gate::calculate_poleni_discharge(const double global_h_1, const double global_h_2){

	double delta_h_1=0.0;
	double delta_h_2=0.0;
	double q_buff=0.0;

	delta_h_1=global_h_1-this->global_h_sill;
	delta_h_2=global_h_2-this->global_h_sill;

	//1 is the main river |, by flow 1->2 sideweir reduction is taken; 2 is the floodplain or the diversion river |-, by flow 2->1 no sideweir reduction is taken


	//same waterlevels=> no flow
	if(abs(delta_h_1-delta_h_2)<constant::flow_epsilon){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//both waterlevel are under the weirsill=> no flow
	else if(delta_h_1<=0.0 && delta_h_2<=0.0){
		this->coupling_v=0.0;
		q_buff=0.0;
	}
	//flow out of 1 into the 2 without submerged weirflow reduction due to h2, with side weir reduction
	else if(delta_h_1> 0.0 && delta_h_2 <= 0.0){
		q_buff=-1.0*constant::Cfacweir*this->poleni_fac*pow(delta_h_1,(3.0/2.0))*this->width;
		//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
		q_buff=constant::side_weir_reduction*q_buff;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff*(this->width/delta_h_1);
	}
	//flow out of 2 into 1 without submerged weirflow reduction due to h2, without side weir reduction
	else if(delta_h_1<= 0.0 && delta_h_2 > 0.0){
		q_buff=constant::Cfacweir*this->poleni_fac*pow(delta_h_2,(3.0/2.0))*this->width;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->width*delta_h_2);
	}
	//flow with submerged weirflow reduction
	else if(delta_h_1 > 0.0 && delta_h_2 > 0.0){
		//flow out of 1 into 2 with sideweir reduction
		if(delta_h_1>delta_h_2){
			q_buff=constant::Cfacweir*this->poleni_fac*pow(delta_h_1,(3.0/2.0))*this->width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_2/delta_h_1);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=-1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=-1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}	
			//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
			q_buff=constant::side_weir_reduction*q_buff;
			//calculate the velocity
			this->coupling_v=-1.0*q_buff*(this->width/delta_h_1);;
		}
		//flow out of the 2 into 1 without sideweir reduction
		else{
			q_buff=constant::Cfacweir*this->poleni_fac*pow(delta_h_2,(3.0/2.0))*this->width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_1/delta_h_2);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v=-1.0*q_buff/(this->width*delta_h_2);
		}
	}
	this->discharge=q_buff;
}

//check the parameters
void Hyd_Coupling_Structure_Gate::check_parameters(void){
	_Hyd_Coupling_Structure::check_parameters();
	if(this->local_opening_h<0.0){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Given local opening height is " << this->local_opening_h << label::m << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Set error(s)
Error Hyd_Coupling_Structure_Gate::set_error(const int err_type){
	string place="Hyd_Coupling_Structure_Gate::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://the local opening height is below 0.0
			place.append("check_parameters(void)");
			reason="The local opening height of the gate is < 0.0";
			help="Check the coupling structure settings";
			type=3;
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
