#pragma once

#include <eigen3/Eigen/Dense>

namespace loss
{
    class DisplayObject
    {
        public:

        private:
            DisplayObject *_parent;
            Eigen::Matrix4f _transform;

    };
}
