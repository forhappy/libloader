#include "buildtable.h"
// ----------------- begin print tables --------------
struct instruction normal_insts[256] = {
	[0x0] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x1] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x2] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
	},

	[0x3] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
	},

	[0x4] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x5] = {
		.type = 1,
		.name = "add",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x6] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
	},

	[0x7] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
	},

	[0x10] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x11] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x12] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
	},

	[0x13] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
	},

	[0x14] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x15] = {
		.type = 1,
		.name = "adc",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x16] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
		},
	},

	[0x17] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
		},
	},

	[0x20] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x21] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x22] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
	},

	[0x23] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
	},

	[0x24] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x25] = {
		.type = 1,
		.name = "and",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x26] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
	},

	[0x27] = {
		.type = 1,
		.name = "daa",
		.nr_operades = 0,
	},

	[0x30] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x31] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x32] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
	},

	[0x33] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
	},

	[0x34] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x35] = {
		.type = 1,
		.name = "xor",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x36] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
	},

	[0x37] = {
		.type = 1,
		.name = "aaa",
		.nr_operades = 0,
	},

	[0x40] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x0,
			},
		},
	},

	[0x41] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x1,
			},
		},
	},

	[0x42] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x2,
			},
		},
	},

	[0x43] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x3,
			},
		},
	},

	[0x44] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x4,
			},
		},
	},

	[0x45] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x5,
			},
		},
	},

	[0x46] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x6,
			},
		},
	},

	[0x47] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x7,
			},
		},
	},

	[0x50] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
	},

	[0x51] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x1,
			},
		},
	},

	[0x52] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
		},
	},

	[0x53] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x3,
			},
		},
	},

	[0x54] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x4,
			},
		},
	},

	[0x55] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x5,
			},
		},
	},

	[0x56] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x6,
			},
		},
	},

	[0x57] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x7,
			},
		},
	},

	[0x60] = {
		.type = 1,
		.name = "pusha",
		.nr_operades = 0,
	},

	[0x61] = {
		.type = 1,
		.name = "popa",
		.nr_operades = 0,
	},

	[0x62] = {
		.type = 1,
		.name = "bound",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x600ffff,
			},
		},
	},

	[0x63] = {
		.type = 1,
		.name = "arpl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x2,
			},
		},
	},

	[0x64] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
	},

	[0x65] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
	},

	[0x66] = {
		.type = 4,
		.name = "PREFIX3",
		.nr_operades = 0,
	},

	[0x67] = {
		.type = 5,
		.name = "PREFIX4",
		.nr_operades = 0,
	},

	[0x70] = {
		.type = 1,
		.name = "jo",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x71] = {
		.type = 1,
		.name = "jno",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x72] = {
		.type = 1,
		.name = "jb",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x73] = {
		.type = 1,
		.name = "jae",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x74] = {
		.type = 1,
		.name = "je",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x75] = {
		.type = 1,
		.name = "jne",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x76] = {
		.type = 1,
		.name = "jbe",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x77] = {
		.type = 1,
		.name = "ja",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0x80] = {
		.type = 6,
		.name = "GRP1",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0x81] = {
		.type = 6,
		.name = "GRP1",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0x82] = {
		.type = 6,
		.name = "GRP1",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0x83] = {
		.type = 6,
		.name = "GRP1",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0x84] = {
		.type = 1,
		.name = "test",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x85] = {
		.type = 1,
		.name = "test",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x86] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x1,
			},
		},
	},

	[0x87] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
		},
	},

	[0x90] = {
		.type = 1,
		.name = "nop",
		.nr_operades = 0,
	},

	[0x91] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x1,
			},
		},
	},

	[0x92] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
		},
	},

	[0x93] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x3,
			},
		},
	},

	[0x94] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x4,
			},
		},
	},

	[0x95] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x5,
			},
		},
	},

	[0x96] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x6,
			},
		},
	},

	[0x97] = {
		.type = 1,
		.name = "xchg",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x7,
			},
		},
	},

	[0xa0] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xa,
				.size = 0x1,
			},
		},
	},

	[0xa1] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0xa,
				.size = 0x6000004,
			},
		},
	},

	[0xa2] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xa,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
	},

	[0xa3] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xa,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
	},

	[0xa4] = {
		.type = 1,
		.name = "movs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x12,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x13,
				.size = 0x1,
			},
		},
	},

	[0xa5] = {
		.type = 1,
		.name = "movs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x12,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x13,
				.size = 0x6000004,
			},
		},
	},

	[0xa6] = {
		.type = 1,
		.name = "cmps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x12,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x13,
				.size = 0x1,
			},
		},
	},

	[0xa7] = {
		.type = 1,
		.name = "cmps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x12,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x13,
				.size = 0x6000004,
			},
		},
	},

	[0xb0] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb1] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb2] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb3] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x3,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb4] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x4,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb5] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x5,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb6] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x6,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xb7] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x1000001,
				.reg_num = 0x7,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xc0] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xc1] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xc2] = {
		.type = 1,
		.name = "retn",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x2,
			},
		.jmpnote = 1,
		},
	},

	[0xc3] = {
		.type = 1,
		.name = "retn",
		.nr_operades = 0,
	},

	[0xc4] = {
		.type = 1,
		.name = "les",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x2000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x6400002,
			},
		},
	},

	[0xc5] = {
		.type = 1,
		.name = "lds",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x2000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x6400002,
			},
		},
	},

	[0xc6] = {
		.type = 17,
		.name = "GRP11",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xc7] = {
		.type = 17,
		.name = "GRP11",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
	},

	[0xd0] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x8000001,
				.size = 0x8065730,
			},
		},
	},

	[0xd1] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x8000001,
				.size = 0x8065580,
			},
		},
	},

	[0xd2] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x1,
			},
		},
	},

	[0xd3] = {
		.type = 8,
		.name = "GRP2",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x1,
			},
		},
	},

	[0xd4] = {
		.type = 1,
		.name = "amm",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xd5] = {
		.type = 1,
		.name = "aad",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xd7] = {
		.type = 1,
		.name = "xlat",
		.nr_operades = 0,
	},

	[0xe0] = {
		.type = 1,
		.name = "loopne",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0xe1] = {
		.type = 1,
		.name = "loope",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0xe2] = {
		.type = 1,
		.name = "loop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0xe3] = {
		.type = 1,
		.name = "jecxz",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		.jmpnote = 1,
		},
	},

	[0xe4] = {
		.type = 1,
		.name = "in",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xe5] = {
		.type = 1,
		.name = "in",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
	},

	[0xe6] = {
		.type = 1,
		.name = "out",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
	},

	[0xe7] = {
		.type = 1,
		.name = "out",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x0,
			},
		},
	},

	[0xf0] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
	},

	[0xf2] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
	},

	[0xf3] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
	},

	[0xf4] = {
		.type = 1,
		.name = "hlt",
		.nr_operades = 0,
	},

	[0xf5] = {
		.type = 1,
		.name = "cmc",
		.nr_operades = 0,
	},

	[0xf6] = {
		.type = 9,
		.name = "GRP3",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
	},

	[0xf7] = {
		.type = 9,
		.name = "GRP3",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
	},

};

struct instruction twobytes_insts[256] = {
};

struct instruction threebytes_0f38_insts[256] = {
};

struct instruction threebytes_0f3a_insts[256] = {
};

// vim:ts=4:sw=4
