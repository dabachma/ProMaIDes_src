#include "Fpl_Headers_Precompiled.h"
//#include "FplGui_Frc_Values_Dia.h"
//#include "doublespinboxdelegate.h"

//Default constructor
FplGui_Frc_Values_Dia::FplGui_Frc_Values_Dia(QWidget *parent): QDialog(parent), prob_max(1.0), prob_min(0.0), waterlevel_min(0.0) {

	ui.setupUi(this);
	Sys_DoubleSpinBox_Delegate *column_h;
	Sys_DoubleSpinBox_Delegate *column_prob;
	column_h=new Sys_DoubleSpinBox_Delegate(this);
	column_h->set_spin_box_values(1e30, 0.0, 0.5, 2 );
	column_prob=new Sys_DoubleSpinBox_Delegate(this); 
	column_prob->set_spin_box_values(1.0, 0.0, 0.1, 5);

	

	this->ui.tableWidget->setColumnCount(2);
	this->ui.tableWidget->setItemDelegateForColumn(0, column_h);
	this->ui.tableWidget->setItemDelegateForColumn(1, column_prob);

    this->ui.tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    this->ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	
    this->ui.tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Waterlevel [m]") << tr("Failure probability [-]"));
	this->ui.tableWidget->setAlternatingRowColors(true);

	// Initialize the textfeld
	this->frc_infos = "";
	this->show_frc_infos(this->frc_infos);

	// Initialize the curve
	this->frc_curve=NULL;
	
	// Connect Signals and Slots
	QObject::connect(ui.addPointButton, SIGNAL(clicked()), this, SLOT(addPointButton_clicked()));
	QObject::connect(ui.wl_endSpinBox, SIGNAL(valueChanged(double)), this, SLOT(checkCells(double)));
	QObject::connect(ui.delPointButton, SIGNAL(clicked()),this, SLOT(delPointButton_clicked()));
	QObject::connect(ui.PointSpinBox, SIGNAL(valueChanged(int)), this, SLOT(PointSpinBox_valueChanged(int)));
	QObject::connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui.tableWidget, SIGNAL(cellChanged(int , int )), this, SLOT(checkCells(int , int )));

	// Initialize the fixed values
	this->waterlevel_max=1.0;


	this->number_rows = 0;
	this->number_columns = 0;

	// Initialize the plot
	this->set_plot();
	//set the spinbox value 
	this->ui.wl_endSpinBox->setValue(this->waterlevel_max);

	//set the ok button enable
	this->ui.okButton->setEnabled(false);

	Sys_Memory_Count::self()->add_mem(sizeof(FplGui_Frc_Values_Dia), _sys_system_modules::FPL_SYS);
}
//Default destructor
FplGui_Frc_Values_Dia::~FplGui_Frc_Values_Dia(void){
	// Curve
	this->delete_plot_curve();
	this->x.clear();
	this->y.clear();
	Sys_Memory_Count::self()->minus_mem(sizeof(FplGui_Frc_Values_Dia), _sys_system_modules::FPL_SYS);
}
//_________________
//public
//Return the number of points, which are set
int FplGui_Frc_Values_Dia::get_number_points(void){
	return this->number_rows+2;
}
//Set data of a given fragility curve
void FplGui_Frc_Values_Dia::set_data(const double water_h, const double prob){
	this->addPointButton_clicked();
	int row=ui.tableWidget->currentRow();
	QTableWidgetItem *my_item;
	my_item=new QTableWidgetItem;
	my_item->setData(Qt::EditRole,prob);
	this->ui.tableWidget->setItem(row,1,my_item);

	my_item=new QTableWidgetItem;
	my_item->setData(Qt::EditRole,water_h);
	this->ui.tableWidget->setItem(row,0,my_item);

}
//Set the maximum waterlevel
void FplGui_Frc_Values_Dia::set_maximum_waterlevel(const double max_h){
	this->waterlevel_max=max_h;
	ui.wl_endSpinBox->setValue(this->waterlevel_max);
}
//Return the waterlevel for the given index
double FplGui_Frc_Values_Dia::get_water_level(const int index){
	return this->x.at(index);
}
//Return the failure orobability for the given index
double FplGui_Frc_Values_Dia::get_fail_prob(const int index){
	return this->y.at(index);
}
//Show fpl-section informations to the dialog header
void FplGui_Frc_Values_Dia::show_frc_infos(const QString txt){
	ui.frc_infoTextEdit->setPlainText(txt);
}
//Start the dialog and return the user decision (ok:=true, cancel:=false)
bool FplGui_Frc_Values_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		this->okButton_clicked();
		return true;
	}
}
//Return the monte-carlo annuality of failure probability
double FplGui_Frc_Values_Dia::get_annuality_failure(void){
	return this->ui.doubleSpinBox_mc_result->value();
}
//Set the annaulity of failure
void FplGui_Frc_Values_Dia::set_annuality_failure(const double annu){
	this->ui.doubleSpinBox_mc_result->setValue(annu);
}
//_____________
//private slots
//Add a new row to the table
void FplGui_Frc_Values_Dia::addPointButton_clicked(void){
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);
	ui.PointSpinBox->setValue(ui.tableWidget->rowCount());
	ui.tableWidget->setCurrentCell(row, 1);
}
//Remove the active row in the table
void FplGui_Frc_Values_Dia::delPointButton_clicked(void){ 
	int row = ui.tableWidget->rowCount();
	if(row > 0 && warningMessage()){
		ui.tableWidget->removeRow(ui.tableWidget->rowCount()-1);
		ui.PointSpinBox->setValue(ui.tableWidget->rowCount());
		ui.tableWidget->setCurrentCell(row, 1);
		this->checkCells(0,0);
	}
}
//Add and delete rows with using the spinbox
void FplGui_Frc_Values_Dia::PointSpinBox_valueChanged(int val){
	ui.tableWidget->setRowCount(val);
	this->checkCells(0,0);
}
//Check all cells to correctness
void FplGui_Frc_Values_Dia::checkCells(int, int){
	//Initialize values
	double x = 0.0;
	double y = 0.0;

	bool one_failure_flag = false;
	this->number_rows = ui.tableWidget->rowCount();
	this->number_columns = ui.tableWidget->columnCount();
	this->waterlevel_max = ui.wl_endSpinBox->value();

	// Disable the okButton
	this->ui.okButton->setEnabled(false);

	// Check if the item is alloacted
	for (int i = 0; i < this->number_rows; i++){	
		for (int j = 0; j < this->number_columns; j++){
			// No point allocated
			if(ui.tableWidget->item(i, j) == NULL){
				// Disable the okButton
				this->ui.okButton->setEnabled(false);
				return;
			}
			// If there is no waterlevel value, return
			if(ui.tableWidget->item(i, j)->text().isEmpty()){
				// Disable the okButton
				this->ui.okButton->setEnabled(false);
				ui.tableWidget->item(ui.tableWidget->currentRow(), j)->setBackground(Qt::yellow);
				ui.tableWidget->item(ui.tableWidget->currentRow(), j)->setToolTip("Set values");
				break;
			}
		}
	}
	
	for (int i = 0; i < this->number_rows; i++){
		// Check waterlevel
		y = ui.tableWidget->item(i,0)->text().toDouble();

		if(y < this->waterlevel_min){
			 ui.tableWidget->item(i, 0)->setToolTip("Waterlevel can not be below the min. given waterlevel");
			 ui.tableWidget->item(i, 0)->setBackground(Qt::red);
			 one_failure_flag=true;
		}
		else if(y > this->waterlevel_max){
			 ui.tableWidget->item(i, 0)->setToolTip("Waterlevel can not be above the max. given waterlevel");
			 ui.tableWidget->item(i, 0)->setBackground(Qt::red);
			 one_failure_flag=true;
		}
		else{
			// Reset the color and tool tip except for the fixed values
			ui.tableWidget->item(i, 0)->setBackground(Qt::white);
			ui.tableWidget->item(i, 0)->setToolTip("");
		}
		// Check probabilities
		x = ui.tableWidget->item(i, 1)->text().toDouble();
		if(x > 1){
			// Check the probability column to correctness	
			ui.tableWidget->item(i, 1)->setBackground(Qt::red);
			ui.tableWidget->item(i, 1)->setToolTip("Probabilities have to be between 0 and 1!");
			one_failure_flag=true;
		}
		else if(x < this->prob_min){
			 ui.tableWidget->item(i, 1)->setToolTip("Failure probability can not be below the min. given waterlevel");
			 ui.tableWidget->item(i, 1)->setBackground(Qt::red);
			 one_failure_flag=true;
		}
		else if(x > this->prob_max){
			 ui.tableWidget->item(i, 1)->setToolTip("Failure probability can not be below the min. given waterlevel");
			 ui.tableWidget->item(i, 1)->setBackground(Qt::red);
			 one_failure_flag=true;
		}
		else{
			//reset the color and tool tip except for the fixed values
			ui.tableWidget->item(i, 1)->setBackground(Qt::white);
			ui.tableWidget->item(i, 1)->setToolTip("");
		}
	}
	if(one_failure_flag == true){
		return;
	}
	// Set vectors
	this->get_values();
	// Plot it new
	this->set_fragility2plotcurve();
	// Enable the okButton
	this->ui.okButton->setEnabled(true);
	return ;

}

///Check cell to correctness
 void FplGui_Frc_Values_Dia::checkCells(double){
	this->checkCells(0,0);
}
//Check if the curve are ready to be plotted and plot them
void FplGui_Frc_Values_Dia::okButton_clicked(void){
	for (int row = 0; row < ui.tableWidget->rowCount(); row++){
		for (int column = 0; column < ui.tableWidget->columnCount(); column++){
			checkCells(row, column);
		}
	}
	// Plot it new
	this->set_fragility2plotcurve();
}
//______________
//private
//Confirm the deleting of a row
bool FplGui_Frc_Values_Dia::warningMessage(void){	
	Sys_Diverse_Text_Dia my_dia(this);
	ostringstream info;
	info <<"Do you want to delete this point of the fragility curve?";
	my_dia.set_dialog_question(info.str());

	return my_dia.start_dialog();
}
//Append the fixed and in the table stored values in the arrays
void FplGui_Frc_Values_Dia::get_values(void){
	// Reset the values
	this->x.clear();
	this->y.clear();
	
	// Append the first fixed values at the begin of the list
	x.push_front(this->waterlevel_min);
	y.push_front(this->prob_min);

	// Append the values stored in every rows
	for(int i = 0; i < ui.tableWidget->rowCount(); i++){
		QTableWidgetItem* item0 = ui.tableWidget->item(i,0);
		QTableWidgetItem* item1 = ui.tableWidget->item(i,1);	

		x.push_back(item0->text().toDouble());
		y.push_back(item1->text().toDouble());
	}
	
	// Append the last fixed values at the end of the list
	x.push_back(this->waterlevel_max);
	y.push_back(this->prob_max);
	ui.tableWidget->sortItems(0, Qt::AscendingOrder);
}
//Set the plot curve for the fragility curve
void FplGui_Frc_Values_Dia::set_fragility2plotcurve(void){
	this->allocate_plot_curve();
	// Set layout for the Plot
	// Pen
	this->frc_curve->setPen(QPen(Qt::darkBlue));
	// Style
	this->frc_curve->setStyle(QwtPlotCurve::Lines);
	// Symbol
	const QColor &c = Qt::black;
    //this->frc_curve->setSymbol(QwtSymbol(QwtSymbol::Ellipse, QBrush(c), QPen(c), QSize(5, 5)) );
	// Attach it to the plot
	this->frc_curve->attach(ui.qwtPlot);

	// Set the data
    this->frc_curve->setSamples(this->x, this->y);
	// Reset scale
	this->ui.qwtPlot->setAxisScale(ui.qwtPlot->xBottom, 0, this->waterlevel_max, (this->waterlevel_max/10.0));	
	// Plot it
	this->ui.qwtPlot->replot();
}
//Allocate the fragility curve for ploting
void FplGui_Frc_Values_Dia::allocate_plot_curve(void){
	this->delete_plot_curve();
	try{
		this->frc_curve=new QwtPlotCurve("Curve 1");
	}
	catch(bad_alloc &t){
		Error msg=set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());	
		throw msg;
	}
}
//Delete the plot curve for the fragility curve
void FplGui_Frc_Values_Dia::delete_plot_curve(void){
	if(this->frc_curve!=NULL){
		this->frc_curve->detach();
		delete this->frc_curve;
		this->frc_curve=NULL;
	}
}
//Set the plot parameters
void FplGui_Frc_Values_Dia::set_plot(void){
	// Assign a title to the graph
	this->ui.qwtPlot->setTitle("FRC Curve");

	// Set Background color
	this->ui.qwtPlot->setCanvasBackground(Qt::white);

	// Set axis
	this->ui.qwtPlot->setAxisTitle(ui.qwtPlot->xBottom, "Waterlevel [m]");
	this->ui.qwtPlot->setAxisTitle(ui.qwtPlot->yLeft, "Failure probability [-]");

	// Set axis scales
	this->ui.qwtPlot->setAxisScale(ui.qwtPlot->xBottom, 0, this->waterlevel_max, (this->waterlevel_max/10.0));
	this->ui.qwtPlot->setAxisScale(ui.qwtPlot->yLeft, 0, 1, 0.1);

	// Grid 
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::gray, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this->ui.qwtPlot);
}
//Set error(s)
Error FplGui_Frc_Values_Dia::set_error(const int err_type){
		string place="FplGui_Frc_Values_Dia::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://bad alloc
			place.append("allocate_plot_curve(void)");
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
