#include "drawer.h"

#include "scene.h"


namespace geometry
{

#if 0
Drawer::Drawer()
{
}

bool compareByXY(point2d a, point2d b)
{
    return std::abs(a.x-b.x)<POINT_PREC
            ? a.y < b.y
            : a.x < b.x;
}

void Drawer::redraw(Scene const* scene, double scale, bool exporting)
{
    scene->recalcCaches();
    changeScale(scale);

    // рисуем сцену
    foreach(PPrimitive p, scene->primitives())
    {
        if (exporting)
        {
            p->hover = 0, p->selected = 0;

            if (p->style == StyleViewport)
                continue;
        }
        draw(p);
    }

    // рисуем адорнеры
    foreach(PAdorner ado, scene->getAdorners())
    {
        Q_ASSERT(ado);

        if (ado->isTemporal() && exporting)
            continue;

        foreach(PPrimitive p, ado->draw(scene))
        {
            if (exporting) { p->hover = 0, p->selected = 0; }
            draw(p);
        }
    }

    // рисуем прицел
    if (!exporting) draw_cross(scene);
}


void Drawer::draw_cross(Scene const* scene)
{
    point2d pt = scene->cursorPoint();
    WorldSystem &pw = *scene->worldSystem();
    point3d p = pw.toGlobal(pt);

    QList<point3d> dirs;
    dirs << point3d(1,0)
         << point3d(-1,0)
         << point3d(0,1)
         << point3d(0,-1)
            ;

    QList<point2d> pts;
    foreach(point3d d, dirs)
    {
        pts <<  pw.toUser(p + d);
    }
    point2d lt = scene->visibleLeftTop();
    point2d rb = scene->visibleRightBottom();
    point2d rt(rb.x, lt.y);
    point2d lb(lt.x, rb.y);

    if (lt == rb) return;

    QList<point2d> brds, pts2;
    brds << lt << lb << rt << rb
         << lt << rt << lb << rb;


    for(int p_index=0; p_index<pts.size(); p_index+=2)
    {
        point2d aa = pts[p_index],
                bb = pts[p_index+1];

        pts2.clear();
        for(int b_index = 0; b_index < brds.size(); b_index+=2)
        {
            point2d m1 = brds[b_index];
            point2d m2 = brds[b_index+1];

            bool ins, np;
            point2d q = point2d::cross_segments(
                        aa, bb, m1, m2, &ins, &np);
            if (np)
            {
                pts2 << q;
            }
        }

        std::sort(pts2.begin(), pts2.end(), compareByXY);
        if (pts2.size()==4)
        {
            pts2.takeFirst();
            pts2.takeLast();
        }

        pts[p_index] = pts2[0];
        pts[p_index+1] = pts2[1];
    }

    draw( PrimitiveLine(pts[0], pts[1], StyleAdornerCursorLine));
    draw( PrimitiveLine(pts[2], pts[3], StyleAdornerCursorLine));
}

#endif
}
