#pragma once

#include <eigen3/Eigen/Geometry>

namespace loss
{
    class DisplayObject
    {
        public:
            DisplayObject(DisplayObject *parent);

            Eigen::Projective3f &transform();

            void parent(DisplayObject *parent);
            DisplayObject *parent() const;

        private:
            DisplayObject *_parent;
            Eigen::Projective3f _transform;

    };
}
