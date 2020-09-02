#pragma once
/**\class Hyd_Coast_Model
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/
#ifndef HYD_COAST_MODEL_H
#define HYD_COAST_MODEL_H

//system_hyd class of Instationary boundary conditions
#include "Hyd_Instationary_Boundary.h"
//for parsing the parameters per file
#include "Hyd_Parse_CO.h"
//class of the coast elements
#include "Hyd_Element_Floodplain_Type_Coast.h"
//info class about the hydraulic boundary szenario data
#include "Hyd_Boundary_Szenario.h"


//system_sys classes
#include "_Sys_Common_System.h"

///Model-class for representing a coast system (CO). It is set to elements of a floodplain-model; here no numerical calculation is performed, just a global waterlevel is set \ingroup hyd
/**
	The coast-model (CO) is part of the hydraulic system (Hyd_Hydraulic_System). Just one coast-model per hydraulic system is possible.
	It is represented by a coast-polygon. All elements of the floodplain-models, which are completly inside
	this polygons are belonging to the coast model. The coast-line (outline of the coast-polygon) represents the related coupling
	polysegment to the floodplain-model(s). A river-model outflow profile, which is inside, gets its waterlevel by this coast elements.
	This model performs no numerical calculation, one global waterlevel is set to each associated element.
	\see Hyd_Element_Floodplain_Type_Coast
*/
class Hyd_Coast_Model : public _Sys_Common_System
{
public:
	///Default constructor
	Hyd_Coast_Model(void);
	///Default destructor
	~Hyd_Coast_Model(void);

	//members

	///Coastline-polysegments
	Hyd_Floodplain_Polysegment coastline_polysegment;
	///Coastline-polygon consisting of the Coastline-polysegment
	Hyd_Floodplain_Polygon coast_polygon;




	//method

	///Input all coastmodel members from files
	void input_members(const string global_file, const string global_path);

	///Transfer the coast model data to a database; the coast-line as well as the boundary condition
	void transfer_input_members2database(QSqlDatabase *ptr_database);
	///Transfer a hydraulic boundary szenario from file to a database
	void transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database);
	///Input the coast model from a database selection of the relevant coastline; the coastline as well as the boundary condition
	void input_members(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag);


	///Get the parameters of the coast model
	_hyd_coast_params get_coast_parameter(void);

	///Output the given members
	void output_members(void);
	///Output the geometrie to tecplot
	void output_geometrie2tecplot(void);

	///Output the geometrie to paraview
	void output_geometrie2paraview(void);

	///Make the syncronisation between the models and the boundaries; here set the sea waterlevel
	void make_syncronisation(const double time_point);

	///Get the global sea waterlevel
	double get_global_sea_h(void);

	///Get the global sea waterlevel
	double *get_ptr_global_sea_h(void);

	///Set the system-id (_sys_system_id) of the object
	void set_systemid(const _sys_system_id actuel_id);
	
	///Set a new hydraulic boundary scenario id to the models
	void set_hydraulic_bound_sz(Hyd_Boundary_Szenario bound_sz);

	///Clear the boundary condition, which are set
	void clear_boundary_condition(void);

	///Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
	void set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database);

	///Set the folder for the file-output in case of an database based calculation
	void set_output_folder(const string folder);

	///Clone the members of the coast model
	void clone_model(Hyd_Coast_Model *coast);

	///Get the crude version of the filename for 2d output
	string get_crude_filename_result_1d(void);

private:

	//members
	///The parameter of Hyd_Coast_Model are gathered here
	_hyd_coast_params params;
	///The global waterlevel of the sea
	double global_sea_h;

	///Member for storing the hydraulic boundary szenario information
	Hyd_Boundary_Szenario hyd_sz;
	///Instationary boundary curves for representing the coastal waterlevel
	/**
	As coastal waterlevel a global waterlevel [m] has to be applied. For a stationary boundary condition
	use the same waterlevel values at each boundary time point.
	*/
	Hyd_Instationary_Boundary instat_boundary_curve;



	//method
	///Transfer the coastl-line (Hyd_Floodplain_Polysegment) into the coast-polygon (Hyd_Floodplain_Polygon)
	void polysegment2polygon(void);
	///Get the filename for the coast-model geometrie file
	string get_filename_geometrie2tecplot(const string type);
	///Get the filename for the coast-model geometrie file
	string get_filename_geometrie2paraview(const string type);

	///Complete the filenames with the global path
	void complete_filenames_with_path(string global_path);

	///Set error(s)
	Error set_error(const int err_type);

};
#endif
