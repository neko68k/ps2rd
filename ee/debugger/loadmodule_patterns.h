/*
 * _sceSifLoadModule patterns
 *
 * Copyright (C) 2009-2010 jimmikaelkael <jimmikaelkael@wanadoo.fr>
 * Copyright (C) 2009-2010 Mathias Lafeldt <misfire@debugon.org>
 *
 * This file is part of PS2rd, the PS2 remote debugger.
 *
 * PS2rd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PS2rd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PS2rd.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 */

#ifndef _LOADMODULE_PATTERNS_H_
#define _LOADMODULE_PATTERNS_H_

#include <tamtypes.h>
#include "pattern.h"

static const u32 loadModulepattern0[] = {
	0x27bdff80,	// addiu sp, sp, $ff80
	0xffb40050,	// sd 	 s4, $0050(sp)
	0xffb30040,	// sd 	 s3, $0040(sp)
	0x00e0a02d,	// daddu s4, a3, zero
	0xffb20030,	// sd 	 s2, $0030(sp)
	0x0100982d,	// daddu s3, t0, zero
	0xffb10020,	// sd 	 s1, $0020(sp)
	0x00a0902d,	// daddu s2, a1, zero
	0xffb00010,	// sd 	 s0, $0010(sp)
	0x0080882d,	// daddu s1, a0, zero
	0xffbf0070,	// sd 	 ra, $0070(sp)
	0x00c0802d,	// daddu s0, a2, zero
	0x0c000000,	// jal	 _lf_bind
	0xffb50060	// sd 	 s5, $0060(sp)
};
static const u32 loadModulepattern0_mask[] = {
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xffffffff,
	0xfc000000,
	0xffffffff
};


static const pattern_t _loadmodule_patterns[] = {
        { loadModulepattern0, loadModulepattern0_mask, sizeof(loadModulepattern0), 0 },
        { NULL, NULL, 0, 0 }
};

#endif /* _LOADMODULE_PATTERNS_H_ */
