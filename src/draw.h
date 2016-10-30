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

#ifndef DRAW_HDR
#define DRAW_HDR

#include "main.h"


#define SIGN( diff )  ((diff) > 0 ? 1 : (diff) < 0 ? -1 : 0)
#define TO_SFC_COORD( p, dim, zoom )  (int)((p)->dim * (zoom) / (p)->z)


/* Function prototypes */

void SetPixel_32( SDL_Surface *sfc, int x, int y, Uint32 c );
Uint32 GetPixel_32( SDL_Surface *sfc, int x, int y );
void SetPixel_8( SDL_Surface *sfc, int x, int y, Uint8 c );
Uint8 GetPixel_8( SDL_Surface *sfc, int x, int y );
void DrawLine_32( SDL_Surface *sfc, int x1, int y1, int x2, int y2, Uint32 c );
void DrawFilledGradientCircle( SDL_Surface *sfc, int diam,
                               int center_x, int center_y,
                               Uint32 center_color, Uint32 edge_color );
void SetQuadEdgeArrays( QuadEdge_t *set_x, int start_x,
                        QuadEdge_t *set_y, int start_y,
                        int side, int x1, int y1, int x2, int y2 );
void ComputeFollowOffsets( float zoom, float z, int m_size,
                           int *x_offset, int *y_offset );
void DrawOutlinedQuadWithDecals( SDL_Surface *dst, SDL_Surface *quad_tex,
                                 int n_quad, Uint32 line_color );

#endif  /* DRAW_HDR */
