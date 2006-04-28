/*
    CsoundGUI.hpp:
    Copyright (C) 2006 Istvan Varga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#ifndef CSOUNDGUI_HPP
#define CSOUNDGUI_HPP

#include "csound.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_File_Chooser.H>

#include "CsoundPerformanceSettings.hpp"
#include "CsoundGlobalSettings.hpp"
#include "CsoundPerformance.hpp"

class CsoundGUIMain;

#include "CsoundGlobalSettingsPanel_FLTK.hpp"
#include "CsoundPerformanceSettingsPanel_FLTK.hpp"

struct Csound_Message {
    Csound_Message  *nxt;
    int             attr;
    char            msg[1];
};

#include "CsoundGUIConsole_FLTK.hpp"
#include "ConfigFile.hpp"
#include "CsoundUtility.hpp"
#include "CsoundUtilitiesWindow_FLTK.hpp"
#include "CsoundGUIMain_FLTK.hpp"

#define CSOUND5GUI_GCFGWIN_OPEN         1
#define CSOUND5GUI_PCFGWIN_OPEN         2
#define CSOUND5GUI_UTILWIN_OPEN         4
#define CSOUND5GUI_LISTOPCODES_RUNNING  256
#define CSOUND5GUI_CVANAL_RUNNING       512
#define CSOUND5GUI_PVANAL_RUNNING       1024

#endif  // CSOUNDGUI_HPP

