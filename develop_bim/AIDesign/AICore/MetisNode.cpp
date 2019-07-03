#include "../Point2f.h"
#include "MetisNode.h"

MetisNode::MetisNode(const Box2D & node_box)
	:Weight(0.0f),bInside(false),bIndoor(false),bInfloorbox(false)
{
	const Point2f extent = node_box.GetExtent();
	const Point2f x_extent = Point2f(extent.x, 0.f);
	const Point2f y_entent = Point2f(0.f, extent.y);
	/************************************************************************
	*  ___________max
	* |     |     |
	* |     |     |
	* |-----c-----|
	* |     |     |
	* min___|_____|
	*
	************************************************************************/

	center_point = node_box.GetCenter();
	top_left_point = center_point + x_extent - y_entent;
	top_right_point = node_box.max_point;
	bottom_right_point = center_point - x_extent + y_entent;
	bottom_left_point = node_box.min_point;
}
