#ifndef ELATABBARPRIVATE_H
#define ELATABBARPRIVATE_H

#include "ElaProperty.h"
#include <QMimeData>
#include <QObject>
#include <QPixmap>
#include "ElaLineEdit.h"
class ElaTabBar;
class ElaTabBarStyle;
class QTabBarPrivate;
class ElaTabBarPrivate : public QObject
{
    Q_OBJECT
    Q_D_CREATE(ElaTabBar)
public:
    explicit ElaTabBarPrivate(QObject* parent = nullptr);
    ~ElaTabBarPrivate() override;

private:
    QMimeData* _mimeData{nullptr};
    ElaTabBarStyle* _style{nullptr};
    QTabBarPrivate* _tabBarPrivate{nullptr};
    ElaLineEdit* _editingLine{nullptr};
    int _editingIndex{-1};
    bool _enableRenaming{false};
};

#endif // ELATABBARPRIVATE_H
