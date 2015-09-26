
#pragma once

enum SpeakerPosition
{
	cl_speaker_front_left                = 0x1,
	cl_speaker_front_right               = 0x2,
	cl_speaker_front_center              = 0x4,
	cl_speaker_low_frequency             = 0x8,
	cl_speaker_back_left                 = 0x10,
	cl_speaker_back_right                = 0x20,
	cl_speaker_front_left_of_center      = 0x40,
	cl_speaker_front_right_of_center     = 0x80,
	cl_speaker_back_center               = 0x100,
	cl_speaker_side_left                 = 0x200,
	cl_speaker_side_right                = 0x400,
	cl_speaker_top_center                = 0x800,
	cl_speaker_top_front_left            = 0x1000,
	cl_speaker_top_front_center          = 0x2000,
	cl_speaker_top_front_right           = 0x4000,
	cl_speaker_top_back_left             = 0x8000,
	cl_speaker_top_back_center           = 0x10000,
	cl_speaker_top_back_right            = 0x20000
};

typedef unsigned int SpeakerPositionMask;
