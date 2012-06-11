/************************************************************************************
 *   Copyright (C) 2012 by Savoir-Faire Linux                                       *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
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
#include "VideoCodec.h"
#include "Call.h"
#include "video_interface_singleton.h"

QHash<QString,VideoCodec*> VideoCodec::m_slCodecs;

///Private constructor
VideoCodec::VideoCodec(QString codecName)
{
   VideoInterface& interface = VideoInterfaceSingleton::getInstance();
   QStringList details = interface.getCodecDetails(codecName);
   m_Name = details[0];//TODO do not use stringlist
   m_Id = details[1];//TODO do not use stringlist
}

///Get a codec from a name
VideoCodec* VideoCodec::getCodec(QString name)
{
   return m_slCodecs[name];
}

///Get the current call codec
//TODO move to call.h?
VideoCodec* VideoCodec::getCurrentCodec(Call* call)
{
   VideoInterface& interface = VideoInterfaceSingleton::getInstance();
   return getCodec(interface.getCurrentCodecName(call->getCallId()));
}

///Get the complete video codec list
QList<VideoCodec*> VideoCodec::getCodecList()
{
   return m_slCodecs.values();
}

///Get the current codec name
QString VideoCodec::getCodecName()
{
   return m_Name;
}

///Get the current codec id
QString VideoCodec::getCodecId()
{
   return m_Id;
}