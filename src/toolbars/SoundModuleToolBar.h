/**********************************************************************

  SoundModule: A Digital Audio Editor

  SoundModuleToolBar.h

**********************************************************************/

#ifndef __AUDACITY_SOUNDMODULE_TOOLBAR__
#define __AUDACITY_SOUNDMODULE_TOOLBAR__

#include "ToolBar.h"
#include "../Theme.h"

class wxBoxSizer;
class wxCommandEvent;
class wxDC;
class wxKeyEvent;
class wxTimer;
class wxTimerEvent;
class wxWindow;

class AButton;
class AudacityProject;
class TrackList;
class TimeTrack;

class SoundModuleToolBar:public ToolBar {

 public:

   SoundModuleToolBar();
   virtual ~SoundModuleToolBar();

   void Create(wxWindow *parent);

   void UpdatePrefs();
   virtual void OnKeyEvent(wxKeyEvent & event);

   void OnSoundModule(wxCommandEvent & evt);

   void Populate();
   virtual void Repaint(wxDC *dc);
   virtual void EnableDisableButtons();

   virtual void ReCreateButtons();
   void RegenerateToolsTooltips();

 private:

   AButton *MakeButton(teBmps eEnabledUp, teBmps eEnabledDown, teBmps eDisabled,
      int id,
      bool processdownevents,
      const wxChar *label);

   static
       void MakeAlternateImages(AButton &button, int idx,
       teBmps eEnabledUp,
       teBmps eEnabledDown,
       teBmps eDisabled);

   void ArrangeButtons();
   void SetupCutPreviewTracks(double playStart, double cutStart,
                             double cutEnd, double playEnd);
   void ClearCutPreviewTracks();

   enum
   {
      ID_SOUNDMODULE_BUTTON = 11000,
      BUTTON_COUNT,
   };

   AButton *mPlay;

   static AudacityProject *mBusyProject;

   // Maybe button state values shouldn't be duplicated in this toolbar?
   bool mPaused;         //Play or record is paused or not paused?

   // Activate ergonomic order for transport buttons
   bool mErgonomicTransportButtons;

   wxString mStrLocale; // standard locale abbreviation

   wxBoxSizer *mSizer;

   TrackList* mCutPreviewTracks;

 public:

   DECLARE_CLASS(SoundModuleToolBar);
   DECLARE_EVENT_TABLE();
};

#endif

