/************************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                                       *
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
#ifndef LEGACYHISTORYBACKEND_H
#define LEGACYHISTORYBACKEND_H

#include "abstractitembackend.h"

/// @deprecated Remove once the daemon drop configurationmanager::getHistory()
class LIB_EXPORT LegacyHistoryBackend : public AbstractHistoryBackend
{
public:
   explicit LegacyHistoryBackend(QObject* parent = nullptr);
   virtual ~LegacyHistoryBackend();

   virtual bool load() override;
   virtual bool reload() override;
   virtual bool save(const Call* call) override;
   virtual bool append(const Call* item) override;

   virtual QString name () const override;
   virtual QVariant icon() const override;
   virtual bool isEnabled() const override;
   virtual QByteArray id() const override;

   virtual SupportedFeatures supportedFeatures() const override;

   virtual QList<Call*> items() const override;

   ///Edit 'item', the implementation may be a GUI or somehting else
   virtual bool edit( Call* call) override;
   ///Add a new item to the backend
   virtual bool addNew( Call* call) override;

   ///Add a new phone number to an existing item
   virtual bool addPhoneNumber( Call* call , PhoneNumber* number ) override;
};

#endif
