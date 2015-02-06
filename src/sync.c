//
//  File: sync.c
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
//  This file implements an API for the generation of sync signals.

//  TODO: adding documentation and references to specification.

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "sync.h"

// Auxiliary functions

/*
 *  Function: getHexChar
 *  Hex character to hex value
 */

Bool getHexChar(Uint8 cin, Uint8 *cout, Bool *count) {
    
    cin = tolower(cin);
    
    *count = FALSE;
    *cout = 0;
    
    if (('0' <= cin) && (cin <= '9')) {
        
        *cout = (Uint8) (cin - '0');
        *count = TRUE;
        
        return TRUE;
    }
    
    if (('a' <= cin) && (cin <= 'f')) {
        
        *cout = (Uint8) (cin - 'a' + 10);
        *count = TRUE;
        
        return TRUE;
    }
    
    if (cin == '-') {
        
        *count = FALSE;
        return TRUE;
        
    }
    
    return FALSE;
}

/*
 *  Function: str2UUID
 *  UUID string to UUID value
 */

Bool str2UUID(char *str,UUID uuid) {
    
    for (int i = 0; i < 16; i++) {
        
        Uint8 cm, cl;
        Bool count;
        
        // Get first char
        
        if (!getHexChar(*(str++),&cm,&count)) {
            
            return FALSE;
        }
        
        
        if (count) {                                    // If first not '-'
            
            if (!getHexChar(*(str++),&cl,&count)) {
                
                return FALSE;
            }
            
            if (!count) {                               // If second is '-'
                
                return FALSE;
                
            }

        } else {                                        // If first is '-'
            
            if (!getHexChar(*(str++),&cm,&count)) {
                
                return FALSE;
            }
                                                        // First is not '-'
            if (!count) {                               // If second is '-'
                
                return FALSE;
                
            }
                                                        // Second is not '-'
            if (!getHexChar(*(str++),&cl,&count)) {
                
                return FALSE;
            }
            
            if (!count) {                               // If third is '-'
                
                return FALSE;
                
            }
                                                        // Third is not '-'
        }
        
        uuid[i] = (Uint8) ((cm << 4) | cl) & 0xFF;
        
    }
    
    return TRUE;
    
}

/*
 *  Function: initSyncSample
 *  Intialize syncSample instance
 *  Section 5.3.1.1 of [ADSSTP]
 */

Bool initSyncSample(syncSample *ss) {
    
    ss->marker = SYNCMARKER;
    ss->length = 42;
    ss->timeIndex = 0;
    ss->playoutID = 0;
    ss->euDuration = 0;
    ss->sampleEnum = 0;
    ss->sampleDenom = 0;
    ss->outputOffset = 0;
    ss->screenOffset = 0;
    ss->pictureIndex= 0;
    ss->soundIndex = 0;
    ss->extensionLength = 0;
    ss->extension = NULL;
    
    for (int i = 0; i < 16; i++) {
        
        ss->pictureUUID[i] = 0;
        ss->soundUUID[i] = 0;
        ss->playlistUUID[i] = 0;
    }

    return TRUE;
}

// Section 5.3.3 of [ADSSTP]

Bool setLength(syncSample *ss, Uint16 length) {
    
    ss->length = length;
    
    return TRUE;
}

// Section 5.3.4 of [ADSSTP]

Bool setStatus(syncSample *ss, Uint8 status) {
    
    Bool success = FALSE;
    
    if (status < NSTATES) {
        
        ss->flags = status;
        
        success = TRUE;
    }
    
    return success;
    
}

Bool setTimeIndex(syncSample *ss, Uint32 timeIndex) {
    
    ss->timeIndex = timeIndex;
    
    return TRUE;
}

Bool setPlayoutID(syncSample *ss, Uint32 playoutID) {
    
    ss->playoutID = playoutID;
    
    return TRUE;
    
}

Bool setEUDuration(syncSample *ss, Uint16 euDuration) {
    
    ss->euDuration = euDuration;
    
    return TRUE;
    
}

Bool setSampleDuration(syncSample *ss, Uint32 enumerator, Uint32 denominator) {
    
    ss->sampleEnum = enumerator;
    ss->sampleDenom = denominator;
    
    return TRUE;
    
}

Bool setOutputOffset(syncSample *ss, Sint32 outputOffset) {
    
    ss->outputOffset = outputOffset;
    
    return TRUE;
    
}

Bool setScreenOffset(syncSample *ss, Uint32 screenOffset) {
    
    ss->screenOffset = screenOffset;
    
    return TRUE;
}

Bool setPictureIndex(syncSample *ss, Uint32 pictureIndex) {
    
    ss->pictureIndex = pictureIndex;
    
    return TRUE;
    
}

Bool setPictureUUID(syncSample *ss, char *uuid) {
    
    if (!str2UUID(uuid,ss->pictureUUID)) {
        
        return FALSE;
    }
    
    return TRUE;
    
}

Bool setSoundIndex(syncSample *ss, Uint32 soundIndex) {
    
    ss->soundIndex = soundIndex;
    
    return TRUE;
    
}

Bool setSoundUUID(syncSample *ss, char *uuid) {
    
    if (!str2UUID(uuid,ss->soundUUID)) {
        
        return FALSE;
    }
    
    return TRUE;
    
}

Bool setPlaylistUUID(syncSample *ss, char *uuid) {
    
    if (!str2UUID(uuid,ss->playlistUUID)) {
        
        return FALSE;
    }
    
    return TRUE;
    
}

Bool setExtension(syncSample *ss, Uint16 *extension, Uint16 extensionLength) {
    
    ss->extension = (Uint16 *) calloc(sizeof(Uint16),extensionLength);
    
    if (ss->extension == NULL) {
        
        return FALSE;
        
    }
    
    ss->extensionLength = extensionLength;
    
    for (int i = 0; i < extensionLength; i++) {
        
        ss->extension[i] = extension[i];
        
    }
    
    return TRUE;
}

Bool deleteSyncSample(syncSample *ss) {
    
    free(ss->extension);
    ss->extension = NULL;
    
    return TRUE;
    
}



// Section 5.2 of [ADSSTP]

Bool writeUint16(Uint16 value, Uint8 *buffer, Uint8 *count, Bool first) {
    
    Uint32 lval, tval;
    
    lval = (Uint32) value;
    
    if (first) {
        
        lval |= 0x010000;
    }
    
    tval = ((~lval) + 1) & 0x00FFFFFF;
    
    buffer[0] = (Uint8) (lval & 0xFF);
    buffer[3] = (Uint8) (tval & 0xFF);
    
    buffer[1] = (Uint8) ((lval >> 8) & 0xFF);
    buffer[4] = (Uint8) ((tval >> 8) & 0xFF);

    buffer[2] = (Uint8) ((lval >> 16) & 0xFF);
    buffer[5] = (Uint8) ((tval >> 16) & 0xFF);
    
    *count = 6;
    
    return TRUE;
}

// Section 5.2 of [ADSSTP]

Bool writeUint32(Uint32 value, Uint8 *buffer, Uint8 *count, Bool first) {
    
    Uint8   lcount;
    
    if (!writeUint16((Uint16)((value >> 16) & 0xFFFF), buffer, &lcount,first)) {
        
        return FALSE;
        
    }
    
    *count = lcount;
    
    if (!writeUint16((Uint16)(value & 0xFFFF), buffer+lcount, &lcount,FALSE)) {
        
        return FALSE;
        
    }
    
    *count += lcount;
    
    return TRUE;
    
}

Bool writeSint32(Sint32 value, Uint8 *buffer, Uint8 *count, Bool first) {
    
    return writeUint32((Uint32) value, buffer,count,first);
    
}

Bool writeUUID(UUID uuid, Uint8 *buffer, Uint8 *count, Bool first) {

    Uint16 val;
    Uint8  lcount;
    
    *count = 0;

    for (int i = 0; i < 16; i += 2) {
        
        val = (uuid[i+1] | (uuid[i] << 8)) & 0xFFFF;
        
        if (!writeUint16(val, buffer, &lcount, FALSE)) {
            
            return FALSE;
        }
        
        *count += lcount;
        buffer += lcount;
        
    }
    
    return TRUE;
}

//  Section 5.2 of [ADSSTP]

Bool writeSyncSample(syncSample *ss, Uint8 *buffer, Uint32 *count) {
    
    Uint8 lcount;
    *count = 0;
    
    if (!writeUint16(ss->marker, buffer, &lcount, TRUE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    ss->length = 42 + ss->extensionLength;
    
    if (!writeUint16(ss->length, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint16(ss->flags, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    if (!writeUint32(ss->timeIndex, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint32(ss->playoutID, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    if (!writeUint16(ss->euDuration, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;


    if (!writeUint32(ss->sampleEnum, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint32(ss->sampleDenom, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    if (!writeSint32(ss->outputOffset, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint32(ss->screenOffset, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint32(ss->pictureIndex, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    if (!writeUUID(ss->pictureUUID, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUint32(ss->soundIndex, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    if (!writeUUID(ss->soundUUID, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;
    
    if (!writeUUID(ss->playlistUUID, buffer, &lcount, FALSE)) {
        
        return FALSE;
        
    };
    
    *count += lcount;
    buffer += lcount;

    for (int i = 0; i < ss->extensionLength; i++) {
        
        if (!writeUint16(ss->extension[i], buffer, &lcount, FALSE)) {
            
            return FALSE;
            
        };
        
        *count += lcount;
        buffer += lcount;

    }
    
    return TRUE;
    
}

