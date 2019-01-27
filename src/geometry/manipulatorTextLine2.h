#ifndef MANIPULATORTEXTLINE2_H
#define MANIPULATORTEXTLINE2_H

#include "manipulatorTools.h"
#include "textGrip.h"

namespace geometry
{

class TextLine2
        : public ManipulatorTool
{
    Q_OBJECT

    PTextGrip movingGrip_;

public:
    TextLine2(Manipulator* m, PTextGrip grip);
    static PManipulatorTool create(Manipulator* m, PTextGrip grip);

public:
    void do_prepare();

    void do_drag(point2d, PNeighbourhood, bool);
    void do_drop(point2d, PNeighbourhood);

    QString helperText() const;
};

}
#endif // MANIPULATORTEXTLINE2_H
