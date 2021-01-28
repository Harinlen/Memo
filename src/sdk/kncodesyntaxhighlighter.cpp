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
    QHash<QString, QString> entity;
    {
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
                }
                break;
            }
            default:
                break;
            }
            if(!entity.isEmpty())
            {
                break;
            }
            //Skip to next element.
            reader.readNext();
        }
        while(reader.tokenType() > QXmlStreamReader::Invalid);
    }
    //Now reset the file position.
    ruleFile.seek(0);
    //Construct the dom document.
    QDomDocument syntaxDoc;
    if(!syntaxDoc.setContent(&ruleFile))
    {
        ruleFile.close();
        return;
    }
    ruleFile.close();
    //Parse the syntax file.
    auto nodes = syntaxDoc.childNodes();
    for(int i=0; i<nodes.size(); ++i)
    {
        //Check the name of the nodes.
        if("language" == nodes.item(i).nodeName())
        {
            //Parse the language node.
            parseLanguageNode(nodes.item(i).toElement());
            break;
        }
    }
}

bool KNCodeSyntaxHighlighter::hasCodeLevel() const
{
    return true;
}

void KNCodeSyntaxHighlighter::parseLanguageNode(const QDomElement &lan)
{
    //Parse the internal nodes.
    auto categoryList = lan.childNodes();
    for(int i=0; i<categoryList.size(); ++i)
    {
        //Now check each element.
        auto node = categoryList.at(i);
        auto nodeName = node.nodeName();
        if(nodeName == "highlighting")
        {
            //Update the highlighting rules.
            parseHighlight(node.toElement());
        }
        else if(nodeName == "general")
        {
            //Update the general rules.
            ;
        }
    }
}

void KNCodeSyntaxHighlighter::parseHighlight(const QDomElement &highlights)
{
    //Check for all the highlight info.
    auto parts = highlights.childNodes();
    QHash<QString, QString> lists;
    for(int i=0; i<parts.size(); ++i)
    {
        //Check the part type.
        auto node = parts.at(i);
        auto nodeName = node.nodeName();
        if(nodeName == "list")
        {
            auto element = node.toElement();
            //Parse the list.
            QString listName = element.attribute("name");
            //Parse the internal items.
            auto listItems = element.childNodes();
            QStringList listContents;
            listContents.reserve(listItems.size());
            for(int j=0; j<listItems.size(); ++j)
            {
                //Append the internal data.
                if(QDomNode::ElementNode == listItems.at(j).nodeType())
                {
                    //Extract its internal content.
                    listContents.append(listItems.at(j).toElement().text());
                }
            }
            //Insert the list data to lists.
            lists.insert(listName,
                         QString("\\b(%1)\\b").arg(listContents.join('|')));
        }
        else if(nodeName == "contexts")
        {
            //Parse the contexts.
            ;
        }
        else if(nodeName == "itemDatas")
        {
            //Parse the item datas.
            ;
        }
    }
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
