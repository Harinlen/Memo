/*
 * Copyright (C) Kreogist Dev Team
 *
 * You can redistribute this software and/or modify it under the
 * terms of the HARERU Software License; either version 1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * license file for more details.
 */
#include <QJsonValue>
#include <QJsonArray>
#include <QApplication>
#include <QKeySequence>
#include <QFont>
#include <QMetaType>

#include "knconfigure.h"

#include <QDebug>

QHash<QString, int> KNConfigure::m_typeList=QHash<QString, int>();

KNConfigure::KNConfigure(QObject *parent) :
    QObject(parent),
    m_dataObject(QJsonObject()),
    m_caption(QString())
{
    //Check and build type list.
    buildTypeList();
}

KNConfigure::KNConfigure(const QJsonObject &data,
                         QObject *parent) :
    QObject(parent),
    m_dataObject(data),
    m_caption(QString())
{
    //Check and build type list.
    buildTypeList();
}

QJsonObject KNConfigure::dataObject()
{
    //Update the data object first.
    for(auto i=m_subObjectTable.constBegin(); i!=m_subObjectTable.constEnd();
        ++i)
    {
        //Update the object according to the key.
        m_dataObject.insert(i.key(), i.value()->dataObject());
    }
    //Return the new data object.
    return m_dataObject;
}

KNConfigure *KNConfigure::getConfigure(const QString &key)
{
    //Check if we can find the key in the hash table.
    if(m_subObjectTable.contains(key))
    {
        return m_subObjectTable.value(key);
    }
    //Check whether the key can be found in the object.
    //Get the key object or set the empty object.
    QJsonObject keyObject=
            m_dataObject.contains(key)?
                m_dataObject.value(key).toObject():
                QJsonObject();
    //Construct a KNConfigure.
    KNConfigure *subConfigure=new KNConfigure(keyObject, this);
    //Save the configure in the hash table.
    m_subObjectTable.insert(key, subConfigure);
    //Return the sub configure.
    return subConfigure;
}

QVariant KNConfigure::parseData(const QJsonValue &value,
                                const QVariant &defaultValue)
{
    //Check the type of the value.
    switch(value.type())
    {
    //If the value type is bool, double, string and array, translate the
    //data to the specific type and use the default construct function of
    //the QVariant to build the value.
    case QJsonValue::Bool:
        return value.toBool();
    case QJsonValue::Double:
        return value.toDouble();
    case QJsonValue::String:
        return value.toString();
    case QJsonValue::Array:
        return QVariant(value.toArray());
    //If the type of the value is object, we have to find whether the object
    //contains a key value called "Type"
    case QJsonValue::Object:
    {
        QJsonObject valueObject=value.toObject();
        if(valueObject.contains("Type"))
        {
            //Find the type string data in the hash list.
            int objectType=
                    m_typeList.value(valueObject.value("Type").toString(),
                                     -1);
            //If we cannot find the object type(-1), return the value as a
            //object.
            if(objectType==-1)
            {
                return valueObject;
            }
            //Parse the object according to the type.
            switch(objectType)
            {
            case Font:
            {
                QFont valueFont=QApplication::font();
                valueFont.setFamily(valueObject.value("Family").toString());
                valueFont.setPixelSize(valueObject.value(
                                           "Size").toDouble());
                valueFont.setBold(valueObject.value("Bold").toBool());
                valueFont.setItalic(valueObject.value("Italic").toBool());
                valueFont.setUnderline(valueObject.value(
                                           "Underline").toBool());
                valueFont.setStrikeOut(valueObject.value(
                                           "Strikeout").toBool());
                valueFont.setKerning(valueObject.value("Kerning").toBool());
                return QVariant::fromValue(valueFont);
            }
            case Shortcut:
            {
                //Parse the value from the object.
                QKeySequence valueShortcut(
                            valueObject.value("Key").toString(),
                            QKeySequence::PortableText);
                //Give back the value data.
                return QVariant::fromValue(valueShortcut);
            }
            case CustomObject:
            {
                //Simply get the object data back.
                return valueObject;
            }
            }
        }
        return valueObject;
    }
    //The last type of the data is Null and Undefined. Return the default
    //value.
    default:
        return defaultValue;
    }
}

QVariant KNConfigure::data(const QString &key,
                           const QVariant &defaultValue)
{
    //Find the key in the data object.
    if(m_dataObject.contains(key))
    {
        //Get and parse the data first.
        return parseData(m_dataObject.value(key), defaultValue);
    }
    //Save the data to the object.
    setData(key, defaultValue);
    //Return the default value if we cannot find the key.
    return defaultValue;
}

bool KNConfigure::contains(const QString &key) const
{
    //Simply check the key in the data object.
    return m_dataObject.contains(key);
}

QVariant KNConfigure::getPathData(const QStringList &path,
                                  const QVariant &defaultValue)
{
    //Check the path size.
    if(path.size()==1)
    {
        //Which is the current level.
        return data(path.first(), defaultValue);
    }
    //Or else get the sub configure value.
    return getConfigure(path.first())->getPathData(path.mid(1), defaultValue);
}

void KNConfigure::setData(const QString &key, const QVariant &value)
{
    //Check whether the value is null.
    if(value.isNull())
    {
        //Remove the key from the configure data.
        m_dataObject.remove(key);
        //Complete.
        return;
    }
    //Because the QJsonObject can only insert QJsonValue, and the construct
    //function of QJsonValue only have the following types:
    //   bool, QString, array, double, object.
    //So we have to translate some complex type variant to object.
#if QT_VERSION_MAJOR > 5
    switch(value.metaType().id())
#else
    switch(static_cast<QMetaType::Type>(value.type()))
#endif
    {
    //For the basic types(double, float, int, bool, QString), we will save them
    //directly.
    case QMetaType::LongLong:
        m_dataObject.insert(key, value.toLongLong());
        break;
    case QMetaType::Double:
        m_dataObject.insert(key, value.toDouble());
        break;
    case QMetaType::QString:
        m_dataObject.insert(key, value.toString());
        break;
    case QMetaType::Int:
        m_dataObject.insert(key, value.toInt());
        break;
    case QMetaType::Bool:
        m_dataObject.insert(key, value.toBool());
        break;
    //For advanced types(like Font), we have to translate them to a object.
    case QMetaType::QFont:
    {
        //Generate the font object.
        QFont font=value.value<QFont>();
        QJsonObject fontObject;
        fontObject.insert("Type", QString("Font"));
        fontObject.insert("Family", font.family());
        fontObject.insert("Size", font.pointSize());
        fontObject.insert("Bold", font.bold());
        fontObject.insert("Italic", font.italic());
        fontObject.insert("Underline", font.underline());
        fontObject.insert("Strikeout", font.strikeOut());
        fontObject.insert("Kerning", font.kerning());
        //Insert the font object.
        m_dataObject.insert(key, fontObject);
        break;
    }
    case QMetaType::QKeySequence:
    {
        //Generate a key sequence object.
        QKeySequence keySequence=value.value<QKeySequence>();
        //A shortcut object.
        QJsonObject shortcutObject;
        shortcutObject.insert("Type", QString("Shortcut"));
        shortcutObject.insert("Key", keySequence.toString(
                                  QKeySequence::PortableText));
        //Insert the key sequence object.
        m_dataObject.insert(key, shortcutObject);
        break;
    }
    //For the JSON object, it is quite wired that QVariant doesn't enumerate the
    //JSON type here, but it will use the meta type json object here.
    case QMetaType::QJsonObject:
    {
        //Check with the value contains the custom type.
        QJsonObject customObject=value.toJsonObject();
        //Check the custom object type is matched.
        if("CustomObject"==customObject.value("Type").toString())
        {
            //Simply insert the json object to the data.
            m_dataObject.insert(key, customObject);
        }
        break;
    }
    //For the JSON array, directly save the original data.
    case QMetaType::QJsonArray:
    {
        //Simply insert the json array to the data.
        m_dataObject.insert(key, value.toJsonArray());
        break;
    }
    default:
        return;
    }
    //Emit the signal.
    emit valueChanged();
}

void KNConfigure::remove(const QString &key)
{
    //Remove the key from the object.
    m_dataObject.remove(key);
}

void KNConfigure::setPathData(const QStringList &path, const QVariant &value)
{
    //Check the path depth.
    if(path.size()==1)
    {
        //Set the data.
        setData(path.first(), value);
        //Mission complete.
        return;
    }
    //Or else, call the deeper value.
    getConfigure(path.first())->setPathData(path.mid(1), value);
}

void KNConfigure::setDataObject(const QJsonObject &dataObject)
{
    //Save the data object first.
    m_dataObject=dataObject;
    //Give the new data to the sub object managers.
    for(auto i=m_subObjectTable.begin(); i!=m_subObjectTable.end(); ++i)
    {
        i.value()->setDataObject(
                    m_dataObject.value(i.key()).toObject(QJsonObject()));
    }
}

inline void KNConfigure::buildTypeList()
{
    //Check whether the type list has been built before.
    if(m_typeList.isEmpty())
    {
        m_typeList.insert("Font", Font);
        m_typeList.insert("Shortcut", Shortcut);
        m_typeList.insert("CustomObject", CustomObject);
    }
}
