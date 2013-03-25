/****************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef HISTORYDELEGATE_H
#define HISTORYDELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QTreeView;

class HistoryDelegate : public QStyledItemDelegate
{
public:
   HistoryDelegate(QTreeView* parent = nullptr);

   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
   QTreeView* m_pParent;
};

#endif
