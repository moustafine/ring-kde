/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
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
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

//Parent
#include "sflphoneview.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QPointer>
#include <QtGui/QPalette>
#include <QtGui/QWidget>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtGui/QDropEvent>

//KDE
#include <KLocale>
#include <KAction>
#include <KMenu>
#include <KInputDialog>
#include <kabc/addressbook.h>
#include <KMessageBox>
#include <KIcon>

//sflphone
#include "conf/configurationdialog.h"
#include "klib/kcfg_settings.h"
#include "klib/akonadibackend.h"
#include "lib/phonenumber.h"
#include "accountwizard.h"
#include "actionsetaccountfirst.h"
#include "sflphone.h"
#include "widgets/tips/tipcollection.h"
#include "widgets/callviewtoolbar.h"
#include "extendedaction.h"
#include "delegates/conferencedelegate.h"
#include "delegates/historydelegate.h"
#include "delegates/categorizeddelegate.h"
#include "widgets/tips/dialpadtip.h"
#include "widgets/kphonenumberselector.h"
#include "widgets/callviewoverlay.h"
#include "widgets/autocompletion.h"

//sflphone library
#include "lib/typedefs.h"
#include "lib/dbus/configurationmanager.h"
#include "lib/dbus/callmanager.h"
#include "lib/dbus/instancemanager.h"
#include "lib/sflphone_const.h"
#include "lib/contact.h"
#include "lib/accountlistmodel.h"
#include "lib/phonedirectorymodel.h"
#include "klib/helperfunctions.h"
#include "klib/tipmanager.h"
#include "lib/visitors/accountlistcolorvisitor.cpp"

#define IM_ACTIVE m_pMessageTabBox->isVisible()

QDebug operator<<(QDebug dbg, const Call::State& c)
{
   dbg.nospace() << QString(Call::toHumanStateName(c));
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::DaemonState& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::Action& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

class ColorVisitor : public AccountListColorVisitor {
public:
   ColorVisitor(QPalette pal) : m_Pal(pal) {
      m_Green = QColor(m_Pal.color(QPalette::Base));
      if (m_Green.green()+20 >= 255) {
         m_Green.setRed ( ((int)m_Green.red()  -20));
         m_Green.setBlue( ((int)m_Green.blue() -20));
      }
      else
         m_Green.setGreen(((int)m_Green.green()+20));
      
      m_Red = QColor(m_Pal.color(QPalette::Base));
      if (m_Red.red()+20 >= 255) {
         m_Red.setGreen(  ((int)m_Red.green()  -20));
         m_Red.setBlue(   ((int)m_Red.blue()   -20));
      }
      else
         m_Red.setRed(    ((int)m_Red.red()     +20));

      m_Yellow = QColor(m_Pal.color(QPalette::Base));
      if (m_Yellow.red()+20 >= 255 || m_Green.green()+20 >= 255) {
         m_Yellow.setBlue(((int)m_Yellow.blue() -20));
      }
      else {
         m_Yellow.setGreen(((int)m_Yellow.green()+20));
         m_Yellow.setRed( ((int)m_Yellow.red()   +20));
      }
   }

   virtual QVariant getColor(const Account* a) {
      if(a->registrationStatus() == Account::State::UNREGISTERED || !a->isEnabled())
         return m_Pal.color(QPalette::Base);
      if(a->registrationStatus() == Account::State::REGISTERED || a->registrationStatus() == Account::State::READY) {
         return m_Green;
      }
      if(a->registrationStatus() == Account::State::TRYING)
         return m_Yellow;
      return m_Red;
   }

   virtual QVariant getIcon(const Account* a) {
      if (a->state() == Account::AccountEditState::MODIFIED)
         return KIcon("document-save");
      else if (a->state() == Account::AccountEditState::OUTDATED) {
         return KIcon("view-refresh");
      }
      return QVariant();
   }
private:
   QPalette m_Pal;
   QColor   m_Green;
   QColor   m_Yellow;
   QColor   m_Red;
};

class CallViewEventFilter : public QObject
{
public:
   explicit CallViewEventFilter(SFLPhoneView* parent) : QObject(parent) {
      m_pParent = parent;
   }

protected:
   bool eventFilter(QObject *obj, QEvent *event);

private:
   SFLPhoneView* m_pParent;

};

///Forward keypresses to the filter line edit
bool CallViewEventFilter::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::KeyPress) {
      const int key = static_cast<QKeyEvent*>(event)->key();
      if (key != Qt::Key_Left && key != Qt::Key_Right && key != Qt::Key_Down && key != Qt::Key_Up) {
         m_pParent->keyPressEvent(static_cast<QKeyEvent*>(event));
         return true;
      }
   }
   else if (event->type() == QEvent::Drop) {
      QDropEvent* e = static_cast<QDropEvent*>(event);
      const QModelIndex& idxAt = m_pParent->m_pView->indexAt(e->pos());
      CallModel::instance()->setData(idxAt,-1,Call::Role::DropState);
      e->accept();
      if (!idxAt.isValid()) { //Dropped on empty space
         if (e->mimeData()->hasFormat(MIME_CALLID)) {
            const QByteArray encodedCallId      = e->mimeData()->data( MIME_CALLID      );
            kDebug() << "Call dropped on empty space";
            Call* call =  CallModel::instance()->getCall(encodedCallId);
            if (CallModel::instance()->getIndex(call).parent().isValid()) {
               kDebug() << "Detaching participant";
               CallModel::instance()->detachParticipant(CallModel::instance()->getCall(encodedCallId));
            }
            else
               kDebug() << "The call is not in a conversation (doing nothing)";
         }
         else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
            const QByteArray encodedPhoneNumber = e->mimeData()->data( MIME_PHONENUMBER );
            kDebug() << "Phone number dropped on empty space";
            Call* newCall = CallModel::instance()->addDialingCall();
            newCall->setDialNumber(encodedPhoneNumber);
            newCall->actionPerformed(Call::Action::ACCEPT);
         }
         else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
            const QByteArray encodedContact     = e->mimeData()->data( MIME_CONTACT     );
            kDebug() << "Contact dropped on empty space";
            const PhoneNumber* number = KPhoneNumberSelector().getNumber(encodedContact);
            if (number->uri().isEmpty()) {
               Call* newCall = CallModel::instance()->addDialingCall();
               newCall->setDialNumber(number->uri());
               newCall->actionPerformed(Call::Action::ACCEPT);
            }
         }
         else if (e->mimeData()->hasFormat("text/plain")) {
            Call* newCall = CallModel::instance()->addDialingCall();
            newCall->setDialNumber(e->mimeData()->data( "text/plain" ));
            newCall->actionPerformed(Call::Action::ACCEPT);
         }
         //Remove uneedded tip
         if (TipCollection::removeConference() == TipCollection::manager()->currentTip()) {
            TipCollection::manager()->setCurrentTip(nullptr);
         }
         return true;
      }
      else {
         //1) Get the right action
         const QPoint position = e->pos();
         const QRect targetRect = m_pParent->m_pView->visualRect(idxAt);
         Call::DropAction act = (position.x() < targetRect.x()+targetRect.width()/2)?Call::DropAction::Conference:Call::DropAction::Transfer;
         QMimeData* data = (QMimeData*)e->mimeData(); //Drop the const
         data->setProperty("dropAction",act);

         //2) Send to the model for processing
         m_pParent->m_pView->model()->dropMimeData(data,Qt::MoveAction,idxAt.row(),idxAt.column(),idxAt.parent());
      }

      //Remove item overlays
      for (int i = 0;i < m_pParent->m_pView->model()->rowCount();i++) {
         const QModelIndex& idx = m_pParent->m_pView->model()->index(i,0);
         m_pParent->m_pView->model()->setData(idx,-1,Call::Role::DropState);
         for (int j = 0;j < m_pParent->m_pView->model()->rowCount(idx);j++) {
            m_pParent->m_pView->model()->setData(m_pParent->m_pView->model()->index(j,0,idx),-1,Call::Role::DropState);
         }
      }
   }
   else if (event->type() == QEvent::DragMove) {
      QDragMoveEvent* e = static_cast<QDragMoveEvent*>(event);
      if (TipCollection::removeConference() != TipCollection::manager()->currentTip() /*&& idxAt.parent().isValid()*/) {
         if (e->mimeData()->hasFormat(MIME_CALLID)) {
            TipCollection::removeConference()->setText(i18n("Remove the call from the conference, the call will be put on hold"));
         }
         else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
            TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data(MIME_PHONENUMBER))));
         }
         else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
            Contact* c = AkonadiBackend::instance()->getContactByUid(e->mimeData()->data(MIME_CONTACT));
            if (c) {
               TipCollection::removeConference()->setText(i18n("Call %1",c->formattedName()));
            }
         }
         else if (e->mimeData()->hasFormat("text/plain")) {
            TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data("text/plain"))));
         }
         TipCollection::manager()->setCurrentTip(TipCollection::removeConference());
      }
      if (TipCollection::removeConference() == TipCollection::manager()->currentTip()) {
         if (e->mimeData()->hasFormat(MIME_CALLID)) {
            TipCollection::removeConference()->setText(i18n("Remove the call from the conference, the call will be put on hold"));
         }
         else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
            TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data(MIME_PHONENUMBER))));
         }
         else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
            Contact* c = AkonadiBackend::instance()->getContactByUid(e->mimeData()->data(MIME_CONTACT));
            if (c) {
               TipCollection::removeConference()->setText(i18n("Call %1",c->formattedName()));
            }
         }
         else if (e->mimeData()->hasFormat("text/plain")) {
            TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data("text/plain"))));
         }
      }
      //Just as drop, compute the position
      const QModelIndex& idxAt = m_pParent->m_pView->indexAt(e->pos());
      const QPoint position = e->pos();
      const QRect targetRect = m_pParent->m_pView->visualRect(idxAt);
      Call::DropAction act = (position.x() < targetRect.x()+targetRect.width()/2)?Call::DropAction::Conference:Call::DropAction::Transfer;
      CallModel::instance()->setData(idxAt,act,Call::Role::DropPosition);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
} //eventFilter

///Constructor
SFLPhoneView::SFLPhoneView(QWidget *parent)
   : QWidget(parent), wizard(0),m_pTransferOverlay(nullptr),m_pAutoCompletion(nullptr)
{
   setupUi(this);
   KPhoneNumberSelector::init();

   m_pView->setModel(CallModel::instance());
   connect(CallModel::instance(),SIGNAL(layoutChanged()),m_pView,SLOT(expandAll()));
   m_pView->expandAll();
   m_pConfDelegate = new ConferenceDelegate(m_pView,palette());
   m_pHistoryDelegate = new HistoryDelegate(m_pView);
   m_pConfDelegate->setCallDelegate(m_pHistoryDelegate);
   m_pView->setItemDelegate(m_pConfDelegate);
   m_pView->viewport()->installEventFilter(new CallViewEventFilter(this));
   m_pView->installEventFilter(new CallViewEventFilter(this));
   m_pView->setViewType(CategorizedTreeView::ViewType::Call);

   //Enable on-canvas messages
   TipCollection::setManager(new TipManager(m_pView));
//    if (!CallModel::instance()->getCallList().size())
      TipCollection::manager()->setCurrentTip(TipCollection::dialPad());
//    callView->setTitle(i18n("Calls"));

   QPalette pal = QPalette(palette());
   pal.setColor(QPalette::AlternateBase, Qt::lightGray);
   setPalette(pal);

   AccountListModel::instance()->setColorVisitor(new ColorVisitor(pal));

   m_pMessageBoxW->setVisible(false);

   //                SENDER                             SIGNAL                              RECEIVER                SLOT                      /
   /**/connect(CallModel::instance()        , SIGNAL(incomingCall(Call*))                   , this  , SLOT(on1_incomingCall(Call*))          );
   /**/connect(CallModel::instance()        , SIGNAL(voiceMailNotify(QString,int))          , this  , SLOT(on1_voiceMailNotify(QString,int)) );
   /**/connect(CallModel::instance()        , SIGNAL(callStateChanged(Call*))               , this  , SLOT(updateWindowCallState())          );
   /**/connect(AccountListModel::instance() , SIGNAL(accountStateChanged(Account*,QString)) , this  , SLOT(updateStatusMessage())            );
   /**/connect(AccountListModel::instance() , SIGNAL(accountListUpdated())                  , this  , SLOT(updateStatusMessage())            );
   /**/connect(AccountListModel::instance() , SIGNAL(accountListUpdated())                  , this  , SLOT(updateWindowCallState())          );
   /**/connect(m_pSendMessageLE             , SIGNAL(returnPressed())                       , this  , SLOT(sendMessage())                    );
   /**/connect(m_pSendMessagePB             , SIGNAL(clicked())                             , this  , SLOT(sendMessage())                    );
   /**/connect(m_pView                      , SIGNAL(itemDoubleClicked(QModelIndex))        , this  , SLOT(enter())                          );
   /*                                                                                                                                        */

   AccountListModel::instance()->updateAccounts();

   //Listen for macro
   MacroModel::addListener(this);

   //Auto completion
   if (ConfigurationSkeleton::enableAutoCompletion()) {
      m_pAutoCompletion = new AutoCompletion(m_pView);
      PhoneDirectoryModel::instance()->setCallWithAccount(ConfigurationSkeleton::autoCompleteUseAccount());
      m_pAutoCompletion->setUseUnregisteredAccounts(ConfigurationSkeleton::autoCompleteMergeNumbers());
   }

   m_pCanvasToolbar = new CallViewToolbar(m_pView);
   connect(m_pView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)) , m_pCanvasToolbar, SLOT(updateState()));
   connect(CallModel::instance()    , SIGNAL(callStateChanged(Call*))                 , m_pCanvasToolbar, SLOT(updateState()));
   connect(CallModel::instance()    , SIGNAL(layoutChanged())                         , m_pCanvasToolbar, SLOT(updateState()));
}

///Destructor
SFLPhoneView::~SFLPhoneView()
{
   m_pView->setItemDelegate(nullptr);
   delete m_pConfDelegate;
   delete m_pHistoryDelegate;
   if (m_pAutoCompletion)
      delete m_pAutoCompletion;
}

///Init main window
void SFLPhoneView::loadWindow()
{
   updateWindowCallState ();
   updateRecordButton    ();
   updateVolumeButton    ();
   updateRecordBar       ();
   updateVolumeBar       ();
   updateVolumeControls  ();
   updateDialpad         ();
//    updateStatusMessage   ();
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/


/*****************************************************************************
 *                                                                           *
 *                              Keyboard input                               *
 *                                                                           *
 ****************************************************************************/

///Input grabber
void SFLPhoneView::keyPressEvent(QKeyEvent *event)
{
   switch(event->key()) {
      case Qt::Key_Escape:
         escape();
         break;
      case Qt::Key_Return:
      case Qt::Key_Enter:
         if (m_pAutoCompletion && m_pAutoCompletion->selection()) {
            PhoneNumber* n = m_pAutoCompletion->selection();
            Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
            if (call->state() == Call::State::DIALING) {
               call->setDialNumber(n->uri());
               if (PhoneDirectoryModel::instance()->callWithAccount() && n->account() && n->account()->id() != "IP2IP")
                  call->setAccount(n->account());
            }
         }
         enter();
         break;
      case Qt::Key_Backspace:
         backspace();
         break;
      case Qt::Key_Up:
         if (m_pAutoCompletion && m_pAutoCompletion->isVisible()) {
            m_pAutoCompletion->moveUp();
         }
         break;
      case Qt::Key_Down:
         if (m_pAutoCompletion && m_pAutoCompletion->isVisible()) {
            m_pAutoCompletion->moveDown();
         }
         break;
      default: {
         QString text = event->text();
         if(! text.isEmpty()) {
            typeString(text);
         }
      }
      break;
   };
} //keyPressEvent

void SFLPhoneView::addDTMF(const QString& sequence)
{
   if (sequence == "\n")
      enter();
   else
      typeString(sequence);
}

///Called on keyboard
void SFLPhoneView::typeString(QString str)
{
   /* There is 5 cases
    * 1) There is no call, then create one
    * 2) There is one or more call and the active call is also selected, then send DTMF to the active call
    * 3) There is multiple call, but the active one is not selected, then create a new call or add to existing dialing call
    * 4) There is only inactive calls, then create a new one or add to existing dialing call
    * 5) There is only FAILURE, BUSY or UNKNOWN calls, then create a new one or add to existing dialing call
    * 
    * When adding to dialing call, select it to give user feedback of where the tone went.
    * 
    * Any other comportment need to be documented here or treated as a bug
    */

   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   Call* currentCall = nullptr;
   Call* candidate   = nullptr;


   //If the selected call is also the current one, then send DTMF and exit
   if(call && call->state() == Call::State::CURRENT) {
      currentCall = call;
      call->playDTMF(str);
      return;
   }

   foreach (Call* call2, CallModel::instance()->getCallList()) {
      if(dynamic_cast<Call*>(call2) && currentCall != call2 && call2->state() == Call::State::CURRENT) {
         action(call2, Call::Action::HOLD);
      }
      else if(dynamic_cast<Call*>(call2) && call2->state() == Call::State::DIALING) {
         candidate = call2;
      }
   }

   if(!currentCall && !candidate) {
      kDebug() << "Typing when no item is selected. Opening an item.";
      candidate = CallModel::instance()->addDialingCall();
      const QModelIndex& newCallIdx = CallModel::instance()->getIndex(candidate);
      if (newCallIdx.isValid()) {
         m_pView->selectionModel()->setCurrentIndex(newCallIdx,QItemSelectionModel::SelectCurrent);
      }
   }

   if (!candidate) {
      candidate = CallModel::instance()->addDialingCall();
   }
   if(!currentCall && candidate) {
      candidate->playDTMF(str);
      candidate->appendText(str);
   }
} //typeString

///Called when a backspace is detected
void SFLPhoneView::backspace()
{
   kDebug() << "backspace";
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Backspace on unexisting call.";
   }
   else {
      call->backspaceItemText();
      if(call->state() == Call::State::OVER && CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex())) { //TODO dead code
//          callView->removeItem(callView->getCurrentItem());
      }
   }
}

///Called when escape is detected
void SFLPhoneView::escape()
{
   kDebug() << "escape";
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (m_pTransferOverlay && m_pTransferOverlay->isVisible()) {
      m_pTransferOverlay->setVisible(false);
      updateWindowCallState();
      return;
   }
   /*else */if(!call) {
      kDebug() << "Escape when no item is selected. Doing nothing.";
   }
   else {
      switch (call->state()) {
         case Call::State::TRANSFERRED:
         case Call::State::TRANSF_HOLD:
            action(call, Call::Action::TRANSFER);
            break;
         case Call::State::INCOMING:
         case Call::State::DIALING:
         case Call::State::HOLD:
         case Call::State::RINGING:
         case Call::State::CURRENT:
         case Call::State::FAILURE:
         case Call::State::BUSY:
         case Call::State::OVER:
         case Call::State::ERROR:
         case Call::State::CONFERENCE:
         case Call::State::CONFERENCE_HOLD:
         case Call::State::COUNT:
         default:
            action(call, Call::Action::REFUSE);
      }
   }
} //escape

///Called when enter is detected
void SFLPhoneView::enter()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex()); //TODO use selectionmodel
   if(!call) {
      kDebug() << "Error : Enter on unexisting call.";
   }
   else {
      switch (call->state()) {
         case Call::State::INCOMING:
         case Call::State::DIALING:
         case Call::State::TRANSFERRED:
         case Call::State::TRANSF_HOLD:
            action(call, Call::Action::ACCEPT);
            break;
         case Call::State::HOLD:
            action(call, Call::Action::HOLD);
            break;
         case Call::State::RINGING:
         case Call::State::CURRENT:
         case Call::State::FAILURE:
         case Call::State::BUSY:
         case Call::State::OVER:
         case Call::State::ERROR:
         case Call::State::CONFERENCE:
         case Call::State::CONFERENCE_HOLD:
         case Call::State::COUNT:
         default:
            kDebug() << "Enter when call selected not in appropriate state. Doing nothing.";
      }
   }
}

///Create a call from the clipboard content
void SFLPhoneView::paste()
{
   QClipboard* cb = QApplication::clipboard();
   const QMimeData* data = cb->mimeData();
   if (data->hasFormat(MIME_PHONENUMBER)) {
      typeString(data->data(MIME_PHONENUMBER));
   }
   else {
      typeString(cb->text());
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Change call state
void SFLPhoneView::action(Call* call, Call::Action action)
{
   if(! call) {
      kDebug() << "Error : action " << action << "applied on null object call. Should not happen.";
   }
   else {
      try {
         call->actionPerformed(action);
      }
      catch(const char * msg) {
         KMessageBox::error(this,i18n(msg));
      }
      updateWindowCallState();
   }
} //action

///Select a phone number when calling using a contact
bool SFLPhoneView::selectCallPhoneNumber(Call** call2,Contact* contact)
{
   if (contact->phoneNumbers().count() == 1) {
      *call2 = CallModel::instance()->addDialingCall(contact->formattedName(),AccountListModel::currentAccount());
      if (*call2)
         (*call2)->appendText(contact->phoneNumbers()[0]->uri());
   }
   else if (contact->phoneNumbers().count() > 1) {
      const PhoneNumber* number = KPhoneNumberSelector().getNumber(contact->uid());
      if (!number->uri().isEmpty()) {
         (*call2) = CallModel::instance()->addDialingCall(contact->formattedName(), AccountListModel::currentAccount());
         if (*call2)
            (*call2)->appendText(number->uri());
      }
      else {
         kDebug() << "Operation cancelled";
         return false;
      }
   }
   else {
      kDebug() << "This contact have no valid phone number";
      return false;
   }
   return true;
} //selectCallPhoneNumber


/*****************************************************************************
 *                                                                           *
 *                       Update display related code                         *
 *                                                                           *
 ****************************************************************************/

///Change GUI icons
void SFLPhoneView::updateWindowCallState()
{
   kDebug() << "Call state changed";
   bool    enabledActions [6] = { true             ,true                 , true             , true                 , true               , true                 };
   QString buttonIconFiles[6] = { ICON_CALL        , ICON_HANGUP         , ICON_HOLD        , ICON_TRANSFER        , ICON_REC_DEL_OFF   , ICON_MAILBOX         };
   QString actionTexts    [6] = { ACTION_LABEL_CALL, ACTION_LABEL_HANG_UP, ACTION_LABEL_HOLD, ACTION_LABEL_TRANSFER, ACTION_LABEL_RECORD, ACTION_LABEL_MAILBOX };

   Call* call = 0;

   bool transfer(false),recordActivated(false);

   enabledActions[SFLPhone::Mailbox] = AccountListModel::currentAccount() && ! AccountListModel::currentAccount()->mailbox().isEmpty();

   call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (!call) {
      kDebug() << "No item selected.";
      enabledActions[ SFLPhone::Refuse   ] = false;
      enabledActions[ SFLPhone::Hold     ] = false;
      enabledActions[ SFLPhone::Transfer ] = false;
      enabledActions[ SFLPhone::Record   ] = false;
      m_pMessageBoxW->setVisible(false);
//       callView->overlayToolbar()->setVisible(false);
   }
   else if (call->isConference()) {
      //TODO Something to do?
   }
   else {
      Call::State state = call->state();
      recordActivated = call->recording();

//       callView->overlayToolbar()->updateState(call->state());
//       callView->overlayToolbar()->setVisible(true);

      kDebug() << "Reached  State" << state << "(" << call->state() << ") with call" << call->id();

      switch (state) {
         case Call::State::INCOMING:
            buttonIconFiles [ SFLPhone::Accept   ] = ICON_ACCEPT                 ;
            buttonIconFiles [ SFLPhone::Refuse   ] = ICON_REFUSE                 ;
            actionTexts     [ SFLPhone::Accept   ] = ACTION_LABEL_ACCEPT         ;
            actionTexts     [ SFLPhone::Refuse   ] = ACTION_LABEL_REFUSE         ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)   ;
            if (TipCollection::manager()) {
               TipCollection::manager()->setCurrentTip(TipCollection::ringing());
            }
            break;

         case Call::State::RINGING:
            enabledActions  [ SFLPhone::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            if (TipCollection::manager()) {
               TipCollection::manager()->setCurrentTip(TipCollection::ringing());
            }
            break;

         case Call::State::CURRENT:
            buttonIconFiles [ SFLPhone::Record   ] = ICON_REC_DEL_ON             ;
            m_pMessageBoxW->setVisible((true && ConfigurationSkeleton::displayMessageBox()) || IM_ACTIVE);
            break;

         case Call::State::DIALING:
            enabledActions  [ SFLPhone::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::Record   ] = false                       ;
            actionTexts     [ SFLPhone::Accept   ] = ACTION_LABEL_ACCEPT         ;
            buttonIconFiles [ SFLPhone::Accept   ] = ICON_ACCEPT                 ;
            m_pMessageBoxW->setVisible(false)                                    ;
//             callView->overlayToolbar()->setVisible(false);
            break;

         case Call::State::HOLD:
            buttonIconFiles [ SFLPhone::Hold     ] = ICON_UNHOLD                 ;
            actionTexts     [ SFLPhone::Hold     ] = ACTION_LABEL_UNHOLD         ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::FAILURE:
            //enabledActions  [ SFLPhone::Accept   ] = false                     ;
            enabledActions  [ SFLPhone::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::Record   ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::BUSY:
            //enabledActions  [ SFLPhone::Accept   ] = false                     ;
            enabledActions  [ SFLPhone::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::Record   ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            if (TipCollection::manager())
               TipCollection::manager()->setCurrentTip(TipCollection::endBusy());
            break;

         case Call::State::TRANSFERRED:
            buttonIconFiles [ SFLPhone::Accept   ] = ICON_EXEC_TRANSF            ;
            actionTexts     [ SFLPhone::Transfer ] = ACTION_LABEL_GIVE_UP_TRANSF ;
            buttonIconFiles [ SFLPhone::Record   ] = ICON_REC_DEL_ON             ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)                       ;
            if (!m_pTransferOverlay) {
               m_pTransferOverlay = new CallViewOverlay(m_pView);
            }
            m_pTransferOverlay->setCurrentCall(call);
            m_pTransferOverlay->setVisible(true);
            transfer = true;
            break;

         case Call::State::TRANSF_HOLD:
            buttonIconFiles [ SFLPhone::Accept   ] = ICON_EXEC_TRANSF            ;
            buttonIconFiles [ SFLPhone::Hold     ] = ICON_UNHOLD                 ;
            actionTexts     [ SFLPhone::Transfer ] = ACTION_LABEL_GIVE_UP_TRANSF ;
            actionTexts     [ SFLPhone::Hold     ] = ACTION_LABEL_UNHOLD         ;
            m_pMessageBoxW->setVisible(false)                                    ;
            transfer = true;
            break;

         case Call::State::OVER:
            kDebug() << "Error : Reached CALL_STATE_OVER with call "  << call->id() << "!";
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::ERROR:
            kDebug() << "Error : Reached CALL_STATE_ERROR with call " << call->id() << "!";
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::CONFERENCE:
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)                       ;
            if (TipCollection::manager()->currentTip() == TipCollection::dragAndDrop() && TipCollection::dragAndDrop()) {
               TipCollection::manager()->hideTip(TipCollection::dragAndDrop());
            }
            break;

         case Call::State::CONFERENCE_HOLD:
            enabledActions  [ SFLPhone::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            if (TipCollection::manager()->currentTip() == TipCollection::dragAndDrop() && TipCollection::dragAndDrop()) {
               TipCollection::manager()->hideTip(TipCollection::dragAndDrop());
            }
            break;
         case Call::State::COUNT:
         default: 
            kDebug() << "Error : Reached unexisting state for call "  << call->id() << "(" << call->state() << "!";
            break;

      }

      //Manage tips
      //There is little way to be sure when to end the ringing animation, for now, brute force the check
      bool displayRinging = false;
      if (TipCollection::ringing()->isVisible() || TipCollection::manager()->currentTip() == TipCollection::ringing()) {
         foreach (Call* call2, CallModel::instance()->getCallList()) {
            if(dynamic_cast<Call*>(call2) && (call2->state() == Call::State::INCOMING || call2->state() == Call::State::RINGING)) {
               displayRinging = true;
               break;
            }
         }
         if (!displayRinging) {
            TipCollection::manager()->hideTip(TipCollection::ringing());
         }
      }
      if (TipCollection::dragAndDrop()) {
         int activeCallCounter=0;
         foreach (Call* call2, CallModel::instance()->getCallList()) {
            if (dynamic_cast<Call*>(call2)) {
               activeCallCounter += (call2->state() == Call::State::CURRENT || call2->state() == Call::State::HOLD);
               activeCallCounter -= (call2->state() == Call::State::INCOMING || call2->state() ==Call::State::RINGING)*1000;
            }
         }
         if (activeCallCounter >= 2 && !CallModel::instance()->getConferenceList().size()) {
            TipCollection::manager()->setCurrentTip(TipCollection::dragAndDrop());
         }
         else if (TipCollection::manager()->currentTip() == TipCollection::dragAndDrop()) {
            TipCollection::manager()->hideTip(TipCollection::dragAndDrop());
         }
      }
   }

   kDebug() << "Updating Window.";

   emit enabledActionsChangeAsked     ( enabledActions  );
   emit actionIconsChangeAsked        ( buttonIconFiles );
   emit actionTextsChangeAsked        ( actionTexts     );
   emit transferCheckStateChangeAsked ( transfer        );
   emit recordCheckStateChangeAsked   ( recordActivated );

   kDebug() << "Window updated.";
} //updateWindowCallState

///Change icon of the record button
void SFLPhoneView::updateRecordButton()
{
   kDebug() << "updateRecordButton";
   CallManagerInterface & callManager = DBus::CallManager::instance();
   double recVol = callManager.getVolume(RECORD_DEVICE);
   if(recVol     == 0.00) {
      static const QIcon recVol0 = QIcon(ICON_REC_VOL_0);
      toolButton_recVol->setIcon(recVol0);
   }
   else if(recVol < 0.33) {
      static const QIcon recVol1 = QIcon(ICON_REC_VOL_1);
      toolButton_recVol->setIcon(recVol1);
   }
   else if(recVol < 0.67) {
      static const QIcon recVol2 = QIcon(ICON_REC_VOL_2);
      toolButton_recVol->setIcon(recVol2);
   }
   else {
      static const QIcon recVol3 = QIcon(ICON_REC_VOL_3);
      toolButton_recVol->setIcon(recVol3);
   }

   if(recVol > 0) {
      toolButton_recVol->setChecked(false);
   }
}

///Update the colunm button icon
void SFLPhoneView::updateVolumeButton()
{
   kDebug() << "updateVolumeButton";
   CallManagerInterface& callManager = DBus::CallManager::instance();
   const double sndVol = callManager.getVolume(SOUND_DEVICE);

   if(sndVol     == 0.00) {
      toolButton_sndVol->setIcon(QIcon(ICON_SND_VOL_0));
   }
   else if(sndVol < 0.33) {
      toolButton_sndVol->setIcon(QIcon(ICON_SND_VOL_1));
   }
   else if(sndVol < 0.67) {
      toolButton_sndVol->setIcon(QIcon(ICON_SND_VOL_2));
   }
   else {
      toolButton_sndVol->setIcon(QIcon(ICON_SND_VOL_3));
   }

   if(sndVol > 0) {
      toolButton_sndVol->setChecked(false);
   }
}

///Update the record bar
void SFLPhoneView::updateRecordBar(double _value)
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   const double recVol = callManager.getVolume(RECORD_DEVICE);
   kDebug() << "updateRecordBar" << recVol;
   const int value = (_value > 0)?_value:(int)(recVol * 100);
   slider_recVol->setValue(value);
}

///Update the volume bar
void SFLPhoneView::updateVolumeBar(double _value)
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   const double sndVol = callManager.getVolume(SOUND_DEVICE);
   kDebug() << "updateVolumeBar" << sndVol;
   const int value = (_value > 0)?_value:(int)(sndVol * 100);
   slider_sndVol->setValue(value);
}

///Hide or show the volume control
void SFLPhoneView::updateVolumeControls()
{
   toolButton_recVol->setVisible ( SFLPhone::app()->action_displayVolumeControls->isChecked() && ConfigurationSkeleton::displayVolume() );
   toolButton_sndVol->setVisible ( SFLPhone::app()->action_displayVolumeControls->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_recVol->setVisible     ( SFLPhone::app()->action_displayVolumeControls->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_sndVol->setVisible     ( SFLPhone::app()->action_displayVolumeControls->isChecked() && ConfigurationSkeleton::displayVolume() );
}

///Hide or show the dialpad
void SFLPhoneView::updateDialpad()
{
   widget_dialpad->setVisible(ConfigurationSkeleton::displayDialpad());//TODO use display variable
}

///Change the statusbar message
void SFLPhoneView::updateStatusMessage()
{
   const Account* account = AccountListModel::currentAccount();

   if(!account) {
      emit statusMessageChangeAsked(i18n("No registered accounts"));
   }
   else {
      emit statusMessageChangeAsked(i18n("Using account \'%1\' (%2)",
         account->alias(), account->registrationStatus()));
   }
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///Proxy to hide or show the volume control
void SFLPhoneView::displayVolumeControls(bool checked)
{
   //ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();
   ConfigurationSkeleton::setDisplayVolume(checked);
   updateVolumeControls();
}

///Proxy to hide or show the dialpad
void SFLPhoneView::displayDialpad(bool checked)
{
   ConfigurationSkeleton::setDisplayDialpad(checked);
   updateDialpad();
}

///Display a notification popup (freedesktop notification)
void SFLPhoneView::displayMessageBox(bool checked)
{
   ConfigurationSkeleton::setDisplayMessageBox(checked);
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   m_pMessageBoxW->setVisible(checked
      && call
      && (call->state()   == Call::State::CURRENT
         || call->state() == Call::State::HOLD
      )
   );
}

///Input grabber
void SFLPhoneView::on_widget_dialpad_typed(QString text)
{
   typeString(text);
}

///The value on the slider changed
void SFLPhoneView::on_slider_recVol_valueChanged(int value)
{
   kDebug() << "on_slider_recVol_valueChanged(" << value << ")";
   CallManagerInterface & callManager = DBus::CallManager::instance();
   Q_NOREPLY callManager.setVolume(RECORD_DEVICE, (double)value / 100.0);
   updateRecordButton();
}

///The value on the slider changed
void SFLPhoneView::on_slider_sndVol_valueChanged(int value)
{
   kDebug() << "on_slider_sndVol_valueChanged(" << value << ")";
   CallManagerInterface & callManager = DBus::CallManager::instance();
   Q_NOREPLY callManager.setVolume(SOUND_DEVICE, (double)value / 100.0);
   updateVolumeButton();
}

///The mute button have been clicked
void SFLPhoneView::on_toolButton_recVol_clicked(bool checked)
{
   mute(checked);
}

///The mute button have been clicked
void SFLPhoneView::on_toolButton_sndVol_clicked(bool checked)
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   kDebug() << "on_toolButton_sndVol_clicked().";
   if(!checked) {
      toolButton_sndVol->setChecked(false);
      slider_sndVol->setEnabled(true);
      Q_NOREPLY callManager.setVolume(SOUND_DEVICE, (double)slider_sndVol->value() / 100.0);
   }
   else {
      toolButton_sndVol->setChecked(true);
      slider_sndVol->setEnabled(false);
      Q_NOREPLY callManager.setVolume(SOUND_DEVICE, 0.0);
   }

   updateVolumeButton();
}

///Pick the default account and load it
void SFLPhoneView::setAccountFirst(Account * account)
{
   kDebug() << "setAccountFirst : " << (account ? account->alias() : QString()) << (account ? account->id() : QString());
   if(account) {
      AccountListModel::instance()->setPriorAccount(account);
   }
   else {
      AccountListModel::instance()->setPriorAccount(nullptr);
   }
   kDebug() << "Current account id" << (AccountListModel::currentAccount()?AccountListModel::currentAccount()->id():"<no account>");
   updateStatusMessage();
}

///Show the configuration dialog
void SFLPhoneView::configureSflPhone()
{
   QPointer<ConfigurationDialog> configDialog = new ConfigurationDialog(this);
   configDialog->setModal(true);

   connect(configDialog, SIGNAL(changesApplied()), this, SLOT(loadWindow()));
   configDialog->exec();
   disconnect(configDialog, SIGNAL(changesApplied()), this, SLOT(loadWindow()));
   delete configDialog;
}

///Show the account creation wizard
void SFLPhoneView::accountCreationWizard()
{
   if (!wizard) {
      wizard = new AccountWizard(this);
      wizard->setModal(false);
   }
   wizard->show();
}

///Call
void SFLPhoneView::accept() //TODO dead code?
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Calling when no item is selected. Opening an item.";
      Call* newCall = CallModel::instance()->addDialingCall();
      const QModelIndex& newCallIdx = CallModel::instance()->getIndex(newCall);
      if (newCallIdx.isValid()) {
         m_pView->selectionModel()->setCurrentIndex(newCallIdx,QItemSelectionModel::SelectCurrent);
      }
   }
   else {
      const Call::State state = call->state();
      if (state == Call::State::RINGING || state == Call::State::CURRENT || state == Call::State::HOLD || state == Call::State::BUSY) {
         kDebug() << "Calling when item currently ringing, current, hold or busy. Opening an item.";
         Call* newCall = CallModel::instance()->addDialingCall();
         const QModelIndex& newCallIdx = CallModel::instance()->getIndex(newCall);
         if (newCallIdx.isValid()) {
            m_pView->selectionModel()->setCurrentIndex(newCallIdx,QItemSelectionModel::SelectCurrent);
         }
      }
      else {
         action(call, Call::Action::ACCEPT);
      }
   }
} //accept

///Call
void SFLPhoneView::hangup()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (call) {
      action(call, Call::Action::REFUSE);
   }
} //hangup

///Refuse call
void SFLPhoneView::refuse()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Hanging up when no item selected. Should not happen.";
   }
   else {
      action(call, Call::Action::REFUSE);
   }
}

///Put call on hold
void SFLPhoneView::hold()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Holding when no item selected. Should not happen.";
   }
   else {
      action(call, Call::Action::HOLD);
   }
}

///Remove call from hold
void SFLPhoneView::unhold()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Un-Holding when no item selected. Should not happen.";
   }
   else {
      action(call, Call::Action::HOLD);
   }
}

///Transfer a call
void SFLPhoneView::transfer()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Transferring when no item selected. Should not happen.";
   }
   else {
      action(call, Call::Action::TRANSFER);
   }
}

///Record a call
void SFLPhoneView::record()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Recording when no item selected. Should not happen.";
   }
   else {
      action(call, Call::Action::RECORD);
   }
}

//Mute a call
void SFLPhoneView::mute(bool value)
{
   CallManagerInterface & callManager = DBus::CallManager::instance();
   kDebug() << "on_toolButton_recVol_clicked().";
   if(!value) {
      toolButton_recVol->setChecked(false);
      slider_recVol->setEnabled(true);
      Q_NOREPLY callManager.setVolume(RECORD_DEVICE, (double)slider_recVol->value() / 100.0);
      SFLPhone::app()->muteAction()->setText(i18nc("Mute the current audio device","Mute"));
   }
   else {
      toolButton_recVol->setChecked(true);
      slider_recVol->setEnabled(false);
      Q_NOREPLY callManager.setVolume(RECORD_DEVICE, 0.0);
      SFLPhone::app()->muteAction()->setText(i18nc("Unmute the current audio device","Unmute"));
   }
   updateRecordButton();
}

///Access the voice mail list
void SFLPhoneView::mailBox()
{
   Account* account = AccountListModel::currentAccount();
   const QString mailBoxNumber = account->mailbox();
   Call* call = CallModel::instance()->addDialingCall();
   if (call) {
      call->appendText(mailBoxNumber);
      action(call, Call::Action::ACCEPT);
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(this);
   }
}

///When a call is coming (dbus)
void SFLPhoneView::on1_incomingCall(Call* call)
{
   kDebug() << "Signal : Incoming Call ! ID = " << call->id();

   updateWindowCallState();

   SFLPhone::app()->activateWindow  (      );
   SFLPhone::app()->raise           (      );
   SFLPhone::app()->setVisible      ( true );

   const QModelIndex& idx = CallModel::instance()->getIndex(call);
   if (idx.isValid() && (call->state() == Call::State::RINGING || call->state() == Call::State::INCOMING)) {
      m_pView->selectionModel()->clearSelection();
      m_pView->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::SelectCurrent);
   }

   emit incomingCall(call);
}

///When a new voice mail is coming
void SFLPhoneView::on1_voiceMailNotify(const QString &accountID, int count)
{
   kDebug() << "Signal : VoiceMail Notify ! " << count << " new voice mails for account " << accountID;
}

///When the volume change
void SFLPhoneView::on1_volumeChanged(const QString& device, double value)
{
   Q_UNUSED(device)
   kDebug() << "Signal : Volume Changed !" << value;
   if(! (toolButton_recVol->isChecked() && value == 0.0))
      updateRecordBar(value);
   if(! (toolButton_sndVol->isChecked() && value == 0.0))
      updateVolumeBar(value);
}

///Send a text message
void SFLPhoneView::sendMessage()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (dynamic_cast<Call*>(call) && !m_pSendMessageLE->text().isEmpty()) {
      call->sendTextMessage(m_pSendMessageLE->text());
   }
   m_pSendMessageLE->clear();
}

#include "sflphoneview.moc"
