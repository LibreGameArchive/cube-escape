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

#include "SDL.h"

#include "maze.h"


/* Globals */
int high_exit_dist,
    good_exit_distance,
    place_exit,
    to_quad_array[6][4] = { {2, 4, 3, 5},  {3, 4, 2, 5},  {4, 0, 5, 1},
                            {5, 0, 4, 1},  {0, 2, 1, 3},  {1, 2, 0, 3} };


int CreateSurfaceMaze( int size, int level, int max_psg_count )
{
    SDL_Surface *cur_sfc = NULL;
    SfcMazeConnect_t *con[4] = { NULL };
    int tot_num_rooms = 0,
        open[4] = { 0 },
        stack_size = 0,
        psg_count = 0,
        finished = 0,
        n_opens = 0,
        choice = 0,
        half_sz = 0,
        quad = 0,
        off = 0,
        dir = 0,
        cx = 0,
        cy = 0,
        i = 0,
        q = 0,
        x = 0,
        y = 0;
    SfcMazeRoom_t *room_stack;

    half_sz = size / 2;
    tot_num_rooms = half_sz * half_sz * NUM_QUADS;
    room_stack = (SfcMazeRoom_t *)
        malloc(sizeof(SfcMazeRoom_t) * tot_num_rooms);

    for (quad = 0; quad < NUM_QUADS; ++quad)
    {
        for (i = 0, off = 1; i < half_sz; ++i, off += 2)
        {
            connects[quad][level][i + half_sz * 0].fr_x = off;
            connects[quad][level][i + half_sz * 0].fr_y = 0;
            connects[quad][level][i + half_sz * 0].to_qd =
                to_quad_array[quad][0];

            connects[quad][level][i + half_sz * 1].fr_x = size - 1;
            connects[quad][level][i + half_sz * 1].fr_y = off;
            connects[quad][level][i + half_sz * 1].to_qd =
                to_quad_array[quad][1];

            connects[quad][level][i + half_sz * 2].fr_x = size - off - 1;
            connects[quad][level][i + half_sz * 2].fr_y = size - 1;
            connects[quad][level][i + half_sz * 2].to_qd =
                to_quad_array[quad][2];

            connects[quad][level][i + half_sz * 3].fr_x = 0;
            connects[quad][level][i + half_sz * 3].fr_y = size - off - 1;
            connects[quad][level][i + half_sz * 3].to_qd =
                to_quad_array[quad][3];

            if (quad % 2 == 0)
            {
                connects[quad][level][i + half_sz * 0].to_x = size - 1;
                connects[quad][level][i + half_sz * 0].to_y = size - off - 1;

                connects[quad][level][i + half_sz * 1].to_x = size - off - 1;
                connects[quad][level][i + half_sz * 1].to_y = 0;

                connects[quad][level][i + half_sz * 2].to_x = size - 1;
                connects[quad][level][i + half_sz * 2].to_y = size - off - 1;

                connects[quad][level][i + half_sz * 3].to_x = off;
                connects[quad][level][i + half_sz * 3].to_y = size - 1;
            }
            else
            {
                connects[quad][level][i + half_sz * 0].to_x = 0;
                connects[quad][level][i + half_sz * 0].to_y = off;

                connects[quad][level][i + half_sz * 1].to_x = off;
                connects[quad][level][i + half_sz * 1].to_y = size - 1;

                connects[quad][level][i + half_sz * 2].to_x = 0;
                connects[quad][level][i + half_sz * 2].to_y = off;

                connects[quad][level][i + half_sz * 3].to_x = size - off - 1;
                connects[quad][level][i + half_sz * 3].to_y = 0;
            }
        }

        SDL_FillRect(quad_sfcs[quad][level], NULL, WALL_COLOR);
    }

    quad = rand() % NUM_QUADS;
    cx = rand() % half_sz * 2 + 1;
    cy = rand() % half_sz * 2 + 1;
    room_stack[0].x = cx;
    room_stack[0].y = cy;
    room_stack[0].quad = quad;
    SetPixel_32(quad_sfcs[quad][level], cx, cy, ROOM_COLOR);
    stack_size = 1;

    while (!finished)
    {
        psg_count = 0;

        while (psg_count == 0 || (n_opens > 0 && psg_count < max_psg_count))
        {
            n_opens = 0;
            cur_sfc = quad_sfcs[quad][level];

            if ( cx >= 3 &&
                 GetPixel_32(cur_sfc, cx - 2, cy) == WALL_COLOR )
                open[n_opens++] = LEFT;
            if ( cx < size - 3 &&
                 GetPixel_32(cur_sfc, cx + 2, cy) == WALL_COLOR )
                open[n_opens++] = RIGHT;
            if ( cy >= 3 &&
                 GetPixel_32(cur_sfc, cx, cy - 2) == WALL_COLOR )
                open[n_opens++] = UP;
            if ( cy < size - 3 &&
                 GetPixel_32(cur_sfc, cx, cy + 2) == WALL_COLOR )
                open[n_opens++] = DOWN;

            if (cx == 1)
            {
                for (i = half_sz * 3; i < half_sz * 4; ++i)
                {
                    con[0] = &connects[quad][level][i];
                    if (con[0]->fr_x + 1 == cx && con[0]->fr_y == cy)
                        break;
                }
                x = con[0]->to_x;
                y = con[0]->to_y;
                if (y == 0)
                    y = 1;
                else
                    y = size - 2;
                if ( GetPixel_32( quad_sfcs[con[0]->to_qd][level],
                                  x, y ) == WALL_COLOR )
                    open[n_opens++] = TURN_LEFT;
            }
            if (cx == size - 2)
            {
                for (i = half_sz; i < half_sz * 2; ++i)
                {
                    con[1] = &connects[quad][level][i];
                    if (con[1]->fr_x - 1 == cx && con[1]->fr_y == cy)
                        break;
                }
                x = con[1]->to_x;
                y = con[1]->to_y;
                if (y == 0)
                    y = 1;
                else
                    y = size - 2;
                if ( GetPixel_32( quad_sfcs[con[1]->to_qd][level],
                                  x, y ) == WALL_COLOR )
                    open[n_opens++] = TURN_RIGHT;
            }
            if (cy == 1)
            {
                for (i = 0; i < half_sz; ++i)
                {
                    con[2] = &connects[quad][level][i];
                    if (con[2]->fr_x == cx && con[2]->fr_y + 1 == cy)
                        break;
                }
                x = con[2]->to_x;
                if (x == 0)
                    x = 1;
                else
                    x = size - 2;
                y = con[2]->to_y;
                if ( GetPixel_32( quad_sfcs[con[2]->to_qd][level],
                                 x, y ) == WALL_COLOR )
                    open[n_opens++] = TURN_UP;
            }
            if (cy == size - 2)
            {
                for (i = half_sz * 2; i < half_sz * 3; ++i)
                {
                    con[3] = &connects[quad][level][i];
                    if (con[3]->fr_x == cx && con[3]->fr_y - 1 == cy)
                        break;
                }
                x = con[3]->to_x;
                if (x == 0)
                    x = 1;
                else
                    x = size - 2;
                y = con[3]->to_y;
                if ( GetPixel_32( quad_sfcs[con[3]->to_qd][level],
                                  x, y ) == WALL_COLOR )
                    open[n_opens++] = TURN_DOWN;
            }

            if (n_opens)
            {
                choice = open[rand() % n_opens];
                switch (choice)
                {
                    case LEFT:
                        SetPixel_32(cur_sfc, --cx, cy, ROOM_COLOR);
                        SetPixel_32(cur_sfc, --cx, cy, ROOM_COLOR);
                        break;

                    case RIGHT:
                        SetPixel_32(cur_sfc, ++cx, cy, ROOM_COLOR);
                        SetPixel_32(cur_sfc, ++cx, cy, ROOM_COLOR);
                        break;

                    case UP:
                        SetPixel_32(cur_sfc, cx, --cy, ROOM_COLOR);
                        SetPixel_32(cur_sfc, cx, --cy, ROOM_COLOR);
                        break;

                    case DOWN:
                        SetPixel_32(cur_sfc, cx, ++cy, ROOM_COLOR);
                        SetPixel_32(cur_sfc, cx, ++cy, ROOM_COLOR);
                        break;

                    case TURN_LEFT:
                        SetPixel_32(cur_sfc, --cx, cy, ROOM_COLOR);
                        quad = con[0]->to_qd;
                        cur_sfc = quad_sfcs[quad][level];
                        cx = con[0]->to_x;
                        cy = con[0]->to_y;
                        if (cy == 0)
                            SetPixel_32( cur_sfc, cx, cy++, ROOM_COLOR );
                        else
                            SetPixel_32( cur_sfc, cx, cy--, ROOM_COLOR );
                        SetPixel_32(cur_sfc, cx, cy, ROOM_COLOR);
                        break;

                    case TURN_RIGHT:
                        SetPixel_32(cur_sfc, ++cx, cy, ROOM_COLOR);
                        quad = con[1]->to_qd;
                        cur_sfc = quad_sfcs[quad][level];
                        cx = con[1]->to_x;
                        cy = con[1]->to_y;
                        if (cy == 0)
                            SetPixel_32( cur_sfc, cx, cy++, ROOM_COLOR );
                        else
                            SetPixel_32( cur_sfc, cx, cy--, ROOM_COLOR );
                        SetPixel_32(cur_sfc, cx, cy, ROOM_COLOR);
                        break;

                    case TURN_UP:
                        SetPixel_32(cur_sfc, cx, --cy, ROOM_COLOR);
                        quad = con[2]->to_qd;
                        cur_sfc = quad_sfcs[quad][level];
                        cx = con[2]->to_x;
                        cy = con[2]->to_y;
                        if (cx == 0)
                            SetPixel_32( cur_sfc, cx++, cy, ROOM_COLOR );
                        else
                            SetPixel_32( cur_sfc, cx--, cy, ROOM_COLOR );
                        SetPixel_32(cur_sfc, cx, cy, ROOM_COLOR);
                        break;

                    case TURN_DOWN:
                        SetPixel_32(cur_sfc, cx, ++cy, ROOM_COLOR);
                        quad = con[3]->to_qd;
                        cur_sfc = quad_sfcs[quad][level];
                        cx = con[3]->to_x;
                        cy = con[3]->to_y;
                        if (cx == 0)
                            SetPixel_32( cur_sfc, cx++, cy, ROOM_COLOR );
                        else
                            SetPixel_32( cur_sfc, cx--, cy, ROOM_COLOR );
                        SetPixel_32(cur_sfc, cx, cy, ROOM_COLOR);
                        break;

                    default:
                        fprintf(stderr, "Invalid opening value\n");

                }

                room_stack[stack_size].x = cx;
                room_stack[stack_size].y = cy;
                room_stack[stack_size++].quad = quad;
            }
            else
                stack_size--;

            psg_count++;
        }

        if (stack_size > 1)
        {
            /*
             * Randomly pick a room from the stack: Swap with last stack
             * element to make it the current room to build the maze
             * from.
             */
            i = rand() % stack_size;
            cx = room_stack[i].x;
            cy = room_stack[i].y;
            quad = room_stack[i].quad;
            room_stack[i].x = room_stack[stack_size - 1].x;
            room_stack[i].y = room_stack[stack_size - 1].y;
            room_stack[i].quad = room_stack[stack_size - 1].quad;
            room_stack[stack_size - 1].x = cx;
            room_stack[stack_size - 1].y = cy;
            room_stack[stack_size - 1].quad = quad;
        }
        else
            finished = 1;
    }

#ifdef DEBUG_ORIENT_CORNERS
    /* Set upper-left pixel for judging face orientation. */
    SetPixel_32(quad_sfcs[0][level], 0, 0, 0xff0000);
    SetPixel_32(quad_sfcs[1][level], 0, 0, 0x7f0000);
    SetPixel_32(quad_sfcs[2][level], 0, 0, 0x00ff00);
    SetPixel_32(quad_sfcs[3][level], 0, 0, 0x007f00);
    SetPixel_32(quad_sfcs[4][level], 0, 0, 0x0000ff);
    SetPixel_32(quad_sfcs[5][level], 0, 0, 0x00007f);
#endif

    free(room_stack);
    return 0;
}


int CalcExits( SfcMazeRoom_t *start, int num_levels, float distance_mult )
{
    int distance[num_levels],
        level = 0;

    exit_up_rooms[0].x = 0;
    exit_up_rooms[0].y = 0;
    exit_up_rooms[0].quad = 1;

    exit_dn_rooms[0].quad = -1;

    exit_dn_rooms[1].x = start->x;
    exit_dn_rooms[1].y = start->y;
    exit_dn_rooms[1].quad = start->quad;

    for (level = 1; level < num_levels; ++level)
    {
        high_exit_dist = 0;
        place_exit = 0;
        RecurseTowardsExit(level, start, NONE, 0);
        good_exit_distance = MAX(1, high_exit_dist * distance_mult);

        place_exit = 1;
        distance[level] = RecurseTowardsExit(level, start, NONE, 0);
        while ( ( distance[level] =
                  RecurseTowardsExit(level, start, NONE, 0) ) == -1 )
        {
            good_exit_distance--;
            distance[level] = RecurseTowardsExit(level, start, NONE, 0);
        }

        if (level < num_levels - 1)
        {
            exit_dn_rooms[level + 1].x = exit_up_rooms[level].x + 2;
            exit_dn_rooms[level + 1].y = exit_up_rooms[level].y + 2;
            exit_dn_rooms[level + 1].quad = exit_up_rooms[level].quad;
            start = &exit_dn_rooms[level + 1];
        }
        else
        {
            exit_final_room.x = exit_up_rooms[level].x;
            exit_final_room.y = exit_up_rooms[level].y;
            exit_final_room.quad = exit_up_rooms[level].quad;
            exit_up_rooms[level].quad = -1;
        }
    }
}


int RecurseTowardsExit( int level, SfcMazeRoom_t *room, int from_dir,
                        int distance )
{
    SDL_Surface *cur_sfc = quad_sfcs[room->quad][level];
    SfcMazeConnect_t *con[4] = { NULL };
    SfcMazeRoom_t next_room = { 0 };
    int open[4] = { 0 },
        n_opens = 0,
        half_sz = 0,
        dist = 0,
        temp = 0,
        i = 0,
        r = 0,
        quad = room->quad,
        x = room->x,
        y = room->y;

    half_sz = cur_sfc->w / 2;
    if (place_exit && distance == good_exit_distance)
    {
        exit_up_rooms[level].x = x;
        exit_up_rooms[level].y = y;
        exit_up_rooms[level].quad = quad;
        return good_exit_distance;
    }

    if (distance > high_exit_dist)
    {
        high_exit_dist = distance;
    }

    if ( from_dir != LEFT && x >= 3 &&
         GetPixel_32(cur_sfc, x - 1, y) == ROOM_COLOR )
        open[n_opens++] = LEFT;
    if ( from_dir != RIGHT && x < cur_sfc->w - 3 &&
         GetPixel_32(cur_sfc, x + 1, y) == ROOM_COLOR )
        open[n_opens++] = RIGHT;
    if ( from_dir != UP && y >= 3 &&
         GetPixel_32(cur_sfc, x, y - 1) == ROOM_COLOR )
        open[n_opens++] = UP;
    if ( from_dir != DOWN && y < cur_sfc->h - 3 &&
         GetPixel_32(cur_sfc, x, y + 1) == ROOM_COLOR )
        open[n_opens++] = DOWN;

    if ( from_dir != LEFT && x == 1 &&
         GetPixel_32(cur_sfc, x - 1, y) == ROOM_COLOR )
    {
        for (i = half_sz * 3; i < half_sz * 4; ++i)
        {
            con[0] = &connects[quad][level][i];
            if (con[0]->fr_x + 1 == x && con[0]->fr_y == y)
                break;
        }
        open[n_opens++] = TURN_LEFT;
    }
    if ( from_dir != RIGHT && x == cur_sfc->w - 2 &&
         GetPixel_32(cur_sfc, x + 1, y) == ROOM_COLOR )
    {
        for (i = half_sz; i < half_sz * 2; ++i)
        {
            con[1] = &connects[quad][level][i];
            if (con[1]->fr_x - 1 == x && con[1]->fr_y == y)
                break;
        }
        open[n_opens++] = TURN_RIGHT;
    }
    if ( from_dir != UP && y == 1 &&
         GetPixel_32(cur_sfc, x, y - 1) == ROOM_COLOR )
    {
        for (i = 0; i < half_sz; ++i)
        {
            con[2] = &connects[quad][level][i];
            if (con[2]->fr_x == x && con[2]->fr_y + 1 == y)
                break;
        }
        open[n_opens++] = TURN_UP;
    }
    if ( from_dir != DOWN && y == cur_sfc->h - 2 &&
         GetPixel_32(cur_sfc, x, y + 1) == ROOM_COLOR )
    {
        for (i = half_sz * 2; i < half_sz * 3; ++i)
        {
            con[3] = &connects[quad][level][i];
            if (con[3]->fr_x == x && con[3]->fr_y - 1 == y)
                break;
        }
        open[n_opens++] = TURN_DOWN;
    }

    if (n_opens)
    {
        for (i = 0; i < n_opens; ++i)
        {
            r = rand() % n_opens;
            temp = open[i];
            open[i] = open[r];
            open[r] = temp;
        }
        for (i = 0; i < n_opens; ++i)
        {
            switch (open[i])
            {
                case LEFT:
                    next_room.x = x - 2;
                    next_room.y = y;
                    next_room.quad = quad;
                    dist = RecurseTowardsExit( level, &next_room, RIGHT,
                                               distance + 1 );
                    if (dist != -1)
                        return dist;
                    break;

                case RIGHT:
                    next_room.x = x + 2;
                    next_room.y = y;
                    next_room.quad = quad;
                    dist = RecurseTowardsExit( level, &next_room, LEFT,
                                               distance + 1 );
                    if (dist != -1)
                        return dist;
                    break;

                case UP:
                    next_room.x = x;
                    next_room.y = y - 2;
                    next_room.quad = quad;
                    dist = RecurseTowardsExit( level, &next_room, DOWN,
                                               distance + 1 );
                    if (dist != -1)
                        return dist;
                    break;

                case DOWN:
                    next_room.x = x;
                    next_room.y = y + 2;
                    next_room.quad = quad;
                    dist = RecurseTowardsExit( level, &next_room, UP,
                                               distance + 1 );
                    if (dist != -1)
                        return dist;
                    break;

                case TURN_LEFT:
                    next_room.x = con[0]->to_x;
                    next_room.y = con[0]->to_y;
                    next_room.quad = con[0]->to_qd;
                    if (next_room.y == 0)
                    {
                        next_room.y++;
                        dist = RecurseTowardsExit( level, &next_room, UP,
                                                   distance + 1 );
                    }
                    else
                    {
                        next_room.y--;
                        dist = RecurseTowardsExit( level, &next_room, DOWN,
                                                   distance + 1 );
                    }
                    if (dist != -1)
                        return dist;
                    break;

                case TURN_RIGHT:
                    next_room.x = con[1]->to_x;
                    next_room.y = con[1]->to_y;
                    next_room.quad = con[1]->to_qd;
                    if (next_room.y == 0)
                    {
                        next_room.y++;
                        dist = RecurseTowardsExit( level, &next_room, UP,
                                                   distance + 1 );
                    }
                    else
                    {
                        next_room.y--;
                        dist = RecurseTowardsExit( level, &next_room, DOWN,
                                                   distance + 1 );
                    }
                    if (dist != -1)
                        return dist;
                    break;

                case TURN_UP:
                    next_room.x = con[2]->to_x;
                    next_room.y = con[2]->to_y;
                    next_room.quad = con[2]->to_qd;
                    if (next_room.x == 0)
                    {
                        next_room.x++;
                        dist = RecurseTowardsExit( level, &next_room, LEFT,
                                                   distance + 1 );
                    }
                    else
                    {
                        next_room.x--;
                        dist = RecurseTowardsExit( level, &next_room, RIGHT,
                                                   distance + 1 );
                    }
                    if (dist != -1)
                        return dist;
                    break;

                case TURN_DOWN:
                    next_room.x = con[3]->to_x;
                    next_room.y = con[3]->to_y;
                    next_room.quad = con[3]->to_qd;
                    if (next_room.x == 0)
                    {
                        next_room.x++;
                        dist = RecurseTowardsExit( level, &next_room, LEFT,
                                                   distance + 1 );
                    }
                    else
                    {
                        next_room.x--;
                        dist = RecurseTowardsExit( level, &next_room, RIGHT,
                                                   distance + 1 );
                    }
                    if (dist != -1)
                        return dist;
                    break;

                default:
                    fprintf(stderr, "Invalid opening value\n");

            }
        }
    }

    return -1;
}
