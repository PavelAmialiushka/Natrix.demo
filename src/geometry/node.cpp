#include "node.h"
#include "worldsystem.h"
#include "scene.h"

namespace geometry
{

Node::Node()
    : joint_type_(0)
    , weld_position_(0)
{
}

void Node::setPObject(PObject object)
{
    object_ = object;
}

PObject Node::object() const
{
    Q_ASSERT(this);
    auto object = PObject(object_);
    Q_ASSERT(object);
    return object;
}

point3d Node::globalPoint() const
{
    return global_point_;
}
point2d Node::localPoint() const
{
    return local_point_;
}
point3d Node::direction() const
{
    return dir_;
}

int Node::jointType() const
{
    return joint_type_;
}

double Node::weldPosition() const
{
    return weld_position_;
}

void Node::setWeldPosition(double d)
{
    weld_position_ = d;
}

void Node::setJointType(int t)
{
    joint_type_ = t;
}

void Node::setPoint(PWorldSystem sys, point3d const &pt, point3d const& dr)
{
    //Q_ASSERT( dr.isAxeParallel() );
    global_point_ = pt;
    local_point_ = sys->toUser(pt);
    dir_ = dr;
}

}
