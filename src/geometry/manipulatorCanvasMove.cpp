#include "manipulatorCanvasMove.h"
#include "moveAdorner.h"
#include "canvasrectangle.h"
#include "sceneprocedures.h"
#include "neighbourhood.h"
#include "canvasGrip.h"
#include "ghostAdorner.h"

#include <algorithm>

namespace geometry {


QString CanvasMove::helperText() const
{
    return trUtf8("<b>Перемещение холста:</b>")
            + helperTab + trUtf8("<b>перетащите</b> холст чтобы изменить его положение, <b>удерживая CTRL</b> чтобы создать новый холст")
            + helperTab + trUtf8("<b>перетащите на существующий холст</b> чтобы удалить холст");
}

CanvasMove::CanvasMove(Manipulator* m, point2d pt, PCanvasRectangle label)
    : ManipulatorTool(m)
    , start_(pt)
    , canvas_(label)
{
    supportModes_ = Clicking | Dragging;
}

PManipulatorTool CanvasMove::create(Manipulator *m, point2d pt, PCanvasRectangle label)
{
    return PManipulatorTool(new CanvasMove(m, pt, label));
}

void CanvasMove::do_prepare()
{
    nextTool_ = manipulator_->prevTool();

    for(int index=0; index < scene_->canvasCount(); ++index)
    {
        PCanvasRectangle canvas = scene_->canvas(index);
        if (canvas == canvas_) continue;
        scene_->pushGrip(
                    new CanvasGrip(canvas->info().index, canvas));
    }

}


static bool canAcceptText(PObject object)
{
    if (auto el = object.dynamicCast<Element>())
        return el->canBreakLine(0);

    return 1;
}

void CanvasMove::do_click(point2d pt, PNeighbourhood n)
{
    do_drag(pt, n);
}

static bool canvasOverlap(Scene* scene_, PCanvasRectangle canvas0, point2d delta)
{
    for(int index=0; index < scene_->canvasCount(); ++index)
    {
        PCanvasRectangle canvas = scene_->canvas(index);
        if (canvas0 == canvas) continue;

        if (canvas->info().leftTop == canvas0->info().leftTop + delta
                && canvas->info().rightBottom == canvas0->info().rightBottom + delta)
            return true;
    }

    return false;
}


static bool snapDelta(Scene* scene_, point2d c0, point2d &delta)
{
    double w = 0, h = 0;

    QList<double> hor, ver;

    // какой объект двигаем    
    CanvasInfo inf;
    for(int index=0; index < scene_->canvasCount(); ++index)
    {
        inf = scene_->canvas(index)->info();
        ver << inf.leftTop.x << inf.rightBottom.x;
        hor << inf.leftTop.y << inf.rightBottom.y;

        if (inf.center == c0)
        {
            w = (inf.rightBottom - inf.leftTop).x;
            h = (inf.rightBottom - inf.leftTop).y;
        }
    }

    if (w == 0 || h == 0)
    {
        w = (inf.rightBottom - inf.leftTop).x;
        h = (inf.rightBottom - inf.leftTop).y;
    }

    auto c1 = c0 + delta;
    double left = c1.x - w/2;
    double right = c1.x + w/2;
    double top = c1.y - h/2;
    double bottom = c1.y + h/2;

    auto closest_to = [](double x0)
    {
        return [=](double x1, double x2)
        {
            return fabs(x0-x1) < fabs(x0-x2);
        };
    };

    double dx = *std::min_element(ver.begin(), ver.end(), closest_to(left)) - left;
    double dx2 = *std::min_element(ver.begin(), ver.end(), closest_to(right)) - right;
    if (fabs(dx2)<fabs(dx))
        dx = dx2;
    if (fabs(dx)>w/20)
        dx = 0;

    double dy = *std::min_element(hor.begin(), hor.end(), closest_to(top)) - top;
    double dy2 = *std::min_element(hor.begin(), hor.end(), closest_to(bottom)) - bottom;
    if (fabs(dy2)<fabs(dy))
        dy = dy2;
    if (fabs(dy)>h/20)
        dy = 0;

    delta += point2d(dx, dy);
    return true;
}

void CanvasMove::do_drag(point2d pt, PNeighbourhood n, bool)
{
    auto mode = manipulator_->getMode(SelectMode);
    auto delta = pt - start_;
    point2d c0 = canvas_.dynamicCast<CanvasRectangle>()->info().center;

    Command cmd;
    if (mode == Append)
    {
        // копирование
        if (snapDelta(scene_, c0, delta))
        {
            CanvasInfo inf = canvas_->info();
            inf.center += delta;
            inf.leftTop += delta;
            inf.rightBottom += delta;

            inf.index = 1;
            for(int index=0; index < scene_->canvasCount(); ++index)
                inf.index = std::max(inf.index, 1 + scene_->canvas(index)->info().index);

            scene_->pushGrip(
                        new CanvasGrip(canvas_->info().index,
                                       canvas_));

            PLabel newBy = CanvasRectangle::create(scene_, inf);
            if (!canvasOverlap(scene_, newBy.dynamicCast<CanvasRectangle>(), 0))
            {
                cmd << cmdAttachLabel(scene_, newBy);
                n->hoverState[newBy] = Newby;

                scene_->pushGrip(
                            new CanvasGrip(inf.index,
                                           newBy.dynamicCast<CanvasRectangle>()));
           }
        }
    } else
    {
        // перемещение
        if (snapDelta(scene_, c0, delta))
        {
            scene_->pushAdorner(new GhostAdorner(canvas_));

            if (canvasOverlap(scene_, canvas_, delta))
            {
                // удаление
                cmd << cmdDetachLabel(scene_, canvas_);

                // удаление может сломать нумерацию канвасов
                // поэтому должно переменовывать все канвасы
                QList<PCanvasRectangle> canvases;
                for(int index = 0; index < scene_->canvasCount(); ++index)
                {
                    auto canvas = scene_->canvas(index);
                    if (canvas != canvas_)
                        canvases << canvas;
                }
                std::sort(canvases.begin(), canvases.end(), byCanvasIndex);
                int index=0;
                foreach(PCanvasRectangle cnv, canvases)
                {
                    auto cnv2 = toCanvasRectangle(cnv->clone(scene_, 0));
                    auto info = cnv2->info();
                    info.index = ++index;
                    cnv2->setInfo(info);
                    cmd << cmdReplaceLabel(scene_, cnv, cnv2, false);
                }
            }
            else
            {
                // перемещение
                PLabel newBy = canvas_->clone(scene_, delta);
                cmd << cmdReplaceLabel(scene_, canvas_, newBy, false);

                scene_->pushGrip(
                            new CanvasGrip(
                                canvas_->info().index,
                                newBy.dynamicCast<CanvasRectangle>()));
            }
        }
    }

    commandList_.addAndExecute( cmd );

    setCursor("size");
    scene_->pushAdorner(new MoveAdorner(start_, pt));
}

void CanvasMove::do_drop(point2d pt, PNeighbourhood n)
{
    do_drag(pt, n);
}


} // namespace geometry

