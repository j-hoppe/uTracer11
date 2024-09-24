///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.1.0-0-g733bf3d)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "uTracer11FB.h"

///////////////////////////////////////////////////////////////////////////

Pdp1134CpuKY11LBStatusPanelFB::Pdp1134CpuKY11LBStatusPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("KY11-LB interface signals on M8266") ), wxVERTICAL );

	m_scrolledWindow1 = new wxScrolledWindow( sbSizer1->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	m_scrolledWindow1->SetScrollRate( 5, 5 );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 6, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6 = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MPC = MICRO PROGAM COUNTER:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1134Ky11MpcText = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1134Ky11MpcText->Wrap( -1 );
	pdp1134Ky11MpcText->SetFont( wxFont( 14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1134Ky11MpcText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText7 = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("FP = FP11-A ATTACHED:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1134Ky11FpText = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1134Ky11FpText->Wrap( -1 );
	pdp1134Ky11FpText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1134Ky11FpText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText13 = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("SBF = SERVICE BR FAILED:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer1->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1134Ky11SbfText = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1134Ky11SbfText->Wrap( -1 );
	pdp1134Ky11SbfText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1134Ky11SbfText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText15 = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("LIR = LOAD IR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer1->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1134Ky11LirText = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1134Ky11LirText->Wrap( -1 );
	pdp1134Ky11LirText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1134Ky11LirText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText17 = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("PBP = PFAIL BR PEND:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer1->Add( m_staticText17, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1134Ky11PbpText = new wxStaticText( m_scrolledWindow1, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1134Ky11PbpText->Wrap( -1 );
	pdp1134Ky11PbpText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1134Ky11PbpText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );


	m_scrolledWindow1->SetSizer( fgSizer1 );
	m_scrolledWindow1->Layout();
	fgSizer1->Fit( m_scrolledWindow1 );
	sbSizer1->Add( m_scrolledWindow1, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( sbSizer1 );
	this->Layout();
}

Pdp1134CpuKY11LBStatusPanelFB::~Pdp1134CpuKY11LBStatusPanelFB()
{
}

UnibusSignalsPanelFB::UnibusSignalsPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("UNIBUS signals") ), wxVERTICAL );

	m_panel8 = new wxPanel( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel8, wxID_ANY, wxT("DATA TRANSFER") ), wxVERTICAL );

	m_panel11 = new wxPanel( sbSizer3->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxVERTICAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticUnibusSignalText1 = new wxStaticText( m_panel11, wxID_ANY, wxT("ADDR:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticUnibusSignalText1->Wrap( -1 );
	m_staticUnibusSignalText1->SetMinSize( wxSize( 60,-1 ) );

	fgSizer2->Add( m_staticUnibusSignalText1, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalAddrText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalAddrText->Wrap( -1 );
	unibusSignalAddrText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalAddrText, 0, wxALL, 5 );

	m_staticUnibusSignalText12119 = new wxStaticText( m_panel11, wxID_ANY, wxT("DATA:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUnibusSignalText12119->Wrap( -1 );
	fgSizer2->Add( m_staticUnibusSignalText12119, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalDataText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalDataText->Wrap( -1 );
	unibusSignalDataText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalDataText, 0, wxALL, 5 );

	m_staticUnibusSignalText12 = new wxStaticText( m_panel11, wxID_ANY, wxT("Control:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUnibusSignalText12->Wrap( -1 );
	fgSizer2->Add( m_staticUnibusSignalText12, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalControlText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalControlText->Wrap( -1 );
	unibusSignalControlText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalControlText, 0, wxALL, 5 );

	m_staticUnibusSignalText1211 = new wxStaticText( m_panel11, wxID_ANY, wxT("MSYN,SSYN:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUnibusSignalText1211->Wrap( -1 );
	fgSizer2->Add( m_staticUnibusSignalText1211, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalMsynSsynText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalMsynSsynText->Wrap( -1 );
	unibusSignalMsynSsynText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalMsynSsynText, 0, wxALL, 5 );

	m_staticUnibusSignalText1213 = new wxStaticText( m_panel11, wxID_ANY, wxT("PA,PB:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUnibusSignalText1213->Wrap( -1 );
	fgSizer2->Add( m_staticUnibusSignalText1213, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalPaPbText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalPaPbText->Wrap( -1 );
	unibusSignalPaPbText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalPaPbText, 0, wxALL, 5 );

	m_staticUnibusSignalText1215 = new wxStaticText( m_panel11, wxID_ANY, wxT("INTR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticUnibusSignalText1215->Wrap( -1 );
	fgSizer2->Add( m_staticUnibusSignalText1215, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalIntrText = new wxStaticText( m_panel11, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalIntrText->Wrap( -1 );
	unibusSignalIntrText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer2->Add( unibusSignalIntrText, 0, wxALL, 5 );


	m_panel11->SetSizer( fgSizer2 );
	m_panel11->Layout();
	fgSizer2->Fit( m_panel11 );
	sbSizer3->Add( m_panel11, 1, wxEXPAND | wxALL, 5 );


	m_panel8->SetSizer( sbSizer3 );
	m_panel8->Layout();
	sbSizer3->Fit( m_panel8 );
	sbSizer2->Add( m_panel8, 3, wxALL|wxEXPAND, 5 );

	m_panel9 = new wxPanel( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel9, wxID_ANY, wxT("PRORITY ARBITRATION") ), wxVERTICAL );

	m_panel12 = new wxPanel( sbSizer4->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticUnibusSignalText14 = new wxStaticText( m_panel12, wxID_ANY, wxT("BR4,5,6,7:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticUnibusSignalText14->Wrap( -1 );
	m_staticUnibusSignalText14->SetMinSize( wxSize( 60,-1 ) );

	fgSizer4->Add( m_staticUnibusSignalText14, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalBr4567Text = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalBr4567Text->Wrap( -1 );
	unibusSignalBr4567Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalBr4567Text, 0, wxALL, 5 );

	m_staticText148 = new wxStaticText( m_panel12, wxID_ANY, wxT("BG4,5,6,7:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText148->Wrap( -1 );
	fgSizer4->Add( m_staticText148, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalBg4567Text = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalBg4567Text->Wrap( -1 );
	unibusSignalBg4567Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalBg4567Text, 0, wxALL, 5 );

	m_staticText150 = new wxStaticText( m_panel12, wxID_ANY, wxT("NPR:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText150->Wrap( -1 );
	fgSizer4->Add( m_staticText150, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalNprText = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalNprText->Wrap( -1 );
	unibusSignalNprText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalNprText, 0, wxALL, 5 );

	m_staticText152 = new wxStaticText( m_panel12, wxID_ANY, wxT("NPG:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText152->Wrap( -1 );
	fgSizer4->Add( m_staticText152, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalNpgText = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalNpgText->Wrap( -1 );
	unibusSignalNpgText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalNpgText, 0, wxALL, 5 );

	m_staticText154 = new wxStaticText( m_panel12, wxID_ANY, wxT("SACK:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText154->Wrap( -1 );
	fgSizer4->Add( m_staticText154, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalSackText = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalSackText->Wrap( -1 );
	unibusSignalSackText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalSackText, 0, wxALL, 5 );

	m_staticText156 = new wxStaticText( m_panel12, wxID_ANY, wxT("BBSY:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText156->Wrap( -1 );
	fgSizer4->Add( m_staticText156, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalBbsyText = new wxStaticText( m_panel12, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalBbsyText->Wrap( -1 );
	unibusSignalBbsyText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer4->Add( unibusSignalBbsyText, 0, wxALL, 5 );


	m_panel12->SetSizer( fgSizer4 );
	m_panel12->Layout();
	fgSizer4->Fit( m_panel12 );
	sbSizer4->Add( m_panel12, 1, wxEXPAND | wxALL, 5 );


	m_panel9->SetSizer( sbSizer4 );
	m_panel9->Layout();
	sbSizer4->Fit( m_panel9 );
	sbSizer2->Add( m_panel9, 3, wxEXPAND | wxALL, 5 );

	m_panel10 = new wxPanel( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( m_panel10, wxID_ANY, wxT("INITIALIZATION") ), wxVERTICAL );

	m_panel13 = new wxPanel( sbSizer5->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText159 = new wxStaticText( m_panel13, wxID_ANY, wxT("INIT:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText159->Wrap( -1 );
	m_staticText159->SetMinSize( wxSize( 60,-1 ) );

	fgSizer5->Add( m_staticText159, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalInitText = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalInitText->Wrap( -1 );
	unibusSignalInitText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer5->Add( unibusSignalInitText, 0, wxALL, 5 );

	m_staticText161 = new wxStaticText( m_panel13, wxID_ANY, wxT("ACLO:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	fgSizer5->Add( m_staticText161, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalAcloText = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalAcloText->Wrap( -1 );
	unibusSignalAcloText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer5->Add( unibusSignalAcloText, 0, wxALL, 5 );

	m_staticText163 = new wxStaticText( m_panel13, wxID_ANY, wxT("DCLO:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText163->Wrap( -1 );
	fgSizer5->Add( m_staticText163, 0, wxALIGN_RIGHT|wxALL, 5 );

	unibusSignalDcloText = new wxStaticText( m_panel13, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	unibusSignalDcloText->Wrap( -1 );
	unibusSignalDcloText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer5->Add( unibusSignalDcloText, 0, wxALL, 5 );


	m_panel13->SetSizer( fgSizer5 );
	m_panel13->Layout();
	fgSizer5->Fit( m_panel13 );
	sbSizer5->Add( m_panel13, 1, wxEXPAND | wxALL, 5 );


	m_panel10->SetSizer( sbSizer5 );
	m_panel10->Layout();
	sbSizer5->Fit( m_panel10 );
	sbSizer2->Add( m_panel10, 2, wxALL|wxEXPAND, 5 );


	this->SetSizer( sbSizer2 );
	this->Layout();
	sbSizer2->Fit( this );
}

UnibusSignalsPanelFB::~UnibusSignalsPanelFB()
{
}

InternalStatePanelFB::InternalStatePanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Internal state vars") ), wxVERTICAL );

	internalStateFlexGridSizerPanelFB = new wxScrolledWindow( sbSizer12->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	internalStateFlexGridSizerPanelFB->SetScrollRate( 5, 5 );
	internalStateFlexGridSizerFB = new wxFlexGridSizer( 0, 2, 0, 0 );
	internalStateFlexGridSizerFB->SetFlexibleDirection( wxBOTH );
	internalStateFlexGridSizerFB->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	internalStateFlexGridSizerPanelFB->SetSizer( internalStateFlexGridSizerFB );
	internalStateFlexGridSizerPanelFB->Layout();
	internalStateFlexGridSizerFB->Fit( internalStateFlexGridSizerPanelFB );
	sbSizer12->Add( internalStateFlexGridSizerPanelFB, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( sbSizer12 );
	this->Layout();
	sbSizer12->Fit( this );
}

InternalStatePanelFB::~InternalStatePanelFB()
{
}

TracePanelFB::TracePanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_panel14 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	traceClearButtonFB = new wxButton( m_panel14, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( traceClearButtonFB, 0, wxALL, 5 );

	traceToClipboardButtonFB = new wxButton( m_panel14, wxID_ANY, wxT("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( traceToClipboardButtonFB, 0, wxALL, 5 );


	m_panel14->SetSizer( bSizer9 );
	m_panel14->Layout();
	bSizer9->Fit( m_panel14 );
	bSizer8->Add( m_panel14, 1, wxEXPAND | wxALL, 5 );

	traceGridFB = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );

	// Grid
	traceGridFB->CreateGrid( 5, 10 );
	traceGridFB->EnableEditing( false );
	traceGridFB->EnableGridLines( true );
	traceGridFB->EnableDragGridSize( false );
	traceGridFB->SetMargins( 0, 0 );

	// Columns
	traceGridFB->AutoSizeColumns();
	traceGridFB->EnableDragColMove( false );
	traceGridFB->EnableDragColSize( true );
	traceGridFB->SetColLabelValue( 0, wxT("MPC") );
	traceGridFB->SetColLabelValue( 1, wxT("UB-Ctrl") );
	traceGridFB->SetColLabelValue( 2, wxT("UB-Addr") );
	traceGridFB->SetColLabelValue( 3, wxT("UB-Data") );
	traceGridFB->SetColLabelValue( 4, wxT("Label") );
	traceGridFB->SetColLabelValue( 5, wxT("Opcode") );
	traceGridFB->SetColLabelValue( 6, wxT("Operands") );
	traceGridFB->SetColLabelValue( 7, wxT("Addr hex") );
	traceGridFB->SetColLabelValue( 8, wxT("Data hex") );
	traceGridFB->SetColLabelValue( 9, wxT("Comment") );
	traceGridFB->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	traceGridFB->AutoSizeRows();
	traceGridFB->EnableDragRowSize( true );
	traceGridFB->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	traceGridFB->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer8->Add( traceGridFB, 9, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer8 );
	this->Layout();

	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( TracePanelFB::TracePanelFBOnSize ) );
	traceClearButtonFB->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TracePanelFB::traceClearButtonOnButtonClick ), NULL, this );
	traceToClipboardButtonFB->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TracePanelFB::traceToClipboardButtonOnButtonClick ), NULL, this );
	traceGridFB->Connect( wxEVT_SIZE, wxSizeEventHandler( TracePanelFB::traceGridFBOnSize ), NULL, this );
}

TracePanelFB::~TracePanelFB()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( TracePanelFB::TracePanelFBOnSize ) );
	traceClearButtonFB->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TracePanelFB::traceClearButtonOnButtonClick ), NULL, this );
	traceToClipboardButtonFB->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TracePanelFB::traceToClipboardButtonOnButtonClick ), NULL, this );
	traceGridFB->Disconnect( wxEVT_SIZE, wxSizeEventHandler( TracePanelFB::traceGridFBOnSize ), NULL, this );

}

Pdp11DataPathPanelFB::Pdp11DataPathPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{

	// Connect Events
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( Pdp11DataPathPanelFB::Pdp11DataPathPanelOnPaint ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( Pdp11DataPathPanelFB::Pdp11DataPathPanelOnSize ) );
}

Pdp11DataPathPanelFB::~Pdp11DataPathPanelFB()
{
	// Disconnect Events
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( Pdp11DataPathPanelFB::Pdp11DataPathPanelOnPaint ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( Pdp11DataPathPanelFB::Pdp11DataPathPanelOnSize ) );

}

Pdp11uFlowPanelFB::Pdp11uFlowPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{

	// Connect Events
	this->Connect( wxEVT_PAINT, wxPaintEventHandler( Pdp11uFlowPanelFB::Pdp11uFlowPanelOnPaint ) );
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( Pdp11uFlowPanelFB::Pdp11uFlowPanelOnSize ) );
}

Pdp11uFlowPanelFB::~Pdp11uFlowPanelFB()
{
	// Disconnect Events
	this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( Pdp11uFlowPanelFB::Pdp11uFlowPanelOnPaint ) );
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( Pdp11uFlowPanelFB::Pdp11uFlowPanelOnSize ) );

}

Pdp11uWordPanelFB::Pdp11uWordPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
}

Pdp11uWordPanelFB::~Pdp11uWordPanelFB()
{
}

Pdp1140CpuKM11StatusPanelFB::Pdp1140CpuKM11StatusPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );

	m_panel15 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel15, wxID_ANY, wxT("KM11A: KD11-A Maintenance Console") ), wxVERTICAL );

	m_panel161 = new wxPanel( sbSizer10->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText6 = new wxStaticText( m_panel161, wxID_ANY, wxT("PUPP = prev MPC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aPuppText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aPuppText->Wrap( -1 );
	pdp1140Km11aPuppText->SetFont( wxFont( 14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aPuppText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText7 = new wxStaticText( m_panel161, wxID_ANY, wxT("BUPP = next MPC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aBuppText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aBuppText->Wrap( -1 );
	pdp1140Km11aBuppText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aBuppText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText13 = new wxStaticText( m_panel161, wxID_ANY, wxT("TRAP:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSizer1->Add( m_staticText13, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aTrapText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aTrapText->Wrap( -1 );
	pdp1140Km11aTrapText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aTrapText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText172 = new wxStaticText( m_panel161, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText172->Wrap( -1 );
	fgSizer1->Add( m_staticText172, 0, wxALL, 5 );

	m_staticText1721 = new wxStaticText( m_panel161, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1721->Wrap( -1 );
	fgSizer1->Add( m_staticText1721, 0, wxALL, 5 );

	m_staticText15 = new wxStaticText( m_panel161, wxID_ANY, wxT("SSYN:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText15->Wrap( -1 );
	fgSizer1->Add( m_staticText15, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aSsynText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aSsynText->Wrap( -1 );
	pdp1140Km11aSsynText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aSsynText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText17 = new wxStaticText( m_panel161, wxID_ANY, wxT("MSYN:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	fgSizer1->Add( m_staticText17, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aMsynText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aMsynText->Wrap( -1 );
	pdp1140Km11aMsynText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aMsynText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText1115 = new wxStaticText( m_panel161, wxID_ANY, wxT("C:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1115->Wrap( -1 );
	fgSizer1->Add( m_staticText1115, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aCText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aCText->Wrap( -1 );
	pdp1140Km11aCText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aCText, 0, wxALL, 5 );

	m_staticText1112 = new wxStaticText( m_panel161, wxID_ANY, wxT("V:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1112->Wrap( -1 );
	fgSizer1->Add( m_staticText1112, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aVText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aVText->Wrap( -1 );
	pdp1140Km11aVText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aVText, 0, wxALL, 5 );

	m_staticText1113 = new wxStaticText( m_panel161, wxID_ANY, wxT("Z:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1113->Wrap( -1 );
	fgSizer1->Add( m_staticText1113, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aZText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aZText->Wrap( -1 );
	pdp1140Km11aZText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aZText, 0, wxALL, 5 );

	m_staticText1114 = new wxStaticText( m_panel161, wxID_ANY, wxT("N:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1114->Wrap( -1 );
	fgSizer1->Add( m_staticText1114, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aNText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aNText->Wrap( -1 );
	pdp1140Km11aNText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aNText, 0, wxALL, 5 );

	m_staticText111 = new wxStaticText( m_panel161, wxID_ANY, wxT("T:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	fgSizer1->Add( m_staticText111, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11aTText = new wxStaticText( m_panel161, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11aTText->Wrap( -1 );
	pdp1140Km11aTText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer1->Add( pdp1140Km11aTText, 0, wxALL, 5 );


	m_panel161->SetSizer( fgSizer1 );
	m_panel161->Layout();
	fgSizer1->Fit( m_panel161 );
	sbSizer10->Add( m_panel161, 1, wxEXPAND | wxALL, 5 );


	m_panel15->SetSizer( sbSizer10 );
	m_panel15->Layout();
	sbSizer10->Fit( m_panel15 );
	bSizer10->Add( m_panel15, 1, wxEXPAND | wxALL, 5 );

	m_panel16 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer11;
	sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( m_panel16, wxID_ANY, wxT("KM11B: KT11-D, KE11-E,F Maintenance Console") ), wxVERTICAL );

	m_panel162 = new wxPanel( sbSizer11->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer11;
	fgSizer11 = new wxFlexGridSizer( 0, 4, 0, 0 );
	fgSizer11->SetFlexibleDirection( wxBOTH );
	fgSizer11->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText61 = new wxStaticText( m_panel162, wxID_ANY, wxT("PBA<17:06> = MMU phys addr:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	fgSizer11->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bPbaText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bPbaText->Wrap( -1 );
	pdp1140Km11bPbaText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bPbaText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText134 = new wxStaticText( m_panel162, wxID_ANY, wxT("ROM A,B = mem access mode (MPTPI/D):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText134->Wrap( -1 );
	fgSizer11->Add( m_staticText134, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bRomAbText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bRomAbText->Wrap( -1 );
	pdp1140Km11bRomAbText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bRomAbText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText135 = new wxStaticText( m_panel162, wxID_ANY, wxT("ROM C = clock mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText135->Wrap( -1 );
	fgSizer11->Add( m_staticText135, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bRomCText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bRomCText->Wrap( -1 );
	pdp1140Km11bRomCText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bRomCText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText151 = new wxStaticText( m_panel162, wxID_ANY, wxT("ROM D = KD11 reloc:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText151->Wrap( -1 );
	fgSizer11->Add( m_staticText151, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bRomDText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bRomDText->Wrap( -1 );
	pdp1140Km11bRomDText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bRomDText, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText171 = new wxStaticText( m_panel162, wxID_ANY, wxT("B15 = CPU B reg <15>, div ALU func:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText171->Wrap( -1 );
	fgSizer11->Add( m_staticText171, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bB15Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bB15Text->Wrap( -1 );
	pdp1140Km11bB15Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bB15Text, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );

	m_staticText1311 = new wxStaticText( m_panel162, wxID_ANY, wxT("ECIN 00 = external ALU carry:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1311->Wrap( -1 );
	fgSizer11->Add( m_staticText1311, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bEcin00Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bEcin00Text->Wrap( -1 );
	pdp1140Km11bEcin00Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bEcin00Text, 0, wxALL, 5 );

	m_staticText1312 = new wxStaticText( m_panel162, wxID_ANY, wxT("EXP UNFL = exponential underflow EXI1 FP flow:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1312->Wrap( -1 );
	fgSizer11->Add( m_staticText1312, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bExpUnflText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bExpUnflText->Wrap( -1 );
	pdp1140Km11bExpUnflText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bExpUnflText, 0, wxALL, 5 );

	m_staticText1313 = new wxStaticText( m_panel162, wxID_ANY, wxT("EXP OVFL = exponentail overflow EXI1 FP flow:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1313->Wrap( -1 );
	fgSizer11->Add( m_staticText1313, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bExpOvflText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bExpOvflText->Wrap( -1 );
	pdp1140Km11bExpOvflText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bExpOvflText, 0, wxALL, 5 );

	m_staticText1314 = new wxStaticText( m_panel162, wxID_ANY, wxT("DR00 = multi function condition:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1314->Wrap( -1 );
	fgSizer11->Add( m_staticText1314, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bDr00Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bDr00Text->Wrap( -1 );
	pdp1140Km11bDr00Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bDr00Text, 0, wxALL, 5 );

	m_staticText1315 = new wxStaticText( m_panel162, wxID_ANY, wxT("DR09 = normalization test:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1315->Wrap( -1 );
	fgSizer11->Add( m_staticText1315, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bDr09Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bDr09Text->Wrap( -1 );
	pdp1140Km11bDr09Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bDr09Text, 0, wxALL, 5 );

	m_staticText13151 = new wxStaticText( m_panel162, wxID_ANY, wxT("MSR00 = MSR reg <00> bit, ALU FDIV:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13151->Wrap( -1 );
	fgSizer11->Add( m_staticText13151, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bMsr00Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bMsr00Text->Wrap( -1 );
	pdp1140Km11bMsr00Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bMsr00Text, 0, wxALL, 5 );

	m_staticText13152 = new wxStaticText( m_panel162, wxID_ANY, wxT("MSR01 = MSR reg <01> bit, ALU FMUL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13152->Wrap( -1 );
	fgSizer11->Add( m_staticText13152, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bMsr01Text = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bMsr01Text->Wrap( -1 );
	pdp1140Km11bMsr01Text->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bMsr01Text, 0, wxALL, 5 );

	m_staticText13153 = new wxStaticText( m_panel162, wxID_ANY, wxT("EPS(C) = ext CPU state Carry:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13153->Wrap( -1 );
	fgSizer11->Add( m_staticText13153, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bEpsCText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bEpsCText->Wrap( -1 );
	pdp1140Km11bEpsCText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bEpsCText, 0, wxALL, 5 );

	m_staticText131531 = new wxStaticText( m_panel162, wxID_ANY, wxT("EPS(V) = ext CPU state Overflow:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131531->Wrap( -1 );
	fgSizer11->Add( m_staticText131531, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bEpsVText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bEpsVText->Wrap( -1 );
	pdp1140Km11bEpsVText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bEpsVText, 0, wxALL, 5 );

	m_staticText131532 = new wxStaticText( m_panel162, wxID_ANY, wxT("EPS(Z) = ext CPU state Zero:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131532->Wrap( -1 );
	fgSizer11->Add( m_staticText131532, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bEpsZText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bEpsZText->Wrap( -1 );
	pdp1140Km11bEpsZText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bEpsZText, 0, wxALL, 5 );

	m_staticText131533 = new wxStaticText( m_panel162, wxID_ANY, wxT("EPS(N) = ext CPU state Negative:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText131533->Wrap( -1 );
	fgSizer11->Add( m_staticText131533, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5 );

	pdp1140Km11bEpsNText = new wxStaticText( m_panel162, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	pdp1140Km11bEpsNText->Wrap( -1 );
	pdp1140Km11bEpsNText->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	fgSizer11->Add( pdp1140Km11bEpsNText, 0, wxALL, 5 );


	m_panel162->SetSizer( fgSizer11 );
	m_panel162->Layout();
	fgSizer11->Fit( m_panel162 );
	sbSizer11->Add( m_panel162, 1, wxALL|wxEXPAND, 5 );


	m_panel16->SetSizer( sbSizer11 );
	m_panel16->Layout();
	sbSizer11->Fit( m_panel16 );
	bSizer10->Add( m_panel16, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer10 );
	this->Layout();
}

Pdp1140CpuKM11StatusPanelFB::~Pdp1140CpuKM11StatusPanelFB()
{
}

MemoryPanelFB::MemoryPanelFB( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	memoryLoadPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( memoryLoadPanel, wxID_ANY, wxT("Load") ), wxVERTICAL );

	m_panel16 = new wxPanel( sbSizer7->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText45 = new wxStaticText( m_panel16, wxID_ANY, wxT("Formats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText45->Wrap( -1 );
	bSizer5->Add( m_staticText45, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	memoryFileFormatComboBox = new wxComboBox( m_panel16, wxID_ANY, wxT("memoryGridClearButtonWxFB"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	memoryFileFormatComboBox->Append( wxT("Addr-values pairs (text)") );
	memoryFileFormatComboBox->Append( wxT("MACRO11 listing") );
	memoryFileFormatComboBox->Append( wxT("Absolute paper tape") );
	memoryFileFormatComboBox->SetSelection( 1 );
	bSizer5->Add( memoryFileFormatComboBox, 0, wxALL, 5 );

	m_staticText44 = new wxStaticText( m_panel16, wxID_ANY, wxT("Filename (.mac or .ptap):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText44->Wrap( -1 );
	bSizer5->Add( m_staticText44, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	memoryLoadFilePickerWxFB = new wxFilePickerCtrl( m_panel16, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	bSizer5->Add( memoryLoadFilePickerWxFB, 999, wxALL|wxEXPAND, 5 );

	depositMemoryButton = new wxButton( m_panel16, wxID_ANY, wxT("DEPOSIT"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( depositMemoryButton, 0, wxALL, 5 );


	m_panel16->SetSizer( bSizer5 );
	m_panel16->Layout();
	bSizer5->Fit( m_panel16 );
	sbSizer7->Add( m_panel16, 1, wxEXPAND | wxALL, 5 );


	memoryLoadPanel->SetSizer( sbSizer7 );
	memoryLoadPanel->Layout();
	sbSizer7->Fit( memoryLoadPanel );
	bSizer8->Add( memoryLoadPanel, 0, wxEXPAND | wxALL, 5 );

	m_panel17 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( m_panel17, wxID_ANY, wxT("Save") ), wxVERTICAL );


	m_panel17->SetSizer( sbSizer8 );
	m_panel17->Layout();
	sbSizer8->Fit( m_panel17 );
	bSizer8->Add( m_panel17, 0, wxEXPAND | wxALL, 5 );

	m_panel13 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( m_panel13, wxID_ANY, wxT("Manual EXAM / DEPOSIT") ), wxHORIZONTAL );

	m_staticText48 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Addr:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText48->Wrap( -1 );
	sbSizer9->Add( m_staticText48, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	manualExamDepositAddrTextCtrl = new wxTextCtrl( sbSizer9->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	manualExamDepositAddrTextCtrl->SetToolTip( wxT("18bit octal value only") );

	sbSizer9->Add( manualExamDepositAddrTextCtrl, 0, wxALL, 5 );

	manualExamButton = new wxButton( sbSizer9->GetStaticBox(), wxID_ANY, wxT("EXAM"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( manualExamButton, 0, wxALL, 5 );

	m_staticText50 = new wxStaticText( sbSizer9->GetStaticBox(), wxID_ANY, wxT("Data:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText50->Wrap( -1 );
	sbSizer9->Add( m_staticText50, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	manualDepositDataTextCtrl = new wxTextCtrl( sbSizer9->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	manualDepositDataTextCtrl->SetToolTip( wxT("16bit octal value only") );

	sbSizer9->Add( manualDepositDataTextCtrl, 0, wxALL, 5 );

	manualDepositButton = new wxButton( sbSizer9->GetStaticBox(), wxID_ANY, wxT("DEPOSIT"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( manualDepositButton, 0, wxALL, 5 );


	m_panel13->SetSizer( sbSizer9 );
	m_panel13->Layout();
	sbSizer9->Fit( m_panel13 );
	bSizer8->Add( m_panel13, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText46 = new wxStaticText( this, wxID_ANY, wxT("Known UNIBUS Memory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText46->Wrap( -1 );
	bSizer6->Add( m_staticText46, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	memoryGridClearButtonFB = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( memoryGridClearButtonFB, 0, wxALL, 5 );


	bSizer8->Add( bSizer6, 0, wxEXPAND, 5 );

	memoryGridFB = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	memoryGridFB->CreateGrid( 5, 9 );
	memoryGridFB->EnableEditing( false );
	memoryGridFB->EnableGridLines( true );
	memoryGridFB->EnableDragGridSize( false );
	memoryGridFB->SetMargins( 0, 0 );

	// Columns
	memoryGridFB->EnableDragColMove( false );
	memoryGridFB->EnableDragColSize( true );
	memoryGridFB->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	memoryGridFB->EnableDragRowSize( true );
	memoryGridFB->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	memoryGridFB->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	memoryGridFB->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );
	memoryGridFB->SetMinSize( wxSize( -1,100 ) );

	bSizer8->Add( memoryGridFB, 1, wxALL, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText47 = new wxStaticText( this, wxID_ANY, wxT("Known UNIBUS IOpage:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText47->Wrap( -1 );
	bSizer7->Add( m_staticText47, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	ioPageGridClearButtonFB = new wxButton( this, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( ioPageGridClearButtonFB, 0, wxALL, 5 );


	bSizer8->Add( bSizer7, 0, wxEXPAND, 5 );

	ioPageGridFB = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	ioPageGridFB->CreateGrid( 5, 9 );
	ioPageGridFB->EnableEditing( false );
	ioPageGridFB->EnableGridLines( true );
	ioPageGridFB->EnableDragGridSize( false );
	ioPageGridFB->SetMargins( 0, 0 );

	// Columns
	ioPageGridFB->EnableDragColMove( false );
	ioPageGridFB->EnableDragColSize( true );
	ioPageGridFB->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	ioPageGridFB->EnableDragRowSize( true );
	ioPageGridFB->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	ioPageGridFB->SetDefaultCellBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	ioPageGridFB->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	ioPageGridFB->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );
	ioPageGridFB->SetMinSize( wxSize( -1,100 ) );

	bSizer8->Add( ioPageGridFB, 1, wxALL, 5 );


	this->SetSizer( bSizer8 );
	this->Layout();

	// Connect Events
	memoryFileFormatComboBox->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MemoryPanelFB::memoryFileFormatComboBoxOnCombobox ), NULL, this );
	memoryLoadFilePickerWxFB->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( MemoryPanelFB::memoryLoadFilePickerOnFileChanged ), NULL, this );
	depositMemoryButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::depositMemoryButtonOnButtonClick ), NULL, this );
	manualExamButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::manualExamButtonOnButtonClick ), NULL, this );
	manualDepositButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::manualDepositButtonOnButtonClick ), NULL, this );
	memoryGridClearButtonFB->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::memoryGridClearButtonFBOnButtonClick ), NULL, this );
	memoryGridFB->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MemoryPanelFB::memoryGridFBOnGridCellLeftClick ), NULL, this );
	ioPageGridClearButtonFB->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::ioPageGridClearButtonFBOnButtonClick ), NULL, this );
	ioPageGridFB->Connect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MemoryPanelFB::ioPageGridFBOnGridCellLeftClick ), NULL, this );
}

MemoryPanelFB::~MemoryPanelFB()
{
	// Disconnect Events
	memoryFileFormatComboBox->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( MemoryPanelFB::memoryFileFormatComboBoxOnCombobox ), NULL, this );
	memoryLoadFilePickerWxFB->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( MemoryPanelFB::memoryLoadFilePickerOnFileChanged ), NULL, this );
	depositMemoryButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::depositMemoryButtonOnButtonClick ), NULL, this );
	manualExamButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::manualExamButtonOnButtonClick ), NULL, this );
	manualDepositButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::manualDepositButtonOnButtonClick ), NULL, this );
	memoryGridClearButtonFB->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::memoryGridClearButtonFBOnButtonClick ), NULL, this );
	memoryGridFB->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MemoryPanelFB::memoryGridFBOnGridCellLeftClick ), NULL, this );
	ioPageGridClearButtonFB->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryPanelFB::ioPageGridClearButtonFBOnButtonClick ), NULL, this );
	ioPageGridFB->Disconnect( wxEVT_GRID_CELL_LEFT_CLICK, wxGridEventHandler( MemoryPanelFB::ioPageGridFBOnGridCellLeftClick ), NULL, this );

}

MainFrameFB::MainFrameFB( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	topHorizontalSizer = new wxBoxSizer( wxHORIZONTAL );

	toolsNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	runPanel = new wxPanel( toolsNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* executePanelSizer;
	executePanelSizer = new wxBoxSizer( wxVERTICAL );

	uMachineStateText = new wxStaticText( runPanel, wxID_ANY, wxT("µMachine running"), wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_CENTER_HORIZONTAL );
	uMachineStateText->Wrap( -1 );
	uMachineStateText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	executePanelSizer->Add( uMachineStateText, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	powerCycleButton = new wxButton( runPanel, wxID_ANY, wxT("Power Cycle"), wxDefaultPosition, wxDefaultSize, 0 );
	executePanelSizer->Add( powerCycleButton, 0, wxALL|wxEXPAND, 5 );

	manClockEnableButton = new wxToggleButton( runPanel, wxID_ANY, wxT("MAN CLK ENAB"), wxDefaultPosition, wxDefaultSize, 0 );
	manClockEnableButton->SetToolTip( wxT("Hotkeys F2 = enable, F3 = disable") );

	executePanelSizer->Add( manClockEnableButton, 0, wxALL|wxEXPAND, 5 );

	microStepButton = new wxButton( runPanel, wxID_ANY, wxT("MAN CLK"), wxDefaultPosition, wxDefaultSize, 0 );
	microStepButton->SetToolTip( wxT("Hotkey F4") );

	executePanelSizer->Add( microStepButton, 0, wxALL|wxEXPAND, 5 );

	microRunUntilButton = new wxButton( runPanel, wxID_ANY, wxT("Step until stop condition"), wxDefaultPosition, wxDefaultSize, 0 );
	executePanelSizer->Add( microRunUntilButton, 0, wxALL, 5 );

	m_staticText2 = new wxStaticText( runPanel, wxID_ANY, wxT("Stop at uPC:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	executePanelSizer->Add( m_staticText2, 0, wxALL, 5 );

	stopUpcTextCtrl = new wxTextCtrl( runPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	executePanelSizer->Add( stopUpcTextCtrl, 0, wxALL, 5 );

	m_staticText21 = new wxStaticText( runPanel, wxID_ANY, wxT("Stop at UNIBUS:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	executePanelSizer->Add( m_staticText21, 0, wxALL, 5 );

	stopUnibusCycleComboBox = new wxComboBox( runPanel, wxID_ANY, wxT("any"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	stopUnibusCycleComboBox->Append( wxT("DATI") );
	stopUnibusCycleComboBox->Append( wxT("DATO") );
	stopUnibusCycleComboBox->Append( wxT("any") );
	executePanelSizer->Add( stopUnibusCycleComboBox, 0, wxALL, 5 );

	stopUnibusAddrTextCtrl = new wxTextCtrl( runPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	executePanelSizer->Add( stopUnibusAddrTextCtrl, 0, wxALL, 5 );

	m_staticText211 = new wxStaticText( runPanel, wxID_ANY, wxT("Stop repeat count:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText211->Wrap( -1 );
	executePanelSizer->Add( m_staticText211, 0, wxALL, 5 );

	stopRepeatCountTextCtrl = new wxTextCtrl( runPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	executePanelSizer->Add( stopRepeatCountTextCtrl, 0, wxALL, 5 );


	runPanel->SetSizer( executePanelSizer );
	runPanel->Layout();
	executePanelSizer->Fit( runPanel );
	toolsNotebook->AddPage( runPanel, wxT("Control"), true );

	topHorizontalSizer->Add( toolsNotebook, 0, wxEXPAND | wxALL, 5 );

	documentsNotebookFB = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	EventPanelFB = new wxPanel( documentsNotebookFB, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer6;
	sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( EventPanelFB, wxID_ANY, wxT("Events") ), wxVERTICAL );

	eventsTextCtrl = new wxTextCtrl( sbSizer6->GetStaticBox(), wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTE_MULTILINE );
	eventsTextCtrl->SetFont( wxFont( 9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Courier New") ) );

	sbSizer6->Add( eventsTextCtrl, 1, wxALL|wxEXPAND, 5 );


	EventPanelFB->SetSizer( sbSizer6 );
	EventPanelFB->Layout();
	sbSizer6->Fit( EventPanelFB );
	documentsNotebookFB->AddPage( EventPanelFB, wxT("Events"), false );
	pdp11StatusPanelFB = new wxPanel( documentsNotebookFB, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	pdp11StatusSizerFB = new wxBoxSizer( wxHORIZONTAL );


	pdp11StatusPanelFB->SetSizer( pdp11StatusSizerFB );
	pdp11StatusPanelFB->Layout();
	pdp11StatusSizerFB->Fit( pdp11StatusPanelFB );
	documentsNotebookFB->AddPage( pdp11StatusPanelFB, wxT("CPU signals"), false );

	topHorizontalSizer->Add( documentsNotebookFB, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( topHorizontalSizer );
	this->Layout();
	statusBar = this->CreateStatusBar( 1, wxSTB_SIZEGRIP, wxID_ANY );
	updateTimer.SetOwner( this, updateTimer.GetId() );

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_SHOW, wxShowEventHandler( MainFrameFB::OnShow ) );
	powerCycleButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::powerCycleButtonOnButtonClick ), NULL, this );
	manClockEnableButton->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::manClockEnableButtonOnToggleButton ), NULL, this );
	microStepButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::microStepButtonOnButtonClick ), NULL, this );
	microRunUntilButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::microRunUntilButtonOnButtonClick ), NULL, this );
	this->Connect( updateTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( MainFrameFB::updateTimerOnTimer ) );
}

MainFrameFB::~MainFrameFB()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SHOW, wxShowEventHandler( MainFrameFB::OnShow ) );
	powerCycleButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::powerCycleButtonOnButtonClick ), NULL, this );
	manClockEnableButton->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::manClockEnableButtonOnToggleButton ), NULL, this );
	microStepButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::microStepButtonOnButtonClick ), NULL, this );
	microRunUntilButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameFB::microRunUntilButtonOnButtonClick ), NULL, this );
	this->Disconnect( updateTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( MainFrameFB::updateTimerOnTimer ) );

}
