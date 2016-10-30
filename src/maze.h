/* 
 * Copyright (C) 2009, 2013  Sean McKean
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAZE_HDR
#define MAZE_HDR

#include "main.h"


/* Function prototypes */

int CreateSurfaceMaze( int size, int level, int max_psg_count );
int CalcExits( SfcMazeRoom_t *start, int num_levels, float distance_mult );
int RecurseTowardsExit( int level, SfcMazeRoom_t *room, int from_dir,
                        int distance );

#endif  /* MAZE_HDR */
