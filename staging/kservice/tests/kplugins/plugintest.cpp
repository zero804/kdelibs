/******************************************************************************
*   Copyright 2013 Sebastian Kügler <sebas@kde.org>                           *
*                                                                             *
*   This library is free software; you can redistribute it and/or             *
*   modify it under the terms of the GNU Library General Public               *
*   License as published by the Free Software Foundation; either              *
*   version 2 of the License, or (at your option) any later version.          *
*                                                                             *
*   This library is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          *
*   Library General Public License for more details.                          *
*                                                                             *
*   You should have received a copy of the GNU Library General Public License *
*   along with this library; see the file COPYING.LIB.  If not, write to      *
*   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
*   Boston, MA 02110-1301, USA.                                               *
*******************************************************************************/

#include "plugintest.h"

#include <kqpluginfactory.h>
#include <klocalizedstring.h>


#include <qcommandlineparser.h>
#include <QDir>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTimer>
#include <QPluginLoader>

#include <QDebug>

#include <iostream>
#include <iomanip>


class PluginTestPrivate {
public:
    QString pluginName;
    QCommandLineParser *parser;
};

PluginTest::PluginTest(int& argc, char** argv, QCommandLineParser *parser) :
    QCoreApplication(argc, argv)
{
    d = new PluginTestPrivate;
    d->parser = parser;
    QTimer::singleShot(0, this, SLOT(runMain()));
}

PluginTest::~PluginTest()
{
    delete d;
}

void PluginTest::runMain()
{
    qDebug() << " libs are in: " << QCoreApplication::libraryPaths();
    qDebug() << "plugin test runs: ";
    loadKQPlugin();
    exit(0);
}

void PluginTest::loadKQPlugin()
{
    qDebug() << "Load KQPlugin";
    QString pluginPath = "/home/sebas/kf5/install/lib/x86_64-linux-gnu/kplugins/";
    QCoreApplication::addLibraryPath(pluginPath);
    QPluginLoader loader("/home/sebas/kf5/install/lib/x86_64-linux-gnu/kplugins/libkqpluginfactory.so", this);
    KQPluginFactory *factory = qobject_cast<KQPluginFactory*>(loader.instance());
    //QObject *factory = loader.instance();
    if (factory) {
        qDebug() << "loaded successfully and cast";
        qDebug() << "metadata: " << loader.metaData();
        QObject *o = factory->createInstance<QObject>(this, QVariantList() << "Peter");
        qDebug() << " objec name:" << o->objectName();

    } else {
        qDebug() << "loading failed somehow";
    }
    //KQPluginFactory* factory = new KQPluginFactory(KPluginInfo(), this);

}

#include "moc_plugintest.cpp"
