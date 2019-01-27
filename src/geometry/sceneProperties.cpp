#include "Element.h"
#include "SceneProperties.h"
#include "TextLabel.h"
#include "label.h"
#include "line.h"

#include <QSet>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <scene.h>

#include "sceneProcedures.h"
#include "object.h"
#include "bendjoiner.h"

#include "lineinfo.h"
#include "element.h"
#include "teeJoiner.h"

namespace geometry
{

SceneProperties::SceneProperties()
{
    textSize.reset(ScenePropertyType::TextSize);
    textRotation.reset(ScenePropertyType::TextRotation);
    textDecoration.reset(ScenePropertyType::TextDecorection);

    valveSize.reset(ScenePropertyType::ValveSize);
    valveFlanges.reset(ScenePropertyType::ValveFlanges);

    lineStyle.reset(ScenePropertyType::LineStyle);
    bendStyle.reset(ScenePropertyType::BendStyle);
    teeStyle.reset(ScenePropertyType::TeeStyle);
}

void SceneProperties::loadDefaults()
{
    if (textSize.empty())
        addText(defaultTextInfo);

    if (valveSize.empty())
        addElement(defaultElementInfo);

    if (lineStyle.empty())
        lineStyle.addValue( defaultLineInfo.lineStyle );
    if (bendStyle.empty())
        bendStyle.addValue( defaultLineInfo.bendStyle );
    if (teeStyle.empty())
        teeStyle.addValue( defaultLineInfo.teeStyle );
}

void SceneProperties::loadSelected()
{
    // выделение
    foreach(auto it, selection)
    {
        if (auto label = it.dynamicCast<TextLabel>())
        {
            addText(label->info());
        } else if (auto joiner = it.dynamicCast<Joiner>())
        {
            lineStyle.addValue( joiner->lineStyle() );

            if (auto bend = it.dynamicCast<BendJoiner>())
                bendStyle.addValue(bend->bendStyle());
            if (auto tee = it.dynamicCast<TeeJoiner>())
                teeStyle.addValue(tee->teeStyle());
        } else if (auto elem = it.dynamicCast<Element>())
        {
            lineStyle.addValue( elem->lineStyle() );
            addElement(elem->info());

        } else if (auto line = it.dynamicCast<Line>())
        {
            lineStyle.addValue( line->lineStyle() );
        }
    }
}

void SceneProperties::addElement(const ElementInfo &info)
{
    int vs = (info.scaleFactor + 4) / 2;
    valveSize.addValue(vs);

    int hf = (int)info.hasFlanges();
    valveFlanges.addValue(hf);
}

int SceneProperties::getElementScale(int x)
{
    // 0=> -4
    // 1=> -2
    // 2=> 0
    // 3=> +2
    // 4=> +4

    return (x - 2) * 2;
}

void SceneProperties::addLineInfo(LineInfo info)
{
    lineStyle.addValue(info.lineStyle);
    bendStyle.addValue(info.bendStyle);
    teeStyle.addValue(info.teeStyle);
}

void SceneProperties::addText(const TextInfo &info)
{
    textSize.addValue( info.scale + 1 );
    textRotation.addValue( info.rotationIndex );
    textDecoration.addValue( info.decoration );
}

int SceneProperties::getTextScale(int x)
{
    return x - 1;
}




}
