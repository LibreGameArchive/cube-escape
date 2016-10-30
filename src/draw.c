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

#include "SDL.h"

#include "draw.h"


void SetPixel_32( SDL_Surface *sfc, int x, int y, Uint32 c )
{
    if (x < 0 || x >= sfc->w || y < 0 || y >= sfc->h)
        return;

    ((Uint32 *)sfc->pixels)[x + y * sfc->w] = c;
}


Uint32 GetPixel_32( SDL_Surface *sfc, int x, int y )
{
    if (x < 0 || x >= sfc->w || y < 0 || y >= sfc->h)
        return black;

    return ((Uint32 *)sfc->pixels)[x + y * sfc->w];
}


void SetPixel_8( SDL_Surface *sfc, int x, int y, Uint8 c )
{
    if (x < 0 || x >= sfc->w || y < 0 || y >= sfc->h)
        return;

    ((Uint8 *)sfc->pixels)[x + y * sfc->w] = c;
}


Uint8 GetPixel_8( SDL_Surface *sfc, int x, int y )
{
    if (x < 0 || x >= sfc->w || y < 0 || y >= sfc->h)
        return 0;

    return ((Uint8 *)sfc->pixels)[x + y * sfc->w];
}


void DrawLine_32( SDL_Surface *sfc, int x1, int y1, int x2, int y2, Uint32 c )
{
    int sign1 = SIGN(x2 - x1),
        sign2 = SIGN(y2 - y1),
        dim = 0;
    float sub1 = 0.0f,
          sub2 = 0.0f;

    if (abs(x1 - x2) > abs(y1 - y2))
    {
        sub1 = x2 - x1;
        sub2 = y2 - y1 + sign2;
        for (dim = x1; dim != x2 + sign1; dim += sign1)
            SetPixel_32(sfc, dim, y1 + (int)(sub2 * (dim - x1) / sub1), c);
    }
    else
    {
        sub1 = y2 - y1;
        sub2 = x2 - x1 + sign1;
        for (dim = y1; dim != y2 + sign2; dim += sign2)
            SetPixel_32(sfc, x1 + (int)(sub2 * (dim - y1) / sub1), dim, c);
    }
}


void DrawFilledGradientCircle( SDL_Surface *sfc, int diam,
                               int up_left_x, int up_left_y,
                               Uint32 center_color, Uint32 edge_color )
{
    int half = diam / 2,
        x = 0,
        y = 0;
    float fx = 0.0f,
          fy = 0.0f,
          d = 0.0f;
    Uint8 cr = 0,
          cg = 0,
          cb = 0,
          ca = 0,
          er = 0,
          eg = 0,
          eb = 0,
          ea = 0;
    Uint32 color = 0;

    SDL_GetRGBA(center_color, sfc->format, &cr, &cg, &cb, &ca);
    SDL_GetRGBA(edge_color, sfc->format, &er, &eg, &eb, &ea);

    for (y = 0; y < diam; ++y)
        for (x = 0; x < diam; ++x)
        {
            fx = (float)x - half + 0.5f;
            fy = (float)y - half + 0.5f;
            d = sqrt(fx * fx + fy * fy);
            color = SDL_MapRGBA( sfc->format,
                                 cr + (float)(er - cr) * d / half,
                                 cg + (float)(eg - cg) * d / half,
                                 cb + (float)(eb - cb) * d / half,
                                 ca + (float)(ea - ca) * d / half );
            if (d < half)
            {
                SetPixel_32( sfc, x + up_left_x, y + up_left_y,
                             color );
            }
        }
}


void SetQuadEdgeArrays( QuadEdge_t *set_x, int start_x,
                        QuadEdge_t *set_y, int start_y,
                        int side, int x1, int y1, int x2, int y2 )
{
    int sign1 = SIGN(x2 - x1),
        sign2 = SIGN(y2 - y1),
        dim1 = 0,
        dim2 = 0,
        ix = 0,
        iy = 0;
    float sub1 = 0.0f,
          sub2 = 0.0f;

    if (abs(x1 - x2) > abs(y1 - y2))
    {
        sub1 = x2 - x1;
        sub2 = y2 - y1 + sign2;
        for (dim1 = x1; dim1 != x2 + sign1; dim1 += sign1)
        {
            dim2 = y1 + (int)(sub2 * (dim1 - x1) / sub1);
            ix = dim1 - start_x;
            iy = dim2 - start_y;

            set_x[ix].scr_x = dim1;
            set_x[ix].scr_y = dim2;
            set_y[iy].scr_x = dim1;
            set_y[iy].scr_y = dim2;
            switch (side)
            {
                case 0:
                    set_x[ix].tex.x = (float)(dim1 - x1) / sub1 + 0.00001f;
                    set_x[ix].tex.y = 0.0f;
                    set_y[iy].tex.x = (float)(dim1 - x1) / sub1 + 0.00001f;
                    set_y[iy].tex.y = 0.0f;
                    break;

                case 1:
                    set_x[ix].tex.x = 1.0f;
                    set_x[ix].tex.y = (float)(dim1 - x1) / sub1 + 0.00001f;
                    set_y[iy].tex.x = 1.0f;
                    set_y[iy].tex.y = (float)(dim1 - x1) / sub1 + 0.00001f;
                    break;

                case 2:
                    set_x[ix].tex.x = 1.0f - (float)(dim1 - x1) / sub1;
                    set_x[ix].tex.y = 1.0f;
                    set_y[iy].tex.x = 1.0f - (float)(dim1 - x1) / sub1;
                    set_y[iy].tex.y = 1.0f;
                    break;

                case 3:
                    set_x[ix].tex.x = 0.0f;
                    set_x[ix].tex.y = 1.0f - (float)(dim1 - x1) / sub1;
                    set_y[iy].tex.x = 0.0f;
                    set_y[iy].tex.y = 1.0f - (float)(dim1 - x1) / sub1;
                    break;

                default:
                    fprintf(stderr, "Invalid quad edge value (x > y)\n");
                    Quit(3);
                    break;

            }
        }
    }
    else
    {
        sub1 = y2 - y1;
        sub2 = x2 - x1 + sign1;
        for (dim1 = y1; dim1 != y2 + sign2; dim1 += sign2)
        {
            dim2 = x1 + (int)(sub2 * (dim1 - y1) / sub1);
            ix = dim2 - start_x;
            iy = dim1 - start_y;

            set_x[ix].scr_x = dim2;
            set_x[ix].scr_y = dim1;
            set_y[iy].scr_x = dim2;
            set_y[iy].scr_y = dim1;
            switch (side)
            {
                case 0:
                    set_x[ix].tex.x = (float)(dim1 - y1) / sub1 + 0.00001f;
                    set_x[ix].tex.y = 0.0f;
                    set_y[iy].tex.x = (float)(dim1 - y1) / sub1 + 0.00001f;
                    set_y[iy].tex.y = 0.0f;
                    break;

                case 1:
                    set_x[ix].tex.x = 1.0f;
                    set_x[ix].tex.y = (float)(dim1 - y1) / sub1 + 0.00001f;
                    set_y[iy].tex.x = 1.0f;
                    set_y[iy].tex.y = (float)(dim1 - y1) / sub1 + 0.00001f;
                    break;

                case 2:
                    set_x[ix].tex.x = 1.0f - (float)(dim1 - y1) / sub1;
                    set_x[ix].tex.y = 1.0f;
                    set_y[iy].tex.x = 1.0f - (float)(dim1 - y1) / sub1;
                    set_y[iy].tex.y = 1.0f;
                    break;

                case 3:
                    set_x[ix].tex.x = 0.0f;
                    set_x[ix].tex.y = 1.0f - (float)(dim1 - y1) / sub1;
                    set_y[iy].tex.x = 0.0f;
                    set_y[iy].tex.y = 1.0f - (float)(dim1 - y1) / sub1;
                    break;

                default:
                    fprintf(stderr, "Invalid quad edge value (x <= y)\n");
                    Quit(3);
                    break;

            }
        }
    }
}


void ComputeFollowOffsets( float zoom, float z, int m_size,
                           int *x_offset, int *y_offset )
{
    float x_distance = 0.0f,
          y_distance = 0.0f;
    int x1 = 0,
        y1 = 0,
        x2 = 0,
        y2 = 0;

    x_distance = y_distance = (2.0f * zoom) / z / m_size;

    if (fast_graphics)
    {
        if ( player.traversing == TRAVERSE_FOLLOW &&
             rotating == ROTATE_NONE )
        {
            x1 = player.to_room.x;
            y1 = player.to_room.y;
        }
        else
        {
            x1 = player.room.x;
            y1 = player.room.y;
        }
        x2 = 0;
        y2 = 0;
    }
    else if ( player.traversing == TRAVERSE_FOLLOW &&
              rotating == ROTATE_NONE )
    {
        x1 = player.to_room.x;
        y1 = player.to_room.y;
        x2 = player.move_x_opp;
        y2 = player.move_y_opp;
    }
    else
    {
        x1 = player.room.x;
        y1 = player.room.y;
        x2 = player.move_x;
        y2 = player.move_y;
    }
    switch (player.orient)
    {
        case UP:
            *x_offset = +x_distance * ( (float)m_size / 2.0f - x1 -
                                        (float)x2 / SPRITE_SIZE - 0.5f );
            *y_offset = +y_distance * ( (float)m_size / 2.0f - y1 -
                                        (float)y2 / SPRITE_SIZE - 0.5f );
            break;

        case DOWN:
            *x_offset = -x_distance * ( (float)m_size / 2.0f - x1 -
                                        (float)x2 / SPRITE_SIZE - 0.5f );
            *y_offset = -y_distance * ( (float)m_size / 2.0f - y1 -
                                        (float)y2 / SPRITE_SIZE - 0.5f );
            break;

        case LEFT:
            *x_offset = -y_distance * ( (float)m_size / 2.0f - y1 -
                                        (float)y2 / SPRITE_SIZE - 0.5f );
            *y_offset = +x_distance * ( (float)m_size / 2.0f - x1 -
                                        (float)x2 / SPRITE_SIZE - 0.5f );
            break;

        case RIGHT:
            *x_offset = +y_distance * ( (float)m_size / 2.0f - y1 -
                                        (float)y2 / SPRITE_SIZE - 0.5f );
            *y_offset = -x_distance * ( (float)m_size / 2.0f - x1 -
                                        (float)x2 / SPRITE_SIZE - 0.5f );
            break;

    }
}


void DrawOutlinedQuadWithDecals( SDL_Surface *dst, SDL_Surface *quad_tex,
                                 int n_quad, Uint32 line_color )
{
    QuadEdge_t *qe_x = NULL,
               *qe_y = NULL,
               *qe1 = NULL,
               *qe2 = NULL;
    Point_t *pt1 = NULL,
            *pt2 = NULL;
    int m_size = (maze_size > -1 ? maze_size : 1),
        texture_hold[dst->w][dst->h],
        sprite_hold[dst->w][dst->h],
        sprite_w = SPRITE_SIZE * m_size,
        sprite_h = SPRITE_SIZE * m_size,
        x_offset = 0,
        y_offset = 0,
        edge_x = 0,
        edge_y = 0,
        size_x = 0,
        size_y = 0,
        mid_x = dst->w / 2,
        mid_y = dst->h / 2,
        temp_min = 0,
        temp_max = 0,
        min_x = 0,
        max_x = 0,
        min_y = 0,
        max_y = 0,
        tex_x = 0,
        tex_y = 0,
        ex_a = 0,
        x1 = 0,
        y1 = 0,
        x2 = 0,
        y2 = 0,
        x3 = 0,
        y3 = 0,
        w = 0,
        h = 0,
        i = 0;
    float factor = 0.0f,
          diff1 = 0.0f,
          diff2 = 0.0f,
          zoom = 0;
    Uint32 pl_mask_color = GetPixel_32(player_sfc, 0, 0),
           ticks = SDL_GetTicks() - player.move_ticks_start,
           color = 0;
    Uint8 cpy_r = 0,
          cpy_g = 0,
          cpy_b = 0,
          ex_r = 0,
          ex_g = 0,
          ex_b = 0,
          pl_r = 0,
          pl_g = 0,
          pl_b = 0,
          r = 0,
          g = 0,
          b = 0;

    /* Set offsets if camera is strictly following player. */
    if ( follow_player &&
         (n_quad == player.room.quad || n_quad == player.to_room.quad) )
    {
        zoom = player_zoom;
        ComputeFollowOffsets( zoom, center.z - 1.0f, m_size,
                              &x_offset, &y_offset );
    }
    else
    {
        x_offset = 0;
        y_offset = 0;
        if (player.level == 0)
            zoom = full_zoom / 3;
        else
            zoom = full_zoom;
    }

    min_x = dst->w;
    max_x = -1;
    min_y = dst->h;
    max_y = -1;
    for (i = 0; i < 4; ++i)
    {
        pt1 = &copy_points[quads[n_quad][i]];
        x1 = TO_SFC_COORD(pt1, x, zoom) + mid_x + x_offset;
        y1 = TO_SFC_COORD(pt1, y, zoom) + mid_y + y_offset;

        if (x1 < min_x)
            min_x = x1;
        if (x1 > max_x)
            max_x = x1;

        if (y1 < min_y)
            min_y = y1;
        if (y1 > max_y)
            max_y = y1;
    }

    size_x = max_x - min_x + 3;
    size_y = max_y - min_y + 3;

    qe_x = (QuadEdge_t *)malloc(sizeof(QuadEdge_t) * size_x * 2);
    qe_y = (QuadEdge_t *)malloc(sizeof(QuadEdge_t) * size_y * 2);

    /* Fill quad-edge arrays. */
    for (i = 0; i < 4; ++i)
    {
        pt1 = &copy_points[quads[n_quad][i]];
        pt2 = &copy_points[quads[n_quad][(i + 1) % 4]];
        x1 = TO_SFC_COORD(pt1, x, zoom) + mid_x + x_offset;
        y1 = TO_SFC_COORD(pt1, y, zoom) + mid_y + y_offset;
        x2 = TO_SFC_COORD(pt2, x, zoom) + mid_x + x_offset;
        y2 = TO_SFC_COORD(pt2, y, zoom) + mid_y + y_offset;

        if (x1 <= x2)
            edge_x = 0;
        else
            edge_x = 1;

        if (y1 >= y2)
            edge_y = 0;
        else
            edge_y = 1;

        SetQuadEdgeArrays( qe_x + size_x * edge_x, min_x - 1,
                           qe_y + size_y * edge_y, min_y - 1,
                           i, x1, y1, x2, y2 );
    }

    /* Set multiplicative factor for quad shading. */
    if ( rotating == ROTATE_CCWISE || rotating == ROTATE_CWISE ||
         rotating == ROTATE_NONE )
        factor = 1.0f;
    else if (rotating == ROTATE_LEFT || rotating == ROTATE_RIGHT)
    {
        diff1 = fabs( copy_points[quads[n_quad][0]].x -
                      copy_points[quads[n_quad][1]].x );
        diff2 = fabs( copy_points[quads[n_quad][1]].x -
                      copy_points[quads[n_quad][2]].x );
        if (diff1 == 0.0f)
            factor = diff2;
        else if (diff2 == 0.0f)
            factor = diff1;
        else
            factor = MIN(diff1, diff2);
        factor /= 2.0f;
    }
    else if (rotating == ROTATE_UP || rotating == ROTATE_DOWN)
    {
        diff1 = fabs( copy_points[quads[n_quad][0]].y -
                      copy_points[quads[n_quad][1]].y );
        diff2 = fabs( copy_points[quads[n_quad][1]].y -
                      copy_points[quads[n_quad][2]].y );
        if (diff1 == 0.0f)
            factor = diff2;
        else if (diff2 == 0.0f)
            factor = diff1;
        else
            factor = MIN(diff1, diff2);
        factor /= 2.0f;
    }

    /* Adjust player sprite if moving. */
    if (!fast_graphics && player.x_dir != 0)
    {
        player.move_x = SPRITE_SIZE * ticks / pl_move_time;
        if (player.move_x > SPRITE_SIZE)
            player.move_x = SPRITE_SIZE;
        player.move_x *= player.x_dir;
        if (player.traversing == NOT_TRAVERSING)
        {
            player.move_x_opp =
                SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE;
            if (player.move_x_opp > 0)
                player.move_x_opp = 0;
            player.move_x_opp *= player.x_dir;
        }
        else if (player.to_room.y == 0)
        {
            player.move_y_opp =
                +(SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE);
            if (player.move_y_opp > 0)
                player.move_y_opp = 0;
        }
        else
        {
            player.move_y_opp =
                -(SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE);
            if (player.move_y_opp < 0)
                player.move_y_opp = 0;
        }
    }
    else if (!fast_graphics && player.y_dir != 0)
    {
        player.move_y =
            SPRITE_SIZE * ticks / pl_move_time;
        if (player.move_y > SPRITE_SIZE)
            player.move_y = SPRITE_SIZE;
        player.move_y *= player.y_dir;
        if (player.traversing == NOT_TRAVERSING)
        {
            player.move_y_opp =
                SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE;
            if (player.move_y_opp > 0)
                player.move_y_opp = 0;
            player.move_y_opp *= player.y_dir;
        }
        else if (player.to_room.x == 0)
        {
            player.move_x_opp =
                +(SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE);
            if (player.move_x_opp > 0)
                player.move_x_opp = 0;
        }
        else
        {
            player.move_x_opp =
                -(SPRITE_SIZE * ticks / pl_move_time - SPRITE_SIZE);
            if (player.move_x_opp < 0)
                player.move_x_opp = 0;
        }
    }
    else
    {
        player.move_x = 0;
        player.move_x_opp = 0;
        player.move_y = 0;
        player.move_y_opp = 0;
    }

    /* Fill xxx_hold arrays. */
    w = MIN(dst->w, max_x);
    for (x1 = MAX(0, min_x + 1); x1 < w; ++x1)
    {
        qe1 = &qe_x[x1 - min_x + 1];
        qe2 = &qe_x[x1 - min_x + 1 + size_x];
        temp_min = MIN(qe1->scr_y, qe2->scr_y);
        temp_max = MAX(qe1->scr_y, qe2->scr_y);
        h = MIN(dst->h - 1, temp_max);
        for (y1 = MAX(0, temp_min + 1); y1 < h; ++y1)
        {
            if (qe1->scr_y == qe2->scr_y)
                y2 = quad_tex->h - 1;
            else
                y2 = (int)( ( (float)(y1 - qe1->scr_y) /
                              (qe2->scr_y - qe1->scr_y) *
                              (qe2->tex.y - qe1->tex.y) +
                              qe1->tex.y ) * quad_tex->h );
            texture_hold[x1][y1] = y2;

            if (qe1->scr_y == qe2->scr_y)
                y2 = sprite_h - 1;
            else
                y2 = (int)( ( (float)(y1 - qe1->scr_y) /
                              (qe2->scr_y - qe1->scr_y) *
                              (qe2->tex.y - qe1->tex.y) +
                              qe1->tex.y ) * sprite_h );
            sprite_hold[x1][y1] = y2;
        }
    }

    /* Set pixels onto destination surface. */
    h = MIN(dst->h, max_y);
    for (y1 = MAX(0, min_y + 1); y1 < h; ++y1)
    {
        qe1 = &qe_y[y1 - min_y + 1];
        qe2 = &qe_y[y1 - min_y + 1 + size_y];
        temp_min = MIN(qe1->scr_x, qe2->scr_x);
        temp_max = MAX(qe1->scr_x, qe2->scr_x);
        w = MIN(dst->w, temp_max);
        for (x1 = MAX(0, temp_min + 1); x1 < w; ++x1)
        {
            if (qe1->scr_x == qe2->scr_x)
                x2 = quad_tex->w - 1;
            else
                x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                              (qe2->scr_x - qe1->scr_x) *
                              (qe2->tex.x - qe1->tex.x) +
                              qe1->tex.x ) * quad_tex->w );
            y2 = texture_hold[x1][y1];
            color = GetPixel_32(quad_tex, x2, y2);
            SDL_GetRGB(color, dst->format, &r, &g, &b);
            r *= factor;  g *= factor;  b *= factor;
            color = SDL_MapRGB(dst->format, r, g, b);
            SetPixel_32(dst, x1, y1, color);

            if (exit_dn_rooms[player.level].quad == n_quad)
            {
                if (qe1->scr_x == qe2->scr_x)
                    x2 = sprite_w - 1;
                else
                    x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                                  (qe2->scr_x - qe1->scr_x) *
                                  (qe2->tex.x - qe1->tex.x) +
                                  qe1->tex.x ) * sprite_w );
                y2 = sprite_hold[x1][y1];
                if ( x2 / SPRITE_SIZE == exit_dn_rooms[player.level].x &&
                     y2 / SPRITE_SIZE == exit_dn_rooms[player.level].y )
                {
                    x3 = x2 % SPRITE_SIZE;
                    y3 = y2 % SPRITE_SIZE;
                    color = GetPixel_8(exit_dn_sfc, x3, y3);
                    color = SDL_MapRGB( Screen->format,
                                        exit_dn_pal_cpy[color].r,
                                        exit_dn_pal_cpy[color].g,
                                        exit_dn_pal_cpy[color].b );
                    SDL_GetRGB(color, Screen->format, &ex_r, &ex_g, &ex_b);
                    ex_r *= factor;  ex_g *= factor;  ex_b *= factor;
                    if (!fast_graphics)
                    {
                        ex_a = MAX( abs(SPRITE_SIZE / 2 - x2 % SPRITE_SIZE),
                                    abs(SPRITE_SIZE / 2 - y2 % SPRITE_SIZE) );
                        ex_a = 256 - 256 * ex_a * 2 / SPRITE_SIZE;
                        ex_a = MAX(0, MIN(255, ex_a));
                        cpy_r = (Uint8)((ex_a * ex_r + (255 - ex_a) * r) / 256);
                        cpy_g = (Uint8)((ex_a * ex_g + (255 - ex_a) * g) / 256);
                        cpy_b = (Uint8)((ex_a * ex_b + (255 - ex_a) * b) / 256);
                        color = SDL_MapRGB(dst->format, cpy_r, cpy_g, cpy_b);
                    }
                    else
                    {
                        color = SDL_MapRGB(dst->format, ex_r, ex_g, ex_b);
                    }
                    SetPixel_32(dst, x1, y1, color);
                }
            }

            if (exit_up_rooms[player.level].quad == n_quad)
            {
                if (qe1->scr_x == qe2->scr_x)
                    x2 = sprite_w - 1;
                else
                    x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                                  (qe2->scr_x - qe1->scr_x) *
                                  (qe2->tex.x - qe1->tex.x) +
                                  qe1->tex.x ) * sprite_w );
                y2 = sprite_hold[x1][y1];
                if ( x2 / SPRITE_SIZE == exit_up_rooms[player.level].x &&
                     y2 / SPRITE_SIZE == exit_up_rooms[player.level].y )
                {
                    x3 = x2 % SPRITE_SIZE;
                    y3 = y2 % SPRITE_SIZE;
                    color = GetPixel_8(exit_up_sfc, x3, y3);
                    color = SDL_MapRGB( Screen->format,
                                        exit_up_pal_cpy[color].r,
                                        exit_up_pal_cpy[color].g,
                                        exit_up_pal_cpy[color].b );
                    SDL_GetRGB(color, Screen->format, &ex_r, &ex_g, &ex_b);
                    ex_r *= factor;  ex_g *= factor;  ex_b *= factor;
                    if (!fast_graphics)
                    {
                        ex_a = MAX( abs(SPRITE_SIZE / 2 - x2 % SPRITE_SIZE),
                                    abs(SPRITE_SIZE / 2 - y2 % SPRITE_SIZE) );
                        ex_a = 256 - 256 * ex_a * 2 / SPRITE_SIZE;
                        ex_a = MAX(0, MIN(255, ex_a));
                        cpy_r = (Uint8)((ex_a * ex_r + (255 - ex_a) * r) / 256);
                        cpy_g = (Uint8)((ex_a * ex_g + (255 - ex_a) * g) / 256);
                        cpy_b = (Uint8)((ex_a * ex_b + (255 - ex_a) * b) / 256);
                        color = SDL_MapRGB(dst->format, cpy_r, cpy_g, cpy_b);
                    }
                    else
                    {
                        color = SDL_MapRGB(dst->format, ex_r, ex_g, ex_b);
                    }
                    SetPixel_32(dst, x1, y1, color);
                }
            }

            if ( player.level == total_num_levels - 1 &&
                 exit_final_room.quad == n_quad )
            {
                if (qe1->scr_x == qe2->scr_x)
                    x2 = sprite_w - 1;
                else
                    x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                                  (qe2->scr_x - qe1->scr_x) *
                                  (qe2->tex.x - qe1->tex.x) +
                                  qe1->tex.x ) * sprite_w );
                y2 = sprite_hold[x1][y1];
                if ( x2 / SPRITE_SIZE == exit_final_room.x &&
                     y2 / SPRITE_SIZE == exit_final_room.y )
                {
                    x3 = x2 % SPRITE_SIZE;
                    y3 = y2 % SPRITE_SIZE;
                    color = GetPixel_8(exit_final_sfc, x3, y3);
                    color = SDL_MapRGB( Screen->format,
                                        exit_final_pal_cpy[color].r,
                                        exit_final_pal_cpy[color].g,
                                        exit_final_pal_cpy[color].b );
                    SDL_GetRGB(color, Screen->format, &ex_r, &ex_g, &ex_b);
                    ex_r *= factor;  ex_g *= factor;  ex_b *= factor;
                    ex_a = MAX( abs(SPRITE_SIZE / 2 - x2 % SPRITE_SIZE),
                                abs(SPRITE_SIZE / 2 - y2 % SPRITE_SIZE) );
                    if (!fast_graphics)
                    {
                        ex_a = 256 - 256 * ex_a * 2 / SPRITE_SIZE;
                        ex_a = MAX(0, MIN(255, ex_a));
                        cpy_r = (Uint8)((ex_a * ex_r + (255 - ex_a) * r) / 256);
                        cpy_g = (Uint8)((ex_a * ex_g + (255 - ex_a) * g) / 256);
                        cpy_b = (Uint8)((ex_a * ex_b + (255 - ex_a) * b) / 256);
                        color = SDL_MapRGB(dst->format, cpy_r, cpy_g, cpy_b);
                    }
                    else
                    {
                        color = SDL_MapRGB(dst->format, ex_r, ex_g, ex_b);
                    }
                    SetPixel_32(dst, x1, y1, color);
                }
            }

            if (n_quad == player.room.quad)
            {
                if (qe1->scr_x == qe2->scr_x)
                    x2 = sprite_w - 1;
                else
                    x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                                  (qe2->scr_x - qe1->scr_x) *
                                  (qe2->tex.x - qe1->tex.x) +
                                  qe1->tex.x ) * sprite_w );
                y2 = sprite_hold[x1][y1];
                if ( x2 / SPRITE_SIZE == player.room.x &&
                     y2 / SPRITE_SIZE == player.room.y )
                {
                    x3 = x2 % SPRITE_SIZE - player.move_x;
                    y3 = y2 % SPRITE_SIZE - player.move_y;
                    if ( x3 >= 0 && x3 < SPRITE_SIZE &&
                         y3 >= 0 && y3 < SPRITE_SIZE )
                    {
                        color = GetPixel_32( player_sfc,
                                             x3 % SPRITE_SIZE,
                                             y3 % SPRITE_SIZE );
                        if (color != pl_mask_color)
                        {
                            SDL_GetRGB( color, player_sfc->format,
                                        &pl_r, &pl_g, &pl_b );
                            pl_r *= factor; pl_g *= factor; pl_b *= factor;
                            color = SDL_MapRGB( dst->format,
                                                pl_r, pl_g, pl_b );
                            SetPixel_32(dst, x1, y1, color);
                        }
                    }
                }
            }
            if ( !fast_graphics && player.move_ticks_start > 0 &&
                 n_quad == player.to_room.quad )
            {
                if (qe1->scr_x == qe2->scr_x)
                    x2 = sprite_w - 1;
                else
                    x2 = (int)( ( (float)(x1 - qe1->scr_x) /
                                  (qe2->scr_x - qe1->scr_x) *
                                  (qe2->tex.x - qe1->tex.x) +
                                  qe1->tex.x ) * sprite_w );
                y2 = sprite_hold[x1][y1];
                if ( x2 / SPRITE_SIZE == player.to_room.x &&
                     y2 / SPRITE_SIZE == player.to_room.y )
                {
                    x3 = x2 % SPRITE_SIZE - player.move_x_opp;
                    y3 = y2 % SPRITE_SIZE - player.move_y_opp;
                    if ( x3 >= 0 && x3 < SPRITE_SIZE &&
                         y3 >= 0 && y3 < SPRITE_SIZE )
                    {
                        color = GetPixel_32( player_sfc,
                                             x3 % SPRITE_SIZE,
                                             y3 % SPRITE_SIZE );
                        if (color != pl_mask_color)
                        {
                            SDL_GetRGB( color, player_sfc->format,
                                        &pl_r, &pl_g, &pl_b );
                            pl_r *= factor;
                            pl_g *= factor;
                            pl_b *= factor;
                            color = SDL_MapRGB( dst->format,
                                                pl_r, pl_g, pl_b );
                            SetPixel_32(dst, x1, y1, color);
                        }
                    }
                }
            }
        }
    }

    /* Draw lines around edges of the cube. */
    for (i = 0; i < 4; ++i)
    {
        pt1 = &copy_points[quads[n_quad][i]];
        pt2 = &copy_points[quads[n_quad][(i + 1) % 4]];
        x1 = TO_SFC_COORD(pt1, x, zoom) + mid_x + x_offset;
        y1 = TO_SFC_COORD(pt1, y, zoom) + mid_y + y_offset;
        x2 = TO_SFC_COORD(pt2, x, zoom) + mid_x + x_offset;
        y2 = TO_SFC_COORD(pt2, y, zoom) + mid_y + y_offset;

        DrawLine_32(dst, x1, y1, x2, y2, line_color);
    }

    free(qe_x);
    free(qe_y);
}
