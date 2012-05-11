// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QCloseEvent>

class AboutDlg;
class Renderer;

/*! \class MainWindow
 *  \brief The main window of the game.
 */
class MainWindow : public QWidget
{
    Q_OBJECT

public:

    //! Constructor
    MainWindow();

    //! Return the singleton MainWindow.
    static MainWindow * instance();

    //! Run the game
    void runGame();

signals:

    void closed();

protected:

    //! \reimp
    void closeEvent(QCloseEvent * event);

private:

    static MainWindow * m_instance;

    AboutDlg * m_aboutDlg;
};

#endif // MAINWINDOW_HPP

