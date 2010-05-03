/*
 * mycdvd.h - CDVD wrapper and helper functions
 *
 * Copyright (C) 2009 Mathias Lafeldt <misfire@debugon.org>
 *
 * This file is part of ps2rd, the PS2 remote debugger.
 *
 * ps2rd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ps2rd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ps2rd.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MYCDVD_H_
#define _MYCDVD_H_

#include <tamtypes.h>
#include <libcdvd.h>

#define CDVD_BLOCK	0
#define CDVD_NOBLOCK	1

#if 0
int cdOpenTray(void);
int cdCloseTray(void);
void cdToggleTray(void);
int cdIsPS2Game(void);
#endif
void _cdStandby(int mode);
void _cdStop(int mode);
int cdGetElf(char *elfname);
int cdRunElf(void);

#endif /* _MYCDVD_H_ */
