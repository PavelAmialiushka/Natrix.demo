#include "manipulator.h"
#include "manipulatorTools.h"
#include "manipulatorLineStart.h"
#include "manipulatorLineContinue.h"
#include "line.h"
#include "element.h"
#include "global.h"
#include "nodepointadorner.h"

#include "bendJoiner.h"
#include "endcupjoiner.h"
#include "sceneprocedures.h"
#include "weldjoiner.h"

#include <qDebug>
#include <qalgorithms.h>

namespace geometry
{

LineStart::LineStart(Manipulator* m)
    : ManipulatorTool(m)
{
    lineInfo_ = scene_->defaultLineInfo();
    supportModes_ = Clicking | DragToClick;
}

LineStart::LineStart(Manipulator* m, LineInfo info)
    : ManipulatorTool(m)
    , lineInfo_(info)
{
    supportModes_ = Clicking | DragToClick;
}

QString LineStart::helperText() const
{
    return QObject::trUtf8(
                "<b>Pисуем линию:</b> щелкните мышкой на начальной точке линии"
                );
}

void LineStart::do_takeToolProperty(SceneProperties &props)
{
    props.addLineInfo(lineInfo_);
}

bool LineStart::do_updateToolProperties(ScenePropertyValue v)
{
    if (!lineInfo_.apply(v))
        return false;

    return true;
}

void LineStart::do_click(point2d pt,
                         PNeighbourhood n)
{
    // если кликнули
    // конец линии, отвод или продолждение линии
    // свободный нод, прикрепляемся к ноду
    // середина линии - тройник продолжаем
    // отвод - тройник из отвода

    point3d start = scene_->worldSystem()->toGlobal(pt);

    if (n->closestObjects.empty())
    {   // под мышкой ничего нет

        // объект создается следующей командой
        scene_->pushAdorner(new NodePointAdorner(start, AdornerLineFromFree));
        nextTool_ = LineContinue::create_free(manipulator_, start, lineInfo_);
    } else
    {
        PObject object = n->closestObjects.first();
        ObjectInfo info = n->objectInfo[object];

        // существует и разрешает присоединение
        bool canStart = false;
        if (auto line = object.dynamicCast<Line>())
        {
            double d1 = line->nodeAt(0)->globalPoint().distance(info.closestPoint);
            double d2 = line->nodeAt(1)->globalPoint().distance(info.closestPoint);
            if (d1 >= lineMinimumSize && d2 >= lineMinimumSize)
            {
                canStart = true;
                scene_->pushAdorner(new NodePointAdorner(info.closestPoint,AdornerLineFromLine));
            }
        } else if (object.dynamicCast<Element>())
        {
            if (PNode node = info.node(object))
            {
                if ( !connectedNode(scene_, node) && node->jointType()==0 )
                {
                    canStart = true;
                    scene_->pushAdorner(new NodePointAdorner(info.closestPoint, AdornerLineFromNode));
                }
            }
        } else if (object.dynamicCast<EndCupJoiner>()
                   || object.dynamicCast<BendJoiner>()
                   || object.dynamicCast<WeldJoiner>())
        {
            canStart = true;
            scene_->pushAdorner(new NodePointAdorner(info.closestPoint, AdornerLineFromNode));
        }

        if (canStart)
        {
            ConnectionData data = {info.closestPoint, info, object};
            nextTool_ = LineContinue::createStartingFromLine(manipulator_, data,
                                                             lineInfo_);
        } else
        {
            scene_->pushAdorner(new NodePointAdorner(info.closestPoint, AdornerLineFromNone));
        }
    }
}
}
