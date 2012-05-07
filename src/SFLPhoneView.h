/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/

#ifndef SFLPHONEVIEW_H
#define SFLPHONEVIEW_H

#include "ui_SFLPhoneView_base.h"
#include <QtGui/QWidget>

//Qt
class QString;
class QKeyEvent;
class QErrorMessage;
class QListWidget;

//SFLPhone
class ConfigurationDialog;
class AccountWizard;
class CallView;


/**
 * This is the main view class for sflphone-client-kde.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 * As the state of the view has effects on the window,
 * it emits some signals to ask for changes that the window has
 * to treat.
 *
 * @short Main view
 * @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
 * @version 0.9.6
 */
class SFLPhoneView : public QWidget, public Ui::SFLPhone_view
{
   Q_OBJECT

private:
   AccountWizard * wizard;
   QErrorMessage * errorWindow;

protected:

   /**
    * override context menu handling
    * @param event
    */
   void contextMenuEvent(QContextMenuEvent *event);

public:
   //Constructors & Destructors
   /**
    *   This constructor does not load the window as it would
    *   better wait for the parent window to connect to the signals
    *   for updating it (statusMessageChangeAsked...).
    *   You should call the loadWindow() method once
    *   you have constructed the object and connected the
    *   expected signals.
    * @param parent
    */
   SFLPhoneView(QWidget *parent);
   virtual ~SFLPhoneView();

   //Getters


   QErrorMessage * getErrorWindow();


   //Daemon getters
   /**
   * Used to sort contacts according to their types with Kabc.
   * @return the integer resulting to the flags of the types
   * chosen to be displayed in SFLphone configuration.
   */
   int phoneNumberTypesDisplayed();


   bool selectCallPhoneNumber(Call* call,Contact* contact);

private slots:
   /**
    *   Performs the action action on the call call, then updates window.
    *   The call object will handle the action with its "actionPerformed" method.
    *   See the documentation for more details.
    * @param call the call on which to perform the action
    * @param action the code of the action to perform
    */
   void action(Call * call, call_action action);

   /**
    *   Sets the account account to be the prior account.
    *   That means it's gonna be used when the user places a call
    *   if it's defined and registered, else the first registered of
    *   accountList will be used.
    * @param account the account to use prior
    */
   void setAccountFirst(Account * account);

   /**
    *   Handles the behaviour when the user types something with
    *   the dialpad widget or his keyboard (normally it's a one char
    *   string but we use a string just in case).
    *   Adds str to the selected item if in the main window
    *   and creates a new item if no item is selected.
    *   Send DTMF if appropriate according to current item's state.
    *   Adds str to the search bar if in history or address book.
    * @param str the string sent by the user
    */
   void typeString(QString str);

   /**
    *   Handles the behaviour when the user types a backspace
    *   according to the current state (window, item selected...)
    */
   void backspace();

   /**
    *   Handles the behaviour when the user types escape
    *   according to the current state (window, item selected...)
    */
   void escape();

   /**
    *   Handles the behaviour when the user types enter
    *   according to the current state (window, item selected...)
    */
   void enter();

   /**
    *   Displays a message window with editable text of the selected
    *   phone number in history or address book.
    *   Once the user accepts, place a call with the chosen phone number.
    *   Keeps the peer name of the contact or past call.
    */
   void editBeforeCall();

   /**
    * Updates the history's search bar's display according to the current
    * text searched.
    * If empty, hide the search bar.
    */
   void updateRecordButton   ();
   void updateVolumeButton   ();
   void updateRecordBar      (double _value = -1);
   void updateVolumeBar      (double _value = -1);
   void updateVolumeControls ();
   void updateDialpad        ();
   void sendMessage();



public slots:
   /**
    * Updates all the display
    * according to the settings.
    */
   void loadWindow();

   /**
    *   Updates the toolbar's actions' display according to the selected
    *   item's state.
    */
   void updateWindowCallState();


   void updateStatusMessage();


   virtual void keyPressEvent(QKeyEvent *event)
   {
      int key = event->key();
      if(key == Qt::Key_Escape)
         escape();
      else if(key == Qt::Key_Return || key == Qt::Key_Enter)
         enter();
      else if(key == Qt::Key_Backspace)
         backspace();
      else
      {
         QString text = event->text();
         if(! text.isEmpty())
         {
            typeString(text);
         }
      }
   }

   void displayVolumeControls(bool checked = true);
   void displayDialpad(bool checked = true);
   void displayMessageBox(bool checked = true);
   void configureSflPhone();
   void accountCreationWizard();
   void accept   ();
   void refuse   ();
   void hold     ();
   void transfer ();
   void record   ();
   void mailBox  ();

   void on_widget_dialpad_typed(QString text);

   void on_slider_recVol_valueChanged ( int value    );
   void on_slider_sndVol_valueChanged ( int value    );
   void on_toolButton_recVol_clicked  ( bool checked );
   void on_toolButton_sndVol_clicked  ( bool checked );

   void on1_error(MapStringString details);
   void on1_incomingCall(Call* call);
   void on1_voiceMailNotify(const QString &accountID, int count);
   void on1_volumeChanged(const QString &device, double value);

signals:
   void statusMessageChangeAsked      ( const QString&  message            );
   void windowTitleChangeAsked        ( const QString&  title              );
   void enabledActionsChangeAsked     ( const bool*     enabledActions     );
   void actionIconsChangeAsked        ( const QString*  actionIcons        );
   void actionTextsChangeAsked        ( const QString*  actionTexts        );
   void transferCheckStateChangeAsked ( bool            transferCheckState );
   void recordCheckStateChangeAsked   ( bool            recordCheckState   );
   void addressBookEnableAsked        ( bool            enableAddressBook  );
   void screenChanged(int screen);
   void incomingCall(const Call * call);


};

#endif // SFLPHONEVIEW_H
