 //#include "Sys_Database_Browser.h"
#include "Sys_Headers_Precompiled.h"

 //constructor
Sys_Database_Browser::Sys_Database_Browser(void){
	this->querymodel=NULL;
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_Database_Browser));//count the memory
 }
//destructor
Sys_Database_Browser::~Sys_Database_Browser(void){
	if(this->querymodel!=NULL){
		delete this->querymodel;
		this->querymodel=NULL;
	}
	Sys_Memory_Count::self()->minus_mem((sizeof(Sys_Database_Browser)));//count the memory	
}
//____________________________
//public
//set different filter
//Set the database tablename for the request
void Sys_Database_Browser::set_tablename(const string tablename){
	this->my_tablename=tablename;
	this->table_filter << " from " << tablename;
	this->column_filter<< "Select * ";
	this->join_filter.str("");
	this->sort_filter.str("");
	this->where_filter.str("");
}

//Set a "where"-filter for selecting required data records of a table
void Sys_Database_Browser::set_where_filter(const string filter){
	this->where_filter<<" where " << filter;
}
//Set a filter for selecting required columns of a table (select)
void Sys_Database_Browser::set_column_filter(const string filter){
	this->column_filter.str("");
	this->column_filter<< "Select " << filter;
}
//Set a filter for sorting the selceting (order by)
void Sys_Database_Browser::set_sort_filter(const string column_name){
	this->sort_filter<<" order by " << column_name;
}
//Set a filter for joining two tables (left join)
void Sys_Database_Browser::set_join_filter(const string filter, const string join_tablename){
	this->join_filter<<" left join "<< join_tablename << " on " << filter;
}
//Allocate and launch the request and get a pointer to the selected results (QSqlQueryModel)
QSqlQueryModel* Sys_Database_Browser::table_to_model(QSqlDatabase *ptr_database){
	//allocate the query model
	this->querymodel =new QSqlQueryModel();
	//set the filter for the request
	ostringstream filterstream;
	filterstream<< this->column_filter.str() << this->table_filter.str(); 
	filterstream<< this->join_filter.str() << this->where_filter.str(); 
	filterstream<< this->sort_filter.str();

	//send the request to the database
	Data_Base::database_request(this->querymodel,filterstream.str(),ptr_database);


	if(this->querymodel->lastError().isValid()){
		Error msg=set_error(0);
		msg.make_second_info((this->querymodel->lastError().text().toStdString()));
		msg.make_second_info("\n");
		throw (msg);
	}
	//return the pointer to the query-model
	return querymodel;
}
//Delete the selected rows of the table (use it after launching the request with table_to_model(QSqlDatabase *ptr_database))
void Sys_Database_Browser::delete_rows(QSqlDatabase *ptr_database){
	
	ostringstream cout;
	cout << this->querymodel->rowCount() << " Numbers of record will be deleted in Table " << this->my_tablename << endl;
	Sys_Common_Output::output_fpl->output_txt(cout.str(), false);
	ostringstream del_filter;
	del_filter << " delete " << this->table_filter.str() << this->where_filter.str();
	QSqlQuery myquery(*ptr_database);
	myquery.exec(del_filter.str().c_str());
}
//Get a string for inserting a polygon-datatype to a database table field (update); "where"-filter and tablename have to be set before
string Sys_Database_Browser::get_inserting_polygon_filter(const string column_name, _Geo_Polygon *polygon){
	ostringstream filter_buff;
	//update hyd_fp_elem set poly=GEOMFROMTEXT('polygon(( 0 0, 0 1, 1 1, 1 0, 0 0))') where elem_id=0
	filter_buff << "update " << this->my_tablename;
	filter_buff << FORMAT_FIXED_REAL << P(3);
	//\todo postgre specif text
	filter_buff << " set "  << column_name << "=" << "ST_GEOMFROMTEXT('POLYGON((";


	for(int i=0; i< polygon->get_number_points(); i++){
		filter_buff<<polygon->get_point(i)->get_xcoordinate() << "  ";
		filter_buff<<polygon->get_point(i)->get_ycoordinate()<< ", ";
	}
	//set the first point again
	filter_buff<<polygon->get_point(0)->get_xcoordinate() << "  ";
	filter_buff<<polygon->get_point(0)->get_ycoordinate()<< " ";

	filter_buff << "))') ";
	filter_buff << this->where_filter.str();

	return filter_buff.str();
}
//Get a string for inserting a polyline-datatype to a database table field (update); "where"-filter and tablename have to be set before
string Sys_Database_Browser::get_inserting_polyline_filter(const string column_name, Geo_Polysegment *polyline){

	ostringstream filter_buff;
	//update hyd_fp_elem set poly=GEOMFROMTEXT('linestring( 0 0, 0 1, 1 1, 1 0, 0 0)') where elem_id=0
	filter_buff << "update " << this->my_tablename;
	filter_buff << FORMAT_FIXED_REAL << P(3);

	filter_buff << " set "  << column_name << "=" << "ST_GEOMFROMTEXT('LINESTRING(";

	for(int i=0; i< polyline->get_number_segments(); i++){
		filter_buff<<polyline->my_segment[i].point1.get_xcoordinate() << "  ";
		filter_buff<<polyline->my_segment[i].point1.get_ycoordinate()<< ", ";
	}
	//set the last point 
	filter_buff<<polyline->my_segment[polyline->get_number_segments()-1].point2.get_xcoordinate() << "  ";
	filter_buff<<polyline->my_segment[polyline->get_number_segments()-1].point2.get_ycoordinate()<< "  ";

	filter_buff << ")') ";
	filter_buff << this->where_filter.str();

	return filter_buff.str();
}
//Get a string for inserting a polyline-datatype to a database table field (update); "where"-filter and tablename have to be set before
string Sys_Database_Browser::get_inserting_polyline_filter(const string column_name, Geo_Point *points, const int number){

	ostringstream filter_buff;
	//update hyd_fp_elem set poly=GEOMFROMTEXT('linestring( 0 0, 0 1, 1 1, 1 0, 0 0)') where elem_id=0
	filter_buff << "update " << this->my_tablename;
	filter_buff << FORMAT_FIXED_REAL << P(3);
	
	filter_buff << " set "  << column_name << "=" << "ST_GEOMFROMTEXT('LINESTRING(";
	
	for(int i=0; i< number-1; i++){
		filter_buff<<points[i].get_xcoordinate() << "  ";
		filter_buff<<points[i].get_ycoordinate()<< ", ";
	}
	//set the last point 
	if(number!=0){
		filter_buff<<points[number-1].get_xcoordinate() << "  ";
		filter_buff<<points[number-1].get_ycoordinate()<< "  ";
	}

	filter_buff << ")') ";
	filter_buff << this->where_filter.str();

	return filter_buff.str();
}
//________________________________
//private
//Set error(s)
Error Sys_Database_Browser::set_error(const int err_type){
		string place="Sys_Database_Browser::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://table not found (fatal error)
			place.append("table_to_model(void)");
			reason="Table not found";
			help="Check the table name";
			type=2;
			fatal=true;//fatal error;
			break;
		case 1://bad alloc
			place.append("model_to_view(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
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