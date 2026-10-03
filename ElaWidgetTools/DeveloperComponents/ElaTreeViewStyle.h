#ifndef ELATREEVIEWSTYLE_H
#define ELATREEVIEWSTYLE_H

#include <QProxyStyle>

#include "ElaDef.h"
class ElaTreeViewStyle : public QProxyStyle
{
    Q_OBJECT
    Q_PROPERTY_CREATE(int, ItemHeight)
    Q_PROPERTY_CREATE(int, HeaderMargin)
    // 保存 PE_IndicatorBranch 使用的图标字体大小。
    Q_PROPERTY_CREATE(int, BranchIndicatorSize)
    // 行内内容共用左内边距，默认保留原有 11 像素，不改变各元素之间的间距。
    Q_PROPERTY_CREATE(int, ItemContentLeftPadding)
    // 默认显示选中竖线，可由特定树关闭而保留选中背景。
    Q_PROPERTY_CREATE(bool, SelectionIndicatorVisible)
public:
    explicit ElaTreeViewStyle(QStyle* style = nullptr);
    ~ElaTreeViewStyle();
    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    void drawControl(ControlElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = nullptr) const override;
    QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption* option = nullptr, const QWidget* widget = nullptr) const override;
    QRect subElementRect(SubElement element, const QStyleOption* option, const QWidget* widget) const override;

private:
    ElaThemeType::ThemeMode _themeMode;
};

#endif // ELATREEVIEWSTYLE_H
