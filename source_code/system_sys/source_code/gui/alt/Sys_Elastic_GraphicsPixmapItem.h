/** \class Sys_Elastic_GraphicsPixmapItem
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef Sys_Elastic_GraphicsPixmapItem_H
#define Sys_Elastic_GraphicsPixmapItem_H

//qt-libs
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include <QGraphicsScene>
#include <QTimer>

//system sys
#include "Sys_Elastic_GraphicsTreeObject.h"
#include "Sys_Elastic_PixmapButtonMenu.h"
#include "Sys_Elastic_PixmapButton.h"
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

/// \ingroup sys
/**

*/
class Sys_Elastic_GraphicsPixmapItem 
	: public Sys_Elastic_GraphicsTreeObject 
{
	//qt-macro
    Q_OBJECT

public:
    ///Default constructor
    explicit Sys_Elastic_GraphicsPixmapItem(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent = 0);
    ///Default destructor
    explicit Sys_Elastic_GraphicsPixmapItem(QGraphicsScene *scene, QObject *parent = 0);
    ///Default destructor
    ~Sys_Elastic_GraphicsPixmapItem(void);
    ///Add a new pixmap to the button menu
    bool add_pixmap(const QString &labelText, const QPixmap &pixmap);
	///Overriding Qt method. 
	/**
	 * Provides a QRect which is the area that becomes redrawn
	 * by calling the update() method.
	 */
	virtual QRectF boundingRect(void) const;
	///Get the center pos of the the item
	virtual QPointF get_absolute_center(void) const;
	///Return the text of the currently checked button`s label.
	/**
	 *  \return The text of the currently checked button, an empty QString if none is checked.
	 */
	QString get_active_button_text(void) const;
	///Return the menu index of the currently checked button.
	/**
	 * \return The menu index of the currently checked button, -1 if none is checked.
	 */
	int get_active_button_index(void) const;
	///
	Qt::AspectRatioMode get_aspect_ratio_mode(void) const;
	///Get the button`s menu index by it`s label text.
	int get_button_index(const QString &labelText) const;
	///Get the point where to start the edge connecting the items.
	virtual QPointF get_edge_docking_point(const qreal angle);
	///Get the item`s height.
	qreal get_height(void) const;
	///Get the item`s size.
	QSizeF get_size(void) const;
	///Get the item`s width.
	qreal get_width(void) const;
    ///Paint the item.
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    ///
    bool remove_pixmap(const QString &labelText);
    ///
    bool remove_pixmap(const QPixmap &pixmap);
	///
	void set_aspect_ratio_mode(Qt::AspectRatioMode mode);
	///
	void set_height(const qreal height);
	///Set a pixmap that is used to display inactive state on multi pixmap items.
	void set_null_pixmap(const QPixmap &pixmap);
	///
	virtual void set_pos(const QPointF &pos);
	///
	virtual void set_pos(const qreal x, const qreal y);
	///
    void set_size(const QSizeF &size);
    ///
    void set_size(const qreal width, const qreal height);
	///
	void set_width(const qreal width);

public slots:
	///React on menu clicks here.
	void on_menu_clicked(Sys_Elastic_PixmapButton *button, const bool checked);
	///Set the currently active button by index.
	void set_active_button(const int index);
	///Set the currently active button by text.
	void set_active_button(const QString &labelText);
	///Set the item disabled.
	/**
	 * If the item is disabled, the menu will not 
	 * occur when you move the mouse over it. So
	 * you can not change the item`s state via the gui.
	 */
	virtual void set_disabled(void);
    ///Set the item enabled.
	/**
	 * \see set_disabled(void)
	 */
	virtual void set_enabled(const bool enabled = true);

signals:
	///
	void position_changed(const QPointF &pos);

protected:
    //Event handling
	///Handle item movements
	QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    ///Handle hover enter events. Highlight the item
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    ///Handle hover leave events. Turn of item highlighting
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    ///Handle mouse press events.
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    ///Handle mouse release events.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    ///Handle mouse wheel events.
    void wheelEvent(QGraphicsSceneWheelEvent *event);

private:
	///
	QSizeF size;
	///
	QPixmap nullPixmap;
	///
	QPixmap pixmap;
	///
	Qt::TransformationMode transformationMode;
	///
	QPointF offset;
	///
	Sys_Elastic_PixmapButtonMenu *buttonMenu;
	///
	bool in;
	///
	QColor inColor;
	///
	qreal inFrameWidth;
	///
	bool disabledMenuRise;
	///
	Qt::AspectRatioMode aspectMode;
};

#endif // Sys_Elastic_GraphicsPixmapItem_H
