/*
 *  ======== dcbso.c ========
 *
 * dcbso -d <file duration in edit units>  -d <numEditUnits>
 *                                         -f <edit unit rate = 24|48> 
 *                                         -i <initial edit unit count> 
 *                                         -r <sample rate = [48000|96000]> 
 *                                         -p <PlayoutId> <output file path> 
*/

/* 
** dcbso outputs a mono WAV file containing 
** <file duration in  edit units>*<edit unit rate> 24-bit samples, 
** with the sync signal starting at sample 0 and containing 
** <file duration in edit units> number of sync frames 
** at <edit unit rate> sync frames per second. 
**
** The "edit unit count" field of the sync signal increments by one 
** at every sync frame, 
** and is equal to <initial edit unit count> in the first sync frame. 
**
** The "Playout Id" field of the sync signal is equal to <PlayoutId>.
*/

// Note: for MSVC++ 2005 and higher, define the following:  _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
// Note: for MSVC++ 2005 and higher, define the following:  POSIX
#define POSIX
 
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
// #include <io.h>
#include "sync.h"


#define FALSE 0
#define TRUE 1

typedef int Int32;

#define USAGE "-d <numEditUnits>  -f <editUnitRate i.e [24|48]> -i <initialEditUnit> -r <sampleRate i.e. [48000|96000]> -p <PlayoutId> <outputFile>"

static char pn[BUFSIZ];        /* program name */

/* Windows WAVE File Encoding Tags */
#define WAVE_FORMAT_UNKNOWN                  0x0000 /* Unknown Format */
#define WAVE_FORMAT_PCM                      0x0001 /* PCM */
#define WAVE_FORMAT_ADPCM                    0x0002 /* Microsoft ADPCM Format */
#define WAVE_FORMAT_IEEE_FLOAT               0x0003 /* IEEE Float */
#define WAVE_FORMAT_VSELP                    0x0004 /* Compaq Computer's VSELP */
#define WAVE_FORMAT_IBM_CSVD                 0x0005 /* IBM CVSD */
#define WAVE_FORMAT_ALAW                     0x0006 /* ALAW */
#define WAVE_FORMAT_MULAW                    0x0007 /* MULAW */
#define WAVE_FORMAT_OKI_ADPCM                0x0010 /* OKI ADPCM */
#define WAVE_FORMAT_DVI_ADPCM                0x0011 /* Intel's DVI ADPCM */
#define WAVE_FORMAT_MEDIASPACE_ADPCM         0x0012 /*Videologic's MediaSpace ADPCM*/
#define WAVE_FORMAT_SIERRA_ADPCM             0x0013 /* Sierra ADPCM */
#define WAVE_FORMAT_G723_ADPCM               0x0014 /* G.723 ADPCM */
#define WAVE_FORMAT_DIGISTD                  0x0015 /* DSP Solution's DIGISTD */
#define WAVE_FORMAT_DIGIFIX                  0x0016 /* DSP Solution's DIGIFIX */
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM       0x0017 /* Dialogic OKI ADPCM */
#define WAVE_FORMAT_MEDIAVISION_ADPCM        0x0018 /* MediaVision ADPCM */
#define WAVE_FORMAT_CU_CODEC                 0x0019 /* HP CU */
#define WAVE_FORMAT_YAMAHA_ADPCM             0x0020 /* Yamaha ADPCM */
#define WAVE_FORMAT_SONARC                   0x0021 /* Speech Compression's Sonarc */
#define WAVE_FORMAT_TRUESPEECH               0x0022 /* DSP Group's True Speech */
#define WAVE_FORMAT_ECHOSC1                  0x0023 /* Echo Speech's EchoSC1 */
#define WAVE_FORMAT_AUDIOFILE_AF36           0x0024 /* Audiofile AF36 */
#define WAVE_FORMAT_APTX                     0x0025 /* APTX */
#define WAVE_FORMAT_AUDIOFILE_AF10           0x0026 /* AudioFile AF10 */
#define WAVE_FORMAT_PROSODY_1612             0x0027 /* Prosody 1612 */
#define WAVE_FORMAT_LRC                      0x0028 /* LRC */
#define WAVE_FORMAT_AC2                      0x0030 /* Dolby AC2 */
#define WAVE_FORMAT_GSM610                   0x0031 /* GSM610 */
#define WAVE_FORMAT_MSNAUDIO                 0x0032 /* MSNAudio */
#define WAVE_FORMAT_ANTEX_ADPCME             0x0033 /* Antex ADPCME */
#define WAVE_FORMAT_CONTROL_RES_VQLPC        0x0034 /* Control Res VQLPC */
#define WAVE_FORMAT_DIGIREAL                 0x0035 /* Digireal */
#define WAVE_FORMAT_DIGIADPCM                0x0036 /* DigiADPCM */
#define WAVE_FORMAT_CONTROL_RES_CR10         0x0037 /* Control Res CR10 */
#define WAVE_FORMAT_VBXADPCM                 0x0038 /* NMS VBXADPCM */
#define WAVE_FORMAT_ROLAND_RDAC              0x0039 /* Roland RDAC */
#define WAVE_FORMAT_ECHOSC3                  0x003A /* EchoSC3 */
#define WAVE_FORMAT_ROCKWELL_ADPCM           0x003B /* Rockwell ADPCM */
#define WAVE_FORMAT_ROCKWELL_DIGITALK        0x003C /* Rockwell Digit LK */
#define WAVE_FORMAT_XEBEC                    0x003D /* Xebec */
#define WAVE_FORMAT_G721_ADPCM               0x0040 /* Antex Electronics G.721 */
#define WAVE_FORMAT_G728_CELP                0x0041 /* G.728 CELP */
#define WAVE_FORMAT_MSG723                   0x0042 /* MSG723 */
#define WAVE_FORMAT_MPEG                     0x0050 /* MPEG Layer 1,2 */
#define WAVE_FORMAT_RT24                     0x0051 /* RT24 */
#define WAVE_FORMAT_PAC                      0x0051 /* PAC */
#define WAVE_FORMAT_MPEGLAYER3               0x0055 /* MPEG Layer 3 */
#define WAVE_FORMAT_CIRRUS                   0x0059 /* Cirrus */
#define WAVE_FORMAT_ESPCM                    0x0061 /* ESPCM */
#define WAVE_FORMAT_VOXWARE                  0x0062 /* Voxware (obsolete) */
#define WAVE_FORMAT_CANOPUS_ATRAC            0x0063 /* Canopus Atrac */
#define WAVE_FORMAT_G726_ADPCM               0x0064 /* G.726 ADPCM */
#define WAVE_FORMAT_G722_ADPCM               0x0065 /* G.722 ADPCM */
#define WAVE_FORMAT_DSAT                     0x0066 /* DSAT */
#define WAVE_FORMAT_DSAT_DISPLAY             0x0067 /* DSAT Display */
#define WAVE_FORMAT_VOXWARE_BYTE_ALIGNED     0x0069 /* Voxware Byte Aligned (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC8              0x0070 /* Voxware AC8 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC10             0x0071 /* Voxware AC10 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC16             0x0072 /* Voxware AC16 (obsolete) */
#define WAVE_FORMAT_VOXWARE_AC20             0x0073 /* Voxware AC20 (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT24             0x0074 /* Voxware MetaVoice (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29             0x0075 /* Voxware MetaSound (obsolete) */
#define WAVE_FORMAT_VOXWARE_RT29HW           0x0076 /* Voxware RT29HW (obsolete) */
#define WAVE_FORMAT_VOXWARE_VR12             0x0077 /* Voxware VR12 (obsolete) */
#define WAVE_FORMAT_VOXWARE_VR18             0x0078 /* Voxware VR18 (obsolete) */
#define WAVE_FORMAT_VOXWARE_TQ40             0x0079 /* Voxware TQ40 (obsolete) */
#define WAVE_FORMAT_SOFTSOUND                0x0080 /* Softsound */
#define WAVE_FORMAT_VOXWARE_TQ60             0x0081 /* Voxware TQ60 (obsolete) */
#define WAVE_FORMAT_MSRT24                   0x0082 /* MSRT24 */
#define WAVE_FORMAT_G729A                    0x0083 /* G.729A */
#define WAVE_FORMAT_MVI_MV12                 0x0084 /* MVI MV12 */
#define WAVE_FORMAT_DF_G726                  0x0085 /* DF G.726 */
#define WAVE_FORMAT_DF_GSM610                0x0086 /* DF GSM610 */
#define WAVE_FORMAT_ISIAUDIO                 0x0088 /* ISIAudio */
#define WAVE_FORMAT_ONLIVE                   0x0089 /* Onlive */
#define WAVE_FORMAT_SBC24                    0x0091 /* SBC24 */
#define WAVE_FORMAT_DOLBY_AC3_SPDIF          0x0092 /* Dolby AC3 SPDIF */
#define WAVE_FORMAT_ZYXEL_ADPCM              0x0097 /* ZyXEL ADPCM */
#define WAVE_FORMAT_PHILIPS_LPCBB            0x0098 /* Philips LPCBB */
#define WAVE_FORMAT_PACKED                   0x0099 /* Packed */
#define WAVE_FORMAT_RHETOREX_ADPCM           0x0100 /* Rhetorex ADPCM */
#define WAVE_FORMAT_IRAT                     0x0101 /* BeCubed Software's IRAT */
#define WAVE_FORMAT_VIVO_G723                0x0111 /* Vivo G.723 */
#define WAVE_FORMAT_VIVO_SIREN               0x0112 /* Vivo Siren */
#define WAVE_FORMAT_DIGITAL_G723             0x0123 /* Digital G.723 */
#define WAVE_FORMAT_CREATIVE_ADPCM           0x0200 /* Creative ADPCM */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH8     0x0202 /* Creative FastSpeech8 */
#define WAVE_FORMAT_CREATIVE_FASTSPEECH10    0x0203 /* Creative FastSpeech10 */
#define WAVE_FORMAT_QUARTERDECK              0x0220 /* Quarterdeck */
#define WAVE_FORMAT_FM_TOWNS_SND             0x0300 /* FM Towns Snd */
#define WAVE_FORMAT_BTV_DIGITAL              0x0400 /* BTV Digital */
#define WAVE_FORMAT_VME_VMPCM                0x0680 /* VME VMPCM */
#define WAVE_FORMAT_OLIGSM                   0x1000 /* OLIGSM */
#define WAVE_FORMAT_OLIADPCM                 0x1001 /* OLIADPCM */
#define WAVE_FORMAT_OLICELP                  0x1002 /* OLICELP */
#define WAVE_FORMAT_OLISBC                   0x1003 /* OLISBC */
#define WAVE_FORMAT_OLIOPR                   0x1004 /* OLIOPR */
#define WAVE_FORMAT_LH_CODEC                 0x1100 /* LH Codec */
#define WAVE_FORMAT_NORRIS                   0x1400 /* Norris */
#define WAVE_FORMAT_ISIAUDIO2                0x1401 /* ISIAudio */
#define WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS  0x1500 /* Soundspace Music Compression */
#define WAVE_FORMAT_DVM                      0x2000 /* DVM */
#define WAVE_FORMAT_EXTENSIBLE               0xFFFE /* SubFormat */
#define WAVE_FORMAT_DEVELOPMENT              0xFFFF /* Development */


/*
** Note: PCM Wave files are only supported for files under 4 GB.
**       Larger files have to be in Broadcast Wave format.
*/

typedef struct WaveHeader {
    char  chunkId[4];     // "RIFF"
    Int32 chunkSize;      // PCM fileSize + 36 (= subChunk2Size+36) (Note: 36 = (sizeof(WaveHeader) - offsetof(WaveHeader.format)))
    char  format[4];      // "WAVE"
    char  subChunk1ID[4]; // "fmt "
    Int32 subChunk1Size;  // 16  (sizeof struct WaveHeader.audioFormat thru WaveHeader.bitsPerSample)
    short audioFormat;    // 1  => linear PCM file
    short numChannels;    // 1 => mono, 2 => stereo
    Int32 sampleRate;     // e.g. 8000 or 44100
    Int32 byteRate;       // (sampleRate*numChannels*bitsPerSample)/8
    short blockAlign;     // (numChannels*bitsPerSample)/8
    short bitsPerSample;  // e.g. 16, 24
    char  subChunk2ID[4]; // "data"
    Int32 subChunk2Size;  // (numSamples*numChannels*bitsPerSample)/8 (a.k.a. PCM fileSize)
} WaveHeader; // 44 bytes

WaveHeader waveHeader = {
    "RIFF",          // char chunkId[4];      // "RIFF"
    0,               // Int32 chunkSize;      // PCM fileSize + 36 (= subChunk2Size+36) (Note: 36 = (sizeof(WaveHeader) - offsetof(WaveHeader.format)))
    "WAVE",          // char format[4];       // "WAVE"
    "fmt ",          // char subChunk1ID[4];  // "fmt "
    16,              // Int32 subChunk1Size;  // 16  (sizeof struct WaveHeader.audioFormat thru WaveHeader.bitsPerSample)
    WAVE_FORMAT_PCM, // short audioFormat;    // 1  => linear PCM file
    1,               // short numChannels;    // 1 => mono, 2 => stereo
    48000,           // Int32 sampleRate;     // e.g. 8000 or 44100
    ((48000*24)/8),  // Int32 byteRate;       // (sampleRate*numChannels*bitsPerSample)/8
    ((1*24)/8),      // short blockAlign;     // (numChannels*bitsPerSample)/8
    24,              // short bitsPerSample;  // e.g. 16, 24
    "data",          // char subChunk2ID[4];  // "data"
    0                // Int32 subChunk2Size;  // (numSamples*numChannels*bitsPerSample)/8 (a.k.a. PCM fileSize)
};    


/*
**    Description of each edit unit:
**
** Sample Number  Content   Description
**   0            0x000004  Length – 4 more samples in this packet.
**   1            0x000001  Status bits. Content is playing.
**   2            0x123456  Current edit unit number (this increments on each edit unit)
**   3            0x000012  Two msb of PID, right justified, pad with leading zeroes.
**   4            0x345678  16 lsb of PID.
**   5–3999       0xffffff  Pad to end of edit unit (assumes 24 edit units per second at 96KHz)
*/

#define NUM_EDIT_UNIT_HEADER_SAMPLES  5

typedef struct {
    Int32 length;                // **   0  0x000004  length – 4 more samples in this packet.
    Int32 systemStatus;          // **   1  0x000001  Status bits. Content is playing.
    Int32 currentEditUnit;       // **   2  0x123456  Current edit unit number (this increments on each edit unit)
    Int32 msbPID;                // **   3  0x000012  8 msb of PID, right justified, pad with leading zeroes.
    Int32 lsbPID;                // **   4  0x345678  24 lsb of PID.
} EditUnitHdr;

EditUnitHdr editUnitHdr;

void GetAsciiInteger(char *cp, Int32 *valPtr);

void GetAsciiInteger(char *cp, Int32 *valPtr)
{
    if ((*cp == '0') && (toupper(cp[1]) == 'X'))
    {
        (void)sscanf(&cp[2], "%x", valPtr);
    }
    else {
        (void)sscanf(cp, "%d", valPtr);
    }    
}    

/*
 *  ======== main ========
 */

int main ( int argC,  char *argV[])
{
    int           argI;
    int           digIdx;
    int           i;
    FILE         *fpOut = stdout;
    Int32         sampleRate = 48000;   // 48000 or 96000
    Int32         editUnitRate = 24;   // 24 or 48
    Int32         numEditUnits = 1;   
    Int32         initialEditUnit = 1;
    Int32         playoutID = 0x12345678;
    char         *fileName = 0;
    int           oFlag = FALSE;
    Int32         editUnitSize;
    Int32         numPad;
    Int32         numSamples;     // total number of samples in file
    Int32         numAudioBytes;  // total number of bytes of audio in file (i.e. numSamples * 3)
    Int32         currentEditUnit;
    Int32         editUnitIdx;
    Int32         padVal = 0xffffffff;
    Int32        *samplePtr;
    
    /* Testing */
    
    syncSample sSample;
    
    syncSample *pSyncSample = &sSample;
    
    initSyncSample(pSyncSample);
    
    Uint32 tValue = 0x1;
    
    Uint8 *pValue = (Uint8 *) &tValue;
    
    printf("%d\n", pValue[0]);
    printf("%d\n", pValue[1]);
    printf("%d\n", pValue[2]);
    
    Uint8   buffer[276];
    
    Uint8 count;
    
    // writeUint32(tValue, buffer, &count,TRUE);
    
    for (int i=0;i<12;i++) {
        
        
        printf("%d\n", buffer[i]);
        
    }
    
    UUID uuid;
    
    char *str = "0123-4567-89ab-cdef-0123-4567-89ab-cdef-0123-4567-89ab-cdef-0123-4567-89ab-cdef";
    
    Bool success;
    
    success = str2UUID(str,uuid);
    
    success = setPictureUUID(pSyncSample, str);
    
    Uint16 extension[2] = {1,2};
    
    success = setExtension(pSyncSample, extension, 2);
    
    Uint32 lcount;
    
    success = writeSyncSample(pSyncSample, buffer, &lcount);
    
    deleteSyncSample(pSyncSample);
    
    return 0;
    
    /* initialize program name variable */

    strcpy(pn, argV[0]);

    /* get input arguments */
    if (argC < 1) {
        fprintf(stderr, "%s: Not enough arguments\n", pn);
        fprintf(stderr, "usage: %s %s\n", pn, USAGE);
        exit(1);
    }
    
    for (argI = 1; argI < argC; argI++) {
         if (argV[argI][0] == '-') {
            switch (argV[argI][1]) {
              case 'd':   // number of edit units
              case 'D':   // number of edit units
                if (isdigit(argV[argI][2])) {  // allow number to immediately follow switch (without a space)
                    digIdx = 2;
                }
                else {
                    argI++;
                    digIdx = 0;
                }
                GetAsciiInteger(&argV[argI][digIdx], &numEditUnits);
                if (numEditUnits <= 0) {
                    fprintf(stderr, "%s: Number of edit units must be postive: %s\n", pn, &argV[argI][digIdx]);
                    fprintf(stderr, "usage: %s %s\n", pn, USAGE);
                    exit(1);
                }
                break;
                
              case 'f':   // edit unit rate {24,48}
              case 'F':   // edit unit rate {24,48}
                if (isdigit(argV[argI][2])) {  // allow number to immediately follow switch (without a space)
                    digIdx = 2;
                }
                else {
                    argI++;
                    digIdx = 0;
                }
                GetAsciiInteger(&argV[argI][digIdx], &editUnitRate);
                
                /* only allow all conceivable edit rates */
                if ((editUnitRate !=  24) &&
                    (editUnitRate !=  48) &&
                    (editUnitRate !=  96) &&
                    (editUnitRate !=  30) &&
                    (editUnitRate !=  60) &&
                    (editUnitRate != 120) &&
                    (editUnitRate !=  25) &&
                    (editUnitRate !=  50) &&
                    (editUnitRate != 100))
                {
                    fprintf(stderr, "%s: Illegal edit unit rate: %s\n", pn, &argV[argI][digIdx]);
                    fprintf(stderr, "usage: %s %s\n", pn, USAGE);
                    exit(1);
                }
                break;
                
              case 'i':   // initial edit unit count
              case 'I':   // initial edit unit count
                if (isdigit(argV[argI][2])) {  // allow number to immediately follow switch (without a space)
                    digIdx = 2;
                }
                else {
                    argI++;
                    digIdx = 0;
                }
                GetAsciiInteger(&argV[argI][digIdx], &initialEditUnit);
                if ((initialEditUnit < 0) ||
                    (initialEditUnit > 0xffffff))
                {
                    fprintf(stderr, "%s: Illegal Initial Edit Unit Count: %s (must be less than 0xffffff)\n", pn, &argV[argI][digIdx]);
                    fprintf(stderr, "usage: %s %s\n", pn, USAGE);
                    exit(1);
                }
                break;
                
              case 'r':   // sample rate = [48000|96000] 
              case 'R':   // sample rate = [48000|96000] 
                if (isdigit(argV[argI][2])) {  // allow number to immediately follow switch (without a space)
                    digIdx = 2;
                }
                else {
                    argI++;
                    digIdx = 0;
                }
                GetAsciiInteger(&argV[argI][digIdx], &sampleRate);
                if ((sampleRate != 48000) &&
                    (sampleRate != 96000))
                {
                    fprintf(stderr, "%s: Illegal Sample Rate: %s (must be 48000 or 96000)\n", pn, &argV[argI][digIdx]);
                    fprintf(stderr, "usage: %s %s\n", pn, USAGE);
                    exit(1);
                }
                break;
                
              case 'p':   // PlayoutId 
              case 'P':   // PlayoutId 
                if (isdigit(argV[argI][2])) {  // allow number to immediately follow switch (without a space)
                    digIdx = 2;
                }
                else {
                    argI++;
                    digIdx = 0;
                }
                GetAsciiInteger(&argV[argI][digIdx], &playoutID);
                break;
                
              case 'o':        /* get output file name */
              case 'O':        /* get output file name */
                oFlag = TRUE;
                argI++;
                fileName = argV[argI];
                break;

              case '?':
              default:
                fprintf(stderr, "usage: %s %s\n", pn, USAGE);
                exit(1);

            }
        }            
        else {
            break;
       }        
    }
    
    if (argC - argI > 0) {
        if (oFlag) {
            fprintf(stderr, "%s: can't specify output file name twice: '%s' and '%s'\n", pn, fileName, argV[argI]);
            exit(1);
        }
        oFlag = TRUE;
        fileName = argV[argI];
    }
    
    if (oFlag) {
        if ((fpOut = fopen(fileName, "wb")) == NULL) {
             fprintf(stderr, "%s: can't open %s\n", pn, fileName);
             exit(1);
        }
    }
     
    editUnitSize = (sampleRate / editUnitRate);
    numPad = editUnitSize - NUM_EDIT_UNIT_HEADER_SAMPLES;
    numSamples = editUnitSize * numEditUnits;
    numAudioBytes = numSamples * ((1 * 24) / 8);
    waveHeader.sampleRate = sampleRate;
    waveHeader.byteRate = ((sampleRate * 24) / 8);
    waveHeader.subChunk2Size = numAudioBytes;
    waveHeader.chunkSize = (44 - 8) + numAudioBytes;      // PCM fileSize + 36 (= subChunk2Size+36) (Note: 36 = (sizeof(WaveHeader) - offsetof(WaveHeader.format)))
    
    /* Write out the wave header to the output file */
    fwrite((char *)&waveHeader, 1, sizeof(waveHeader), fpOut);

    /* Initialize the edit unit header */

    currentEditUnit = initialEditUnit;
    editUnitHdr.length = 0x000004;
    editUnitHdr.systemStatus = 0x000001;
    editUnitHdr.currentEditUnit = initialEditUnit;
    editUnitHdr.msbPID = (playoutID >> 24) & 0x000000ff;
    editUnitHdr.lsbPID = playoutID & 0x00ffffff;
    /* 
    ** Write out all of the edit units 
    */
    for (editUnitIdx = 0; editUnitIdx < numEditUnits; editUnitIdx++) {
        /* Update the current edit unit */
        editUnitHdr.currentEditUnit = currentEditUnit;
        
        /* Write out the edit unit header (i.e. NUM_EDIT_UNIT_HEADER_SAMPLES samples) */
        samplePtr = (Int32 *)&editUnitHdr;
        for (i = 0; i < NUM_EDIT_UNIT_HEADER_SAMPLES; i++, samplePtr++) {
           fwrite((char *)samplePtr, 1, 3, fpOut);
        }
        
        /* Write out the padding samples */
        for (i = 0; i < numPad; i++) {
           fwrite((char *)&padVal, 1, 3, fpOut);
        }
        
        /* Increment the current edit unit for the next edit unit */
        currentEditUnit++;
    }
    
    (void)fclose(fpOut);
    exit(0);
    return (0);
}
