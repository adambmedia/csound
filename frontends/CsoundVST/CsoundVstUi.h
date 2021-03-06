// generated by Fast Light User Interface Designer (fluid) version 1.0108

#ifndef CsoundVstUi_h
#define CsoundVstUi_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "CsoundVstFltk.hpp"
#include <FL/Fl_Button.H>
extern void onNew(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *newButton;
extern void onNewVersion(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *newVersionButton;
extern void onOpen(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *openButton;
extern void onImport(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *importButton;
extern void onSave(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *saveButton;
extern void onSaveAs(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *saveAsButton;
extern void onPerform(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *performButton;
extern void onStop(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *stopPerformingButton;
extern void onEdit(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *editButton;
extern void onSettingsApply(Fl_Button*, CsoundVstFltk*);
extern Fl_Button *settingsApplyButton;
#include <FL/Fl_Tabs.H>
extern Fl_Tabs *mainTabs;
#include <FL/Fl_Group.H>
extern Fl_Group *settingsGroup;
#include <FL/Fl_Check_Button.H>
extern void onSettingsVstPluginMode(Fl_Check_Button*, CsoundVstFltk*);
extern Fl_Check_Button *settingsVstPluginModeEffect;
extern void onSettingsVstInstrumentMode(Fl_Check_Button*, CsoundVstFltk*);
extern Fl_Check_Button *settingsVstPluginModeInstrument;
#include <FL/Fl_Input.H>
extern Fl_Input *commandInput;
extern Fl_Input *settingsEditSoundfileInput;
#include <FL/Fl_Browser.H>
extern Fl_Browser *runtimeMessagesBrowser;
extern Fl_Group *orchestraGroup;
#include <FL/Fl_Text_Editor.H>
extern Fl_Text_Editor *orchestraTextEdit;
extern Fl_Group *scoreGroup;
extern Fl_Text_Editor *scoreTextEdit;
extern Fl_Group *aboutGroup;
#include <FL/Fl_Text_Display.H>
extern Fl_Text_Display *aboutTextDisplay;
Fl_Double_Window* make_window(CsoundVstFltk *csoundVstFltk);
#endif
