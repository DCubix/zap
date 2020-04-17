#ifndef SPRITES_HPP
#define SPRITES_HPP

enum SpriteIndex {
	SpriteWireV = 0,
	SpriteWireH,
	SpriteWireLB,
	SpriteWireLT,
	SpriteWireRB,
	SpriteWireRT,

	SpriteWireTC,
	SpriteWireRC,
	SpriteWireBC,
	SpriteWireLC,

	Background,
	Cursor,

	SpriteWireRTB,
	SpriteWireLRB,
	SpriteWireLTB,
	SpriteWireLRT,

	SpriteWireLRTB,
	SpriteWireLCRCTCBC,

	Unused0,
	Unused1,

	SpriteWireTCR,
	SpriteWireRCB,
	SpriteWireBCL,
	SpriteWireLCB,

	SpriteWireTCL,
	SpriteWireRCT,
	SpriteWireBCR,
	SpriteWireLCT,

	SpriteWireTCRC,
	SpriteWireTCLC,
	SpriteWireLCBC,
	SpriteWireBCRC,

	SpriteWireRCTB,
	SpriteWireLRBC,
	SpriteWireLCTB,
	SpriteWireLRTC,

	SpriteWireRCTCB,
	SpriteWireLRCBC,
	SpriteWireLCTBC,
	SpriteWireLCRTC,

	SpriteWireRCTBC,
	SpriteWireLCRBC,
	SpriteWireLCTCB,
	SpriteWireLRCTC,

	SpriteWireRCTCBC,
	SpriteWireLCRCBC,
	SpriteWireLCTCBC,
	SpriteWireLCRCTC,

	SpriteWireLRCTB,
	SpriteWireLRTBC,
	SpriteWireLCRTB,
	SpriteWireLRTCB,

	SpriteWireLRCTBC,
	SpriteWireLCRTBC,
	SpriteWireLCRTCB,
	SpriteWireLRCTCB,

	SpriteWireLRCTCBC,
	SpriteWireLCRCTBC,
	SpriteWireLCRTCBC,
	SpriteWireLCRCTCB,

	Unused2,

	Power,
	LEDOff,
	LEDOn,

	ANDH,
	ANDV,
	ORH,
	ORV,

	NotR,
	NotB,
	NotL,
	NotT,

	SwitchOffR,
	SwitchOffB,
	SwitchOffL,
	SwitchOffT,

	SwitchOnR,
	SwitchOnB,
	SwitchOnL,
	SwitchOnT
};

#endif // SPRITES_HPP