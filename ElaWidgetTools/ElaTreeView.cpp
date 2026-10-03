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

// 同步设置行内内容的左侧留白，绘制与点击命中共用样式中的子元素矩形。
void ElaTreeView::setItemContentLeftPadding(int padding)
{
    Q_D(ElaTreeView);
    if (padding < 0 || d->_treeViewStyle->getItemContentLeftPadding() == padding) return;
    d->_treeViewStyle->setItemContentLeftPadding(padding);
    viewport()->update(); // 只重绘内容位置，不调整行背景、行高、列宽或层级缩进。
    Q_EMIT pItemContentLeftPaddingChanged();
}

// 返回当前树实例的内容左侧留白，其他树继续使用原有默认值。
int ElaTreeView::getItemContentLeftPadding() const
{
    Q_D(const ElaTreeView);
    return d->_treeViewStyle->getItemContentLeftPadding();
}

// 切换当前树的选中竖线，只刷新绘制，不改变选中状态或内容布局。
void ElaTreeView::setSelectionIndicatorVisible(bool visible)
{
    Q_D(ElaTreeView);
    if (d->_treeViewStyle->getSelectionIndicatorVisible() == visible) return;
    d->_treeViewStyle->setSelectionIndicatorVisible(visible);
    viewport()->update();
    Q_EMIT pSelectionIndicatorVisibleChanged();
}

// 返回当前树是否显示选中竖线，其他树实例沿用默认显示行为。
bool ElaTreeView::getSelectionIndicatorVisible() const
{
    Q_D(const ElaTreeView);
    return d->_treeViewStyle->getSelectionIndicatorVisible();
}
