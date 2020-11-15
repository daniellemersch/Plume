// Created by Danielle Mersch
// Copyright Unil. All rights reserved.

#include <libants/utils.h>
#include <libants/trackcvt.h>

#include "mainwin.h"

const int CORR = 5;   ///< scaling factor for images
const unsigned int IMG_S = IMAGE_WIDTH * IMAGE_HEIGHT;  ///< size of high resolution image
const unsigned int IMG_WIDTH = 608;  ///< width of the rescaled image
const unsigned int IMG_HEIGHT = 912;  ///< height of rescaled image
const unsigned int IMG_HEIGHT_ROTATED = IMG_WIDTH;
const unsigned int IMG_WIDTH_ROTATED = IMG_HEIGHT;
const unsigned int IMG_SIZE = IMG_WIDTH * IMG_HEIGHT;  ///< size of rescaled image
const unsigned int FRAME_NUMBER_LENGTH = 8; ///< number of digits to specify the framenumber in the filename
const unsigned int UPPER_PENCIL = 20;			///< upper limit of pencil thickness

const uint8_t COLOR_OPACITY = 255;       ///< opacity of zones with color (255 = opaque 100%, 0 = transparent)
const uint8_t BACKGROUND_OPACITY = 0;    ///< opacity of zones without color

const uint8_t colors_r[NUMBER_LINES_COLOR] = { 255, 255, 205, 153,  65,  39 };
const uint8_t colors_g[NUMBER_LINES_COLOR] = { 255, 140,   0,  50, 105, 139 };
const uint8_t colors_b[NUMBER_LINES_COLOR] = {   0,   0,   0, 204, 225,  34 };

//const int ANGLE = 21;		///
//const int RULER = 22;
//const int POINTER = 23;

//====================================================================================
/**\brief Colors a point of size PIX_X, PIX_Y with color 
 */
void MainWin::color_bitmap(const int x, const int y, const int color){
  wxClientDC dc(m_bitmap1);
  if (color >= Plume::YELLOW && color <= Plume::GREEN) {
    wxColor my_color;
    my_color.Set(colors_r[color - 1], colors_g[color - 1], colors_b[color - 1]);
    wxPen colorpen(my_color, 1);
    dc.SetPen(colorpen);
  }
	for (int dx(0); dx < thickness; dx++){
		for (int dy(0); dy < thickness; dy++){
			int px = dx + x;
			int py = dy + y;
			if (px < IMG_WIDTH && py < IMG_HEIGHT){
			  uint8_t* bitmap(plm.get_bitmap());
				bitmap[px + py * IMG_WIDTH] = color;
				dc.DrawPoint(py, IMG_WIDTH - px);
			}
		}
	}
}


//====================================================================================
/* \brief Reads an image file in format PPM
 * \param filename Name of the image file
 * \return wxImage Pointer to image that is opened
 */
wxImage* MainWin::load_pgm(const char* filename){
	char tmp[128];

	/// opens the image file 
	ifstream f;
	f.open(filename, ios::in | ios::binary);
	if (!f.is_open()){
		::wxMessageBox(wxT("Unable to open the specified file."), wxT("Error"), wxICON_ERROR);
		return NULL;
	}
	
	// check whether the image type is a binary pgm file
	string s; //< string in which we read the data
	getline(f, s); 
	rtrim(s);
	if (s.compare("P5") != 0){
		f.close();
		::wxMessageBox(wxT("The file contains data in an unsupported format."), wxT("Error"), wxICON_ERROR);
		return NULL;
	}
	s.clear();
	
	// check the size of the image, if a high resolution image, it needs to be rescaled
	getline(f, s);
	int xs(0), ys(0);  //< size of the image read
	istringstream ss;
	ss.str(s);
	ss>> xs;
	ss>>ys;
	
	
	if (xs != IMAGE_WIDTH && ys != IMAGE_HEIGHT) {
		f.close();
		::wxMessageBox(wxT("The image contained in the file doesn't have the expected size."), wxT("Error"), wxICON_ERROR);
		return NULL;
	}
	
	// check whether the maximum value a pixel can have is 255
	getline(f, s);
	if (atoi(s.c_str())!=255) {
		f.close();
		::wxMessageBox(wxT("The image pixel format is not supported."), wxT("Error"), wxICON_ERROR);
		return NULL;
	}
	
	// read image
	unsigned char* buffer = (unsigned char*) malloc(IMG_S);
	f.read((char*) buffer, IMG_S);
	f.close();

	// wxImage require a color image, hence we allocate space for a color copy (color image = 3 * gray image)
	unsigned char* buffer2 = (unsigned char*) malloc(IMG_S * 3);
	unsigned char* bptr = buffer2;
	for (int p(0); p<IMG_S; p++){
		*bptr++ = buffer[p];
		*bptr++ = buffer[p];
		*bptr++ = buffer[p];
	}
	free(buffer);
	wxImage* img = new wxImage(IMAGE_WIDTH, IMAGE_HEIGHT, buffer2);
	img -> Rescale(IMG_WIDTH, IMG_HEIGHT,wxIMAGE_QUALITY_HIGH);
  return img;
}


//====================================================================================
void MainWin::load_plume( wxCommandEvent& event ){
	
	if (bitmap_state){
		int msg = ::wxMessageBox(wxT("The current marked zones have not been saved. Do you want to overwrite them with the new file ?"), wxT("Warning"), wxICON_QUESTION|wxYES_NO);
		if (msg == wxNO){
			return;
		}
	}
	
	wxFileDialog odlg(this, wxT("Open plume file"), wxT(""), wxT(""), wxT("Plume files (*.plume)|*.plume"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	
	if (odlg.ShowModal()!=wxID_OK){ 
		return;
	}
	
  plm.read_plume((const char*)odlg.GetPath().mb_str());
  alt_bitmap_state = false;

	// if an image is loaded, refreshes it
	if (image!=NULL) {
		m_bitmap1 ->Refresh();
	}
}


//====================================================================================
/** Writes the color image to a file
 * \param image Pointer to the image data
 * \param filename Name of the output file
 * \return True if the image was written, false if not
 */
bool MainWin::write_png(const uint8_t* bitmap, wxString filename){
	// wxImage require a color image, hence we allocate space for a color copy (color image = 3 * gray image)
	uint8_t* buffer2 = (uint8_t*) malloc(IMG_SIZE * 3);
	uint8_t* bptr = buffer2;
	uint8_t* alpha = (uint8_t*) malloc(IMG_SIZE);
	uint8_t* aptr = alpha;
	for (int p(0); p<IMG_SIZE; p++){
		switch (bitmap[p]) {
			case Plume::YELLOW:
				*bptr++ = 255;
				*bptr++ = 255;
				*bptr++ = 0;
				*aptr++ = COLOR_OPACITY;
				break;
			case Plume::ORANGE:
				*bptr++ = 255;
				*bptr++ = 140;
				*bptr++ = 0;
				*aptr++ = COLOR_OPACITY;
				break;
			case Plume::RED:
				*bptr++ = 205;
				*bptr++ = 0;
				*bptr++ = 0;
				*aptr++ = COLOR_OPACITY;
				break;
			case Plume::PURPLE:
				*bptr++ = 153;
				*bptr++ = 50;
				*bptr++ = 204;
				*aptr++ = COLOR_OPACITY;
				break;
			case Plume::BLUE:
				*bptr++ = 65;
				*bptr++ = 105;
				*bptr++ = 225;
				*aptr++ = COLOR_OPACITY;
				break;
			case Plume::GREEN:
				*bptr++ = 39;
				*bptr++ = 139;
				*bptr++ = 34;
				*aptr++ = COLOR_OPACITY;
				break;
			default:
				*bptr++ = 255;
				*bptr++ = 255;
				*bptr++ = 255;
				*aptr++ = BACKGROUND_OPACITY;
				break;
		}
	}
	wxImage* img = new wxImage(IMG_WIDTH, IMG_HEIGHT, buffer2);
	img->SetAlpha(alpha);
	if (!img->SaveFile(filename, wxBITMAP_TYPE_PNG)){
		return false;
	}
	return true;
}

//====================================================================================
// constructor
MainWin::MainWin(wxWindow* parent):MainWindow(parent) /*, timer(this, TIMER_ID)*/{
	::wxInitAllImageHandlers();
	// set default values  for class variable
	statusbar->SetStatusText(wxT(""), 0);
	statusbar->SetStatusText(wxT(""), 1);
	framenum = 0;
	image = NULL;
	m_bitmap1->Show(false);
	draw_overlay = true;
	alt_bitmap_state = false;
	choice = 0;
	stateR = 0;
	wxPoint p (-1, -1);
	coorR1=p;
	coorR2=p;
	distance = wxT("");
	coorA1=p;
	coorA2=p;
	angle = wxT("");
	coorP = p;
	point = wxT("");
	
	bitmap_state = false;
	thickness = 2;
	
	//toolbar
	espace = new wxStaticText( m_toolBar1, wxID_ANY, wxT(" "), wxDefaultPosition, wxDefaultSize, 0 );
	espace->Wrap( -1 );
	m_toolBar1->AddControl( espace );

	m_staticText1 = new wxStaticText( m_toolBar1, wxID_ANY, wxT("Zone markers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_toolBar1->AddControl( m_staticText1 );
//	m_toolBar1->AddControl( espace );
	
	m_toolBar1->AddTool( ID_YELLOW, wxT("Yellow pen"), wxBitmap( wxT("crayon_jaune_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_ORANGE, wxT("Orange pen"), wxBitmap( wxT("crayon_orange_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_RED, wxT("Red pen"), wxBitmap( wxT("crayon_rouge_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_PURPLE, wxT("Purple pen"), wxBitmap( wxT("crayon_violet_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_BLUE, wxT("Blue pen"), wxBitmap( wxT("crayon_bleu_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_GREEN, wxT("Green pen"), wxBitmap( wxT("crayon_vert_vertical.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	
//	m_toolBar1->AddControl( espace );
	m_toolBar1->AddTool( ID_THINPENCIL, wxT("Thinner pen mark"), wxBitmap( wxT("crayon_fin.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_BIGPENCIL, wxT("Thicker pen"), wxBitmap( wxT("crayon_epais.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_RUBBER, wxT("Rubber"), wxBitmap( wxT("gomme.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );

	
//	m_toolBar1->AddControl( espace );
	m_toolBar1->AddSeparator();
	m_staticText2 = new wxStaticText( m_toolBar1, wxID_ANY, wxT("Measuring tools"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_toolBar1->AddControl( m_staticText2 );
//	m_toolBar1->AddControl( espace );
	m_toolBar1->AddTool( ID_RULER, wxT("Ruler"), wxBitmap( wxT("ruler.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_ANGLE, wxT("Angle"), wxBitmap( wxT("angle.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->AddTool( ID_POINTER, wxT("Pointer"), wxBitmap( wxT("pointer.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_CHECK, wxEmptyString, wxEmptyString );
	m_toolBar1->Realize();
	m_toolBar1->ToggleTool(ID_YELLOW, false);
	m_toolBar1->ToggleTool(ID_ORANGE, false);
	m_toolBar1->ToggleTool(ID_RED, false);
	m_toolBar1->ToggleTool(ID_PURPLE, false);
	m_toolBar1->ToggleTool(ID_BLUE, false);
	m_toolBar1->ToggleTool(ID_GREEN, false);
	m_toolBar1->ToggleTool(ID_RULER, false);
	m_toolBar1->ToggleTool(ID_ANGLE, false);
	m_toolBar1->ToggleTool(ID_POINTER, false);
	update_controls();
	
	winname_open = false;
}


//====================================================================================
// destructor
MainWin::~MainWin(){ 
	if (image!=NULL){
		delete image;
	}
}

//==================================================================================== 
void MainWin::load_image( wxCommandEvent& event ){

	// opens dialog windows and requests user to chose pgm file
	wxFileDialog odlg(this, wxT("Open frame bitmap"), wxT(""), wxT(""), wxT("PGM files (*.pgm)|*.pgm|PNG files (*.png)|*.png"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (odlg.ShowModal()!=wxID_OK){ 
		return;
	}

	// if an image was loaded, deletes it
	if (image!=NULL) {
		delete image;
		image = NULL;
	}
		
	// Loads image chosen by user
	wxImage* img = new wxImage;
	wxString fname = odlg.GetPath();
	m_bitmap1->SetCursor(*wxHOURGLASS_CURSOR);
	wxFileName fn(fname);
	string name = (const char*)fn.GetFullName().mb_str();
	string::size_type n = name.find_last_of('.');
	if (name.substr(n) == ".png"){
		img -> LoadFile(fname, wxBITMAP_TYPE_PNG);
	}else{
		img = load_pgm(fname.mb_str());
	}
	m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);

	// if the image was read correctly, updates the framenumber
	if (img!=NULL) {
		
		*img = img -> Rotate90(false);
		
		set_frames();
		image = img;
		statusbar->SetStatusText(fname, 0);

	}
	m_bitmap1->Show(true);
	update_image();
	
	// activate toolbar
	update_controls();
}

//====================================================================================
void MainWin::save_image( wxCommandEvent& event ){
	
	const uint8_t* bitmap = plm.get_bitmap();
	
	if (bitmap_state){
		int i(0);
		bool state = false;
		do {
			if (bitmap[i]!= 255){
				state = true; 
			}
			i++;
		}while (i < IMG_SIZE && !state);
		bitmap_state = state;
	}
	
	if (!bitmap_state){
		::wxMessageBox(wxT("Nothing to save."), wxT("Information"));
		return;
	}
	
	// opens dialog windows and asks user where to save the modified tags file
	wxFileDialog sdlg(this, wxT("Save bitmap"), wxT(""), wxT(""), wxT("Image files|*.png"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (sdlg.ShowModal()!=wxID_OK){
		return;
	}
	
	if (bitmap==NULL){
		::wxMessageBox(wxT("Internal error: bitmap object not found."), wxT("Error"), wxICON_ERROR);
		return;
	}
	
	wxString filename = sdlg.GetPath();
	if (!filename.EndsWith(wxT(".png"))){
		filename = filename + wxT(".png";)
	}
	
	if (!write_png(bitmap, filename)){
			::wxMessageBox(wxT("Could not write image of drawing."), wxT("Error"), wxICON_ERROR);
		return;
	}

}

//====================================================================================
void MainWin::save_binary( wxCommandEvent& event ){ 

  bool framenum_state(false);

  const uint8_t* bitmap = plm.get_bitmap();

	if (bitmap_state){
		int i(0);
		bool state = false;
		do {
			if (bitmap[i]!= 255){
				state = true; 
			}
			i++;
		}while (i < IMG_SIZE && !state);
		bitmap_state = state;
	}
		
	if (!bitmap_state && !framenum_state){
		::wxMessageBox(wxT("Nothing to save."), wxT("Information"));
		return;
	}
	// opens dialog windows and asks user where to save the modified tags file
	wxFileDialog sdlg(this, wxT("Save plume file"), wxT(""), wxT(""), wxT("Plume files|*.plume"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (sdlg.ShowModal()!=wxID_OK){
		return;
	}
	
	if (bitmap==NULL){
		::wxMessageBox(wxT("Internal error: bitmap object not found."), wxT("Error"), wxICON_ERROR);
		return;
	}
	
	wxString filename = sdlg.GetPath();
	if (!filename.EndsWith(wxT(".plume"))){
		filename = filename + wxT(".plume";)
	}

  if (!plm.write_plume((const char*)filename.mb_str())) {
		::wxMessageBox(wxT("Could not write plume file."), wxT("Error"), wxICON_ERROR);
		return;
	}
}

//====================================================================================
void MainWin::close_plume( wxCommandEvent& event ){
	this ->Close();
}

//====================================================================================
void MainWin::update_image(){
	
	if (image==NULL){ 
		::wxMessageBox(wxT("No image."), wxT("Error"), wxICON_ERROR);

		return;
	}
	wxBitmap bmp (*image);
  m_bitmap1->SetBitmap(bmp);
  m_bitmap1->Refresh();
}


//====================================================================================
void MainWin::change_cursor( wxMouseEvent& event ){
	update_cursor();
}

//=========================================================================================
void MainWin::update_cursor(){
	if (image == NULL){
		m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
	}else{
		
		switch (choice){
			case 0:
				m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
				break;
			case 1:		//color zones
					if (m_toolBar1->GetToolState(ID_YELLOW)){
						wxImage yellow_mouse(wxT("crayon_jaune.png"));
						yellow_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						yellow_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(yellow_mouse));
					}else if (m_toolBar1->GetToolState(ID_ORANGE)){
						wxImage orange_mouse("crayon_orange.png");
						orange_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						orange_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(orange_mouse));
					}else if (m_toolBar1->GetToolState(ID_RED)){
						wxImage red_mouse("crayon_rouge.png");
						red_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						red_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(red_mouse));
					}else if (m_toolBar1->GetToolState(ID_PURPLE)){
						wxImage purple_mouse("crayon_violet.png");
						purple_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						purple_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(purple_mouse));
					}else if (m_toolBar1->GetToolState(ID_BLUE)){
						wxImage blue_mouse("crayon_bleu.png");
						blue_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						blue_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(blue_mouse));
					}else if (m_toolBar1->GetToolState(ID_GREEN)){
						wxImage green_mouse("crayon_vert.png");
						green_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 1);
						green_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
						m_bitmap1 -> SetCursor(wxCursor(green_mouse));
					}else if (m_toolBar1->GetToolState(ID_RUBBER)){
						wxImage rubber_mouse("gomme_mouse.png");
						rubber_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 3);
						rubber_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 12);
						m_bitmap1 -> SetCursor(wxCursor(rubber_mouse));
					}else {
						m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
					}
				break;
				
			case 2:  // ruler	or angle
			
				if (m_toolBar1->GetToolState(ID_RULER)){
					wxImage ruler_mouse("croix_bleu_regle.png");
					ruler_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
					ruler_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
					m_bitmap1 -> SetCursor(wxCursor(ruler_mouse));
				}else if (m_toolBar1->GetToolState(ID_ANGLE)){
					wxImage angle_mouse("croix_vert_angle.png");
					angle_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
					angle_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
					m_bitmap1 -> SetCursor(wxCursor(angle_mouse));
				}else if (m_toolBar1->GetToolState(ID_POINTER)){
					wxImage pointer_mouse("croix_rouge_pointer.png");
					pointer_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
					pointer_mouse.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
					m_bitmap1 -> SetCursor(wxCursor(pointer_mouse));
				}else{
					m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
				}
				break;
		}	
	}
}
	
//====================================================================================
void MainWin::mark_spot( wxMouseEvent& event ){
	
	if (choice == 2){
	
		if (stateR == 1){
			coorR1 = event.GetPosition();
			stateR = 2;
			statusbar->SetStatusText(wxT("Select second point..."), 1);
			
		}else if (stateR == 2){
			coorR2 = event.GetPosition();
			//calculate size & display
			int dx = abs(coorR1.x - coorR2.x) * CORR;
			int dy = abs(coorR1.y - coorR2.y) * CORR;
			double size = sqrt(dx*dx + dy*dy);
			
			statusbar->SetStatusText(wxString::Format(wxT("Distance %0.1f px"), size), 1);
			stateR = 0;
			draw_overlay = true;
			m_bitmap1->SetBitmap(wxBitmap(*image));   // sets the bitmap back to the background image
			wxPoint p(-1,-1);
			coorR1 = p;
			coorR2 = p;
			choice = 0;
			
			
		} else if (stateA == 1){
			coorA1 = event.GetPosition();
			stateA = 2;
			statusbar->SetStatusText(wxT("Select direction point..."), 1);
		}else if (stateA == 2){
			coorA2 = event.GetPosition();
			int dx = coorA2.x - coorA1.x;
			int dy = coorA2.y - coorA1.y;
			double a = 180/M_PI * atan2(-dy, dx) - 90;
			limit_angle(a);
			angle = wxString::Format(wxT("%0.2f"), a);
			statusbar->SetStatusText(wxString::Format(wxT("Angle %0.2f degrees"),a), 1);
			stateA = 0;
			draw_overlay = true;
			m_bitmap1->SetBitmap(wxBitmap(*image));   // sets the bitmap back to the background image
			wxPoint p(-1,-1);
			coorA1 = p;
			coorA2 = p;
			choice = 0;
			
		} else if (stateP){
			coorP = event.GetPosition();
			int x = IMAGE_WIDTH - coorP.y * CORR;
			int y = coorP.x * CORR;
			point = wxString::Format(wxT("Point at x= %d, y= %d"), x, y);
			stateP = false;
			draw_overlay = true;
			m_bitmap1->SetBitmap(wxBitmap(*image));   // sets the bitmap back to the background image
			coorP.x = -1;
			coorP.y = -1;
			statusbar->SetStatusText(point, 1);
			choice = 0;
		}
		
	}else if (choice == 1){
	  alt_bitmap_state = false;
		pencil_state = false;
		if (tool == ID_RUBBER) {
		  m_bitmap1->SetCursor(*wxHOURGLASS_CURSOR);
		  m_bitmap1->Refresh();    // redraws the bitmap when the rubber is used
		  m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
		}
	}

	update_controls();
	update_cursor();
}


//=========================================================================================
void MainWin::color_position( wxMouseEvent& event ){
	
	if (pencil_state){
		if (choice == 1){
			
				wxPoint p = event.GetPosition();
				int temp = p.x;
				p.x = IMG_HEIGHT_ROTATED - p.y;
				p.y = temp;
				switch (tool) {
					case ID_YELLOW:
						if (plm.get_zonename(Plume::YELLOW) == "" && !winname_open) {
						  winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the yellow zones."),wxT("Please enter text:"), wxT("yellow zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()== wxID_OK){
                plm.set_zonename(Plume::YELLOW, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::YELLOW);
						bitmap_state = true;
						break;
					case ID_ORANGE:
						if (plm.get_zonename(Plume::ORANGE) == "" && !winname_open){
							winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the orange zones."),wxT("Please enter text:"), wxT("orange zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()==wxID_OK){
								plm.set_zonename(Plume::ORANGE, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::ORANGE);
						bitmap_state = true;
						break;
					case ID_RED:
						if (plm.get_zonename(Plume::RED) == "" && !winname_open){
							winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the red zones."),wxT("Please enter text:"), wxT("red zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()==wxID_OK){
								plm.set_zonename(Plume::RED, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::RED);
						bitmap_state = true;
						break;
					case ID_PURPLE:
						if (plm.get_zonename(Plume::PURPLE) == "" && !winname_open){
              winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the purple zones."),wxT("Please enter text:"), wxT("purple zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()==wxID_OK){
								plm.set_zonename(Plume::PURPLE, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::PURPLE);
						bitmap_state = true;
						break;
					case ID_BLUE:
						if (plm.get_zonename(Plume::BLUE) == "" && !winname_open){
							winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the blue zones."),wxT("Please enter text:"), wxT("blue zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()==wxID_OK){
								plm.set_zonename(Plume::BLUE, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::BLUE);
						bitmap_state = true;
						break;
					case ID_GREEN:
						if (plm.get_zonename(Plume::GREEN) == "" && !winname_open){
							winname_open = true;
							wxTextEntryDialog tdlg(this, wxT("Please enter a short description for the green zones."),wxT("Please enter text:"), wxT("green zone"), wxOK | wxCANCEL);
							if (tdlg.ShowModal()==wxID_OK){
								plm.set_zonename(Plume::GREEN, (const char*)tdlg.GetValue().mb_str());
							}
							winname_open = false;
							pencil_state = false;
							update_status();
							break;
						}
						color_bitmap(p.x, p.y, Plume::GREEN);
						bitmap_state = true;
						break;
					case ID_RUBBER:
						color_bitmap(p.x, p.y, Plume::RUBBER);
						break;
			}
			//m_bitmap1->Refresh();
			
		}else if (choice == 2){
			if (stateR == 2){
				coorR2 = event.GetPosition();
				int dx = abs(coorR1.x - coorR2.x) * CORR;
				int dy = abs(coorR1.y - coorR2.y) * CORR;
				double size = sqrt(dx*dx + dy*dy);
				distance = wxString::Format(wxT("%0.2f"), size);
			}else if (stateA == 2){
				coorA2 = event.GetPosition();
				int dx = coorA2.x - coorA1.x;
				int dy = coorA2.y - coorA1.y;
				double a = 180/M_PI * atan2(-dy, dx) - 90;
				limit_angle(a);
				angle = wxString::Format(wxT("%0.2f"), a);
			}else if (stateP){
			  coorP = event.GetPosition();
				int x = IMAGE_WIDTH - coorP.y * CORR;
				int y = coorP.x * CORR;
				point = wxString::Format(wxT("x= %d, y= %d"), x, y);
			}
			m_bitmap1->Refresh();
		}
	}
}

//=========================================================================================
void MainWin::update_status(){
	if (m_toolBar1->GetToolState(ID_YELLOW) && plm.get_zonename(Plume::YELLOW) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::YELLOW).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_ORANGE) && plm.get_zonename(Plume::ORANGE) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::ORANGE).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_RED) && plm.get_zonename(Plume::RED) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::RED).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_PURPLE) && plm.get_zonename(Plume::PURPLE) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::PURPLE).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_BLUE) && plm.get_zonename(Plume::BLUE) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::BLUE).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_GREEN) && plm.get_zonename(Plume::GREEN) != ""){
		statusbar->SetStatusText((const char*)plm.get_zonename(Plume::GREEN).c_str(), 1);
	}else if (m_toolBar1->GetToolState(ID_RUBBER)){
		statusbar->SetStatusText(wxT("Erasing..."), 1);
	}
}

//=========================================================================================
void MainWin::OnPencil(wxCommandEvent& event){
	if (image == NULL){
		return;
	}
	
	if (choice != 1){
		choice = 1;
		tool = event.GetId();
	
		update_status();
	
	}else{
		choice = 0;
	}
	update_controls();
	update_cursor();
}

//=========================================================================================
void MainWin::OnTool(wxCommandEvent& event){
	if (image == NULL){
		return;
	}
	
	if (choice != 2){
		choice = 2;
		tool = event.GetId();

		if (m_toolBar1->GetToolState(ID_RULER)){
			stateR = 1;
			statusbar->SetStatusText(wxT("Select first point..."), 1);
			draw_overlay = false;
      m_bitmap1->SetCursor(*wxHOURGLASS_CURSOR);
			render_to_bitmap();
			m_bitmap1->SetBitmap(alt_bitmap);
			m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
		}else{
			stateR = 0;
		}
		
		if (m_toolBar1->GetToolState(ID_ANGLE)){
			stateA = 1;
			statusbar->SetStatusText(wxT("Select reference point..."), 1);
			draw_overlay = false;
			m_bitmap1->SetCursor(*wxHOURGLASS_CURSOR);
			render_to_bitmap();
			m_bitmap1->SetBitmap(alt_bitmap);
			m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
		}else{
			stateA = 0;
		}
		
		if (m_toolBar1->GetToolState(ID_POINTER)){
			stateP = true;
			pencil_state = true;
			draw_overlay = false;
      m_bitmap1->SetCursor(*wxHOURGLASS_CURSOR);
			render_to_bitmap();
			m_bitmap1->SetBitmap(alt_bitmap);
			m_bitmap1->SetCursor(*wxSTANDARD_CURSOR);
			statusbar->SetStatusText(wxT("Select a point..."), 1);
		}else{
			stateP = false;
			pencil_state = false;
		}
	}else{
		choice = 0;
		stateP = false;
		stateR = 0;
		stateA = 0;
		statusbar->SetStatusText(wxT(""), 1);
	}

  if (stateA == 0 && stateR == 0 && stateP == 0 && draw_overlay == false) {
    draw_overlay = true;
    m_bitmap1->SetBitmap(wxBitmap(*image));
	}
	update_controls();
	update_cursor();
	m_bitmap1->Refresh();
}

//=========================================================================================
void MainWin::render_to_bitmap(){

  if (alt_bitmap_state) {
    return;
  }

  wxMemoryDC dc;

  // copies the background image into the alternative bitmap
  alt_bitmap = wxBitmap(*image);
  
  const uint8_t* bitmap = plm.get_bitmap();
  
  dc.SelectObject(alt_bitmap);
  for (int p(0); p<IMG_SIZE; p++){
    wxPoint pt (p%IMG_WIDTH, p/IMG_WIDTH);
    int temp (pt.x);
    pt.x = pt.y;
    pt.y = IMG_HEIGHT_ROTATED - temp;
    wxColor color;
    switch (bitmap[p]){
      case Plume::YELLOW:
        color.Set(255, 255, 0);
        break;
      case Plume::ORANGE:
        color.Set(255, 140, 0);
        break;
      case Plume::RED:
        color.Set(205, 0, 0);
        break;
      case Plume::PURPLE:
        color.Set(153, 50, 204);
        break;
      case Plume::BLUE:
        color.Set(65, 105, 225);
        break;
      case Plume::GREEN:
        color.Set(39, 139, 34);
        break;
    }
    if (color.Ok()){
      wxPen colorpen(color, 1);  // pen with color ad width 1
      dc.SetPen(colorpen);
      dc.DrawPoint(pt.x, pt.y);
    }
  }
  alt_bitmap_state = true;	
}

//=========================================================================================
void MainWin::draw_image( wxPaintEvent& event ){
	wxAutoBufferedPaintDC dc(m_bitmap1);
  wxBitmap bmp_obj = m_bitmap1->GetBitmap();
  dc.DrawBitmap(bmp_obj, 0, 0, false);
	const uint8_t* bitmap = plm.get_bitmap();
	
	if (draw_overlay) {

    // draw colored zones
    for (int p(0); p<IMG_SIZE; p++){
      
      wxPoint pt (p%IMG_WIDTH, p/IMG_WIDTH);
      int temp (pt.x);
      pt.x = pt.y;
      pt.y = IMG_HEIGHT_ROTATED - temp;
      wxColor color;
      switch (bitmap[p]){
        case Plume::YELLOW:
          color.Set(255, 255, 0);
          break;
        case Plume::ORANGE:
          color.Set(255, 140, 0);
          break;
        case Plume::RED:
          color.Set(205, 0, 0);
          break;
        case Plume::PURPLE:
          color.Set(153, 50, 204);
          break;
        case Plume::BLUE:
          color.Set(65, 105, 225);
          break;
        case Plume::GREEN:
          color.Set(39, 139, 34);
          break;
      }
      if (color.Ok()){
        wxPen colorpen(color, 1);  // pen with color ad width 1
        dc.SetPen(colorpen);
        dc.DrawPoint(pt.x, pt.y);
      }
    }
  }
	
	// draw ruler line
	if (stateR == 2 && coorR1.x!=-1 && coorR2.x!=-1){
		wxColor blue(0,45,240);
		wxPen bluepen(blue, 1);
		dc.SetPen(bluepen);
		dc.DrawLine(coorR1.x, coorR1.y, coorR2.x, coorR2.y);
		dc.DrawRotatedText(distance, coorR2.x + 3, coorR2.y - 3, 0);
	}
	
	// draw angle line
	if (stateA == 2  && coorA1.x!=-1 && coorA2.x!=-1){
		wxColor vert(42,142,42);
		wxPen greenpen(vert, 1);
		dc.SetPen(greenpen);
		dc.DrawLine(coorA1.x, coorA1.y, coorA2.x, coorA2.y);
		dc.DrawRotatedText(angle, coorA2.x + 3, coorA2.y - 3, 0);
	}
	
	if (stateP && coorP.x!=-1 && coorP.x!=-1){
		dc.DrawRotatedText(point, coorP.x + 3, coorP.y - 3, 0);
	}
}

//====================================================================================
void MainWin::activate_pencil( wxMouseEvent& event ){
	if (image == NULL){
		return;
	}
	pencil_state = true;
	color_position(event);
}

//=========================================================================================
void MainWin::update_controls(){
	
	if (image == NULL){
		m_toolBar1->ToggleTool(ID_YELLOW, false);
		m_toolBar1->ToggleTool(ID_ORANGE, false);
		m_toolBar1->ToggleTool(ID_RED, false);
		m_toolBar1->ToggleTool(ID_PURPLE, false);
		m_toolBar1->ToggleTool(ID_BLUE, false);
		m_toolBar1->ToggleTool(ID_GREEN, false);
		m_toolBar1->ToggleTool(ID_RUBBER, false);
		m_toolBar1->ToggleTool(ID_RULER, false);
		m_toolBar1->ToggleTool(ID_ANGLE, false);
		m_toolBar1->ToggleTool(ID_POINTER, false);
		m_toolBar1->ToggleTool(ID_THINPENCIL, false);
		m_toolBar1->ToggleTool(ID_BIGPENCIL, false);
		
		m_toolBar1 ->EnableTool(ID_YELLOW, false);
		m_toolBar1 ->EnableTool(ID_ORANGE, false);
		m_toolBar1 ->EnableTool(ID_RED, false);
		m_toolBar1 ->EnableTool(ID_PURPLE, false);
		m_toolBar1 ->EnableTool(ID_BLUE, false);
		m_toolBar1 ->EnableTool(ID_GREEN, false);
		m_toolBar1 ->EnableTool(ID_RUBBER, false);
		m_toolBar1 ->EnableTool(ID_RULER, false);
		m_toolBar1 ->EnableTool(ID_ANGLE, false);
		m_toolBar1 ->EnableTool(ID_POINTER, false);
		m_toolBar1 ->EnableTool(ID_THINPENCIL, false);
		m_toolBar1 ->EnableTool(ID_BIGPENCIL, false);
	}else{
		
		switch (choice){
			case 0:
				m_toolBar1->ToggleTool(ID_YELLOW, false);
				m_toolBar1->ToggleTool(ID_ORANGE, false);
				m_toolBar1->ToggleTool(ID_RED, false);
				m_toolBar1->ToggleTool(ID_PURPLE, false);
				m_toolBar1->ToggleTool(ID_BLUE, false);
				m_toolBar1->ToggleTool(ID_GREEN, false);
				m_toolBar1->ToggleTool(ID_RUBBER, false);
				m_toolBar1->ToggleTool(ID_RULER, false);
				m_toolBar1->ToggleTool(ID_ANGLE, false);
				m_toolBar1->ToggleTool(ID_POINTER, false);
				
				m_toolBar1 ->EnableTool(ID_YELLOW, true);
				m_toolBar1 ->EnableTool(ID_ORANGE, true);
				m_toolBar1 ->EnableTool(ID_RED, true);
				m_toolBar1 ->EnableTool(ID_PURPLE, true);
				m_toolBar1 ->EnableTool(ID_BLUE, true);
				m_toolBar1 ->EnableTool(ID_GREEN, true);
				m_toolBar1 ->EnableTool(ID_RUBBER, true);
				m_toolBar1 ->EnableTool(ID_RULER, true);
				m_toolBar1 ->EnableTool(ID_ANGLE, true);
				m_toolBar1 ->EnableTool(ID_POINTER, true);
				m_toolBar1 ->EnableTool(ID_THINPENCIL, true);
				m_toolBar1 ->EnableTool(ID_BIGPENCIL, true);
				break;
				
			case 1:		//tools inactivates and color pencils enabled
				m_toolBar1 ->EnableTool(ID_RULER, false);
				m_toolBar1 ->EnableTool(ID_ANGLE, false);
				m_toolBar1 ->EnableTool(ID_POINTER, false);
				m_toolBar1 ->EnableTool(ID_YELLOW, true);
				m_toolBar1 ->EnableTool(ID_ORANGE, true);
				m_toolBar1 ->EnableTool(ID_RED, true);
				m_toolBar1 ->EnableTool(ID_PURPLE, true);
				m_toolBar1 ->EnableTool(ID_BLUE, true);
				m_toolBar1 ->EnableTool(ID_GREEN, true);
				m_toolBar1 ->EnableTool(ID_RUBBER, true);
				m_toolBar1 ->EnableTool(ID_THINPENCIL, true);
				m_toolBar1 ->EnableTool(ID_BIGPENCIL, true);
				
				if (m_toolBar1->GetToolState(ID_YELLOW)){
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if (m_toolBar1->GetToolState(ID_ORANGE)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if (m_toolBar1->GetToolState(ID_RED)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if (m_toolBar1->GetToolState(ID_PURPLE)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if (m_toolBar1->GetToolState(ID_BLUE)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if (m_toolBar1->GetToolState(ID_GREEN)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_RUBBER, false);
				}else if(m_toolBar1->GetToolState(ID_RUBBER)){
					m_toolBar1 ->EnableTool(ID_YELLOW, false);
					m_toolBar1 ->EnableTool(ID_ORANGE, false);
					m_toolBar1 ->EnableTool(ID_RED, false);
					m_toolBar1 ->EnableTool(ID_PURPLE, false);
					m_toolBar1 ->EnableTool(ID_BLUE, false);
					m_toolBar1 ->EnableTool(ID_GREEN, false);
				}
				break;
			
			case 2:  // color pencils disabeled and a tool enabled : ruler, angle or pointer
				m_toolBar1 ->EnableTool(ID_YELLOW, false);
				m_toolBar1 ->EnableTool(ID_ORANGE, false);
				m_toolBar1 ->EnableTool(ID_RED, false);
				m_toolBar1 ->EnableTool(ID_PURPLE, false);
				m_toolBar1 ->EnableTool(ID_BLUE, false);
				m_toolBar1 ->EnableTool(ID_GREEN, false);
				m_toolBar1 ->EnableTool(ID_RUBBER, false);
				m_toolBar1 ->EnableTool(ID_THINPENCIL, false);
				m_toolBar1 ->EnableTool(ID_BIGPENCIL, false);
				
				
				m_toolBar1 ->EnableTool(ID_RULER,true);
				m_toolBar1 ->EnableTool(ID_ANGLE, true);
				m_toolBar1 ->EnableTool(ID_POINTER, true);
				
				if (m_toolBar1->GetToolState(ID_RULER)){
					m_toolBar1 ->EnableTool(ID_ANGLE, false);
					m_toolBar1 ->EnableTool(ID_POINTER, false);
				}else if (m_toolBar1->GetToolState(ID_ANGLE)){
					m_toolBar1 ->EnableTool(ID_RULER, false);
					m_toolBar1 ->EnableTool(ID_POINTER, false);
				}else if (m_toolBar1->GetToolState(ID_POINTER)){
					m_toolBar1 ->EnableTool(ID_RULER, false);
					m_toolBar1 ->EnableTool(ID_ANGLE, false);
				}
				break;
		}	
		
	}
	
}

//====================================================================================
void MainWin::toggle_patch( wxMoveEvent& event ){
	update_controls();
}

//====================================================================================
void MainWin::OnPencilThickness(wxCommandEvent& event){
	if (m_toolBar1->GetToolState(ID_BIGPENCIL)){
		if (thickness < UPPER_PENCIL){
			thickness++;
		}
		m_toolBar1 ->ToggleTool(ID_BIGPENCIL, false);
	}else if (m_toolBar1->GetToolState(ID_THINPENCIL)){
		if (thickness > 1){
			thickness--;
		}
		m_toolBar1 ->ToggleTool(ID_THINPENCIL, false);
	}
}


//====================================================================================
void MainWin::reset_bitmap( wxCommandEvent& event ){
	uint8_t* bitmap = plm.get_bitmap();
	memset(bitmap, 255, IMG_WIDTH * IMG_HEIGHT);
	bitmap_state = false;
	plm.set_zonename(Plume::YELLOW, "");
	plm.set_zonename(Plume::ORANGE, "");
	plm.set_zonename(Plume::RED, "");
	plm.set_zonename(Plume::PURPLE, "");
	plm.set_zonename(Plume::BLUE, "");
	plm.set_zonename(Plume::GREEN, "");
	plm.set_firstframe(0);
	plm.set_lastframe(0);
	statusbar->SetStatusText(wxT(""), 1);
	m_bitmap1 ->Refresh();
}

//====================================================================================
void MainWin::open_colorcode( wxCommandEvent& event ){

	ColorWin* ma_fenetre = new ColorWin(this);
	ma_fenetre -> set_yellow((const char*)plm.get_zonename(Plume::YELLOW).c_str());
	ma_fenetre -> set_orange((const char*)plm.get_zonename(Plume::ORANGE).c_str());
	ma_fenetre -> set_red((const char*)plm.get_zonename(Plume::RED).c_str());
	ma_fenetre -> set_purple((const char*)plm.get_zonename(Plume::PURPLE).c_str());
	ma_fenetre -> set_blue((const char*)plm.get_zonename(Plume::BLUE).c_str());
	ma_fenetre -> set_green((const char*)plm.get_zonename(Plume::GREEN).c_str());
	
	if ((ma_fenetre -> ShowModal()) == 0){
			plm.set_zonename(Plume::YELLOW, (const char*)ma_fenetre -> get_yellow().mb_str());
			plm.set_zonename(Plume::ORANGE, (const char*)ma_fenetre -> get_orange().mb_str());
			plm.set_zonename(Plume::RED, (const char*)ma_fenetre -> get_red().mb_str());
			plm.set_zonename(Plume::PURPLE, (const char*)ma_fenetre -> get_purple().mb_str());
			plm.set_zonename(Plume::BLUE, (const char*)ma_fenetre -> get_blue().mb_str());
			plm.set_zonename(Plume::GREEN, (const char*)ma_fenetre -> get_green().mb_str());
	}
	delete ma_fenetre;
	
	update_status();
	
}


//====================================================================================
void MainWin::open_framelimits( wxCommandEvent& event ){
	event.Skip();
	set_frames();
}

//====================================================================================
void MainWin::set_frames(){
	FrameWin* ma_fenetre = new FrameWin(this);
	ma_fenetre -> set_startframe((const char*)(to_string(plm.get_firstframe())).c_str());
	ma_fenetre -> set_stopframe((const char*)(to_string(plm.get_lastframe())).c_str());
	
	if ((ma_fenetre -> ShowModal()) == 0){				 
		plm.set_firstframe(atoi(ma_fenetre -> get_startframe().mb_str()));
		plm.set_lastframe(atoi(ma_fenetre -> get_stopframe().mb_str()));
	}
	delete ma_fenetre;
}



//====================================================================================
BEGIN_EVENT_TABLE(MainWin, MainWindow)
EVT_TOOL_RANGE(ID_YELLOW, ID_RUBBER, MainWin::OnPencil)
EVT_TOOL_RANGE(ID_RULER, ID_POINTER, MainWin::OnTool)
EVT_TOOL_RANGE(ID_BIGPENCIL, ID_THINPENCIL, MainWin::OnPencilThickness)
EVT_MOVE( MainWin::toggle_patch)
END_EVENT_TABLE()
