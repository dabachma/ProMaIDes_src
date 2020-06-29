#pragma once
/**\class Hyd_Element_Coup_Members
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/
#ifndef HYD_ELEMENT_COUP_MEMBERS_H
#define HYD_ELEMENT_COUP_MEMBERS_H

//hyd system


///This class group the members for a coupling condition in a floodplain element \ingroup hyd
/**

*/
class Hyd_Element_Coup_Members
{
///Class where the members are required
friend class Hyd_Element_Floodplain_Type_Standard;

public:
	///Default constructor
	Hyd_Element_Coup_Members(void);
	///Default destructor
	~Hyd_Element_Coup_Members(void);


	//method
	///Reset volumes and discharge
	void reset_vol_q(void);

	///Get the sum of inflow volume
	double get_sum_inflow(void);
	///Get the sum of outflow volume
	double get_sum_outflow(void);

	///Get sum of coupling discharge
	double sum_coupling_discharge(void);

	///Calculate hydrological balance
	void calculate_hydro_balance(const double delta_t);

private:

	//members
	///Discharge which comming from coupled floodplain models
	double coupling_q_fp;
	///Discharge which comming from coupled river models due to overflow
	double coupling_q_rv_overflow;
	///Discharge which comming from coupled river models due to hydraulic structure coupling
	double coupling_q_rv_structure;
	///Discharge which comming from coupled river models due to dikebreak coupling
	double coupling_q_rv_dikebreak;
	///Discharge which comming from a coupled coast model due to overflow
	double coupling_q_co_overflow;
	///Discharge which comming from a coupled coast model due to dikebreak coupling
	double coupling_q_co_dikebreak;
	///Discharge, which comming from a coupled river model directly from the outflow profile 
	double coupling_q_rv_direct;
	///Make a hydrological balance for the coupling discharge to the element due to a coupling between two floodplain models
	_hyd_hydrological_balance coupling_volume_fp;
	///Make a hydrological balance for the coupling discharge to the element due to river overflow
	_hyd_hydrological_balance coupling_volume_overflow_rv;
	///Make a hydrological balance for the coupling discharge to the element due to river coupling via a hydraulic structure
	_hyd_hydrological_balance coupling_volume_structure_rv;
	///Make a hydrological balance for the coupling discharge to the element due to river coupling via a dikebreak
	_hyd_hydrological_balance coupling_volume_dikebreak_rv;
	///Make a hydrological balance for the coupling discharge to the element due to coast model coupling via a dikebreak
	_hyd_hydrological_balance coupling_volume_dikebreak_co;
	///Make a hydrological balance for the coupling discharge to the element due to costal overflow
	_hyd_hydrological_balance coupling_volume_overflow_co;
	///Make a hydrological balance for the coupling discharge from a coupled river model directly from the outflow profile
	_hyd_hydrological_balance coupling_volume_direct_rv;

  


};
#endif
