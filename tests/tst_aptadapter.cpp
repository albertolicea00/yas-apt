#include <QTest>

#include "aptadapter.h"

class TestAptAdapter : public QObject {
    Q_OBJECT
private slots:
    void searchParsesNameDashDescription()
    {
        AptAdapter adapter;
        const auto packages = adapter.parseSearch(QStringLiteral(
            "curl - command line tool for transferring data with URL syntax\n"
            "curlftpfs - filesystem to access FTP hosts based on FUSE and cURL\n"));
        QCOMPARE(packages.size(), 2);
        QCOMPARE(packages.at(0).id, QStringLiteral("curl"));
        QVERIFY(packages.at(0).description.startsWith(QStringLiteral("command line")));
    }

    void installedParsesDpkgQueryTabs()
    {
        AptAdapter adapter;
        const auto packages = adapter.parseInstalled(QStringLiteral(
            "curl\t8.5.0-2ubuntu10\tcommand line tool\n"
            "git\t1:2.43.0-1ubuntu7\tfast version control\n"));
        QCOMPARE(packages.size(), 2);
        QCOMPARE(packages.at(1).id, QStringLiteral("git"));
        QCOMPARE(packages.at(1).installedVersion, QStringLiteral("1:2.43.0-1ubuntu7"));
        QVERIFY(packages.at(1).installed());
    }

    void upgradableParsesAptList()
    {
        AptAdapter adapter;
        const auto packages = adapter.parseOutdated(QStringLiteral(
            "Listing... Done\n"
            "curl/noble-updates 8.5.0-2ubuntu10.6 amd64 [upgradable from: 8.5.0-2ubuntu10]\n"));
        QCOMPARE(packages.size(), 1);
        QCOMPARE(packages.at(0).id, QStringLiteral("curl"));
        QCOMPARE(packages.at(0).version, QStringLiteral("8.5.0-2ubuntu10.6"));
        QCOMPARE(packages.at(0).installedVersion, QStringLiteral("8.5.0-2ubuntu10"));
        QVERIFY(packages.at(0).outdated());
    }

    void mutationsUsePkexecWithNoninteractiveFrontend()
    {
        AptAdapter adapter;
        const auto cmd = adapter.installCommand("curl", "");
        QCOMPARE(cmd.program, QStringLiteral("pkexec"));
        QVERIFY(cmd.arguments.contains(QStringLiteral("DEBIAN_FRONTEND=noninteractive")));
        QVERIFY(cmd.arguments.contains(QStringLiteral("-y")));
        // reads stay unprivileged
        QCOMPARE(adapter.searchCommand("x").program, QStringLiteral("apt-cache"));
    }
};

QTEST_MAIN(TestAptAdapter)
#include "tst_aptadapter.moc"
