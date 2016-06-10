/**********************************************************************

  SoundModule: A Digital Audio Editor

  ExportSoundModule.cpp

**********************************************************************/

#include "../Audacity.h"
#include "../Project.h"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/log.h>
#include <wx/process.h>
#include <wx/textctrl.h>
#include <FileDialog.h>
#include "Export.h"
#include "ExportSoundModule.h"

#include "../Mix.h"
#include "../Prefs.h"
#include "../Internat.h"
#include "../float_cast.h"
#include "../widgets/FileHistory.h"

//
#include "../WaveTrack.h"
#include "../effects/EffectManager.h"
#include "../toolbars/SelectionBar.h"
#include "../SoundModule/FlashInfo.h"
#include "../SoundModule/USBManager.h"

/****************************************************************************/
/*                                                                          */
/*  クラス名：  メインクラス                                                */
/*                                                                          */
/****************************************************************************/
class PipoApp
{
    USBManager  g_UsbManager;
    BYTE        g_bWriteData[65];//     = {0};

public:
    PipoApp();
    ~PipoApp();

    BYTE GetUSBStatus()
    { return g_UsbManager.GetStatus(); }
    
    int WriteData(ProgressDialog *);
public:
    DWORD               dwDataSize;
    BYTE                pWriteData[FLASH_DATA_SIZE];
};
//

//----------------------------------------------------------------------------
// ExportSoundModuleOptions
//----------------------------------------------------------------------------

class ExportSoundModuleOptions : public wxDialog
{
public:

   ExportSoundModuleOptions(wxWindow *parent);
   void PopulateOrExchange(ShuttleGui & S);
   void OnOK(wxCommandEvent & event);

private:
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ExportSoundModuleOptions, wxDialog)
   EVT_BUTTON(wxID_OK, ExportSoundModuleOptions::OnOK)
END_EVENT_TABLE()

///
///
ExportSoundModuleOptions::ExportSoundModuleOptions(wxWindow *parent)
:  wxDialog(parent, wxID_ANY,
            wxString(_("Specify SoundModule Options")))
{
   ShuttleGui S(this, eIsCreatingFromPrefs);

   PopulateOrExchange(S);
}

///
///
void ExportSoundModuleOptions::PopulateOrExchange(ShuttleGui & S)
{
   S.StartHorizontalLay(wxEXPAND, 0);
   {
      S.StartStatic(_("SoundModule Export Setup"), true);
      {
      }
      S.EndStatic();
   }
   S.EndHorizontalLay();

   S.AddStandardButtons();

   Layout();
   Fit();
   SetMinSize(GetSize());
   Center();

   return;
}

///
///
void ExportSoundModuleOptions::OnOK(wxCommandEvent& WXUNUSED(event))
{
   ShuttleGui S(this, eIsSavingToPrefs);

   PopulateOrExchange(S);

   EndModal(wxID_OK);

   return;
}


class ExportSoundModule : public ExportPlugin
{
public:

   ExportSoundModule();
   void Destroy();

   // Required

   bool DisplayOptions(wxWindow *parent, int format = 0);
   int Export(AudacityProject *project,
               int channels,
               wxString fName,
               bool selectedOnly,
               double t0,
               double t1,
               MixerSpec *mixerSpec = NULL,
               Tags *metadata = NULL,
               int subformat = 0);
};

ExportSoundModule::ExportSoundModule()
:  ExportPlugin()
{
   AddFormat();
   SetFormat(wxT("SOUNDMODULE"),0);
   AddExtension(wxT(""),0);
   SetMaxChannels(255,0);
   SetCanMetaData(false,0);
   SetDescription(_("(SoundModule)"),0);
}

void ExportSoundModule::Destroy()
{
   delete this;
}


int ExportSoundModule::Export(AudacityProject *project,
                      int WXUNUSED(channels),
                      wxString fName,
                      bool selectionOnly,
                      double WXUNUSED(t0),
                      double WXUNUSED(t1),
                      MixerSpec *mixerSpec,
                      Tags *WXUNUSED(metadata),
                      int WXUNUSED(subformat))
{
    int updateResult = eProgressSuccess;
    //
    {
        project->AS_SetRate(SOUNDMODULE_RATE);
        project->GetSelectionBar()->SetRate(SOUNDMODULE_RATE);
    }

// stereo to mono
    {
        project->OnSelectAll();
        project->OnStereoToMono(0);
    }

// resample
    {
        TrackListIterator iter(project->GetTracks());
        int newRate = SOUNDMODULE_RATE;
        int ndx = 0;
        for (Track *t = iter.First(); t; t = iter.Next())
        {
            wxString msg;

            msg.Printf(_("Resampling track %d"), ++ndx);

            ProgressDialog progress(_("Resample"), msg);

            if (t->GetKind() == Track::Wave)
            {
                if (!((WaveTrack*)t)->Resample(newRate, &progress))
                {
                    wxMessageBox(wxT("Resample Failed!"));
                    return eProgressFailed;
                }
            }
        }
    }

// normalize
    {
        const wxString command = wxT("Normalize");
        const wxString params = wxT(" ApplyGain=yes RemoveDcOffset=yes Level=-1.000000 StereoIndependent=no");
        const PluginID & ID = EffectManager::Get().GetEffectByIdentifier(command);
        if (!ID.empty())
        {

            project->OnSelectAll();
            project->OnEffect(ALL_EFFECTS | CONFIGURED_EFFECT, ID, params, false);
        }
        else
        {
            wxMessageBox(
                wxString::Format(
                _("Your batch command of %s was not recognized."), command.c_str()));

            return eProgressFailed;
        }
    }

//// [TruncateSilence]
//    {}

// Align Together
    {
        project->OnSelectAll();
        project->OnAlignNoSync(1/* Align Together */);
    }

// Start to Zero
    {
        project->OnSelectAll();
        project->OnAlign(0/* Start to Zero */);
    }

// Align End to End
    {
        project->OnSelectAll();
        project->OnAlignNoSync(0/* Align End to End */);
    }

// Select All
    {
        project->OnSelectAll();
    }

    // Need to reset
    project->FinishAutoScroll();

    //
    {
        double t0x[HEADER_COUNT_MAX];
        double t1x[HEADER_COUNT_MAX];
        bool tooMany = false;
        bool tooLong = false;
        int nTotal = 0;
        int const nMax = 15; // HEADER_COUNT_MAX;
        double tTotal = 0.0;
        double const tMax = (double)SOUNDMODULE_SAMPLE_COUNT_MAX / (double)SOUNDMODULE_RATE;
        TrackListIterator iter(project->GetTracks());
        Track *t = iter.First();
        while (t) {
            if (t->GetSelected()) {
                if (nTotal >= nMax) {
                    tooMany = true;
                    break;
                }

                t0x[nTotal] = ((WaveTrack*)t)->GetStartTime();
                t1x[nTotal] = ((WaveTrack*)t)->GetEndTime();
                double tTime = t1x[nTotal] - t0x[nTotal];
                if ((tTotal + tTime) > tMax) {
                    tooLong = true;
                    break;
                }

                ++nTotal;
                tTotal += tTime;
            }
            t = iter.Next();
        }
        while (t) {
            if (t->GetSelected()) {
                t->SetSelected(false);
            }
            t = iter.Next();
        }
        //if (tooMany) {
        //          wxMessageBox(wxT("Sound Track is too Many!"));
        //          return eProgressFailed;
        //}
        if (tooLong) {
            wxMessageBox(wxT("Sound Track is too Long!"));
            return eProgressFailed;
        }
        if (nTotal == 0) {
            wxMessageBox(wxT("No sounds to process!"));
            return eProgressFailed;
        }
        if (t1x[nTotal - 1] - t0x[0] <= 0) {
            wxMessageBox(wxT("No sounds to process!"));
            return eProgressFailed;
        }

        {
            PipoApp pipo;
			memset(&pipo.pWriteData[HEADER_START_ADDRESS], 0xAB, HEADER_PAGE_SIZE);
			memset(&pipo.pWriteData[SOUNDMODULE_SAMPLE_START_ADDRESS], 128, SOUNDMODULE_SAMPLE_COUNT_MAX);
			{
			if (nTotal <= HEADER_COUNT_MAX_EX_V1) {
				FlashDataHeader         *header = (FlashDataHeader*)&pipo.pWriteData[HEADER_START_ADDRESS];
				for (int n = 0; n < nTotal; ++n) {
					DWORD dwStartAddress = (DWORD)floor(SOUNDMODULE_SAMPLE_START_ADDRESS + t0x[n] * SOUNDMODULE_RATE + 0.5);
					DWORD dwDataSize = (DWORD)floor((t1x[n] - t0x[n]) * SOUNDMODULE_RATE + 0.5);
					header[n].dwHeaderStartID = (n == 0) ? HEADER_ID : HEADER_ID_2;
					header[n].bHeaderSize = (BYTE)sizeof(FlashDataHeader);
					header[n].dwDataSize = dwDataSize;
					header[n].dwDataTotalPage = (dwDataSize + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
					header[n].bRestSize = (BYTE)(dwDataSize % FLASH_PAGE_SIZE);
					header[n].bStartSector = (dwStartAddress >> 16) & 255;
					header[n].bStartPage = (dwStartAddress >> 8) & 255;
					header[n].bStartByte = dwStartAddress & 255;
					header[n].dwHeaderEndID = HEADER_ID;
				}
				((FlashDataHeaderInfo*)&pipo.pWriteData[HEADER_INFO_ADDRESS])->bHeaderCount = nTotal;
				pipo.dwDataSize = SOUNDMODULE_SAMPLE_START_ADDRESS;
			}
			else {
				DWORD dwStartAddressExV2 = (DWORD)HEADER_START_ADDRESS + nTotal * sizeof(FlashDataHeader) + sizeof(FlashDataHeaderInfo);
				FlashDataHeader         *header = (FlashDataHeader*)&pipo.pWriteData[HEADER_START_ADDRESS];
				for (int n = 0; n < HEADER_COUNT_MAX_EX_V1; ++n) {
					DWORD dwStartAddress = dwStartAddressExV2 + (DWORD)floor(t0x[n] * SOUNDMODULE_RATE + 0.5);
					DWORD dwDataSize = (DWORD)floor((t1x[n] - t0x[n]) * SOUNDMODULE_RATE + 0.5);
					header[n].dwHeaderStartID = (n == 0) ? HEADER_ID : HEADER_ID_2;
					header[n].bHeaderSize = (BYTE)sizeof(FlashDataHeader);
					header[n].dwDataSize = dwDataSize;
					header[n].dwDataTotalPage = (dwDataSize + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
					header[n].bRestSize = (BYTE)(dwDataSize % FLASH_PAGE_SIZE);
					header[n].bStartSector = (dwStartAddress >> 16) & 255;
					header[n].bStartPage = (dwStartAddress >> 8) & 255;
					header[n].bStartByte = dwStartAddress & 255;
					header[n].dwHeaderEndID = HEADER_ID;
				}
				((FlashDataHeaderInfo*)&pipo.pWriteData[HEADER_INFO_ADDRESS])->bHeaderCount = HEADER_COUNT_MAX_EX_V1;
				((FlashDataHeaderInfo*)&pipo.pWriteData[HEADER_INFO_ADDRESS])->bEx = HEADER_EX_V2;
				((FlashDataHeaderInfo*)&pipo.pWriteData[HEADER_INFO_ADDRESS])->wHeaderCount = nTotal;
				header = (FlashDataHeader*)(&pipo.pWriteData[HEADER_START_ADDRESS] + sizeof(FlashDataHeaderInfo));
				for (int n = HEADER_COUNT_MAX_EX_V1; n < nTotal; ++n) {
					DWORD dwStartAddress = dwStartAddressExV2 + (DWORD)floor(t0x[n] * SOUNDMODULE_RATE + 0.5);
					DWORD dwDataSize = (DWORD)floor((t1x[n] - t0x[n]) * SOUNDMODULE_RATE + 0.5);
					header[n].dwHeaderStartID = HEADER_ID_2;
					header[n].bHeaderSize = (BYTE)sizeof(FlashDataHeader);
					header[n].dwDataSize = dwDataSize;
					header[n].dwDataTotalPage = (dwDataSize + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
					header[n].bRestSize = (BYTE)(dwDataSize % FLASH_PAGE_SIZE);
					header[n].bStartSector = (dwStartAddress >> 16) & 255;
					header[n].bStartPage = (dwStartAddress >> 8) & 255;
					header[n].bStartByte = dwStartAddress & 255;
					header[n].dwHeaderEndID = HEADER_ID;
				}
				pipo.dwDataSize = dwStartAddressExV2;
			}
            }
            //
            {
            double t0 = t0x[0];
			double t1 = t1x[nTotal - 1];
   // Turn off logging to prevent broken pipe messages
   wxLogNull nolog;
   sampleCount maxBlockLen = SOUNDMODULE_RATE * 1 /* 44100 * 5 */;
            int numWaveTracks;
   WaveTrack **waveTracks;
   TrackList *tracks = project->GetTracks();
   tracks->GetWaveTracks(selectionOnly, &numWaveTracks, &waveTracks);
   Mixer *mixer = CreateMixer(numWaveTracks,
                            waveTracks,
                            tracks->GetTimeTrack(),
                            t0,
                            t1,
                            1 /* channels */,
                            maxBlockLen,
                            true,
                            SOUNDMODULE_RATE /* rate */,
                            int8Sample,
                            true,
                            mixerSpec);

   size_t numBytes = 0;
   samplePtr mixed = NULL;

   // Prepare the progress display
   ProgressDialog *progress = new ProgressDialog(
      _("Exporting the audio using USB"));

   // Start piping the mixed data to the command
   while (updateResult == eProgressSuccess ) {
         sampleCount numSamples = mixer->Process(maxBlockLen);
         if (numSamples == 0) {
            break;
         }

         mixed = mixer->GetBuffer();
         numBytes = numSamples * SAMPLE_SIZE(int8Sample);
         if(numBytes > sizeof(pipo.pWriteData) - pipo.dwDataSize) {
             numBytes = sizeof(pipo.pWriteData) - pipo.dwDataSize;
         }
         while(numBytes) {
             pipo.pWriteData[pipo.dwDataSize] = *(BYTE*)mixed + 128;
             --numBytes;
             ++pipo.dwDataSize;
             ++mixed;
         }
         if (sizeof(pipo.pWriteData) == pipo.dwDataSize) {
            break;
         }
      updateResult = progress->Update(mixer->MixGetCurrentTime()-t0, t1-t0);
   }
   if (updateResult == eProgressSuccess)
   {
       // HEADER_IDは最初のUSBブロックだけ
       // もし最初以外のブロックにHEADER_IDがあったらデータを編集する // fake
       DWORD dwRoop = (DWORD)(pipo.dwDataSize / USB_BUFFER_SIZE);
       if ((pipo.dwDataSize % USB_BUFFER_SIZE) != 0)
       {
           dwRoop++;
       }

       for (DWORD i = 1; i < dwRoop; i++)
       {
           FlashDataHeader *header = (FlashDataHeader*)(pipo.pWriteData + (i * USB_BUFFER_SIZE));
           if (header->dwHeaderStartID == HEADER_ID &&
               header->bHeaderSize == (BYTE)sizeof(FlashDataHeader) &&
               header->dwHeaderEndID == HEADER_ID)
           {
               header->dwHeaderStartID = HEADER_ID_NOT_HEADER;
           }
       }

   }
   if (updateResult == eProgressSuccess)
   {
       updateResult = pipo.WriteData(progress);
       if (updateResult != eProgressSuccess)
       {
           switch (pipo.GetUSBStatus())
           {
           case UMS_ERR_INIT:
               wxMessageBox(wxT("Init USB Failed!"));
               break;
           case UMS_ERR_OPEN:
               wxMessageBox(wxT("Open USB Failed!"));
               break;
           default:
			   switch (updateResult)
			   {
			   case eProgressSuccess:
				   break;
			   case eProgressCancelled:
				   wxMessageBox(wxT("SoundModule(USB) WriteData Cancelled!"));
				   break;
			   case eProgressStopped:
				   wxMessageBox(wxT("SoundModule(USB) WriteData Stopped!"));
				   break;
			   case eProgressFailed:
			   default:
				   wxMessageBox(wxT("SoundModule(USB) WriteData Failed!"));
				   break;
			   }
           }
           //return eProgressFailed;
       }
   }

   delete progress;
   delete mixer;
   delete[] waveTracks;
            }
        }
    }

    return updateResult;
}

bool ExportSoundModule::DisplayOptions(wxWindow *parent, int WXUNUSED(format))
{
   ExportSoundModuleOptions od(parent);

   od.ShowModal();

   return true;
}

ExportPlugin *New_ExportSoundModule()
{
   return new ExportSoundModule();
}

//

/****************************************************************************/
/*                                                                          */
/*  関数名  ：  PipoApp                                                         */
/*                                                                          */
/*  説明    ：  コンストラクタ                                              */
/*                                                                          */
/****************************************************************************/
PipoApp::PipoApp(): dwDataSize(0)
{
    //m_hInstance           = NULL;
    //m_hWnd                = NULL;
    
    return;
}


/****************************************************************************/
/*                                                                          */
/*  関数名  ：  ~PipoApp                                                    */
/*                                                                          */
/*  説明    ：  デストラクタ                                                */
/*                                                                          */
/****************************************************************************/
PipoApp::~PipoApp()
{
    return;
}


int PipoApp::WriteData(ProgressDialog *progress)
{
    int updateResult = eProgressSuccess;
    BOOL                bRet                    = TRUE;
    DWORD               dwRoop                  = 0;
    DWORD               i/* ,t */;
    DWORD               dataSize = dwDataSize;

    bRet = g_UsbManager.InitUSBManager();
    if( bRet == FALSE )
    {
        updateResult = eProgressFailed;
        goto END;
    }
    
    //データの書き込み ------------------------------------------------------->
    dwRoop = (DWORD)(dataSize / USB_BUFFER_SIZE);
    if( (dataSize % USB_BUFFER_SIZE) != 0 )
    {
        dwRoop++;
    }
    
	for (i = 0; updateResult == eProgressSuccess && i < dwRoop; i++)
    {
        if( dataSize < USB_BUFFER_SIZE )
        {
            bRet = g_UsbManager.WriteDataEx( (void*)(pWriteData + (i * USB_BUFFER_SIZE)), dataSize );
        }
        else
        {
            bRet = g_UsbManager.WriteDataEx( (void*)(pWriteData + (i * USB_BUFFER_SIZE)), USB_BUFFER_SIZE );
        }
        if (bRet == FALSE)
        {
            updateResult = eProgressFailed;
            goto END;
        }
        
        dataSize    = dataSize - USB_BUFFER_SIZE;
        updateResult = progress->Update((int)i, (int)dwRoop);
    }
    if (updateResult == eProgressSuccess)
        updateResult = progress->Update((int)dwRoop, (int)dwRoop);
    //------------------------------------------------------------------------<
    
END:
    return updateResult;
}
