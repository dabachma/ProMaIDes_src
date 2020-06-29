//#include "Fpl_Data_Results.h"
#include "Fpl_Headers_Precompiled.h"

//constructor
Fpl_Data_Results::Fpl_Data_Results(void){
	this->my_browser_all=NULL;
	this->my_browser_one=NULL;
	this->win_result=NULL;
	this->my_layout=NULL;
	this->frc_plot=NULL;
	this->frccurve=NULL;
	Memory_Count::self()->add_mem(sizeof(Fpl_Data_Results));//count the memory
}
//destructor
Fpl_Data_Results::~Fpl_Data_Results(void){
	if(this->my_layout!=NULL){
		delete this->my_layout;
		this->my_layout=NULL;
	}
	if(this->my_browser_all!=NULL){
		delete this->my_browser_all;
		this->my_browser_all=NULL;
	}
	if(this->my_browser_one!=NULL){
		delete this->my_browser_one;
		this->my_browser_one=NULL;
	}
	if(this->win_result!=NULL){
		delete this->win_result;
		this->win_result=NULL;
	}
	Memory_Count::self()->minus_mem((sizeof(Fpl_Data_Results)));//count the memory
}
//______________________
//public
//decide per console which action is desired
void Fpl_Data_Results::action(void){
	int action_number=0;
	bool stop_flag=false;
	bool flag=false;

	try{
		do{
			//output requriered mem
			Memory_Count::self()->output_mem();
			//read in which action
			do{
				cout << endl;
				cout << "=======================================================================" <<endl;
				cout << "AreaState " << this->system_id.area_state << " Measure " << this->system_id.area_state << endl;
				cout << "=======================================================================" <<endl;
				cout << "FPL-RESULTS-MENU " << endl;
				cout << "Which action do you want to make ?" << endl;
				cout << " (0) Go back to the FPL-DATA-MENU " << endl;
				cout << " (1) Show Frc-Table for all available section" << endl;
				cout << " (2) Show Frc-Table and -Graph for one section" << endl;
				if(!(flag=(cin  >> action_number))){//check if the input is valif
					cout << "Wrong input! Do it again" << endl;
					cin.clear();//clear the error flags
					cin.ignore();//clear cin
				}
				else if(action_number < 0 || action_number>4){
					cout << "Wrong number! Do it again! " << endl;
				}
			}
			while(!flag || action_number<0 || action_number>4);

			//make the action
			switch(action_number){
				case 0:
					stop_flag=true;
					 break;
				case 1:
					this->show_frc_table();
					 break;
				case 2:
					this->show_graph_frc(this->ask_section("all"));
					break;
			}
		}
		while(!stop_flag);
	}catch(Error msg){
		throw msg;
	}
}
//show frc-table
QTableView* Fpl_Data_Results::show_frc_table(void){
	try{
		this->my_browser_all=new Database_Viewer;
		try{
			Frc_Sim::set_output_table(this->ptr_database->get_database());
		}
		catch(Error msg){
			throw msg;
		}
		this->my_browser_all->set_tablename(Frc_Sim::output_table->table_name.name.c_str());
		ostringstream sort_filter;
		sort_filter<< Frc_Sim::output_table->get_column_name("SECTIONNR") << ", " ;
		sort_filter << Frc_Sim::output_table->get_column_name("WATERLEVEL");
		this->my_browser_all->set_sort_filter(sort_filter.str().c_str());
		this->my_browser_all->table_to_model(this->ptr_database->get_database());
		
		if(this->ptr_outputwin==NULL){
			this->my_browser_all->model_to_view()->showMaximized();
			QApplication::exec();
			return 0;
		}
		else{
			return (this->my_browser_all->model_to_view());
		}
	}
	catch(bad_alloc&){
		Error msg=set_error(1);
		throw msg;
		
	}
	catch(Error msg){
		throw msg;
	}
}
//frc- table to graph
QWidget* Fpl_Data_Results::show_graph_frc(const int actuel_sec_num){
	try{
		//make the parentwindow
		this->win_result=new QWidget;
		//make the layout
		this->my_layout=new QBoxLayout(QBoxLayout::TopToBottom, this->win_result);
		//set the table with the results
		try{
			Frc_Sim::set_output_table(this->ptr_database->get_database());
		}
		catch(Error msg){
			throw msg;
		}
		//mysql query with the table_model
		QSqlTableModel model(0,*this->ptr_database->get_database());
		//string for the query
		ostringstream filter;
		filter << Frc_Sim::output_table->get_column_name("SECTIONNR") << "=" << actuel_sec_num;
		filter << " and " << Frc_Sim::output_table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter << " and " << Frc_Sim::output_table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		model.setTable((Frc_Sim::output_table->table_name.name).c_str());
		model.setFilter(filter.str().c_str());
		//set the query
		Data_Base::database_request(&model);
		//check the table and query
		if(model.lastError().isValid()){//table is not found
			Error msg=set_error(0);
			msg.make_second_info((model.lastError().text().toStdString()));
			msg.make_second_info("\n");
			throw (msg);
		}
		//sort the values
		model.sort(2,Qt::AscendingOrder);
		//make the chart
		//plot erzeugen
		frc_plot= new QwtPlot(win_result);

		//groesse festsetzen
		frc_plot->resize(800,800);

		//plot beschriften
		frc_plot->setTitle("FRC-Curve");
		frc_plot->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
		// Set axis titles
		frc_plot->setAxisTitle(frc_plot->xBottom, "waterlevel [m]");
		frc_plot->setAxisTitle(frc_plot->yLeft, "probability");

		//curve für plot erzeugen
		ostringstream name_curve;
		name_curve << "Section nr " <<  actuel_sec_num ;
		frccurve = new QwtPlotCurve(name_curve.str().c_str());
		
		//style of the curve
		frccurve->setStyle(QwtPlotCurve::Lines);
		frccurve->setPen(QPen(Qt::black));

		//symbole einstellen
		QwtSymbol sym;
		sym.setStyle(QwtSymbol::Cross);
		sym.setPen(QColor(Qt::black));
		sym.setSize(5);
		frccurve->setSymbol(sym);

		//an plot andocken
		frccurve->attach(frc_plot);

		//daten für kurve erzeugen
		//the position to the distribution tables is set
		double *x_value = new double[model.rowCount()];
		double *y_value = new double[model.rowCount()];
		for(int i=0; i<model.rowCount();i++){
			x_value[i] = model.record(i).value((Frc_Sim::output_table->get_column_name("WATERLEVEL")).c_str()).toDouble();
			y_value[i] = model.record(i).value((Frc_Sim::output_table->get_column_name("PROBABILITY")).c_str()).toDouble();
		}

		//daten an curve übergeben
		frccurve->setData(x_value, y_value,model.rowCount());
		delete x_value;
		delete y_value;

		//plot zeigen
		frc_plot->setWindowTitle("Selected FRC-Curve");
		//frc_plot.show();
		this->my_layout->addWidget(frc_plot);
		//make the frc-table for one section	
		this->my_layout->addWidget(this->show_frc_table(actuel_sec_num));

		this->win_result->setLayout(this->my_layout);
		

		if(this->ptr_outputwin==NULL){
			this->win_result->show();
			QApplication::exec();
			return 0;
		}
		else {
			return this->win_result;
		}

		
	}
	catch(bad_alloc&){
		Error msg=set_error(1);
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}
}
//______________________
//private
//show a frc-table for one section->show_graph_frc
QTableView* Fpl_Data_Results::show_frc_table(const int actuel_sec_num){
	try{
		this->my_browser_one=new Database_Viewer;
		this->my_browser_one->set_parentwindow(this->win_result);
		//set the filter
		ostringstream filter;
		filter << Frc_Sim::output_table->get_column_name("SECTIONNR")<< " = " << actuel_sec_num;
		filter << " and " << Frc_Sim::output_table->get_column_name("AREA_STATE")<< " = " << this->system_id.area_state;
		filter << " and " << Frc_Sim::output_table->get_column_name("MEASURE")<< " = " << this->system_id.measure_nr;
		this->my_browser_one->set_tablename(Frc_Sim::output_table->table_name.name.c_str());
		this->my_browser_one->set_where_filter(filter.str());
		this->my_browser_one->set_sort_filter(Frc_Sim::output_table->get_column_name("WATERLEVEL"));
		this->my_browser_one->table_to_model(this->ptr_database->get_database());
		return (this->my_browser_one->model_to_view());
	}
	catch(bad_alloc&){
		Error msg=set_error(1);
		throw msg;
	}
	catch(Error msg){
		throw msg;
	}
}

//ask which section
int Fpl_Data_Results::ask_section(const string section_specifier){
	//how many section are there
	int sec_num;
	try{
		Fpl_Data_Section my_datasection;
		my_datasection.set_database(this->ptr_database);
		my_datasection.set_systemid(this->system_id);
		sec_num=my_datasection.ask_section(section_specifier);
	}
	catch(Error msg){
		throw msg;
	}

	return sec_num;
}
//build the error msg
Error Fpl_Data_Results::set_error(const int err_type){
		string place="Fpl_Data_Results::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0:
			place.append("show_graph_frc(const int actuel_sec_num)");
			reason="Table is not found; can not display the table";
			help="Check the table ";
			info << "Tablename " << Frc_Sim::output_table->table_name.name << endl;
			type=9;
			break;
		case 1://bad alloc
			place.append("show...");
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
	msg.set_msg(place,reason,help,type,fatal);
	msg.make_second_info(info.str());

return msg;
}