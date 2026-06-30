#ifndef ELATABBAR_H
#define ELATABBAR_H

#include <QDrag>
#include <QTabBar>

#include "ElaProperty.h"
class ElaTabBarPrivate;
class ELA_EXPORT ElaTabBar : public QTabBar
{
    Q_OBJECT
    Q_Q_CREATE(ElaTabBar)
    Q_PROPERTY_CREATE_Q_H(QSize, TabSize)
    Q_PROPERTY_CREATE_Q_H(bool, EnableRenaming)
public:
    explicit ElaTabBar(QWidget* parent = nullptr);
    ~ElaTabBar() override;
Q_SIGNALS:
    Q_SIGNAL void tabDragCreate(QMimeData* mimeData);
    Q_SIGNAL void tabDragEnter(QMimeData* mimeData);
    Q_SIGNAL void tabDragLeave(QMimeData* mimeData);
    Q_SIGNAL void tabDragDrop(QMimeData* mimeData);

protected:
    QSize sizeHint() const;
    QSize tabSizeHint(int index) const override;
    void resizeEvent(QResizeEvent* event) override;
    void tabInserted(int index) override;
    void tabRemoved(int index) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dragMoveEvent(QDragMoveEvent* event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void startEditing(int index);
    void commitEditing(bool save = true);
    void updateCloseButtonVisibility();
};

#endif // ELATABBAR_H
