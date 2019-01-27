#include "manipulatorLineBreaker.h"
#include "sceneprocedures.h"

#include "nodepointadorner.h"
#include "global.h"
#include "line.h"
#include "endcupjoiner.h"
#include "element.h"

#include "bendJoiner.h"
#include "moveProcedures.h"
#include "teeJoiner.h"
#include "weldjoiner.h"
#include "manipulator.h"
#include "ghostAdorner.h"

namespace geometry
{


LineBreaker::LineBreaker(Manipulator* m)
    : ManipulatorTool(m)
{
}

PManipulatorTool LineBreaker::create(Manipulator* m)
{
    auto p = PManipulatorTool(new LineBreaker(m));
    return p;
}

QString LineBreaker::helperText() const
{
    return trUtf8("<b>Разорвать линию:</b> укажите линию, которую нужно разорвать");
}

void LineBreaker::do_click(point2d pt, PNeighbourhood nei)
{
    PObject object;
    point3d el1, el2, p1, p2;

    auto cmdReplaceJoiner = [&](PNode lineNode)
    {
        Command cmd;
        auto neibNode = connectedNode(scene_, lineNode);
        Q_ASSERT(neibNode); // если нет, повреждён чертёж
        if (!neibNode) return cmd;

        auto neib = neibNode->object();

        if (neib.dynamicCast<EndCupJoiner>())
        {
            cmd << cmdDetachObject(scene_, neib, true);
        }
        else if (neib.dynamicCast<BendJoiner>())
        {
            auto node = secondNode(neibNode);
            auto newby = EndCupJoiner::create(scene_, node->globalPoint(), node->direction());

            cmd << cmdReplaceObject(scene_, neib, newby);
            nei->hoverState[newby] = HoverState::Newby;
        }
        else if (neib.dynamicCast<WeldJoiner>())
        {
            auto node = secondNode(neibNode);
            auto newby = EndCupJoiner::create(scene_, node->globalPoint(), node->direction());

            cmd << cmdReplaceObject(scene_, neib, newby);
            nei->hoverState[newby] = HoverState::Newby;
        }
        else if (neib.dynamicCast<TeeJoiner>())
        {
            auto node1 = secondNode(neibNode);
            auto node2 = thirdNode(neibNode);
            if (node1->direction() == -node2->direction())
            {
                point3d a, b;
                auto line1 = connectedNode(scene_, node1)->object();
                auto line2 = connectedNode(scene_, node2)->object();
                mostDistantPoints(line1, line2, a, b);

                PObject line = Line::create(scene_, a, b, line1);
                cmd << cmdReplaceObject2to1(scene_, line1, line2, line);
                cmd << cmdDetachObject(scene_, neib, true);
                nei->hoverState[line] = HoverState::Newby;
            }
            else
            {
                auto newby= BendJoiner::create(scene_, node1->globalPoint(),
                                               node1->direction(), node2->direction(),
                                               neib
#ifdef  _COPY_JOINER_WELD_POSITION
                                               , node1->weldPosition(), node2->weldPosition()
#endif
                                               );
                cmd << cmdReplaceObject(scene_, neib, newby);
                nei->hoverState[newby] = HoverState::Newby;

            }
        }
        return cmd;
    };

    auto doBreak = [&]()
    {
        Command cmd;
        PObject line1, line2;

        if ((p1 == el1 && p2 == el2)
            || (p1 == el2 && p2 == el1))
        {
            cmd << cmdDetachObject(scene_, object, true);
            cmd << cmdReplaceJoiner(object->nodeAtPoint(p1));
            cmd << cmdReplaceJoiner(object->nodeAtPoint(p2));

            scene_->pushAdorner( new GhostAdorner(object) );
        } else
        {
            if (p1 == el1)
            {
                cmd << cmdReplaceJoiner(object->nodeAtPoint(p1));
            }
            else
            {
                line1 = Line::create(scene_, p1, el1, object);
                PObject cup1 = EndCupJoiner::create(scene_, el1, (p1-el1).normalized());
                cmd << cmdAttachObject(scene_, cup1, object);
                nei->hoverState[cup1] = HoverState::Newby;
            }

            if (p2 == el2)
            {
                cmd << cmdReplaceJoiner(object->nodeAtPoint(p2));
            }
            else
            {
                line2 = Line::create(scene_, p2, el2, object);
                PObject cup2 = EndCupJoiner::create(scene_, el2, (p2-el2).normalized());
                cmd << cmdAttachObject(scene_, cup2, object);
                nei->hoverState[cup2] = HoverState::Newby;
            }
        }

        // используем контекст перемещения
        // для того, чтобы не потерять маркеры
        MoveData data;

        if (line1 && line2) cmd << cmdReplaceObject1to2(scene_, object, line1, line2, &data);
        else if (line1) cmd << cmdReplaceObject(scene_, object, line1, &data);
        else if (line2) cmd << cmdReplaceObject(scene_, object, line2, &data);

        commandList_.addAndExecute(cmd);
    };

    double size = lineMinimumSize;
    foreach(object, nei->closestObjects)
    {
        if (object.dynamicCast<Line>())
        {
            p1 = object->globalPoint(0);
            p2 = object->globalPoint(1);

            ObjectInfo info = nei->objectInfo[object];
            point3d start = info.closestPoint;

            // пытаемся создать краевые сдвиги
            bool ok;
            std::tie(el1, el2) = breakOrShortLineBy(p1, p2,
                                                    start, size, true, &ok);
            if (!ok)
                el1 = p1, el2 = p2;

            // откусить слева
            if (p1 == el1)
            {
                PNode other = connectedNode(scene_, object->nodeAt(0));
                if (other)
                {
                    // получилось
                    return doBreak();
                }
            }

            // откусить справа
            if (p2 == el2)
            {
                PNode other = connectedNode(scene_, object->nodeAt(1));
                if (other)
                {
                    return doBreak();
                }
            }

            // обыкновенное сечение
            std::tie(el1, el2) = breakLineBy(p1, p2, start, size, &ok);
            if (!ok)
                el1 = p1, el2 = p2;

            return doBreak();

            scene_->pushAdorner(
                new NodePointAdorner(start, AdornerLineFromNone));
        }
        else if (object.dynamicCast<Element>()
                 || object.dynamicCast<TeeJoiner>()
                 || object.dynamicCast<BendJoiner>()
                 )
        {
            // нодов может быть несколько
            PNode node = nei->objectInfo[object].node(object);
            if (!node) return;

            PNode lineNode = connectedNode(scene_, node);
            if (!lineNode) return;

            if (auto line = lineNode->object().dynamicCast<Line>())
            {
                object = line;
                p1 = lineNode->globalPoint();
                p2 = secondNode(lineNode)->globalPoint();

                bool ok;
                std::tie(el1, el2) = breakOrShortLineBy(p1, p2,
                                                        p1, size, true, &ok);
                if (!ok)
                    el1 = p1, el2 = p2;
                return doBreak();
            } else if (auto element = lineNode->object().dynamicCast<Element>())
            {
                // TODO разрвать две задвижки
                // необходимо учитывать glue элементы
//                if (manipulator_->getMode(LineMode))
//                {
//                    PNode node = nei->objectInfo[object].node(object);
//                    if (!node) return;

//                    MoveData data;
//                    data.startPoint = node->globalPoint();
//                    data.destination = data.startPoint.polar_to(
//                                secondNode(lineNode)->globalPoint(), size);
//                    data.object = element;
//                    data.fixedObjects << object;

//                    bool ok = false;
//                    commandList_.addAndExecute(
//                                cmdMoveObject(scene_, data, &ok) );
//                }
            }
        }
    }
}



}
