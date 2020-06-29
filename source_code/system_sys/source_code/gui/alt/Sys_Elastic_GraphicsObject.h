#ifndef SYS_ELASTIC_GRAPHICSOBJECT_H
#define SYS_ELASTIC_GRAPHICSOBJECT_H

#include <QGraphicsItem>
#include <QObject>

///
/**
 * A GraphicsObject is a connection between a QGraphicsItem and a QObject. So it has
 * the ability to define signals and slots.
 */
class Sys_Elastic_GraphicsObject : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
	///
    /**
     *
     */
    explicit Sys_Elastic_GraphicsObject(QGraphicsItem *parentItem, QObject *parent);
    ///
    /**
     *
     */
    explicit Sys_Elastic_GraphicsObject(QObject *parent = 0);
    ///
    /**
     *
     */
    ~Sys_Elastic_GraphicsObject(void);
    ///
    /**
     *
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
	///
	/**
	 *
	 */
	virtual QRectF boundingRect(void) const = 0;
};

#endif // SYS_ELASTIC_GRAPHICSOBJECT_H
