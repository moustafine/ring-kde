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

//Parent
#include "Codec.h"

//Qt
#include <QtCore/QString>

//SFLPhone library
#include "lib/configurationmanager_interface_singleton.h"
#include "lib/sflphone_const.h"

///Constructor
Codec::Codec(int payload, bool enabled)
{
   ConfigurationManagerInterface & configurationManager = ConfigurationManagerInterfaceSingleton::getInstance();
   QStringList details = configurationManager.getAudioCodecDetails(payload);
   m_Payload   = QString::number(payload);
   m_Enabled   = enabled;
   m_Name      = details[CODEC_NAME];
   m_Frequency = details[CODEC_SAMPLE_RATE];
   m_Bitrate   = details[CODEC_BIT_RATE];
   m_Bandwidth = details[CODEC_BANDWIDTH];
}


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Return the payload
QString Codec::getPayload() const
{
  return m_Payload;
}

///Return the codec name
QString Codec::getName() const
{
  return m_Name;
}

///Return the frequency
QString Codec::getFrequency() const
{
  return m_Frequency;
}

///Return the bitrate
QString Codec::getBitrate() const
{
  return m_Bitrate;
}

///Return the bandwidth
QString Codec::getBandwidth() const
{
  return m_Bandwidth;
}

///Is this codec enabled
bool Codec::isEnabled() const
{
  return m_Enabled;
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set the payload
void Codec::setPayload(QString payload)
{
  m_Payload = payload;
}

///Set the codec name
void Codec::setName(QString name)
{
  m_Name = name;
}

///Set the frequency
void Codec::setFrequency(QString frequency)
{
  m_Frequency = frequency;
}

///Set the bitrate
void Codec::setBitrate(QString bitrate)
{
  m_Bitrate = bitrate;
}

///Set the bandwidth
void Codec::setBandwidth(QString bandwidth)
{
  m_Bandwidth = bandwidth;
}

///Make this cedec enabled
void Codec::setEnabled(bool enabled)
{
  m_Enabled = enabled;
}
