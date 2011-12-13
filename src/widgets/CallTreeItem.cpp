/***************************************************************************
 *   Author : Mathieu Leduc-Hamel mathieu.leduc-hamel@savoirfairelinux.com *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/

//Parent
#include "CallTreeItem.h"

//Qt
#include <QtCore/QStringList>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

//KDE
#include <KLocale>
#include <KDebug>
#include <KIcon>

//SFLPhone library
#include "lib/sflphone_const.h"
#include "lib/Contact.h"
#include "lib/Call.h"

//SFLPhone
#include "AkonadiBackend.h"


///Constant
const char * CallTreeItem::callStateIcons[12] = {ICON_INCOMING, ICON_RINGING, ICON_CURRENT, ICON_DIALING, ICON_HOLD, ICON_FAILURE, ICON_BUSY, ICON_TRANSFER, ICON_TRANSF_HOLD, "", "", ICON_CONFERENCE};

///Constructor
CallTreeItem::CallTreeItem(QWidget *parent)
   : QWidget(parent), m_pItemCall(0), m_Init(false)
{
   setMaximumSize(99999,50);
}

///Destructor
CallTreeItem::~CallTreeItem()
{
   
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Return the call item
Call* CallTreeItem::call() const
{
   return m_pItemCall;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Set the call item
void CallTreeItem::setCall(Call *call)
{
   m_pItemCall = call;
   
   if (m_pItemCall->isConference()) {
      if (!m_Init) {
         m_pHistoryPeerL = new QLabel(i18n("Conference"),this);
         m_pIconL = new QLabel("",this);
         QHBoxLayout* mainLayout = new QHBoxLayout();
         mainLayout->addWidget(m_pIconL);
         mainLayout->addWidget(m_pHistoryPeerL);
         setLayout(mainLayout);
         m_Init = true;
      }
      m_pIconL->setPixmap(QPixmap(ICON_CONFERENCE).scaled(QSize(48,48)));
      m_pIconL->setVisible(true);
      m_pHistoryPeerL->setVisible(true);
      return;
   }

   m_pIconL            = new QLabel();
   m_pCallNumberL      = new QLabel(m_pItemCall->getPeerPhoneNumber());
   m_pTransferPrefixL  = new QLabel(i18n("Transfer to : "));
   m_pTransferNumberL  = new QLabel();
   m_pPeerL            = new QLabel();
   QSpacerItem* verticalSpacer = new QSpacerItem(16777215, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
   
   QHBoxLayout* mainLayout = new QHBoxLayout();
   mainLayout->setContentsMargins ( 3, 1, 2, 1);
   
   m_pCodecL = new QLabel(this);
   //m_pCodecL->setText("Codec: "+m_pItemCall->getCurrentCodecName());

   m_pSecureL = new QLabel(this);
   
   mainLayout->setSpacing(4);
   QVBoxLayout* descr = new QVBoxLayout();
   descr->setMargin(1);
   descr->setSpacing(1);
   QHBoxLayout* transfer = new QHBoxLayout();
   transfer->setMargin(0);
   transfer->setSpacing(0);
   mainLayout->addWidget(m_pIconL);
        
   if(! m_pItemCall->getPeerName().isEmpty()) {
      m_pPeerL->setText(m_pItemCall->getPeerName());
      descr->addWidget(m_pPeerL);
   }

   descr->addWidget(m_pCallNumberL);
   descr->addWidget(m_pSecureL);
   descr->addWidget(m_pCodecL);
   transfer->addWidget(m_pTransferPrefixL);
   transfer->addWidget(m_pTransferNumberL);
   descr->addLayout(transfer);
   descr->addItem(verticalSpacer);
   mainLayout->addLayout(descr);
   
   setLayout(mainLayout);
   setMinimumSize(QSize(50, 30));

   connect(m_pItemCall, SIGNAL(changed()), this,     SLOT(updated()));

   updated();
}

///Update data
void CallTreeItem::updated()
{
   qDebug() << "Updating tree item";
   Contact* contact = AkonadiBackend::getInstance()->getContactByPhone(m_pItemCall->getPeerPhoneNumber());
   if (contact) {
      m_pIconL->setPixmap(*contact->getPhoto());
      m_pPeerL->setText("<b>"+contact->getFormattedName()+"</b>");
   }
   else {
      m_pIconL->setPixmap(QPixmap(KIcon("user-identity").pixmap(QSize(48,48))));

      if(! m_pItemCall->getPeerName().trimmed().isEmpty()) {
         m_pPeerL->setText("<b>"+m_pItemCall->getPeerName()+"</b>");
      }
      else {
         m_pPeerL->setText(i18n("<b>Unknow</b>"));
      }
   }
      
   call_state state = m_pItemCall->getState();
   bool recording = m_pItemCall->getRecording();
   if(state != CALL_STATE_OVER) {
      if(state == CALL_STATE_CURRENT && recording) {
         m_pIconL->setPixmap(QPixmap(ICON_CURRENT_REC));
      }
      else {
         QString str = QString(callStateIcons[state]);
         m_pIconL->setPixmap(QPixmap(str));
      }
      bool transfer = state == CALL_STATE_TRANSFER || state == CALL_STATE_TRANSF_HOLD;
      m_pTransferPrefixL->setVisible(transfer);
      m_pTransferNumberL->setVisible(transfer);

      if(!transfer) {
         m_pTransferNumberL->setText("");
      }
      m_pTransferNumberL->setText(m_pItemCall->getTransferNumber());
      m_pCallNumberL->setText(m_pItemCall->getPeerPhoneNumber());
                
      if(state == CALL_STATE_DIALING) {
         m_pCallNumberL->setText(m_pItemCall->getCallNumber());
      }
      else {
         m_pCodecL->setText("Codec: "+m_pItemCall->getCurrentCodecName());
         if (m_pItemCall->isSecure())
            m_pSecureL->setText("⚷");
      }
   }
   else {
      //qDebug() << "Updating item of call of state OVER. Doing nothing.";
   }
}
