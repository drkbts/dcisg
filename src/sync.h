//
//  File: sync.h
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
//  This is the header file for sync signal API implementation
//  in sync.c


#ifndef DCBSO_sync_h
#define DCBSO_sync_h

#define FALSE 0
#define TRUE 1

typedef unsigned char   Bool;

typedef unsigned char   Uint8;
typedef unsigned short  Uint16;
typedef unsigned int    Uint32;

typedef int             Sint32;

// Section 5.3.1.3 of [ADSSTP]

typedef Uint8   UUID[16];

// Section 5.3.1 of [ADSSTP]

#define SYNCMARKER  0xAAF0

// Section 5.3.2 of [ADSSTP]

#define NSTATES 3
#define STOPPED 0
#define PAUSED  1
#define PLAYING 2

// Section 5.3.1 of [ADSSTP]

typedef struct syncSample {
    
    Uint16  marker;
    Uint16  length;
    Uint16  flags;
    Uint32  timeIndex;
    Uint32  playoutID;
    Uint16  euDuration;
    Uint32  sampleEnum;
    Uint32  sampleDenom;
    Sint32  outputOffset;
    Uint32  screenOffset;
    Uint32  pictureIndex;
    UUID    pictureUUID;
    Uint32  soundIndex;
    UUID    soundUUID;
    UUID    playlistUUID;
    Uint16  extensionLength;
    Uint16* extension;
    
} syncSample;

// Auxiliary functions

Bool getHexChar(Uint8 cin, Uint8 *cout, Bool *count);

Bool str2UUID(char *str,UUID uuid);

// SyncSample setter functions

Bool initSyncSample(syncSample *ss);

Bool setStatus(syncSample *ss, Uint8 status);

Bool setTimeIndex(syncSample *ss, Uint32 timeIndex);

Bool setPlayoutID(syncSample *ss, Uint32 playoutID);

Bool setEUDuration(syncSample *ss, Uint16 euDuration);

Bool setSampleDuration(syncSample *ss, Uint32 enumerator, Uint32 denominator);

Bool setOutputOffset(syncSample *ss, Sint32 outputOffset);

Bool setScreenOffset(syncSample *ss, Uint32 screenOffset);

Bool setPictureIndex(syncSample *ss, Uint32 pictureIndex);

Bool setPictureUUID(syncSample *ss, char *uuid);

Bool setSoundIndex(syncSample *ss, Uint32 soundIndex);

Bool setSoundUUID(syncSample *ss, char *uuid);

Bool setPlaylistUUID(syncSample *ss, char *uuid);

Bool setExtension(syncSample *ss, Uint16 *extension, Uint16 extensionLength);

Bool deleteSyncSample(syncSample *ss);

// Output functions

Bool writeUint16(Uint16 value, Uint8 *buffer, Uint8 *count, Bool first);

Bool writeUint32(Uint32 value, Uint8 *buffer, Uint8 *count, Bool first);

Bool writeSint32(Sint32 value, Uint8 *buffer, Uint8 *count, Bool first);

Bool writeUUID(UUID uuid, Uint8 *buffer, Uint8 *count, Bool first);

Bool writeSyncSample(syncSample *ss, Uint8 *buffer, Uint32 *count);


#endif
