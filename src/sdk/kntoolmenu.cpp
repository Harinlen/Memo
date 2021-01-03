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
#include "kntoolhash.h"
#include "knuimanager.h"
#include "knfilemanager.h"

#include "kntoolmenu.h"

KNToolMenu::KNToolMenu(QWidget *parent) :
    QMenu(parent)
{
    //Recast the parent as the file manager.
    KNFileManager *manager = static_cast<KNFileManager *>(parent);
    //Construct the tool menu.
    addMenu(new KNToolMd5(manager, this));
    addMenu(new KNToolSha256(manager, this));
    //Link the translator.
    knUi->addTranslate(this, &KNToolMenu::retranslate);
}

void KNToolMenu::retranslate()
{
    //Update the title.
    setTitle(tr("&Tools"));
}
