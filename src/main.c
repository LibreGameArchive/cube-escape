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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

#include "main.h"


const float hf_pi = M_PI / 2.0f;


void InitGame( time_t seed )
{
    TTF_Font *fnt = NULL;
    SDL_PixelFormat eight_bit_fmt,
                    *fmt = NULL;
    SDL_Rect dst_rect;
    SDL_Color descend_fg = { 63, 63, 255 },
              ascend_fg = { 63, 255, 63 },
              exit_fg = { 255, 63, 63 };
    SDL_Event evt;
    int quad = 0,
        x = 0,
        y = 0;

    srand(seed);

    putenv("SDL_VIDEO_WINDOW_POS=center");
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        Quit(1);
    }
    SDL_WM_SetCaption(TITLE, TITLE);
    icon_sfc = IMG_Load(ICON_NAME);
    SDL_WM_SetIcon(icon_sfc, NULL);
    Screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SCREEN_FLAGS);
    if (Screen == NULL)
    {
        fprintf( stderr, "Error initializing framebuffer: %s\n",
                 SDL_GetError() );
        Quit(1);
    }
    fmt = Screen->format;

    if (TTF_Init())
    {
        fprintf( stderr, "Error initializing framebuffer: %s\n",
                 TTF_GetError() );
        Quit(3);
    }

    white = SDL_MapRGB(fmt, 255, 255, 255);
    SDL_GetRGB(white, fmt, &white_c.r, &white_c.g, &white_c.b);
    grey = SDL_MapRGB(fmt, 127, 127, 127);
    SDL_GetRGB(grey, fmt, &grey_c.r, &grey_c.g, &grey_c.b);
    dk_grey = SDL_MapRGB(fmt, 63, 63, 63);
    SDL_GetRGB(dk_grey, fmt, &dk_grey_c.r, &dk_grey_c.g, &dk_grey_c.b);
    black = SDL_MapRGB(fmt, 0, 0, 0);
    SDL_GetRGB(black, fmt, &black_c.r, &black_c.g, &black_c.b);
    red = SDL_MapRGB(fmt, 255, 0, 0);
    green = SDL_MapRGB(fmt, 0, 255, 0);
    SDL_GetRGB(green, fmt, &green_c.r, &green_c.g, &green_c.b);
    blue = SDL_MapRGB(fmt, 0, 0, 255);
    med_red = SDL_MapRGB(fmt, 127, 0, 0);
    med_green = SDL_MapRGB(fmt, 0, 127, 0);
    med_blue = SDL_MapRGB(fmt, 0, 0, 127);
    dk_red = SDL_MapRGB(fmt, 63, 0, 0);
    dk_green = SDL_MapRGB(fmt, 0, 63, 0);
    dk_blue = SDL_MapRGB(fmt, 0, 0, 63);
    faded_blue = SDL_MapRGB(fmt, 31, 31, 63);
    SDL_GetRGB(faded_blue, fmt, &mouse_c.r, &mouse_c.g, &mouse_c.b);
    ckey = SDL_MapRGB(fmt, 255, 255, 0);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    alpha_mask = 0x000000ff;
#else
    alpha_mask = 0xff000000;
#endif

    player_sfc = NewSurface(SCREEN_FLAGS, fmt, SPRITE_SIZE, SPRITE_SIZE);
    DrawFilledGradientCircle(player_sfc, SPRITE_SIZE, 0, 0, green, dk_green);

    exit_up_sfc = SDL_CreateRGBSurface( SCREEN_FLAGS,
                                        SPRITE_SIZE, SPRITE_SIZE, 8,
                                        0, 0, 0, 0 );
    exit_dn_sfc = SDL_CreateRGBSurface( SCREEN_FLAGS,
                                        SPRITE_SIZE, SPRITE_SIZE, 8,
                                        0, 0, 0, 0 );
    exit_final_sfc = SDL_CreateRGBSurface( SCREEN_FLAGS,
                                           SPRITE_SIZE, SPRITE_SIZE, 8,
                                           0, 0, 0, 0 );
    for (x = 0; x < SPRITE_SIZE / 2; ++x)
    {
        dst_rect.x = dst_rect.y = x;
        dst_rect.w = dst_rect.h = SPRITE_SIZE - x * 2;
        SDL_FillRect(exit_up_sfc, &dst_rect, x);
        SDL_FillRect(exit_dn_sfc, &dst_rect, x);
        SDL_FillRect(exit_final_sfc, &dst_rect, x);
    }
    for (x = 0; x < SPRITE_SIZE; ++x)
    {
        if (x < SPRITE_SIZE / 2)
        {
            exit_up_pal[x].r = 64 * 2 * x / SPRITE_SIZE;
            exit_up_pal[x].g = 256 * 2 * x / SPRITE_SIZE;
            exit_up_pal[x].b = 64 * 2 * x / SPRITE_SIZE;
        }
        else
        {
            exit_up_pal[x].r = 127 - 64 * 2 * x / SPRITE_SIZE;
            exit_up_pal[x].g = 255 - 256 * 2 * x / SPRITE_SIZE;
            exit_up_pal[x].b = 127 - 64 * 2 * x / SPRITE_SIZE;
        }

        if (x < SPRITE_SIZE / 2)
        {
            exit_dn_pal[x].r = 64 * 2 * x / SPRITE_SIZE;
            exit_dn_pal[x].g = 64 * 2 * x / SPRITE_SIZE;
            exit_dn_pal[x].b = 256 * 2 * x / SPRITE_SIZE;
        }
        else
        {
            exit_dn_pal[x].r = 127 - 64 * 2 * x / SPRITE_SIZE;
            exit_dn_pal[x].g = 127 - 64 * 2 * x / SPRITE_SIZE;
            exit_dn_pal[x].b = 255 - 256 * 2 * x / SPRITE_SIZE;
        }

        if (x < SPRITE_SIZE / 2)
        {
            exit_final_pal[x].r = 256 * 2 * x / SPRITE_SIZE;
            exit_final_pal[x].g = 64 * 2 * x / SPRITE_SIZE;
            exit_final_pal[x].b = 64 * 2 * x / SPRITE_SIZE;
        }
        else
        {
            exit_final_pal[x].r = 255 - 256 * 2 * x / SPRITE_SIZE;
            exit_final_pal[x].g = 127 - 64 * 2 * x / SPRITE_SIZE;
            exit_final_pal[x].b = 127 - 64 * 2 * x / SPRITE_SIZE;
        }
    }
    SDL_SetColors(exit_up_sfc, exit_up_pal, 0, SPRITE_SIZE);
    SDL_SetColors(exit_dn_sfc, exit_dn_pal, 0, SPRITE_SIZE);
    SDL_SetColors(exit_final_sfc, exit_final_pal, 0, SPRITE_SIZE);

    fade_sfc = SDL_DisplayFormat(Screen);
    SDL_FillRect(fade_sfc, NULL, black);
    SDL_SetAlpha(fade_sfc, SDL_SRCALPHA, 0);

    prev_sfc = SDL_DisplayFormat(Screen);
    SDL_FillRect(prev_sfc, NULL, black);

    button_sfcs[BUTTON_UP] = CreateNewButton(72, 24, "up-arrow.png");
    button_rects[BUTTON_UP].x = CENTER_X_SCREEN(button_sfcs[BUTTON_UP]->w);
    button_rects[BUTTON_UP].y = 0;
    button_rects[BUTTON_UP].w = button_sfcs[BUTTON_UP]->w;
    button_rects[BUTTON_UP].h = button_sfcs[BUTTON_UP]->h;

    button_sfcs[BUTTON_DOWN] = CreateNewButton(72, 24, "down-arrow.png");
    button_rects[BUTTON_DOWN].x = CENTER_X_SCREEN(button_sfcs[BUTTON_DOWN]->w);
    button_rects[BUTTON_DOWN].y = SCREEN_H - button_sfcs[BUTTON_DOWN]->h;
    button_rects[BUTTON_DOWN].w = button_sfcs[BUTTON_DOWN]->w;
    button_rects[BUTTON_DOWN].h = button_sfcs[BUTTON_DOWN]->h;

    button_sfcs[BUTTON_LEFT] = CreateNewButton(24, 72, "left-arrow.png");
    button_rects[BUTTON_LEFT].x = 0;
    button_rects[BUTTON_LEFT].y = CENTER_Y_SCREEN(button_sfcs[BUTTON_DOWN]->h);
    button_rects[BUTTON_LEFT].w = button_sfcs[BUTTON_LEFT]->w;
    button_rects[BUTTON_LEFT].h = button_sfcs[BUTTON_LEFT]->h;

    button_sfcs[BUTTON_RIGHT] = CreateNewButton(24, 72, "right-arrow.png");
    button_rects[BUTTON_RIGHT].x = SCREEN_W - button_sfcs[BUTTON_RIGHT]->w;
    button_rects[BUTTON_RIGHT].y =
        CENTER_Y_SCREEN(button_sfcs[BUTTON_RIGHT]->h);
    button_rects[BUTTON_RIGHT].w = button_sfcs[BUTTON_RIGHT]->w;
    button_rects[BUTTON_RIGHT].h = button_sfcs[BUTTON_RIGHT]->h;

    button_sfcs[BUTTON_CWISE] = CreateNewButton(40, 40, "turn-cwise.png");
    button_rects[BUTTON_CWISE].x = SCREEN_W - button_sfcs[BUTTON_CWISE]->w;
    button_rects[BUTTON_CWISE].y = 0;
    button_rects[BUTTON_CWISE].w = button_sfcs[BUTTON_CWISE]->w;
    button_rects[BUTTON_CWISE].h = button_sfcs[BUTTON_CWISE]->h;

    button_sfcs[BUTTON_CCWISE] = CreateNewButton(40, 40, "turn-ccwise.png");
    button_rects[BUTTON_CCWISE].x = 0;
    button_rects[BUTTON_CCWISE].y = 0;
    button_rects[BUTTON_CCWISE].w = button_sfcs[BUTTON_CCWISE]->w;
    button_rects[BUTTON_CCWISE].h = button_sfcs[BUTTON_CCWISE]->h;

    button_sfcs[BUTTON_ZOOM_IN] = CreateNewButton(40, 40, "zoom-in.png");
    button_rects[BUTTON_ZOOM_IN].x =
        SCREEN_W - button_sfcs[BUTTON_ZOOM_IN]->w;
    button_rects[BUTTON_ZOOM_IN].y =
        SCREEN_H - button_sfcs[BUTTON_ZOOM_IN]->h;
    button_rects[BUTTON_ZOOM_IN].w = button_sfcs[BUTTON_ZOOM_IN]->w;
    button_rects[BUTTON_ZOOM_IN].h = button_sfcs[BUTTON_ZOOM_IN]->h;

    button_sfcs[BUTTON_ZOOM_OUT] = CreateNewButton(40, 40, "zoom-out.png");
    button_rects[BUTTON_ZOOM_OUT].x =
        SCREEN_W - button_sfcs[BUTTON_ZOOM_OUT]->w * 2 + 1;
    button_rects[BUTTON_ZOOM_OUT].y =
        SCREEN_H - button_sfcs[BUTTON_ZOOM_OUT]->h;
    button_rects[BUTTON_ZOOM_OUT].w = button_sfcs[BUTTON_ZOOM_OUT]->w;
    button_rects[BUTTON_ZOOM_OUT].h = button_sfcs[BUTTON_ZOOM_OUT]->h;

    fnt = TTF_OpenFont(FONT_NAME, 24);

    descend_txt_sfc = CreateShadowedText( fnt, "Press 'd' to descend a level",
                                          descend_fg );
    descend_rect.x = CENTER_X_SCREEN(descend_txt_sfc->w);
    descend_rect.y =
        SCREEN_H - button_sfcs[BUTTON_ZOOM_IN]->h - descend_txt_sfc->h - 4;
    ascend_txt_sfc = CreateShadowedText( fnt, "Press 'a' to ascend a level",
                                         ascend_fg );
    ascend_rect.x = CENTER_X_SCREEN(ascend_txt_sfc->w);
    ascend_rect.y =
        SCREEN_H - button_sfcs[BUTTON_ZOOM_IN]->h - ascend_txt_sfc->h - 4;
    exit_txt_sfc = CreateShadowedText( fnt, "Press 'a' to exit the maze",
                                       exit_fg );
    exit_rect.x = CENTER_X_SCREEN(exit_txt_sfc->w);
    exit_rect.y =
        SCREEN_H - button_sfcs[BUTTON_ZOOM_IN]->h - exit_txt_sfc->h - 4;

    TTF_CloseFont(fnt);

#ifdef DEBUG_SHOW_MAP
    show_full_map = 1;
#else
    show_full_map = 0;
#endif

    k_return = 0;
    k_up = 0;
    k_down = 0;
    k_left = 0;
    k_right = 0;
    k_shift = 0;
    k_ctrl = 0;
    k_spc = 0;

    n_frames = 0;
    total_ticks = 0;

    playing = 0;
    total_num_levels = prev_num_levels = 0;

    quid_sfcs = NULL;
    for (quad = 0; quad < NUM_QUADS; ++quad)
    {
        quad_sfcs[quad] = NULL;
        hide_sfcs[quad] = NULL;
        unhide_starts[quad] = NULL;
        connects[quad] = NULL;
    }

    maze_sfc = NewSurface(SCREEN_FLAGS, fmt, SCREEN_W, SCREEN_H);

    fade_start = 0;
    current_quad = 0;
    pl_move_time = 0;

    exit_choice = DEFAULT_EXIT_DISTANCE_CHOICE;
    total_num_levels = DEFAULT_NUM_LEVELS;
    start_level = DEFAULT_START_LEVEL;
    fast_graphics = DEFAULT_GRAPHICS_CHOICE;
    show_ctrl_buttons = DEFAULT_SHOW_BUTTONS;
}


void StartNewMaze()
{
    InitMazeStructure(prev_num_levels, total_num_levels);
    prev_num_levels = total_num_levels;
    ResetObjects();
    NextState = GamePlay;
}


void InitMazeStructure( int old_num_levels, int num_levels )
{
    SfcMazeRoom_t start;
    int num_connects = 0,
        level = 0,
        quad = 0,
        size = 0,
        i = 0,
        x = 0,
        y = 0;

    if (old_num_levels > 0)
    {
        for (level = 0; level < old_num_levels; ++level)
        {
            if (quid_sfcs)
                SDL_FreeSurface(quid_sfcs[level]);
            for (quad = 0; quad < NUM_QUADS; ++quad)
            {
                if (quad_sfcs[quad])
                    SDL_FreeSurface(quad_sfcs[quad][level]);
                if (hide_sfcs[quad])
                    SDL_FreeSurface(hide_sfcs[quad][level]);
                if (unhide_starts[quad])
                    free(unhide_starts[quad][level]);
                if (connects[quad])
                    free(connects[quad][level]);
            }
        }
        if (quid_sfcs)
            free(quid_sfcs);
        for (quad = 0; quad < NUM_QUADS; ++quad)
        {
            if (quad_sfcs[quad])
                free(quad_sfcs[quad]);
            if (hide_sfcs[quad])
                free(hide_sfcs[quad]);
            if (unhide_starts[quad])
                free(unhide_starts[quad]);
            if (connects[quad])
                free(connects[quad]);
        }
    }

    quid_sfcs = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * num_levels);
    for (quad = 0; quad < NUM_QUADS; ++quad)
    {
        quad_sfcs[quad] = (SDL_Surface **)
            malloc(sizeof(SDL_Surface *) * num_levels);
        hide_sfcs[quad] = (SDL_Surface **)
            malloc(sizeof(SDL_Surface *) * num_levels);
        unhide_starts[quad] = (Uint32 **)
            malloc(sizeof(Uint32 *) * num_levels);
        connects[quad] = (SfcMazeConnect_t **)
            malloc(sizeof(SfcMazeConnect_t *) * num_levels);
    }

    for (level = 0; level < num_levels; ++level)
    {
        size = LEVEL_SIZE(level);
        quid_sfcs[level] = NewSurface( SCREEN_FLAGS, Screen->format,
                                       size, size );
        for (quad = 0; quad < NUM_QUADS; ++quad)
        {
            quad_sfcs[quad][level] =
                NewSurface(SCREEN_FLAGS, Screen->format, size, size);
            hide_sfcs[quad][level] =
                NewAlphaSurface(SCREEN_FLAGS, Screen->format, size, size);
            SDL_FillRect(hide_sfcs[quad][level], NULL, HIDE_COLOR);
            unhide_starts[quad][level] =
                (Uint32 *)malloc(size * size * sizeof(Uint32));
            for (y = 0; y < size; ++y)
                for (x = 0; x < size; ++x)
                    unhide_starts[quad][level][x + y * size] = 0;
            if (size == 1)
                num_connects = 1;
            else
                num_connects = size / 2 * 4;
            connects[quad][level] = (SfcMazeConnect_t *)
                malloc(sizeof(SfcMazeConnect_t) * num_connects);
        }
    }

    for (quad = 0; quad < 6; ++quad)
        SDL_FillRect(quad_sfcs[quad][0], NULL, WALL_COLOR);
    SDL_FillRect(quad_sfcs[1][0], NULL, HIDE_COLOR);

    for (level = 1; level < num_levels; ++level)
    {
        size = LEVEL_SIZE(level);
        CreateSurfaceMaze(size, level, MAX_PSG_LENGTH);
    }

    player.level = 0;
    player.orient = UP;
    player.x_dir = 0;
    player.y_dir = 0;
    player.traversing = NOT_TRAVERSING;

    follow_player = 0;

    if (exit_up_rooms)
        free(exit_up_rooms);
    if (exit_dn_rooms)
        free(exit_dn_rooms);
    exit_up_rooms = (SfcMazeRoom_t *)
        malloc(sizeof(SfcMazeRoom_t) * num_levels);
    exit_dn_rooms = (SfcMazeRoom_t *)
        malloc(sizeof(SfcMazeRoom_t) * num_levels);
    start.x = 1;
    start.y = 1;
    start.quad = 0;
    CalcExits(&start, num_levels, exit_mult);

    full_zoom = FULL_ZOOM_START;

    player_zoom = SPRITE_SIZE * 8;
    player_zoom_speed = player_zoom / PLAYER_ZOOM_SPEED_DIV;

    redraw_maze = 1;

    points[0].x = -1.0f;  points[0].y = -1.0f;  points[0].z = +5.0f;
    points[1].x = +1.0f;  points[1].y = -1.0f;  points[1].z = +5.0f;
    points[2].x = +1.0f;  points[2].y = +1.0f;  points[2].z = +5.0f;
    points[3].x = -1.0f;  points[3].y = +1.0f;  points[3].z = +5.0f;
    points[4].x = -1.0f;  points[4].y = -1.0f;  points[4].z = +7.0f;
    points[5].x = +1.0f;  points[5].y = -1.0f;  points[5].z = +7.0f;
    points[6].x = +1.0f;  points[6].y = +1.0f;  points[6].z = +7.0f;
    points[7].x = -1.0f;  points[7].y = +1.0f;  points[7].z = +7.0f;
    for (i = 0; i < NUM_POINTS; ++i)
    {
        copy_points[i].x = points[i].x;
        copy_points[i].y = points[i].y;
        copy_points[i].z = points[i].z;
    }

    center.x = 0.0f;  center.y = 0.0f;  center.z = 6.0f;

    quads[0][0] = 0;  quads[0][1] = 1;  quads[0][2] = 2;  quads[0][3] = 3;
    quads[1][0] = 7;  quads[1][1] = 6;  quads[1][2] = 5;  quads[1][3] = 4;
    quads[2][0] = 5;  quads[2][1] = 1;  quads[2][2] = 0;  quads[2][3] = 4;
    quads[3][0] = 7;  quads[3][1] = 3;  quads[3][2] = 2;  quads[3][3] = 6;
    quads[4][0] = 2;  quads[4][1] = 1;  quads[4][2] = 5;  quads[4][3] = 6;
    quads[5][0] = 7;  quads[5][1] = 4;  quads[5][2] = 0;  quads[5][3] = 3;

    player.level = start_level - 1;
    maze_size = player.level * 4 - 1;

    fast_move = FAST_READY;

    if (maze_size == -1)
    {
        player.room.x = player.to_room.x = 0;
        player.room.y = player.to_room.y = 0;
        player.room.quad = player.to_room.quad = 1;
        player.orient = UP;
    }
    else
    {
        player.room.x = player.to_room.x = exit_dn_rooms[player.level].x;
        player.room.y = player.to_room.y = exit_dn_rooms[player.level].y;
        player.room.quad = player.to_room.quad = current_quad =
            exit_dn_rooms[player.level].quad;
        for (i = 0; i < NUM_POINTS; ++i)
        {
            switch (player.room.quad)
            {
                case 0:
                    /* No rotation needed. */
                    break;

                case 1:
                    RotatePoints( &points[i].y, &points[i].z,
                                  &copy_points[i].y, &copy_points[i].z,
                                  center.y, center.z, (float)M_PI, 1 );
                    break;

                case 2:
                    RotatePoints( &points[i].y, &points[i].z,
                                  &copy_points[i].y, &copy_points[i].z,
                                  center.y, center.z, (float)-M_PI / 2.0f, 1 );
                    break;

                case 3:
                    RotatePoints( &points[i].y, &points[i].z,
                                  &copy_points[i].y, &copy_points[i].z,
                                  center.y, center.z, (float)M_PI / 2.0f, 1 );
                    break;

                case 4:
                    RotatePoints( &points[i].x, &points[i].z,
                                  &copy_points[i].x, &copy_points[i].z,
                                  center.x, center.z, (float)M_PI / 2.0f, 1 );
                    break;

                case 5:
                    RotatePoints( &points[i].x, &points[i].z,
                                  &copy_points[i].x, &copy_points[i].z,
                                  center.x, center.z, (float)-M_PI / 2.0f, 1 );
                    break;

            }
        }
        switch (player.room.quad)
        {
            case 0:
                player.orient = UP;
                break;

            case 1:
                player.orient = UP;
                break;

            case 2:
                player.orient = LEFT;
                break;

            case 3:
                player.orient = RIGHT;
                break;

            case 4:
                player.orient = RIGHT;
                break;

            case 5:
                player.orient = RIGHT;
                break;

        }
    }

    RotatePalette(exit_up_sfc, exit_up_pal, exit_up_pal_cpy, SPRITE_SIZE, 0);
    RotatePalette(exit_dn_sfc, exit_dn_pal, exit_dn_pal_cpy, SPRITE_SIZE, 0);
    RotatePalette( exit_final_sfc, exit_final_pal, exit_final_pal_cpy,
                   SPRITE_SIZE, 0 );

    SetRestorePoints();
}


void ResetObjects()
{
    if (maze_size > -1)
    {
        player_zoom_speed = player_zoom / PLAYER_ZOOM_SPEED_DIV;
        player.to_room.quad = current_quad = player.room.quad;
    }
    player.move_ticks_start = 0;
    player.traversing = NOT_TRAVERSING;

    rotation_start = 0;
    rotating = ROTATE_NONE;

    SearchMaze(&player.room);
}


int PickCurrentQuad()
{
    int total = 0,
        quad = 0,
        i = 0;

    /* Pick quad facing camera as current quad. */
    for (quad = 0; quad < NUM_QUADS; ++quad)
    {
        total = 0;
        for (i = 0; i < 4; ++i)
            if (points[quads[quad][i]].z < center.z)
                total++;
        if (total == 4)
            return quad;
    }
}


void SetRestorePoints()
{
    int i = 0;

    for (i = 0; i < NUM_POINTS; ++i)
    {
        restore_points[i].x = points[i].x;
        restore_points[i].y = points[i].y;
        restore_points[i].z = points[i].z;
    }
}


void RestorePoints()
{
    int i = 0;

    for (i = 0; i < NUM_POINTS; ++i)
    {
        points[i].x = copy_points[i].x = restore_points[i].x;
        points[i].y = copy_points[i].y = restore_points[i].y;
        points[i].z = copy_points[i].z = restore_points[i].z;
    }
    current_quad = player.room.quad;
    redraw_maze = 1;
}


SDL_Surface *NewSurface( int flags, SDL_PixelFormat *fmt,
                         Uint32 w, Uint32 h )
{
    SDL_Surface *return_sfc = NULL;

    return_sfc = SDL_CreateRGBSurface( flags, w, h, fmt->BitsPerPixel,
                                       fmt->Rmask, fmt->Gmask, fmt->Bmask, 0 );

    if (return_sfc == NULL)
    {
        fprintf(stderr, "Unable to create new SDL_Surface\n");
        Quit(2);
    }

    return return_sfc;
}


SDL_Surface *NewAlphaSurface( int flags, SDL_PixelFormat *fmt,
                              Uint32 w, Uint32 h )
{
    SDL_Surface *return_sfc = NULL;

    return_sfc = SDL_CreateRGBSurface( flags, w, h, fmt->BitsPerPixel,
                                       fmt->Rmask, fmt->Gmask,
                                       fmt->Bmask, alpha_mask );

    if (return_sfc == NULL)
    {
        fprintf(stderr, "Unable to create new SDL_Surface\n");
        Quit(2);
    }

    return return_sfc;
}


SDL_Surface *CreateShadowedText( TTF_Font *fnt, const char *str, SDL_Color c )
{
    SDL_Surface *sfc = NULL,
                *txt = NULL;
    SDL_Rect dst_rect;
    int x = 0,
        y = 0;

    txt = TTF_RenderText_Solid(fnt, str, black_c);
    sfc = NewSurface(SCREEN_FLAGS, Screen->format, txt->w + 4, txt->h + 4);
    SDL_FillRect(sfc, NULL, ckey);
    SDL_SetColorKey(sfc, SDL_SRCCOLORKEY, ckey);
    for (y = 0; y <= 4; ++y)
        for (x = 0; x <= 4; ++x)
        {
            dst_rect.x = x;
            dst_rect.y = y;
            SDL_BlitSurface(txt, NULL, sfc, &dst_rect);
        }
    SDL_FreeSurface(txt);
    txt = TTF_RenderText_Solid(fnt, str, c);
    SDL_SetColorKey(txt, SDL_SRCCOLORKEY, 0);
    dst_rect.x = 2;
    dst_rect.y = 2;
    SDL_BlitSurface(txt, NULL, sfc, &dst_rect);
    SDL_FreeSurface(txt);

    return sfc;
}


SDL_Surface *CreateNewButton( int w, int h, const char *img_file )
{
    SDL_Surface *temp_sfc = NULL,
                *img_sfc = NULL,
                *sfc = NULL;
    SDL_PixelFormat fmt;
    SDL_Rect rect;
    int start_x = 0,
        start_y = 0,
        x = 0,
        y = 0;
    char path_name[80] = DATA_DIR;

    fmt.BitsPerPixel = 8;
    fmt.Rmask = 0;  fmt.Gmask = 0;  fmt.Bmask = 0;  fmt.Amask = 0;

    sfc = NewSurface(SCREEN_FLAGS, &fmt, w, h);

    SDL_FillRect(sfc, NULL, 0);
    rect.x = 0;  rect.y = 0;  rect.w = w;  rect.h = 1;
    SDL_FillRect(sfc, &rect, 1);
    rect.x = 0;  rect.y = 0;  rect.w = 1;  rect.h = h;
    SDL_FillRect(sfc, &rect, 1);
    rect.x = 0;  rect.y = h - 1;  rect.w = w;  rect.h = 1;
    SDL_FillRect(sfc, &rect, 1);
    rect.x = w - 1;  rect.y = 0;  rect.w = 1;  rect.h = h;
    SDL_FillRect(sfc, &rect, 1);

    strcat(path_name, img_file);
    temp_sfc = IMG_Load(path_name);
    img_sfc = SDL_DisplayFormat(temp_sfc);
    SDL_FreeSurface(temp_sfc);

    start_x = sfc->w / 2 - img_sfc->w / 2;
    start_y = sfc->h / 2 - img_sfc->h / 2;
    for (y = 0; y < img_sfc->h; ++y)
        for (x = 0; x < img_sfc->w; ++x)
        {
            if (GetPixel_32(img_sfc, x, y) == white)
                SetPixel_8(sfc, start_x + x, start_y + y, 2);
            else if (GetPixel_32(img_sfc, x, y) == green)
                SetPixel_8(sfc, start_x + x, start_y + y, 3);
        }
    SDL_SetColors(sfc, &black_c, 0, 1);
    SDL_SetColors(sfc, &grey_c, 1, 1);
    SDL_SetColors(sfc, &white_c, 2, 1);
    SDL_SetColors(sfc, &green_c, 3, 1);

    SDL_FreeSurface(img_sfc);

    return sfc;
}


void RotatePoints( float *orig_dim1, float *orig_dim2,
                   float *copy_dim1, float *copy_dim2,
                   float center_dim1, float center_dim2, float angle_delta,
                   int rotate_original )
{
    float a = *orig_dim1 - center_dim1,
          b = *orig_dim2 - center_dim2,
          angle = atan2(*orig_dim1 - center_dim1, *orig_dim2 - center_dim2),
          dist = sqrt(a * a + b * b);

    *copy_dim1 = dist * sin(angle + angle_delta) + center_dim1;
    *copy_dim2 = dist * cos(angle + angle_delta) + center_dim2;
    if (rotate_original)
    {
        *orig_dim1 = *copy_dim1;
        *orig_dim2 = *copy_dim2;
    }
}


int RotateCube( int traversing )
{
    int rotate_original = 0,
        i = 0;
    float angle = hf_pi * (SDL_GetTicks() - rotation_start) / MAZE_ROTATE_TIME;

    redraw_maze = 1;

    if (traversing != TRAVERSE_FOLLOW)
    {
        if (angle >= hf_pi)
        {
            angle = hf_pi;
            rotate_original = 1;
        }
        else
            rotate_original = 0;
    }
    else
    {
        if ( fast_graphics ||
             (float)(SDL_GetTicks() - player.move_ticks_start) /
             PLAYER_KEY_MOVE_TIME > 0.5f )
        {
            angle = hf_pi;
            rotate_original = 1;
        }
        else
        {
            angle = 0.0f;
            rotate_original = 0;
        }
    }

    for (i = 0; i < NUM_POINTS; ++i)
        switch (rotating)
        {
            case ROTATE_UP:
                RotatePoints( &points[i].y, &points[i].z,
                              &copy_points[i].y, &copy_points[i].z,
                              center.y, center.z, +angle, rotate_original );
                break;

            case ROTATE_DOWN:
                RotatePoints( &points[i].y, &points[i].z,
                              &copy_points[i].y, &copy_points[i].z,
                              center.y, center.z, -angle, rotate_original );
                break;

            case ROTATE_LEFT:
                RotatePoints( &points[i].x, &points[i].z,
                              &copy_points[i].x, &copy_points[i].z,
                              center.x, center.z, +angle, rotate_original );
                break;

            case ROTATE_RIGHT:
                RotatePoints( &points[i].x, &points[i].z,
                              &copy_points[i].x, &copy_points[i].z,
                              center.x, center.z, -angle, rotate_original );
                break;

            case ROTATE_CCWISE:
                RotatePoints( &points[i].x, &points[i].y,
                              &copy_points[i].x, &copy_points[i].y,
                              center.x, center.y, +angle, rotate_original );
                break;

            case ROTATE_CWISE:
                RotatePoints( &points[i].x, &points[i].y,
                              &copy_points[i].x, &copy_points[i].y,
                              center.x, center.y, -angle, rotate_original );
                break;

            case ROTATE_NONE:
                return -1;

        }

    /* Finish rotation of cube by making faces orthogonal to camera. */
    if (rotate_original)
    {
        for (i = 0; i < NUM_POINTS; ++i)
        {
            if (points[i].x < center.x)
                points[i].x = copy_points[i].x = center.x - 1.0f;
            else
                points[i].x = copy_points[i].x = center.x + 1.0f;

            if (points[i].y < center.y)
                points[i].y = copy_points[i].y = center.y - 1.0f;
            else
                points[i].y = copy_points[i].y = center.y + 1.0f;

            if (points[i].z < center.z)
                points[i].z = copy_points[i].z = center.z - 1.0f;
            else
                points[i].z = copy_points[i].z = center.z + 1.0f;
        }
        current_quad = PickCurrentQuad();
        if (player.to_room.quad == current_quad)
        {
            /* Set player orientation correctly. */
            if ( points[quads[current_quad][0]].y < center.y &&
                 points[quads[current_quad][1]].y < center.y )
                player.orient = UP;
            else if ( points[quads[current_quad][1]].y < center.y &&
                      points[quads[current_quad][2]].y < center.y )
                player.orient = RIGHT;
            else if ( points[quads[current_quad][2]].y < center.y &&
                      points[quads[current_quad][3]].y < center.y )
                player.orient = DOWN;
            else
                player.orient = LEFT;

            SetRestorePoints();
        }

        return 1;
    }

    return 0;
}


void RotatePalette( SDL_Surface *sfc, SDL_Color *palette, SDL_Color *copy,
                    int size, Uint32 ticks )
{
    size_t sz_color = sizeof(SDL_Color);
    int offset = 0,
        end = 0;

    offset = (ticks % EXIT_PALETTE_TIME) * size / EXIT_PALETTE_TIME;
    end = size - offset - 1;

    memcpy(copy, palette + end, sz_color * offset);
    memcpy(copy + offset, palette, sz_color * end);

    SDL_SetColors(sfc, copy, 0, SPRITE_SIZE / 2);
}


void SetUnhideTicks( int quad, int x, int y, Uint32 ticks )
{
    Uint32 *set = &unhide_starts[quad][player.level][x + y * maze_size];
    if (*set == 0)
        *set = ticks;
}


void SearchMaze( SfcMazeRoom_t *from )
{
    int finished = 0,
        quad = from->quad,
        x = 0,
        y = 0;
    Uint32 ticks = SDL_GetTicks();

    if (ticks == 0)
        ticks = 1;

    finished = 0;
    for (x = from->x, y = from->y; x < maze_size && !finished; ++x)
    {
        if (GetPixel_32(quad_sfcs[quad][player.level], x, y) == WALL_COLOR)
            finished = 1;
        SetUnhideTicks(quad, x, y, ticks);
        if (y > 0)
            SetUnhideTicks(quad, x, y - 1, ticks);
        if (y < maze_size - 1)
            SetUnhideTicks(quad, x, y + 1, ticks);
    }

    finished = 0;
    for (x = from->x, y = from->y; x >= 0 && !finished; --x)
    {
        if (GetPixel_32(quad_sfcs[quad][player.level], x, y) == WALL_COLOR)
            finished = 1;
        SetUnhideTicks(quad, x, y, ticks);
        if (y > 0)
            SetUnhideTicks(quad, x, y - 1, ticks);
        if (y < maze_size - 1)
            SetUnhideTicks(quad, x, y + 1, ticks);
    }

    finished = 0;
    for (x = from->x, y = from->y; y < maze_size && !finished; ++y)
    {
        if (GetPixel_32(quad_sfcs[quad][player.level], x, y) == WALL_COLOR)
            finished = 1;
        SetUnhideTicks(quad, x, y, ticks);
        if (x > 0)
            SetUnhideTicks(quad, x - 1, y, ticks);
        if (x < maze_size - 1)
            SetUnhideTicks(quad, x + 1, y, ticks);
    }

    finished = 0;
    for (x = from->x, y = from->y; y >= 0 && !finished; --y)
    {
        if (GetPixel_32(quad_sfcs[quad][player.level], x, y) == WALL_COLOR)
            finished = 1;
        SetUnhideTicks(quad, x, y, ticks);
        if (x > 0)
            SetUnhideTicks(quad, x - 1, y, ticks);
        if (x < maze_size - 1)
            SetUnhideTicks(quad, x + 1, y, ticks);
    }
}


void MovePlayerByMouse( int to_x, int to_y, int on_click )
{
    if (player.room.x == to_x && player.room.y == to_y)
    {
        if (on_click)
        {
            if (to_y == 0)
            {
                switch (player.orient)
                {
                    case UP:
                        MovePlayerByKey(UP);
                        break;

                    case DOWN:
                        MovePlayerByKey(DOWN);
                        break;

                    case LEFT:
                        MovePlayerByKey(RIGHT);
                        break;

                    case RIGHT:
                        MovePlayerByKey(LEFT);
                        break;

                }
                if (fast_graphics)
                    fast_move = FAST_PLAYER_MOVE;
            }
            else if (to_y == maze_size - 1)
            {
                switch (player.orient)
                {
                    case UP:
                        MovePlayerByKey(DOWN);
                        break;

                    case DOWN:
                        MovePlayerByKey(UP);
                        break;

                    case LEFT:
                        MovePlayerByKey(LEFT);
                        break;

                    case RIGHT:
                        MovePlayerByKey(RIGHT);
                        break;

                }
                if (fast_graphics)
                    fast_move = FAST_PLAYER_MOVE;
            }
            else if (to_x == 0)
            {
                switch (player.orient)
                {
                    case UP:
                        MovePlayerByKey(LEFT);
                        break;

                    case DOWN:
                        MovePlayerByKey(RIGHT);
                        break;

                    case LEFT:
                        MovePlayerByKey(UP);
                        break;

                    case RIGHT:
                        MovePlayerByKey(DOWN);
                        break;

                }
                if (fast_graphics)
                    fast_move = FAST_PLAYER_MOVE;
            }
            else if (to_x == maze_size - 1)
            {
                switch (player.orient)
                {
                    case UP:
                        MovePlayerByKey(RIGHT);
                        break;

                    case DOWN:
                        MovePlayerByKey(LEFT);
                        break;

                    case LEFT:
                        MovePlayerByKey(DOWN);
                        break;

                    case RIGHT:
                        MovePlayerByKey(UP);
                        break;

                }
                if (fast_graphics)
                    fast_move = FAST_PLAYER_MOVE;
            }
        }

        return;
    }

    player.traversing = NOT_TRAVERSING;
    if ( to_x < player.room.x &&
         GetPixel_32( quad_sfcs[player.room.quad][player.level],
                      player.room.x - 1, player.room.y ) == ROOM_COLOR )
    {
        player.x_dir = -1;
        player.y_dir = 0;
        player.to_room.x = player.room.x + player.x_dir;
        player.to_room.y = player.room.y + player.y_dir;
        player.to_room.quad = player.room.quad;
        player.move_ticks_start = MAX(1, SDL_GetTicks());
        pl_move_time = PLAYER_MOUSE_MOVE_TIME;
        if (fast_graphics)
            fast_move = FAST_PLAYER_MOVE;
        SearchMaze(&player.to_room);
    }
    else if ( to_y < player.room.y &&
              GetPixel_32( quad_sfcs[player.room.quad][player.level],
                           player.room.x, player.room.y - 1 ) == ROOM_COLOR )
    {
        player.x_dir = 0;
        player.y_dir = -1;
        player.to_room.x = player.room.x + player.x_dir;
        player.to_room.y = player.room.y + player.y_dir;
        player.to_room.quad = player.room.quad;
        player.move_ticks_start = MAX(1, SDL_GetTicks());
        pl_move_time = PLAYER_MOUSE_MOVE_TIME;
        if (fast_graphics)
            fast_move = FAST_PLAYER_MOVE;
        SearchMaze(&player.to_room);
    }
    else if ( to_x > player.room.x &&
              GetPixel_32( quad_sfcs[player.room.quad][player.level],
                           player.room.x + 1, player.room.y ) == ROOM_COLOR )
    {
        player.x_dir = 1;
        player.y_dir = 0;
        player.to_room.x = player.room.x + player.x_dir;
        player.to_room.y = player.room.y + player.y_dir;
        player.to_room.quad = player.room.quad;
        player.move_ticks_start = MAX(1, SDL_GetTicks());
        pl_move_time = PLAYER_MOUSE_MOVE_TIME;
        if (fast_graphics)
            fast_move = FAST_PLAYER_MOVE;
        SearchMaze(&player.to_room);
    }
    else if ( to_y > player.room.y &&
              GetPixel_32( quad_sfcs[player.room.quad][player.level],
                           player.room.x, player.room.y + 1 ) == ROOM_COLOR )
    {
        player.x_dir = 0;
        player.y_dir = 1;
        player.to_room.x = player.room.x + player.x_dir;
        player.to_room.y = player.room.y + player.y_dir;
        player.to_room.quad = player.room.quad;
        player.move_ticks_start = MAX(1, SDL_GetTicks());
        pl_move_time = PLAYER_MOUSE_MOVE_TIME;
        if (fast_graphics)
            fast_move = FAST_PLAYER_MOVE;
        SearchMaze(&player.to_room);
    }
}


void MovePlayerByKey( int dir )
{
    int over_edge = 0,
        half_sz = maze_size / 2,
        i = 0,
        x = 0,
        y = 0;

    player.x_dir = 0;
    player.y_dir = 0;

    switch (dir)
    {
        case UP:
            if (player.orient == UP)
                player.y_dir = -1;
            else if (player.orient == RIGHT)
                player.x_dir = +1;
            else if (player.orient == DOWN)
                player.y_dir = +1;
            else if (player.orient == LEFT)
                player.x_dir = -1;
            break;

        case RIGHT:
            if (player.orient == UP)
                player.x_dir = +1;
            else if (player.orient == RIGHT)
                player.y_dir = +1;
            else if (player.orient == DOWN)
                player.x_dir = -1;
            else if (player.orient == LEFT)
                player.y_dir = -1;
            break;

        case DOWN:
            if (player.orient == UP)
                player.y_dir = +1;
            else if (player.orient == RIGHT)
                player.x_dir = -1;
            else if (player.orient == DOWN)
                player.y_dir = -1;
            else if (player.orient == LEFT)
                player.x_dir = +1;
            break;

        case LEFT:
            if (player.orient == UP)
                player.x_dir = -1;
            else if (player.orient == RIGHT)
                player.y_dir = -1;
            else if (player.orient == DOWN)
                player.x_dir = +1;
            else if (player.orient == LEFT)
                player.y_dir = +1;
            break;

    }

    if (player.room.x == 0 && player.x_dir == -1)
        over_edge = LEFT;
    else if (player.room.x == maze_size - 1 && player.x_dir == +1)
        over_edge = RIGHT;
    else if (player.room.y == 0 && player.y_dir == -1)
        over_edge = UP;
    else if (player.room.y == maze_size - 1 && player.y_dir == +1)
        over_edge = DOWN;
    else
    {
        /* Move does not traverse an edge of the cube. */
        player.traversing = NOT_TRAVERSING;
        if ( GetPixel_32( quad_sfcs[player.room.quad][player.level],
                          player.room.x + player.x_dir,
                          player.room.y + player.y_dir ) == ROOM_COLOR )

        {
            player.move_ticks_start = MAX(1, SDL_GetTicks());
            player.to_room.x = player.room.x + player.x_dir;
            player.to_room.y = player.room.y + player.y_dir;
            player.to_room.quad = player.room.quad;
            pl_move_time = PLAYER_KEY_MOVE_TIME;
            SearchMaze(&player.to_room);
        }
        else
        {
            player.x_dir = 0;
            player.y_dir = 0;
        }
        return;
    }

    for (i = 0; i < half_sz * 4; ++i)
    {
        x = connects[current_quad][player.level][i].fr_x;
        y = connects[current_quad][player.level][i].fr_y;
        if (player.room.x == x && player.room.y == y)
            break;
    }

    /* Traverse the edge. */
    switch (over_edge)
    {
        case UP:
            switch (player.orient)
            {
                case UP:
                    rotating = ROTATE_DOWN;
                    break;

                case RIGHT:
                    rotating = ROTATE_RIGHT;
                    break;

                case DOWN:
                    rotating = ROTATE_UP;
                    break;

                case LEFT:
                    rotating = ROTATE_LEFT;
                    break;

            }
            break;

        case RIGHT:
            switch (player.orient)
            {
                case UP:
                    rotating = ROTATE_LEFT;
                    break;

                case RIGHT:
                    rotating = ROTATE_DOWN;
                    break;

                case DOWN:
                    rotating = ROTATE_RIGHT;
                    break;

                case LEFT:
                    rotating = ROTATE_UP;
                    break;

            }
            break;

        case DOWN:
            switch (player.orient)
            {
                case UP:
                    rotating = ROTATE_UP;
                    break;

                case RIGHT:
                    rotating = ROTATE_LEFT;
                    break;

                case DOWN:
                    rotating = ROTATE_DOWN;
                    break;

                case LEFT:
                    rotating = ROTATE_RIGHT;
                    break;

            }
            break;

        case LEFT:
            switch (player.orient)
            {
                case UP:
                    rotating = ROTATE_RIGHT;
                    break;

                case RIGHT:
                    rotating = ROTATE_UP;
                    break;

                case DOWN:
                    rotating = ROTATE_LEFT;
                    break;

                case LEFT:
                    rotating = ROTATE_DOWN;
                    break;

            }
            break;

    }

    player.to_room.x = connects[current_quad][player.level][i].to_x;
    player.to_room.y = connects[current_quad][player.level][i].to_y;
    player.to_room.quad = connects[current_quad][player.level][i].to_qd;
    player.move_ticks_start = MAX(1, SDL_GetTicks());
    if (follow_player)
        player.traversing = TRAVERSE_FOLLOW;
    else
        player.traversing = TRAVERSE_CENTER;
    rotation_start = MAX(1, SDL_GetTicks());
    pl_move_time = PLAYER_KEY_MOVE_TIME;
    SearchMaze(&player.to_room);
}


int PlayerOnExit( SfcMazeRoom_t *exit )
{
    if ( player.room.quad == exit->quad &&
         player.room.x == exit->x && player.room.y == exit->y )
        return 1;

    return 0;
}


void Quit( int code )
{
    int level = 0,
        quad = 0,
        i = 0;

    for (level = 0; level < total_num_levels; ++level)
    {
        if (quid_sfcs)
            SDL_FreeSurface(quid_sfcs[level]);
        for (quad = 0; quad < NUM_QUADS; ++quad)
        {
            if (quad_sfcs[quad])
                SDL_FreeSurface(quad_sfcs[quad][level]);
            if (hide_sfcs[quad])
                SDL_FreeSurface(hide_sfcs[quad][level]);
            if (unhide_starts[quad])
                free(unhide_starts[quad][level]);
            if (connects[quad])
                free(connects[quad][level]);
        }
    }
    if (quid_sfcs)
        free(quid_sfcs);
    for (quad = 0; quad < NUM_QUADS; ++quad)
    {
        if (quad_sfcs[quad])
            free(quad_sfcs[quad]);
        if (hide_sfcs[quad])
            free(hide_sfcs[quad]);
        if (unhide_starts[quad])
            free(unhide_starts[quad]);
        if (connects[quad])
            free(connects[quad]);
    }
    SDL_FreeSurface(player_sfc);
    SDL_FreeSurface(exit_up_sfc);
    SDL_FreeSurface(exit_dn_sfc);
    SDL_FreeSurface(exit_final_sfc);
    SDL_FreeSurface(fade_sfc);
    SDL_FreeSurface(prev_sfc);
    for (i = 0; i < NUM_BUTTONS; ++i)
        SDL_FreeSurface(button_sfcs[i]);
    SDL_FreeSurface(maze_sfc);
    SDL_FreeSurface(icon_sfc);
    if (exit_up_rooms)
        free(exit_up_rooms);
    if (exit_dn_rooms)
        free(exit_dn_rooms);
    TTF_Quit();
    SDL_Quit();

#ifdef DEBUG_DIAGNOSTICS
    printf("Exit status:  %s\n", (code == 0) ? "Normal" : "Fatal");
#endif
    exit(code);
}


int main( int argc, char *argv[] )
{
    int finished = 0;

    InitGame(time(NULL));
    NextState = IntroScreen;
    state_arg = NULL;
    while (!finished)
    {
        FunctionState = NextState;
        finished = FunctionState();
    }
#ifdef DEBUG_DIAGNOSTICS
    if (total_ticks > 0)
        printf("Average FPS: %.2f\n", (float)n_frames * 1000.0f / total_ticks);
#endif
    Quit(0);
}
