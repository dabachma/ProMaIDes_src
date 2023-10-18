#include "Fpl_Headers_Precompiled.h"
#include "FplGui_FaultTree_GraphicsScene.h"

FplGui_FaultTree_GraphicsScene::FplGui_FaultTree_GraphicsScene(QObject *parent) : Sys_GraphicsScene(parent) {
	mainItem = new QGraphicsItemGroup();
	mainItem->setZValue(10);

	QGraphicsTextItem *mainItemText = new QGraphicsTextItem(tr("failure"), mainItem, this);
	mainItemText->setFont(QFont("Arial", 18));
	//mainItemText->setTextWidth();
	QGraphicsRectItem *mainItemRect = new QGraphicsRectItem(mainItem, this);
	mainItemRect->setPen(QPen(QBrush(Qt::black), 2));
	mainItemRect->setRect(mainItemText->boundingRect());

	mainItem->addToGroup(mainItemRect);
	mainItem->addToGroup(mainItemText);
	mainItem->setPos((0 - mainItem->boundingRect().width() / 2), (0 - mainItem->boundingRect().height()));
	addItem(mainItem);

	or_signs = QList<QGraphicsPixmapItem*>();
	and_signs = QList<QGraphicsPixmapItem*>();
}

FplGui_FaultTree_GraphicsScene::~FplGui_FaultTree_GraphicsScene(void) {
	qDeleteAll(or_signs);
	qDeleteAll(and_signs);
}

//______________
//public methods
//Show the legend 
void FplGui_FaultTree_GraphicsScene::show_legend(const bool state) {

}

//Move the legend to given position, with position as center
void FplGui_FaultTree_GraphicsScene::move_legend_to(QPointF &pos) {

}

//Add a new 'or'-sign to the scene
void FplGui_FaultTree_GraphicsScene::add_or_sign_at(QPointF &pos) {
	QGraphicsPixmapItem *sign = new QGraphicsPixmapItem(QPixmap(":/or_sign"));
	sign->setZValue(10);
	sign->setToolTip(tr("boolean operator: OR"));
	sign->setPos((pos.x() - sign->boundingRect().width() / 2), (pos.y() - sign->boundingRect().height() / 2));
	addItem(sign);
	or_signs.append(sign);
}

//Remove a 'or'-sign with given index
void FplGui_FaultTree_GraphicsScene::remove_or_sign(int index) {
	if (index > 0 && index < or_signs.length()) {
		removeItem(or_signs.value(index));
		or_signs.removeAt(index);
	}
}

//Add a new 'and'-sign to the scene
void FplGui_FaultTree_GraphicsScene::add_and_sign_at(QPointF &pos) {
	QGraphicsPixmapItem *sign = new QGraphicsPixmapItem(QPixmap(":/and_sign"));
	sign->setZValue(10);
	sign->setToolTip(tr("boolean operator: AND"));
	sign->setPos((pos.x() - sign->boundingRect().width() / 2), (pos.y() - sign->boundingRect().height() / 2));
	addItem(sign);
	and_signs.append(sign);
}

//Remove a 'and'-sign with given index
void FplGui_FaultTree_GraphicsScene::remove_and_sign(int index) {
	if (index > 0 && index < and_signs.length()) {
		removeItem(and_signs.value(index));
		and_signs.removeAt(index);
	}
}

//
QGraphicsItemGroup *FplGui_FaultTree_GraphicsScene::get_main_item(void) {
	return mainItem;
}
