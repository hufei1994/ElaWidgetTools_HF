#include "ElaMenu.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "DeveloperComponents/ElaMenuStyle.h"
#include "ElaCheckBox.h"
#include "private/ElaMenuPrivate.h"
ElaMenu::ElaMenu(QWidget* parent)
    : QMenu(parent), d_ptr(new ElaMenuPrivate())
{
    Q_D(ElaMenu);
    d->q_ptr = this;
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("ElaMenu");
    d->_menuStyle = new ElaMenuStyle(style());
    setStyle(d->_menuStyle);
    d->_pAnimationImagePosY = 0;
}

ElaMenu::ElaMenu(const QString& title, QWidget* parent)
    : ElaMenu(parent)
{
    setTitle(title);
}

ElaMenu::~ElaMenu()
{
    Q_D(ElaMenu);
    delete d->_menuStyle;
}

void ElaMenu::setMenuItemHeight(int menuItemHeight)
{
    Q_D(ElaMenu);
    d->_menuStyle->setMenuItemHeight(menuItemHeight);
}

int ElaMenu::getMenuItemHeight() const
{
    Q_D(const ElaMenu);
    return d->_menuStyle->getMenuItemHeight();
}

QAction* ElaMenu::addMenu(QMenu* menu)
{
    return QMenu::addMenu(menu);
}

ElaMenu* ElaMenu::addMenu(const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    QMenu::addAction(menu->menuAction());
    return menu;
}

ElaMenu* ElaMenu::addMenu(const QIcon& icon, const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    menu->setIcon(icon);
    QMenu::addAction(menu->menuAction());
    return menu;
}

ElaMenu* ElaMenu::addMenu(ElaIconType::IconName icon, const QString& title)
{
    ElaMenu* menu = new ElaMenu(title, this);
    QMenu::addAction(menu->menuAction());
    menu->menuAction()->setProperty("ElaIconType", QChar((unsigned short)icon));
    return menu;
}

QAction* ElaMenu::addElaIconAction(ElaIconType::IconName icon, const QString& text)
{
    QAction* action = new QAction(text, this);
    action->setProperty("ElaIconType", QChar((unsigned short)icon));
    QMenu::addAction(action);
    return action;
}

QAction* ElaMenu::addElaIconAction(ElaIconType::IconName icon, const QString& text, const QKeySequence& shortcut)
{
    QAction* action = new QAction(text, this);
    action->setShortcut(shortcut);
    action->setProperty("ElaIconType", QChar((unsigned short)icon));
    QMenu::addAction(action);
    return action;
}

QAction* ElaMenu::addCheckBox(const QString& text, bool& isChecked)
{
    return addCheckBox(text, &isChecked);
}

QAction* ElaMenu::addCheckBox(const QString& text, bool* isChecked)
{
    QWidgetAction* action = new QWidgetAction(this);
    QWidget* widget = new QWidget(this);
    widget->setFixedHeight(getMenuItemHeight());
    widget->setObjectName("ElaMenuCheckBoxItem");
    widget->setStyleSheet("#ElaMenuCheckBoxItem:hover{background-color:rgba(0, 0, 0, 0.1);}"); // 简单模拟，实际受Style控制
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(10, 0, 10, 0);
    ElaCheckBox* checkBox = new ElaCheckBox(text, widget);
    // checkBox->setFixedHeight(20); 
    QFont font = checkBox->font();
    font.setPixelSize(13);
    checkBox->setFont(font);
    if (isChecked)
    {
        checkBox->setChecked(*isChecked);
        connect(checkBox, &ElaCheckBox::clicked, this, [this, isChecked, checkBox]() {
            *isChecked = checkBox->isChecked();
            Q_EMIT pCheckBoxClicked(checkBox->text(), checkBox->isChecked());
        });
    }
    // 拦截点击事件防止菜单关闭
    checkBox->installEventFilter(this);
    layout->addWidget(checkBox);
    action->setDefaultWidget(widget);
    QMenu::addAction(action);
    return action;
}

bool ElaMenu::isHasChildMenu() const
{
    QList<QAction*> actionList = this->actions();
    for (auto action: actionList)
    {
        if (action->isSeparator())
        {
            continue;
        }
        if (action->menu())
        {
            return true;
        }
    }
    return false;
}

bool ElaMenu::isHasIcon() const
{
    QList<QAction*> actionList = this->actions();
    for (auto action: actionList)
    {
        if (action->isSeparator())
        {
            continue;
        }
        QMenu* menu = action->menu();
        if (menu && (!menu->icon().isNull() || !menu->property("ElaIconType").toString().isEmpty()))
        {
            return true;
        }
        if (!action->icon().isNull() || !action->property("ElaIconType").toString().isEmpty())
        {
            return true;
        }
    }
    return false;
}

void ElaMenu::showEvent(QShowEvent* event)
{
    Q_EMIT menuShow();
    Q_D(ElaMenu);
    //消除阴影偏移
    move(this->pos().x() - 6, this->pos().y());
    updateGeometry();
    if (!d->_animationPix.isNull())
    {
        d->_animationPix = QPixmap();
    }
    d->_animationPix = this->grab(this->rect());
    QPropertyAnimation* posAnimation = new QPropertyAnimation(d, "pAnimationImagePosY");
    connect(posAnimation, &QPropertyAnimation::finished, this, [=]() {
        d->_animationPix = QPixmap();
        update();
    });
    connect(posAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
        update();
    });
    posAnimation->setEasingCurve(QEasingCurve::OutCubic);
    posAnimation->setDuration(400);
    int targetPosY = height();
    if (targetPosY > 160)
    {
        if (targetPosY < 320)
        {
            targetPosY = 160;
        }
        else
        {
            targetPosY /= 2;
        }
    }

    if (pos().y() + d->_menuStyle->getMenuItemHeight() + 9 >= QCursor::pos().y())
    {
        posAnimation->setStartValue(-targetPosY);
    }
    else
    {
        posAnimation->setStartValue(targetPosY);
    }

    posAnimation->setEndValue(0);
    posAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    // 解决嵌入式Widget动画重影问题
    QList<QWidget*> widgets = findChildren<QWidget*>();
    for (auto widget : widgets)
    {
        if (widget->parent() == this && widget->inherits("QWidget") && !widget->inherits("ElaMenu"))
        {
            widget->hide();
        }
    }
    connect(posAnimation, &QPropertyAnimation::finished, this, [=]() {
        QList<QWidget*> widgets = findChildren<QWidget*>();
        for (auto widget : widgets)
        {
            if (widget->parent() == this && widget->inherits("QWidget") && !widget->inherits("ElaMenu"))
            {
                widget->show();
            }
        }
    });

    QMenu::showEvent(event);
}

void ElaMenu::paintEvent(QPaintEvent* event)
{
    Q_D(ElaMenu);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    if (!d->_animationPix.isNull())
    {
        painter.drawPixmap(QRect(0, d->_pAnimationImagePosY, width(), height()), d->_animationPix);
    }
    else
    {
        QMenu::paintEvent(event);
    }
}
