
SoundModule: 
  WWW: http://bit-trade-one.co.jp/

Version 2.1.0 

Contents of this README:

1.  Source Code, Libraries and Additional Copyright Information
2.  Compilation Instructions

-------------------------------------------------------------------------------

1.  Source Code, Libraries and Additional Copyright Information

Source code to this program is always available; for more information visit
our web site at:

  http://bit-trade-one.co.jp/

SoundModule is built upon other free libraries; some of these libraries may have
come with SoundModule in the lib-src directory.  Others you are expected to install
first if you want SoundModule to have certain capabilities.  Most of these libraries
are not distributed under the terms of the GPL, but rather some other free,
GPL-compatible license.  Specifically:

  Audacity: GPL

  expat: BSD-like license.
    Provides XML parsing.  Included with SoundModule.

  FFmpeg: GPL or LGPL (according to how you obtain/configure it)
    Provides decoding/encoding of additional formats. Optional separate
    download.

  libid3tag: GPL
    Reads/writes ID3 tags in MP3 files.  Optional
    separate download as part of libmad.

  libflac: Xiph.Org BSD-like licence (the parts we use)
    Decodes and Encodes Free Lossless Audio Codec files. Optional separate
    download.

  libmad: GPL
    Decodes MP3 files.  Optional separate download.

  libnyquist: BSD-like license.
    Functional language for manipulating audio; available
    within SoundModule for effects processing.

  libogg: BSD-like license.
    Optional separate download, along with libvorbis.

  libsndfile: LGPL
    Reads and writes uncompressed PCM audio files.
    Included with SoundModule.

  libsoxr: LGPL
    The SoX Resampler library performs one-dimensional sample-rate conversion.

  libvamp: new-style BSD
    Plug-in interface and support library for audio analysis plug-ins.
    Included with SoundModule.

  libvorbis: BSD-like license.
    Decodes and encodes Ogg Vorbis files.  Optional
    separate download.

  lv2: a merging of the lilv (ISC license), lv2 (LGPL), msinttypes, serd (ISC), 
    sord and sratom libraries to support LV2 plug-ins. 

  portsmf: BSD-like license.
    library for reading and writing midi files. Included with SoundModule

  sbsms: GPL v2
    Pitch and tempo changing library. Included in SoundModule

  SoundTouch: LGPL
    Changes tempo without changing pitch and vice versa.
    Included in audacity

  Twolame: LGPL
    Encodes MPEG I layer 2 audio (used in DVDs and Radio). Optional separate
    download.

  wxWidgets: wxWindows license (based on LGPL)
    Cross-platform GUI library - must be downloaded and
    compiled separately.


For more information, see the documentation inside each library's
source code directory.

--------------------------------------------------------------------------------
Additional copyright information:
--------------------------------------------------------------------------------

Nyquist

Copyright (c) 2000-2002, by Roger B. Dannenberg
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions of source code must retain the copyright notice, the
list of conditions, and the disclaimer, all three of which appear below under
"COPYRIGHT AND LICENSE INFORMATION FOR XLISP."

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Redistributions in binary form must reproduce the copyright notice, the
list of conditions, and the disclaimer, all three of which appear below under
"COPYRIGHT AND LICENSE INFORMATION FOR XLISP," in the documentation
and/or other materials provided with the distribution.

Neither the name of Roger B. Dannenberg, Carnegie Mellon University, nor the
names of any contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

COPYRIGHT AND LICENSE INFORMATION FOR XLISP (part of Nyquist):

Copyright (c) 1984-2002, by David Michael Betz
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

Neither the name of David Michael Betz nor the names of any contributors may be
used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


--------------------------------------------------------------------------------

2. Compilation instructions

To compile on Windows using MSVC++, please follow the instructions in
win\compile.txt in the source code. 
