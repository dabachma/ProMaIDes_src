#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Element_Coup_Members.h"

//constructor
Hyd_Element_Coup_Members::Hyd_Element_Coup_Members(void){



	this->reset_vol_q();
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Element_Coup_Members), _sys_system_modules::HYD_SYS);
}

//destructor
Hyd_Element_Coup_Members::~Hyd_Element_Coup_Members(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Element_Coup_Members), _sys_system_modules::HYD_SYS);
}
//_____________
//public
//Reset volumes and discharge
void Hyd_Element_Coup_Members::reset_vol_q(void){
	this->coupling_q_fp=0.0;
	this->coupling_q_rv_overflow=0.0;
	this->coupling_q_rv_structure=0.0;
	this->coupling_q_rv_dikebreak=0.0;
	this->coupling_q_co_overflow=0.0;
	this->coupling_q_co_dikebreak=0.0;
	this->coupling_q_rv_direct=0.0;
	this->coupling_volume_fp.volume_in=0.0;
	this->coupling_volume_fp.volume_out=0.0;
	this->coupling_volume_fp.volume_total=0.0;
	this->coupling_volume_overflow_rv.volume_in=0.0;
	this->coupling_volume_overflow_rv.volume_out=0.0;
	this->coupling_volume_overflow_rv.volume_total=0.0;
	this->coupling_volume_structure_rv.volume_in=0.0;
	this->coupling_volume_structure_rv.volume_out=0.0;
	this->coupling_volume_structure_rv.volume_total=0.0;
	this->coupling_volume_dikebreak_rv.volume_in=0.0;
	this->coupling_volume_dikebreak_rv.volume_out=0.0;
	this->coupling_volume_dikebreak_rv.volume_total=0.0;
	this->coupling_volume_dikebreak_co.volume_in=0.0;
	this->coupling_volume_dikebreak_co.volume_out=0.0;
	this->coupling_volume_dikebreak_co.volume_total=0.0;
	this->coupling_volume_overflow_co.volume_in=0.0;
	this->coupling_volume_overflow_co.volume_out=0.0;
	this->coupling_volume_overflow_co.volume_total=0.0;
	this->coupling_volume_direct_rv.volume_in=0.0;
	this->coupling_volume_direct_rv.volume_out=0.0;
	this->coupling_volume_direct_rv.volume_total=0.0;

}
//Get the sum of inflow
double Hyd_Element_Coup_Members::get_sum_inflow(void){
	double sum=this->coupling_volume_fp.volume_in+this->coupling_volume_overflow_rv.volume_in+
		this->coupling_volume_structure_rv.volume_in+
		this->coupling_volume_dikebreak_rv.volume_in+this->coupling_volume_overflow_co.volume_in+
		this->coupling_volume_dikebreak_co.volume_in+this->coupling_volume_direct_rv.volume_in;
	return sum;
}
//Get the sum of outflow
double Hyd_Element_Coup_Members::get_sum_outflow(void){
	double sum=this->coupling_volume_fp.volume_out+this->coupling_volume_overflow_rv.volume_out+
		this->coupling_volume_structure_rv.volume_out+
		this->coupling_volume_dikebreak_rv.volume_out+this->coupling_volume_overflow_co.volume_out+
		this->coupling_volume_dikebreak_co.volume_out+this->coupling_volume_direct_rv.volume_out;
	return sum;
}
//Get sum of coupling discharge
double Hyd_Element_Coup_Members::sum_coupling_discharge(void){
	double sum=this->coupling_q_fp+this->coupling_q_rv_overflow+this->coupling_q_rv_structure+
		this->coupling_q_rv_dikebreak+this->coupling_q_co_overflow+this->coupling_q_co_dikebreak+
		this->coupling_q_rv_direct;
	return sum;
}
//Calculate hydrological balance
void Hyd_Element_Coup_Members::calculate_hydro_balance(const double delta_t){
		//from floodplain
	if(this->coupling_q_fp!=0.0){
		if(this->coupling_q_fp>0.0){
			this->coupling_volume_fp.volume_in=this->coupling_volume_fp.volume_in+this->coupling_q_fp*delta_t;
		}
		else if(this->coupling_q_fp<0.0){
			this->coupling_volume_fp.volume_out=this->coupling_volume_fp.volume_out-this->coupling_q_fp*delta_t;
		}
		//total 
		this->coupling_volume_fp.volume_total=this->coupling_volume_fp.volume_in-this->coupling_volume_fp.volume_out;
	}

	//from river overflow
	if(this->coupling_q_rv_overflow!=0.0){
		if(this->coupling_q_rv_overflow>0.0){
			this->coupling_volume_overflow_rv.volume_in=this->coupling_volume_overflow_rv.volume_in+this->coupling_q_rv_overflow*delta_t;
		}
		else if(this->coupling_q_rv_overflow<0.0){
			this->coupling_volume_overflow_rv.volume_out=this->coupling_volume_overflow_rv.volume_out-this->coupling_q_rv_overflow*delta_t;
		}
		//total 
		this->coupling_volume_overflow_rv.volume_total=this->coupling_volume_overflow_rv.volume_in-this->coupling_volume_overflow_rv.volume_out;
	}
	//from river structure
	if(this->coupling_q_rv_structure!=0.0){
		if(this->coupling_q_rv_structure>0.0){
			this->coupling_volume_structure_rv.volume_in=this->coupling_volume_structure_rv.volume_in+this->coupling_q_rv_structure*delta_t;
		}
		else if(this->coupling_q_rv_structure<0.0){
			this->coupling_volume_structure_rv.volume_out=this->coupling_volume_structure_rv.volume_out-this->coupling_q_rv_structure*delta_t;
		}
		//total 
		this->coupling_volume_structure_rv.volume_total=this->coupling_volume_structure_rv.volume_in-this->coupling_volume_structure_rv.volume_out;
	}
	//from river dikebreak
	if(this->coupling_q_rv_dikebreak!=0.0){
		if(this->coupling_q_rv_dikebreak>0.0){
			this->coupling_volume_dikebreak_rv.volume_in=this->coupling_volume_dikebreak_rv.volume_in+this->coupling_q_rv_dikebreak*delta_t;
		}
		else if(this->coupling_q_rv_dikebreak<0.0){
			this->coupling_volume_dikebreak_rv.volume_out=this->coupling_volume_dikebreak_rv.volume_out-this->coupling_q_rv_dikebreak*delta_t;
		}
		//total 
		this->coupling_volume_dikebreak_rv.volume_total=this->coupling_volume_dikebreak_rv.volume_in-this->coupling_volume_dikebreak_rv.volume_out;
	}
	//from coast overflow
	if(this->coupling_q_co_overflow!=0.0){
		if(this->coupling_q_co_overflow>0.0){
			this->coupling_volume_overflow_co.volume_in=this->coupling_volume_overflow_co.volume_in+this->coupling_q_co_overflow*delta_t;
		}
		else if(this->coupling_q_co_overflow<0.0){
			this->coupling_volume_overflow_co.volume_out=this->coupling_volume_overflow_co.volume_out-this->coupling_q_co_overflow*delta_t;
		}
		//total 
		this->coupling_volume_overflow_co.volume_total=this->coupling_volume_overflow_co.volume_in-this->coupling_volume_overflow_co.volume_out;
	}
	//from coast dikebreak
	if(this->coupling_q_co_dikebreak!=0.0){
		if(this->coupling_q_co_dikebreak>0.0){
			this->coupling_volume_dikebreak_co.volume_in=this->coupling_volume_dikebreak_co.volume_in+this->coupling_q_co_dikebreak*delta_t;
		}
		else if(this->coupling_q_co_dikebreak<0.0){
			this->coupling_volume_dikebreak_co.volume_out=this->coupling_volume_dikebreak_co.volume_out-this->coupling_q_co_dikebreak*delta_t;
		}
		//total 
		this->coupling_volume_dikebreak_co.volume_total=this->coupling_volume_dikebreak_co.volume_in-this->coupling_volume_dikebreak_co.volume_out;
	}
	//from direct river coupling
	if(this->coupling_q_rv_direct!=0.0){
		if(this->coupling_q_rv_direct>0.0){
			this->coupling_volume_direct_rv.volume_in=this->coupling_volume_direct_rv.volume_in+this->coupling_q_rv_direct*delta_t;
		}
		else if(this->coupling_q_rv_direct<0.0){
			this->coupling_volume_direct_rv.volume_out=this->coupling_volume_direct_rv.volume_out-this->coupling_q_rv_direct*delta_t;
		}
		//total 
		this->coupling_volume_direct_rv.volume_total=this->coupling_volume_direct_rv.volume_in-this->coupling_volume_direct_rv.volume_out;
	}

}