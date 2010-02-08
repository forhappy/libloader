#include "x86_opcode.h"
// ----------------- begin print tables --------------
static struct opcode_table_entry normal_insts[256] = {
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
				.size = 0x1,
			},
		},
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x8] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0x9] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0xa] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0xb] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0xc] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 0,
	},

	[0xd] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 0,
	},

	[0xe] = {
		.type = 1,
		.name = "pushcs",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf] = {
		.type = 37,
		.name = "ESCAPETWOBYTES",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x18] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x19] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x1a] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x1b] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x1c] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 0,
	},

	[0x1d] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 0,
	},

	[0x1e] = {
		.type = 1,
		.name = "pushds",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x1f] = {
		.type = 1,
		.name = "popds",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x26] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x27] = {
		.type = 1,
		.name = "daa",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x28] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x29] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x2a] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x2b] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x2c] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 0,
	},

	[0x2d] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 0,
	},

	[0x2e] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x2f] = {
		.type = 1,
		.name = "das",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x36] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x37] = {
		.type = 1,
		.name = "aaa",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x38] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},

	[0x39] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},

	[0x3a] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},

	[0x3b] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},

	[0x3c] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 0,
	},

	[0x3d] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 0,
	},

	[0x3e] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x3f] = {
		.type = 1,
		.name = "aas",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x48] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x49] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0x4a] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0x4b] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x3,
			},
		},
		.req_modrm = 0,
	},

	[0x4c] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x4,
			},
		},
		.req_modrm = 0,
	},

	[0x4d] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x5,
			},
		},
		.req_modrm = 0,
	},

	[0x4e] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x6,
			},
		},
		.req_modrm = 0,
	},

	[0x4f] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x7,
			},
		},
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x58] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x59] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0x5a] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0x5b] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x3,
			},
		},
		.req_modrm = 0,
	},

	[0x5c] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x4,
			},
		},
		.req_modrm = 0,
	},

	[0x5d] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x5,
			},
		},
		.req_modrm = 0,
	},

	[0x5e] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x6,
			},
		},
		.req_modrm = 0,
	},

	[0x5f] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x7,
			},
		},
		.req_modrm = 0,
	},

	[0x60] = {
		.type = 1,
		.name = "pusha",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x61] = {
		.type = 1,
		.name = "popa",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
	},

	[0x64] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x65] = {
		.type = 3,
		.name = "PREFIX2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x66] = {
		.type = 4,
		.name = "PREFIX3",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x67] = {
		.type = 5,
		.name = "PREFIX4",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x68] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
	},

	[0x69] = {
		.type = 1,
		.name = "imul",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x2000004,
			},
		},
		.req_modrm = 1,
	},

	[0x6a] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0x6b] = {
		.type = 1,
		.name = "imul",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x6c] = {
		.type = 1,
		.name = "insb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x13,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0x6d] = {
		.type = 1,
		.name = "ins",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x13,
				.size = 0x2000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0x6e] = {
		.type = 1,
		.name = "outs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0x6f] = {
		.type = 1,
		.name = "outs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x78] = {
		.type = 1,
		.name = "js",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x79] = {
		.type = 1,
		.name = "jns",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7a] = {
		.type = 1,
		.name = "jp",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7b] = {
		.type = 1,
		.name = "jnp",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7c] = {
		.type = 1,
		.name = "jl",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7d] = {
		.type = 1,
		.name = "jge",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7e] = {
		.type = 1,
		.name = "jle",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x7f] = {
		.type = 1,
		.name = "jg",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x80] = {
		.type = 7,
		.name = "GRP1_0x80",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x81] = {
		.type = 8,
		.name = "GRP1_0x81",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x82] = {
		.type = 9,
		.name = "GRP1_0x82",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x83] = {
		.type = 10,
		.name = "GRP1_0x83",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
	},

	[0x88] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},

	[0x89] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},

	[0x8a] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},

	[0x8b] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},

	[0x8c] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x10e,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x8d] = {
		.type = 1,
		.name = "lea",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x8e] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10e,
				.size = 0x2,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x8f] = {
		.type = 11,
		.name = "GRP1A_0x8f",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x90] = {
		.type = 1,
		.name = "nop",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0x98] = {
		.type = 1,
		.name = "cwtl",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x99] = {
		.type = 1,
		.name = "cltd",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x9a] = {
		.type = 1,
		.name = "callf",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x6400002,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x9b] = {
		.type = 1,
		.name = "fwait",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x9c] = {
		.type = 1,
		.name = "pushf",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x4,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0x9d] = {
		.type = 1,
		.name = "popf",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x4,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0x9e] = {
		.type = 1,
		.name = "sahf",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x9f] = {
		.type = 1,
		.name = "lahf",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0xa8] = {
		.type = 1,
		.name = "test",
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
		.req_modrm = 0,
	},

	[0xa9] = {
		.type = 1,
		.name = "test",
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
		.req_modrm = 0,
	},

	[0xaa] = {
		.type = 1,
		.name = "stosb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x13,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xab] = {
		.type = 1,
		.name = "stos",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x13,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xac] = {
		.type = 1,
		.name = "lodsb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0xad] = {
		.type = 1,
		.name = "lods",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xae] = {
		.type = 1,
		.name = "scasb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x13,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0xaf] = {
		.type = 1,
		.name = "scas",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0xb8] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xb9] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x1,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xba] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xbb] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x3,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xbc] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x4,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xbd] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x5,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xbe] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x6,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xbf] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0xc00,
				.size = 0x51000008,
				.reg_num = 0x7,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x6000004,
			},
		},
		.req_modrm = 0,
	},

	[0xc0] = {
		.type = 12,
		.name = "GRP2_0xc0",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xc1] = {
		.type = 13,
		.name = "GRP2_0xc1",
		.nr_operades = 0,
		.req_modrm = 0,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xc3] = {
		.type = 1,
		.name = "retn",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
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
		.req_modrm = 1,
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
		.req_modrm = 1,
	},

	[0xc6] = {
		.type = 28,
		.name = "GRP11_0xc6",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xc7] = {
		.type = 29,
		.name = "GRP11_0xc7",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xc8] = {
		.type = 1,
		.name = "enter",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x2,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
	},

	[0xc9] = {
		.type = 1,
		.name = "leave",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xca] = {
		.type = 1,
		.name = "lret",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x6,
				.size = 0x2,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xcb] = {
		.type = 1,
		.name = "lret",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xcc] = {
		.type = 1,
		.name = "int3",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xcd] = {
		.type = 1,
		.name = "int",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xce] = {
		.type = 1,
		.name = "into",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xcf] = {
		.type = 1,
		.name = "iret",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd0] = {
		.type = 14,
		.name = "GRP2_0xd0",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd1] = {
		.type = 15,
		.name = "GRP2_0xd1",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd2] = {
		.type = 16,
		.name = "GRP2_0xd2",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd3] = {
		.type = 17,
		.name = "GRP2_0xd3",
		.nr_operades = 0,
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0xd7] = {
		.type = 1,
		.name = "xlat",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd8] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd9] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xda] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xdb] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xdc] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xdd] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xde] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xdf] = {
		.type = 40,
		.name = "ESCAPECOPROCESSOR",
		.nr_operades = 0,
		.req_modrm = 0,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		},
		.req_modrm = 0,
		.jmpnote = 1,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
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
		.req_modrm = 0,
	},

	[0xe8] = {
		.type = 1,
		.name = "call",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xe9] = {
		.type = 1,
		.name = "jmp",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xea] = {
		.type = 1,
		.name = "ljmp",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x6400002,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xeb] = {
		.type = 1,
		.name = "jmp",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x1,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0xec] = {
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
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0xed] = {
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
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
		},
		.req_modrm = 0,
	},

	[0xee] = {
		.type = 1,
		.name = "out",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xef] = {
		.type = 1,
		.name = "out",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x400,
				.size = 0x1000002,
				.reg_num = 0x2,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x11000004,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xf0] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf2] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf3] = {
		.type = 2,
		.name = "PREFIX1",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf4] = {
		.type = 1,
		.name = "hlt",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf5] = {
		.type = 1,
		.name = "cmc",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf6] = {
		.type = 18,
		.name = "GRP3_0xf6",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf7] = {
		.type = 19,
		.name = "GRP3_0xf7",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf8] = {
		.type = 1,
		.name = "clc",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xf9] = {
		.type = 1,
		.name = "stc",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xfa] = {
		.type = 1,
		.name = "cli",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xfb] = {
		.type = 1,
		.name = "sti",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xfc] = {
		.type = 1,
		.name = "cld",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xfd] = {
		.type = 1,
		.name = "std",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xfe] = {
		.type = 20,
		.name = "GRP4_0xfe",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xff] = {
		.type = 21,
		.name = "GRP5_0xff",
		.nr_operades = 0,
		.req_modrm = 0,
	},

};

static struct opcode_table_entry twobytes_insts[256] = {
	[0x0] = {
		.type = 22,
		.name = "GRP6_0x0f00",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x1] = {
		.type = 36,
		.name = "XXXGRP7",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x2] = {
		.type = 1,
		.name = "lar",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "lsl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "syscall",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "clts",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x7] = {
		.type = 1,
		.name = "sysret",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x8] = {
		.type = 1,
		.name = "invd",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x9] = {
		.type = 1,
		.name = "wbinvd",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xd] = {
		.type = 1,
		.name = "nop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x10] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x1,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x3,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x4,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x2,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x11] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x5,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x7,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x8,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x6,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x12] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x9,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0xb,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0xc,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x13] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xd,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xe,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x14] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xf,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x10,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x15] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x11,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x12,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x16] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x13,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x14,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x15,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x17] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x16,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x17,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x18] = {
		.type = 34,
		.name = "GRP16_0x0f18",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x1f] = {
		.type = 1,
		.name = "nop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x20] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10d,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x101,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x21] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10d,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x102,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x22] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x101,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x10d,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x23] = {
		.type = 1,
		.name = "mov",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x102,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x10d,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x28] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x44,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x45,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x29] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x46,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x47,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2a] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x48,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x4a,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x4b,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x49,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2b] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x4c,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x4d,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2c] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x4e,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x4f,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2d] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x50,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x52,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x53,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x51,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2e] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x54,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x55,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x2f] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x56,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x57,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x30] = {
		.type = 1,
		.name = "wrmsr",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x31] = {
		.type = 1,
		.name = "rdtsc",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x32] = {
		.type = 1,
		.name = "rdmsr",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x33] = {
		.type = 1,
		.name = "rdpmc",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x34] = {
		.type = 1,
		.name = "sysenter",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x35] = {
		.type = 1,
		.name = "sysexit",
		.nr_operades = 0,
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x38] = {
		.type = 38,
		.name = "ESCAPETHREEBYTESA",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x3a] = {
		.type = 39,
		.name = "ESCAPETHREEBYTESB",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x40] = {
		.type = 1,
		.name = "cmovo",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x41] = {
		.type = 1,
		.name = "cmovno",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x42] = {
		.type = 1,
		.name = "cmovb",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x43] = {
		.type = 1,
		.name = "cmovae",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x44] = {
		.type = 1,
		.name = "cmove",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x45] = {
		.type = 1,
		.name = "cmovne",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x46] = {
		.type = 1,
		.name = "cmovbe",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x47] = {
		.type = 1,
		.name = "cmova",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x48] = {
		.type = 1,
		.name = "cmovs",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x49] = {
		.type = 1,
		.name = "cmovns",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4a] = {
		.type = 1,
		.name = "cmovp",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4b] = {
		.type = 1,
		.name = "cmovnp",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4c] = {
		.type = 1,
		.name = "cmovl",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4d] = {
		.type = 1,
		.name = "cmovge",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4e] = {
		.type = 1,
		.name = "cmovle",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4f] = {
		.type = 1,
		.name = "cmovg",
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
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x50] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x18,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x19,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x51] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x1a,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x1c,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x1d,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x1b,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x52] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x1e,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x1f,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x53] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x20,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x21,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x54] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x22,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x23,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x55] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x24,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x25,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x56] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x26,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x27,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x57] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x28,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x29,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x58] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x58,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x5a,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x5b,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x59,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x59] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x5c,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x5e,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x5f,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x5d,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5a] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x60,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x62,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x63,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x61,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5b] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x64,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x65,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x66,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5c] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x67,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x69,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x6a,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x68,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5d] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x6b,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x6d,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x6e,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x6c,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5e] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x6f,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x71,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x72,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x70,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x5f] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x73,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x75,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x76,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x74,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x60] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x2a,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x2b,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x61] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x2c,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x2d,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x62] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x2e,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x2f,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x63] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x30,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x31,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x64] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x32,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x33,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x65] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x34,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x35,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x66] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x36,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x37,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x67] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x38,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x39,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x68] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x77,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x78,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x69] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x79,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x7a,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6a] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x7b,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x7c,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6b] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x7d,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x7e,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6c] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x7f,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6d] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x80,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6e] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x81,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x82,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6f] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x83,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x84,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x85,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x70] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x3a,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x3b,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x3d,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x3c,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x71] = {
		.type = 30,
		.name = "GRP12_0x0f71",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x72] = {
		.type = 31,
		.name = "GRP13_0x0f72",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x73] = {
		.type = 32,
		.name = "GRP14_0x0f73",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x74] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x3e,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x3f,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x75] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x40,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x41,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x76] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x42,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x43,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x77] = {
		.type = 1,
		.name = "emms",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x78] = {
		.type = 1,
		.name = "vmread",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x79] = {
		.type = 1,
		.name = "vmwrite",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x7c] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x87,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x86,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x7d] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x89,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x88,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x7e] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x8a,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x8b,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x8c,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x7f] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x8d,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x8e,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x8f,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x80] = {
		.type = 1,
		.name = "jo",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x81] = {
		.type = 1,
		.name = "jno",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x82] = {
		.type = 1,
		.name = "jb",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x83] = {
		.type = 1,
		.name = "jnb",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x84] = {
		.type = 1,
		.name = "je",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x85] = {
		.type = 1,
		.name = "jne",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x86] = {
		.type = 1,
		.name = "jna",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x87] = {
		.type = 1,
		.name = "ja",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x7,
				.size = 0x2000004,
			},
		},
		.req_modrm = 0,
		.jmpnote = 1,
	},

	[0x90] = {
		.type = 1,
		.name = "seto",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x91] = {
		.type = 1,
		.name = "setno",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x92] = {
		.type = 1,
		.name = "setb",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x93] = {
		.type = 1,
		.name = "setnb",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x94] = {
		.type = 1,
		.name = "sete",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x95] = {
		.type = 1,
		.name = "setne",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x96] = {
		.type = 1,
		.name = "setna",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x97] = {
		.type = 1,
		.name = "seta",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa0] = {
		.type = 1,
		.name = "pushfs",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xa1] = {
		.type = 1,
		.name = "popfs",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xa2] = {
		.type = 1,
		.name = "cpuid",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0xa3] = {
		.type = 1,
		.name = "bt",
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
		.req_modrm = 1,
	},

	[0xa4] = {
		.type = 1,
		.name = "shld",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa5] = {
		.type = 1,
		.name = "shld",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[2] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xb0] = {
		.type = 1,
		.name = "cmpxchg",
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
		.req_modrm = 1,
	},

	[0xb1] = {
		.type = 1,
		.name = "cmpxchg",
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
		.req_modrm = 1,
	},

	[0xb2] = {
		.type = 1,
		.name = "lss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x6400002,
			},
		},
		.req_modrm = 1,
	},

	[0xb3] = {
		.type = 1,
		.name = "btr",
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
		.req_modrm = 1,
	},

	[0xb4] = {
		.type = 1,
		.name = "lfs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x6400002,
			},
		},
		.req_modrm = 1,
	},

	[0xb5] = {
		.type = 1,
		.name = "lgs",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x6400002,
			},
		},
		.req_modrm = 1,
	},

	[0xb6] = {
		.type = 1,
		.name = "movzx",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xb7] = {
		.type = 1,
		.name = "movzx",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0xc0] = {
		.type = 1,
		.name = "xadd",
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
		.req_modrm = 1,
	},

	[0xc1] = {
		.type = 1,
		.name = "xadd",
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
		.req_modrm = 1,
	},

	[0xc2] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x90,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x92,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x93,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x91,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xc3] = {
		.type = 1,
		.name = "movnti",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x105,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xc4] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x94,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x95,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xc5] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x96,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x97,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xc6] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x98,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x99,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0xc7] = {
		.type = 26,
		.name = "GRP9_0x0fc7",
		.nr_operades = 0,
		.req_modrm = 0,
	},

};

static struct opcode_table_entry threebytes_0f38_insts[256] = {
};

static struct opcode_table_entry threebytes_0f3a_insts[256] = {
};

static struct opcode_table_entry group_insts[28][8] = {
	[0] = {
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
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "adc",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "and",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "xor",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},


	},
	[1] = {
	[0x0] = {
		.type = 1,
		.name = "add",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "adc",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "and",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "xor",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},


	},
	[2] = {
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
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "adc",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "and",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "xor",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},


	},
	[3] = {
	[0x0] = {
		.type = 1,
		.name = "add",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "or",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "adc",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "sbb",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "and",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "sub",
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
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "xor",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "cmp",
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
		.req_modrm = 1,
	},


	},
	[4] = {
	[0x0] = {
		.type = 1,
		.name = "pop",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},


	},
	[5] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
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
		.req_modrm = 1,
	},


	},
	[6] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
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
		.req_modrm = 1,
	},


	},
	[7] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},


	},
	[8] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x201,
				.size = 0x10,
			},
		},
		.req_modrm = 1,
	},


	},
	[9] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
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
		.req_modrm = 1,
	},


	},
	[10] = {
	[0x0] = {
		.type = 1,
		.name = "rol",
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
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "ror",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "rcl",
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
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "rcr",
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
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "shl",
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
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "shr",
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
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "sar",
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
		.req_modrm = 1,
	},


	},
	[11] = {
	[0x0] = {
		.type = 1,
		.name = "test",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "not",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "neg",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "mul",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "imul",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "div",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "idiv",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x1000001,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},


	},
	[12] = {
	[0x0] = {
		.type = 1,
		.name = "test",
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
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "not",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "neg",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "mul",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "imul",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "div",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "idiv",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
			[1] = {
				.addressing = 0x400,
				.size = 0x51000008,
				.reg_num = 0x0,
			},
		},
		.req_modrm = 1,
	},


	},
	[13] = {
	[0x0] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},


	},
	[14] = {
	[0x0] = {
		.type = 1,
		.name = "inc",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "dec",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "calln",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "callf",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6400002,
			},
		},
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "jmpn",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "jmpf",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6400002,
			},
		},
		.req_modrm = 1,
		.jmpnote = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "push",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},


	},
	[15] = {
	[0x0] = {
		.type = 1,
		.name = "sldt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "str",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "lldt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "ltr",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "verr",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "verw",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},


	},
	[16] = {
	[0x0] = {
		.type = 1,
		.name = "sgdt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0xffff,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "sidt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0xffff,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "lgdt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0xffff,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "lidt",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0xffff,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "smsw",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x6000004,
			},
		},
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "lmsw",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x2,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "invlpg",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},


	},
	[17] = {
	[0x0] = {
		.type = 1,
		.name = "vmxxx",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x1] = {
		.type = 1,
		.name = "monitorormwait",
		.nr_operades = 0,
		.req_modrm = 0,
	},

	[0x2] = {
		.type = 1,
		.name = "xgetorsetbv",
		.nr_operades = 0,
		.req_modrm = 0,
	},


	},
	[18] = {

	},
	[19] = {
	[0x1] = {
		.type = 1,
		.name = "cmpxch8b",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x9a,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x9b,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x9c,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x7] = {
		.type = 1,
		.name = "vmptrst",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},


	},
	[20] = {

	},
	[21] = {
	[0x0] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},


	},
	[22] = {
	[0x0] = {
		.type = 1,
		.name = "mov",
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
		.req_modrm = 1,
	},


	},
	[23] = {
	[0x2] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x9d,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0x9e,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x4] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x9f,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa0,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xa1,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa2,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},


	},
	[24] = {
	[0x2] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xa3,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa4,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x4] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xa5,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa6,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xa7,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xa8,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},


	},
	[25] = {
	[0x2] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xa9,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xaa,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x3] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xab,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x6] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0xac,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xad,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},

	[0x7] = {
		.type = 41,
		.name = "SPECPREFIXINSTRUCTION",
		.nr_operades = 4,
		.operades = {
			[0] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[1] = {
				.addressing = 0xae,
				.size = 0x0,
			},
			[2] = {
				.addressing = 0x0,
				.size = 0x0,
			},
			[3] = {
				.addressing = 0x0,
				.size = 0x0,
			},
		},
		.req_modrm = 0,
	},


	},
	[26] = {

	},
	[27] = {
	[0x0] = {
		.type = 1,
		.name = "prefetch",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x1] = {
		.type = 1,
		.name = "prefetch",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "prefetch",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "prefetch",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},


	},
};
static struct opcode_table_entry spec_prefix_insts[175] = { 
	[0x1] = {
		.type = 1,
		.name = "movups",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x2] = {
		.type = 1,
		.name = "movss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x3] = {
		.type = 1,
		.name = "movupd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x4] = {
		.type = 1,
		.name = "movsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x5] = {
		.type = 1,
		.name = "movups",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x6] = {
		.type = 1,
		.name = "movss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x7] = {
		.type = 1,
		.name = "movupd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x8] = {
		.type = 1,
		.name = "movsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x9] = {
		.type = 1,
		.name = "movlpsormovhlps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xa] = {
		.type = 1,
		.name = "movlpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xb] = {
		.type = 1,
		.name = "movddup",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xc] = {
		.type = 1,
		.name = "movsldup",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xd] = {
		.type = 1,
		.name = "movlps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xe] = {
		.type = 1,
		.name = "movlpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0xf] = {
		.type = 1,
		.name = "unpcklps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x10] = {
		.type = 1,
		.name = "unpcklpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x11] = {
		.type = 1,
		.name = "unpckhps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x12] = {
		.type = 1,
		.name = "unpckhpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x13] = {
		.type = 1,
		.name = "movhpsormovlhps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x14] = {
		.type = 1,
		.name = "movhpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x15] = {
		.type = 1,
		.name = "movshdup",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x16] = {
		.type = 1,
		.name = "movhps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x17] = {
		.type = 1,
		.name = "movhpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x18] = {
		.type = 1,
		.name = "movmskps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x10f,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x19] = {
		.type = 1,
		.name = "movmskpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x10f,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x1a] = {
		.type = 1,
		.name = "sqrtps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x1b] = {
		.type = 1,
		.name = "sqrtss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x1c] = {
		.type = 1,
		.name = "sqrtpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x1d] = {
		.type = 1,
		.name = "sqrtsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x1e] = {
		.type = 1,
		.name = "rsqrtps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x1f] = {
		.type = 1,
		.name = "rsqrtss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x20] = {
		.type = 1,
		.name = "rcpps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x21] = {
		.type = 1,
		.name = "rcpss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x22] = {
		.type = 1,
		.name = "andps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x23] = {
		.type = 1,
		.name = "andpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x24] = {
		.type = 1,
		.name = "andnps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x25] = {
		.type = 1,
		.name = "andnpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x26] = {
		.type = 1,
		.name = "orps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x27] = {
		.type = 1,
		.name = "orpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x28] = {
		.type = 1,
		.name = "xorps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x29] = {
		.type = 1,
		.name = "xorpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x2a] = {
		.type = 1,
		.name = "punpcklbw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x2b] = {
		.type = 1,
		.name = "punpcklbw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x2c] = {
		.type = 1,
		.name = "punpcklwd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x2d] = {
		.type = 1,
		.name = "punpcklwd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x2e] = {
		.type = 1,
		.name = "punpckldq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x2f] = {
		.type = 1,
		.name = "punpckldq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x30] = {
		.type = 1,
		.name = "packsswb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x31] = {
		.type = 1,
		.name = "packsswb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x32] = {
		.type = 1,
		.name = "pcmpgtb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x33] = {
		.type = 1,
		.name = "pcmpgtb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x34] = {
		.type = 1,
		.name = "pcmpgtw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x35] = {
		.type = 1,
		.name = "pcmpgtw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x36] = {
		.type = 1,
		.name = "pcmpgtd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x37] = {
		.type = 1,
		.name = "pcmpgtd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x38] = {
		.type = 1,
		.name = "packuswb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x39] = {
		.type = 1,
		.name = "packuswb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x3a] = {
		.type = 1,
		.name = "pshufw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3b] = {
		.type = 1,
		.name = "pshufd",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3c] = {
		.type = 1,
		.name = "pshufhw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3d] = {
		.type = 1,
		.name = "pshuflw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x3e] = {
		.type = 1,
		.name = "pcmpeqb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x3f] = {
		.type = 1,
		.name = "pcmpeqb",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x40] = {
		.type = 1,
		.name = "pcmpeqw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x41] = {
		.type = 1,
		.name = "pcmpeqw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x42] = {
		.type = 1,
		.name = "pcmpeqd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x43] = {
		.type = 1,
		.name = "pcmpeqd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x44] = {
		.type = 1,
		.name = "movaps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x45] = {
		.type = 1,
		.name = "movapd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x46] = {
		.type = 1,
		.name = "movaps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x47] = {
		.type = 1,
		.name = "movapd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x48] = {
		.type = 1,
		.name = "cvtpi2ps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x100008,
			},
		},
		.req_modrm = 1,
	},

	[0x49] = {
		.type = 1,
		.name = "cvtsi2ss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x4a] = {
		.type = 1,
		.name = "cvtpi2pd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x100008,
			},
		},
		.req_modrm = 1,
	},

	[0x4b] = {
		.type = 1,
		.name = "cvtsi2sd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x4c] = {
		.type = 1,
		.name = "movntps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x4d] = {
		.type = 1,
		.name = "movntpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x4e] = {
		.type = 1,
		.name = "cvttps2pi",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x100008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x4f] = {
		.type = 1,
		.name = "cvttss2si",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x50] = {
		.type = 1,
		.name = "cvtps2pi",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x100008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x51] = {
		.type = 1,
		.name = "cvtss2si",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x52] = {
		.type = 1,
		.name = "cvtpd2pi",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10c,
				.size = 0x100008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x53] = {
		.type = 1,
		.name = "cvtsd2si",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x54] = {
		.type = 1,
		.name = "ucomiss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x55] = {
		.type = 1,
		.name = "ucomisd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x56] = {
		.type = 1,
		.name = "comiss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x57] = {
		.type = 1,
		.name = "comisd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x58] = {
		.type = 1,
		.name = "addps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x59] = {
		.type = 1,
		.name = "addss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x5a] = {
		.type = 1,
		.name = "addpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x5b] = {
		.type = 1,
		.name = "addsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x5c] = {
		.type = 1,
		.name = "mulps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x5d] = {
		.type = 1,
		.name = "mulss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x5e] = {
		.type = 1,
		.name = "mulpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x5f] = {
		.type = 1,
		.name = "mulsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x60] = {
		.type = 1,
		.name = "cvtps2pd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x61] = {
		.type = 1,
		.name = "cvtss2sd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x62] = {
		.type = 1,
		.name = "cvtpd2ps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x63] = {
		.type = 1,
		.name = "cvtsd2ss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x64] = {
		.type = 1,
		.name = "cvtdq2ps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x65] = {
		.type = 1,
		.name = "cvtps2dq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x66] = {
		.type = 1,
		.name = "cvttps2dq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x67] = {
		.type = 1,
		.name = "subps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x68] = {
		.type = 1,
		.name = "subss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x69] = {
		.type = 1,
		.name = "subpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x6a] = {
		.type = 1,
		.name = "subsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x6b] = {
		.type = 1,
		.name = "minps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x6c] = {
		.type = 1,
		.name = "minss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x6d] = {
		.type = 1,
		.name = "minpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x6e] = {
		.type = 1,
		.name = "minsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x6f] = {
		.type = 1,
		.name = "divps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x70] = {
		.type = 1,
		.name = "divss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x71] = {
		.type = 1,
		.name = "difpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x72] = {
		.type = 1,
		.name = "divsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x73] = {
		.type = 1,
		.name = "maxps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x74] = {
		.type = 1,
		.name = "maxss",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
		},
		.req_modrm = 1,
	},

	[0x75] = {
		.type = 1,
		.name = "maxpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x76] = {
		.type = 1,
		.name = "maxsd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
		},
		.req_modrm = 1,
	},

	[0x77] = {
		.type = 1,
		.name = "punpckhbw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x78] = {
		.type = 1,
		.name = "punpckhbw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x79] = {
		.type = 1,
		.name = "punpckhwd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x102,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x7a] = {
		.type = 1,
		.name = "punpckhwd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x7b] = {
		.type = 1,
		.name = "punpkchdq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x7c] = {
		.type = 1,
		.name = "punpckhdq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x7d] = {
		.type = 1,
		.name = "packssdw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x7e] = {
		.type = 1,
		.name = "packssdw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x7f] = {
		.type = 1,
		.name = "punpcklqdq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x80] = {
		.type = 1,
		.name = "punpckhqdq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x81] = {
		.type = 1,
		.name = "movdormovq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x82] = {
		.type = 1,
		.name = "movdormovq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x83] = {
		.type = 1,
		.name = "movdq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10c,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x84] = {
		.type = 1,
		.name = "movdqa",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x85] = {
		.type = 1,
		.name = "movdqu",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x86] = {
		.type = 1,
		.name = "haddps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x87] = {
		.type = 1,
		.name = "haddpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x88] = {
		.type = 1,
		.name = "hsubps",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
		},
		.req_modrm = 1,
	},

	[0x89] = {
		.type = 1,
		.name = "hsubpd",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
		},
		.req_modrm = 1,
	},

	[0x8a] = {
		.type = 1,
		.name = "movdormovq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10b,
				.size = 0x4,
			},
		},
		.req_modrm = 1,
	},

	[0x8b] = {
		.type = 1,
		.name = "movdormovq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x103,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x8c] = {
		.type = 1,
		.name = "movq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x8d] = {
		.type = 1,
		.name = "movq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10c,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x8e] = {
		.type = 1,
		.name = "movdqa",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x8f] = {
		.type = 1,
		.name = "movdqu",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x111,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x110,
				.size = 0x80,
			},
		},
		.req_modrm = 1,
	},

	[0x90] = {
		.type = 1,
		.name = "cmpps",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x91] = {
		.type = 1,
		.name = "cmpss",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800010,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x92] = {
		.type = 1,
		.name = "cmppd",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x93] = {
		.type = 1,
		.name = "cmpsd",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x800008,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x800008,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x94] = {
		.type = 1,
		.name = "pinsrw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x10b,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x95] = {
		.type = 1,
		.name = "pinsrw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x103,
				.size = 0x8,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x96] = {
		.type = 1,
		.name = "pextrw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x97] = {
		.type = 1,
		.name = "pextrw",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x105,
				.size = 0x4,
			},
			[1] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x98] = {
		.type = 1,
		.name = "shufps",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0x60010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0x60010,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x99] = {
		.type = 1,
		.name = "shufpd",
		.nr_operades = 3,
		.operades = {
			[0] = {
				.addressing = 0x110,
				.size = 0xa0010,
			},
			[1] = {
				.addressing = 0x111,
				.size = 0xa0010,
			},
			[2] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x9a] = {
		.type = 1,
		.name = "vmptrld",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x9b] = {
		.type = 1,
		.name = "vmclear",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x9c] = {
		.type = 1,
		.name = "vmxon",
		.nr_operades = 1,
		.operades = {
			[0] = {
				.addressing = 0x108,
				.size = 0x8,
			},
		},
		.req_modrm = 1,
	},

	[0x9d] = {
		.type = 1,
		.name = "psrlw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x9e] = {
		.type = 1,
		.name = "psrlw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0x9f] = {
		.type = 1,
		.name = "psraw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa0] = {
		.type = 1,
		.name = "psraw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa1] = {
		.type = 1,
		.name = "psllw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa2] = {
		.type = 1,
		.name = "psllw",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa3] = {
		.type = 1,
		.name = "psrld",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa4] = {
		.type = 1,
		.name = "psrld",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa5] = {
		.type = 1,
		.name = "psrad",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa6] = {
		.type = 1,
		.name = "psrad",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa7] = {
		.type = 1,
		.name = "pslld",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa8] = {
		.type = 1,
		.name = "pslld",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xa9] = {
		.type = 1,
		.name = "psrlq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xaa] = {
		.type = 1,
		.name = "psrlq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xab] = {
		.type = 1,
		.name = "psrldq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xac] = {
		.type = 1,
		.name = "psllq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x109,
				.size = 0x8,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xad] = {
		.type = 1,
		.name = "psllq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},

	[0xae] = {
		.type = 1,
		.name = "pslldq",
		.nr_operades = 2,
		.operades = {
			[0] = {
				.addressing = 0x10f,
				.size = 0x80,
			},
			[1] = {
				.addressing = 0x6,
				.size = 0x1,
			},
		},
		.req_modrm = 1,
	},


};
// vim:ts=4:sw=4
