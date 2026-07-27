#include "ElaTreeView.h"

#include "ElaScrollBar.h"
#include "ElaTreeViewPrivate.h"
#include "ElaTreeViewStyle.h"
ElaTreeView::ElaTreeView(QWidget* parent)
    : QTreeView(parent), d_ptr(new ElaTreeViewPrivate())
{
    Q_D(ElaTreeView);
    d->q_ptr = this;
    setObjectName("ElaTreeView");
    setStyleSheet(
        "#ElaTreeView{background-color:transparent;}"
        "QHeaderView{background-color:transparent;border:0px;}");

    setAnimated(true);
    setMouseTracking(true);

    ElaScrollBar* hScrollBar = new ElaScrollBar(this);
    hScrollBar->setIsAnimation(true);
    connect(hScrollBar, &ElaScrollBar::rangeAnimationFinished, this, [=]() {
        doItemsLayout();
    });
    setHorizontalScrollBar(hScrollBar);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    ElaScrollBar* vScrollBar = new ElaScrollBar(this);
    vScrollBar->setIsAnimation(true);
    connect(vScrollBar, &ElaScrollBar::rangeAnimationFinished, this, [=]() {
        doItemsLayout();
    });
    setVerticalScrollBar(vScrollBar);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    d->_treeViewStyle = new ElaTreeViewStyle(style());
    setStyle(d->_treeViewStyle);
}

ElaTreeView::~ElaTreeView()
{
    Q_D(ElaTreeView);
    delete d->_treeViewStyle;
}

void ElaTreeView::setItemHeight(int itemHeight)
{
    Q_D(ElaTreeView);
    if (itemHeight > 0)
    {
        d->_treeViewStyle->setItemHeight(itemHeight);
        doItemsLayout();
    }
}

int ElaTreeView::getItemHeight() const
{
    Q_D(const ElaTreeView);
    return d->_treeViewStyle->getItemHeight();
}

void ElaTreeView::setHeaderMargin(int headerMargin)
{
    Q_D(ElaTreeView);
    if (headerMargin >= 0)
    {
        d->_treeViewStyle->setHeaderMargin(headerMargin);
        doItemsLayout();
    }
}

int ElaTreeView::getHeaderMargin() const
{
    Q_D(const ElaTreeView);
    return d->_treeViewStyle->getHeaderMargin();
}

// 设置折叠/展开箭头的像素大小，并立即刷新当前树视口。
void ElaTreeView::setBranchIndicatorSize(int branchIndicatorSize)
{
    Q_D(ElaTreeView);
    if (branchIndicatorSize <= 0 || d->_treeViewStyle->getBranchIndicatorSize() == branchIndicatorSize)
    {
        return;
    }
    d->_treeViewStyle->setBranchIndicatorSize(branchIndicatorSize);
    doItemsLayout();
    viewport()->update();
    Q_EMIT pBranchIndicatorSizeChanged();
}

// 返回当前折叠/展开箭头的像素大小。
int ElaTreeView::getBranchIndicatorSize() const
{
    Q_D(const ElaTreeView);
    return d->_treeViewStyle->getBranchIndicatorSize();
}
