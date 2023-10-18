#include "Sys_Headers_Precompiled.h"
//#include "Geo_Convex_Polygon.h"

//constructor
Geo_Convex_Polygon::Geo_Convex_Polygon(void){
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Convex_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//destructor
Geo_Convex_Polygon::~Geo_Convex_Polygon(void){

	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Convex_Polygon)-sizeof(Geo_Simple_Polygon), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//_____________________________
//public
//Check if a given convex polygon is completly inside this convex polygon
bool Geo_Convex_Polygon::check_polygon_inside(_Geo_Polygon *check_polygon){
	bool inside_flag=false;
	int at_bound_count=0;

	//check the given polygon if it is completly inside
	for (int i=0; i<check_polygon->get_number_points();i++){
		if(this->check_point_inside(check_polygon->get_point(i))!=true){
			if(this->check_point_atboundary(check_polygon->get_point(i))!=true){
				return inside_flag=false;
			}
			else{
				at_bound_count++;
			}
		}
	}
	if(at_bound_count==check_polygon->get_number_points()){
		Geo_Point buffer;
		if(check_polygon->get_point_inside(&buffer)==true){
			if(this->check_point_inside(&buffer)!=true){
				return inside_flag=false;
			}
		}
		else{
			return inside_flag=false;
		}

	}

	//check this polygon if it is completly outside
	for(int i=0; i<this->get_number_points();i++){
		if(check_polygon->check_point_inside(this->get_point(i))==true){
			return inside_flag=false;
		}
	}

	return inside_flag=true;

}