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

#ifndef STATES_HDR
#define STATES_HDR

#include "main.h"


/* Function prototypes */

int IntroScreen( void );
int OptionsScreen( void );
int GamePlay( void );
int WinScreen( void );
int ConfirmMessage( void );

void ZoomIn( void );
void ZoomOut( void );
void UpdateNumberText( char *str, SDL_Surface *sfc, SDL_Surface *choice_sfc,
                       int cw, int ch  );

#endif  /* STATES_HDR */
