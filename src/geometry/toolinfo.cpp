#include "toolinfo.h"
#include "manipulator.h"

#include "manipulatorElementStart.h"
#include "manipulatorCanvasStart.h"
#include "manipulatorLineStart.h"
#include "manipulatorWeldStart.h"
#include "manipulatorErase.h"
#include "manipulatorLineBreaker.h"
#include "manipulatorSelector.h"
#include "manipulatorMoveStart.h"
#include "manipulatorTextStart.h"
#include "manipulatorLabelStart.h"
#include "manipulatorLineShortener.h"

namespace geometry
{

ToolInfo::~ToolInfo()
{}

struct T_Select : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return Selector::create(m);
    }
};

struct T_Layout : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return CanvasStart::create(m);
    }
};

struct T_Line : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)

    {
        return PManipulatorTool(new LineStart(m));
    }
};

struct T_Weld : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)

    {
        return PManipulatorTool(new WeldStart(m));
    }
};

struct T_Move : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return MoveStart::create(m);
    }
};

struct T_Erase : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return EraseObjects::create(m);
    }
};

struct T_Text : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return TextStart::create(m);
    }
};

struct T_Break : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return LineBreaker::create(m);
    }
};

struct T_Short : ToolInfo
{ PManipulatorTool createTool(Manipulator* m)
    {
        return LineShortener::create(m);
    }
};


static ToolSet primaryToolSet()
{
    ToolSet set;
    set.name = QObject::trUtf8("базовые");

    set.tools << ToolInfo::create("select", QObject::trUtf8("Выбрать"), QString("Space"), new T_Select);
    set.tools << ToolInfo::create("layout", QObject::trUtf8("Холсты"), QString(""), new T_Layout);
    set.tools << ToolInfo::create("line", QObject::trUtf8("Труба"), QString("L"), new T_Line);
    set.tools << ToolInfo::create("weld", QObject::trUtf8("Шов"), QString("W"), new T_Weld);
    set.tools << ToolInfo::create("text", QObject::trUtf8("Текст"), QString("T"), new T_Text);
    set.tools << ToolInfo::create("move", QObject::trUtf8("Сдвигать"), QString("M"), new T_Move);
    set.tools << ToolInfo::create("erase", QObject::trUtf8("Удалять"), QString("E"), new T_Erase);
    set.tools << ToolInfo::create("breaker", QObject::trUtf8("Разбить"), QString("B"), new T_Break);
    set.tools << ToolInfo::create("shortener", QObject::trUtf8("Укоротить"), QString("S"), new T_Short);
    return set;
}

struct T_Element : ToolInfo
{
    QString name;
    T_Element(QString name)
        : name(name)
    {}

    PManipulatorTool createTool(Manipulator* m)
    {
        return PManipulatorTool(new StartElement(m, name));
    }
};

#define DECLARE_ELEMENT_TYPE(name, utf8name, keyName) \
    set.tools << ToolInfo::create(name, QObject::trUtf8(utf8name), QString(keyName), new T_Element(QString(name) + "Element"))

static ToolSet toolSetNo2()
{
    ToolSet set;
    set.name = QObject::trUtf8("арматура");

    DECLARE_ELEMENT_TYPE("Valve", "Задвижка", "Z");
    DECLARE_ELEMENT_TYPE("CheckValve", "КОП", "");
    DECLARE_ELEMENT_TYPE("KipValve", "Клапан КИП", "");
    DECLARE_ELEMENT_TYPE("ElectricValve", "Электрозадвижка", "");
    DECLARE_ELEMENT_TYPE("CondTapper", "Конедсатоотводчик", "");
    DECLARE_ELEMENT_TYPE("Canceler", "Компенсатор", "");

    DECLARE_ELEMENT_TYPE("Trans", "Переход", "R");
    DECLARE_ELEMENT_TYPE("Semisphere", "Сферическая заглушка", "");

    DECLARE_ELEMENT_TYPE("FlangePair", "Фланцевая пара", "F");
    DECLARE_ELEMENT_TYPE("FlangePairBlind", "Заглушка", "");
    DECLARE_ELEMENT_TYPE("Diaphragm", "Диафрагма", "");

    DECLARE_ELEMENT_TYPE("SafetyValve", "ППК", "P");
    return set;
}

static ToolSet toolSetNo3()
{
    ToolSet set;
    set.name = QObject::trUtf8("аппараты");

    DECLARE_ELEMENT_TYPE("VesselNossle", "Аппарат", "V");
    DECLARE_ELEMENT_TYPE("VesselNossle2", "Аппарат 2", "");

    DECLARE_ELEMENT_TYPE("Pump", "Насос", "N");
    DECLARE_ELEMENT_TYPE("Pump2", "Насос 2", "");
    return set;    
}

struct T_Label : ToolInfo
{
    QString name;
    T_Label(QString name)
        : name(name)
    {}

    PManipulatorTool createTool(Manipulator* m)
    {
        return PManipulatorTool(new LabelStart(m, name));
    }
};

#define DECLARE_LABEL_TYPE(name, utf8name, keyName) \
    set.tools << ToolInfo::create(name, QObject::trUtf8(utf8name), QString(keyName), new T_Label(QString(name)))

static ToolSet toolSetNo4()
{
    ToolSet set;
    set.name = QObject::trUtf8("доп.элемент");

    DECLARE_LABEL_TYPE("Support", "Опора", "O");
    DECLARE_LABEL_TYPE("SupportSpring", "Пруж. опора", "");
    DECLARE_LABEL_TYPE("Clip", "Подвеска", "");
    DECLARE_LABEL_TYPE("ClipSpring", "Пруж.подвеска", "");

    DECLARE_LABEL_TYPE("ConSupport", "Конс.опора", "");
    DECLARE_LABEL_TYPE("ConSupportSpring", "Конс.пруж.опора", "");
    DECLARE_LABEL_TYPE("ConClip", "Конс.подвеска", "");
    DECLARE_LABEL_TYPE("ConClipSpring", "Конс.пруж.подвеск", "");

    DECLARE_LABEL_TYPE("Wall", "Стена/перекрытие", "D");
    DECLARE_LABEL_TYPE("Underground", "Земля", "");

    DECLARE_LABEL_TYPE("Arrow", "Направление", "A");
    return set;
}

static ToolSet toolSetNo5()
{
    ToolSet set;
    set.name = QObject::trUtf8("врезки");

    DECLARE_LABEL_TYPE("NosslePlug", "Пробка", "X");
    DECLARE_LABEL_TYPE("NossleValve", "Вентилёк", "");
    DECLARE_LABEL_TYPE("NosslePI", "Манометр", "");
    DECLARE_LABEL_TYPE("NossleTI", "Термопара", "");
    return set;
}


ToolSetFactory::ToolSetFactory()
{
    sets_ << primaryToolSet()
          << toolSetNo2()
          << toolSetNo4()
          << toolSetNo5()
          << toolSetNo3()
          ;
}

PToolInfo ToolSetFactory::toolByName(QString name)
{
    foreach(ToolSet set, sets_)
    {
        foreach(PToolInfo info, set.tools)
        {
            if (info->regName.toLower() == name.toLower())
                return info;
        }
    }
    return PToolInfo();
}

ToolSetFactory& ToolSetFactory::inst()
{
    static ToolSetFactory instance;
    return instance;
}

QList<ToolSet> ToolSetFactory::sets()
{
    return inst().sets_;
}

PToolInfo ToolSetFactory::toolSelect()
{
    return sets_[0].tools[0];
}
PToolInfo ToolSetFactory::toolLine()
{
    return sets_[0].tools[2];
}
PToolInfo ToolSetFactory::toolMove()
{
    return sets_[0].tools[5];
}
PToolInfo ToolSetFactory::toolErase()
{
    return sets_[0].tools[6];
}
PToolInfo ToolSetFactory::toolElement()
{
    return sets_[1].tools[0];
}


}
