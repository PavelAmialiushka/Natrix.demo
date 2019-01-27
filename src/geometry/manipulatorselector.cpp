#include "manipulatorselector.h"

#include "rectangleAdorner.h"
#include "manipulator.h"
#include "manipulatorTextEdit.h"
#include "canvasRectangle.h"

namespace geometry
{

Selector::Selector(Manipulator* m)
    : ManipulatorTool(m)    
{
    preserveSelection_ = true;
    supportModes_ = Dragging | Clicking;
}

PManipulatorTool Selector::create(Manipulator* m)
{
    return PManipulatorTool(new Selector(m));
}

QString Selector::helperText() const
{
    return trUtf8("<b>Выделение:</b> счелкните на объекте, который необходимо выделить"
                  " либо обведите группу элементов")
           + helperTab + trUtf8("удерживайте <b>SHIFT</b> чтобы добавить к выделению")
           + helperTab + trUtf8("удерживайте <b>CTRL+SHIFT</b> чтобы отнять от выделения");
}

void Selector::makeSelection(PNeighbourhood nei, bool combine)
{
    auto mode = manipulator_->getMode(SelectMode);

    // сбрасываем предыдущее выделение, при необходимости
    if (combine && mode == Normal)
        selRemObjects += scene_->selectedObjects();

    // добавляем все объекты к выделению
    QSet<PObjectToSelect> changed;
    foreach(PObject obj, nei->closestObjects)
    {
        changed << obj;
        if ( (mode == Append || mode == Normal)
            || (mode == Modify && !scene_->selectedObjects().contains(obj)))
            selAddObjects << obj;
        else
            selRemObjects << obj;
    }

    // добавляем метки к выделению
    foreach(PLabel lab, nei->closestLabels)
    {
        if (lab.dynamicCast<CanvasRectangle>())
            continue;

        changed << lab;
        if ( (mode == Append || mode == Normal)
            || (mode == Modify && !scene_->selectedObjects().contains(lab)))
            selAddObjects << lab;
        else
            selRemObjects << lab;
    }

    foreach(PObjectToSelect obj, changed)
        nei->hoverState[obj] = ToBeSelected;
}

void Selector::do_prepare()
{
    selAddObjects.clear();
    selRemObjects.clear();

    auto mode = manipulator_->getMode(SelectMode);
    if (mode == Append)
        setCursor("+");
    else if (mode == Remove)
        setCursor("-");
    else if (mode == Modify)
        setCursor("+-");
    else
        setCursor("");
}

static void clearToBeSelectedItems(Scene* scene_)
{
//    scene_->setHoverObjects( scene_->getHoverObjects(ToBeSelected),
//                             NoHover, ToBeSelected);
}

void Selector::do_move(point2d start, PNeighbourhood nei)
{
    clearToBeSelectedItems(scene_);
    makeSelection(nei, false);
}

void Selector::do_click(point2d start, PNeighbourhood nei)
{
    makeSelection(nei);
}

void Selector::do_doubleClick(point2d point, PNeighbourhood nei)
{
    // добавляем метки к выделению

    foreach(PLabel lab, nei->closestLabels)
    {
        if (auto label = lab.dynamicCast<TextLabel>())
        {
            nextTool_ = TextEdit::createOnExistingLabel(
                        manipulator_,
                        label, point,
                        false, /* select all*/
                        true /* return to prev tool */);
            continue;
        }
    }
}

void Selector::do_drag(point2d start, PNeighbourhood nei, bool started)
{
    if (started) lt = start;
    else         rb = start;

    auto adorner = new RectangleAdorner(lt, rb);
    scene_->pushAdorner(adorner);

    // получаем список объектов, подавших под выделение
    auto n = scene_->getObjectsRectangle(lt, rb);
    qSwap(*nei, *n);

    // оменяем предыдущее временное выделение
    clearToBeSelectedItems(scene_);

    // делаем выделение
    makeSelection(nei);
}

void Selector::do_drop(point2d, PNeighbourhood nei)
{
    // получаем список объектов, подавших под выделение
    nei = scene_->getObjectsRectangle(lt, rb);

    // делаем выделение
    makeSelection(nei);
}

void Selector::do_commit()
{
    if (selAddObjects.size() || selRemObjects.size())
    {
        selRemObjects -= selAddObjects;
        scene_->addSelectedObjects(selAddObjects);
        scene_->removeSelectedObjects(selRemObjects);

        scene_->updatePropertiesAfterSceneChange();

        clearToBeSelectedItems(scene_);
        selAddObjects.clear();
        selRemObjects.clear();
    }
}

}
