#include "ElaComboBox.h"

#include "ElaComboBoxStyle.h"
#include "ElaScrollBar.h"
#include "ElaTheme.h"
#include "private/ElaComboBoxPrivate.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QDebug>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QScreen>
Q_PROPERTY_CREATE_Q_CPP(ElaComboBox, int, BorderRadius)
ElaComboBox::ElaComboBox(QWidget* parent)
    : QComboBox(parent), d_ptr(new ElaComboBoxPrivate())
{
    Q_D(ElaComboBox);
    d->q_ptr = this;
    d->_pBorderRadius = 3;
    d->_themeMode = eTheme->getThemeMode();
    setObjectName("ElaComboBox");
    setFixedHeight(35);
    d->_comboBoxStyle = new ElaComboBoxStyle(style());
    setStyle(d->_comboBoxStyle);

    //调用view 让container初始化
    setView(new QListView(this));
    QAbstractItemView* comboBoxView = this->view();
    comboBoxView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    ElaScrollBar* scrollBar = new ElaScrollBar(this);
    comboBoxView->setVerticalScrollBar(scrollBar);
    ElaScrollBar* floatVScrollBar = new ElaScrollBar(scrollBar, comboBoxView);
    floatVScrollBar->setIsAnimation(true);
    comboBoxView->setAutoScroll(false);
    comboBoxView->setSelectionMode(QAbstractItemView::NoSelection);
    comboBoxView->setObjectName("ElaComboBoxView");
    comboBoxView->setStyleSheet("#ElaComboBoxView{background-color:transparent;}");
    comboBoxView->setStyle(d->_comboBoxStyle);
    QWidget* container = this->findChild<QFrame*>();
    if (container)
    {
        container->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        container->setAttribute(Qt::WA_TranslucentBackground);
        container->setObjectName("ElaComboBoxContainer");
        container->setStyle(d->_comboBoxStyle);
        QLayout* layout = container->layout();
        while (layout->count())
        {
            layout->takeAt(0);
        }
        layout->addWidget(view());
        layout->setContentsMargins(ElaComboBoxStyle::bodyInset(), 0,
                                   ElaComboBoxStyle::bodyInset(), 6);
#ifndef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        container->setStyleSheet("background-color:transparent;");
#endif
#endif
    }
    QComboBox::setMaxVisibleItems(5);
    connect(eTheme, &ElaTheme::themeModeChanged, d, &ElaComboBoxPrivate::onThemeChanged);
}

ElaComboBox::~ElaComboBox()
{
    Q_D(ElaComboBox);
    delete d->_comboBoxStyle;
}

void ElaComboBox::setEditable(bool editable)
{
    Q_D(ElaComboBox);
    QComboBox::setEditable(editable);
    if (editable)
    {
        lineEdit()->setStyle(d->_comboBoxStyle);
        d->onThemeChanged(d->_themeMode);
    }
}

void ElaComboBox::showPopup()
{
    Q_D(ElaComboBox);
    bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);
    QComboBox::showPopup();
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
    if (count() > 0)
    {
        QWidget* container = this->findChild<QFrame*>();
        if (container)
        {
            const int preferredContainerHeight = qMin(count(), maxVisibleItems()) * 35 + 8;
            const QPoint comboTopLeft = mapToGlobal(QPoint(0, 0));
            QScreen* popupScreen = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            if (window())
            {
                popupScreen = window()->screen();
            }
#endif
            if (!popupScreen)
            {
                popupScreen = QApplication::screenAt(comboTopLeft);
            }
            if (!popupScreen)
            {
                popupScreen = QApplication::primaryScreen();
            }
            const QRect availableRect = popupScreen ? popupScreen->availableGeometry() : QRect(comboTopLeft, QSize(width(), preferredContainerHeight + height() + 3));
            const int spaceBelow = availableRect.bottom() - (comboTopLeft.y() + height() + 3) + 1;
            const int spaceAbove = comboTopLeft.y() - availableRect.top();
            d->_isPopupOpenUpward = spaceBelow < preferredContainerHeight && spaceAbove > spaceBelow;
            const int availableHeight = qMax(1, d->_isPopupOpenUpward ? spaceAbove : spaceBelow);
            const int containerHeight = qMax(1, qMin(preferredContainerHeight, availableHeight));
            const int viewHeight = qMax(1, containerHeight - 8);
            view()->resize(view()->width(), viewHeight);
            container->move(QPoint(comboTopLeft.x(), d->_isPopupOpenUpward ? comboTopLeft.y() + 2 : comboTopLeft.y() + height() + 3));
            QLayout* layout = container->layout();
            while (layout->count())
            {
                layout->takeAt(0);
            }
            QPropertyAnimation* fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
            connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
                const int popupHeight = qMax(1, value.toInt());
                container->setFixedHeight(popupHeight);
                if (d->_isPopupOpenUpward)
                {
                    container->move(QPoint(comboTopLeft.x(), comboTopLeft.y() + 3 - popupHeight));
                }
            });
            fixedSizeAnimation->setStartValue(1);
            fixedSizeAnimation->setEndValue(containerHeight);
            fixedSizeAnimation->setEasingCurve(QEasingCurve::OutCubic);
            fixedSizeAnimation->setDuration(400);
            fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);

            QPropertyAnimation* viewPosAnimation = new QPropertyAnimation(view(), "pos");
            connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
                d->_isAllowHidePopup = true;
                layout->addWidget(view());
            });
            QPoint viewPos = view()->pos();
            const int showSlideOffset = d->_isPopupOpenUpward ? view()->height() : -view()->height();
            viewPosAnimation->setStartValue(QPoint(viewPos.x(), viewPos.y() + showSlideOffset));
            viewPosAnimation->setEndValue(viewPos);
            viewPosAnimation->setEasingCurve(QEasingCurve::OutCubic);
            viewPosAnimation->setDuration(400);
            viewPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        }
        //指示器动画
        QPropertyAnimation* rotateAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandIconRotate");
        connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
            update();
        });
        rotateAnimation->setDuration(300);
        rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
        rotateAnimation->setStartValue(d->_comboBoxStyle->getExpandIconRotate());
        rotateAnimation->setEndValue(-180);
        rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        QPropertyAnimation* markAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandMarkWidth");
        markAnimation->setDuration(300);
        markAnimation->setEasingCurve(QEasingCurve::InOutSine);
        markAnimation->setStartValue(d->_comboBoxStyle->getExpandMarkWidth());
        markAnimation->setEndValue(width() / 2 - d->_pBorderRadius -
                                   ElaComboBoxStyle::bodyInset());
        markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void ElaComboBox::hidePopup()
{
    Q_D(ElaComboBox);
    if (d->_isAllowHidePopup)
    {
        QWidget* container = this->findChild<QFrame*>();
        if (container)
        {
            const QPoint comboTopLeft = mapToGlobal(QPoint(0, 0));
            int containerHeight = container->height();
            QLayout* layout = container->layout();
            while (layout->count())
            {
                layout->takeAt(0);
            }
            QPoint viewPos = view()->pos();
            QPropertyAnimation* viewPosAnimation = new QPropertyAnimation(view(), "pos");
            connect(viewPosAnimation, &QPropertyAnimation::finished, this, [=]() {
                layout->addWidget(view());
                QMouseEvent focusEvent(QEvent::MouseButtonPress, QPoint(-1, -1), QPoint(-1, -1), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
                QApplication::sendEvent(parentWidget(), &focusEvent);
                QComboBox::hidePopup();
                container->setFixedHeight(containerHeight);
                view()->move(viewPos);
            });
            viewPosAnimation->setStartValue(viewPos);
            const int hideSlideOffset = d->_isPopupOpenUpward ? view()->height() : -view()->height();
            viewPosAnimation->setEndValue(QPoint(viewPos.x(), viewPos.y() + hideSlideOffset));
            viewPosAnimation->setEasingCurve(QEasingCurve::InCubic);
            viewPosAnimation->start(QAbstractAnimation::DeleteWhenStopped);

            QPropertyAnimation* fixedSizeAnimation = new QPropertyAnimation(container, "maximumHeight");
            connect(fixedSizeAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
                const int popupHeight = qMax(1, value.toInt());
                container->setFixedHeight(popupHeight);
                if (d->_isPopupOpenUpward)
                {
                    container->move(QPoint(comboTopLeft.x(), comboTopLeft.y() + 3 - popupHeight));
                }
            });
            fixedSizeAnimation->setStartValue(container->height());
            fixedSizeAnimation->setEndValue(1);
            fixedSizeAnimation->setEasingCurve(QEasingCurve::InCubic);
            fixedSizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
            d->_isAllowHidePopup = false;
        }
        //指示器动画
        QPropertyAnimation* rotateAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandIconRotate");
        connect(rotateAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
            update();
        });
        rotateAnimation->setDuration(300);
        rotateAnimation->setEasingCurve(QEasingCurve::InOutSine);
        rotateAnimation->setStartValue(d->_comboBoxStyle->getExpandIconRotate());
        rotateAnimation->setEndValue(0);
        rotateAnimation->start(QAbstractAnimation::DeleteWhenStopped);
        QPropertyAnimation* markAnimation = new QPropertyAnimation(d->_comboBoxStyle, "pExpandMarkWidth");
        markAnimation->setDuration(300);
        markAnimation->setEasingCurve(QEasingCurve::InOutSine);
        markAnimation->setStartValue(d->_comboBoxStyle->getExpandMarkWidth());
        markAnimation->setEndValue(0);
        markAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void ElaComboBox::paintEvent(QPaintEvent* event)
{
    Q_D(ElaComboBox);
    if (lineEdit() && lineEdit()->palette().color(QPalette::Text) != ElaThemeColor(d->_themeMode, BasicText))
    {
        d->onThemeChanged(d->_themeMode);
    }
    QComboBox::paintEvent(event);
}
