/** \class Sys_Elastic_GraphicsView
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010
*/
#ifndef SYS_ELASTIC_GRAPHICSVIEW_H
#define SYS_ELASTIC_GRAPHICSVIEW_H

//qt-libs
#include <QGraphicsView>

//system sys
#include "Sys_Elastic_GraphicsScene.h"
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

/// \ingroup sys
/**

*/
class Sys_Elastic_GraphicsView 
	: public QGraphicsView
{
	//qt-macro
    Q_OBJECT

public:
	///
	explicit Sys_Elastic_GraphicsView(Sys_Elastic_GraphicsScene *scene, QWidget *parent = 0);
	///Default constructor
    explicit Sys_Elastic_GraphicsView(QWidget *parent = 0);
	///Default destructor
	~Sys_Elastic_GraphicsView(void);
};

#endif // SYS_ELASTIC_GRAPHICSVIEW_H
