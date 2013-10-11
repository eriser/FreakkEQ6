#define PLUG_MFR "Freakk"
#define PLUG_NAME "FreakkWah"

#define PLUG_CLASS_NAME FreakkWah

#define BUNDLE_MFR "Freakk"
#define BUNDLE_NAME "FreakkWah"

#define PLUG_ENTRY FreakkWah_Entry
#define PLUG_VIEW_ENTRY FreakkWah_ViewEntry

#define PLUG_ENTRY_STR "FreakkWah_Entry"
#define PLUG_VIEW_ENTRY_STR "FreakkWah_ViewEntry"

#define VIEW_CLASS FreakkWah_View
#define VIEW_CLASS_STR "FreakkWah_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'FW01'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'KF01'

// ProTools stuff

#if (defined(AAX_API) || defined(RTAS_API)) && !defined(_PIDS_)
  #define _PIDS_
  const int PLUG_TYPE_IDS[2] = {'EFN1', 'EFN2'};
  const int PLUG_TYPE_IDS_AS[2] = {'EFA1', 'EFA2'}; // AudioSuite
#endif

#define PLUG_MFR_PT "Freakk\nFreakk\nAcme"
#define PLUG_NAME_PT "FreakkWah\nIPEF"
#define PLUG_TYPE_PT "Effect"
#define PLUG_DOES_AUDIOSUITE 1

/* PLUG_TYPE_PT can be "None", "EQ", "Dynamics", "PitchShift", "Reverb", "Delay", "Modulation", 
"Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" 
instrument determined by PLUG _IS _INST
*/

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0

#define PLUG_DOES_STATE_CHUNKS 0

//#define KNOB_ID 101
#define BG_ID                   100
#define KNOB_THRESH_ID  101
#define KNOB_Q_ID  102
#define KNOB_FREQ_ID  103

// Image resource locations for this plug.
//#define KNOB_FN "resources/img/knob.png"
#define BG_FN	"resources/img/FreakkWah_250.png"
#define KNOB_THRESH_FN  "resources/img/KnobIndicator_250_3.png"
#define KNOB_Q_FN  "resources/img/KnobIndicator_250_3.png"
#define KNOB_FREQ_FN  "resources/img/KnobIndicator_250_3.png"

// GUI default dimensions
#define GUI_WIDTH 250
#define GUI_HEIGHT 450


// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#if defined(SA_API) && !defined(OS_IOS)
#include "app_wrapper/app_resource.h"
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 "Fx"

/* "Fx|Analyzer"", "Fx|Delay", "Fx|Distortion", "Fx|Dynamics", "Fx|EQ", "Fx|Filter",
"Fx", "Fx|Instrument", "Fx|InstrumentExternal", "Fx|Spatial", "Fx|Generator",
"Fx|Mastering", "Fx|Modulation", "Fx|PitchShift", "Fx|Restoration", "Fx|Reverb",
"Fx|Surround", "Fx|Tools", "Instrument", "Instrument|Drum", "Instrument|Sampler",
"Instrument|Synth", "Instrument|Synth|Sampler", "Instrument|External", "Spatial",
"Spatial|Fx", "OnlyRT", "OnlyOfflineProcess", "Mono", "Stereo",
"Surround"
*/
