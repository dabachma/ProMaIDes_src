//#include "Hyd_River_Profile_Point.h"
#include "Hyd_Headers_Precompiled.h"

//constrortur
Hyd_River_Profile_Point::Hyd_River_Profile_Point(void){
	this->number=0;
	this->x_global=0.0; // Gauss coordinates for each profile point
	this->y_global=0.0; // Gauss coordinates for each profile point
	this->z_global=0.0; // Gauss coordinates for each profile point
	this->mat_type=0.0; // Integer value representing the material type given in a table file (Global)
	this->identity=0; // 1=Left FP, 2=Main Channel, 3=RightFP
	this->set_point_name(hyd_label::profile_point_x_z);
}
//constructor
Hyd_River_Profile_Point::~Hyd_River_Profile_Point(void){
}
//_______________________
//public
//input the point members per file
bool Hyd_River_Profile_Point::input_members(QFile *profile_file, int *point_counter, int *line_counter){
	string buffer;
	QString qbuffer;
	stringstream stream_buff;
	qint64 get_pointer=0;

	get_pointer=profile_file->pos();
	qbuffer=profile_file->readLine();
	buffer=qbuffer.toStdString();
	(*line_counter)++;
	_Hyd_Parse_IO::erase_comment(&buffer);
	_Hyd_Parse_IO::erase_carriageReturn(&buffer);
	_Hyd_Parse_IO::erase_leading_whitespace_tabs(&buffer);
	_Hyd_Parse_IO::erase_end_whitespace_tabs(&buffer);
	//rewinf the ifstream by one line
	if(buffer.empty()!=true){

		if(buffer.find("ZONE",0)!=-1){
			//rewind the line
			profile_file->seek(get_pointer);
			(*line_counter)--;
			return false;
		}
		if(functions::count_number_columns(buffer)!=6){
			Error msg=this->set_error(0);
			ostringstream info;
			info <<"Found line: " << buffer << endl;
			msg.make_second_info(info.str());
			throw msg;

		}
		this->number=*point_counter;

		//for the reason of conversion
		double buff_mat_type=0.0;
		double buff_id=1.0;

		stream_buff << buffer;
		stream_buff >> this->x_global;
		stream_buff >> this->y_global;
		stream_buff >> this->z_global;

		stream_buff >> buff_mat_type;
		this->mat_type=(int)buff_mat_type;

		stream_buff >> this->x_coordinate;
		
		stream_buff >> buff_id;
		this->identity=(int)buff_id;

		if(stream_buff.fail()==true){
			return false;
		}
		stream_buff.str("");
		stream_buff.clear();
		(*point_counter)++;
	}
	return true;
}
//Input the river profile point data from a selection of a database table with the given index
void Hyd_River_Profile_Point::input_members_per_database(const QSqlQueryModel *query_result, const int index){

	this->number=index;
	this->x_global=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_x_co)).c_str()).toDouble();
	this->y_global=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_y_co)).c_str()).toDouble();
	this->z_global=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_z_co)).c_str()).toDouble();
	this->x_coordinate=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_dist)).c_str()).toDouble();
	this->mat_type=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_mat_id)).c_str()).toInt();
	this->identity=query_result->record(index).value((_Hyd_River_Profile_Type::profile_point_table->get_column_name(hyd_label::profpoint_ch_id)).c_str()).toInt();
	
}
//output the members
void Hyd_River_Profile_Point::output_members(ostringstream *cout){
		*cout << W(8)<< this->number <<W(12) <<this->x_global << W(18) <<this->y_global << W(18) <<this->z_global ;
		*cout << W(14) <<this->x_coordinate<<W(14) <<this->y_coordinate << W(14) <<this->mat_type ;
		*cout << W(14) <<this->identity <<endl;
}
//Get the global x- and y-coodinates
Geo_Point Hyd_River_Profile_Point::get_global_x_y_coordinates(void){
	Geo_Point buffer;

	buffer.set_point_coordinate(this->x_global,this->y_global);
	buffer.set_point_name(hyd_label::profile_point_x_y);

	return buffer;

}
//Get the global z-coordinate of the point
double Hyd_River_Profile_Point::get_global_z_coordinate(void){
	return this->z_global;
}
//Set the global z-coordinate of the point
void Hyd_River_Profile_Point::set_global_z_coordinate(const double height) {
	this->z_global = height;
}
//Get the material identifier
int Hyd_River_Profile_Point::get_mat_id(void){
	return this->mat_type;
}
//Get the channel type identifier
int Hyd_River_Profile_Point::get_channel_type(void){
	return this->identity;
}
//Copy points
void Hyd_River_Profile_Point::copy_points(Hyd_River_Profile_Point *point){
	this->operator =(*point);
	this->number=point->number;
	this->x_global=point->x_global; 
	this->y_global=point->y_global; 
	this->z_global=point->z_global; 
	this->mat_type=point->mat_type; 
	this->identity=point->identity;
}
//________
//private
//set the error
Error Hyd_River_Profile_Point::set_error(const int err_type){
		string place="Hyd_River_Profile_Point::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("input_members(QFile *profile_file, int *point_counter, int *line_counter)");
			reason="There are not 6 columns found in file to specify the profile point";
			help="Check the given columns of the profile point. It must be: X  Y  Z  MatType  Distance  Ident.";
			type=1;
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