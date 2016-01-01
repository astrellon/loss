#include "display_object.h"

namespace loss
{
    DisplayObject::DisplayObject(DisplayObject *parent) :
        _parent(parent)
    {

    }

    Eigen::Projective3f &DisplayObject::transform()
    {
        return _transform;
    }

    void DisplayObject::parent(DisplayObject *parent)
    {
        _parent = parent;
    }
    DisplayObject *DisplayObject::parent() const
    {
        return _parent;
    }
}
