/***************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "playeroverlay.h"

#include <QtCore/QFile>

#include <KIcon>
#include <KMessageBox>
#include <KDebug>

#include "lib/call.h"

PlayerOverlay::PlayerOverlay(Call* call, QWidget* parent) : QWidget(parent),m_pCall(call)
{
   setupUi(this);
   m_pPlayer->setVisible(false);
   m_pDelete->setIcon( KIcon("edit-delete") );
   m_pPlay->setIcon( KIcon( "media-playback-start" ));
}

void PlayerOverlay::slotDeleteRecording()
{
   int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to delete this recording?"), i18n("Delete recording"));
   if (ret == KMessageBox::Yes) {
      kDebug() << "Deleting file";
      QFile::remove(m_pCall->getRecordingPath());
      setVisible(false);
   }
}

void PlayerOverlay::setCall(Call* call)
{
   m_pCall = call;
}
