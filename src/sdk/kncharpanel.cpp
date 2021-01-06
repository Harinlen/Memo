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
#include <QStandardItemModel>

#include "knuimanager.h"

#include "kncharpanel.h"

KNCharPanel::KNCharPanel(QWidget *parent) :
    QTreeView(parent),
    m_model(new QStandardItemModel(256, 5, this))
{
    //Create the model.
    for(int i=0; i<256; ++i)
    {
        //Update the data.
        m_model->setData(m_model->index(i, 0), QString::number(i));
        QString hexId = QString::number(i, 16);
        if(hexId.length() < 2)
        {
            hexId = "0" + hexId;
        }
        m_model->setData(m_model->index(i, 1), hexId);
    }
    //For 0 to 32, set the char.
    m_model->setData(m_model->index(0, 2), "NULL");
    m_model->setData(m_model->index(1, 2), "SOH");
    m_model->setData(m_model->index(2, 2), "STX");
    m_model->setData(m_model->index(3, 2), "ETX");
    m_model->setData(m_model->index(4, 2), "EOT");
    m_model->setData(m_model->index(5, 2), "ENQ");
    m_model->setData(m_model->index(6, 2), "ACK");
    m_model->setData(m_model->index(7, 2), "BEL");
    m_model->setData(m_model->index(8, 2), "BS");
    m_model->setData(m_model->index(9, 2), "TAB");
    m_model->setData(m_model->index(10, 2), "LF");
    m_model->setData(m_model->index(11, 2), "VT");
    m_model->setData(m_model->index(12, 2), "FF");
    m_model->setData(m_model->index(13, 2), "CR");
    m_model->setData(m_model->index(14, 2), "SO");
    m_model->setData(m_model->index(15, 2), "SI");
    m_model->setData(m_model->index(16, 2), "DLE");
    m_model->setData(m_model->index(17, 2), "DC1");
    m_model->setData(m_model->index(18, 2), "DC2");
    m_model->setData(m_model->index(19, 2), "DC3");
    m_model->setData(m_model->index(20, 2), "DC4");
    m_model->setData(m_model->index(21, 2), "NAK");
    m_model->setData(m_model->index(22, 2), "SYN");
    m_model->setData(m_model->index(23, 2), "ETB");
    m_model->setData(m_model->index(24, 2), "CAN");
    m_model->setData(m_model->index(25, 2), "EM");
    m_model->setData(m_model->index(26, 2), "SUB");
    m_model->setData(m_model->index(27, 2), "ESC");
    m_model->setData(m_model->index(28, 2), "FS");
    m_model->setData(m_model->index(29, 2), "GS");
    m_model->setData(m_model->index(30, 2), "RS");
    m_model->setData(m_model->index(31, 2), "US");
    for(int i=0; i<32; ++i)
    {
        m_model->setData(m_model->index(i, 3), "");
        m_model->setData(m_model->index(i, 4), "");
    }
    m_model->setData(m_model->index(32, 2), "Space");
    for(int i=33; i<127; ++i)
    {
        m_model->setData(m_model->index(i, 2), QChar(static_cast<char>(i)));
    }
    for(int i=32; i<127; ++i)
    {
        m_model->setData(m_model->index(i, 3), "&#"+QString::number(i));
        m_model->setData(m_model->index(i, 4), "");
    }
    m_model->setData(m_model->index(34, 4), "&quot;");
    m_model->setData(m_model->index(38, 4), "&amp;");
    m_model->setData(m_model->index(60, 4), "&lt;");
    m_model->setData(m_model->index(62, 4), "&gt;");
    m_model->setData(m_model->index(127, 2), "DEL");
    m_model->setData(m_model->index(127, 3), "");
    m_model->setData(m_model->index(127, 4), "");
    m_model->setData(m_model->index(128, 2), QChar(static_cast<char>(128)));
    m_model->setData(m_model->index(128, 3), "&#8364");
    m_model->setData(m_model->index(128, 4), "&euro;");
    for(int i=129; i<256; ++i)
    {
        m_model->setData(m_model->index(i, 2), "?");
    }
    for(int i=129; i<160; ++i)
    {
        m_model->setData(m_model->index(i, 3), "");
    }
    m_model->setData(m_model->index(130, 3), "&#8218");
    m_model->setData(m_model->index(131, 3), "&#402");
    m_model->setData(m_model->index(132, 3), "&#82222");
    m_model->setData(m_model->index(133, 3), "&#82230");
    m_model->setData(m_model->index(134, 3), "&#82224");
    m_model->setData(m_model->index(135, 3), "&#82225");
    m_model->setData(m_model->index(137, 3), "&#82240");
    m_model->setData(m_model->index(138, 3), "&#352");
    m_model->setData(m_model->index(140, 3), "&#338");
    m_model->setData(m_model->index(145, 3), "&#8216");
    m_model->setData(m_model->index(146, 3), "&#8217");
    m_model->setData(m_model->index(147, 3), "&#8220");
    m_model->setData(m_model->index(148, 3), "&#8221");
    m_model->setData(m_model->index(149, 3), "&#8226");
    m_model->setData(m_model->index(150, 3), "&#8211");
    m_model->setData(m_model->index(151, 3), "&#8212");
    m_model->setData(m_model->index(153, 3), "&#8482");
    m_model->setData(m_model->index(154, 3), "&#353");
    m_model->setData(m_model->index(156, 3), "&#339");
    m_model->setData(m_model->index(159, 3), "&#376");
    for(int i=160; i<256; ++i)
    {
        m_model->setData(m_model->index(i, 3), "&#"+QString::number(i));
    }
    for(int i=129; i<160; ++i)
    {
        m_model->setData(m_model->index(i, 4), "");
    }
    m_model->setData(m_model->index(160, 4), "&nbsp;");
    m_model->setData(m_model->index(161, 4), "&iexcl;");
    m_model->setData(m_model->index(162, 4), "&cent;");
    m_model->setData(m_model->index(163, 4), "&pound;");
    m_model->setData(m_model->index(164, 4), "&curren;");
    m_model->setData(m_model->index(165, 4), "&yen;");
    m_model->setData(m_model->index(166, 4), "&brvbar;");
    m_model->setData(m_model->index(167, 4), "&sect;");
    m_model->setData(m_model->index(168, 4), "&uml;");
    m_model->setData(m_model->index(169, 4), "&copy;");
    m_model->setData(m_model->index(170, 4), "&ordf;");
    m_model->setData(m_model->index(171, 4), "&laquo;");
    m_model->setData(m_model->index(172, 4), "&not;");
    m_model->setData(m_model->index(173, 4), "&shy;");
    m_model->setData(m_model->index(174, 4), "&reg;");
    m_model->setData(m_model->index(175, 4), "&macr;");
    m_model->setData(m_model->index(176, 4), "&deg;");
    m_model->setData(m_model->index(177, 4), "&plusmn;");
    m_model->setData(m_model->index(178, 4), "&sup2;");
    m_model->setData(m_model->index(179, 4), "&sup3;");
    m_model->setData(m_model->index(180, 4), "&acute;");
    m_model->setData(m_model->index(181, 4), "&micro;");
    m_model->setData(m_model->index(182, 4), "&para;");
    m_model->setData(m_model->index(183, 4), "&middot;");
    m_model->setData(m_model->index(184, 4), "&cedil;");
    m_model->setData(m_model->index(185, 4), "&sup1;");
    m_model->setData(m_model->index(186, 4), "&ordm;");
    m_model->setData(m_model->index(187, 4), "&raquo;");
    m_model->setData(m_model->index(188, 4), "&frac14;");
    m_model->setData(m_model->index(189, 4), "&frac12;");
    m_model->setData(m_model->index(190, 4), "&frac34;");
    m_model->setData(m_model->index(191, 4), "&iquest;");
    m_model->setData(m_model->index(192, 4), "&Agrave;");
    m_model->setData(m_model->index(193, 4), "&Aacute;");
    m_model->setData(m_model->index(194, 4), "&Acirc;");
    m_model->setData(m_model->index(195, 4), "&Atilde;");
    m_model->setData(m_model->index(196, 4), "&Auml;");
    m_model->setData(m_model->index(197, 4), "&Aring;");
    m_model->setData(m_model->index(198, 4), "&AElig;");
    m_model->setData(m_model->index(199, 4), "&Ccedil;");
    m_model->setData(m_model->index(200, 4), "&Egrave;");
    m_model->setData(m_model->index(201, 4), "&Eacute;");
    m_model->setData(m_model->index(202, 4), "&Ecirc;");
    m_model->setData(m_model->index(203, 4), "&Euml;");
    m_model->setData(m_model->index(204, 4), "&Igrave;");
    m_model->setData(m_model->index(205, 4), "&Iacute;");
    m_model->setData(m_model->index(206, 4), "&Icirc;");
    m_model->setData(m_model->index(207, 4), "&Iuml;");
    m_model->setData(m_model->index(208, 4), "&ETH;");
    m_model->setData(m_model->index(209, 4), "&Ntilde;");
    m_model->setData(m_model->index(210, 4), "&Ograve;");
    m_model->setData(m_model->index(211, 4), "&Oacute;");
    m_model->setData(m_model->index(212, 4), "&Ocirc;");
    m_model->setData(m_model->index(213, 4), "&Otilde;");
    m_model->setData(m_model->index(214, 4), "&Ouml;");
    m_model->setData(m_model->index(215, 4), "&times;");
    m_model->setData(m_model->index(216, 4), "&Oslash;");
    m_model->setData(m_model->index(217, 4), "&Ugrave;");
    m_model->setData(m_model->index(218, 4), "&Uacute;");
    m_model->setData(m_model->index(219, 4), "&Ucirc;");
    m_model->setData(m_model->index(220, 4), "&Uuml;");
    m_model->setData(m_model->index(221, 4), "&Yacute;");
    m_model->setData(m_model->index(222, 4), "&THORN;");
    m_model->setData(m_model->index(223, 4), "&szlig;");
    m_model->setData(m_model->index(224, 4), "&agrave;");
    m_model->setData(m_model->index(225, 4), "&aacute;");
    m_model->setData(m_model->index(226, 4), "&acirc;");
    m_model->setData(m_model->index(227, 4), "&atilde;");
    m_model->setData(m_model->index(228, 4), "&auml;");
    m_model->setData(m_model->index(229, 4), "&aring;");
    m_model->setData(m_model->index(230, 4), "&aelig;");
    m_model->setData(m_model->index(231, 4), "&ccedil;");
    m_model->setData(m_model->index(232, 4), "&egrave;");
    m_model->setData(m_model->index(233, 4), "&eacute;");
    m_model->setData(m_model->index(234, 4), "&ecirc;");
    m_model->setData(m_model->index(235, 4), "&euml;");
    m_model->setData(m_model->index(236, 4), "&igrave;");
    m_model->setData(m_model->index(237, 4), "&iacute;");
    m_model->setData(m_model->index(238, 4), "&icirc;");
    m_model->setData(m_model->index(239, 4), "&iuml;");
    m_model->setData(m_model->index(240, 4), "&eth;");
    m_model->setData(m_model->index(241, 4), "&ntilde;");
    m_model->setData(m_model->index(242, 4), "&ograve;");
    m_model->setData(m_model->index(243, 4), "&oacute;");
    m_model->setData(m_model->index(244, 4), "&ocirc;");
    m_model->setData(m_model->index(245, 4), "&otilde;");
    m_model->setData(m_model->index(246, 4), "&ouml;");
    m_model->setData(m_model->index(247, 4), "&divide;");
    m_model->setData(m_model->index(248, 4), "&oslash;");
    m_model->setData(m_model->index(249, 4), "&ugrave;");
    m_model->setData(m_model->index(250, 4), "&uacute;");
    m_model->setData(m_model->index(251, 4), "&ucirc;");
    m_model->setData(m_model->index(252, 4), "&uuml;");
    m_model->setData(m_model->index(253, 4), "&yacute;");
    m_model->setData(m_model->index(254, 4), "&thorn;");
    m_model->setData(m_model->index(255, 4), "&yuml;");
    //Configure the tree view.
    setIndentation(0);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setModel(m_model);
    //Configure the activate signals.
    connect(this, &QTreeView::activated,
            this, &KNCharPanel::onActivate);
    //Link the translator.
    knUi->addTranslate(this, &KNCharPanel::retranslate);
}

void KNCharPanel::retranslate()
{
    //Update the title.
    m_model->setHeaderData(0, Qt::Horizontal, tr("Value"), Qt::DisplayRole);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Hex"), Qt::DisplayRole);
    m_model->setHeaderData(2, Qt::Horizontal, tr("Character"), Qt::DisplayRole);
    m_model->setHeaderData(3, Qt::Horizontal, tr("HTML Number"), Qt::DisplayRole);
    m_model->setHeaderData(4, Qt::Horizontal, tr("HTML Name"), Qt::DisplayRole);
    //Adjust the column.
    for(int i=0; i<4; ++i)
    {
        resizeColumnToContents(i);
    }
}

void KNCharPanel::onActivate(const QModelIndex &index)
{
    //Check the column of the item.
    switch(index.column())
    {
    case 0:
    case 1:
    case 3:
    case 4:
        //Insert the text directly.
        emit requireInsert(m_model->data(index, Qt::DisplayRole).toString());
        return;
    case 2:
        emit requireInsert(QChar(static_cast<char>(index.row())));
        break;
    }
}

