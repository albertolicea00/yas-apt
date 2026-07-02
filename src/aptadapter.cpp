#include "aptadapter.h"

using yas::CliAction;
using yas::CliCommand;
using yas::Package;

// APT adapter. Read operations use apt-cache/dpkg-query (stable scripting
// interfaces; `apt` itself warns its CLI is unstable). Mutations run through
// pkexec (polkit) — never the whole GUI as root. DEBIAN_FRONTEND is passed
// via `pkexec env ...` because pkexec sanitizes the environment.
namespace {

CliCommand root(QStringList aptGetArgs)
{
    return {QStringLiteral("pkexec"),
            QStringList{QStringLiteral("env"),
                        QStringLiteral("DEBIAN_FRONTEND=noninteractive"),
                        QStringLiteral("apt-get")} + aptGetArgs};
}

} // namespace

QString AptAdapter::displayName() const { return QStringLiteral("APT"); }
QString AptAdapter::cliProgram() const { return QStringLiteral("apt-get"); }
QStringList AptAdapter::cliSearchPaths() const
{
    return {QStringLiteral("/usr/bin"), QStringLiteral("/usr/sbin")};
}
QStringList AptAdapter::cliVersionArguments() const { return {QStringLiteral("--version")}; }

CliCommand AptAdapter::searchCommand(const QString &query) const
{
    return {QStringLiteral("apt-cache"), {QStringLiteral("search"), query}};
}

CliCommand AptAdapter::infoCommand(const QString &packageId, const QString &) const
{
    return {QStringLiteral("apt-cache"), {QStringLiteral("show"), packageId}};
}

CliCommand AptAdapter::listInstalledCommand() const
{
    return {QStringLiteral("dpkg-query"),
            {QStringLiteral("-W"),
             QStringLiteral("-f=${Package}\\t${Version}\\t${binary:Summary}\\n")}};
}

CliCommand AptAdapter::listOutdatedCommand() const
{
    return {QStringLiteral("apt"), {QStringLiteral("list"), QStringLiteral("--upgradable")}};
}

CliCommand AptAdapter::installCommand(const QString &packageId, const QString &) const
{
    return root({QStringLiteral("install"), QStringLiteral("-y"), packageId});
}

CliCommand AptAdapter::uninstallCommand(const QString &packageId, const QString &) const
{
    return root({QStringLiteral("remove"), QStringLiteral("-y"), packageId});
}

CliCommand AptAdapter::upgradeCommand(const QString &packageId, const QString &) const
{
    return root({QStringLiteral("install"), QStringLiteral("--only-upgrade"),
                 QStringLiteral("-y"), packageId});
}

CliCommand AptAdapter::upgradeAllCommand() const
{
    return root({QStringLiteral("upgrade"), QStringLiteral("-y")});
}

CliCommand AptAdapter::pinCommand(const QString &packageId, const QString &) const
{
    return {QStringLiteral("pkexec"), {QStringLiteral("apt-mark"), QStringLiteral("hold"),
                                       packageId}};
}

CliCommand AptAdapter::unpinCommand(const QString &packageId, const QString &) const
{
    return {QStringLiteral("pkexec"), {QStringLiteral("apt-mark"), QStringLiteral("unhold"),
                                       packageId}};
}

QList<Package> AptAdapter::parseSearch(const QString &stdOut) const
{
    // "name - description" per line.
    QList<Package> result;
    const QStringList lines = stdOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        const qsizetype sep = line.indexOf(QStringLiteral(" - "));
        if (sep <= 0)
            continue;
        Package p;
        p.id = line.left(sep).trimmed();
        p.name = p.id;
        p.description = line.mid(sep + 3).trimmed();
        result.append(p);
    }
    return result;
}

QList<Package> AptAdapter::parseInfo(const QString &stdOut) const
{
    // RFC-822 style stanza from apt-cache show (first stanza only).
    Package p;
    const QStringList lines = stdOut.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        if (line.startsWith(QStringLiteral("Package:")))
            p.id = p.name = line.mid(8).trimmed();
        else if (line.startsWith(QStringLiteral("Version:")) && p.version.isEmpty())
            p.version = line.mid(8).trimmed();
        else if (line.startsWith(QStringLiteral("Homepage:")))
            p.homepage = line.mid(9).trimmed();
        else if (line.startsWith(QStringLiteral("Description-en:"))
                 || line.startsWith(QStringLiteral("Description:"))) {
            p.description = line.section(QLatin1Char(':'), 1).trimmed();
        } else if (line.trimmed().isEmpty() && !p.id.isEmpty()) {
            break; // end of first stanza
        }
    }
    if (p.id.isEmpty())
        return {};
    return {p};
}

QList<Package> AptAdapter::parseInstalled(const QString &stdOut) const
{
    // dpkg-query tab-separated: package \t version \t summary
    QList<Package> result;
    const QStringList lines = stdOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        const QStringList cols = line.split(QLatin1Char('\t'));
        if (cols.size() < 2)
            continue;
        Package p;
        p.id = cols.value(0);
        p.name = cols.value(0);
        p.installedVersion = cols.value(1);
        p.version = cols.value(1);
        p.description = cols.value(2);
        p.kind = QStringLiteral("deb");
        result.append(p);
    }
    return result;
}

QList<Package> AptAdapter::parseOutdated(const QString &stdOut) const
{
    // "name/suite version arch [upgradable from: old]"
    QList<Package> result;
    const QStringList lines = stdOut.split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (!line.contains(QStringLiteral("[upgradable from:")))
            continue;
        Package p;
        p.id = line.section(QLatin1Char('/'), 0, 0).trimmed();
        p.name = p.id;
        p.source = line.section(QLatin1Char('/'), 1).section(QLatin1Char(' '), 0, 0);
        const QStringList tokens = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        p.version = tokens.value(1);
        const qsizetype from = line.indexOf(QStringLiteral("[upgradable from:"));
        p.installedVersion =
            line.mid(from + 17).remove(QLatin1Char(']')).trimmed();
        result.append(p);
    }
    return result;
}

QList<CliAction> AptAdapter::actionCatalog() const
{
    return {
        {QStringLiteral("update"), tr("Update package lists"),
         tr("Fetch the newest package indexes from all sources"),
         root({QStringLiteral("update")}), false, false, true},
        {QStringLiteral("autoremove"), tr("Remove unused dependencies"),
         tr("Uninstall packages that were installed automatically and are no longer needed"),
         root({QStringLiteral("autoremove"), QStringLiteral("-y")}), false, true, true},
        {QStringLiteral("clean"), tr("Clean package cache"),
         tr("Delete all downloaded .deb files from the local cache"),
         root({QStringLiteral("clean")}), false, true, false},
        {QStringLiteral("autoclean"), tr("Clean obsolete cache"),
         tr("Delete only cached .deb files that can no longer be downloaded"),
         root({QStringLiteral("autoclean")}), false, false, false},
        {QStringLiteral("holds"), tr("List holds"),
         tr("Show packages currently held back from upgrades"),
         {QStringLiteral("apt-mark"), {QStringLiteral("showhold")}}, false, false, false},
        {QStringLiteral("depends"), tr("Show dependencies"),
         tr("List the dependencies of a package"),
         {QStringLiteral("apt-cache"), {QStringLiteral("depends")}}, true, false, false},
        {QStringLiteral("rdepends"), tr("Reverse dependencies"),
         tr("List packages that depend on a package"),
         {QStringLiteral("apt-cache"), {QStringLiteral("rdepends")}}, true, false, false},
        {QStringLiteral("policy"), tr("Show version policy"),
         tr("Show installed/candidate versions and their sources"),
         {QStringLiteral("apt-cache"), {QStringLiteral("policy")}}, true, false, false},
    };
}
