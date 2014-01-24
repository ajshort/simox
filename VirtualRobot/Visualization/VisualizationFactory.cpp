#include "VisualizationFactory.h"

using Eigen::Matrix4f;
using Eigen::Vector3f;

namespace VirtualRobot
{

VisualizationNodePtr VisualizationFactory::createLine(const Vector3f &from, const Vector3f &to,
                                                      float width, float r, float g, float b)
{
    Matrix4f fromPose = Matrix4f::Identity();
    Matrix4f toPose = Matrix4f::Identity();

    fromPose.block<3, 1>(0, 3) = from;
    toPose.block<3, 1>(0, 3) = to;

    return createLine(fromPose, toPose, width, r, g, b);
}

}
