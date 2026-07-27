#ifndef ELATREEVIEW_H
#define ELATREEVIEW_H

#include <QTreeView>

#include "ElaProperty.h"

class ElaTreeViewPrivate;
class ELA_EXPORT ElaTreeView : public QTreeView
{
    Q_OBJECT
    Q_Q_CREATE(ElaTreeView)
    Q_PROPERTY_CREATE_Q_H(int, ItemHeight)
    Q_PROPERTY_CREATE_Q_H(int, HeaderMargin)
    // 对外暴露分支箭头大小，默认值由 ElaTreeViewStyle 保持为 17 像素。
    Q_PROPERTY_CREATE_Q_H(int, BranchIndicatorSize)
public:
    explicit ElaTreeView(QWidget* parent = nullptr);
    ~ElaTreeView();
};

#endif // ELATREEVIEW_H
