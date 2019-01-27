#ifndef SCENEPROPERTIES_H
#define SCENEPROPERTIES_H

#include <QObject>
#include <QSet>

#include "BaseObject.h"

#include "scenePropertyValue.h"
#include "textInfo.h"
#include "lineinfo.h"
#include "element.h"

class QtProperty;

namespace geometry
{

struct SceneProperties
{
    SceneProperties();

    QSet<PObjectToSelect> selection;

    LineInfo defaultLineInfo;
    TextInfo defaultTextInfo;
    ElementInfo defaultElementInfo;

    ScenePropertyValue textSize;
    ScenePropertyValue textRotation;
    ScenePropertyValue textDecoration;

    ScenePropertyValue valveSize;
    ScenePropertyValue valveFlanges;

    ScenePropertyValue lineStyle;
    ScenePropertyValue bendStyle;
    ScenePropertyValue teeStyle;

    void loadSelected();
    void addElement(class ElementInfo const&);
    static int getElementScale(int x);

    void addLineInfo(LineInfo);

    void addText(class TextInfo const&);
    static int getTextScale(int x);


public:
    void loadDefaults();
};

}

#endif // SCENEPROPERTIES_H
