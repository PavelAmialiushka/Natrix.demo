#ifndef PROPERTYTREEWIDGET_H
#define PROPERTYTREEWIDGET_H

#include "PropertyBarModel.h"

#include <QWidget>

#include <geometry/SceneProperties.h>

namespace Ui {
class PropertyTreeWidget;
}

class PropertyTreeWidget : public QWidget
{
    Q_OBJECT

    geometry::Scene* scene_;

    PropertyBarModel* textSizeBar_;
    PropertyBarModel* textRotationBar_;
    PropertyBarModel* textDecorationBar_;

    PropertyBarModel* valveSizeBar_;
    PropertyBarModel* valveFlangesBar_;

    PropertyBarModel* lineStyleBar_;
    PropertyBarModel* bendStyleBar_;
    PropertyBarModel* teeStyleBar_;

public:
    explicit PropertyTreeWidget(QWidget *parent, geometry::Scene* scene);
    ~PropertyTreeWidget();

public slots:
    void updatePropertiesAfterSceneChange();
    void onPropertyValueChanged(ScenePropertyValue);

private:
    Ui::PropertyTreeWidget *ui;
};

#endif // PROPERTYTREEWIDGET_H
