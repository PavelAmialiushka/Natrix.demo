#include "manipulatorMoveStart.h"
#include "manipulatorMoveContinue.h"
#include "manipulatorMoveLabel.h"
#include "manipulatorCanvasMove.h"
#include "manipulatorTools.h"
#include "moveProcedures.h"
#include "neighbourhood.h"
#include "nodepointadorner.h"

#include "canvasRectangle.h"
#include "joiner.h"

namespace geometry
{

QString MoveStart::helperText() const
{
    return trUtf8("<b>Перемещение:</b> выберите элемент, который хотите переместить");
}
MoveStart::MoveStart(Manipulator* m)
    : ManipulatorTool(m)
{
}

PManipulatorTool MoveStart::create(Manipulator* m)
{
    return PManipulatorTool( new MoveStart(m) );
}

void MoveStart::do_click(point2d pt, PNeighbourhood n)
{
    bool label=false;
    bool object=true;
    QList<PLabel> labels;
    QList<PCanvasRectangle> canvases;
    foreach(PLabel l, n->closestLabels)
    {
        if (auto c = l.dynamicCast<CanvasRectangle>())
        {
            canvases << c;
        }
        else
        {
            labels << l;
            label = true;
        }
    }

    if (n->closestObjects.empty())
        object = false;

    if (!label && !object && canvases.empty())
    {   // под мышкой ничего нет
        setCursor("stop");
        return;
    } else if (!label && !object)
    {
        setCursor("move");
        nextTool_ = CanvasMove::create(manipulator_, pt, canvases.first());
        return;
    }

    if (label && object)
    {
         double dla = n->labelInfo[labels.first()].distance;
         double dob = n->objectInfo[n->closestObjects.first()].distance;

         if (dob >= dla) label = 0;
         else object = 0;
    }

    if (object)
    {
        PObject object = n->closestObjects.first();
        ObjectInfo info = n->objectInfo[object];

        MoveData data;
        data.object = object;
        data.startPoint = info.closestPoint;
        nextTool_ = MoveContinue::create(manipulator_, data);

        // adorner
        auto t =  object.dynamicCast<Joiner>()
                        ? AdornerMovingFromNode
                        : AdornerMovingFromLine;
        scene_->pushAdorner(new NodePointAdorner(data.startPoint, t));
        setCursor("size");
    } else
    {
        PLabel label = labels.first();

        nextTool_ = PManipulatorTool( new MoveLabel(manipulator_, pt, label) );
        setCursor("size");
    }
}

}
