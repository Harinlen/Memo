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
#include "knsingletonapplication.h"
#include "knversion.h"
#include "knglobal.h"

#include "knmainwindow.h"

int main(int argc, char *argv[])
{
    //Create a singleton pattern application.
    KNSingletonApplication app(argc, argv, APP_HANDLER);
    //Check the running status of the instance.
    if(!app.isInstanceRunning())
    {
        //Current instance is not running, means it finds another running
        //instance. Exit the current instance.
        return EXIT_SUCCESS;
    }
    //Construct the main window.
    KNMainWindow mainWindow;
    //Show the main window.
    knGlobal->startUp();
    return app.exec();
}
