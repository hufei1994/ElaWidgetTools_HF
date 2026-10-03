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
    // 同步调整复选框、图标和文字前的留白，保持内容内部间距及行容器位置。
    Q_PROPERTY_CREATE_Q_H(int, ItemContentLeftPadding)
    // 控制当前树的选中竖线，关闭后仍保留选中背景及选择交互。
    Q_PROPERTY_CREATE_Q_H(bool, SelectionIndicatorVisible)
public:
    explicit ElaTreeView(QWidget* parent = nullptr);
    ~ElaTreeView();
};

#endif // ELATREEVIEW_H
