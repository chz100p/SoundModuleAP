/**********************************************************************

  SoundModule: A Digital Audio Editor

  SoundModuleToolBar.cpp

*//*******************************************************************/

#include "../Audacity.h"
#include "../Experimental.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/dc.h>
#include <wx/event.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/timer.h>
#endif
#include <wx/tooltip.h>

#include "SoundModuleToolBar.h"
#include "MeterToolBar.h"

#include "../AColor.h"
#include "../AllThemeResources.h"
#include "../AudioIO.h"
#include "../ImageManipulation.h"
#include "../Prefs.h"
#include "../Project.h"
#include "../Theme.h"
#include "../Track.h"
#include "../widgets/AButton.h"

#include "../export/Export.h"

IMPLEMENT_CLASS(SoundModuleToolBar, ToolBar);

//static
AudacityProject *SoundModuleToolBar::mBusyProject = NULL;

////////////////////////////////////////////////////////////
/// Methods for SoundModuleToolBar
////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(SoundModuleToolBar, ToolBar)
   EVT_CHAR(SoundModuleToolBar::OnKeyEvent)
   EVT_BUTTON(ID_SOUNDMODULE_BUTTON,   SoundModuleToolBar::OnSoundModule)
END_EVENT_TABLE()

//Standard constructor
SoundModuleToolBar::SoundModuleToolBar()
: ToolBar(SoundModuleBarID, _("OrenoKoukaon!!"), wxT("SoundModule"))
{
   mPaused = false;

   gPrefs->Read(wxT("/GUI/ErgonomicTransportButtons"), &mErgonomicTransportButtons, true);
   mStrLocale = gPrefs->Read(wxT("/Locale/Language"), wxT(""));

   mSizer = NULL;
   mCutPreviewTracks = NULL;
}

SoundModuleToolBar::~SoundModuleToolBar()
{
}


void SoundModuleToolBar::Create(wxWindow * parent)
{
   ToolBar::Create(parent);
}

// This is a convenience function that allows for button creation in
// MakeButtons() with fewer arguments
AButton *SoundModuleToolBar::MakeButton(teBmps eEnabledUp, teBmps eEnabledDown, teBmps eDisabled,
                                    int id,
                                    bool processdownevents,
                                    const wxChar *label)
{
   AButton *r = ToolBar::MakeButton(
      bmpRecoloredUpLarge, bmpRecoloredDownLarge, bmpRecoloredHiliteLarge,
      eEnabledUp, eEnabledDown, eDisabled,
      wxWindowID(id),
      wxDefaultPosition, processdownevents,
      theTheme.ImageSize( bmpRecoloredUpLarge ));
   r->SetLabel(label);
   r->SetFocusRect( r->GetRect().Deflate( 12, 12 ) );

   return r;
}

void SoundModuleToolBar::Populate()
{
   MakeButtonBackgroundsLarge();

   mPlay = MakeButton( bmpSoundModuleFlash, bmpSoundModuleFlash, bmpSoundModuleFlashDisabled,
      ID_SOUNDMODULE_BUTTON, true, _("OrenoKoukaon!!"));

#if wxUSE_TOOLTIPS
   RegenerateToolsTooltips();
   wxToolTip::Enable(true);
   wxToolTip::SetDelay(1000);
#endif

   // Set default order and mode
   ArrangeButtons();
}

void SoundModuleToolBar::RegenerateToolsTooltips()
{
#if wxUSE_TOOLTIPS
   for (long iWinID = ID_SOUNDMODULE_BUTTON; iWinID < BUTTON_COUNT; iWinID++)
   {
      wxWindow* pCtrl = this->FindWindow(iWinID);
      wxString strToolTip = pCtrl->GetLabel();
      pCtrl->SetToolTip(strToolTip);
   }
#endif
}

void SoundModuleToolBar::UpdatePrefs()
{
   bool updated = false;
   bool active;

   gPrefs->Read( wxT("/GUI/ErgonomicTransportButtons"), &active, true );
   if( mErgonomicTransportButtons != active )
   {
      mErgonomicTransportButtons = active;
      updated = true;
   }
   wxString strLocale = gPrefs->Read(wxT("/Locale/Language"), wxT(""));
   if (mStrLocale != strLocale)
   {
      mStrLocale = strLocale;
      updated = true;
   }

   if( updated )
   {
      ReCreateButtons(); // side effect: calls RegenerateToolsTooltips()
      Updated();
   }
   else
      // The other reason to regenerate tooltips is if keyboard shortcuts for
      // transport buttons changed, but that's too much work to check for, so just
      // always do it. (Much cheaper than calling ReCreateButtons() in all cases.
      RegenerateToolsTooltips();


   // Set label to pull in language change
   SetLabel(_("Transport"));

   // Give base class a chance
   ToolBar::UpdatePrefs();
}

void SoundModuleToolBar::ArrangeButtons()
{
   int flags = wxALIGN_CENTER | wxRIGHT;

   // (Re)allocate the button sizer
   if( mSizer )
   {
      Detach( mSizer );
      delete mSizer;
   }
   mSizer = new wxBoxSizer( wxHORIZONTAL );
   Add( mSizer, 1, wxEXPAND );

   // Start with a little extra space
   mSizer->Add( 5, 55 );

   // Add the buttons in order based on ergonomic setting
   if( mErgonomicTransportButtons )
   {
      mSizer->Add( mPlay,   0, flags, 2 );
   }
   else
   {
      mSizer->Add( mPlay,   0, flags, 2 );
   }

   // Layout the sizer
   mSizer->Layout();

   // Layout the toolbar
   Layout();

   // (Re)Establish the minimum size
   SetMinSize( GetSizer()->GetMinSize() );
}

void SoundModuleToolBar::ReCreateButtons()
{
    bool playDown = false;
    bool playShift = false;

   // ToolBar::ReCreateButtons() will get rid of its sizer and
   // since we've attached our sizer to it, ours will get deleted too
   // so clean ours up first.
   if( mSizer )
   {
       playDown = mPlay->IsDown();
       playShift = mPlay->WasShiftDown();
      Detach( mSizer );

      delete mSizer;
      mSizer = NULL;
   }

   ToolBar::ReCreateButtons();

   EnableDisableButtons();

   RegenerateToolsTooltips();
}

void SoundModuleToolBar::Repaint( wxDC *dc )
{
#ifndef USE_AQUA_THEME
   wxSize s = mSizer->GetSize();
   wxPoint p = mSizer->GetPosition();

   wxRect bevelRect( p.x, p.y, s.GetWidth() - 1, s.GetHeight() - 1 );
   AColor::Bevel( *dc, true, bevelRect );
#endif
}

void SoundModuleToolBar::EnableDisableButtons()
{
   //TIDY-ME: Button logic could be neater.
   AudacityProject *p = GetActiveProject();
   bool tracks = false;
   bool playing = mPlay->IsDown();
   bool recording = false; // mRecord->IsDown();
   bool busy = gAudioIO->IsBusy() || playing || recording;

   // Only interested in audio type tracks
   if (p) {
      TrackListIterator iter( p->GetTracks() );
      for (Track *t = iter.First(); t; t = iter.Next()) {
         if (t->GetKind() == Track::Wave
#if defined(USE_MIDI)
         || t->GetKind() == Track::Note
#endif
         ) {
            tracks = true;
            break;
         }
      }
   }

   const bool enablePlay = (!recording) || (tracks && !busy);
   mPlay->SetEnabled(enablePlay);
}

void SoundModuleToolBar::OnKeyEvent(wxKeyEvent & event)
{
   if (event.ControlDown() || event.AltDown()) {
      event.Skip();
      return;
   }

   if (event.GetKeyCode() == WXK_SPACE) {
      return;
   }
   event.Skip();
}

void SoundModuleToolBar::OnSoundModule(wxCommandEvent & WXUNUSED(evt))
{
   AudacityProject *project = GetActiveProject();
   if( project == NULL )
   {
      wxMessageBox( wxT("No project to process!") );
      goto END;
   }
   TrackList * tracks = project->GetTracks();
   if( tracks == NULL )
   {
      wxMessageBox( wxT("No tracks to process!") );
      goto END;
   }

   double endTime = tracks->GetEndTime();
   if (endTime <= 0.0f)
   {
      wxMessageBox( wxT("No sounds to process!") );
      goto END;
   }

   {
   Exporter exporter;
   exporter.Process(project, 1, wxT("SoundModule"), wxT(""), false, 0.0, endTime);
   }

END:
   mPlay->PopUp();
}

void SoundModuleToolBar::SetupCutPreviewTracks(double WXUNUSED(playStart), double cutStart,
                                           double cutEnd, double  WXUNUSED(playEnd))
{
   ClearCutPreviewTracks();
   AudacityProject *p = GetActiveProject();
   if (p) {
      // Find first selected track (stereo or mono) and duplicate it
      Track *track1 = NULL, *track2 = NULL;
      TrackListIterator it(p->GetTracks());
      for (Track *t = it.First(); t; t = it.Next())
      {
         if (t->GetKind() == Track::Wave && t->GetSelected())
         {
            track1 = t;
            track2 = t->GetLink();
            break;
         }
      }

      if (track1)
      {
         // Duplicate and change tracks
         track1 = track1->Duplicate();
         track1->Clear(cutStart, cutEnd);
         if (track2)
         {
            track2 = track2->Duplicate();
            track2->Clear(cutStart, cutEnd);
         }

         mCutPreviewTracks = new TrackList();
         mCutPreviewTracks->Add(track1);
         if (track2)
            mCutPreviewTracks->Add(track2);
      }
   }
}

void SoundModuleToolBar::ClearCutPreviewTracks()
{
   if (mCutPreviewTracks)
   {
      mCutPreviewTracks->Clear(true); /* delete track contents too */
      delete mCutPreviewTracks;
      mCutPreviewTracks = NULL;
   }
}

