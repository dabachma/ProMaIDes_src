#pragma once
/**\class FplGui_FaultTree_GraphicsScene
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef FPLGUI_FAULTTREE_GRAPHICSSCENE_H
#define FPLGUI_FAULTTREE_GRAPHICSSCENE_H

//system sys
#include "Sys_GraphicsScene.h"

///
/**

*/
class FplGui_FaultTree_GraphicsScene : public Sys_GraphicsScene
{
public:
	///Default constructor
	FplGui_FaultTree_GraphicsScene(QObject *parent = 0);
	///Default destructor
	~FplGui_FaultTree_GraphicsScene(void);

	///Show the legend 
	void show_legend(const bool state);
	///Move the legend to given position, with position as center
	void move_legend_to(QPointF &pos);
	///Add a new 'or'-sign to the scene
	void add_or_sign_at(QPointF &pos);
	///Remove a 'or'-sign with given index
	void remove_or_sign(int index);
	///Add a new 'and'-sign to the scene
	void add_and_sign_at(QPointF &pos);
	///Remove a 'and'-sign with given index
	void remove_and_sign(int index);
	///
	QGraphicsItemGroup *get_main_item(void);

private:
	QGraphicsItemGroup *mainItem;
	QList<QGraphicsPixmapItem*> or_signs;
	QList<QGraphicsPixmapItem*> and_signs;
};

#endif // FPLGUI_FAULTTREE_GRAPHICSSCENE_H

