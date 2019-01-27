#ifndef MANIPULATORLINESHORTENER_H
#define MANIPULATORLINESHORTENER_H

#include "manipulatorTools.h"


namespace geometry
{

class LineShortener : public ManipulatorTool
{
    Q_OBJECT

public:
    LineShortener(Manipulator* m);

    static
    PManipulatorTool create(Manipulator* m);

public:
    void do_click(point2d, PNeighbourhood);
    QString helperText() const;
};

}

#endif // MANIPULATORLINESHORTENER_H
