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

#ifndef TYPES_HDR
#define TYPES_HDR


enum
{
    ROTATE_NONE,
    ROTATE_UP,
    ROTATE_DOWN,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    ROTATE_CWISE,
    ROTATE_CCWISE
} RotateDir;

enum
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    TURN_UP,
    TURN_DOWN,
    TURN_RIGHT,
    TURN_LEFT
} GeneralDirection;

enum
{
    NOT_TRAVERSING,
    TRAVERSE_CENTER,
    TRAVERSE_FOLLOW
} Traversal;

enum
{
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_CWISE,
    BUTTON_CCWISE,
    BUTTON_ZOOM_IN,
    BUTTON_ZOOM_OUT
} ButtonActions;

enum
{
    FAST_READY,
    FAST_PLAYER_MOVE,
    FAST_ROTATE_MAZE,
    FAST_FINISH_ROTATE
} FastMovement;

typedef struct
{
    float x;
    float y;
    float z;
} Point_t;

typedef struct
{
    int scr_x;
    int scr_y;
    Point_t tex;
} QuadEdge_t;

typedef struct
{
    int fr_x;  /* x and y coordinates to go from. */
    int fr_y;
    int to_qd;  /* Quad to connect fr_x and fr_y to. */
    int to_x;  /* x and y coordinates to connect to. */
    int to_y;
} SfcMazeConnect_t;

typedef struct
{
    int x;
    int y;
    int quad;
} SfcMazeRoom_t;

typedef struct
{
    SfcMazeRoom_t room;
    SfcMazeRoom_t to_room;
    int orient;
    int x_dir;
    int y_dir;
    int move_x;
    int move_y;
    int move_x_opp;
    int move_y_opp;
    int traversing;
    int level;
    /* Set to SDL_GetTicks() when player starts moving (for offsets). */
    Uint32 move_ticks_start;
} Player_t;

#endif  /* TYPES_HDR */
