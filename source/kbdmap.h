/* COPYING ******************************************************************
For copyright and licensing terms, see the file named COPYING.
// **************************************************************************
*/

#if !defined(INCLUDE_KBDMAP_H)
#define INCLUDE_KBDMAP_H

#include <stdint.h>

enum { KBDMAP_ROWS = 0x13, KBDMAP_COLS = 0x10 };

/// The entries in a keyboard map file are 10 bigendian 32-bit integers that unpack to this.
struct kbdmap_entry {
	uint32_t cmd,p[16];
};

enum {
	KBDMAP_ACTION_MASK	= 0xFF000000,
	KBDMAP_ACTION_UCS3	= 0x01000000,
	KBDMAP_ACTION_SYSTEM	= 0x02000000,
	KBDMAP_ACTION_MODIFIER	= 0x03000000,
	// Don't clash with 0x04000000 being X mouse positions in input messages.
	// Don't clash with 0x05000000 being Y mouse positions in input messages.
	// Don't clash with 0x06000000 being mouse wheel in input messages.
	// Don't clash with 0x07000000 being mouse buttons in input messages.
	// Don't clash with 0x09000000 being pasted UCS3 in input messages.
	KBDMAP_ACTION_SCREEN	= 0x0A000000,
	KBDMAP_ACTION_CONSUMER	= 0x0C000000,
	KBDMAP_ACTION_EXTENDED	= 0x0E000000,
	KBDMAP_ACTION_FUNCTION	= 0x0F000000,
	KBDMAP_ACTION_FUNCTION1	= 0x1F000000,
};

enum {
	KBDMAP_MODIFIER_1ST_LEVEL2	=  0U,
	KBDMAP_MODIFIER_1ST_LEVEL3	=  1U,
	KBDMAP_MODIFIER_1ST_GROUP2	=  2U,
	KBDMAP_MODIFIER_1ST_CONTROL	=  3U,
	KBDMAP_MODIFIER_1ST_SUPER	=  4U,
	KBDMAP_MODIFIER_1ST_ALT		=  5U,
	KBDMAP_MODIFIER_1ST_META	=  6U,
	KBDMAP_MODIFIER_2ND_LEVEL2	=  8U,
	KBDMAP_MODIFIER_2ND_LEVEL3	=  9U,
	KBDMAP_MODIFIER_2ND_GROUP2	= 10U,
	KBDMAP_MODIFIER_2ND_CONTROL	= 11U,
	KBDMAP_MODIFIER_2ND_SUPER	= 12U,
	KBDMAP_MODIFIER_2ND_ALT		= 13U,
	KBDMAP_MODIFIER_2ND_META	= 14U,
	KBDMAP_MODIFIER_LEVEL2		= 16U,
	KBDMAP_MODIFIER_LEVEL3		= 17U,
	KBDMAP_MODIFIER_CAPS		= 18U,
	KBDMAP_MODIFIER_NUM		= 19U,
	KBDMAP_MODIFIER_SCROLL		= 20U,
};

enum {
	KBDMAP_MODIFIER_CMD_MOMENTARY	= 0x01,	///< a momentary contact version of the modifier
	KBDMAP_MODIFIER_CMD_LATCH	= 0x02,	///< a latching version of the modifier
	KBDMAP_MODIFIER_CMD_LOCK	= 0x03,	///< a locking version of the modifier
};

enum {
	// ISO 9995 "E" row
	KBDMAP_INDEX_1			= 0x0001,
	KBDMAP_INDEX_2			= 0x0002,
	KBDMAP_INDEX_3			= 0x0003,
	KBDMAP_INDEX_4			= 0x0004,
	KBDMAP_INDEX_5			= 0x0005,
	KBDMAP_INDEX_6			= 0x0006,
	KBDMAP_INDEX_7			= 0x0007,
	KBDMAP_INDEX_8			= 0x0008,
	KBDMAP_INDEX_9			= 0x0009,
	KBDMAP_INDEX_0			= 0x000A,
	KBDMAP_INDEX_MINUS		= 0x000B,
	KBDMAP_INDEX_EQUALS		= 0x000C,
	KBDMAP_INDEX_BACKSPACE		= 0x000F,

	// ISO 9995 "D" row
	KBDMAP_INDEX_TAB		= 0x0100,
	KBDMAP_INDEX_Q			= 0x0101,
	KBDMAP_INDEX_W			= 0x0102,
	KBDMAP_INDEX_E			= 0x0103,
	KBDMAP_INDEX_R			= 0x0104,
	KBDMAP_INDEX_T			= 0x0105,
	KBDMAP_INDEX_Y			= 0x0106,
	KBDMAP_INDEX_U			= 0x0107,
	KBDMAP_INDEX_I			= 0x0108,
	KBDMAP_INDEX_O			= 0x0109,
	KBDMAP_INDEX_P			= 0x010A,
	KBDMAP_INDEX_LEFTBRACE		= 0x010B,
	KBDMAP_INDEX_RIGHTBRACE		= 0x010C,
	KBDMAP_INDEX_RETURN		= 0x010F,

	// ISO 9995 "C" row
	KBDMAP_INDEX_A			= 0x0201,
	KBDMAP_INDEX_S			= 0x0202,
	KBDMAP_INDEX_D			= 0x0203,
	KBDMAP_INDEX_F			= 0x0204,
	KBDMAP_INDEX_G			= 0x0205,
	KBDMAP_INDEX_H			= 0x0206,
	KBDMAP_INDEX_J			= 0x0207,
	KBDMAP_INDEX_K			= 0x0208,
	KBDMAP_INDEX_L			= 0x0209,
	KBDMAP_INDEX_SEMICOLON		= 0x020A,
	KBDMAP_INDEX_APOSTROPHE		= 0x020B,
	KBDMAP_INDEX_GRAVE		= 0x020C,
	KBDMAP_INDEX_EUROPE1		= 0x020D,	// U.S. \|, U.K. #~

	// ISO 9995 "B" row
	KBDMAP_INDEX_EUROPE2		= 0x0301,	// U.K. \|, Brazilian \|, Other <>
	KBDMAP_INDEX_Z			= 0x0302,
	KBDMAP_INDEX_X			= 0x0303,
	KBDMAP_INDEX_C			= 0x0304,
	KBDMAP_INDEX_V			= 0x0305,
	KBDMAP_INDEX_B			= 0x0306,
	KBDMAP_INDEX_N			= 0x0307,
	KBDMAP_INDEX_M			= 0x0308,
	KBDMAP_INDEX_COMMA		= 0x0309,
	KBDMAP_INDEX_DOT		= 0x030A,
	KBDMAP_INDEX_SLASH		= 0x030B,	// Brazilian ;:, Other /?
	KBDMAP_INDEX_ROMAJI		= 0x030C,	// Brazilian /?, Japanese "_ | \ Z"
	KBDMAP_INDEX_YEN		= 0x030D,	// Japanese Yen or overbar

	// Modifier row
	KBDMAP_INDEX_SHIFT1		= 0x0400,
	KBDMAP_INDEX_SHIFT2		= 0x0401,
	KBDMAP_INDEX_OPTION		= 0x0402,
	KBDMAP_INDEX_CONTROL1		= 0x0404,
	KBDMAP_INDEX_CONTROL2		= 0x0405,
	KBDMAP_INDEX_SUPER1		= 0x0406,
	KBDMAP_INDEX_SUPER2		= 0x0407,
	KBDMAP_INDEX_ALT		= 0x0408,
	KBDMAP_INDEX_CAPSLOCK		= 0x040C,
	KBDMAP_INDEX_SCROLLLOCK		= 0x040D,
	KBDMAP_INDEX_NUMLOCK		= 0x040E,

	// ISO 9995 "A" row
	KBDMAP_INDEX_KATAHIRA		= 0x0501,
	KBDMAP_INDEX_HALF_FULL_WIDTH	= 0x0502,	// zenkaku/hankaku
	KBDMAP_INDEX_HIRAGANA		= 0x0503,
	KBDMAP_INDEX_KATAKANA		= 0x0504,
	KBDMAP_INDEX_HENKAN		= 0x0505,
	KBDMAP_INDEX_MUHENKAN		= 0x0506,
	KBDMAP_INDEX_HANGUL_ENGLISH	= 0x0508,
	KBDMAP_INDEX_HANJA		= 0x0509,
	KBDMAP_INDEX_ALTERNATE_ERASE	= 0x050D,
	KBDMAP_INDEX_COMPOSE		= 0x050E,
	KBDMAP_INDEX_SPACE		= 0x050F,

	// Cursor/editing keypad
	KBDMAP_INDEX_HOME		= 0x0600,
	KBDMAP_INDEX_UP_ARROW		= 0x0601,
	KBDMAP_INDEX_PAGE_UP		= 0x0602,
	KBDMAP_INDEX_LEFT_ARROW		= 0x0603,
	KBDMAP_INDEX_RIGHT_ARROW	= 0x0604,
	KBDMAP_INDEX_END		= 0x0605,
	KBDMAP_INDEX_DOWN_ARROW		= 0x0606,
	KBDMAP_INDEX_PAGE_DOWN		= 0x0607,
	KBDMAP_INDEX_INSERT		= 0x0608,
	KBDMAP_INDEX_DELETE		= 0x0609,

	// Calculator keypad part 1
	KBDMAP_INDEX_KP_ASTERISK	= 0x0700,
	KBDMAP_INDEX_KP_7		= 0x0701,
	KBDMAP_INDEX_KP_8		= 0x0702,
	KBDMAP_INDEX_KP_9		= 0x0703,
	KBDMAP_INDEX_KP_MINUS		= 0x0704,
	KBDMAP_INDEX_KP_4		= 0x0705,
	KBDMAP_INDEX_KP_5		= 0x0706,
	KBDMAP_INDEX_KP_6		= 0x0707,
	KBDMAP_INDEX_KP_PLUS		= 0x0708,
	KBDMAP_INDEX_KP_1		= 0x0709,
	KBDMAP_INDEX_KP_2		= 0x070A,
	KBDMAP_INDEX_KP_3		= 0x070B,
	KBDMAP_INDEX_KP_0		= 0x070C,
	KBDMAP_INDEX_KP_DECIMAL		= 0x070D,
	KBDMAP_INDEX_KP_ENTER		= 0x070E,
	KBDMAP_INDEX_KP_SLASH		= 0x070F,

	// Calculator keypad part 2
	KBDMAP_INDEX_KP_THOUSANDS	= 0x0800,
	KBDMAP_INDEX_KP_JPCOMMA		= 0x0801,
	KBDMAP_INDEX_KP_EQUALS		= 0x0802,
	KBDMAP_INDEX_KP_AS400_EQUALS	= 0x0803,
	KBDMAP_INDEX_KP_SIGN		= 0x0804,
	KBDMAP_INDEX_KP_LBRACKET	= 0x0805,
	KBDMAP_INDEX_KP_RBRACKET	= 0x0806,
	KBDMAP_INDEX_KP_LBRACE		= 0x0807,
	KBDMAP_INDEX_KP_RBRACE		= 0x0808,

	// Function keypad part 1
	KBDMAP_INDEX_ESC		= 0x0900,
	KBDMAP_INDEX_F1			= 0x0901,
	KBDMAP_INDEX_F2			= 0x0902,
	KBDMAP_INDEX_F3			= 0x0903,
	KBDMAP_INDEX_F4			= 0x0904,
	KBDMAP_INDEX_F5			= 0x0905,
	KBDMAP_INDEX_F6			= 0x0906,
	KBDMAP_INDEX_F7			= 0x0907,
	KBDMAP_INDEX_F8			= 0x0908,
	KBDMAP_INDEX_F9			= 0x0909,
	KBDMAP_INDEX_F10		= 0x090A,
	KBDMAP_INDEX_F11		= 0x090B,
	KBDMAP_INDEX_F12		= 0x090C,
	KBDMAP_INDEX_F13		= 0x090D,
	KBDMAP_INDEX_F14		= 0x090E,
	KBDMAP_INDEX_F15		= 0x090F,

	// Function keypad part 2
	KBDMAP_INDEX_F16		= 0x0A00,
	KBDMAP_INDEX_F17		= 0x0A01,
	KBDMAP_INDEX_F18		= 0x0A02,
	KBDMAP_INDEX_F19		= 0x0A03,
	KBDMAP_INDEX_F20		= 0x0A04,
	KBDMAP_INDEX_F21		= 0x0A05,
	KBDMAP_INDEX_F22		= 0x0A06,
	KBDMAP_INDEX_F23		= 0x0A07,
	KBDMAP_INDEX_F24		= 0x0A08,

	// System commands keypad
	KBDMAP_INDEX_POWER		= 0x0D01,
	KBDMAP_INDEX_SLEEP		= 0x0D02,
	KBDMAP_INDEX_WAKE		= 0x0D03,
	KBDMAP_INDEX_DEBUG		= 0x0D04,

	// Application Commands keypad part 1
	KBDMAP_INDEX_PAUSE		= 0x0E00,
	KBDMAP_INDEX_NEXTPREV_TASK	= 0x0E01,
	KBDMAP_INDEX_ATTENTION		= 0x0E02,
	KBDMAP_INDEX_APPLICATION	= 0x0E03,
	KBDMAP_INDEX_BREAK		= 0x0E04,
	KBDMAP_INDEX_PRINT_SCREEN	= 0x0E05,
	KBDMAP_INDEX_MUTE		= 0x0E0D,
	KBDMAP_INDEX_VOLUME_UP		= 0x0E0E,
	KBDMAP_INDEX_VOLUME_DOWN	= 0x0E0F,

	// Application Commands keypad part 2
	KBDMAP_INDEX_EXECUTE		= 0x0F01,
	KBDMAP_INDEX_HELP		= 0x0F02,
	KBDMAP_INDEX_MENU		= 0x0F03,
	KBDMAP_INDEX_SELECT		= 0x0F04,
	KBDMAP_INDEX_CANCEL		= 0x0F05,
	KBDMAP_INDEX_CLEAR		= 0x0F06,
	KBDMAP_INDEX_PRIOR		= 0x0F07,
	KBDMAP_INDEX_APP_RETURN		= 0x0F08,
	KBDMAP_INDEX_SEPARATOR		= 0x0F09,
	KBDMAP_INDEX_OUT		= 0x0F0A,
	KBDMAP_INDEX_OPER		= 0x0F0B,
	KBDMAP_INDEX_CLEAR_OR_AGAIN	= 0x0F0C,
	KBDMAP_INDEX_EXSEL		= 0x0F0D,

	// Application Commands keypad part 3
	KBDMAP_INDEX_STOP		= 0x1001,
	KBDMAP_INDEX_AGAIN		= 0x1002,
	KBDMAP_INDEX_PROPERTIES		= 0x1003,
	KBDMAP_INDEX_UNDO		= 0x1004,
	KBDMAP_INDEX_REDO		= 0x1005,
	KBDMAP_INDEX_COPY		= 0x1006,
	KBDMAP_INDEX_OPEN		= 0x1007,
	KBDMAP_INDEX_PASTE		= 0x1008,
	KBDMAP_INDEX_FIND		= 0x1009,
	KBDMAP_INDEX_CUT		= 0x100A,

	// Consumer keypad part 1
	KBDMAP_INDEX_CALCULATOR		= 0x1100,
	KBDMAP_INDEX_FILE_MANAGER	= 0x1101,
	KBDMAP_INDEX_WWW		= 0x1102,
	KBDMAP_INDEX_HOME_PAGE		= 0x1103,
	KBDMAP_INDEX_REFRESH		= 0x1104,
	KBDMAP_INDEX_MAIL		= 0x1105,
	KBDMAP_INDEX_BOOKMARKS		= 0x1106,
	KBDMAP_INDEX_COMPUTER		= 0x1107,
	KBDMAP_INDEX_BACK		= 0x1108,
	KBDMAP_INDEX_FORWARD		= 0x1109,
	KBDMAP_INDEX_LOCK		= 0x110A,
	KBDMAP_INDEX_CLI		= 0x110B,
	KBDMAP_INDEX_NEXT_TRACK		= 0x110C,
	KBDMAP_INDEX_PREV_TRACK		= 0x110D,
	KBDMAP_INDEX_PLAY_PAUSE		= 0x110E,
	KBDMAP_INDEX_STOP_PLAYING	= 0x110F,

	// Consumer keypad part 2
	KBDMAP_INDEX_RECORD		= 0x1200,
	KBDMAP_INDEX_REWIND		= 0x1201,
	KBDMAP_INDEX_FAST_FORWARD	= 0x1202,
	KBDMAP_INDEX_EJECT		= 0x1203,
	KBDMAP_INDEX_NEW		= 0x1204,
	KBDMAP_INDEX_EXIT		= 0x1205,
};

#endif
