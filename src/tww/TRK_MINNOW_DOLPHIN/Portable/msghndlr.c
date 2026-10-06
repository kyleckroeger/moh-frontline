/* Frontline's message handlers. The function bodies follow Pikmin's CC0
   msghndlr.c (https://github.com/doldecomp/pikmin): TRKMessageIntoReply and
   TRKSendACK are inline, the one-byte append is inlined, and locals are
   declared in the order that gives the target's stack slots (the transfer
   buffers are not 32-byte aligned). From the TWW version (this file's
   upstream): the IsTRKConnected flag and its accessors, set by
   TRKDoConnect/TRKDoDisconnect. Frontline's TRKDoSetOption reads three bytes
   and switches serial I/O for option 1. Functions are emitted in reverse
   source order (-inline deferred). */
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "TRK_MINNOW_DOLPHIN/utils/common/MWTrace.h"
#include "trk.h"
#include "string.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/target_options.h"

/* Types and declarations from Pikmin's CC0 MetroTRK headers (trktypes.h,
   trkenum.h) that the TWW headers do not have. */
typedef struct DSVersions {
	u8 kernelMajor;
	u8 kernelMinor;
	u8 protocolMajor;
	u8 protocolMinor;
} DSVersions;

typedef struct DSCPUType {
	u8 cpuMajor;
	u8 cpuMinor;
	u8 bigEndian;
	u8 defaultTypeSize;
	u8 fpTypeSize;
	u8 extended1TypeSize;
	u8 extended2TypeSize;
} DSCPUType;

DSError TRKTargetVersions(DSVersions*);
DSError TRKTargetCPUType(DSCPUType*);

/* The append is inlined, as in Pikmin's trk.h (msgbuf.c's body). */
inline DSError TRKAppendBuffer1_ui8(TRKBuffer* buffer, const u8 data) {
    if (buffer->position >= 0x880) {
        return DS_MessageBufferOverflow;
    }

    buffer->data[buffer->position++] = data;
    buffer->length++;
    return DS_NoError;
}

static BOOL IsTRKConnected;

BOOL GetTRKConnected(void) {
    return IsTRKConnected;
}

void SetTRKConnected(BOOL isTRKConnected) {
    IsTRKConnected = isTRKConnected;
}

inline void TRKMessageIntoReply(TRKBuffer* buffer, MessageCommandID ackCmd, DSReplyError errSentInAck)
{
	TRKResetBuffer(buffer, 1);

	TRKAppendBuffer1_ui8(buffer, ackCmd);
	TRKAppendBuffer1_ui8(buffer, errSentInAck);
}

inline DSError TRKSendACK(TRKBuffer* buffer)
{
	DSError err;
	int ackTries;

	ackTries = 3;
	do {
		err = TRKMessageSend(buffer);
		--ackTries;
	} while (err != DS_NoError && ackTries > 0);

	return err;
}

DSError TRKStandardACK(TRKBuffer* buffer, MessageCommandID commandID, DSReplyError replyError)
{
	TRKMessageIntoReply(buffer, commandID, replyError);
	return TRKSendACK(buffer);
}

DSError TRKDoUnsupported(TRKBuffer* buffer)
{
	return TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_UnsupportedCommandError);
}

DSError TRKDoConnect(TRKBuffer* buffer)
{
	IsTRKConnected = TRUE;
	return TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
}

DSError TRKDoDisconnect(TRKBuffer* buffer)
{
	DSError error;
	TRKEvent event;

	IsTRKConnected = FALSE;
	error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	if (error == DS_NoError) {
		TRKConstructEvent(&event, 1);
		TRKPostEvent(&event);
	}
	return error;
}

DSError TRKDoReset(TRKBuffer* buffer)
{
	TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
	__TRK_reset();
	return DS_NoError;
}

DSError TRKDoVersions(TRKBuffer* buffer)
{
	DSError error;
	DSVersions versions;

	if (buffer->length != 1) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
	} else {
		TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
		error = TRKTargetVersions(&versions);

		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, versions.kernelMajor);
		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, versions.kernelMinor);
		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, versions.protocolMajor);
		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, versions.protocolMinor);

		if (error != DS_NoError)
			error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_CWDSError);
		else
			error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoSupportMask(TRKBuffer* buffer)
{
	DSError error;
	u8 mask[32];

	if (buffer->length != 1) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
	} else {
		TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
		error = TRKTargetSupportMask(mask);

		if (error == DS_NoError)
			error = TRKAppendBuffer(buffer, mask, 32);
		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui8(buffer, 2);

		if (error != DS_NoError)
			error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_CWDSError);
		else
			error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoCPUType(TRKBuffer* buffer)
{
	DSError error;
	DSCPUType cputype;

	if (buffer->length != 1) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}

	TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	error = TRKTargetCPUType(&cputype);

	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.cpuMajor);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.cpuMinor);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.bigEndian);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.defaultTypeSize);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.fpTypeSize);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.extended1TypeSize);
	if (error == DS_NoError)
		error = TRKAppendBuffer1_ui8(buffer, cputype.extended2TypeSize);

	if (error != DS_NoError)
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_CWDSError);
	else
		error = TRKSendACK(buffer);

	return error;
}

DSError TRKDoReadMemory(TRKBuffer* buffer)
{
	DSError error;
	DSReplyError replyError;
	u8 msg_command;
	u8 msg_options;
	u16 msg_length;
	u32 msg_start;
	u32 length;
	u8 tempBuf[0x800];

	if (buffer->length != 8) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}

	TRKSetBufferPosition(buffer, DSREPLY_NoError);
	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_length);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui32(buffer, &msg_start);

	if (msg_options & 2) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_UnsupportedOptionError);
		return error;
	}

	if (msg_length > 0x800) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_ParameterError);
		return error;
	}

	TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	if (error == DS_NoError) {
		length = (u32)msg_length;
		error  = TRKTargetAccessMemory(tempBuf, msg_start, &length, (msg_options & 8) ? MEMACCESS_UserMemory : MEMACCESS_DebuggerMemory, 1);
		msg_length = (u16)length;
		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui16(buffer, msg_length);
		if (error == DS_NoError)
			error = TRKAppendBuffer(buffer, tempBuf, length);
	}

	if (error != DS_NoError) {
		switch (error) {
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidMemory:
			replyError = DSREPLY_InvalidMemoryRange;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
			break;
		}
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, replyError);
	} else {
		error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoWriteMemory(TRKBuffer* buffer)
{
	DSError error;
	DSReplyError replyError;
	u8 msg_command;
	u8 msg_options;
	u16 msg_length;
	u32 msg_start;
	u32 length;
	u8 tmpBuffer[0x800];

	if (buffer->length <= 8) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}

	TRKSetBufferPosition(buffer, DSREPLY_NoError);
	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_length);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui32(buffer, &msg_start);

	if (msg_options & 2) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_UnsupportedOptionError);
		return error;
	}

	if ((buffer->length != msg_length + 8) || (msg_length > 0x800)) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_ParameterError);
	} else {
		if (error == DS_NoError) {
			length = (u32)msg_length;
			error  = TRKReadBuffer(buffer, tmpBuffer, length);
			if (error == DS_NoError) {
				error = TRKTargetAccessMemory(tmpBuffer, msg_start, &length,
				                              (msg_options & 8) ? MEMACCESS_UserMemory : MEMACCESS_DebuggerMemory, FALSE);
			}
			msg_length = (u16)length;
		}

		if (error == DS_NoError)
			TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

		if (error == DS_NoError)
			error = TRKAppendBuffer1_ui16(buffer, msg_length);

		if (error != DS_NoError) {
			switch (error) {
			case DS_CWDSException:
				replyError = DSREPLY_CWDSException;
				break;
			case DS_InvalidMemory:
				replyError = DSREPLY_InvalidMemoryRange;
				break;
			case DS_InvalidProcessID:
				replyError = DSREPLY_InvalidProcessID;
				break;
			case DS_InvalidThreadID:
				replyError = DSREPLY_InvalidThreadID;
				break;
			case DS_OSError:
				replyError = DSREPLY_OSError;
				break;
			default:
				replyError = DSREPLY_CWDSError;
				break;
			}
			error = TRKStandardACK(buffer, DSMSG_ReplyACK, replyError);
		} else {
			error = TRKSendACK(buffer);
		}
	}

	return error;
}

DSError TRKDoReadRegisters(TRKBuffer* buffer)
{
	DSReplyError replyError;
	DSError error;
	u8 msg_command;
	u8 msg_options;
	u16 msg_firstRegister;
	u16 msg_lastRegister;
	u32 registerDataLength;
	DSMessageRegisterOptions options;

	if (buffer->length != 6) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}
	TRKSetBufferPosition(buffer, DSREPLY_NoError);
	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_firstRegister);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_lastRegister);

	if (msg_firstRegister > msg_lastRegister) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_InvalidRegisterRange);
		return error;
	}

	if (error == DS_NoError)
		TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	options = (DSMessageRegisterOptions)(msg_options & 7);
	switch (options) {
	case DSREG_Default:
		error = TRKTargetAccessDefault(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, TRUE);
		break;
	case DSREG_FP:
		error = TRKTargetAccessFP(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, TRUE);
		break;
	case DSREG_Extended1:
		error = TRKTargetAccessExtended1(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, TRUE);
		break;
	case DSREG_Extended2:
		error = TRKTargetAccessExtended2(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, TRUE);
		break;
	default:
		error = DS_UnsupportedError;
		break;
	}

	if (error != DS_NoError) {
		switch (error) {
		case DS_UnsupportedError:
			replyError = DSREPLY_UnsupportedOptionError;
			break;
		case DS_InvalidRegister:
			replyError = DSREPLY_InvalidRegisterRange;
			break;
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
		}

		error = TRKStandardACK(buffer, DSMSG_ReplyACK, replyError);
	} else {
		error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoWriteRegisters(TRKBuffer* buffer)
{
	DSReplyError replyError;
	DSError error;
	u8 msg_command;
	u8 msg_options;
	u16 msg_firstRegister;
	u16 msg_lastRegister;
	u32 registerDataLength;
	DSMessageRegisterOptions options;

	if (buffer->length <= 6) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}
	TRKSetBufferPosition(buffer, DSREPLY_NoError);
	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_firstRegister);

	if (error == DS_NoError)
		error = TRKReadBuffer1_ui16(buffer, &msg_lastRegister);

	if (msg_firstRegister > msg_lastRegister) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_InvalidRegisterRange);
		return error;
	}

	options = (DSMessageRegisterOptions)msg_options;
	switch (options) {
	case DSREG_Default:
		error = TRKTargetAccessDefault(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, FALSE);
		break;
	case DSREG_FP:
		error = TRKTargetAccessFP(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, FALSE);
		break;
	case DSREG_Extended1:
		error = TRKTargetAccessExtended1(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, FALSE);
		break;
	case DSREG_Extended2:
		error = TRKTargetAccessExtended2(msg_firstRegister, msg_lastRegister, buffer, &registerDataLength, FALSE);
		break;
	default:
		error = DS_UnsupportedError;
		break;
	}

	if (error == DS_NoError)
		TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	if (error != DS_NoError) {
		switch (error) {
		case DS_UnsupportedError:
			replyError = DSREPLY_UnsupportedOptionError;
			break;
		case DS_InvalidRegister:
			replyError = DSREPLY_InvalidRegisterRange;
			break;
		case DS_MessageBufferReadError:
			replyError = DSREPLY_PacketSizeError;
			break;
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
		}

		error = TRKStandardACK(buffer, DSMSG_ReplyACK, replyError);
	} else {
		error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoFlushCache(TRKBuffer* buffer)
{
	u32 msg_start;
	u32 msg_end;
	u8 msg_command;
	u8 msg_options;
	DSError error;
	DSReplyError replyErr;

	if (buffer->length != 10) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}

	TRKSetBufferPosition(buffer, DSREPLY_NoError);
	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui32(buffer, &msg_start);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui32(buffer, &msg_end);

	if (msg_start > msg_end) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_InvalidMemoryRange);
		return error;
	}

	if (error == DS_NoError)
		error = TRKTargetFlushCache(msg_options, (void*)msg_start, (void*)msg_end);

	if (error == DS_NoError)
		TRKMessageIntoReply(buffer, DSMSG_ReplyACK, DSREPLY_NoError);

	if (error != DS_NoError) {
		switch (error) {
		case DS_UnsupportedError:
			replyErr = DSREPLY_UnsupportedOptionError;
			break;
		default:
			replyErr = DSREPLY_CWDSError;
			break;
		}

		error = TRKStandardACK(buffer, DSMSG_ReplyACK, replyErr);
	} else {
		error = TRKSendACK(buffer);
	}

	return error;
}

DSError TRKDoContinue(TRKBuffer* buffer)
{
	DSError error;

	error = TRKTargetStopped();
	if (error == DS_NoError) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NotStopped);
		return error;
	}

	error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
	if (error == DS_NoError)
		error = TRKTargetContinue();

	return error;
}

DSError TRKDoStep(TRKBuffer* buffer)
{
	u32 pc;
	u8 msg_command;
	u8 msg_options;
	u8 msg_count;
	u32 msg_rangeStart;
	u32 msg_rangeEnd;
	DSError error;

	if (buffer->length < 3) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
		return error;
	}

	TRKSetBufferPosition(buffer, DSREPLY_NoError);

	error = TRKReadBuffer1_ui8(buffer, &msg_command);
	if (error == DS_NoError)
		error = TRKReadBuffer1_ui8(buffer, &msg_options);

	switch (msg_options) {
	case DSSTEP_IntoCount:
	case DSSTEP_OverCount:
		if (error == DS_NoError)
			TRKReadBuffer1_ui8(buffer, &msg_count);
		if (msg_count >= 1) {
			break;
		}
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_ParameterError);
		return error;
	case DSSTEP_IntoRange:
	case DSSTEP_OverRange:
		if (buffer->length != 10) {
			error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_PacketSizeError);
			return error;
		}

		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(buffer, &msg_rangeStart);
		if (error == DS_NoError)
			error = TRKReadBuffer1_ui32(buffer, &msg_rangeEnd);

		pc = TRKTargetGetPC();
		if (pc >= msg_rangeStart && pc <= msg_rangeEnd) {
			break;
		}
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_ParameterError);
		return error;
	default:
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_UnsupportedOptionError);
		return error;
	}

	if (!TRKTargetStopped()) {
		error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NotStopped);
		return error;
	}

	error = TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
	if (error == DS_NoError) {
		switch (msg_options) {
		case DSSTEP_IntoCount:
		case DSSTEP_OverCount:
			error = TRKTargetSingleStep(msg_count, (msg_options == DSSTEP_OverCount));
			break;
		case DSSTEP_IntoRange:
		case DSSTEP_OverRange:
			error = TRKTargetStepOutOfRange(msg_rangeStart, msg_rangeEnd, (msg_options == DSSTEP_OverRange));
			break;
		}
	}

	return error;
}

DSError TRKDoStop(TRKBuffer* b)
{
	DSReplyError replyError;

	switch (TRKTargetStop()) {
	case DS_NoError:
		replyError = DSREPLY_NoError;
		break;
	case DS_InvalidProcessID:
		replyError = DSREPLY_InvalidProcessID;
		break;
	case DS_InvalidThreadID:
		replyError = DSREPLY_InvalidThreadID;
		break;
	case DS_OSError:
		replyError = DSREPLY_OSError;
		break;
	default:
		replyError = DSREPLY_Error;
		break;
	}

	return TRKStandardACK(b, DSMSG_ReplyACK, replyError);
}

DSError TRKDoSetOption(TRKBuffer* buffer)
{
	DSError error;
	u8 spacer = 0;
	u8 option = 0;
	u8 enable = 0;

	TRKSetBufferPosition(buffer, 0);
	error = TRKReadBuffer1_ui8(buffer, &spacer);
	if (error == DS_NoError) {
		error = TRKReadBuffer1_ui8(buffer, &option);
	}
	if (error == DS_NoError) {
		error = TRKReadBuffer1_ui8(buffer, &enable);
	}
	if (error != DS_NoError) {
		TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_Error);
	} else if (option == 1) {
		SetUseSerialIO(enable);
	}
	return TRKStandardACK(buffer, DSMSG_ReplyACK, DSREPLY_NoError);
}
