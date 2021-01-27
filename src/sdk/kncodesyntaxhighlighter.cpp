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
#include <QDir>
#include <QFileInfo>
#include <QDomDocument>
#include <QTextDocument>
#include <QXmlStreamReader>

#include "kncodesyntaxhighlighter.h"

bool KNCodeSyntaxHighlighter::syntaxLoaded = false;
QHash<QString, QString> KNCodeSyntaxHighlighter::extMap = QHash<QString, QString>();
QHash<QString, QString> KNCodeSyntaxHighlighter::namePathMap = QHash<QString, QString>();

KNCodeSyntaxHighlighter::KNCodeSyntaxHighlighter(const QString &syntaxName,
                                                 QObject *parent) :
    KNSyntaxHighlighter(parent)
{
    //Load the syntax rule of the syntax name.
    loadRules(syntaxName);
}

KNSyntaxHighlighter *KNCodeSyntaxHighlighter::get(const QString &filePath)
{
    //Check whether the global syntax is loaded.
    if(!syntaxLoaded)
    {
        QHash<QString, int> syntaxPriority;
        //Load the information from the rows.
        loadDir(":/syntax/", syntaxPriority);
        //Load the syntax.
        syntaxLoaded = true;
    }
    //Extract the suffix of the file path.
    const QString &&suffix = QFileInfo(filePath).suffix();
    // Find the suffix in the global rules.
    if(extMap.contains(suffix))
    {
        return new KNCodeSyntaxHighlighter(extMap.value(suffix));
    }
    //Or else, we have to use a default syntax highlighter, which does nothing.
    return new KNSyntaxHighlighter();
}

#include <QDebug>

void KNCodeSyntaxHighlighter::loadRules(const QString &syntaxName)
{
    //Open the rule file.
    QFile ruleFile(namePathMap.value(syntaxName));
    if(!ruleFile.open(QIODevice::ReadOnly))
    {
        return;
    }
//    QDomDocument syntaxDoc;
//    if(!syntaxDoc.setContent(&ruleFile))
//    {
//        ruleFile.close();
//        return;
//    }
    QHash<QString, QString> entity;
    QXmlStreamReader reader(&ruleFile);
    reader.readNext();
    do
    {
        int type = reader.tokenType();
        switch(type)
        {
        case QXmlStreamReader::DTD:
        {
            //Build the internal entity.
            auto ent = reader.entityDeclarations();
            QTextDocument doc;
            for(auto i=0; i<ent.size(); ++i)
            {
                doc.setHtml(ent.at(i).value().toString());
                entity.insert(ent.at(i).name().toString(),
                              doc.toPlainText());
                qDebug()<<doc.toPlainText();
            }
            break;
        }
//        default:
//            qDebug()<<type<<reader.text();
        }
        //Skip to next element.
        reader.readNext();
    }
    while(reader.tokenType() > QXmlStreamReader::Invalid);
    ruleFile.close();
    //Parse the syntax file.
//    auto nodes = syntaxDoc.childNodes();
//    auto dtype = syntaxDoc.doctype();
//    auto nots = dtype.entities();
//    for(int i=0; i<nots.size(); ++i)
//    {
//        qDebug()<<nots.item(i).nodeType();
//    }
}

bool KNCodeSyntaxHighlighter::hasCodeLevel() const
{
    return true;
}

void KNCodeSyntaxHighlighter::loadDir(const QString &dirPath, QHash<QString, int> &priorityMap)
{
    //List the dir information.
    QDir syntaxDir(dirPath);
    QStringList syntaxType;
    syntaxType << "*.xml";
    auto fileList = syntaxDir.entryList(syntaxType);
    for(auto fileName : fileList)
    {
        //Load the xml information.
        loadXml(syntaxDir.filePath(fileName), priorityMap);
    }
}

void KNCodeSyntaxHighlighter::loadXml(const QString &xmlPath, QHash<QString, int> &priorityMap)
{
    //Load the XML file and only record its header.
    QFile xmlFile(xmlPath);
    if(!xmlFile.open(QIODevice::ReadOnly))
    {
        return;
    }
    //Set the document.
    QDomDocument syntaxDoc;
    if(!syntaxDoc.setContent(&xmlFile))
    {
        xmlFile.close();
        return;
    }
    xmlFile.close();
    //Show the information.
    auto header = syntaxDoc.firstChildElement("language");
    auto syntaxName = header.attribute("name"),
            syntaxExts = header.attribute("extensions");
    int syntaxPriority = header.attribute("priority").toInt();
    //Insert the file to file map.
    namePathMap.insert(syntaxName, xmlPath);
    //Also insert for its priority.
    priorityMap.insert(syntaxName, syntaxPriority);
    //Extract the syntax extensions.
    auto extensions = syntaxExts.split(';');
    for(auto ext : extensions)
    {
        if(ext.isEmpty())
        {
            continue;
        }
        int dotPos = ext.indexOf('.');
        if(dotPos > -1)
        {
            //Only save the data after first dot.
            ext = ext.mid(dotPos + 1);
        }
        if(!extMap.contains(ext) ||
                syntaxPriority > priorityMap.value(extMap.value(ext)))
        {
            //Update the extsion value.
            extMap.insert(ext, syntaxName);
        }
    }
}
