/*
 * This file was generated by dbusxml2cpp version 0.6
 * Command line was: dbusxml2cpp -c ConfigurationManagerInterface -p configurationmanager_interface_p.h:configurationmanager_interface.cpp -i metatypes.h configurationmanager-introspec.xml
 *
 * dbusxml2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef CONFIGURATIONMANAGER_INTERFACE_P_H_1238787208
#define CONFIGURATIONMANAGER_INTERFACE_P_H_1238787208

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "metatypes.h"

/*
 * Proxy class for interface org.sflphone.SFLphone.ConfigurationManager
 */
class ConfigurationManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.sflphone.SFLphone.ConfigurationManager"; }

public:
    ConfigurationManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~ConfigurationManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusReply<QString> addAccount(MapStringString details)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(details);
        return callWithArgumentList(QDBus::Block, QLatin1String("addAccount"), argumentList);
    }

    inline QDBusReply<void> enableStun()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("enableStun"), argumentList);
    }

    inline QDBusReply<MapStringString> getAccountDetails(const QString &accountID)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(accountID);
        return callWithArgumentList(QDBus::Block, QLatin1String("getAccountDetails"), argumentList);
    }

    inline QDBusReply<QStringList> getAccountList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAccountList"), argumentList);
    }

    inline QDBusReply<QStringList> getActiveCodecList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getActiveCodecList"), argumentList);
    }

    inline QDBusReply<MapStringInt> getAddressbookSettings()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAddressbookSettings"), argumentList);
    }

    inline QDBusReply<int> getAudioDeviceIndex(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(name);
        return callWithArgumentList(QDBus::Block, QLatin1String("getAudioDeviceIndex"), argumentList);
    }

    inline QDBusReply<QStringList> getAudioInputDeviceList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAudioInputDeviceList"), argumentList);
    }

    inline QDBusReply<int> getAudioManager()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAudioManager"), argumentList);
    }

    inline QDBusReply<QStringList> getAudioOutputDeviceList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getAudioOutputDeviceList"), argumentList);
    }

    inline QDBusReply<QStringList> getCodecDetails(int payload)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(payload);
        return callWithArgumentList(QDBus::Block, QLatin1String("getCodecDetails"), argumentList);
    }

    inline QDBusReply<QStringList> getCodecList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getCodecList"), argumentList);
    }

    inline QDBusReply<QStringList> getCurrentAudioDevicesIndex()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getCurrentAudioDevicesIndex"), argumentList);
    }

    inline QDBusReply<QString> getCurrentAudioOutputPlugin()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getCurrentAudioOutputPlugin"), argumentList);
    }

    inline QDBusReply<int> getDialpad()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getDialpad"), argumentList);
    }

    inline QDBusReply<MapStringString> getHookSettings()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getHookSettings"), argumentList);
    }

    inline QDBusReply<QStringList> getInputAudioPluginList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getInputAudioPluginList"), argumentList);
    }

    inline QDBusReply<int> getMailNotify()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getMailNotify"), argumentList);
    }

    inline QDBusReply<int> getMaxCalls()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getMaxCalls"), argumentList);
    }

    inline QDBusReply<int> getNotify()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getNotify"), argumentList);
    }

    inline QDBusReply<QStringList> getOutputAudioPluginList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getOutputAudioPluginList"), argumentList);
    }

    inline QDBusReply<QStringList> getPlaybackDeviceList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getPlaybackDeviceList"), argumentList);
    }

    inline QDBusReply<int> getPulseAppVolumeControl()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getPulseAppVolumeControl"), argumentList);
    }

    inline QDBusReply<QStringList> getRecordDeviceList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getRecordDeviceList"), argumentList);
    }

    inline QDBusReply<QString> getRecordPath()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getRecordPath"), argumentList);
    }

    inline QDBusReply<QString> getRingtoneChoice()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getRingtoneChoice"), argumentList);
    }

    inline QDBusReply<QStringList> getRingtoneList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getRingtoneList"), argumentList);
    }

    inline QDBusReply<int> getSearchbar()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getSearchbar"), argumentList);
    }

    inline QDBusReply<int> getSipPort()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getSipPort"), argumentList);
    }

    inline QDBusReply<QString> getStunServer()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getStunServer"), argumentList);
    }

    inline QDBusReply<QStringList> getToneLocaleList()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getToneLocaleList"), argumentList);
    }

    inline QDBusReply<QString> getVersion()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getVersion"), argumentList);
    }

    inline QDBusReply<int> getVolumeControls()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("getVolumeControls"), argumentList);
    }

    inline QDBusReply<int> isIax2Enabled()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("isIax2Enabled"), argumentList);
    }

    inline QDBusReply<int> isRingtoneEnabled()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("isRingtoneEnabled"), argumentList);
    }

    inline QDBusReply<int> isStartHidden()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("isStartHidden"), argumentList);
    }

    inline QDBusReply<int> isStunEnabled()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("isStunEnabled"), argumentList);
    }

    inline QDBusReply<int> popupMode()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("popupMode"), argumentList);
    }

    inline QDBusReply<void> removeAccount(const QString &accoundID)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(accoundID);
        return callWithArgumentList(QDBus::Block, QLatin1String("removeAccount"), argumentList);
    }

    inline QDBusReply<void> ringtoneEnabled()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("ringtoneEnabled"), argumentList);
    }

    inline QDBusReply<void> sendRegister(const QString &accountID, int expire)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(accountID) << qVariantFromValue(expire);
        return callWithArgumentList(QDBus::Block, QLatin1String("sendRegister"), argumentList);
    }

    inline QDBusReply<void> setAccountDetails(const QString &accountID, MapStringString details)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(accountID) << qVariantFromValue(details);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAccountDetails"), argumentList);
    }

    inline QDBusReply<void> setActiveCodecList(const QStringList &list)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(list);
        return callWithArgumentList(QDBus::Block, QLatin1String("setActiveCodecList"), argumentList);
    }

    inline QDBusReply<void> setAddressbookSettings(MapStringInt settings)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(settings);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAddressbookSettings"), argumentList);
    }

    inline QDBusReply<void> setAudioInputDevice(int index)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(index);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAudioInputDevice"), argumentList);
    }

    inline QDBusReply<void> setAudioManager(int api)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(api);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAudioManager"), argumentList);
    }

    inline QDBusReply<void> setAudioOutputDevice(int index)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(index);
        return callWithArgumentList(QDBus::Block, QLatin1String("setAudioOutputDevice"), argumentList);
    }

    inline QDBusReply<void> setDialpad()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setDialpad"), argumentList);
    }

    inline QDBusReply<void> setHookSettings(MapStringString settings)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(settings);
        return callWithArgumentList(QDBus::Block, QLatin1String("setHookSettings"), argumentList);
    }

    inline QDBusReply<void> setInputAudioPlugin(const QString &audioPlugin)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(audioPlugin);
        return callWithArgumentList(QDBus::Block, QLatin1String("setInputAudioPlugin"), argumentList);
    }

    inline QDBusReply<void> setMailNotify()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setMailNotify"), argumentList);
    }

    inline QDBusReply<void> setMaxCalls(int calls)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(calls);
        return callWithArgumentList(QDBus::Block, QLatin1String("setMaxCalls"), argumentList);
    }

    inline QDBusReply<void> setNotify()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setNotify"), argumentList);
    }

    inline QDBusReply<void> setOutputAudioPlugin(const QString &audioPlugin)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(audioPlugin);
        return callWithArgumentList(QDBus::Block, QLatin1String("setOutputAudioPlugin"), argumentList);
    }

    inline QDBusReply<void> setPulseAppVolumeControl()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setPulseAppVolumeControl"), argumentList);
    }

    inline QDBusReply<void> setRecordPath(const QString &rec)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(rec);
        return callWithArgumentList(QDBus::Block, QLatin1String("setRecordPath"), argumentList);
    }

    inline QDBusReply<void> setRingtoneChoice(const QString &tone)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(tone);
        return callWithArgumentList(QDBus::Block, QLatin1String("setRingtoneChoice"), argumentList);
    }

    inline QDBusReply<void> setSearchbar()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setSearchbar"), argumentList);
    }

    inline QDBusReply<void> setSipPort(int port)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(port);
        return callWithArgumentList(QDBus::Block, QLatin1String("setSipPort"), argumentList);
    }

    inline QDBusReply<void> setStunServer(const QString &server)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(server);
        return callWithArgumentList(QDBus::Block, QLatin1String("setStunServer"), argumentList);
    }

    inline QDBusReply<void> setVolumeControls()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("setVolumeControls"), argumentList);
    }

    inline QDBusReply<void> startHidden()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("startHidden"), argumentList);
    }

    inline QDBusReply<void> switchPopupMode()
    {
        QList<QVariant> argumentList;
        return callWithArgumentList(QDBus::Block, QLatin1String("switchPopupMode"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void accountsChanged();
    void errorAlert(int code);
    void parametersChanged(MapStringString details);
};

namespace org {
  namespace sflphone {
    namespace SFLphone {
      typedef ::ConfigurationManagerInterface ConfigurationManager;
    }
  }
}
#endif
