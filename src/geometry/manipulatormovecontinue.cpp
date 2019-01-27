#include "manipulatormovecontinue.h"
#include "manipulatormoveStart.h"
#include "sceneprocedures.h"
#include "manipulator.h"

#include "neighbourhood.h"
#include "nodepointadorner.h"
#include "moveAdorner.h"

#include "joiner.h"

#include <QSet>

namespace geometry
{

QString MoveContinue::helperText() const
{
    return trUtf8("<b>Перемещение:</b> указываем новое положение элемента" )
            + helperTab + trUtf8("удерживайте <b>CTRL</b> чтобы присоединить перемещаемый элемент к другому контуру")
            + helperTab + trUtf8("удерживайте <b>SHIFT</b> чтобы смещать часть контура вдоль осей")
            + helperTab + ManipulatorTool::helperText();
}

MoveContinue::MoveContinue(Manipulator* m, MoveData data)
    : ManipulatorTool(m)
    , data_(data)
{
    fastRedrawPossible_ = true;
    supportModes_ = Clicking | DragToClick;
}

PManipulatorTool MoveContinue::create(Manipulator* m, MoveData data)
{
    return PManipulatorTool( new MoveContinue(m, data) );
}

void MoveContinue::do_prepare()
{
    nextTool_ = PManipulatorTool( new MoveStart(manipulator_));
}

void MoveContinue::do_tearDown()
{
    data_.adornerCache.clear();
}

static QList<point3d> proposeMoveDirections(PObject object)
{
    QSet<point3d> result;
    foreach(PNode node, object->nodes())
    {
        result << node->direction();
        result << -node->direction();
    }

    return result.toList();
}

void MoveContinue::do_click(point2d clicked, PNeighbourhood nei)
{
    if (manipulator_->getMode(ControlKey))
    {
        return modifyMove(clicked, nei);
    }
    else
    {
        return simpleMove(clicked, nei);
    }
}

void MoveContinue::simpleMove(point2d clicked, PNeighbourhood nei)
{
    setCursor("stop");

    // устанаваливаем местку старта
    auto t = data_.object.dynamicCast<Joiner>()
            ? AdornerMovingFromNode
            : AdornerMovingFromLine;
    scene_->pushAdorner(new NodePointAdorner(data_.startPoint, t));

    // определяем конечную точку перемещения
    if (manipulator_->getMode(ShiftKey) ==0)
    {
        // свободное перемещение
        data_.destination = clicked >> *scene_->worldSystem();
        scene_->pushAdorner(new MoveAdorner(data_.startPoint, data_.destination, false));
    } else
    {
        // перемещение по плоскостям
        QList<point3d> variants = generateDirections();
        variants << proposeMoveDirections(data_.object);

//        if (manipulator_->getMode(ShiftKey))
//        {
//            if (variants.contains( manipulator_->preferedDirection() ))
//            {
//                variants.clear();
//                variants << manipulator_->preferedDirection();
//            }
//        }

        bool success;
        point3d result_end;
        std::tie(result_end, success) = selectDirection(variants, data_.startPoint, clicked, 2);

        // если направление не выбрано то отмена
        if (!success)
            return;

        Q_ASSERT(result_end != data_.startPoint);
        nextTool_ = PManipulatorTool(new MoveStart(manipulator_));

        // определяем конец движения
        data_.destination = result_end;
        scene_->pushAdorner(new MoveAdorner(data_.startPoint, data_.destination, true));
    }

    nextTool_ = PManipulatorTool(new MoveStart(manipulator_));
    commandList_.addAndExecute(
                manipulator_->getMode(ShiftKey)==0
                ? cmdShiftObject(scene_, data_)
                : cmdMoveObject(scene_, data_) );

    // выводим подсказку
    setCursor("size");
    scene_->pushAdorner(new NodePointAdorner(data_.destination, t));
}

void MoveContinue::modifyMove(point2d clicked, PNeighbourhood nei)
{
    setCursor("stop");
    auto t = data_.object.dynamicCast<Joiner>()
                ? AdornerMovingFromNode
                : AdornerMovingFromLine;
    scene_->pushAdorner(new NodePointAdorner(data_.startPoint, t));
    nextTool_ = PManipulatorTool(new MoveStart(manipulator_));

    // если есть куда прилепиться
    if (!nei->closestObjects.empty())
    {
        PObject landObject = nei->closestObjects.first();

        bool canMove;
        MoveData localData = data_;
        localData.destination = nei->objectInfo[landObject].closestPoint;
        localData.fixedObjects << landObject;

        Command cmd = cmdMoveConnectObject(scene_, localData, landObject, &canMove);
        if (canMove)
        {
            commandList_.addAndExecute( cmd );
            setCursor("size");
            return;
        }
    }

    return simpleMove(clicked, nei);
}


void MoveContinue::do_commit()
{
}


}
