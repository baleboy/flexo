/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -c DbusIfAdaptor -a dbusifadaptor.h:dbusifadaptor.cpp com.balenet.flexo.xml
 *
 * qdbusxml2cpp is Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "dbusifadaptor.h"

/*
 * Implementation of adaptor class DbusIfAdaptor
 */

DBusIfAdaptor::DBusIfAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

DBusIfAdaptor::~DBusIfAdaptor()
{
    // destructor
}

void DBusIfAdaptor::Activate()
{
    // handle method call com.balenet.flexo.Activate
    QMetaObject::invokeMethod(parent(), "Activate");
}
