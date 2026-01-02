// SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "hostnameutil.h"

#include "plasmasetup_hostnameutil_debug.h"

#include <KLocalizedString>

#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QRegularExpression>
#include <QSysInfo>
#include <QVariant>

namespace
{
enum class HostnameValidationResult {
    Valid,
    Empty,
    TooLong,
    LeadingDot,
    TrailingDot,
    ConsecutiveDots,
    EmptyLabel,
    LabelTooLong,
    InvalidCharacters,
};

const QRegularExpression VALID_HOSTNAME_REGEX(QStringLiteral("^[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?$"));
constexpr int MAX_HOSTNAME_LENGTH = 253;
constexpr int MAX_LABEL_LENGTH = 63;

HostnameValidationResult validateHostname(const QString &hostname)
{
    const QString trimmed = hostname.trimmed();

    if (trimmed.isEmpty()) {
        return HostnameValidationResult::Empty;
    }

    if (trimmed.size() > MAX_HOSTNAME_LENGTH) {
        return HostnameValidationResult::TooLong;
    }

    if (trimmed.startsWith(u'.')) {
        return HostnameValidationResult::LeadingDot;
    }

    if (trimmed.endsWith(u'.')) {
        return HostnameValidationResult::TrailingDot;
    }

    if (trimmed.contains(QStringLiteral(".."))) {
        return HostnameValidationResult::ConsecutiveDots;
    }

    const auto labels = trimmed.split(u'.');
    for (const QString &label : labels) {
        if (label.isEmpty()) {
            return HostnameValidationResult::EmptyLabel;
        }

        if (label.size() > MAX_LABEL_LENGTH) {
            return HostnameValidationResult::LabelTooLong;
        }

        if (!VALID_HOSTNAME_REGEX.match(label).hasMatch()) {
            return HostnameValidationResult::InvalidCharacters;
        }
    }

    return HostnameValidationResult::Valid;
}

QString hostnameValidationMessageForResult(HostnameValidationResult result)
{
    switch (result) {
    case HostnameValidationResult::Valid:
        return QString();
    case HostnameValidationResult::Empty:
        return i18nc("@info", "Hostname cannot be empty.");
    case HostnameValidationResult::TooLong:
        return i18nc("@info", "Hostname is too long (maximum 253 characters).");
    case HostnameValidationResult::LeadingDot:
        return i18nc("@info", "Hostname cannot start with a dot.");
    case HostnameValidationResult::TrailingDot:
        return i18nc("@info", "Hostname cannot end with a dot.");
    case HostnameValidationResult::ConsecutiveDots:
        return i18nc("@info", "Hostname cannot contain consecutive dots.");
    case HostnameValidationResult::EmptyLabel:
        return i18nc("@info", "Hostname labels cannot be empty.");
    case HostnameValidationResult::LabelTooLong:
        return i18nc("@info", "Each hostname label must be at most 63 characters.");
    case HostnameValidationResult::InvalidCharacters:
        return i18nc("@info", "Hostnames may contain letters, numbers, and hyphens. Each label must start and end with a letter or number.");
    }

    return QString();
}
} // namespace

HostnameUtil::HostnameUtil(QObject *parent)
    : QObject(parent)
    , m_dbusInterface(new OrgFreedesktopHostname1Interface(QStringLiteral("org.freedesktop.hostname1"),
                                                           QStringLiteral("/org/freedesktop/hostname1"),
                                                           QDBusConnection::systemBus(),
                                                           this))
{
    loadHostname();
}

QString HostnameUtil::hostname() const
{
    return m_hostname;
}

bool HostnameUtil::hostnameIsDefault() const
{
    if (m_hostname.startsWith(QStringLiteral("localhost"))) {
        return true;
    }

    QString defaultHostname = m_dbusInterface->defaultHostname();
    return m_hostname == defaultHostname;
}

void HostnameUtil::setHostname(const QString &hostname)
{
    qCInfo(PlasmaSetupHostnameUtil) << "Setting hostname to:" << hostname;

    const QString trimmed = hostname.trimmed();
    if (trimmed == m_hostname) {
        return;
    }

    const HostnameValidationResult validationResult = validateHostname(trimmed);
    if (validationResult != HostnameValidationResult::Valid) {
        qCWarning(PlasmaSetupHostnameUtil) << "Rejected invalid hostname" << trimmed << hostnameValidationMessage(trimmed);
        return;
    }

    m_hostname = trimmed;
    Q_EMIT hostnameChanged();

    setHostnameOnSystem(trimmed);
}

bool HostnameUtil::isHostnameValid(const QString &hostname) const
{
    return validateHostname(hostname.trimmed()) == HostnameValidationResult::Valid;
}

QString HostnameUtil::hostnameValidationMessage(const QString &hostname) const
{
    const HostnameValidationResult result = validateHostname(hostname.trimmed());
    return hostnameValidationMessageForResult(result);
}

void HostnameUtil::loadHostname()
{
    QString hostname = readHostnameViaDBus(QStringLiteral("StaticHostname"));
    if (hostname.isEmpty()) {
        hostname = readHostnameViaDBus(QStringLiteral("Hostname"));
    }
    if (hostname.isEmpty()) {
        hostname = QSysInfo::machineHostName();
    }

    hostname = hostname.trimmed();

    if (hostname != m_hostname) {
        m_hostname = hostname;
        Q_EMIT hostnameChanged();
    }
}

QString HostnameUtil::readHostnameViaDBus(const QString &propertyName)
{
    QString value;

    if (propertyName == QLatin1String("StaticHostname")) {
        value = m_dbusInterface->staticHostname();
    } else if (propertyName == QLatin1String("Hostname")) {
        value = m_dbusInterface->hostname();
    } else {
        qCWarning(PlasmaSetupHostnameUtil) << "Unknown property name requested:" << propertyName;
        return {};
    }

    return value;
}

void HostnameUtil::setHostnameOnSystem(const QString &hostname)
{
    setStaticHostname(hostname);
    setTransientHostname(hostname);
}

void HostnameUtil::setStaticHostname(const QString &hostname)
{
    const bool interactive = false;
    QDBusPendingReply<> reply = m_dbusInterface->SetStaticHostname(hostname, interactive);
    QDBusPendingCallWatcher *staticWatcher = new QDBusPendingCallWatcher(reply, this);
    connect(staticWatcher, &QDBusPendingCallWatcher::finished, [hostname](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();

        if (watcher->isError()) {
            qCWarning(PlasmaSetupHostnameUtil) << "Failed to set static hostname:" << hostname << ";" << watcher->error().message();
            return;
        }

        qCInfo(PlasmaSetupHostnameUtil) << "Successfully set static hostname.";
    });
}

void HostnameUtil::setTransientHostname(const QString &hostname)
{
    const bool interactive = false;
    QDBusPendingReply<> reply = m_dbusInterface->SetHostname(hostname, interactive);
    QDBusPendingCallWatcher *transientWatcher = new QDBusPendingCallWatcher(reply, this);
    connect(transientWatcher, &QDBusPendingCallWatcher::finished, [hostname](QDBusPendingCallWatcher *watcher) {
        watcher->deleteLater();

        if (watcher->isError()) {
            qCWarning(PlasmaSetupHostnameUtil) << "Failed to set transient hostname:" << hostname << ";" << watcher->error().message();
            return;
        }

        qCInfo(PlasmaSetupHostnameUtil) << "Successfully set transient hostname.";
    });
}

#include "moc_hostnameutil.cpp"
