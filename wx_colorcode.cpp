///////////////////////////////////////////////////////////////////////////// C++ code generated with wxFormBuilder (version Sep 12 2010)// http://www.wxformbuilder.org///// PLEASE DO "NOT" EDIT THIS FILE!/////////////////////////////////////////////////////////////////////////////Copyright (c) Danielle Mersch. All rights reserved.#include "wx_colorcode.h"///////////////////////////////////////////////////////////////////////////ColorWindow::ColorWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ){	this->SetSizeHints( wxDefaultSize, wxDefaultSize );		wxBoxSizer* bSizer4;	bSizer4 = new wxBoxSizer( wxVERTICAL );		wxBoxSizer* bSizer5;	bSizer5 = new wxBoxSizer( wxHORIZONTAL );		m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Yellow areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText1->Wrap( -1 );	bSizer5->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );		yellow_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	yellow_n->SetMaxLength( 50 ); 	bSizer5->Add( yellow_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );		wxBoxSizer* bSizer6;	bSizer6 = new wxBoxSizer( wxHORIZONTAL );		m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Orange areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText2->Wrap( -1 );	bSizer6->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );		orange_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	orange_n->SetMaxLength( 50 ); 	bSizer6->Add( orange_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer6, 1, wxEXPAND, 5 );		wxBoxSizer* bSizer7;	bSizer7 = new wxBoxSizer( wxHORIZONTAL );		m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Red areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText3->Wrap( -1 );	bSizer7->Add( m_staticText3, 0, wxALIGN_CENTER|wxALL, 5 );		red_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	red_n->SetMaxLength( 50 ); 	bSizer7->Add( red_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer7, 1, wxEXPAND, 5 );		wxBoxSizer* bSizer8;	bSizer8 = new wxBoxSizer( wxHORIZONTAL );		m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("Purple areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText4->Wrap( -1 );	bSizer8->Add( m_staticText4, 0, wxALIGN_CENTER|wxALL, 5 );		purple_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	purple_n->SetMaxLength( 50 ); 	bSizer8->Add( purple_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer8, 1, wxEXPAND, 5 );		wxBoxSizer* bSizer9;	bSizer9 = new wxBoxSizer( wxHORIZONTAL );		m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Blue areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText5->Wrap( -1 );	bSizer9->Add( m_staticText5, 0, wxALIGN_CENTER|wxALL, 5 );		blue_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	blue_n->SetMaxLength( 50 ); 	bSizer9->Add( blue_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer9, 1, wxEXPAND, 5 );		wxBoxSizer* bSizer10;	bSizer10 = new wxBoxSizer( wxHORIZONTAL );		m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Green areas:"), wxDefaultPosition, wxSize( 100,-1 ), wxALIGN_RIGHT );	m_staticText6->Wrap( -1 );	bSizer10->Add( m_staticText6, 0, wxALIGN_CENTER|wxALL, 5 );		green_n = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 260,-1 ), 0 );	green_n->SetMaxLength( 50 ); 	bSizer10->Add( green_n, 0, wxALIGN_CENTER|wxALL, 5 );		bSizer4->Add( bSizer10, 1, wxEXPAND, 5 );		m_sdbSizer1 = new wxStdDialogButtonSizer();	m_sdbSizer1OK = new wxButton( this, wxID_OK );	m_sdbSizer1->AddButton( m_sdbSizer1OK );	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );	m_sdbSizer1->Realize();	bSizer4->Add( m_sdbSizer1, 1, wxEXPAND, 5 );		this->SetSizer( bSizer4 );	this->Layout();		this->Centre( wxBOTH );		// Connect Events	m_sdbSizer1Cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorWindow::close_colorcode ), NULL, this );	m_sdbSizer1OK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorWindow::update_colorcode ), NULL, this );}ColorWindow::~ColorWindow(){	// Disconnect Events	m_sdbSizer1Cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorWindow::close_colorcode ), NULL, this );	m_sdbSizer1OK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ColorWindow::update_colorcode ), NULL, this );	}