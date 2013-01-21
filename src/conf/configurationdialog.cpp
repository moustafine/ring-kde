/****************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                               *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#include "configurationdialog.h"

//KDE
#include <KDebug>


#include "klib/configurationskeleton.h"

#include "dlggeneral.h"
#include "dlgdisplay.h"
#include "dlgaccounts.h"
#include "dlgaudio.h"
#include "dlgaddressbook.h"
#include "dlghooks.h"
#include "dlgaccessibility.h"
#include "dlgvideo.h"

#include "lib/sflphone_const.h"

///Constructor
ConfigurationDialog::ConfigurationDialog(SFLPhoneView *parent)
 :KConfigDialog(parent, SETTINGS_NAME, ConfigurationSkeleton::self()),dlgVideo(0),dlgGeneral(0),dlgDisplay(0)
 ,dlgAudio(0),dlgAddressBook(0),dlgHooks(0),dlgAccessibility(0),dlgAccounts(0)
{
   this->setWindowIcon(QIcon(ICON_SFLPHONE));

   dlgGeneral       = new DlgGeneral       (this);
   dlgDisplay       = new DlgDisplay       (this);
   dlgAudio         = new DlgAudio         (this);
   dlgAddressBook   = new DlgAddressBook   (this);
   dlgHooks         = new DlgHooks         (this);
   dlgAccessibility = new DlgAccessibility (this);
   dlgAccounts      = new DlgAccounts      (this);
   #ifdef ENABLE_VIDEO
   dlgVideo         = new DlgVideo         (this);
   #endif

   addPage( dlgAccounts      , i18n("Accounts")                     , "user-identity"                     );
   addPage( dlgDisplay       , i18nc("User interterface settings"   ,"Display"), "applications-graphics"  );
   addPage( dlgGeneral       , i18nc("History settings","History")  , "view-history"               );
   addPage( dlgAudio         , i18n("Audio")                        , "audio-headset"                     );
   addPage( dlgAddressBook   , i18n("Address Book")                 , "x-office-address-book"             );
   addPage( dlgHooks         , i18n("Hooks")                        , "insert-link"                       );
   addPage( dlgAccessibility , i18n("Accessibility")                , "preferences-desktop-accessibility" );
   #ifdef ENABLE_VIDEO
   addPage( dlgVideo         , i18nc("Video conversation","Video")  , "camera-web"                        );
   #endif

   connect(this, SIGNAL(applyClicked()) , this, SLOT(applyCustomSettings()));
   connect(this, SIGNAL(okClicked())    , this, SLOT(applyCustomSettings()));
   connect(this, SIGNAL(cancelClicked()), this, SLOT(cancelSettings())     );

   connect(dlgGeneral, SIGNAL(clearCallHistoryAsked()), this, SIGNAL(clearCallHistoryAsked()));
} //ConfigurationDialog

///Destructor
ConfigurationDialog::~ConfigurationDialog()
{
   delete dlgGeneral      ;
   delete dlgDisplay      ;
   delete dlgAccounts     ;
   delete dlgAudio        ;
   delete dlgAddressBook  ;
   delete dlgHooks        ;
   delete dlgAccessibility;
   #ifdef ENABLE_VIDEO
   delete dlgVideo        ;
   #endif
}

///Update all widgets when something is reloaded
void ConfigurationDialog::updateWidgets()
{
   dlgAudio->updateWidgets        ();
   dlgAccounts->updateWidgets     ();
   dlgGeneral->updateWidgets      ();
   dlgAddressBook->updateWidgets  ();
   dlgAccessibility->updateWidgets();
}

///Save all settings when apply is clicked
void ConfigurationDialog::updateSettings()
{
   dlgAudio->updateSettings        ();
   dlgAccounts->updateSettings     ();
   dlgGeneral->updateSettings      ();
   dlgAddressBook->updateSettings  ();
   dlgAccessibility->updateSettings();
   dlgDisplay->updateSettings      ();
}

///Cancel current modification
void ConfigurationDialog::cancelSettings()
{
   dlgAccounts->cancel();
}

///If the account changed
bool ConfigurationDialog::hasChanged()
{
   bool res =  (dlgAudio       && dlgAudio->hasChanged()       )
            || (dlgAccounts    && dlgAccounts->hasChanged()    )
            || (dlgGeneral     && dlgGeneral->hasChanged()     )
            || (dlgDisplay     && dlgDisplay->hasChanged()     )
            || (dlgAddressBook && dlgAddressBook->hasChanged() )
            || (dlgVideo       && dlgVideo->hasChanged()       );

   return res;
}

bool ConfigurationDialog::hasIncompleteRequiredFields()
{
   return dlgAccounts && dlgAccounts->hasIncompleteRequiredFields();
}

///Update the buttons
void ConfigurationDialog::updateButtons()
{
   bool changed      = hasChanged();
   bool preventApply = hasIncompleteRequiredFields();
   enableButtonApply( changed && (!preventApply) );
   enableButtonOk   ( !preventApply              );
}

///Apply settings
void ConfigurationDialog::applyCustomSettings()
{
   if(hasChanged()) {
          ConfigurationSkeleton::self()->writeConfig();
   }
   updateSettings();
   updateWidgets ();
   updateButtons ();
   emit changesApplied();
}

///Reload the pages
void ConfigurationDialog::reload()
{
   kDebug() << "Reloading config";
   ConfigurationSkeleton::self()->readConfig();
   updateWidgets();
   updateButtons();
}
