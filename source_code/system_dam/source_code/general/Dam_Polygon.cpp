#include "Dam_Headers_Precompiled.h"
//#include "Dam_Polygon.h"
//Default constructor
Dam_Polygon::Dam_Polygon(void){

	this->index=-1;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_Polygon), _sys_system_modules::DAM_SYS);
}
//Default destructor
Dam_Polygon::~Dam_Polygon(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_Polygon), _sys_system_modules::DAM_SYS);
}
//___________
//public
//Input the members per file
void Dam_Polygon::input_members(const int index, const string filename){
	ifstream ifile;
	//open file
	ifile.open(filename.c_str(), ios_base::in);
	if(ifile.is_open()==false){
		Error msg=this->set_error(0);
		ostringstream info;
		info << "Filename: " << filename << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
	int line_counter=0;
	string myline;
	int pos=-1;
	stringstream my_stream;
	int found_index=-1;
	int found_points=0;
	bool found_flag=false;
	

	this->index=index;

	try{
		do{
			//search for the begin of the curve
			do{
				getline(ifile, myline,'\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_carriageReturn(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
				pos=myline.rfind("!BEGIN");
				if(pos>=0){
					myline.erase(pos+6);
				}

			}
			while(myline!="!BEGIN" && ifile.eof()!=true);
			if(ifile.eof()==true){
				break;
			}

			
			do{
				//next line for the curve number and number of points
				getline(ifile, myline,'\n');
				line_counter++;
				_Hyd_Parse_IO::erase_comment(&myline);
				_Hyd_Parse_IO::erase_carriageReturn(&myline);
				_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
				_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			}
			while(myline.empty()==true);
			//read it in
			my_stream << myline;
			string typ_buff;
			my_stream >> found_index >>found_points;
			if(my_stream.fail()==true){
				ostringstream info;
				info << "Wrong input sequenze  : " << my_stream.str() << endl;
				info << "Filename              : " << filename << endl;
				info << "Error occurs near line: "<< line_counter << endl;
				Error msg=this->set_error(1);
				msg.make_second_info(info.str());
				throw msg;
			}
			my_stream.clear();
			my_stream.str("");

			//compare it with the given index
			if(found_index==index){
				found_flag=true;
				try{
					this->set_number_points(found_points);
				}
				catch(Error msg){
					ostringstream info;
					info << "Polygon index         : " << this->index << endl;
					info << "Filename              : " << filename << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					msg.make_second_info(info.str());
					throw msg;
				}
			}
			else{
				found_index=-1;
			}

		}while(found_flag!=true);


		if(found_flag!=true){
			ostringstream info;
			info << "Searched index of the polygon: " << this->index << endl;
			info << "Filename                     : " << filename << endl;
			Error msg=this->set_error(2);
			msg.make_second_info(info.str());
			throw msg;
		}

		int counter=0;
		double x_buffer=0.0;
		double y_buffer=0.0;
		Geo_Point *point_buff=NULL;
		try{
			point_buff=new Geo_Point[this->number_segments];
		}catch(bad_alloc &){
			Error msg=this->set_error(5);
			throw msg;
		}


		//read in the bound points
		for(int i=0; i<this->number_segments; i++){

			//delete the comments
			getline(ifile, myline,'\n');
			line_counter++;
			_Hyd_Parse_IO::erase_comment(&myline);
			_Hyd_Parse_IO::erase_carriageReturn(&myline);
			_Hyd_Parse_IO::erase_leading_whitespace_tabs(&myline);
			_Hyd_Parse_IO::erase_end_whitespace_tabs(&myline);
			if(myline.empty()!=true){
				pos=myline.rfind("!END");
				if(pos>=0){
					myline.erase(pos+4);
				}
				//stop when end is reach
				if(myline=="!END"|| ifile.eof()==true){
					break;
				}
				//read it in
				my_stream << myline;
				my_stream >> x_buffer >> y_buffer;
				if(my_stream.fail()==true){
					ostringstream info;
					info << "Wrong input sequenze  : " << my_stream.str() << endl;
					info << "Filename              : " << filename << endl;
					info << "Error occurs near line: "<< line_counter << endl;
					Error msg=this->set_error(3);
					msg.make_second_info(info.str());
					throw msg;
				}
				my_stream.clear();
				my_stream.str("");
				
				point_buff[counter].set_point_coordinate(x_buffer, y_buffer);
				counter++;

			}
			else{
				i--;
				if (ifile.eof() == true) {
					break;
				}
			}

		}
		//check if all is read
		if(counter!=this->number_segments){
			ostringstream info;
			info << "Filename              : " << filename << endl;
			info << "Error occurs near line: "<< line_counter << endl;
			Error msg=this->set_error(4);
			msg.make_second_info(info.str());
			throw msg;
		}

		try{
			//points to polygon
			this->set_points(point_buff);
		}
		catch(Error msg){
			ostringstream info;
			info << "Polygon index " << this->index << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//delete the point_buff
		if(point_buff!=NULL){
			delete []point_buff;
			point_buff=NULL;
		}
	}
	catch(Error msg){
		//close file
		ifile.close();
		throw msg;
	}

	//close file
	ifile.close();


}
//__________
//private
//set the error
Error Dam_Polygon::set_error(const int err_type){
		string place="Dam_Polygon::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not open file
			place.append("input_members(const int index, const string filename)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 1://wrong input sequenze
			place.append("input_members(const int index, const string filename)");
			reason="Wrong input by reading the index and the number of points";
			help="Check the file";
			type=5;
			break;
		case 2://do not found the index
			place.append("input_members(const int index, const string filename)");
			reason="Can not find the given index";
			help="Check the file and the given index";
			type=5;
			break;
		case 3://can not read in the x-ycoordinates properly
			place.append("input_members(const int index, const string filename)");
			reason="Can not read in the x-, y-coordinates properly of the noflow-polygon";
			help="Check the coordinates in file";
			type=5;
			break;
		case 4://not all points are found
			place.append("input_members(const int index, const string filename)");
			reason="Not all points for the DAM-polygon are found";
			help="Check the number of points and the given point coordinates";
			info << "Number of searched points " << this->number_segments << endl;
			type=5;
			break;
		case 5://can not allocate the point buffer
			place.append("input_members(const int index, const string filename)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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