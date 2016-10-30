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

#ifndef MAIN_HDR
#define MAIN_HDR

#include "SDL.h"
#include "SDL_ttf.h"

#include "types.h"
#include "states.h"
#include "draw.h"
#include "maze.h"


/* These defines control debugging states. */
/*#define DEBUG_DIAGNOSTICS*/
/*#define DEBUG_ORIENT_CORNERS*/
/*#define DEBUG_SHOW_MAP*/

#define SCREEN_W  480
#define SCREEN_H  480
#define SCREEN_FLAGS  SDL_SWSURFACE
#define TITLE  "Cube Escape"
#define OPTION_TEXT_DIST_MULT  1.5
#define TARGET_FPS  40

#define DATA_DIR  "data/"
#define FONT_NAME  DATA_DIR "LiberationMono-Regular.ttf"
#define ICON_NAME  DATA_DIR "icon.png"

#define SPRITE_SIZE  64
#define NUM_BUTTONS  8
#define PLAYER_KEY_MOVE_TIME  80
#define PLAYER_MOUSE_MOVE_TIME  80

#define MAX_PSG_LENGTH  12
#define NUM_POINTS  8
#define NUM_QUADS  6
#define EXIT_PALETTE_TIME  1200

#define MIN_FULL_ZOOM  50.0f
#define MAX_FULL_ZOOM  1500.0f
#define FULL_ZOOM_START  500.0f
#define FULL_ZOOM_SPEED  20.0f
#define PLAYER_ZOOM_SPEED_DIV  30.0f

#define MAZE_ROTATE_TIME  200

#define ROOM_COLOR  black
#define WALL_COLOR  white
#define HIDE_COLOR  (dk_grey | alpha_mask)
#define UNHIDE_COLOR  black
#define UNHIDE_TIME  500
#define LEVEL_CHANGE_TIME  500

#define DEFAULT_EXIT_DISTANCE_CHOICE  2
#define DEFAULT_NUM_LEVELS  6
#define DEFAULT_START_LEVEL  2
#define DEFAULT_GRAPHICS_CHOICE  0
#define DEFAULT_SHOW_BUTTONS  0

#define MIN( x, y )  ((x) < (y) ? (x) : (y))
#define MAX( x, y )  ((x) > (y) ? (x) : (y))

#define LEVEL_SIZE( level )  ((level) > 0 ? (level) * 4 - 1 : 1)

#define CENTER_X_SCREEN( x )  (SCREEN_W / 2 - (x) / 2)
#define CENTER_Y_SCREEN( y )  (SCREEN_H / 2 - (y) / 2)


/* Globals section */

SDL_Surface *Screen,
            *icon_sfc,
            *maze_sfc,
            *fade_sfc,
            *prev_sfc,
            *player_sfc,
            *descend_txt_sfc,
            *ascend_txt_sfc,
            *exit_txt_sfc,
            *exit_up_sfc,
            *exit_dn_sfc,
            *exit_final_sfc,
            *button_sfcs[NUM_BUTTONS],
            **quid_sfcs,
            **quad_sfcs[NUM_QUADS],
            **hide_sfcs[NUM_QUADS];

SDL_Rect button_rects[NUM_BUTTONS],
         descend_rect,
         ascend_rect,
         exit_rect;

Uint32 white,
       grey,
       dk_grey,
       black,
       red,
       green,
       blue,
       med_red,
       med_green,
       med_blue,
       dk_red,
       dk_green,
       dk_blue,
       faded_blue,
       ckey,
       alpha_mask;

SDL_Color exit_up_pal[SPRITE_SIZE],
          exit_dn_pal[SPRITE_SIZE],
          exit_final_pal[SPRITE_SIZE],
          exit_up_pal_cpy[SPRITE_SIZE],
          exit_dn_pal_cpy[SPRITE_SIZE],
          exit_final_pal_cpy[SPRITE_SIZE],
          white_c,
          grey_c,
          dk_grey_c,
          black_c,
          green_c,
          mouse_c;

Player_t player;

SfcMazeRoom_t *exit_up_rooms,
              *exit_dn_rooms,
              exit_final_room;

Point_t restore_points[NUM_POINTS],
        points[NUM_POINTS],
        copy_points[NUM_POINTS],
        center;

SfcMazeConnect_t **connects[NUM_QUADS];

int maze_size,
    start_ticks,
    n_frames,
    tick_count,
    current_quad,
    exit_choice,
    total_num_levels,
    start_level,
    prev_num_levels,
    rotating,
    show_full_map,
    follow_player,
    playing,
    fast_graphics,
    fast_move,
    redraw_maze,
    pl_move_time,
    show_ctrl_buttons;

Sint16 quads[6][4];

int k_return,
    k_up,
    k_down,
    k_left,
    k_right,
    k_shift,
    k_ctrl,
    k_spc;

float player_zoom_speed,
      player_zoom,
      full_zoom,
      exit_mult;

Uint32 **unhide_starts[NUM_QUADS],
       rotation_start,
       fade_start,
       total_ticks;

int (*FunctionState)(),
    (*NextState)(),
    (*PreviousState)(),
    (*AffirmState)();

void *state_arg;

/* Function prototypes */

void InitGame( time_t seed );
void StartNewMaze( void );
void InitMazeStructure( int old_num_levels, int num_levels );
void ResetObjects( void );
int PickCurrentQuad( void );
void SetRestorePoints( void );
void RestorePoints( void );
SDL_Surface *NewSurface( int flags, SDL_PixelFormat *fmt,
                         Uint32 w, Uint32 h );
SDL_Surface *NewAlphaSurface( int flags, SDL_PixelFormat *fmt,
                              Uint32 w, Uint32 h );
SDL_Surface *CreateShadowedText( TTF_Font *fnt, const char *str, SDL_Color c );
SDL_Surface *CreateNewButton( int w, int h, const char *icon_name );
void RotatePoints( float *orig_dim1, float *orig_dim2,
                   float *copy_dim1, float *copy_dim2,
                   float center_dim1, float center_dim2, float angle_delta,
                   int rotate_original );
int RotateCube( int traversing );
void RotatePalette( SDL_Surface *sfc, SDL_Color *palette, SDL_Color *copy,
                    int size, Uint32 ticks );
void SetUnhideTicks( int quad, int x, int y, Uint32 ticks );
void SearchMaze( SfcMazeRoom_t *from );
void MovePlayerByMouse( int up_to_x, int up_to_y, int on_click );
void MovePlayerByKey( int dir );
int PlayerOnExit( SfcMazeRoom_t *exit );
void Quit( int code );

#endif  /* MAIN_HDR */
