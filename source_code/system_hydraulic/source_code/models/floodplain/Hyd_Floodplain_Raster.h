#pragma once
/**\class Hyd_Floodplain_Raster
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef  HYD_FLOODPLAIN_RASTER_H
#define HYD_FLOODPLAIN_RASTER_H


//hyd classes
//geometrie class for the raster
#include "Hyd_Floodplainraster_Polygon.h"
#include "Hyd_Floodplainraster_Segment.h"
#include "Hyd_Floodplainraster_Point.h"
#include "Hyd_Floodplainraster_Point_List.h"
//general floodplain classes
#include "Hyd_Param_FP.h"
#include "Hyd_Element_Floodplain.h"
//coupling class
#include "Hyd_Coupling_Point_FP2CO_List.h"
#include "Hyd_Coupling_Point_RV2FP_List.h"
#include "Hyd_Coupling_Point_FP2FP_List.h"

//geo classes
#include "Geo_Point_List.h"

///Geometrical raster of the floodplain model \ingroup hyd
/**
The raster consists of polygons, always rectangular, segments and points.
For the sake of compuation time the segments and the points of the raster are
generated and stored during the geometrical interceptions of the floodplain with other models, e.g.
ither floodplain models, dike lines, noflow polygons, river models etc.
In constrast the polygons are for every interception action generated with the help of the segments.

\see Hyd_Model_Floodplain, Hyd_Floodplainraster_Segment, Hyd_Floodplainraster_Polygon, Hyd_Floodplainraster_Point
*/
class Hyd_Floodplain_Raster
{
public:
	///Default constructor
	Hyd_Floodplain_Raster(void);
	///Default destructor
	~Hyd_Floodplain_Raster(void);

	//members

	///Geometrical boundary of the floodplain as a polygon
	Hyd_Floodplain_Polygon geometrical_bound;
	///Geometrical element as a raster polygon 
	Hyd_Floodplainraster_Polygon raster_elem;

	//methods
	///Set the geometric information
	void set_geometric_info(const _hyd_floodplain_geo_info geometric_info);
	///Set the geometric information per database
	void set_geometric_info(QSqlTableModel *results, const int index);
	///Get the geometric information
	_hyd_floodplain_geo_info get_geometric_info(void);
	///Generate the model geometrical boundary as a polygon in global coordinates
	void make_boundary2polygon(void);
	///Set a geometrical raster polygon with a given element index without any pointer to the floodplain element
	void set_geometrical_raster_polygon(const int element_index);

	///Output the geometrical boundary of the floodplain model
	void output_geometrical_boundary(void);
	///Output the geometrical boundary to tecplot
	void output_geometrical_boundary2tecplot(ofstream *output_file);
	///Output members
	void output_members(void);

	///Set the index of the floodplain model
	void set_index_fp_model(const int index);

	///Set a pointer to the floodplain elemnts
	void set_floodplain_elem_pointer(Hyd_Element_Floodplain *floodplain_pointer);
	///Generate and allocate the raster points and segments
	void generate_alloc_points_segments(void);
	///Delete the raster the segments
	void delete_raster_segments(void);
	///Delete the raster points
	void delete_raster_points(void);

	///Delete raster points and raster segments
	void delete_points_segments(void);

	///Get the pointer to all raster points
	Hyd_Floodplainraster_Point* get_ptr_raster_points(void);
	///Get the pointer to all rastersegments
	Hyd_Floodplainraster_Segment* get_ptr_raster_segments(void);

	///Get the number of raster points
	int get_number_raster_points(void);
	///Clone the raster points
	void clone_raster_points(Hyd_Floodplain_Raster *raster);
	///Get a raster point with a given index
	Hyd_Floodplainraster_Point * get_raster_point(const int index);

	///Set a given type to Hyd_Element_Floodplain (e.g. noflow) if a raster polygon (Hyd_Floodplain_Raster_Polygon) is inside a given polygon 
	void set_rasterpolygon_inside(Hyd_Floodplain_Polygon *polygon);
	///Set a given type to Hyd_Element_Floodplain (e.g. noflow) if a raster polygon (Hyd_Floodplain_Raster_Polygon) intercepts a given polygon 
	void set_rasterpolygon_intercepted(Hyd_Floodplain_Polygon *polygon,  _hyd_elem_type type);


	///Intercept a Hyd_Floodplainpolysegment with the raster (e.g. dikeline, riverline)
	void interception_polysegments2raster(Hyd_Floodplain_Polysegment *line);

	///Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_FP2CO_List (floodplain to coast model)
	void assign_elements2couplingpointlist(Hyd_Coupling_Point_FP2CO_List *list);
	///Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_RV2FP_List (river to floodplain model)
	void assign_elements2couplingpointlist(Hyd_Coupling_Point_RV2FP_List *list);
	///Assign the relevant elements of coupling to other models to a Hyd_Coupling_Point_RV2FP_List (floodplain to floodplain model)
	void assign_elements2couplingpointlist(Hyd_Coupling_Point_FP2FP_List *list, const bool first_fp_flag);

	///Find the index of a raster element where a given point is inside; if it is at boundary nothing is found
	int find_elem_index_by_point(Geo_Point *point);

	///Find the index of a raster element where a given point is inside or at boundary
	int find_elem_index_by_point_withboundary(Geo_Point *point);

	///Search for a convenient coupling element at neighbouring elements in case of an river element type (RV2FP couplings)
	void search_convenient_coupling_element(int *elem_index, Geo_Point point1, Geo_Point point2, const bool left_flag);

	///Set raster segments to noflow, if there are inside or at boundray of a given polygon
	void set_raster_segments2noflow(Hyd_Floodplain_Polygon *polygon);

	///Get the complete raster polygons
	void get_complete_raster_polygons(Hyd_Floodplainraster_Polygon **polygons);

protected:
	//members
	///Number of rasterpolygons = number elements
	int number_polygons;
	///Geometrical information
	_hyd_floodplain_geo_info geo_info; 


	//method
	///Convert an element index and a given neighbourig direction into an neighbouring element index
	int convert_2neighbouring_elemindex(_hyd_all_element_direction direction, const int elem_index);


private:
	//members
	///Number of rasterpoints
	int number_points;
	///Number of segments
	int number_segments;

	///Index of the floodplain model
	int index_fp_model;

	///The points of the floodplain raster
	Hyd_Floodplainraster_Point *raster_points;
	///The segments of the raster
	Hyd_Floodplainraster_Segment *raster_segments;
	///Pointer to the floodplain elements
	Hyd_Element_Floodplain *floodplain_pointer;

	///A list for the setted raster points
	Hyd_Floodplainraster_Point_List raster_point_list;


	//methods

	///Allocate the raster points
	void allocate_raster_points(void);
	///Allocate the raster segments
	void allocate_raster_segments(void);
	///Set a geometrical raster polygon with a given element index and connect the pointer to the floodplain element
	void set_raster_polygon(const int element_index);
	///Assign values (heights or noflow flags) and the corresponding index of a point to Hyd_Floodplainraster_Point_List
	void assign_points_value2list(Hyd_Floodplain_Polysegment *line);
	///Assign a value from a line to Hyd_Floodplainraster_Point (two interception point are clear on the raster segments
	bool assign_points_value2list_segments( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line);
	///Assign a value from a line to Hyd_Floodplainraster_Point (two interception are on a point
	bool assign_points_value2list_twopoints( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line);
	///Assign a value from a line to Hyd_Floodplainraster_Point (first point is hit, second is on segment) 
	bool assign_points_value2list_point_segment( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line);
	///Assign a value from a line to Hyd_Floodplainraster_Point (second point is hit, first is on segment) 
	bool assign_points_value2list_segment_point( _geo_multisegment_interception point_1, _geo_multisegment_interception point_2, Hyd_Floodplain_Polysegment *line);
	///Assign the points with values in the list via the Hyd_Floodplainraster_Segment to Hyd_Element_Floodplain
	void assign_listvalues2elements(Hyd_Floodplain_Polysegment *line);

	///Convert two interception points of a rasterpolygon into the element index
	int convert_interception_points2elemindex(_geo_multisegment_interception point_1 , _geo_multisegment_interception point_2, const _hyd_floodplain_polysegment_type type);

	///Convert two interception points of a rasterpolygon into the element index; just for one point=> the intercepting line starts in a boundary element 
	int convert_interception_points2elemindex(_geo_multisegment_interception point_1);

	///Find the index of a segment, when two point indices are given
	int point_index2segment_index(const int point_index1, const int point_index2);
	///Find the indices of two points, when a segment index is given
	void segment_index2point_index(const int seg_index, int *point_index1, int *point_index2);


	///Find element index, when two segments indices are given
	int segments_index2elem_index(const int seg_index_1 , const int seg_index_2, const _hyd_floodplain_polysegment_type type);
	///Find element index, when two point indices are given; there will be only a result, by diagonal points
	int points_index2elem_index(const int point_index_1 , const int point_index_2);
	///Find element index, when two point indices are given; if the points are on the same segment the element index is decided with the linetype
	int points_index2elem_index(const int point_index_1 , const int point_index_2, const _hyd_floodplain_polysegment_type type);
	///Find element index, when one segments index and one point index are given
	int points_segment_index2elem_index(const int seg_index , const int point_index, const _hyd_floodplain_polysegment_type type);

	///Set an element type (_hyd_elem_type) to the element with the given element index
	void set_elem_type_per_elemindex(const int elem_index, _hyd_elem_type type);

	///Decide the element index by a given _hyd_floodplain_polysegment_type and two element indices
	int decide_elemindex_per_type(const int index_1, const int index_2, const _hyd_floodplain_polysegment_type type);

	//Convert an angle to a correspondig direction of a neighbouring raster element (four direction)
	_hyd_all_element_direction convert_angle2four_direction(const double angle);
	///Convert an angle to a correspondig direction of a neighbouring raster element (eight direction)
	_hyd_all_element_direction convert_angle2direction(const double angle);
	///Convert an angle to a second possible direction of a neighbouring raster element (eight direction)
	_hyd_all_element_direction convert_angle2seconddirection(const double angle);

	
	///Calculate the distances between two given elment mid point
	double calculate_distance_midpoints(const int index_1, const int index_2);

	///Set error(s)
	Error set_error(const int err_type);
};

#endif