/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
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
#include "abstractbookmarkmodel.h"

//Qt
#include <QtCore/QMimeData>

//SFLPhone
#include "historymodel.h"
#include "dbus/presencemanager.h"

//Model item/index
class NumberTreeBackend : public HistoryTreeBackend, public QObject
{
   friend class AbstractBookmarkModel;
   public:
      NumberTreeBackend(QString name): HistoryTreeBackend(HistoryTreeBackend::Type::BOOKMARK),m_Name(name),m_IsMostPopular(false){}
      virtual QObject* getSelf() { return this; }

   private:
      QString  m_Name         ;
      bool     m_IsMostPopular;
      Contact* m_pContact     ;
};

AbstractBookmarkModel::AbstractBookmarkModel(QObject* parent) : QAbstractItemModel(parent){
   setObjectName("AbstractBookmarkModel");
   reloadCategories();
   m_lMimes << MIME_PLAIN_TEXT << MIME_PHONENUMBER;

   //Connect
   connect(&DBus::PresenceManager::instance(),SIGNAL(newServerSubscriptionRequest(QString)),this,SLOT(slotRequest(QString)));
   connect(&DBus::PresenceManager::instance(),SIGNAL(newClientSubscription(QString,bool,QString)),
      this,SLOT(slotIncomingNotifications(QString,bool,QString)));
}


///Reload bookmark cateogries
void AbstractBookmarkModel::reloadCategories()
{
   beginResetModel();
   m_hCategories.clear();
   foreach(TopLevelItem* item, m_lCategoryCounter) {
      foreach (NumberTreeBackend* child, item->m_lChildren) {
         delete child;
      }
      delete item;
   }
   m_lCategoryCounter.clear();
   m_isContactDateInit = false;

   //Load most used contacts
   if (displayFrequentlyUsed()) {
      TopLevelItem* item = new TopLevelItem("Most popular");
      m_hCategories["mp"] = item;
      m_lCategoryCounter << item;
      const QStringList cl = HistoryModel::getNumbersByPopularity();
      for (int i=0;i<((cl.size()>=10)?10:cl.size());i++) {
         NumberTreeBackend* bm = new NumberTreeBackend(cl[i]);
         bm->m_IsMostPopular = true;
         item->m_lChildren << bm;
      }
   }

   foreach(const QString& bookmark, bookmarkList()) {
      NumberTreeBackend* bm = new NumberTreeBackend(bookmark);
      const QString val = category(bm);
      if (!m_hCategories[val]) {
         TopLevelItem* item = new TopLevelItem(val);
         m_hCategories[val] = item;
         m_lCategoryCounter << item;
      }
      TopLevelItem* item = m_hCategories[val];
      if (item) {
         item->m_lChildren << bm;
      }
      else
         qDebug() << "ERROR count";
   }
   endResetModel();
   emit layoutChanged();
   emit dataChanged(index(0,0),index(rowCount()-1,0));
} //reloadCategories

//Do nothing
bool AbstractBookmarkModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

///Get bookmark model data HistoryTreeBackend::Type and Call::Role
QVariant AbstractBookmarkModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();

   HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(index.internalPointer());
   if (!modelItem)
      return QVariant();
   switch (modelItem->type()) {
      case HistoryTreeBackend::Type::TOP_LEVEL:
         switch (role) {
            case Qt::DisplayRole:
               return static_cast<TopLevelItem*>(modelItem)->m_Name;
         }
         break;
      case HistoryTreeBackend::Type::CALL:
      case HistoryTreeBackend::Type::BOOKMARK:
         return commonCallInfo(m_lCategoryCounter[index.parent().row()]->m_lChildren[index.row()],role);
         break;
      case HistoryTreeBackend::Type::NUMBER:
         break;
   };
   return QVariant();
} //Data

///Get header data
QVariant AbstractBookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant("Contacts");
   return QVariant();
}


///Get the number of child of "parent"
int AbstractBookmarkModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid() || !parent.internalPointer())
      return m_lCategoryCounter.size();
   else if (!parent.parent().isValid()) {
      return m_lCategoryCounter[parent.row()]->m_lChildren.size();
   }
   return 0;
}

Qt::ItemFlags AbstractBookmarkModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable | (index.parent().isValid()?Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled:Qt::ItemIsEnabled);
}

///There is only 1 column
int AbstractBookmarkModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

///Get the bookmark parent
QModelIndex AbstractBookmarkModel::parent( const QModelIndex& index) const
{
   if (!index.isValid() || !index.internalPointer()) {
      return QModelIndex();
   }
   const HistoryTreeBackend* modelItem = static_cast<HistoryTreeBackend*>(index.internalPointer());
   if (modelItem->type() == HistoryTreeBackend::Type::BOOKMARK) {
      const QString val = category(static_cast<NumberTreeBackend*>(index.internalPointer()));
      if (static_cast<const NumberTreeBackend*>(modelItem)->m_IsMostPopular)
         return AbstractBookmarkModel::index(0,0);
      else if (m_hCategories[val])
         return AbstractBookmarkModel::index(m_lCategoryCounter.indexOf(m_hCategories[val]),0);
      else AbstractBookmarkModel::index(0,0);
   }
   return QModelIndex();
} //parent

///Get the index
QModelIndex AbstractBookmarkModel::index(int row, int column, const QModelIndex& parent) const
{
   if (parent.isValid())
      return createIndex(row,column,m_lCategoryCounter[parent.row()]->m_lChildren[row]);
   else {
      return createIndex(row,column,m_lCategoryCounter[row]);
   }
}

///Get bookmarks mime types
QStringList AbstractBookmarkModel::mimeTypes() const
{
   return m_lMimes;
}

///Generate mime data
QMimeData* AbstractBookmarkModel::mimeData(const QModelIndexList &indexes) const
{
   QMimeData *mimeData = new QMimeData();
   foreach (const QModelIndex &index, indexes) {
      if (index.isValid()) {
         QString text = data(index, Call::Role::Number).toString();
         mimeData->setData(MIME_PLAIN_TEXT , text.toUtf8());
         mimeData->setData(MIME_PHONENUMBER, text.toUtf8());
         return mimeData;
      }
   }
   return mimeData;
} //mimeData

///Get call info TODO use Call:: one
QVariant AbstractBookmarkModel::commonCallInfo(NumberTreeBackend* number, int role) const
{
   if (!number)
      return QVariant();
   QVariant cat;
   switch (role) {
      case Qt::DisplayRole:
      case Call::Role::Name:
         cat = number->m_Name;
         break;
      case Call::Role::Number:
         cat = number->m_Name;//call->getPeerPhoneNumber();
         break;
      case Call::Role::Direction:
         cat = 4;//call->getHistoryState();
         break;
      case Call::Role::Date:
         cat = "N/A";//call->getStartTimeStamp();
         break;
      case Call::Role::Length:
         cat = "N/A";//call->getLength();
         break;
      case Call::Role::FormattedDate:
         cat = "N/A";//QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).toString();
         break;
      case Call::Role::HasRecording:
         cat = false;//call->hasRecording();
         break;
      case Call::Role::Historystate:
         cat = Call::HistoryState::NONE;//call->getHistoryState();
         break;
      case Call::Role::Filter:
         cat = number->m_Name;//call->getHistoryState()+'\n'+commonCallInfo(call,Name).toString()+'\n'+commonCallInfo(call,Number).toString();
         break;
      case Call::Role::FuzzyDate:
         cat = "N/A";//timeToHistoryCategory(QDateTime::fromTime_t(call->getStartTimeStamp().toUInt()).date());
         break;
      case Call::Role::IsBookmark:
         cat = true;
         break;
   }
   return cat;
} //commonCallInfo

///Get category
QString AbstractBookmarkModel::category(NumberTreeBackend* number) const
{
   QString cat = commonCallInfo(number).toString();
   if (cat.size())
      cat = cat[0].toUpper();
   return cat;
}

void AbstractBookmarkModel::slotIncomingNotifications(const QString& uri, bool status, const QString& message)
{
   Q_UNUSED(uri)
   Q_UNUSED(status)
   Q_UNUSED(message)
}

void AbstractBookmarkModel::slotRequest(const QString& uri)
{
   Q_UNUSED(uri)
   DBus::PresenceManager::instance().approvePresSubServer(uri,true);
}


void AbstractBookmarkModel::reloadPresence()
{
   foreach(Subscription* item,m_lTracker)
      delete item;
   m_lTracker.clear();
}

