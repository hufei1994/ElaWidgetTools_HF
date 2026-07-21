#include "ElaMenu.h"

#include <QApplication>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>

#include "DeveloperComponents/ElaMenuStyle.h"
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
    // 延长 Style 生命周期，确保 QMenu 析构并释放 QWidgetAction 时继承的样式仍然有效。
    d->_menuStyle->setParent(this);
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
    // ElaMenuStyle 由 QObject 父子关系在基类完成 QWidgetAction 清理后自动释放。
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

QAction* ElaMenu::addCheckableAction(const QString& text, bool* isChecked)
{
    QAction* action = new QAction(text, this);
    action->setCheckable(true);
    action->setProperty("ElaMenuKeepOpenOnTrigger", true);
    if (isChecked)
    {
        action->setChecked(*isChecked);
    }
    connect(action, &QAction::toggled, this, [this, action, isChecked](bool checked) {
        if (isChecked)
        {
            *isChecked = checked;
        }
        Q_EMIT pCheckableActionToggled(action, checked);
    });
    QMenu::addAction(action);
    return action;
}

void ElaMenu::mouseReleaseEvent(QMouseEvent* event)
{
    QAction* action = actionAt(event->pos());
    if (!action || event->button() != Qt::LeftButton ||
        !action->property("ElaMenuKeepOpenOnTrigger").toBool() || !action->isEnabled())
    {
        QMenu::mouseReleaseEvent(event);
        return;
    }

    // 统一走 QAction 的原生触发路径：独占组保持单选，未分组动作仍可自由多选。
    setActiveAction(action);
    action->trigger();
    update(actionGeometry(action));
    event->accept();
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
