/************************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                                       *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>                  *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#ifndef SFLPHONEENGINE_H
#define SFLPHONEENGINE_H

//Base
#include "../SortableDockCommon.h"
#include <Plasma/DataEngine>

//Qt
#include <QHash>

//KDE
namespace Plasma {
   class Service;
}

//SFLPhone
#include "../../lib/CallModel.h"

//Typedef
typedef QHash<QString,QVariant>                 HashStringString;
typedef QHash<QString,QHash<QString,QVariant> > ContactHash     ;
class Call;

///SFLPhoneEngine: SFLPhone KDE plasma dataengine
class SFLPhoneEngine : public Plasma::DataEngine,public SortableDockCommon<>
{
   Q_OBJECT

   public:
      //Constructor
      SFLPhoneEngine(QObject* parent, const QVariantList& args);
      ~SFLPhoneEngine() {};

      //Getter
      Plasma::Service*    serviceForSource (const QString &source)       ;
      virtual QStringList sources          (                     ) const ;
      static CallModel<>* getModel         (                     )       ;

      //Friends
      friend class SFLPhoneService;


   protected:
      //Reimplementation
      bool sourceRequestEvent(const QString& name   );
      bool updateSourceEvent (const QString& source );


   private:
      //Attributes
      static CallModel<>*  m_pModel   ;
      ContactHash          m_hContacts;

      //Getter
      QString getCallStateName(call_state state);

      //Callback
      void updateHistory        ();
      void updateCallList       ();
      void updateAccounts       ();
      void updateConferenceList ();
      void updateContacts       ();
      void updateBookmarkList   ();
      void updateInfo           ();

      //Mutator
      void generateNumberList(QString name);


   private slots:
      //Slots
      void updateCollection        (                                                  );
      void callStateChangedSignal  (Call* call                                        );
      void incomingCallSignal      (Call* conf                                        );
      void incomingMessageSignal   ( const QString& accountId, const QString& message );
      void voiceMailNotifySignal   ( const QString& accountId, int count              );
};

#endif // SFLPHONEENGINE_H
