#ifndef ELATOOLBUTTONPRIVATE_H
#define ELATOOLBUTTONPRIVATE_H

#include <QObject>
#include <QColor>

#include "ElaProperty.h"
class ElaToolButton;
class ElaToolButtonStyle;
class ElaToolButtonPrivate : public QObject
{
    Q_OBJECT
    Q_D_CREATE(ElaToolButton)
public:
    explicit ElaToolButtonPrivate(QObject* parent = nullptr);
    ~ElaToolButtonPrivate();

private:
    ElaToolButtonStyle* _toolButtonStyle{nullptr};
    QColor _iconColor; // 无效颜色表示沿用样式默认图标色，不覆盖原逻辑。
};

#endif // ELATOOLBUTTONPRIVATE_H
