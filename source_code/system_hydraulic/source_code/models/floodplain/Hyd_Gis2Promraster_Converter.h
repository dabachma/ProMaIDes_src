#pragma once
/**\class Hyd_Gis2Promraster_Converter
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/
#ifndef  HYD_GIS2PROMRASTER_CONVERTER_H
#define HYD_GIS2PROMRASTER_CONVERTER_H

//sys system
#include "Sys_Multi_Filechooser_Dia.h"
//hyd system
#include "Hyd_Floodplain_Raster.h"


///Class to convert a given GIS-raster as ascii into a ProMaIDes-raster \ingroup hyd
/**
The ProMaIDes-raster can be rotated, the element can be rectangular. The output of ArcGis is the main source
for the conversion.

\see Hyd_Floodplain_Raster
*/
class Hyd_Gis2Promraster_Converter : public Hyd_Floodplain_Raster
{
public:
	///Default constructor
	Hyd_Gis2Promraster_Converter(void);
	///Default destructor
	~Hyd_Gis2Promraster_Converter(void);


	//members

	//method
	///Read in and set the Gis raster
	void input_gis_raster(QString file_name);
	///Read in and set the Prom-raster
	void input_prom_raster(QString file_name);
	///Intercept the rasters
	bool intercept_rasters(Hyd_Gis2Promraster_Converter *gis_raster);
	///Output the converted Prom-raster to file
	void output_prom_raster2file(void);


private:


	//members
	///List of the secondary information per raster element
	QList<double> *second_infos;

	///Type of the raster
	bool prom_raster;

	///No-info value
	double noinfo_value;

	///File name
	string file_name;

	//Last found index of the element
	int last_found_id;

	///List where a flag is stored, if a element is completly outside a given raster
	QList<bool> outside_list;


	//Method
	///Read the raster information per file
	void read_raster_infos(ifstream *ifile, int *line_counter);
	///Read the charactersistics of the elements per file
	void read_raster_elem_characteristic(ifstream *ifile, int *line_counter);
	///Find the key values for the file input
	void find_key_values_file(string myline , int *must_found_counter);

	///Allocate the secondary information per raster element
	void allocate_secondary_info(void);
	///Delete the secondary information per raster element 
	void delete_secondary_info(void);

	///Check the geometrical information of the raster
	void check_raster_info(void);

	///Sort raster characteristics from read down-right to up-left
	void sort_raster_characteristics(void);
	///Check raster as identical
	bool check_raster_identical(Hyd_Gis2Promraster_Converter *gis_raster);

	///Search for the elemet
	void search_element(Geo_Point *mid, Hyd_Gis2Promraster_Converter *gis_raster, const int id_element); 


	///Calculate the value for each element
	void calculate_value_element(const double no_info);

	///Set if elements are completely outside the raster
	void set_completly_ouside_elements(Hyd_Floodplain_Polygon *gis_bound);

	///Set warning(s)
	Warning set_warning(const int warn_type);
	///Set error(s)
	Error set_error(const int err_type);

};
#endif
