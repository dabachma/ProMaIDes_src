//#include "Geo_Polysegment.h"
#include "Sys_Headers_Precompiled.h"

//Default constructor
Geo_Polysegment::Geo_Polysegment(void){
	this->my_segment=NULL;
	this->number_points=0;
	this->number_segments=0;
	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;
	this->is_closed_flag=false;
	
	//common
	Sys_Memory_Count::self()->add_mem(sizeof(Geo_Polysegment), _sys_system_modules::GEOSYS_SYS);//count the memory

}
//Default destructor
Geo_Polysegment::~Geo_Polysegment(void){

	this->delete_segment();

	//common
	Sys_Memory_Count::self()->minus_mem(sizeof(Geo_Polysegment), _sys_system_modules::GEOSYS_SYS);//count the memory
}
//________________________
//public
//Set the points of the polysegment and the number of points; the segments are here generated
void Geo_Polysegment::set_segments(const int number, Geo_Point *points){
	this->number_points=number;
	this->number_segments=this->number_points-1;
	//allocate the points 
	try{
		this->my_segment=new Geo_Segment[this->number_segments];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	//check points
	try{
		this->check_points(points);
	}
	catch(Error msg){
		throw msg;
	}

	//copy the points into the segments of this class
	for(int i=0; i<this->number_segments; i++){
		this->my_segment[i].set_points(&points[i],&points[i+1]);
	}
	//check segments
	try{
		this->check_simple_polysegment();
	}
	catch(Error msg){
		throw msg;
	}

	//calc the values
	this->calc_total_length();
}
//Set number of points
void Geo_Polysegment::set_number_of_points(const int number){
	this->number_points=number;
	if(this->is_closed_flag==false){
		this->number_segments=this->number_points-1;
	}
	else{
		this->number_segments=this->number_points;
	}
}
///Get number of points
int Geo_Polysegment::get_number_of_points(void){
	return this->number_points;
}
//Set the points (corresponding to the before setted number; the segments are then generated)
void Geo_Polysegment::set_points(Geo_Point *points){
	this->delete_segment();
	//allocate the points 
	this->allocate_segment();
	//check points
	try{
		this->check_points(points);
	}
	catch(Error msg){
		throw msg;
	}
	//copy the points into the segmenst of this class
	if(this->is_closed_flag==false){
		for(int i=0; i<this->number_segments; i++){
			this->my_segment[i].set_points(&points[i],&points[i+1]);
		}
	}
	else{
		for(int i=0; i<this->number_segments-1; i++){
			this->my_segment[i].set_points(&points[i],&points[i+1]);
		}
		//set the last segment
		this->my_segment[this->number_segments-1].set_points(&points[this->number_points-1],&points[0]);

	}
	//check segments
	try{
		this->check_simple_polysegment();
	}
	catch(Error msg){
		throw msg;
	}


	//calc the values
	this->calc_total_length();
}
//Set a closed polysegment with a Geo_Simple_Polygon
void Geo_Polysegment::set_polysegment(Geo_Simple_Polygon *polygon){
	this->is_closed_flag=true;
	this->set_number_of_points(polygon->get_number_points());

	//allocate the points 
	try{
		this->my_segment=new Geo_Segment[this->number_segments];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}
	//set it
	for(int i=0; i<this->number_segments-1; i++){
		this->my_segment[i].set_points(polygon->get_point(i),polygon->get_point(i+1));
	}
	//set the last segment this->number_points-1
	this->my_segment[this->number_segments-1].set_points(polygon->get_point(this->number_points-1),polygon->get_point(0));

	
	//check segments
	try{
		this->check_simple_polysegment();
	}
	catch(Error msg){
		throw msg;
	}

	//calc the values
	this->calc_total_length();
}
//Get the number of segments
int Geo_Polysegment::get_number_segments(void){
	return this->number_segments;
}
//Get the index of the point with the nearest point to the given on
int Geo_Polysegment::get_nearest_point_index(Geo_Point *point){
	double distance=0.0;
	double min_distance=this->my_segment[0].point1.distance(point);;
	int min_point_index=0;
	for(int i=1; i< this->number_segments ; i++){
		distance=this->my_segment[i].point1.distance(point);
		if(distance<=min_distance){
			min_distance=distance;
			min_point_index=i;
		}
	}
	//check the last point
	distance=this->my_segment[this->number_segments-1].point2.distance(point);
	if(distance<=min_distance){
		min_distance=distance;
		min_point_index=this->number_segments;
	}

	return min_point_index;
}
//Get the total distance of the polysegment (start to end)
double Geo_Polysegment::get_total_distance(void){
	return this->total_length;
}
//Get the total horizontal distance (start to end)
double Geo_Polysegment::get_horizontal_distance(void){
	return this->total_horizontal;
}
//Get the total vertical distance (start to end)
double Geo_Polysegment::get_vertical_distance(void){
	return this->total_vertical;
}
//Get the distance along a polysegment from the beginning to a given point
double Geo_Polysegment::get_distance_along_polysegment(Geo_Point * check_point){

	int index_segment=-1;
	double length=0.0;
	//find at which segment the point is
	for(int i=0; i< this->number_segments; i++){
		if(this->my_segment[i].calc_point_on_segment(check_point)==true){
			index_segment=i;
			break;
		}
	}
	if(index_segment==-1){
		Error msg=this->set_error(3);
		ostringstream info;
		info << FORMAT_FIXED_REAL << P(5)<< "Given point: x " << check_point->get_xcoordinate() << " y " << check_point->get_ycoordinate() << endl;
		this->output_members(&info);


		msg.make_second_info(info.str());
		throw msg;
	}
	//sum up the length 
	for(int i=0; i< index_segment; i++){
		length=length+this->my_segment[i].get_distance();
	}
	//sum up the rest to the point
	length=length+this->my_segment[index_segment].point1.distance(check_point);

	return length;
}
//Get a pointer to the last segment
Geo_Segment* Geo_Polysegment::get_last_segment(void){
	if(this->number_segments>0){
		return &(this->my_segment[this->number_segments-1]);
	}
	else{
		return NULL;
	}
}
//Get a pointer to the last point
Geo_Point* Geo_Polysegment::get_last_point(void){
	if(this->number_segments>0){
		return &(this->my_segment[this->number_segments-1].point2);
	}
	else{
		return NULL;
	}
}
//Get a pointer to the first segment
Geo_Segment* Geo_Polysegment::get_first_segment(void){
	if(this->number_segments>0){
		return &(this->my_segment[0]);
	}
	else{
		return NULL;
	}
}
//Get a pointer to the first point
Geo_Point* Geo_Polysegment::get_first_point(void){
	if(this->number_segments>0){
		return &(this->my_segment[0].point1);
	}
	else{
		return NULL;
	}
}
//Get a pointer to a segment by a given index
Geo_Segment* Geo_Polysegment::get_segment(const int index){
	if(index >=0 && index < this->number_segments){
		return &(this->my_segment[index]);
	}
	else{
		return NULL;
	}
}
//Get the maximal y-value of the cubature
double Geo_Polysegment::get_max_y(void){
	double max=0.0;



	for(int i=0; i< this->number_segments; i++){
		if(i==0){
			max=this->my_segment[i].point1.get_ycoordinate();
		}
		else{
			if(max < this->my_segment[i].point1.get_ycoordinate()){
				max=this->my_segment[i].point1.get_ycoordinate();
			}
		}
		if(i==this->number_segments-1){
			if(max < this->my_segment[i].point2.get_ycoordinate()){
				max=this->my_segment[i].point2.get_ycoordinate();
			}
		}
	}
	return max;
}
//Calculate the first _geo_interception_point with a Geo_Straight_Line
_geo_interception_point Geo_Polysegment::calc_first_interception(Geo_Straight_Line *my_line){
	_geo_interception_point intercept_point;
	//init the intercept point
	intercept_point.indefinite_flag=false;
	intercept_point.interception_flag=false;
	intercept_point.interception_point.set_point_name(label::interception_point);

	for(int i=0;i<this->number_segments;i++){
		//set the points to a segment
		this->my_segment[i].calc_interception(my_line, &intercept_point);
		if(intercept_point.interception_flag==true){
			return intercept_point;
		}
	}
	return intercept_point;
}
//Calculate the last _geo_interception_point with a Geo_Straight_Line
_geo_interception_point Geo_Polysegment::calc_last_interception(Geo_Straight_Line *my_line){
	_geo_interception_point intercept_point;
	//init the intercept point
	intercept_point.indefinite_flag=false;
	intercept_point.interception_flag=false;
	intercept_point.interception_point.set_point_name(label::interception_point);
	Geo_Interception_Point_List buffer;

	for(int i=0;i<this->number_segments;i++){
		//set the points to a segment
		this->my_segment[i].calc_interception(my_line, &intercept_point);
		if(intercept_point.interception_flag==true){
			_geo_multisegment_interception buffer2;
			buffer2.distance=0.0;
			buffer2.index_intercepts=-1;
			buffer2.index_is_intercept=-1;
			buffer2.index_point_is_intercepted=-1;
			buffer2.line_interception=intercept_point;
			buffer.set_new_point(&buffer2);
		}
	}
	return buffer.get_last_point().line_interception;

}
//Calculate the first _geo_interception_point with a Geo_Segment
_geo_interception_point Geo_Polysegment::calc_first_interception(Geo_Segment *my_line) {
	_geo_interception_point intercept_point;
	//init the intercept point
	intercept_point.indefinite_flag = false;
	intercept_point.interception_flag = false;
	intercept_point.interception_point.set_point_name(label::interception_point);

	for (int i = 0; i < this->number_segments; i++) {
		//set the points to a segment
		this->my_segment[i].calc_interception(my_line, &intercept_point);
		if (intercept_point.interception_flag == true) {
			return intercept_point;
		}
	}
	return intercept_point;
}
//Calculate the last _geo_interception_point with a Geo_Segment
_geo_interception_point Geo_Polysegment::calc_last_interception(Geo_Segment *my_line) {
	_geo_interception_point intercept_point;
	//init the intercept point
	intercept_point.indefinite_flag = false;
	intercept_point.interception_flag = false;
	intercept_point.interception_point.set_point_name(label::interception_point);
	Geo_Interception_Point_List buffer;

	for (int i = 0; i < this->number_segments; i++) {
		//set the points to a segment
		this->my_segment[i].calc_interception(my_line, &intercept_point);
		if (intercept_point.interception_flag == true) {
			_geo_multisegment_interception buffer2;
			buffer2.distance = 0.0;
			buffer2.index_intercepts = -1;
			buffer2.index_is_intercept = -1;
			buffer2.index_point_is_intercepted = -1;
			buffer2.line_interception = intercept_point;
			buffer.set_new_point(&buffer2);
		}
	}
	if (buffer.get_number_points() > 0) {
		return buffer.get_last_point().line_interception;
	}
	else {
		return intercept_point;
	}

}
//Calculate the interception points with a Geo_Segement and fill the Geo_Interception_Point_List 
void Geo_Polysegment::calculate_segment_interception(Geo_Interception_Point_List *intercept_list, Geo_Segment *check_segment){
	//make the interception
	_geo_multisegment_interception buffer;
	buffer.line_interception.interception_point.set_point_name(label::interception_point);


	for(int i=0; i< this->number_segments;i++){
		this->my_segment[i].calc_interception(check_segment, &(buffer.line_interception));
		if(buffer.line_interception.interception_flag==true){
			buffer.index_intercepts=0;
			buffer.index_is_intercept=i;
			//check if a point is hit
			if(buffer.line_interception.interception_point==this->my_segment[i].point1){
				buffer.index_point_is_intercepted=0;
			}
			else if(buffer.line_interception.interception_point==this->my_segment[i].point2){
				buffer.index_point_is_intercepted=1;
			}
			else{
				buffer.index_point_is_intercepted=-1;
			}

			try{
				buffer.distance=this->get_distance_along_polysegment(&(buffer.line_interception.interception_point));
			}
			catch(Error msg){
				throw msg;
			}
			intercept_list->set_new_point(&buffer);
			
		}
	}
}
//Output the members to display/console
void Geo_Polysegment::output_members(ostringstream *cout){
	*cout <<" GEO_POLYSEGMENT"<<endl;
	*cout <<"  Number of points (segments)  : " << this->number_points << " (" << this->number_segments<<") "<< endl;
	*cout <<"  Horizontal distance          : " << this->total_horizontal <<label::m<< endl;
	*cout <<"  Vertical distance            : " << this->total_vertical <<label::m<< endl;
	*cout <<"  Distance total               : " << this->total_length << label::m << endl;
	for(int i=0;i< this->number_segments;i++){
		*cout << i <<" ";
		this->my_segment[i].output_members(cout);
	}
}
//Set if the line is closed (true) or not (false)
void Geo_Polysegment::set_is_closed(const bool flag){
	this->is_closed_flag=flag;
}
//Get if the line is closed (true) or not (false)
bool Geo_Polysegment::get_is_closed(void){
	return this->is_closed_flag;
}
//Copy polysegment
void Geo_Polysegment::copy_polysegment(Geo_Polysegment *seg){
	this->number_points=seg->number_points;
	this->number_segments=seg->number_segments;
	this->total_length=seg->total_length;
	this->total_horizontal=seg->total_horizontal;
	this->total_vertical=seg->total_vertical;
	this->is_closed_flag=seg->is_closed_flag;
	
	//allocate the points 
	try{
		this->my_segment=new Geo_Segment[this->number_segments];
	}
	catch(bad_alloc&t){
		Error msg=this->set_error(0);
		ostringstream info;
		info << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	for(int i=0; i< this->number_segments ; i++){
		this->my_segment[i]=seg->my_segment[i];
	}

}
//Get a string for inserting a polysegment to a sql-database 
string Geo_Polysegment::get_polysegment2sql_string(void) {
	ostringstream buff;


	//todo
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('LINESTRING( ";
	for (int i = 0; i < this->get_number_segments(); i++) {
		buff << this->my_segment[i].point1.get_xcoordinate() << "  ";
		buff << this->my_segment[i].point1.get_ycoordinate() << ", ";
	}
	//set the last point 
	buff << this->my_segment[this->get_number_segments() - 1].point2.get_xcoordinate() << "  ";
	buff << this->my_segment[this->get_number_segments() - 1].point2.get_ycoordinate() << "  ";

	buff << ")') ";
	return buff.str();






}
//Get a string for inserting a polysegment to a sql-database (static)
string Geo_Polysegment::get_line2sql_string(const double x_coor1, const double y_coor1, const double x_coor2, const double y_coor2) {
	ostringstream buff;


	//todo
	buff << FORMAT_FIXED_REAL << P(3);
	buff << " ST_GEOMFROMTEXT('LINESTRING( ";
	
	buff << x_coor1 << "  ";
	buff << y_coor1 << ", ";
	
	//set the last point 
	buff << x_coor2 << "  ";
	buff << y_coor2 << "  ";

	buff << ")') ";
	return buff.str();

}
//______________
//private
//calculate the total length (begin and end); also vertical and horizontal distance 
void Geo_Polysegment::calc_total_length(void){
	this->total_length=0.0;
	this->total_horizontal=0.0;
	this->total_vertical=0.0;

	for(int i=0;i<this->number_segments;i++){
		//set the points to a segment
		this->total_length=this->total_length+this->my_segment[i].get_distance();
		this->total_horizontal=this->total_horizontal+this->my_segment[i].get_horizontal_dist();
		this->total_vertical=this->total_vertical+this->my_segment[i].get_vertical_dist();
	}
}
//Check the points
void Geo_Polysegment::check_points(Geo_Point *points){
	for(int i=0; i< this->number_points-1;i++){
		if(points[i]==points[i+1]){
			Error msg=this->set_error(1);
			ostringstream info;
			info<<P(10)<< "Pointnumbers: " << i << " to " << i+1 << endl;
			info<< "x= "<< points[i].get_xcoordinate() <<"  y="<< points[i].get_ycoordinate()<< endl;
			msg.make_second_info(info.str());
			throw msg;
		}
	}
}
//check if the polysegemnt is a simple one=> no interception of the segments
void Geo_Polysegment::check_simple_polysegment(void){

	_geo_interception_point intercept_point;
	intercept_point.interception_point.set_point_name(label::interception_point);



	for (int i=0; i<this->number_segments;i++){
		for (int j=0; j<this->number_segments;j++){
			if(j!=i && j!=i+1 && j!=i-1){
				this->my_segment[i].calc_interception(&(this->my_segment[j]),&intercept_point);
				if(intercept_point.interception_flag==true && intercept_point.interception_point!=this->my_segment[i].point1 &&
					intercept_point.interception_point!=this->my_segment[i].point2){
					Error msg=this->set_error(2);
					ostringstream info;
					info <<"Interception between S.No " << i << " and S.No. " << j << endl;
					info << P(10)<<"x " << intercept_point.interception_point.get_xcoordinate() << " y " << intercept_point.interception_point.get_ycoordinate() << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			else if(j==i+1 || j==i-1){
				this->my_segment[i].calc_interception(&(this->my_segment[j]),&intercept_point);
				if(intercept_point.indefinite_flag==true){
					Error msg=this->set_error(2);
					ostringstream info;
					info <<"Interception between S.No " << i << " and S.No. " << j << endl;
					info << P(10)<<"x " << intercept_point.interception_point.get_xcoordinate() << " y " << intercept_point.interception_point.get_ycoordinate() << endl;
					
					msg.make_second_info(info.str());
					throw msg;
				}
			}
		}
	}

}
//Allocate the segments
void Geo_Polysegment::allocate_segment(void){
	//allocate the points 
	try{
		this->my_segment=new Geo_Segment[this->number_segments];
	}
	catch(bad_alloc&){
		Error msg=this->set_error(0);
		throw msg;
	}
}
//Delete the segments
void Geo_Polysegment::delete_segment(void){
	if(this->my_segment!=NULL){
		delete []this->my_segment;
		this->my_segment=NULL;
	}
}
//set the error
Error Geo_Polysegment::set_error(const int err_type){
		string place="Geo_Polysegment::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("set_points(const int number, Geo_Point *points)/set_points(Geo_Point *points)/copy_polysegment(Geo_Polysegment *seg)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://same points is not possible
			place.append("check_points(Geo_Point *points)");
			reason="Same sequentiell points are given for the polygon";
			help="Check the points for the polygon";
			type=11;
			break;
		case 2://no simple polysegment
			place.append("check_simple_polysegment(void)");
			reason="The segments of the polysegment are intercepting each other => no simple polysegment";
			help="Check the polysegment";
			type=11;
			break;
		case 3://point is not on the polysegment
			place.append("get_distance_along_line(Geo_Point * check_point)");
			reason="Can not evaluate the length to the point along the polysegment, because the point is not on the polysegment";
			help="Check the given point";
			type=6;
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
