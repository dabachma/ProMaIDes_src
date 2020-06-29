#pragma once
/**\class Hyd_Element_Bound_Members
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2012
*/
#ifndef HYD_ELEMENT_BOUND_MEMBERS_H
#define HYD_ELEMENT_BOUND_MEMBERS_H

//hyd system

///Data structure for storing the boundary condition relevant parameters \ingroup hyd
struct _hyd_boundarydata_floodplain_elem{
	///Flag for a boundary condition (true=> there is one; false=> there is none)
	bool boundary_flag;
	///Flag for the type of the boundary condtion (true=>stationary; false=> instationary); just in connection with the boundary_flag
	bool stationary_flag;
	///Value for a stationary boundary condition; 
	double discharge_value;
	///Number of the instationary boundary curve
	int curve_number;
	///Flag if the instationary boundary curve could be connected
	bool found_flag;
	///Pointer to the instationary boundary curve
	Hyd_Instationary_Boundary *ptr_boundarycurve;
	///The type of the boundary condition; for the floodplain only area or point is possible
	_hyd_bound_type boundary_type;
};

///This class group the members for a boundary condition in a floodplain element \ingroup hyd
/**

*/
class Hyd_Element_Bound_Members
{

	///Class where the members are required
friend class Hyd_Element_Floodplain_Type_Standard;

public:
	///Default constructor
	Hyd_Element_Bound_Members(void);
	///Default destructor
	~Hyd_Element_Bound_Members(void);

	//method
	//Initialize the data structures of _hyd_boundarydata_floodplain_elem 
	static void init_boundarydata_structures(_hyd_boundarydata_floodplain_elem * data);

	///Reset volumes and discharge
	void reset_vol_q(void);

	///Calculate boundary value
	void calculate_bound_value(const double time, const double area);

	///Calculate hydrological balance
	void calculate_hydro_balance(const double delta_t);


private:
	//members
	///Data structure of the boundary data of the element (_hyd_boundarydata_floodplain_elem) [boundarycondition variables]
	_hyd_boundarydata_floodplain_elem boundary_data;
	///Value of the boundary discharge [boundarycondition variables]
	double q_boundary;
	///Make a hydrological balance for the boundary condition discharge [boundarycondition variables]
	_hyd_hydrological_balance boundary_volume;
};

#endif