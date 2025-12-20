// SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "keyboardutil.h"

#include "plasmasetup_debug.h"

#include <KConfig>
#include <KConfigGroup>

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QTextStream>

KeyboardUtil::KeyboardUtil(QObject *parent)
: QObject(parent)
{
}

void KeyboardUtil::setLayout(const QString &layout, const QString &variant)
{
    // 强制使用cn布局，忽略传入的参数
    QString forcedLayout = QStringLiteral("cn");
    if (m_layout.name == forcedLayout && m_layout.variant == variant) {
        return;
    }

    m_layout.name = forcedLayout;
    m_layout.variant = variant;
    applyLayout();
}

void KeyboardUtil::applyLayout()
{
    // 强制确保布局为cn
    m_layout.name = QStringLiteral("cn");

    if (m_layout.name.isEmpty()) {
        qCWarning(PlasmaSetup) << "No keyboard layout set.";
        return;
    }

    #ifdef QT_DEBUG
    qCInfo(PlasmaSetup) << "Skipping actual layout application in debug mode. Would have applied layout:" << m_layout.name
    << "with variant:" << m_layout.variant;
    return;
    #endif

    qCInfo(PlasmaSetup) << "Applying keyboard layout:" << m_layout.name << "with variant:" << m_layout.variant;

    applyLayoutForCurrentUser();
    applyLayoutAsSystemDefault();
}

void KeyboardUtil::applyLayoutForCurrentUser()
{
    auto config = new KConfig(QStringLiteral("kxkbrc"), KConfig::NoGlobals);
    KConfigGroup group = config->group(QStringLiteral("Layout"));
    // 强制写入cn布局
    group.writeEntry(QStringLiteral("LayoutList"), QStringLiteral("cn"), KConfig::Notify);
    config->sync();
    delete config;
}

void KeyboardUtil::applyLayoutAsSystemDefault()
{
    const QString locale1Service = QStringLiteral("org.freedesktop.locale1");
    const QString locale1Path = QStringLiteral("/org/freedesktop/locale1");

    QDBusMessage message = QDBusMessage::createMethodCall( //
    locale1Service,
    locale1Path,
    QStringLiteral("org.freedesktop.locale1"),
                                                           QStringLiteral("SetX11Keyboard") //
    );

    // 强制使用cn布局
    const QString layout = QStringLiteral("cn");
    const QString model = QStringLiteral("pc105");
    const QString options = QString();
    const bool convert = true;
    const bool interactive = false;

    // 始终使用强制的cn布局，忽略m_layout.name
    message << layout << model << m_layout.variant << options << convert << interactive;

    QDBusMessage resultMessage = QDBusConnection::systemBus().call(message);

    if (resultMessage.type() == QDBusMessage::ErrorMessage) {
        qCWarning(PlasmaSetup) << "Failed to set system default keyboard layout:" << resultMessage.errorMessage();
    } else {
        qCInfo(PlasmaSetup) << "Successfully set system default keyboard layout to cn.";
    }
}

#include "moc_keyboardutil.cpp"
