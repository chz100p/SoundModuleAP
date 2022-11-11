/**********************************************************************

  SoundModule: A Digital Audio Editor

  ExportSoundModule.h

**********************************************************************/

#ifndef __AUDACITY_ExportSoundModule__
#define __AUDACITY_ExportSoundModule__

// forward declaration of the ExportPlugin class from Export.h
class ExportPlugin;

/** The only part of this class which is publically accessible is the
 * factory method New_ExportSoundModule() which creates a new ExportSoundModule object and
 * returns a pointer to it. The rest of the class declaration is in ExportSoundModule.cpp
 */
ExportPlugin *New_ExportSoundModule();

#endif
