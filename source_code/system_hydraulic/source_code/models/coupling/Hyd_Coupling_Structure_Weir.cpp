#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coupling_Structure_Weir.h"

//constructor
Hyd_Coupling_Structure_Weir::Hyd_Coupling_Structure_Weir(void){
	this->poleni_fac=0.65;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coupling_Structure_Weir), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coupling_Structure_Weir::~Hyd_Coupling_Structure_Weir(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coupling_Structure_Weir), _sys_system_modules::HYD_SYS);
}
//________________
//public
//Get and calculate the discharge for given global waterlevels before (global_h_1) and after the structure (global_h_2)
double Hyd_Coupling_Structure_Weir::get_discharge(const double global_h_1, const double global_h_2){

	//1 is the main river |, by flow 1->2 sideweir reduction is taken; 2 is the floodplain or the diversion river |-, by flow 2->1 no sideweir reduction is taken

	double delta_h_1=0.0;
	double delta_h_2=0.0;
	double q_buff=0.0;

	delta_h_1=global_h_1-this->global_h_sill;
	delta_h_2=global_h_2-this->global_h_sill;

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
	//flow out of 1 into 2 without submerged weirflow reduction due to h2, with side weir reduction
	else if(delta_h_1> 0.0 && delta_h_2 <= 0.0){
		q_buff=-1.0*constant::Cfacweir*this->poleni_fac*pow(delta_h_1,(3.0/2.0))*this->width;
		//factor for a side weir overflow (after "Technische Hydromechanik I", Bollrich [2000])
		q_buff=constant::side_weir_reduction*q_buff;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff*(this->width/delta_h_1);
	}
	//flow out of 2 into 1 without submerged weirflow reduction due to h1, without side weir reduction
	else if(delta_h_1<= 0.0 && delta_h_2 > 0.0){
		q_buff=constant::Cfacweir*this->poleni_fac*pow(delta_h_2,(3.0/2.0))*this->width;
		//calculate the velocity
		this->coupling_v=-1.0*q_buff/(this->width*delta_h_2);
	}
	//submerged weirflow with reduction
	else if(delta_h_1 > 0.0 && delta_h_2 > 0.0){
		//flow out of 1 into 2 with side weir reduction
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
		//flow out of the 2 into the 1 without side weir reduction 
		else{
			q_buff=constant::Cfacweir*this->poleni_fac*pow(delta_h_2,(3.0/2.0))*this->width;
			//reduction of the discharge (submerged weirflow)
			double reduction_term=(1.0-delta_h_1/delta_h_2);
			//replace the ^(1/3) by a fitted arctan-function; at the boundary they have the same values
			if(reduction_term<=0.000463529){
				q_buff=1.0*q_buff*0.057965266895*atan(8984.365582471040*reduction_term);
			}
			else{
				q_buff=1.0*q_buff*pow(reduction_term,(1.0/3.0));
			}
			//calculate the velocity
			this->coupling_v=-1.0*q_buff/(this->width*delta_h_2);
		}
	}
	//set a boundary to the discharge
	if(abs(q_buff)<=this->discharge_boundary){
		discharge=0.0;
		this->coupling_v=0.0;
	}
	this->discharge=q_buff;
	//calculate maximum values
	this->discharge=this->smooth_coupling_discharge(q_buff);

	return this->discharge;
}
//Set the coupling structure member with a structure where the parser has gathered the informations (_hyd_coupling_structure_data)
void Hyd_Coupling_Structure_Weir::set_coupling_structure_data(_hyd_coupling_structure_data data){
	_Hyd_Coupling_Structure::set_coupling_structure_data(data);
	this->poleni_fac=data.uncontrollable_param;

	if(this->controlled_flag==false){
		this->local_h_sill=data.controlled_param;	
		this->control_curve_is_found=true;
	}
	else{
		int index_curve=(int)data.controlled_param;
		this->allocate_control_function(index_curve);
	}
}
//Get the coupling structure data
_hyd_coupling_structure_data Hyd_Coupling_Structure_Weir::get_coupling_structure_data(void){
	_hyd_coupling_structure_data buffer;
	buffer.controlled_flag=this->controlled_flag;
	buffer.controlled_param=this->local_h_sill;
	buffer.uncontrollable_param=this->poleni_fac;
	buffer.width=this->width;
	return buffer;
}
//Output the members
void Hyd_Coupling_Structure_Weir::output_members(ostringstream *cout){
	*cout << P(2)<< FORMAT_FIXED_REAL;
	*cout <<" Width                                       : " << W(7) << this->width<<label::m<<endl;
	*cout <<" Poleni factor                               : " << W(7) << this->poleni_fac<<label::no_unit<<endl;
	*cout <<" Controlled                                  : " << W(7) << functions::convert_boolean2string(this->controlled_flag)<<endl;
	if(this->controlled_flag==false){
		*cout <<" Local sill height                            : " << W(7) << this->local_h_sill << label::m<<endl;
	}
	else{
		//*cout <<" Curve number for local sill height           : " << W(7) << this-><<endl;	
	}
}
//Clone the weir structure
void Hyd_Coupling_Structure_Weir::clone_coupling_structure(_Hyd_Coupling_Structure *structure){
	_Hyd_Coupling_Structure::clone_coupling_structure(structure);
	//cast
	Hyd_Coupling_Structure_Weir *buffer=(Hyd_Coupling_Structure_Weir *)structure;
	this->poleni_fac=buffer->poleni_fac;
}
//_____________
//private
//check the parameters
void Hyd_Coupling_Structure_Weir::check_parameters(void){
	_Hyd_Coupling_Structure::check_parameters();
	if(this->poleni_fac<0.0){
		Warning msg=this->set_warning(0);
		ostringstream info;
		info << "Given Poleni factor is " << this->poleni_fac << label::m << endl;
		msg.make_second_info(info.str());
		msg.output_msg(2);
		//reaction
		this->poleni_fac=constant::poleni_const;
	}
}
//Set error(s)
Error Hyd_Coupling_Structure_Weir::set_error(const int err_type){
	string place="Hyd_Coupling_Structure_Weir::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		//case 1://the relative sill height is below 0.0
		//	place.append("check_parameters(void)");
		//	reason="The relative sill height of the structure is < 0.0";
		//	help="Check the coupling structure settings";
		//	type=3;
		//	break;
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
Warning Hyd_Coupling_Structure_Weir::set_warning(const int warn_type){
		string place="Hyd_Coupling_Structure_Weir::";
		string help;
		string reason;
		string reaction;
		int type=0;
		Warning msg;
		stringstream info;

	switch (warn_type){
		case 0://no coupling between the river an dthe floodplain is possible
			place.append("check_parameters(void)") ;
			reason="The Poleni factor for a weir structure coupling is < 0.0";
			info << "The Poleni factor is set to "<< constant::poleni_const;
			reaction=info.str();
			help= "Check structure coupling settings";
			type=3;	
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
