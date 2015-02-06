//
//  File: dcisg.c
//
//  Author: Ton Kalker, ton.kalker@dts.com
//  Date: 2015.02.03
//
//  Version: 0.1
//
//  This file is part an reference implementation of the
//  (committee) draft version of SMPTE specification on
//  "Aux Data Sync Signal and Transfer Protocol",
//  abbreviated as [ADSSTP]. This code is based on an earlier
//  version written by USL, and referred to as [DCBSO].
//
//  Files: sync.c, sync.h, dcisg.c, dcisg.h
//
//  This file implements a CLI for quick generation of sync signals.
//  It uses the API provided by sync.c.

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "dcisg.h"

/*
 * Function: GetAsciiInteger
 * Reading an integer from a string
 * Derived from [DCBSO]
 */

Bool GetAsciiInteger(char *cp, Sint32 *valPtr)
{
    if ((*cp == '0') && (toupper(cp[1]) == 'X'))
    {
        (void)sscanf(&cp[2], "%x", valPtr);
    }
    else {
        (void)sscanf(cp, "%d", valPtr);
    }
    
    return TRUE;
}

/*
 * Function: initInputs
 * Initialization of syncInput instance
 */

Bool initInputs(syncInput *si) {
    
    si->numFrames = 1;
    si->state = 0;
    si->frameRate = 24;
    si->sampleRate = 48000;
    si->timeIndex = 0;
    si->pictureIndex = 0;
    si->soundIndex = 0;
    si->playoutID = 0x12345678;
    si->outputOffset = 0;
    si->screenOffset = 0;
    si->pictureUUID = "00000000000000000000000000000000";
    si->soundUUID = "00000000000000000000000000000000";
    si->playlistUUID = "00000000000000000000000000000000";
    
    si->programName = "dc-syncgen";
    si->outputFile = NULL;
    si->outputFile = stdout;
    
    return TRUE;
}

/*
 * Function: deleteInputs
 * Cleanup of syncInput instance
 */

Bool deleteInputs(syncInput *si) {
    
    if (si->outputFile != NULL) {
        
        fclose(si->outputFile);
        si->outputFile = NULL;
        
    }
    
    return TRUE;
    
}

/*
 * Function: parseInputs
 * Parsing of input arguments.
 * Derived from [DCBSO]
 */

Bool parseInputs(syncInput *si, int argC,  char *argV[]) {
    
    int digIdx;
    int argI;
    
    char* ProgramName = argV[0];
    
    si->programName = ProgramName;
    
    if (argC < 1) {
        
        SHOWUSAGE;
        
        exit(FALSE);
    }
    
    for (argI = 1; argI < argC; argI++) {
        
        if (argV[argI][0] == '-') {
            
            switch (argV[argI][1]) {
                    
                    // Number of edit units
                    
                case 'd':
                case 'D':
                    
                    if (isdigit(argV[argI][2])) {
                        digIdx = 2;
                    }
                    else {
                        argI++;
                        digIdx = 0;
                    }
                    
                    Sint32 numEditUnits;
                    
                    if (!GetAsciiInteger(&argV[argI][digIdx], &numEditUnits)) {
                        
                        return FALSE;
                        
                    }
                    
                    if (numEditUnits <= 0) {
                        
                        fprintf(stderr, "%s: number of edit units must be postive: %s\n\n", ProgramName, &argV[argI][digIdx]);
                        SHOWUSAGE
                        
                        return FALSE;
                    }
                    
                    si->numFrames = (Uint32) numEditUnits;
                    
                    break;
                    
                    // Frame rate
                    
                case 'f':   // edit unit rate {24,48}
                case 'F':   // edit unit rate {24,48}
                    
                    if (isdigit(argV[argI][2])) {
                        digIdx = 2;
                    }
                    else {
                        argI++;
                        digIdx = 0;
                    }
                    
                    Sint32 editUnitRate;
                    
                    if (!GetAsciiInteger(&argV[argI][digIdx], &editUnitRate)) {
                        
                        return FALSE;
                        
                    };
                    
                    // Allowed edit rates
                    
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
                        fprintf(stderr, "%s: illegal edit unit rate: %s\n\n", ProgramName, &argV[argI][digIdx]);
                        SHOWUSAGE
                        
                        return FALSE;
                    }
                    
                    si->frameRate = (Uint32) editUnitRate;
                    
                    break;
                    
                    // Initial time edit unit
                    
                case 'i':
                case 'I':
                    
                    if (argV[argI][2] == 't') {
                        
                        argI++;
                        digIdx = 0;
                        
                        Sint32 initialTimeEditUnit;
                        
                        if (!GetAsciiInteger(&argV[argI][digIdx], &initialTimeEditUnit)) {
                            
                            return FALSE;
                            
                        };
                        
                        if ((initialTimeEditUnit < 0) || (initialTimeEditUnit > 0xffffff))
                        {
                            fprintf(stderr, "%s: illegal initial iime edit unit index: %s (must be less than 0xffffff)\n\n", ProgramName, &argV[argI][digIdx]);
                            SHOWUSAGE
                            
                            return FALSE;
                        }
                        
                        si->timeIndex = initialTimeEditUnit;
                        
                    }

                    if (argV[argI][2] == 'p') {
                        
                        argI++;
                        digIdx = 0;
                        
                        Sint32 initialPictureEditUnit;
                        
                        if (!GetAsciiInteger(&argV[argI][digIdx], &initialPictureEditUnit)) {
                            
                            return FALSE;
                            
                        };
                        
                        if ((initialPictureEditUnit < 0) || (initialPictureEditUnit > 0xffffff))
                        {
                            fprintf(stderr, "%s: illegal initial picture edit unit index: %s (must be less than 0xffffff)\n\n", ProgramName, &argV[argI][digIdx]);
                            SHOWUSAGE
                            
                            return FALSE;
                        }
                        
                        si->pictureIndex = initialPictureEditUnit;
                        
                    }
                    
                    if (argV[argI][2] == 's') {
                        
                        argI++;
                        digIdx = 0;
                        
                        Sint32 initialSoundEditUnit;
                        
                        if (!GetAsciiInteger(&argV[argI][digIdx], &initialSoundEditUnit)) {
                            
                            return FALSE;
                            
                        };
                        
                        if ((initialSoundEditUnit < 0) || (initialSoundEditUnit > 0xffffff))
                        {
                            fprintf(stderr, "%s: illegal initial sound edit unit index: %s (must be less than 0xffffff)\n\n", ProgramName, &argV[argI][digIdx]);
                            SHOWUSAGE
                            
                            return FALSE;
                        }
                        
                        si->soundIndex = initialSoundEditUnit;
                        
                    }
                    
                    break;
                    
                    // Samplerate
                    
                case 'r':
                case 'R':
                    
                    if (isdigit(argV[argI][2])) {
                        digIdx = 2;
                    }
                    else {
                        argI++;
                        digIdx = 0;
                    }
                    
                    Sint32 sampleRate;
                    
                    if (!GetAsciiInteger(&argV[argI][digIdx], &sampleRate)) {
                        
                        return FALSE;
                        
                    };
                    
                    if ((sampleRate != 48000) &&
                        (sampleRate != 96000))
                    {
                        fprintf(stderr, "%s: illegal sample rate: %s (must be 48000 or 96000)\n\n", ProgramName, &argV[argI][digIdx]);
                        SHOWUSAGE
                        
                        return FALSE;
                    }
                    
                    si->sampleRate = sampleRate;
                    
                    break;
                    
                case 's':
                case 'S':
                    
                    if (isdigit(argV[argI][2])) {
                        digIdx = 2;
                    }
                    else {
                        argI++;
                        digIdx = 0;
                    }
                    
                    Sint32 status;
                    
                    if (!GetAsciiInteger(&argV[argI][digIdx], &status)) {
                        
                        return FALSE;
                        
                    };
                    
                    if ((status < 0) || (status > 2))
                    {
                        fprintf(stderr, "%s: illegal status: %s (must be one of 0, 1 or 2)\n\n", ProgramName, &argV[argI][digIdx]);
                        SHOWUSAGE
                        
                        return FALSE;
                    }
                    
                    si->state = status;
                    
                    break;

                    
                    // Playout ID
                    
                case 'p':
                case 'P':
                    
                    if (isdigit(argV[argI][2])) {
                        digIdx = 2;
                    }
                    else {
                        argI++;
                        digIdx = 0;
                    }
                    
                    Sint32 playoutID;
                    
                    if (!GetAsciiInteger(&argV[argI][digIdx], &playoutID)) {
                        
                        return FALSE;
                        
                    };
                    
                    si->playoutID = playoutID;
                    
                    break;
                    
                case 'u':
                case 'U':
                    
                    if (argV[argI][2] == 'p') {
                        
                        argI++;
                        
                        si->pictureUUID = argV[argI];
                        
                    }
                    
                    if (argV[argI][2] == 's') {
                        
                        argI++;
                        
                        si->soundUUID = argV[argI];
                        
                    }
                    
                    if (argV[argI][2] == 'l') {
                        
                        argI++;
                        
                        si->playlistUUID = argV[argI];
                        
                    }
                    
                    break;

                    
                case 'e':
                case 'E':
                    
                    if (argV[argI][2] == 's') {
                        
                        argI++;
                        digIdx = 0;
                        
                        Sint32 screenOffset;
                        
                        if (!GetAsciiInteger(&argV[argI][digIdx], &screenOffset)) {
                            
                            return FALSE;
                            
                        };
                        
                        si->screenOffset = screenOffset;
                        
                    }
                    
                    if (argV[argI][2] == 'o') {
                        
                        argI++;
                        digIdx = 0;
                        
                        Sint32 outputOffset;
                        
                        if (!GetAsciiInteger(&argV[argI][digIdx], &outputOffset)) {
                            
                            return FALSE;
                            
                        };
                        
                        si->outputOffset = outputOffset;
                        
                    }
                   
                    break;
                    
                    // Output filename
                    
                case 'o':
                case 'O':
                    
                    argI++;
                    si->outputFileName = argV[argI];
                    
                    break;
                    
                    // Fail
                    
                case '?':
                default:
                    
                    SHOWUSAGE
                    
                    return FALSE;
                    
            }
        }
        else {
            
            break;
            
        }
    }
    
    if (argC - argI > 0) {
        
        if (si->outputFileName != NULL) {
            
            fprintf(stderr, "%s: output file name specified twice: '%s' and '%s'\n\n", ProgramName, si->outputFileName, argV[argI]);
            
            return FALSE;
        }
        
        si->outputFileName = argV[argI];
    }
    
    if (si->outputFileName != NULL) {
        
        FILE*   fpOut;
        
        if ((fpOut = fopen(si->outputFileName, "wb")) == NULL) {
            
            fprintf(stderr, "%s: failed to open %s\n\n", ProgramName, si->outputFileName);
            
            return FALSE;
        }
        
        si->outputFile = fpOut;
        
    }
    
    return TRUE;
    
}

/*
 * Function: nextFrame
 * Increment {time/picture/sound} indices with 1
 */


Bool nextFrame(syncSample *ss, Uint8 *buffer, Uint32 *count) {
    
    Uint8 lcount;
    *count = 0;
    
    ss->timeIndex++;
    ss->pictureIndex++;
    ss->soundIndex++;
    
    if (!writeUint32(ss->timeIndex, buffer + 3*6, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    
    if (!writeUint32(ss->pictureIndex, buffer + 16*6, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    
    if (!writeUint32(ss->soundIndex, buffer + 26*6, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    
    return TRUE;
    
}

/*
 * Function: main
 * Sync generation CLI
 * Derived from [DCBSO]
 */

int main ( int argC,  char *argV[])
{
    /*
     * Begin testing-preamble
     */
    
    /*
    char *arguments[29] = {
        "dcisg",
        "-d", "2",                                          // 2 frames
        "-f", "24",                                         // 24 frames per second
        "-r", "48000",                                      // sample rate
        "-s", "2",                                          // play status
        "-p", "0x12345678",                                 // playout id
        "-o", "sync.wav",                                   // output
        "-it","0",                                          // time index starts at zero
        "-ip","0",                                          // picture index starts at zero
        "-is","0",                                          // sound index starts at zero
        "-up", "797e19d4-50c1-4dd5-b8c7-9a082c8f4835",      // picture uuid
        "-us", "4e2ce144-27eb-4384-b560-b804155bed23",      // sound uuid
        "-ul", "785e8a50-9ee4-4146-a0fc-6433b5f4f126",      // playlist uuid
        "-es", "2240",                                      // processing delay (anchor = MB, frame processor delay + 5ms auditorium propagation)
        "-eo", "3760"                                       // output delay (anchor = received sound, 3 frames projector delay - picture output offset)
    };
    
    argC = 0;
    argV = arguments;
     */
    
    /*
     * End testing-preamble
     */
    
    SHOWBANNER
    
    syncInput si;
    
    initInputs(&si);
    
    // Parsing input
    
    if (!parseInputs(&si,argC,argV)) {
        
        fprintf(stderr,"%s: failed to initialize. Exiting.\n\n",si.programName);
        
        return FALSE;
        
    };
    
    // Create Output Buffer
    
    Uint32 frameDuration = si.sampleRate / si.frameRate;
    
    Uint32 frameSize = 3*frameDuration;
    
    Uint8 *frame = NULL;
    
    frame = (Uint8 *) calloc(sizeof(Uint8),frameSize);
    
    if (frame == NULL) {
        
        fprintf(stderr,"%s: failed to allocate memory. Exiting.\n\n",si.programName);
        
        return FALSE;
        
    }
    
    for (int i = 0; i < frameSize; i++) {
        
        frame[i] = 0;
        
    }
    
    // Create WaveHeader
    
    WaveHeader waveHeader = {
        "RIFF",
        0,               // Int32 chunkSize;
        "WAVE",          // char format[4];
        "fmt ",          // char subChunk1ID[4];
        16,              // Int32 subChunk1Size;
        WAVE_FORMAT_PCM, // short audioFormat;
        1,               // short numChannels;
        48000,           // Int32 sampleRate;
        48000*3,         // Int32 byteRate;
        1*3,             // short blockAlign;
        24,              // short bitsPerSample;
        "data",          // char subChunk2ID[4];
        0                // Int32 subChunk2Size;
    };
    
    waveHeader.sampleRate = si.sampleRate;
    waveHeader.byteRate = waveHeader.sampleRate * 3;
    waveHeader.subChunk2Size = waveHeader.byteRate * si.numFrames;
    waveHeader.chunkSize = 36 + waveHeader.subChunk2Size;
    
    // Create Sync Signal
    
    Bool    success = TRUE;
    Uint32  count = 0;
    
    syncSample  sSample;
    
    if (!initSyncSample(&sSample)) success = FALSE;
    
    if (!setStatus(&sSample, (Uint8) si.state)) success = FALSE;
    
    if (!setTimeIndex(&sSample,si.timeIndex)) success = FALSE;
    
    if (!setPlayoutID(&sSample, si.playoutID)) success = FALSE;
    
    if (!setEUDuration(&sSample, (Uint16) frameDuration)) success = FALSE;
    
    if (!setSampleDuration(&sSample, 1, si.sampleRate)) success = FALSE;
    
    if (!setOutputOffset(&sSample, si.outputOffset)) success = FALSE;
    
    if (!setScreenOffset(&sSample, si.screenOffset)) success = FALSE;
    
    if (!setPictureIndex(&sSample, si.pictureIndex)) success = FALSE;
    
    if (!setPictureUUID(&sSample, si.pictureUUID)) success = FALSE;
    
    if (!setSoundIndex(&sSample, si.soundIndex)) success = FALSE;
    
    if (!setSoundUUID(&sSample, si.soundUUID)) success = FALSE;
    
    if (!setPlaylistUUID(&sSample, si.playlistUUID)) success = FALSE;
    
    if (!writeSyncSample(&sSample, frame, &count)) success = FALSE;
    
    if (!success) {
        
        fprintf(stderr, "%s: failed to create first frame. Exiting.\n\n", si.programName);
        
        return FALSE;
        
    }
    
    // Write Header
    
    if (sizeof(waveHeader) != fwrite((char *)&waveHeader, 1, sizeof(waveHeader), si.outputFile)) {
        
        fprintf(stderr,"%s: failed to write file header. Exiting.\n\n", si.programName);
        
        return FALSE;
        
    };
    
    // Write Frames
    
    for (int i = 0; i < si.numFrames; i++) {
        
        if (i > 0) {
            
            if (!nextFrame(&sSample, frame, &count)) {
                
                fprintf(stderr,"%s: failed to create frame [%d]. Exiting.\n\n", si.programName, i);
                
                return FALSE;
              
            };
            
        }
                
        if (frameSize != fwrite((char *) frame, 1, frameSize, si.outputFile)) {
            
            fprintf(stderr,"%s: failed to write frame [%d] to file. Exiting.\n\n", si.programName, i);
            
            return FALSE;
            
        };
        
    }
    
    // Cleaning up
    
    deleteInputs(&si);
    
    deleteSyncSample(&sSample);
    
    free(frame);
    
    // Goodbye
    
    fprintf(stderr,"Goodbye.\n\n");
    
    return TRUE;
    
}