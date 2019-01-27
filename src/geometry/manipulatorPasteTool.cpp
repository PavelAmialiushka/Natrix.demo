#include "manipulatorPasteTool.h"
#include "document.h"
#include "manipulatorselector.h"
#include "sceneprocedures.h"
#include "markLabels.h"

namespace geometry
{


QString PasteTool::helperText() const
{
    return trUtf8("Выберете место для вставки")
           + helperTab + trUtf8("<b>Ctrl +[ или ]</b> чтобы повернуть");
}

PasteTool::PasteTool(Manipulator *m, QByteArray data)
    : ManipulatorTool(m)
    , data_(data)
    , rotation_(0)
{
    correct_ = false;

    QDomDocument doc;
    if (!doc.setContent(data_))
        return;

    auto docContents = doc.elementsByTagName("sktx_copyclip_contents");
    if (!docContents.size())
        return;

    auto docContent = docContents.at(0).toElement();

    if (!Document::loadSceneData(docContent, scene_,
                            objects_, labels_, markers_))
    {
        return;
    }

    QSet<point2d> points;
    foreach(PObject object, objects_)
       for(int index=0; index < object->nodeCount(); ++index)
            points << object->localPoint(index);
    foreach(PLabel label, labels_)
        points << label->leftTop()
               << label->rightBottom();

    double l, t, r, b;
    l=r=points.begin()->x;
    t=b=points.begin()->y;
    foreach(point2d p, points)
    {
        l = std::min(l, p.x); r = std::max(r, p.x);
        b = std::min(b, p.y); t = std::max(t, p.y);
    }
    center_ = (point2d(l,t)+point2d(r,b))/2;

    correct_ = true;
}

PManipulatorTool PasteTool::create(Manipulator *m, PManipulatorTool prev, QByteArray data)
{    
    auto tool = new PasteTool(m, data);
    tool->nextTool_ = prev;

    if (!tool->correct_)
        return PManipulatorTool();

    return QSharedPointer<PasteTool>(tool);
}

// количество шагов, на которые разбивается вращение на 90 градусов
constexpr int ROTATTION_SECTION_COUNT_PER_Q = 2;

void PasteTool::do_click(point2d pt, PNeighbourhood nei)
{
    Command cmd;
    const WorldSystem& ws = *scene_->worldSystem();

    point3d center3 = center_ >> ws;
    point3d delta3 = (pt >> ws) - center3;
    double rotAngle = rotation_ * M_PI_2 / ROTATTION_SECTION_COUNT_PER_Q;

    QMap<PObjectToSelect, PObjectToSelect> objectMap;
    foreach(PObject object, objects_)
    {
        auto clone = object->cloneMoveRotate(scene_, delta3, center3, rotAngle);
        objectMap[object] = clone;
        Q_ASSERT(clone);
        if (clone)
        {
            nei->hoverState[clone] = HoverState::Newby;
            cmd << cmdAttachObject(scene_, clone);
        }
    }

    QMap<PLabel, QList<PMarker>> label2marker;
    foreach(PMarker m, markers_)
        label2marker[m->follower] << m;

    QMap<PLabel, PLabel> labelMap;
    foreach(PLabel label, labels_)
    {
        PLabel clone;
        if (auto markLabel = label.dynamicCast<MarkLabel>())
        {
            clone = markLabel->cloneMoveRotate(scene_, delta3, center3, rotAngle);
        } else
        {
            QList<PMarker> m = label2marker[ label ];
            if (m.size())
            {
                auto marker = m.first();
                clone = label->cloneMoveRotate(scene_, delta3, center3, rotAngle, marker->point);
            } else
            {
                clone = label->cloneMoveRotate(scene_, delta3, center3, rotAngle);
            }
        }
        labelMap[label] = clone;
        Q_ASSERT(clone);
        if (clone)
        {
            nei->hoverState[clone] = HoverState::Newby;
            cmd << cmdAttachLabel(scene_, clone);
        }
    }
    foreach(PMarker sample, markers_)
    {
        PObjectToSelect leader;
        if (auto labelLeader = sample->leader.dynamicCast<Label>())
            leader = labelMap[labelLeader];
        else
            leader = objectMap[sample->leader];
        if (!leader) continue;

        auto follower = labelMap[sample->follower];
        if (!follower) continue;

        PMarker marker = Marker::create(delta3 + sample->point.rotate3dAround(center3, point3d::nz, rotAngle),
                                        leader, follower);
        cmd << cmdAttachMarker(scene_, marker);
    }

    setCursor("");
    commandList_.addAndExecute(cmd);
}

void PasteTool::do_prepare()
{
    nextTool_ = manipulator_->toolInfo()->createTool(manipulator_);
}

void PasteTool::do_changeRotation(int d)
{
    constexpr int POSITIONS = ROTATTION_SECTION_COUNT_PER_Q * 4;
    if (!d) d = 1;
    else d = -1;
    rotation_ = (rotation_ + d + POSITIONS) % POSITIONS;
}



}
