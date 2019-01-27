#include "canvasGrip.h"
#include "canvasRectangle.h"
#include "manipulatorCanvasMove.h"
#include "manipulatorCanvasStart.h"
#include "manipulatorMoveLabel.h"
#include "neighbourhood.h"
#include "utilites.h"

namespace geometry {

QString CanvasStart::helperText() const
{
    return trUtf8("<b>Pедактирование холстов:</b>")             
            + helperTab + trUtf8("<b>перетащите</b> холст чтобы изменить его положение, <b>удерживая CTRL</b> чтобы создать новый холст")
            + helperTab + trUtf8("<b>перетащите на существующий холст</b> чтобы удалить холст");
}

CanvasStart::CanvasStart(Manipulator* m)
    : ManipulatorTool(m)
{
    supportModes_ = Clicking | DragToClick;
}

PManipulatorTool CanvasStart::create(Manipulator *m)
{
    return PManipulatorTool( new CanvasStart(m) );
}

void CanvasStart::do_prepare()
{
    for(int index=0; index < scene_->canvasCount(); ++index)
    {
        PCanvasRectangle canvas=scene_->canvas(index);
        scene_->pushGrip(
                    new CanvasGrip(canvas->info().index, canvas));
    }
}

void CanvasStart::do_click(point2d pt, PNeighbourhood nei)
{
    PCanvasRectangle canvas;

    auto grips = utils::filter_transform<PCanvasGrip>(
                nei->closestGrips,
                toCanvasGrip);
    if (grips.size())
    {
        canvas = grips.first()->canvas();
    } else if (!nei->closestLabels.empty())
    {
        auto rects = utils::filter_transform<PCanvasRectangle>(
                    nei->closestLabels,
                    toCanvasRectangle);
        if (rects.size())
        {
            canvas = rects.first();
        }
    }

    if (canvas)
    {
        nextTool_ = PManipulatorTool( new CanvasMove(manipulator_, pt, canvas) );
        setCursor("move");
        return;
    }

    // под мышкой ничего нет
    setCursor("stop");
}





} // namespace geometry

