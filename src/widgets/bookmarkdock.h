/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef BOOKMARK_DOCK_H
#define BOOKMARK_DOCK_H

#include <QtGui/QDockWidget>

//Qt
class QSplitter;
class QCheckBox;

//KDE
class KLineEdit;

//SFLPhone
class HistoryTreeItem;
class CategorizedTreeWidget;

//Typedef
typedef QList<HistoryTreeItem*> BookmarkList;

///BookmarkDock: Dock for managing favorite contacts
class BookmarkDock : public QDockWidget {
   Q_OBJECT
public:
   //Constructors
   explicit BookmarkDock(QWidget* parent = nullptr);
   virtual ~BookmarkDock();

   //Mutators
   void addBookmark(const QString& phone);
   void removeBookmark(const QString& phone);
private:
   //Attributes
   CategorizedTreeWidget*  m_pItemView  ;
   KLineEdit*              m_pFilterLE  ;
   QSplitter*              m_pSplitter  ;
   BookmarkList            m_pBookmark  ;
   QCheckBox*              m_pMostUsedCK;

   //Mutators
   void addBookmark_internal(const QString& phone);
private Q_SLOTS:
   void filter(QString text);
   void reload();
};

#endif
