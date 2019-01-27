#ifndef MANIPULATORMOVECONTINUE_H
#define MANIPULATORMOVECONTINUE_H

#include "manipulatorTools.h"
#include "moveProcedures.h"

namespace geometry
{

class MoveContinue : public ManipulatorTool
{
    Q_OBJECT

    MoveData data_;
public:
    MoveContinue(Manipulator* m, MoveData data);

    static
    PManipulatorTool create(Manipulator* m, MoveData data);

public:
    void do_prepare() override;
    void do_tearDown() override;
    void do_click(point2d, PNeighbourhood) override;


    void do_commit() override;
    QString helperText() const;

private:
    void simpleMove(point2d, PNeighbourhood);
    void modifyMove(point2d, PNeighbourhood);
};

}

#endif // MANIPULATORMOVECONTINUE_H
