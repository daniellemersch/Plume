//Copyright (c) Danielle Mersch. All rights reserved.

#ifndef __mainwin__
#define __mainwin__

#include <wx/wx.h>
#include <wx/imaglist.h>
#include <wx/msgdlg.h>
#include <wx/ffile.h>
#include <wx/filedlg.h>
#include <wx/numdlg.h>
#include <wx/filename.h>
#include <wx/dcbuffer.h>
#include <wx/accel.h>
#include <wx/cursor.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>
#include <wx/dynarray.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/toolbar.h>
#include <wx/pen.h>
#include <wx/dialog.h>


#include <stdint.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <cmath>

#include <libants/plume.h>

#include "wx_plume.h"
#include "colorwin.h"
#include "frameswin.h"


using namespace std;


const int ID_YELLOW = 1; // IDs for color pencils
const int ID_ORANGE = 2;	
const int ID_RED = 3;
const int ID_PURPLE = 4;
const int ID_BLUE = 5;
const int ID_GREEN = 6;
const int ID_RUBBER = 7;  ///< ID of rubber (i.e. white pencil)

const int ID_RULER = 10;  ///< ID of ruler tool
const int ID_ANGLE = 11;	///< ID of Angle tool
const int ID_POINTER = 12;	///< ID of pointer tool

const int ID_BIGPENCIL = 21;		///< ID of button to make the pencil draw thicker
const int ID_THINPENCIL = 22;		///< ID of button to make the pencil draw thinner

class MainWin : public MainWindow {

	public:
		
		MainWin(wxWindow* parent);
		~MainWin();
	
	protected:
		
		/**\brief Asks the user for the image to load (PGM or PNG), and loads it
		 * \param event
		 */
		void load_image( wxCommandEvent& event );
		
		/**\brief Saves the current drawing in a PNG file
		 * \param event
		 */
		void save_image( wxCommandEvent& event );
		
		/**\brief Asks the user where to save the .plume file, and saves it
		 * \param event
		 */
		void save_binary( wxCommandEvent& event );
		
		/**\brief Closes the Plume application
		 * \param event
		 */
		void close_plume( wxCommandEvent& event );
		
		/**\brief Redraws the entire image
		 */
		void update_image();
		
		/**\brief Called on mouse events, to update the mouse cursor if needed
		 * \param event
		 */
		void change_cursor( wxMouseEvent& event );
		
		/**\brief Updates the mouse cursor depending on the currently selected function
		 */
		void update_cursor();
		
		/**\brief Handles mouse clicks for tools (ruler, angle protractor, coordinates)
		 * \param event
		 */
		void mark_spot( wxMouseEvent& event );
		
		/**\brief Handles clicks on toolbar for tools
		 * \param event
		 */
		void OnTool(wxCommandEvent& event);
		
		/**\brief Handles clicks on toolbar for pencils
		 * \param event
		 */
		void OnPencil(wxCommandEvent& event);
		
		/**\brief Draws the entire image on the screen, including rulers if needed
		 * \param event
		 */
		void draw_image( wxPaintEvent& event );
		
		/**\brief Reacts to mouse movement for drawing
		 * \param event
		 */
		void color_position( wxMouseEvent& event );
		
		/**\brief Updates the state of all buttons according to the currently selected tool
		 */
		void update_controls();
		
		/**\brief Loads the image from a PGM file
		 * \param filename The name of the file to be loaded
		 * \return A pointer to a wxImage object with the loaded image
		 */
		wxImage* load_pgm(const char* filename);
		
		/**\brief Writes a png file with the color overlay, this file can be overlaid on the tracking images with VirtualDub
		 * \param bitmap Bitmap containing the color code of each position
		 * \param filename Name of the png file to be written
		 * \return True is the png image was written with succes, false otherwise
		 */
		bool write_png(const uint8_t* bitmap, wxString filename);

		/**\brief Reacts to mouse clicks for drawing
		 * \param event
		 */
		void activate_pencil( wxMouseEvent& event );
		
		/**\brief Updates the state of all buttons when moving the window (but what was it for??)
		 * \param event
		 */
		void toggle_patch( wxMoveEvent& event );
		
		/**\brief Draws a "point" with the current thickness on the image
		 * \param x X coordinate of the point to be drawn (left corner)
		 * \param y Y coordinate of the point to be drawn (bottom corner)
		 * \param color Zone number
		 */
		void color_bitmap(const int x, const int y, const int color);
		
		/**\brief Sets pencil thickness when clicking on the thickness buttons
		 * \param event
		 */
		void OnPencilThickness(wxCommandEvent& event);
		
		/**\brief Writes a message about the currently selected pencil/rubber in the status bar
		 */
		void update_status();
		
		/**\brief Asks the user for a .plume file and loads it
		 * \param event
		 */
		void load_plume( wxCommandEvent& event );
		
		/**\brief Erases all info about the drawing (plume bitmap, zone names)
		 * \param event
		 */
		void reset_bitmap( wxCommandEvent& event );
		
		/**\brief Displays the window to edit the zone names
		 * \param event
		 */
		void open_colorcode( wxCommandEvent& event );
		
		/**\brief Renders the current plume data over the background image, into the alt_bitmap object
		 */
		void render_to_bitmap();
		
		// editor for frame validity
		void open_framelimits( wxCommandEvent& event );
	
		void set_frames();
		
	private:
		wxImage* image;								///< image that is currently loaded
		wxBitmap alt_bitmap;          ///< alternative bitmap to display instead of backgroud image
		bool alt_bitmap_state;        ///< true if the alternative bitmap is up to date
		int framenum;								  ///< frame number of current image, -1 if no number
		bool draw_overlay;						///< state of the overlay bitmap
		wxStaticText* espace;					///< text on toolbar
		wxStaticText* m_staticText1;	///< text on toolbar: displays the image file loaded
		wxStaticText* m_staticText2;	///< text on toolbar: displays messages associaed with the tool used
    bool winname_open;          ///< true if a window that is asking for the name of a pencil is open
		int tool;										///< ID of tool in use
		int choice;									///< number of selection and usage
		int stateR;									///< state of ruler: 0 no point, 1 first point selected, 2 second point selected
		wxPoint coorR1;							///< position of the first point of the ruler
		wxPoint coorR2;							///< position of the second point of the ruler
		wxString distance;					///< distance between coorR1 and coorR2
		int stateA;									///< state of angle selection
		wxPoint coorA1;							///< reference point for absolut angle measurement
		wxPoint coorA2;							///< point indicating the absolut angle 
		wxString angle;							///< absolut angle as in the tracking data
		bool stateP;								///< state of point
		wxPoint coorP;		
		wxString point;
		bool bitmap_state;					///< true if colored, false if not
		bool pencil_state;
		unsigned int thickness;		/// thickness of point 
		
		Plume plm;                  ///< a Plume object
		
		DECLARE_EVENT_TABLE()
};

#endif //__mainwin__
