#pragma once
/**\class _Hyd_Coupling_Point_List
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef _HYD_COUPLING_POINT_LIST_H
#define _HYD_COUPLING_POINT_LIST_H

//system_sys_ classes
#include "_Sys_Common_System.h"

//system hyd 
#include "_Hyd_Coupling_Point.h"
#include "Hyd_Floodplain_Polysegment.h"
#include "Hyd_Model_Floodplain.h"
#include "Hyd_Model_River.h"

///Base class for the list, where the couplingpoints between the models are gathered \ingroup hyd
/**
 The lists for the couplingpoints gather all points for a linear coupling between the models. Thus,
 each list is related to a polysegment (Hyd_Floodplain_Polysegment), e.g. coast-lines, riverbank-lines etc.
 The lists are needed for different couplings:
 -coupling between river- and floodplain-models
 -coupling between coast- and floodplain-models
 -coupling between floodplain- and floodplain-models
 Punctuell couplings, e.g. river- to river-models do not need any list.
 \see _Hyd_Coupling_Point

*/
class _Hyd_Coupling_Point_List
{
public:
	///Default constructor
	_Hyd_Coupling_Point_List(void);
	///Default destructor
	virtual ~_Hyd_Coupling_Point_List(void);

	//method
	///Get the number of coupling points
	virtual int get_number_couplings(void)=0;
	///Output the setted members of the list
	virtual void output_setted_members(ostringstream *cout)=0;
	///Set the defining line
	void set_defining_polysegment(Hyd_Floodplain_Polysegment *polysegment);
	///Get a pointer of the defining line
	Hyd_Floodplain_Polysegment* get_defining_polysegment(void);
	///Delete the list
	virtual void delete_list(void)=0;

	///Calculate for all points the distance to the next upward point and downward point
	virtual void calculate_alldistance_up_down(void)=0;

	///Sort the points after the distance to the beginning of the defining polysegment
	virtual void sort_distance_along_polysegment(void)=0;

	///Add the relevant coupling points of the defining_polysegment
	virtual void add_relevant_polysegment_points(Hyd_Floodplain_Polygon *fp_boundary);

	///Transfer the information of the coupled elements/profiles etc to the coupling points
	virtual void transfer_informations2points(void)=0;

	///Reset the coupling dicharges of the coupling points
	virtual void reset_coupling_discharge(void)=0;

	///Reset the coupling points
	virtual void reset_points(void)=0;

	///Clone the coupling point list
	void clone_list(_Hyd_Coupling_Point_List *list);


protected:

	//member
	///Number of list-elements in a block for allocation
	const int block_elems;
	///Number of points in list
	int number;
	///Defining polysegment; all points have to be on this polysegment
	Hyd_Floodplain_Polysegment *defining_polysegment;

	///Constant for the minimal distance between the coupling points
	const double min_distance;



	//method
	///Delete the allocated coupling points
	virtual void delete_couplingpoints(void)=0;
	///Allocate the coupling points
	virtual void allocate_couplingpoints(void)=0;
	///Calculate the distance to the startpoint
	virtual void calculate_distances(void)=0;

	///Set error(s)
	Error set_error(const int err_type);
	
};

#endif
