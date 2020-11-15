// Created by Danielle Mersch
// Copyright Unil. All rights reserved.

#include "mainwin.h"

#include <wx/wx.h>
#include <wx/sysopt.h>

class PlumeApp: public wxApp{
public:
  virtual bool OnInit();
};

DECLARE_APP(PlumeApp)

IMPLEMENT_APP(PlumeApp)

bool PlumeApp::OnInit(){
//wxSystemOptions::SetOption(wxT("mac.listctrl.always_use_generic"), true);
MainWin* mw = new MainWin((wxWindow*) NULL);
mw->Show();
return true;
}

