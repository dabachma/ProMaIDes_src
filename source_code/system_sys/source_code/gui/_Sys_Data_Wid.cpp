#include "Sys_Headers_Precompiled.h"
//#include "_Sys_Data_Wid.h"

//Default constructor
_Sys_Data_Wid::_Sys_Data_Wid(QWidget *parent): QWidget(parent){


	this->id_item=NULL;
	this->area2print=NULL;
	this->scroll_area=NULL;

	this->act_print=new QAction("Print...", &this->context_menu);
	this->context_menu.addAction(this->act_print);
	QObject::connect(this->act_print, SIGNAL(triggered()), this, SLOT(print()));
	
	this->act_save2graphic=new QAction("Export as image...", &this->context_menu);
	this->context_menu.addAction(this->act_save2graphic);
	QObject::connect(this->act_save2graphic, SIGNAL(triggered()), this, SLOT(save_as_graphic()));
	

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	QObject::connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(context_menu_request(QPoint)));

}
//Default destructor
_Sys_Data_Wid::~_Sys_Data_Wid(void){

}
//_______
//public
///Set the identifier string to restore the widgets
void _Sys_Data_Wid::set_identifier(const string id, QTreeWidgetItem *item){
	this->setWindowTitle(id.c_str());
	this->id_item=item;
}
//Set the QTreeWidgetItem pointer
void _Sys_Data_Wid::set_treeItem(QTreeWidgetItem *item){
	this->id_item = item;
}
//Get the pointer of the corresponding tree widget item
QTreeWidgetItem * _Sys_Data_Wid::get_ptr_treewidget_item(void){
	return this->id_item;
}
//Get the get last string after "/" as visible identifier (static);
string _Sys_Data_Wid::get_last_string_string_id(const string buffer){
	string buff=buffer;
	int pos1=0;
	string type;

	buff=buff.substr(0, buff.length()-1);

	ostringstream buff_stream;
	buff_stream<<"/"<<risk_label::risk_catchment<<"/";
	pos1=buff.find(buff_stream.str());
	if(pos1>0){
		type=risk_label::risk_catchment;
		type.append("/");
	}
	buff_stream.str("");
	buff_stream<<"/"<<risk_label::risk_scenario<<"/";
	pos1=buff.find(buff_stream.str());
	if(pos1>0){
		type=risk_label::risk_scenario;
		type.append("/");
	}
	buff_stream.str("");
	buff_stream<<"/"<<risk_label::risk_nobreak<<"/";
	pos1=buff.find(buff_stream.str());
	if(pos1>0){
		type=risk_label::risk_nobreak;
		type.append("/");
	}



	pos1=buff.find_last_of("/");
	if(pos1<0){
		return buff;
	}
	
	buff=buff.substr(pos1+1, buff.length());
	type.append(buff);
	
	return type;
}
//_______
//protected



//_______
//protected slots
//Print the widget
void _Sys_Data_Wid::print(void){

		QPrinter *pdfPrinter = new QPrinter;
		QPrintDialog my_dia(pdfPrinter);
		QRect total_rec;
		QRect printer_rec;
		QRect toprint;
		int number_pages=0;
		if(my_dia.exec()==1){
			total_rec=this->area2print->frameGeometry();
			toprint=total_rec;
			printer_rec=pdfPrinter->pageRect();
			toprint.setHeight(printer_rec.height());
			number_pages=(int)(total_rec.height()/printer_rec.height());
			number_pages++;

			QPainter *pdfPainter = new QPainter;
			pdfPainter->begin(pdfPrinter);
			for(int i=0; i< number_pages; i++){
				if(i>0){
					int top=toprint.top();
					top=top+printer_rec.height();
					toprint.moveTop(top);
				}
				QRegion print_region(toprint);
				this->area2print->render(pdfPainter, QPoint(), print_region, RenderFlags( DrawWindowBackground | DrawChildren  ));
				if(number_pages>1 &&i < number_pages-1){
					pdfPrinter->newPage();
				}
			}
			pdfPainter->end();
			delete pdfPainter;
		}
		delete pdfPrinter;


	}

//Save as graphic
void _Sys_Data_Wid::save_as_graphic(void){

	if(this->area2print==NULL){
		return;
	}

    QString fileName = QFileDialog::getSaveFileName(this, "Export as graphic", Sys_Project::get_current_path().c_str(), tr("JPEG (*.jpg);;Portable Network Graphics (*.png);;Windows bitmap (*.bmp)"));
	if (fileName.isEmpty()==false) {
		QDir my_dir;
		string path;
		path=my_dir.absoluteFilePath(fileName).toStdString();
		path=functions::get_file_path(path);
		Sys_Project::set_current_path(path);

		
		QImage my_image(this->area2print->size(), QImage::Format_RGB32);
		this->area2print->render(&my_image, QPoint(), QRegion(), RenderFlags( DrawWindowBackground | DrawChildren  ));



		QImageWriter writer;
		writer.setQuality(80);
		writer.setFileName(fileName);
		if(fileName.endsWith(".png")){
			writer.setFormat("png");
		}
		else if(fileName.endsWith(".jpg")){
            writer.setFormat("jpg");
		}
        else if(fileName.endsWith(".bmp")){
            writer.setFormat("bmp");
        }
		writer.write(my_image);

	}

}
//Handle a context menu request (Right Mouse Click)
void _Sys_Data_Wid::context_menu_request(QPoint point){
	QPoint trans;
	trans = this->mapToGlobal(point);
	this->context_menu.popup(trans);
	
}
