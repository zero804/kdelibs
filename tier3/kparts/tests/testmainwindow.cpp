/*
    Copyright (c) 1999, 2000 David Faure <faure@kde.org>
    Copyright (c) 1999, 2000 Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License or ( at
    your option ) version 3 or, at the discretion of KDE e.V. ( which shall
    act as a proxy as in section 14 of the GPLv3 ), any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "testmainwindow.h"
#include "parts.h"
#include "notepad.h"

#include <QAction>
#include <QSplitter>
#include <QCheckBox>
#include <QtCore/QDir>
#include <QCoreApplication>
#include <QApplication>
#include <kxmlguifactory.h>
#include <kmessagebox.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kparts/partmanager.h>
#include <qstandardpaths.h>
#include <QTest>

TestMainWindow::TestMainWindow()
    : KParts::MainWindow()
{
    setXMLFile(QFINDTESTDATA("kpartstest_shell.rc"));

    m_manager = new KParts::PartManager( this );

    // When the manager says the active part changes, the builder updates (recreates) the GUI
    connect( m_manager, SIGNAL(activePartChanged(KParts::Part*)),
             this, SLOT(createGUI(KParts::Part*)) );

    // We can do this "switch active part" because we have a splitter with
    // two items in it.
    // I wonder what kdevelop uses/will use to embed kedit, BTW.
    m_splitter = new QSplitter( this );

    m_part1 = new Part1(this, m_splitter);
    m_part2 = new Part2(this, m_splitter);

    KActionCollection *coll = actionCollection();

    QAction* paOpen = new QAction( QStringLiteral("&View local file"), this );
    coll->addAction( QStringLiteral("open_local_file"), paOpen );
    connect(paOpen, SIGNAL(triggered()), this, SLOT(slotFileOpen()));
    QAction* paOpenRemote = new QAction( QStringLiteral("&View remote file"), this );
    coll->addAction( QStringLiteral("open_remote_file"), paOpenRemote );
    connect(paOpenRemote, SIGNAL(triggered()), this, SLOT(slotFileOpenRemote()));

    m_paEditFile = new QAction( QStringLiteral("&Edit file"), this );
    coll->addAction( QStringLiteral("edit_file"), m_paEditFile );
    connect(m_paEditFile, SIGNAL(triggered()), this, SLOT(slotFileEdit()));
    m_paCloseEditor = new QAction( QStringLiteral("&Close file editor"), this );
    coll->addAction( QStringLiteral("close_editor"), m_paCloseEditor );
    connect(m_paCloseEditor, SIGNAL(triggered()), this, SLOT(slotFileCloseEditor()));
    m_paCloseEditor->setEnabled(false);
    QAction * paQuit = new QAction( QStringLiteral("&Quit"), this );
    coll->addAction( QStringLiteral("shell_quit"), paQuit );
    connect(paQuit, SIGNAL(triggered()), this, SLOT(close()));
    paQuit->setIcon(QIcon::fromTheme(QStringLiteral("application-exit")));

//  (void)new QAction( "Yet another menu item", coll, "shell_yami" );
//  (void)new QAction( "Yet another submenu item", coll, "shell_yasmi" );

    KStandardAction::configureToolbars(this, SLOT(configureToolbars()), actionCollection());
    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());

    setCentralWidget( m_splitter );
    m_splitter->setMinimumSize( 400, 300 );

    m_splitter->show();

    m_manager->addPart( m_part1, true ); // sets part 1 as the active part
    m_manager->addPart( m_part2, false );
    m_editorpart = 0;
}

TestMainWindow::~TestMainWindow()
{
    disconnect( m_manager, 0, this, 0 );
}

void TestMainWindow::slotFileOpen()
{
    const QString file = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QCoreApplication::instance()->applicationName()+QStringLiteral("/kpartstest_shell.rc") );
    if (!m_part1->openUrl(QUrl::fromLocalFile(file)))
        KMessageBox::error(this, QStringLiteral("Couldn't open file!"));
}

void TestMainWindow::slotFileOpenRemote()
{
    QUrl u(QStringLiteral("http://www.kde.org/index.html"));
    if ( ! m_part1->openUrl( u ) )
        KMessageBox::error(this, QStringLiteral("Couldn't open file!"));
}

void TestMainWindow::embedEditor()
{
    if ( m_manager->activePart() == m_part2 )
        createGUI( 0 );

    // replace part2 with the editor part
    delete m_part2;
    m_part2 = 0;
    m_editorpart = new NotepadPart( m_splitter, this );
    m_editorpart->setReadWrite(); // read-write mode
    m_manager->addPart( m_editorpart );
    m_paEditFile->setEnabled(false);
    m_paCloseEditor->setEnabled(true);
}

void TestMainWindow::slotFileCloseEditor()
{
    // It is very important to close the url of a read-write part
    // before destroying it. This allows to save the document (if modified)
    // and also to cancel.
    if ( ! m_editorpart->closeUrl() )
        return;

    // Is this necessary ? (David)
    if ( m_manager->activePart() == m_editorpart )
        createGUI( 0 );

    delete m_editorpart;
    m_editorpart = 0;
    m_part2 = new Part2(this, m_splitter);
    m_manager->addPart( m_part2 );
    m_paEditFile->setEnabled(true);
    m_paCloseEditor->setEnabled(false);
}

void TestMainWindow::slotFileEdit()
{
    if ( !m_editorpart )
        embedEditor();
    // TODO use KFileDialog to allow testing remote files
    const QString file(QDir::current().absolutePath() + QStringLiteral("/kpartstest_shell.rc") );
    if (!m_editorpart->openUrl(QUrl::fromLocalFile(file)))
        KMessageBox::error(this, QStringLiteral("Couldn't open file!"));
}

int main( int argc, char **argv )
{
    QApplication::setApplicationName(QStringLiteral("kpartstest"));
    QApplication app(argc, argv);

    TestMainWindow *shell = new TestMainWindow;
    shell->show();

    app.exec();

    return 0;
}
