#include "manipulatorTextLine2.h"
#include "manipulatortextedit.h"
#include "sceneProcedures.h"
#include "textLabel.h"
#include "markLabels.h"
#include "marker.h"
#include "manipulator.h"
#include "neighbourhood.h"
#include "utilites.h"
#include "manipulatorTextStart.h"

#include <QDebug>

namespace geometry
{

QString TextLine2::helperText() const
{
    return trUtf8("укажите начало линии текста");
}

TextLine2::TextLine2(Manipulator* m, PTextGrip grip)
    : ManipulatorTool(m)
    , movingGrip_(grip)
{
    supportModes_ = Dragging;
}

PManipulatorTool TextLine2::create(Manipulator* m, PTextGrip grip)
{
    return PManipulatorTool(
                new TextLine2(m, grip));
}

void TextLine2::do_prepare()
{
    nextTool_ = manipulator_->prevTool();
}

void TextLine2::do_drag(point2d point, PNeighbourhood nei, bool)
{
    return do_drop(point, nei);
}

void TextLine2::do_drop(point2d pt, PNeighbourhood nei)
{
    setCursor("moving");

    PMarker marker = movingGrip_->marker();

    Command cmd;
    cmd << cmdDetachMarker(scene_, marker);
    scene_->removeGrip(movingGrip_);

    auto markLabels = utils::filtered(nei->closestLabels, isMarkLabel);

    if (markLabels.size())
    {
        auto under = markLabels.first().dynamicCast<MarkLabel>();
        auto point = under->locateTextGripPoint(pt);

        auto newby = Marker::create(point, under, marker->follower);
        cmd << cmdAttachMarker(scene_, newby);

        scene_->pushGrip(new TextGrip(point >> *scene_->worldSystem(), marker));
    }
    else if (nei->closestObjects.size())
    {
        PObject under = nei->closestObjects.first();
        scene_->analizeCloseObject(nei, under, pt, false);

        ObjectInfo info = nei->objectInfo[under];

        auto newby = Marker::create(info.closestPoint, under, marker->follower);
        cmd << cmdAttachMarker(scene_, newby);

        scene_->pushGrip(new TextGrip(
                             info.closestPoint >> *scene_->worldSystem(),
                             marker));
    }

    commandList_.addAndExecute(cmd);
}


}
