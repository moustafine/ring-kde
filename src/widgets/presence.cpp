/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "presence.h"
#include <presencestatusmodel.h>

Presence::Presence(QWidget* parent) : QWidget(parent)
{
   setupUi(this);
   m_pMessage->setVisible(false);
   m_pPresentRB->setVisible(false);
   m_pAbsentRB->setVisible(false);
   m_pView->setModel(&PresenceStatusModel::instance());

   //Toggle between custom and pre-defined status
   connect(m_pCustomMessageCK              ,&QAbstractButton::toggled,                          &PresenceStatusModel::instance(),&PresenceStatusModel::setUseCustomStatus);
   connect(&PresenceStatusModel::instance(),&PresenceStatusModel::useCustomStatusChanged,           m_pCustomMessageCK,              &QAbstractButton::setChecked);

   //Toggle between present and absent
   connect(m_pPresentRB                    ,&QAbstractButton::toggled,                          &PresenceStatusModel::instance(),&PresenceStatusModel::setCustomStatus);
   connect(&PresenceStatusModel::instance(),&PresenceStatusModel::customStatusChanged,              m_pPresentRB,                    &QAbstractButton::setChecked);

   //Select current pre-defined status
   connect(m_pView->selectionModel()       ,&QItemSelectionModel::currentChanged,&PresenceStatusModel::instance(),&PresenceStatusModel::setCurrentIndex);

   //Change custom text status
   connect(m_pMessage                      ,&QTextEdit::textChanged,                           this,                           &Presence::slotTextChanged);
   connect(&PresenceStatusModel::instance(),&PresenceStatusModel::customMessageChanged,           this,                           &Presence::slotReplaceText);
}

void Presence::slotTextChanged()
{
   const int pos = m_pMessage->textCursor().position();
   PresenceStatusModel::instance().setCustomMessage(m_pMessage->toPlainText());
   m_pMessage->textCursor().setPosition(pos);
}

void Presence::slotReplaceText(const QString& newText)
{
   if (newText != m_pMessage->toPlainText()) {
      const int pos = m_pMessage->textCursor().position();
      m_pMessage->setPlainText(newText);
      m_pMessage->textCursor().setPosition(pos);
   }
}
