//#include "Sys_Elastic_GraphicsEdge.h"
#include "Sys_Headers_Precompiled.h"
#include "Sys_Elastic_GraphicsTreeObject.h"
#include <math.h>

qreal Sys_Elastic_GraphicsEdge::lineWidth = 1.0;

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

//Default constructor
Sys_Elastic_GraphicsEdge::Sys_Elastic_GraphicsEdge(Sys_Elastic_GraphicsTreeObject *root, Sys_Elastic_GraphicsTreeObject *destination)
    : Sys_Elastic_GraphicsObject(root)
{
	this->state = true;
	this->arrowSize = 4.0;
	this->set_arrows_visible(true);

	this->setAcceptedMouseButtons(0);

	this->root = root;
	this->dest = destination;

	this->root->add_edge(this);
	this->dest->add_edge(this);

	this->rootPoint = root->pos();
	this->destPoint = dest->pos();

	ADD_MEM(Sys_Elastic_GraphicsEdge, _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Elastic_GraphicsEdge::~Sys_Elastic_GraphicsEdge(void) 
{
	MIN_MEM(Sys_Elastic_GraphicsEdge, _sys_system_modules::SYS_SYS);
}
//
void Sys_Elastic_GraphicsEdge::adjust(void) 
{
	if (!root || !dest)
        return;

    QLineF line = get_line_between(root, dest);
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(30.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        rootPoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        rootPoint = destPoint = line.p1();
    }
}
//
bool Sys_Elastic_GraphicsEdge::are_arrows_visible(void) const {
	return this->visibleArrows;
}
//
QRectF Sys_Elastic_GraphicsEdge::boundingRect(void) const {
	if (!this->root || !this->dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + this->arrowSize) / 2.0;

    return QRectF(this->rootPoint, QSizeF(this->destPoint.x() - this->rootPoint.x(),
                                      this->destPoint.y() - this->rootPoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//
qreal Sys_Elastic_GraphicsEdge::get_angle(void) const {
	qreal dx = dest->get_absolute_center().x() - root->get_absolute_center().x();
	qreal dy = dest->get_absolute_center().y() - root->get_absolute_center().y();

	return atan(dy / dx);
}
//
qreal Sys_Elastic_GraphicsEdge::get_line_width(void) {
	return Sys_Elastic_GraphicsEdge::lineWidth;
}
//
bool Sys_Elastic_GraphicsEdge::is_visible(void) const {
    return this->isVisible();
}
//
void Sys_Elastic_GraphicsEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	Q_UNUSED(option);
	Q_UNUSED(widget);

	if (!this->root || !this->dest)
        return;

    QLineF line(this->rootPoint, this->destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the line itself
	if (this->state)
		painter->setPen(QPen(Qt::black, this->lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	else
		painter->setPen(QPen(Qt::lightGray, this->lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    painter->drawLine(line);

    // Draw the arrows
	if (this->are_arrows_visible()) {
		double angle = ::acos(line.dx() / line.length());
		if (line.dy() >= 0)
			angle = TwoPi - angle;

		QPointF rootArrowP1 = rootPoint + QPointF(sin(angle + Pi / 3) * arrowSize,
													cos(angle + Pi / 3) * arrowSize);
		QPointF rootArrowP2 = rootPoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
													 cos(angle + Pi - Pi / 3) * arrowSize);
		QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
												cos(angle - Pi / 3) * arrowSize);
		QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
												cos(angle - Pi + Pi / 3) * arrowSize);
		if (state)
			painter->setBrush(Qt::black);
		else
			painter->setBrush(Qt::lightGray);

		painter->drawPolygon(QPolygonF() << line.p1() << rootArrowP1 << rootArrowP2);
		painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
	}
}
//
void Sys_Elastic_GraphicsEdge::set_arrows_visible(const bool visible) {
	this->visibleArrows = visible;
}
//
void Sys_Elastic_GraphicsEdge::set_line_width(const qreal width) {
	Sys_Elastic_GraphicsEdge::lineWidth = width;
}
//
void Sys_Elastic_GraphicsEdge::set_visible(const bool visible) {
    this->setVisible(visible);
}
//____________
//public slots
//
void Sys_Elastic_GraphicsEdge::on_tree_object_state_changed(const bool state) {
	if (state == this->state)
		return;

	this->state = state;
	this->update();

}
//_______________
//private methods
//
QLineF Sys_Elastic_GraphicsEdge::get_line_between(Sys_Elastic_GraphicsTreeObject *root, Sys_Elastic_GraphicsTreeObject *dest) {
	qreal offset = -5.0;

	qreal dx = dest->get_absolute_center().x() - root->get_absolute_center().x();
	qreal dy = dest->get_absolute_center().y() - root->get_absolute_center().y();

	if (dx == 0.) {

		if (dy < 0.) {
			return QLineF(root->get_absolute_center().x(), root->get_absolute_center().y() - (root->get_height() / 2 + offset),
				          dest->get_absolute_center().x(), dest->get_absolute_center().y() + (dest->get_height() / 2 + offset));
		}

		return QLineF(root->get_absolute_center().x(), root->get_absolute_center().y() + (root->get_height() / 2 + offset),
				      dest->get_absolute_center().x(), dest->get_absolute_center().y() - (dest->get_height() / 2 + offset));
	}

	else if (dy == 0.) {
	
		if (dx < 0.) {
			return QLineF(root->get_absolute_center().x() - (root->get_width() / 2 + offset), root->get_absolute_center().y(),
					      dest->get_absolute_center().x() + (dest->get_width() / 2 + offset), dest->get_absolute_center().y());
		}

		return QLineF(root->get_absolute_center().x() + (root->get_width() / 2 + offset), root->get_absolute_center().y(),
					  dest->get_absolute_center().x() - (dest->get_width() / 2 + offset), dest->get_absolute_center().y());
	}

	double angle = atan(dy / dx);
	double m = dy / dx;

	qreal x1, y1, x2, y2;
	x1 = y1 = x2 = y2 = 0.0;

	if (angle >= - Pi / 4. && angle <= Pi / 4.) {

		if (dx > 0.) {
			x1 = root->get_absolute_center().x() + (root->get_width() / 2 + offset);
			y1 = root->get_absolute_center().y() + m * (root->get_width() / 2 + offset);

			x2 = dest->get_absolute_center().x() - (dest->get_width() / 2 + offset);
			y2 = dest->get_absolute_center().y() - m * (dest->get_width() / 2 + offset);
		}
	
		else {
			x1 = root->get_absolute_center().x() - (root->get_width() / 2 + offset);
			y1 = root->get_absolute_center().y() - m * (root->get_width() / 2 + offset);

			x2 = dest->get_absolute_center().x() + (dest->get_width() / 2 + offset);
			y2 = dest->get_absolute_center().y() + m * (dest->get_width() / 2 + offset);
		}
	}

	else if (angle < - Pi / 4. || angle > Pi / 4.) {

		if (dy > 0.) {
			x1 = root->get_absolute_center().x() + (root->get_height() / 2 + offset) / m;
			y1 = root->get_absolute_center().y() + (root->get_height() / 2 + offset);

			x2 = dest->get_absolute_center().x() - (dest->get_height() / 2 + offset) / m;
			y2 = dest->get_absolute_center().y() - (dest->get_height() / 2 + offset);
		}

		else {
			x1 = root->get_absolute_center().x() - (root->get_height() / 2 + offset) / m;
			y1 = root->get_absolute_center().y() - (root->get_height() / 2 + offset);

			x2 = dest->get_absolute_center().x() + (dest->get_height() / 2 + offset) / m;
			y2 = dest->get_absolute_center().y() + (dest->get_height() / 2 + offset);
		}
	}

	return QLineF(x1, y1, x2, y2);
}
