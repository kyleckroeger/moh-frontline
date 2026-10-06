/* Frontline's file-support requests, the functions at the start of the
   file record. TRKRequestSend follows Pikmin's CC0 support.c
   (https://github.com/doldecomp/pikmin) with its two reply bytes declared in
   the other order; the one-byte append is inlined (local inline copy of
   msgbuf.c's TRKAppendBuffer1_ui8). The open, close and position requests are
   Frontline's older protocol, written from the target: the command and
   arguments are appended one by one (no CommandReply header), the I/O result
   is a byte, and the reply is read from position 2. Functions are emitted in
   reverse source order (-inline deferred). TRKSuppAccessFile, the last
   function in the target, is not part of the unit (its read flag and byte
   count are allocated to each other's registers; draft in
   scratch/lib/support_wip.c). */
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/support.h"
#include "TRK_MINNOW_DOLPHIN/utils/common/MWTrace.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "string.h"

/* The append is inlined, as in Pikmin's trk.h (msgbuf.c's body). */
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}


DSError TRKRequestSend(TRKBuffer* msgBuf, int* bufferId, u32 p1, u32 p2, int p3)
{
	int error = DS_NoError;
	TRKBuffer* buffer;
	u32 timer;
	int tries;
	u8 msg_command;
	u8 msg_error;
	BOOL badReply = TRUE;

	*bufferId = -1;

	for (tries = p2 + 1; tries != 0 && *bufferId == -1 && error == DS_NoError; tries--) {
		error = TRKMessageSend(msgBuf);
		if (error == DS_NoError) {
			if (p3) {
				timer = 0;
			}

			while (TRUE) {
				do {
					*bufferId = TRKTestForPacket();
					if (*bufferId != -1)
						break;
				} while (!p3 || ++timer < 79999980);

				if (*bufferId == -1)
					break;

				badReply = FALSE;

				buffer = TRKGetBuffer(*bufferId);
				TRKSetBufferPosition(buffer, 0);

				if ((error = TRKReadBuffer1_ui8(buffer, &msg_command)) != DS_NoError)
					break;

				if (msg_command >= DSMSG_ReplyACK)
					break;

				TRKProcessInput(*bufferId);
				*bufferId = -1;
			}

			if (*bufferId != -1) {
				if (buffer->length < p1) {
					badReply = TRUE;
				}
				if (error == DS_NoError && !badReply) {
					error = TRKReadBuffer1_ui8(buffer, &msg_error);
				}
				if (error == DS_NoError && !badReply) {
					if (msg_command != DSMSG_ReplyACK || msg_error != DSREPLY_NoError) {
						badReply = TRUE;
					}
				}
				if (error != DS_NoError || badReply) {
					TRKReleaseBuffer(*bufferId);
					*bufferId = -1;
				}
			}
		}
	}

	if (*bufferId == -1) {
		error = DS_Error800;
	}

	return error;
}

DSError HandleOpenFileSupportRequest(const char* path, u8 mode, u32* handle, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	*handle = 0;
	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_OpenFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, mode);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui16(buffer, strlen(path) + 1);
	if (error == DS_NoError)
		error = TRKAppendBuffer_ui8(buffer, (u8*)path, strlen(path) + 1);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(replyBuffer, handle);
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}

DSError HandleCloseFileSupportRequest(u32 handle, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_CloseFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, handle);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}

DSError HandlePositionFileSupportRequest(DSFileHandle handle, u32* position, u8 mode, u8* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* replyBuffer;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, DSMSG_PositionFile);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, handle);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui32(buffer, *position);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, mode);
	if (error == DS_NoError) {
		*ioResult = 0;
		error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			replyBuffer = TRKGetBuffer(replyBufferId);
			TRKSetBufferPosition(replyBuffer, 2);
		}
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(replyBuffer, position);
		else
			*position = -1;
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}
