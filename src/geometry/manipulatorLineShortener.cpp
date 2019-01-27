#include "manipulatorLineShortener.h"

#include "global.h"
#include "moveProcedures.h"
#include "nodepointadorner.h"
#include "manipulator.h"
#include "line.h"
#include "neighbourhood.h"

namespace geometry
{


QString LineShortener::helperText() const
{
    return trUtf8("<b>Укоротить линию:</b> укажите линию, которую нужно укоротить")
            + helperTab
            + trUtf8("<b>CTRL+click</b> чтобы удлинить линию");

}

LineShortener::LineShortener(Manipulator *m)
    : ManipulatorTool(m)
{
}

PManipulatorTool LineShortener::create(Manipulator *m)
{
    auto p = PManipulatorTool(new LineShortener(m));
    return p;
}

void LineShortener::do_click(point2d, PNeighbourhood nei)
{
    double size = lineMinimumSize;
    foreach(PObject object, nei->closestObjects)
    {
        if (object.dynamicCast<Line>())
        {
            point3d p1 = object->globalPoint(0);
            point3d p2 = object->globalPoint(1);

            ObjectInfo info = nei->objectInfo[object];
            point3d start = info.closestPoint;

            bool extendMode =
                (manipulator_->getMode(ControlKey)) == 0;

            double lenThird = p1.distance(p2) / 3;
            bool firstThird = p1.distance(start) < lenThird;
            bool lastThird = p2.distance(start) < lenThird;

            if (p1.distance(p2) < lineMinimumSize*2)
                firstThird = lastThird = false;

            point3d d1, d2;
            if (firstThird)
            {
                d2 = p2; d1 = p1.polar_to(p2, extendMode ? size : -size);
                if (d1.distance(d2) < lineMinimumSize)
                {
                    d1 = p1;
                    d2 = d1.polar_to(p2, lineMinimumSize);
                }

            } else if (lastThird)
            {
                d1 = p1; d2 = p2.polar_to(p1, extendMode ? size : -size);
                if (d1.distance(d2) < lineMinimumSize)
                {
                    d2 = p2;
                    d1 = p2.polar_to(p1, lineMinimumSize);
                }
            } else
            {
                d1 = p1.polar_to(p2, extendMode ? size/2 : -size/2);
                d2 = p2.polar_to(p1, extendMode ? size/2 : -size/2);
                if (d1.distance(d2) < lineMinimumSize)
                {
                    d1 = ((p1+p2)/2).polar_to(p1, lineMinimumSize/2);
                    d2 = d1.polar_to(p2, lineMinimumSize);
                }
            }

            d1 -= p1;
            d2 -= p2;

            MoveData data;
            MoveParameters params{ObjectRestrictionType::Rigid};
            params.nodeMoves[object->nodeAt(0)] = NodeMovement{d1};
            params.nodeMoves[object->nodeAt(1)] = NodeMovement{d2};
            params.objectRules[object] = ObjectRestriction{0, ObjectRestrictionType::Resize};

            bool ok;
            Command cmd = cmdMoveNodes(scene_, data, params, &ok);
            if (!ok)
            {
                // сигналим о невозможности укорачивания
                scene_->pushAdorner(
                    new NodePointAdorner(start, AdornerMovingWeld /* TODO поменять */));

                return;
            }

            nei->hoverState[data.replaced(object)] = HoverState::Newby;

            scene_->pushAdorner(
                new NodePointAdorner(start, AdornerMovingFromLine /* TODO возможно, поменять */));

            commandList_.addAndExecute(cmd);
        }
    }
}



}
