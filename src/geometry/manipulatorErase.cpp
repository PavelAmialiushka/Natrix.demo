#include "manipulatorErase.h"
#include "sceneprocedures.h"
#include "rectangleAdorner.h"
#include "canvasRectangle.h"

namespace geometry
{

EraseObjects::EraseObjects(Manipulator* m)
    : ManipulatorTool(m)
{
    supportModes_ = Clicking | Dragging;
}

PManipulatorTool EraseObjects::create(Manipulator* m)
{
    auto p = PManipulatorTool(new EraseObjects(m));
    return p;
}

QString EraseObjects::helperText() const
{
    return trUtf8("<b>Удаление объектов:</b> щелкните на объекте, который необходимо удалить"
                  " либо обведите группу элементов");
}

void EraseObjects::makeSelection(PNeighbourhood nei)
{
    // добавляем все объекты к выделению
    foreach(PObject obj, nei->closestObjects)
    {
        selObjects << obj;
    }

    // добавляем метки к выделению
    foreach(PLabel lab, nei->closestLabels)
    {
        if (lab.dynamicCast<CanvasRectangle>())
            continue;
        selObjects << lab;
    }

    foreach(PObjectToSelect obj, selObjects)
        nei->hoverState[obj] = Newby;
}

void EraseObjects::do_prepare()
{
    setCursor("erase");
    selObjects.clear();
}

void EraseObjects::do_drag(point2d start, PNeighbourhood nei, bool started)
{
    if (started) lt = start;
    else         rb = start;

    auto adorner = new RectangleAdorner(lt, rb);
    scene_->pushAdorner(adorner);

    // получаем список объектов, подавших под выделение
    auto n = scene_->getObjectsRectangle(lt, rb);
    qSwap(*nei, *n);

    makeSelection(nei);
}

void EraseObjects::do_drop(point2d start, PNeighbourhood nei)
{
    rb = start;

    // получаем список объектов, подавших под выделение
    nei = scene_->getObjectsRectangle(lt, rb);

    // делаем выделение
    makeSelection(nei);
}

void EraseObjects::do_click(point2d start, PNeighbourhood nei)
{
    makeSelection(nei);
}

void EraseObjects::do_commit()
{
    if (selObjects.size())
    {
        commandList_.addAndExecute( cmdEraseObjectsToSelect( scene_, selObjects ) ) ;
    }
}

}
