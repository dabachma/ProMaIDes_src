/** \class Sys_Elastic_GraphicsScene
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010
*/
#ifndef SYS_ELASTIC_GRAPHICSSCENE_H
#define SYS_ELASTIC_GRAPHICSSCENE_H

//qt-libs
#include <QGraphicsScene>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

/// \ingroup sys
/**

*/
class Sys_Elastic_GraphicsScene 
	: public QGraphicsScene
{
	//qt-macro
    Q_OBJECT

public:
	///
	explicit Sys_Elastic_GraphicsScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
	///
	explicit Sys_Elastic_GraphicsScene(const QRectF &sceneRect, QObject *parent = 0);
	///Default constructor
    explicit Sys_Elastic_GraphicsScene(QObject *parent = 0);
	///Default destructor
	~Sys_Elastic_GraphicsScene(void);
};

#endif // SYS_ELASTIC_GRAPHICSSCENE_H
