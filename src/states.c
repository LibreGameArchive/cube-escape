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
#include <string.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "states.h"


char *quit_prompt = "Really quit?",
     *abandon_prompt = "Abandon current game?";


int IntroScreen()
{
    TTF_Font *hdr_fnt = NULL,
             *opt_fnt = NULL;
    SDL_Surface *new_txt = NULL,
                *quit_txt = NULL,
                *title_txt = NULL,
                *auth_txt = NULL,
                *resume_txt = NULL,
                *new_dark_txt = NULL,
                *quit_dark_txt = NULL,
                *resume_dark_txt = NULL;
    SDL_Color dk_grey_fg = { 63, 63, 63 },
              med_grey_fg = { 127, 127, 127 };
    SDL_Rect src_rect,
             dst_rect;
    SDL_Event evt;
    int n_choices = 3,
        finished = 0,
        txt_ch_h = 0,
        m_x = 0,
        m_y = 0,
        m_b = 0;
    static int choice = 1;

    hdr_fnt = TTF_OpenFont(FONT_NAME, 40);
    opt_fnt = TTF_OpenFont(FONT_NAME, 32);

    title_txt = TTF_RenderText_Shaded( hdr_fnt, "Cube Escape", white_c, black_c );
    auth_txt = TTF_RenderText_Shaded( opt_fnt, "by Sean McKean", white_c, black_c);
    new_txt = TTF_RenderText_Shaded( opt_fnt, "New Game", white_c, black_c );
    new_dark_txt = TTF_RenderText_Shaded(opt_fnt, "New Game", med_grey_fg, black_c);
    quit_txt = TTF_RenderText_Shaded(opt_fnt, "Quit", white_c, black_c);
    quit_dark_txt = TTF_RenderText_Shaded(opt_fnt, "Quit", med_grey_fg, black_c);
    resume_txt =
        TTF_RenderText_Shaded( opt_fnt, "Resume Game",
                               playing ? white_c : dk_grey_fg,
                               black_c );
    resume_dark_txt =
        TTF_RenderText_Shaded( opt_fnt, "Resume Game",
                               playing ? med_grey_fg : dk_grey_fg,
                               black_c );

    txt_ch_h = TTF_FontHeight(opt_fnt);

    while (!finished)
    {
        SDL_GetMouseState(&m_x, &m_y);

        SDL_FillRect(Screen, NULL, black);
        dst_rect = (SDL_Rect) { CENTER_X_SCREEN(title_txt->w), title_txt->h };
        SDL_BlitSurface(title_txt, NULL, Screen, &dst_rect);
        dst_rect = (SDL_Rect) { CENTER_X_SCREEN(auth_txt->w), title_txt->h*2+4 };
        SDL_BlitSurface(auth_txt, NULL, Screen, &dst_rect);

        dst_rect = (SDL_Rect) {
            CENTER_X_SCREEN(resume_dark_txt->w), SCREEN_H/2-txt_ch_h*3/2 };
        if ( playing &&
             m_x >= dst_rect.x && m_x < dst_rect.x + resume_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            SDL_SetColors(resume_dark_txt, &mouse_c, 0, 1);
            SDL_SetColors(resume_txt, &mouse_c, 0, 1);
            if (m_b)
            {
                NextState = GamePlay;
                finished = 1;
            }
        }
        else
        {
            SDL_SetColors(resume_dark_txt, &black_c, 0, 1);
            SDL_SetColors(resume_txt, &black_c, 0, 1);
        }
        if (choice != 0)
        {
            int x = dst_rect.x, y = dst_rect.y;
            SDL_BlitSurface(resume_dark_txt, NULL, Screen, &dst_rect);
            src_rect.x = 0;  src_rect.y = 0;
            TTF_SizeText(opt_fnt, "R", (int *)&src_rect.w, (int *)&src_rect.h);
            dst_rect.x = x;  dst_rect.y = y;
            SDL_BlitSurface(resume_txt, &src_rect, Screen, &dst_rect);
        }
        else
        {
            SDL_BlitSurface(resume_txt, NULL, Screen, &dst_rect);
        }

        dst_rect.x = CENTER_X_SCREEN(new_dark_txt->w);
        dst_rect.y += OPTION_TEXT_DIST_MULT * txt_ch_h;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + new_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            SDL_SetColors(new_dark_txt, &mouse_c, 0, 1);
            SDL_SetColors(new_txt, &mouse_c, 0, 1);
            if (m_b)
            {
                if (playing)
                {
                    NextState = ConfirmMessage;
                    PreviousState = IntroScreen;
                    AffirmState = OptionsScreen;
                    state_arg = abandon_prompt;
                }
                else
                {
                    NextState = OptionsScreen;
                    PreviousState = IntroScreen;
                }
                finished = 1;
            }
        }
        else
        {
            SDL_SetColors(new_dark_txt, &black_c, 0, 1);
            SDL_SetColors(new_txt, &black_c, 0, 1);
        }
        if (choice != 1)
        {
            int x = dst_rect.x, y = dst_rect.y;
            SDL_BlitSurface(new_dark_txt, NULL, Screen, &dst_rect);
            src_rect.x = 0;  src_rect.y = 0;
            TTF_SizeText(opt_fnt, "N", (int *)&src_rect.w, (int *)&src_rect.h);
            dst_rect.x = x;  dst_rect.y = y;
            SDL_BlitSurface(new_txt, &src_rect, Screen, &dst_rect);
        }
        else
        {
            SDL_BlitSurface(new_txt, NULL, Screen, &dst_rect);
        }

        dst_rect.x = CENTER_X_SCREEN(quit_dark_txt->w);
        dst_rect.y += OPTION_TEXT_DIST_MULT * txt_ch_h;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + quit_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            SDL_SetColors(quit_dark_txt, &mouse_c, 0, 1);
            SDL_SetColors(quit_txt, &mouse_c, 0, 1);
            if (m_b)
            {
                NextState = ConfirmMessage;
                PreviousState = IntroScreen;
                AffirmState = NULL;
                state_arg = quit_prompt;
                finished = 1;
            }
        }
        else
        {
            SDL_SetColors(quit_dark_txt, &black_c, 0, 1);
            SDL_SetColors(quit_txt, &black_c, 0, 1);
        }
        if (choice != 2)
        {
            int x = dst_rect.x, y = dst_rect.y;
            SDL_BlitSurface(quit_dark_txt, NULL, Screen, &dst_rect);
            src_rect.x = 0;  src_rect.y = 0;
            TTF_SizeText(opt_fnt, "Q", (int *)&src_rect.w, (int *)&src_rect.h);
            dst_rect.x = x;  dst_rect.y = y;
            SDL_BlitSurface(quit_txt, &src_rect, Screen, &dst_rect);
        }
        else
        {
            SDL_BlitSurface(quit_txt, NULL, Screen, &dst_rect);
        }

        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        SDL_Delay(1);

        m_b = 0;

        while (SDL_PollEvent(&evt))
            switch (evt.type)
            {
                case SDL_QUIT:
                    NextState = NULL;
                    finished = 1;
                    break;

                case SDL_KEYDOWN:
                    switch (evt.key.keysym.sym)
                    {
                        case SDLK_r:
                            if(playing) {
                                NextState = GamePlay;
                                finished = 1;
                            }
                            break;

                        case SDLK_n:
                            if (playing)
                            {
                                NextState = ConfirmMessage;
                                PreviousState = IntroScreen;
                                AffirmState = OptionsScreen;
                                state_arg = abandon_prompt;
                                finished = 1;
                            }
                            else
                            {
                                NextState = OptionsScreen;
                                PreviousState = IntroScreen;
                                finished = 1;
                            }
                            break;

                        case SDLK_q:
                            NextState = ConfirmMessage;
                            /*
                             * This is for returning to the current screen
                             * if the player cancels the confirm prompt.
                             */
                            PreviousState = IntroScreen;
                            AffirmState = NULL;
                            state_arg = quit_prompt;
                            finished = 1;
                            break;

                        case SDLK_ESCAPE:
                            NextState = ConfirmMessage;
                            PreviousState = IntroScreen;
                            AffirmState = NULL;
                            state_arg = quit_prompt;
                            finished = 1;
                            break;

                        case SDLK_DOWN:
                            choice++;
                            if (choice == n_choices)
                                choice = (playing ? 0 : 1);
                            break;

                        case SDLK_UP:
                            choice--;
                            if (choice < (playing ? 0 : 1))
                                choice = n_choices - 1;
                            break;


                        case SDLK_RETURN:
                            if (choice == 0)
                            {
                                NextState = GamePlay;
                                finished = 1;
                            }
                            else if (choice == 1)
                            {
                                if (playing)
                                {
                                    NextState = ConfirmMessage;
                                    PreviousState = IntroScreen;
                                    AffirmState = OptionsScreen;
                                    state_arg = abandon_prompt;
                                }
                                else
                                {
                                    NextState = OptionsScreen;
                                    PreviousState = IntroScreen;
                                }
                                finished = 1;
                            }
                            else if (choice == 2)
                            {
                                NextState = ConfirmMessage;
                                PreviousState = IntroScreen;
                                AffirmState = NULL;
                                state_arg = quit_prompt;
                                finished = 1;
                            }
                            break;

                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (evt.button.button == 1)
                        m_b = 1;
                    break;

            }
    }

    TTF_CloseFont(hdr_fnt);
    TTF_CloseFont(opt_fnt);
    SDL_FreeSurface(title_txt);
    SDL_FreeSurface(auth_txt);
    SDL_FreeSurface(new_txt);
    SDL_FreeSurface(quit_txt);
    SDL_FreeSurface(resume_txt);
    SDL_FreeSurface(new_dark_txt);
    SDL_FreeSurface(quit_dark_txt);
    SDL_FreeSurface(resume_dark_txt);

    if (NextState)
        return 0;

    return 1;
}


void UpdateNumberText( char *str, SDL_Surface *sfc, SDL_Surface *choice_sfc,
                       int cw, int ch  )
{
    SDL_Rect src_rect,
             dst_rect;
    int i = 0;

    SDL_FillRect(sfc, NULL, black);
    dst_rect.x = 0;
    dst_rect.y = 0;
    for (i = 0; i < strlen(str); ++i)
    {
        src_rect.x = (str[i] - '0') * cw;
        src_rect.y = 0;
        src_rect.w = cw;
        src_rect.h = ch;
        SDL_BlitSurface(choice_sfc, &src_rect, sfc, &dst_rect);
        dst_rect.x += cw;
    }
}


int OptionsScreen()
{
    TTF_Font *hdr_fnt = NULL,
             *opt_fnt = NULL;
    SDL_Surface *title_txt = NULL,
                *help_txt = NULL,
                *dist_prompt_txt = NULL,
                *level_prompt_txt = NULL,
                *level_txt = NULL,
                *level_choice_txt = NULL,
                *start_prompt_txt = NULL,
                *start_txt = NULL,
                *start_choice_txt = NULL,
                *graphics_prompt_txt = NULL,
                *ok_txt = NULL,
                *cancel_txt = NULL,
                *prompt_a_txt = NULL,
                *prompt_b_txt = NULL,
                *adjust_txt = NULL;
    SDL_Color option_fg = { 63, 255, 63 };
    SDL_Rect src_rect,
             dst_rect;
    SDL_Event evt;
    int finished = 0,
        txt_ch_w = 0,
        txt_ch_h = 0,
        n_choices = 4,
        choice = 0,
        m_x = 0,
        m_y = 0,
        m_b = 0,
        i = 0;
    char *dist_strs[] = { "10%", "25%", "50%", "75%", "100%" },
         *graphic_strs[] = { "On", "Off" },
         level_str[16] = "",
         start_str[16] = "";
    float dist_choices[] = { 0.1f, 0.25f, 0.5f, 0.75f, 1.0f };
    size_t num_dist_choices = sizeof(dist_choices) / sizeof(float),
           num_graphics = 2;
    SDL_Surface *dist_txts[sizeof(dist_choices) / sizeof(float)] = { NULL },
                *graphic_txts[2] = { NULL };
    int chosen_dist = exit_choice,
        num_levels = total_num_levels,
        start = start_level,
        graphics = fast_graphics;

    hdr_fnt = TTF_OpenFont(FONT_NAME, 40);
    opt_fnt = TTF_OpenFont(FONT_NAME, 20);

    title_txt = TTF_RenderText_Shaded(hdr_fnt, "Options", white_c, black_c);
    help_txt = TTF_RenderText_Shaded( opt_fnt,
                                       "Use arrow keys or mouse to adjust",
                                       white_c, black_c );
    dist_prompt_txt = TTF_RenderText_Shaded( opt_fnt, "Exit distance:",
                                             white_c, black_c);
    for (i = 0; i < num_dist_choices; ++i)
        dist_txts[i] = TTF_RenderText_Shaded( opt_fnt, dist_strs[i],
                                              option_fg, black_c );
    level_prompt_txt = TTF_RenderText_Shaded( opt_fnt, "Number of levels:",
                                              white_c, black_c );
    level_choice_txt = TTF_RenderText_Shaded( opt_fnt, "0123456789",
                                              option_fg, black_c );
    start_prompt_txt = TTF_RenderText_Shaded( opt_fnt, "Starting level:",
                                              white_c, black_c );
    start_choice_txt = TTF_RenderText_Shaded( opt_fnt, "0123456789",
                                              option_fg, black_c );
    graphics_prompt_txt = TTF_RenderText_Shaded( opt_fnt, "Advanced graphics:",
                                                 white_c, black_c );
    for (i = 0; i < num_graphics; ++i)
        graphic_txts[i] = TTF_RenderText_Shaded( opt_fnt, graphic_strs[i],
                                                 option_fg, black_c );
    ok_txt = TTF_RenderText_Shaded(opt_fnt, "OK", white_c, black_c);
    cancel_txt = TTF_RenderText_Shaded( opt_fnt, "Cancel",
                                        white_c, black_c );
    prompt_a_txt = TTF_RenderText_Shaded( opt_fnt,
                                          "Press Enter key to accept values",
                                          white_c, black_c );
    prompt_b_txt = TTF_RenderText_Shaded( opt_fnt,
                                          "Escape cancels to previous screen",
                                          white_c, black_c );

    TTF_SizeText(opt_fnt, "0", &txt_ch_w, &txt_ch_h);

    sprintf(level_str, "%d", num_levels);
    level_txt = NewSurface( SCREEN_FLAGS, Screen->format,
                            txt_ch_w * (sizeof(level_str) - 1), txt_ch_h );
    UpdateNumberText( level_str, level_txt, level_choice_txt,
                      txt_ch_w, txt_ch_h );
    sprintf(start_str, "%d", start);
    start_txt = NewSurface( SCREEN_FLAGS, Screen->format,
                            txt_ch_w * (sizeof(start_str) - 1), txt_ch_h );
    UpdateNumberText( start_str, start_txt, start_choice_txt,
                      txt_ch_w, txt_ch_h );

    adjust_txt = TTF_RenderText_Shaded( opt_fnt, "<>",
                                        white_c, black_c );

    while (!finished)
    {
        SDL_GetMouseState(&m_x, &m_y);

        SDL_FillRect(Screen, NULL, black);

        dst_rect.x = CENTER_X_SCREEN(title_txt->w);
        dst_rect.y = title_txt->h;
        SDL_BlitSurface(title_txt, NULL, Screen, &dst_rect);

        dst_rect.x = CENTER_X_SCREEN(help_txt->w);
        dst_rect.y = SCREEN_H / 3 - txt_ch_h;
        SDL_BlitSurface(help_txt, NULL, Screen, &dst_rect);

        dst_rect.x = SCREEN_W / 2 - dist_prompt_txt->w;
        dst_rect.y += txt_ch_h * OPTION_TEXT_DIST_MULT;
        SDL_BlitSurface(dist_prompt_txt, NULL, Screen, &dst_rect);
        dst_rect.x += dist_prompt_txt->w + txt_ch_w * 4;
        if (choice == 0)
        {
            dst_rect.x -= txt_ch_w * 3;
            SDL_BlitSurface(adjust_txt, NULL, Screen, &dst_rect);
            dst_rect.x += txt_ch_w * 3;
        }
        if ( m_x >= dst_rect.x &&
             m_x < dst_rect.x + dist_txts[chosen_dist]->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if ((m_b & 1 || m_b & 2) && chosen_dist < num_dist_choices - 1)
                chosen_dist++;
            if ((m_b & 4 || m_b & 8) && chosen_dist > 0)
                chosen_dist--;
            SDL_SetColors(dist_txts[chosen_dist], &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(dist_txts[chosen_dist], &black_c, 0, 1);
        SDL_BlitSurface(dist_txts[chosen_dist], NULL, Screen, &dst_rect);

        dst_rect.x = SCREEN_W / 2 - level_prompt_txt->w;
        dst_rect.y += txt_ch_h * OPTION_TEXT_DIST_MULT;
        SDL_BlitSurface(level_prompt_txt, NULL, Screen, &dst_rect);
        dst_rect.x += level_prompt_txt->w + txt_ch_w * 4;
        if (choice == 1)
        {
            dst_rect.x -= txt_ch_w * 3;
            SDL_BlitSurface(adjust_txt, NULL, Screen, &dst_rect);
            dst_rect.x += txt_ch_w * 3;
        }
        if ( m_x >= dst_rect.x &&
             m_x < dst_rect.x + txt_ch_w * strlen(level_str) &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if (m_b & 1 || m_b & 2)
            {
                num_levels++;
                sprintf(level_str, "%d", num_levels);
            }
            if ((m_b & 4 || m_b & 8) && num_levels > 2)
            {
                num_levels--;
                sprintf(level_str, "%d", num_levels);
                if (num_levels < start)
                {
                    start = num_levels;
                    sprintf(start_str, "%d", start);
                }
            }
            SDL_SetColors(level_choice_txt, &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(level_choice_txt, &black_c, 0, 1);
        UpdateNumberText( level_str, level_txt, level_choice_txt,
                          txt_ch_w, txt_ch_h );
        SDL_BlitSurface(level_txt, NULL, Screen, &dst_rect);

        dst_rect.x = SCREEN_W / 2 - start_prompt_txt->w;
        dst_rect.y += txt_ch_h * OPTION_TEXT_DIST_MULT;
        SDL_BlitSurface(start_prompt_txt, NULL, Screen, &dst_rect);
        dst_rect.x += start_prompt_txt->w + txt_ch_w * 4;
        if (choice == 2)
        {
            dst_rect.x -= txt_ch_w * 3;
            SDL_BlitSurface(adjust_txt, NULL, Screen, &dst_rect);
            dst_rect.x += txt_ch_w * 3;
        }
        if ( m_x >= dst_rect.x &&
             m_x < dst_rect.x + txt_ch_w * strlen(start_str) &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if ((m_b & 1 || m_b & 2) && start < num_levels)
            {
                start++;
                sprintf(start_str, "%d", start);
            }
            if ((m_b & 4 || m_b & 8) && start > 1)
            {
                start--;
                sprintf(start_str, "%d", start);
            }
            SDL_SetColors(start_choice_txt, &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(start_choice_txt, &black_c, 0, 1);
        UpdateNumberText( start_str, start_txt, start_choice_txt,
                          txt_ch_w, txt_ch_h );
        SDL_BlitSurface(start_txt, NULL, Screen, &dst_rect);

        dst_rect.x = SCREEN_W / 2 - graphics_prompt_txt->w;
        dst_rect.y += txt_ch_h * OPTION_TEXT_DIST_MULT;
        SDL_BlitSurface(graphics_prompt_txt, NULL, Screen, &dst_rect);
        dst_rect.x += graphics_prompt_txt->w + txt_ch_w * 4;
        if (choice == 3)
        {
            dst_rect.x -= txt_ch_w * 3;
            SDL_BlitSurface(adjust_txt, NULL, Screen, &dst_rect);
            dst_rect.x += txt_ch_w * 3;
        }
        if ( m_x >= dst_rect.x &&
             m_x < dst_rect.x + graphic_txts[graphics]->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if (m_b & 1 || m_b & 2)
                graphics = (graphics + 1) % num_graphics;
            if (m_b & 4 || m_b & 8)
                graphics = (graphics + num_graphics - 1) % num_graphics;
            SDL_SetColors(graphic_txts[graphics], &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(graphic_txts[graphics], &black_c, 0, 1);
        SDL_BlitSurface(graphic_txts[graphics], NULL, Screen, &dst_rect);

        dst_rect.x = SCREEN_W / 3 - ok_txt->w / 2;
        dst_rect.y += txt_ch_h * 2;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + ok_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if (m_b & 1)
            {
                exit_choice = chosen_dist;
                exit_mult = dist_choices[exit_choice];
                total_num_levels = num_levels;
                start_level = start;
                fast_graphics = graphics;
                NextState = GamePlay;
                StartNewMaze();
                finished = 1;
            }
            SDL_SetColors(ok_txt, &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(ok_txt, &black_c, 0, 1);
        SDL_BlitSurface(ok_txt, NULL, Screen, &dst_rect);
        dst_rect.x = SCREEN_W * 2 / 3 - cancel_txt->w / 2;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + cancel_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + txt_ch_h )
        {
            if (m_b & 1)
            {
                NextState = IntroScreen;
                finished = 1;
            }
            SDL_SetColors(cancel_txt, &mouse_c, 0, 1);
        }
        else
            SDL_SetColors(cancel_txt, &black_c, 0, 1);
        SDL_BlitSurface(cancel_txt, NULL, Screen, &dst_rect);

        dst_rect.x = CENTER_X_SCREEN(prompt_a_txt->w);
        dst_rect.y = SCREEN_H - prompt_a_txt->h * 2;
        SDL_BlitSurface(prompt_a_txt, NULL, Screen, &dst_rect);

        dst_rect.x = CENTER_X_SCREEN(prompt_b_txt->w);
        dst_rect.y = SCREEN_H - prompt_b_txt->h;
        SDL_BlitSurface(prompt_b_txt, NULL, Screen, &dst_rect);

        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        SDL_Delay(1);

        m_b = 0;

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
                case SDL_QUIT:
                    NextState = NULL;
                    finished = 1;
                    break;

                case SDL_KEYDOWN:
                    switch (evt.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            NextState = IntroScreen;
                            finished = 1;
                            break;

                        case SDLK_RETURN:
                            exit_choice = chosen_dist;
                            exit_mult = dist_choices[exit_choice];
                            total_num_levels = num_levels;
                            start_level = start;
                            fast_graphics = graphics;
                            NextState = GamePlay;
                            StartNewMaze();
                            finished = 1;
                            break;

                        case SDLK_q:
                            NextState = ConfirmMessage;
                            PreviousState = OptionsScreen;
                            AffirmState = NULL;
                            state_arg = quit_prompt;
                            finished = 1;
                            break;

                        case SDLK_LEFT:
                            switch (choice)
                            {
                                case 0:
                                    if (chosen_dist > 0)
                                        chosen_dist--;
                                    break;

                                case 1:
                                    if (num_levels > 2)
                                    {
                                        num_levels--;
                                        sprintf(level_str, "%d", num_levels);
                                        if (num_levels < start)
                                        {
                                            start--;
                                            sprintf(start_str, "%d", start);
                                        }
                                    }
                                    break;

                                case 2:
                                    if (start > 1)
                                    {
                                        start--;
                                        sprintf(start_str, "%d", start);
                                    }
                                    break;

                                case 3:
                                    graphics = (graphics + num_graphics - 1) %
                                               num_graphics;
                                    break;

                            }
                            break;

                        case SDLK_RIGHT:
                            switch (choice)
                            {
                                case 0:
                                    if (chosen_dist < num_dist_choices - 1)
                                        chosen_dist++;
                                    break;

                                case 1:
                                    num_levels++;
                                    sprintf(level_str, "%d", num_levels);
                                    break;

                                case 2:
                                    if (start < num_levels)
                                    {
                                        start++;
                                        sprintf(start_str, "%d", start);
                                    }
                                    break;

                                case 3:
                                    graphics = (graphics + 1) % num_graphics;
                                    break;

                            }
                            break;

                        case SDLK_DOWN:
                            choice = (choice + 1) % n_choices;
                            break;

                        case SDLK_UP:
                            if (--choice < 0)
                                choice = n_choices - 1;
                            break;

                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (evt.button.button == 1)
                        m_b |= 1;
                    else if (evt.button.button == 4)
                        m_b |= 2;
                    else if (evt.button.button == 3)
                        m_b |= 4;
                    else if (evt.button.button == 5)
                        m_b |= 8;
                    break;

            }
        }
    }

    TTF_CloseFont(hdr_fnt);
    TTF_CloseFont(opt_fnt);
    SDL_FreeSurface(title_txt);
    SDL_FreeSurface(help_txt);
    SDL_FreeSurface(dist_prompt_txt);
    for (i = 0; i < num_dist_choices; ++i)
        SDL_FreeSurface(dist_txts[i]);
    SDL_FreeSurface(level_prompt_txt);
    SDL_FreeSurface(level_txt);
    SDL_FreeSurface(level_choice_txt);
    SDL_FreeSurface(start_prompt_txt);
    SDL_FreeSurface(start_txt);
    SDL_FreeSurface(start_choice_txt);
    SDL_FreeSurface(graphics_prompt_txt);
    for (i = 0; i < num_graphics; ++i)
        SDL_FreeSurface(graphic_txts[i]);
    SDL_FreeSurface(ok_txt);
    SDL_FreeSurface(cancel_txt);
    SDL_FreeSurface(prompt_a_txt);
    SDL_FreeSurface(prompt_b_txt);
    SDL_FreeSurface(adjust_txt);

    if (NextState)
        return 0;

    return 1;
}


void ZoomIn()
{
    if (!follow_player)
    {
        full_zoom += FULL_ZOOM_SPEED;
        if (full_zoom > MAX_FULL_ZOOM)
            full_zoom = MAX_FULL_ZOOM;
    }
    else
    {
        player_zoom += player_zoom_speed;
    }
    redraw_maze = 1;
}


void ZoomOut()
{
    if (!follow_player)
    {
        full_zoom -= FULL_ZOOM_SPEED;
        if (full_zoom < MIN_FULL_ZOOM)
            full_zoom = MIN_FULL_ZOOM;
    }
    else
    {
        player_zoom -= player_zoom_speed;
        if (player_zoom < MIN_FULL_ZOOM)
            player_zoom = MIN_FULL_ZOOM;
    }
    redraw_maze = 1;
}


int GamePlay()
{
    TTF_Font *fnt = NULL;
    SDL_Surface *sfc = NULL;
    SDL_Rect *rect;
    SDL_Event evt;
    const int hold_ticks = 1000 / TARGET_FPS;
    int player_move_ticks = 0,
        m_b1_pressed_alt = 0,
        mouse_b1_pressed = 0,
        can_click_ctrl = 0,
        draw_quad = 0,
        blit_flag = 0,
        finished = 0,
        x_offset = 0,
        y_offset = 0,
        m_hit_x = 0,
        m_hit_y = 0,
        to_x = 0,
        to_y = 0,
        quad = 0,
        m_x = 0,
        m_y = 0,
        x0 = 0,
        y0 = 0,
        x1 = 0,
        y1 = 0,
        x2 = 0,
        y2 = 0,
        x3 = 0,
        y3 = 0,
        i = 0;
    float ang1 = 0.0f,
          ang2 = 0.0f,
          zoom = 0.0f;
    Point_t *from = NULL,
            *to = NULL;
    Uint32 ticks = 0,
           c = 0;
    Uint8 hide_r = 0,
          hide_g = 0,
          hide_b = 0,
          a = 0;

    playing = 1;

    SDL_GetRGB(HIDE_COLOR, Screen->format, &hide_r, &hide_g, &hide_b);
    tick_count = SDL_GetTicks();

    while (!finished)
    {
        SDL_GetMouseState(&m_x, &m_y);

        SDL_FillRect(Screen, NULL, black);
        if (!fast_graphics || redraw_maze)
        {
            SDL_FillRect(maze_sfc, NULL, black);
            for (quad = 0; quad < NUM_QUADS; ++quad)
            {
                from = &copy_points[quads[quad][0]];
                to = &copy_points[quads[quad][1]];
                ang1 = atan2( to->y / to->z - from->y / from->z,
                              to->x / to->z - from->x / from->z );
                to = &copy_points[quads[quad][3]];
                ang2 = atan2( to->y / to->z - from->y / from->z,
                              to->x / to->z - from->x / from->z );

                draw_quad = 0;
                if (player.level > 0)
                {
                    if ( (ang2 - ang1 > 0.0f && ang2 - ang1 < M_PI) ||
                         (ang2 - ang1 > -2.0f * M_PI && ang2 - ang1 < -M_PI) )
                        draw_quad = 1;
                }
                else
                {
                    if ( (ang2 - ang1 > M_PI && ang2 - ang1 < 2.0f * M_PI) ||
                         (ang2 - ang1 > -1.0f * M_PI && ang2 - ang1 < 0) )
                        draw_quad = 1;
                }
                if (draw_quad)
                {
                    SDL_BlitSurface( quad_sfcs[quad][player.level], NULL,
                                     quid_sfcs[player.level], NULL );
                    if (!show_full_map && player.level > 0)
                        SDL_BlitSurface( hide_sfcs[quad][player.level], NULL,
                                         quid_sfcs[player.level], NULL );
                    DrawOutlinedQuadWithDecals( maze_sfc,
                                                quid_sfcs[player.level],
                                                quad, grey );
                }
            }
            redraw_maze = 0;
        }
        SDL_BlitSurface(maze_sfc, NULL, Screen, NULL);

        if (PlayerOnExit(&exit_up_rooms[player.level]))
            SDL_BlitSurface(ascend_txt_sfc, NULL, Screen, &ascend_rect);
        if (player.level > 0 && PlayerOnExit(&exit_dn_rooms[player.level]))
            SDL_BlitSurface(descend_txt_sfc, NULL, Screen, &descend_rect);
        if ( player.level == total_num_levels - 1 &&
             PlayerOnExit(&exit_final_room) )
            SDL_BlitSurface(exit_txt_sfc, NULL, Screen, &exit_rect);

        ticks = MAX(1, SDL_GetTicks());
        if (!fast_graphics)
        {
            RotatePalette( exit_up_sfc, exit_up_pal, exit_up_pal_cpy,
                           SPRITE_SIZE, ticks );
            RotatePalette( exit_dn_sfc, exit_dn_pal, exit_dn_pal_cpy,
                           SPRITE_SIZE, ticks );
            RotatePalette( exit_final_sfc, exit_final_pal, exit_final_pal_cpy,
                           SPRITE_SIZE, ticks );
        }

        if (fade_start > 0)
        {
            if  (ticks - fade_start < LEVEL_CHANGE_TIME)
            {
                SDL_BlitSurface(Screen, NULL, fade_sfc, NULL);
                SDL_SetAlpha( fade_sfc, SDL_SRCALPHA,
                              (ticks - fade_start) * 256 / LEVEL_CHANGE_TIME );
                SDL_BlitSurface(prev_sfc, NULL, Screen, NULL);
                SDL_BlitSurface(fade_sfc, NULL, Screen, NULL);
            }
            else
                fade_start = 0;
        }
        if (show_ctrl_buttons)
        {
            for (x1 = 0; x1 <= 1; ++x1)
            {
                for (i = 0; i < NUM_BUTTONS; ++i)
                {
                    sfc = button_sfcs[i];
                    rect = &button_rects[i];
                    blit_flag = 0;
                    if (player.level > 0)
                    {
                        if ( x1 == 1 && mouse_b1_pressed && can_click_ctrl &&
                             m_hit_x > rect->x &&
                             m_hit_x < rect->x + rect->w - 1 &&
                             m_hit_y > rect->y &&
                             m_hit_y < rect->y + rect->h - 1 &&
                             m_x > rect->x && m_x < rect->x + rect->w - 1 &&
                             m_y > rect->y && m_y < rect->y + rect->h - 1 )
                        {
                            blit_flag = 1;
                            SDL_SetColors(sfc, &white_c, 1, 1);
                            if (i == BUTTON_ZOOM_OUT)
                            {
                                ZoomOut();
                            }
                            else if (i == BUTTON_ZOOM_IN)
                            {
                                ZoomIn();
                            }
                            else if ( i != BUTTON_ZOOM_IN &&
                                      i != BUTTON_ZOOM_OUT )
                            {
                                if ( rotating == ROTATE_NONE &&
                                     ticks - rotation_start > MAZE_ROTATE_TIME )
                                {
                                    follow_player = 0;
                                    if (player.traversing == TRAVERSE_FOLLOW)
                                        player.traversing = TRAVERSE_CENTER;
                                    fast_move = FAST_ROTATE_MAZE;
                                    switch (i)
                                    {
                                        case BUTTON_UP:
                                            rotating = ROTATE_UP;
                                            break;

                                        case BUTTON_DOWN:
                                            rotating = ROTATE_DOWN;
                                            break;

                                        case BUTTON_LEFT:
                                            rotating = ROTATE_LEFT;
                                            break;

                                        case BUTTON_RIGHT:
                                            rotating = ROTATE_RIGHT;
                                            break;

                                        case BUTTON_CWISE:
                                            rotating = ROTATE_CWISE;
                                            break;

                                        case BUTTON_CCWISE:
                                            rotating = ROTATE_CCWISE;
                                            break;

                                        default:
                                            fprintf( stderr,
                                                     "Unrecognized control "
                                                     "button\n" );
                                            break;

                                    }
                                    rotation_start = MAX(1, SDL_GetTicks());
                                }
                            }
                        }
                        else if (x1 == 0)
                        {
                            blit_flag = 1;
                            if ( m_x > rect->x &&
                                 m_x < rect->x + rect->w - 1 &&
                                 m_y > rect->y &&
                                 m_y < rect->y + rect->h - 1 )
                            {
                                SDL_SetColors(sfc, &mouse_c, 0, 1);
                                SDL_SetColors(sfc, &grey_c, 1, 1);
                            }
                            else
                            {
                                SDL_SetColors(sfc, &black_c, 0, 1);
                                SDL_SetColors(sfc, &grey_c, 1, 1);
                            }
                        }
                    }
                    else
                    {
                        SDL_SetColors(sfc, &dk_grey_c, 0, 1);
                        SDL_SetColors(sfc, &grey_c, 1, 1);
                        blit_flag = 1;
                    }
                    if (blit_flag)
                        SDL_BlitSurface(sfc, NULL, Screen, rect);
                }
            }
        }

        SDL_UpdateRect(Screen, 0, 0, 0, 0);

        if (m_b1_pressed_alt && player.level == 0)
        {
            redraw_maze = 1;

            player.level++;
            maze_size = player.level * 4 - 1;
            player.room.x = 1;
            player.room.y = 1;
            player.room.quad = 0;
            ResetObjects();

            if (!fast_graphics)
            {
                fade_start = MAX(1, SDL_GetTicks());
                SDL_BlitSurface(Screen, NULL, prev_sfc, NULL);
            }
        }
        else if ( mouse_b1_pressed && player.room.quad == current_quad &&
                  player.x_dir == 0 && player.y_dir == 0 &&
                  player.move_ticks_start == 0 && rotating == ROTATE_NONE )
        {
            if (follow_player)
            {
                zoom = player_zoom;
                ComputeFollowOffsets( zoom, center.z - 1.0f, maze_size,
                                      &x_offset, &y_offset );
            }
            else
            {
                zoom = full_zoom;
                x_offset = y_offset = 0;
            }

            to = &points[quads[current_quad][0]];
            x0 = TO_SFC_COORD(to, x, zoom) + SCREEN_W / 2 + x_offset;
            y0 = TO_SFC_COORD(to, y, zoom) + SCREEN_H / 2 + y_offset;
            to = &points[quads[current_quad][1]];
            x1 = TO_SFC_COORD(to, x, zoom) + SCREEN_W / 2 + x_offset;
            y1 = TO_SFC_COORD(to, y, zoom) + SCREEN_H / 2 + y_offset;
            to = &points[quads[current_quad][2]];
            x2 = TO_SFC_COORD(to, x, zoom) + SCREEN_W / 2 + x_offset;
            y2 = TO_SFC_COORD(to, y, zoom) + SCREEN_H / 2 + y_offset;
            to = &points[quads[current_quad][3]];
            x3 = TO_SFC_COORD(to, x, zoom) + SCREEN_W / 2 + x_offset;
            y3 = TO_SFC_COORD(to, y, zoom) + SCREEN_H / 2 + y_offset;

            to_x = -1;
            to_y = -1;
            switch (player.orient)
            {
                case UP:
                    if (m_x >= x0 && m_x < x2 && m_y >= y0 && m_y < y2)
                    {
                        to_x = (m_x - x0) * maze_size / (x2 - x0);
                        to_y = (m_y - y0) * maze_size / (y2 - y0);
#ifdef DEBUG_DIAGNOSTICS
                        printf("%d %d\n", to_x, to_y);
#endif
                    }
                    break;

                case RIGHT:
                    if (m_x >= x1 && m_x < x3 && m_y > y1 && m_y <= y3)
                    {
                        to_y = (m_x - x0) * maze_size / (x2 - x0);
                        to_x = (m_y - y0) * maze_size / (y2 - y0);
#ifdef DEBUG_DIAGNOSTICS
                        printf("%d %d\n", to_x, to_y);
#endif
                    }
                    break;

                case DOWN:
                    if (m_x > x2 && m_x <= x0 && m_y > y2 && m_y <= y0)
                    {
                        to_x = (m_x - x0) * maze_size / (x2 - x0);
                        to_y = (m_y - y0) * maze_size / (y2 - y0);
#ifdef DEBUG_DIAGNOSTICS
                        printf("%d %d\n", to_x, to_y);
#endif
                    }
                    break;

                case LEFT:
                    if (m_x > x3 && m_x <= x1 && m_y >= y3 && m_y < y1)
                    {
                        to_y = (m_x - x0) * maze_size / (x2 - x0);
                        to_x = (m_y - y0) * maze_size / (y2 - y0);
#ifdef DEBUG_DIAGNOSTICS
                        printf("%d %d\n", to_x, to_y);
#endif
                    }
                    break;

            }

            if (to_x > -1 && to_y > -1)
                if (to_x == player.room.x && to_y == player.room.y)
                {
                    if (m_b1_pressed_alt && PlayerOnExit(&exit_final_room))
                    {
                        redraw_maze = 1;
                        NextState = WinScreen;
                        if (!fast_graphics)
                        {
                            fade_start = MAX(1, SDL_GetTicks());
                            SDL_BlitSurface(Screen, NULL, prev_sfc, NULL);
                        }
                        mouse_b1_pressed = 0;
                        finished = 1;
                    }
                    else if ( m_b1_pressed_alt &&
                              PlayerOnExit(&exit_up_rooms[player.level]) )
                    {
                        redraw_maze = 1;
                        if (player.level < total_num_levels - 1)
                        {
                            player.level++;
                            maze_size = player.level * 4 - 1;
                            player.room.x += 2;
                            player.room.y += 2;
                            ResetObjects();

                            if (!fast_graphics)
                            {
                                fade_start = MAX(1, SDL_GetTicks());
                                SDL_BlitSurface(Screen, NULL, prev_sfc, NULL);
                            }
                        }
                        mouse_b1_pressed = 0;
                    }
                    else if ( m_b1_pressed_alt &&
                              PlayerOnExit(&exit_dn_rooms[player.level]) )
                    {
                        redraw_maze = 1;
                        if (player.level > 1)
                        {
                            player.level--;
                            maze_size = player.level * 4 - 1;
                            player.room.x -= 2;
                            player.room.y -= 2;
                            ResetObjects();

                            if (!fast_graphics)
                            {
                                fade_start = MAX(1, SDL_GetTicks());
                                SDL_BlitSurface(Screen, NULL, prev_sfc, NULL);
                            }
                        }
                        else
                        {
                            player.level = 0;
                            maze_size = player.level * 4 - 1;
                            player.room.x = 0;
                            player.room.y = 0;
                            player.room.quad = 1;
                            follow_player = 0;
                            ResetObjects();

                            if (!fast_graphics)
                            {
                                fade_start = MAX(1, SDL_GetTicks());
                                SDL_BlitSurface(Screen, NULL, prev_sfc, NULL);
                            }
                        }
                        mouse_b1_pressed = 0;
                    }
                    else
                        MovePlayerByMouse(to_x, to_y, m_b1_pressed_alt);
                }
                else
                    MovePlayerByMouse(to_x, to_y, m_b1_pressed_alt);
        }

        if (player.level > 0)
            for (quad = 0; quad < NUM_QUADS; ++quad)
                for (i = 0; i < maze_size * maze_size; ++i)
                {
                    if (unhide_starts[quad][player.level][i] > 0)
                    {
                        ticks = MAX( 1, SDL_GetTicks() -
                                     unhide_starts[quad][player.level][i] );
                        if (fast_graphics)
                        {
                            unhide_starts[quad][player.level][i] = 0xffffffff;
                            if (GetPixel_32( hide_sfcs[quad][player.level],
                                             i % maze_size,
                                             i / maze_size ) == HIDE_COLOR )
                            {
                                SetPixel_32( hide_sfcs[quad][player.level],
                                             i % maze_size,
                                             i / maze_size, UNHIDE_COLOR );
                                redraw_maze = 1;
                            }
                        }
                        else if (ticks >= UNHIDE_TIME)
                        {
                            unhide_starts[quad][player.level][i] = 0xffffffff;
                            SetPixel_32( hide_sfcs[quad][player.level],
                                         i % maze_size,
                                         i / maze_size, UNHIDE_COLOR );
                        }
                        else
                        {
                            a = 255 - 255 * ticks / UNHIDE_TIME;
                            c = SDL_MapRGBA(
                                    hide_sfcs[quad][player.level]->format,
                                    hide_r, hide_g, hide_b, a );
                            SetPixel_32( hide_sfcs[quad][player.level],
                                         i % maze_size,
                                         i / maze_size, c );
                        }
                    }
                }

        if (fast_graphics && player.move_ticks_start > 0)
        {
            if (fast_move == FAST_PLAYER_MOVE)
            {
                if (rotating != ROTATE_NONE)
                {
                    player.room.x = player.to_room.x;
                    player.room.y = player.to_room.y;
                    player.room.quad = player.to_room.quad;
                    if (RotateCube(TRAVERSE_FOLLOW))
                    {
                        rotating = ROTATE_NONE;
                        fast_move = FAST_READY;
                    }
                    player.x_dir = 0;
                    player.y_dir = 0;
                    player.move_x = 0;
                    player.move_y = 0;
                    player.move_x_opp = 0;
                    player.move_y_opp = 0;
                }
                else if (player.room.quad == player.to_room.quad)
                { 
                    fast_move = FAST_READY;
                    player.room.x = player.to_room.x;
                    player.room.y = player.to_room.y;
                    player.x_dir = 0;
                    player.y_dir = 0;
                    player.move_x = 0;
                    player.move_y = 0;
                    player.move_x_opp = 0;
                    player.move_y_opp = 0;
                }
                redraw_maze = 1;
            }
            if (SDL_GetTicks() - player.move_ticks_start > pl_move_time)
            {
                player.move_ticks_start = 0;
                fast_move = FAST_READY;
            }
        }
        else if ( !fast_graphics && player.move_ticks_start > 0 &&
                  SDL_GetTicks() - player.move_ticks_start > pl_move_time )
        {
            player.move_ticks_start = 0;
            player.room.x = player.to_room.x;
            player.room.y = player.to_room.y;
            player.room.quad = player.to_room.quad;
            player.x_dir = 0;
            player.y_dir = 0;
            player.move_x = 0;
            player.move_y = 0;
            player.move_x_opp = 0;
            player.move_y_opp = 0;
            redraw_maze = 1;
        }

        ticks = SDL_GetTicks();

        if (k_up && rotating == ROTATE_NONE && player.level > 0)
        {
            if (k_shift)
            {
                ZoomIn();
            }
            else if (k_ctrl)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_UP;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else
            {
                if (player.room.quad == current_quad)
                {
                    if (player.move_ticks_start == 0)
                    {
                        fast_move = FAST_PLAYER_MOVE;
                        MovePlayerByKey(UP);
                    }
                }
                else
                    RestorePoints();
            }
        }

        if (k_down && rotating == ROTATE_NONE && player.level > 0)
        {
            if (k_shift)
            {
                ZoomOut();
            }
            else if (k_ctrl)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_DOWN;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else
            {
                if (player.room.quad == current_quad)
                {
                    if (player.move_ticks_start == 0)
                    {
                        fast_move = FAST_PLAYER_MOVE;
                        MovePlayerByKey(DOWN);
                    }
                }
                else
                {
                    RestorePoints();
                }
            }
        }

        if (k_left && rotating == ROTATE_NONE && player.level > 0)
        {
            if (k_shift)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_CCWISE;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else if (k_ctrl)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_LEFT;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else
            {
                if (player.room.quad == current_quad)
                {
                    if (player.move_ticks_start == 0)
                    {
                        fast_move = FAST_PLAYER_MOVE;
                        MovePlayerByKey(LEFT);
                    }
                }
                else
                {
                    RestorePoints();
                }
            }
        }

        if (k_right && rotating == ROTATE_NONE && player.level > 0)
        {
            if (k_shift)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_CWISE;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else if (k_ctrl)
            {
                fast_move = FAST_ROTATE_MAZE;
                if (ticks - rotation_start > MAZE_ROTATE_TIME)
                {
                    follow_player = 0;
                    if (player.traversing == TRAVERSE_FOLLOW)
                        player.traversing = TRAVERSE_CENTER;
                    rotating = ROTATE_RIGHT;
                    rotation_start = MAX(1, SDL_GetTicks());
                }
            }
            else
            {
                if (player.room.quad == current_quad)
                {
                    if (player.move_ticks_start == 0)
                    {
                        fast_move = FAST_PLAYER_MOVE;
                        MovePlayerByKey(RIGHT);
                    }
                }
                else
                {
                    RestorePoints();
                }
            }
        }

        if (fast_graphics)
        {
            if (rotating != ROTATE_NONE && fast_move == FAST_ROTATE_MAZE)
            {
                if (RotateCube(TRAVERSE_FOLLOW))
                    fast_move = FAST_FINISH_ROTATE;
            }
            else if ( fast_move == FAST_FINISH_ROTATE &&
                      SDL_GetTicks() - rotation_start > MAZE_ROTATE_TIME )
            {
                fast_move = FAST_READY;
                rotation_start = 0;
                rotating = ROTATE_NONE;
            }

        }
        else if (rotating != ROTATE_NONE && RotateCube(player.traversing))
            rotating = ROTATE_NONE;

        while (SDL_GetTicks() - tick_count < hold_ticks)
        {
            SDL_Delay(1);
        }
        n_frames++;
        tick_count = SDL_GetTicks();
        total_ticks += tick_count - tick_count;

        m_b1_pressed_alt = 0;

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
                case SDL_QUIT:
                    NextState = NULL;
                    finished = 1;

                case SDL_KEYDOWN:
                    switch (evt.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            NextState = IntroScreen;
                            finished = 1;
                            break;

                        case SDLK_UP:
                            k_up = 1;
                            break;

                        case SDLK_DOWN:
                            k_down = 1;
                            break;

                        case SDLK_LEFT:
                            k_left = 1;
                            break;

                        case SDLK_RIGHT:
                            k_right = 1;
                            break;

                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            k_shift++;
                            break;

                        case SDLK_LCTRL:
                        case SDLK_RCTRL:
                            k_ctrl++;
                            break;

                        case SDLK_SPACE:
                            k_spc = 1 - k_spc;
                            break;

                        case SDLK_f:
                            if (player.level > 0)
                                follow_player = 1 - follow_player;
                            if (follow_player)
                            {
                                maze_size = player.level * 4 - 1;
                                RestorePoints();
                            }
                            redraw_maze = 1;
                            break;

                        case SDLK_EQUALS:
                            if (!follow_player)
                                full_zoom = FULL_ZOOM_START;
                            else if (player.level > 0)
                                player_zoom = SPRITE_SIZE * 4 * maze_size;
                            redraw_maze = 1;
                            break;

                        case SDLK_a:
                            RestorePoints();
                            if (player.level == 0)
                            {
                                player.level++;
                                maze_size = player.level * 4 - 1;
                                player.room.x = 1;
                                player.room.y = 1;
                                player.room.quad = 0;
                                ResetObjects();

                                if (!fast_graphics)
                                {
                                    fade_start = MAX(1, SDL_GetTicks());
                                    SDL_BlitSurface( Screen, NULL,
                                                     prev_sfc, NULL );
                                }
                            }
                            else if ( player.level == total_num_levels - 1 &&
                                      PlayerOnExit(&exit_final_room) )
                            {
                                NextState = WinScreen;
                                if (!fast_graphics)
                                {
                                    fade_start = MAX(1, SDL_GetTicks());
                                    SDL_BlitSurface( Screen, NULL,
                                                     prev_sfc, NULL );
                                }
                                finished = 1;
                            }
                            else if (
                                PlayerOnExit(&exit_up_rooms[player.level]) )
                            {
                                if (player.level < total_num_levels - 1)
                                {
                                    player.level++;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x += 2;
                                    player.room.y += 2;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                            }
                            redraw_maze = 1;
                            break;

                        case SDLK_d:
                            RestorePoints();
                            if (PlayerOnExit(&exit_dn_rooms[player.level]))
                            {
                                if (player.level > 1)
                                {
                                    player.level--;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x -= 2;
                                    player.room.y -= 2;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                                else
                                {
                                    player.level = 0;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x = 0;
                                    player.room.y = 0;
                                    player.room.quad = 1;
                                    follow_player = 0;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                            }
                            redraw_maze = 1;
                            break;

                        case SDLK_RETURN:
                            RestorePoints();
                            if (player.level == 0)
                            {
                                player.level++;
                                maze_size = player.level * 4 - 1;
                                player.room.x = 1;
                                player.room.y = 1;
                                player.room.quad = 0;
                                ResetObjects();

                                if (!fast_graphics)
                                {
                                    fade_start = MAX(1, SDL_GetTicks());
                                    SDL_BlitSurface( Screen, NULL,
                                                     prev_sfc, NULL );
                                }
                            }
                            else if ( player.level == total_num_levels - 1 &&
                                      PlayerOnExit(&exit_final_room) )
                            {
                                NextState = WinScreen;
                                if (!fast_graphics)
                                {
                                    fade_start = MAX(1, SDL_GetTicks());
                                    SDL_BlitSurface( Screen, NULL,
                                                     prev_sfc, NULL );
                                }
                                finished = 1;
                            }
                            else if (
                                PlayerOnExit(&exit_up_rooms[player.level]) )
                            {
                                if (player.level < total_num_levels - 1)
                                {
                                    player.level++;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x += 2;
                                    player.room.y += 2;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                            }
                            else if (
                                PlayerOnExit(&exit_dn_rooms[player.level]) )
                            {
                                if (player.level > 1)
                                {
                                    player.level--;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x -= 2;
                                    player.room.y -= 2;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                                else
                                {
                                    player.level = 0;
                                    maze_size = player.level * 4 - 1;
                                    player.room.x = 0;
                                    player.room.y = 0;
                                    player.room.quad = 1;
                                    follow_player = 0;
                                    ResetObjects();

                                    if (!fast_graphics)
                                    {
                                        fade_start = MAX(1, SDL_GetTicks());
                                        SDL_BlitSurface( Screen, NULL,
                                                         prev_sfc, NULL );
                                    }
                                }
                            }
                            redraw_maze = 1;
                            break;

#ifdef DEBUG_SHOW_MAP
                        case SDLK_m:
                            show_full_map = 1 - show_full_map;
                            redraw_maze = 1;
                            break;
#endif

                    }
                    break;

                case SDL_KEYUP:
                    switch (evt.key.keysym.sym)
                    {
                        case SDLK_UP:
                            k_up = 0;
                            break;

                        case SDLK_DOWN:
                            k_down = 0;
                            break;

                        case SDLK_LEFT:
                            k_left = 0;
                            break;

                        case SDLK_RIGHT:
                            k_right = 0;
                            break;

                        case SDLK_LSHIFT:
                        case SDLK_RSHIFT:
                            if (k_shift > 0)
                                --k_shift;
                            break;

                        case SDLK_LCTRL:
                        case SDLK_RCTRL:
                            if (k_ctrl > 0)
                                --k_ctrl;
                            break;

                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (evt.button.button == 1)
                    {
                        mouse_b1_pressed = 1;
                        m_b1_pressed_alt = 1;
                        m_hit_x = m_x;
                        m_hit_y = m_y;
                        can_click_ctrl = 1;
                    }
                    else if (evt.button.button == 3)
                    {
                        show_ctrl_buttons = 1 - show_ctrl_buttons;
                        can_click_ctrl = 0;
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (evt.button.button == 1)
                        mouse_b1_pressed = 0;
                    break;

            }
        }
    }

    if (NextState)
        return 0;

    return 1;
}


int WinScreen()
{
    TTF_Font *win_fnt = NULL;
    SDL_Surface *win_a_txt = NULL,
                *win_b_txt = NULL;
    SDL_Rect src_rect,
             dst_rect;
    SDL_Event evt;
    int finished = 0;
    Uint32 ticks = 0;

    playing = 0;

    win_fnt = TTF_OpenFont(FONT_NAME, 24);
    win_a_txt = TTF_RenderText_Shaded( win_fnt, "You have escaped the cube",
                                       white_c, black_c );
    win_b_txt = TTF_RenderText_Shaded( win_fnt, "Good job!",
                                       white_c, black_c );

    while (!finished)
    {
        SDL_FillRect(Screen, NULL, black);
        dst_rect.x = CENTER_X_SCREEN(win_a_txt->w);
        dst_rect.y = SCREEN_H / 2 - win_a_txt->h;
        SDL_BlitSurface(win_a_txt, NULL, Screen, &dst_rect);
        dst_rect.x = CENTER_X_SCREEN(win_b_txt->w);
        dst_rect.y += win_b_txt->h;
        SDL_BlitSurface(win_b_txt, NULL, Screen, &dst_rect);

        ticks = SDL_GetTicks();
        if (fade_start > 0)
        {
            if  (ticks - fade_start < LEVEL_CHANGE_TIME)
            {
                SDL_BlitSurface(Screen, NULL, fade_sfc, NULL);
                SDL_SetAlpha( fade_sfc, SDL_SRCALPHA,
                              (ticks - fade_start) * 256 / LEVEL_CHANGE_TIME );
                SDL_BlitSurface(prev_sfc, NULL, Screen, NULL);
                SDL_BlitSurface(fade_sfc, NULL, Screen, NULL);
            }
            else
                ticks = 0;
        }

        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        SDL_Delay(1);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
                case SDL_QUIT:
                    NextState = NULL;
                    finished = 1;
                    break;

                case SDL_KEYDOWN:
                    if (evt.key.keysym.sym == SDLK_q)
                    {
                        NextState = ConfirmMessage;
                        PreviousState = WinScreen;
                        AffirmState = NULL;
                        state_arg = quit_prompt;
                        finished = 1;
                    }
                    else
                    {
                        NextState = IntroScreen;
                        finished = 1;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    NextState = IntroScreen;
                    finished = 1;
                    break;

            }
        }
    }

    TTF_CloseFont(win_fnt);
    SDL_FreeSurface(win_a_txt);
    SDL_FreeSurface(win_b_txt);

    if (NextState)
        return 0;

    return 1;
}


int ConfirmMessage()
{
    SDL_Surface *back_sfc = NULL,
                *prompt_txt = NULL,
                *yes_txt = NULL,
                *no_txt = NULL;
    TTF_Font *fnt = NULL;
    SDL_Rect src_rect,
             dst_rect;
    SDL_Event evt;
    int mouse_select = 0,
        current_sel = 0,
        finished = 0,
        old_m_x = 0,
        old_m_y = 0,
        m_x = 0,
        m_y = 0,
        m_b = 0;
    char *msg = (char *)state_arg;
    
    fnt = TTF_OpenFont(FONT_NAME, 20);
    prompt_txt = TTF_RenderText_Shaded(fnt, msg, white_c, black_c);
    yes_txt = TTF_RenderText_Shaded(fnt, "Yes", white_c, black_c);
    no_txt = TTF_RenderText_Shaded(fnt, "No", white_c, black_c);
    back_sfc = NewSurface( SCREEN_FLAGS, Screen->format,
                           prompt_txt->w + 4, prompt_txt->h * 2 + 4 );
    SDL_FillRect(back_sfc, NULL, black);
    DrawLine_32(back_sfc, 0, 0, back_sfc->w - 1, 0, white);
    DrawLine_32(back_sfc, 0, 0, 0, back_sfc->h - 1, white);
    DrawLine_32( back_sfc, back_sfc->w - 1, 0,
                 back_sfc->w - 1, back_sfc->h - 1 , white );
    DrawLine_32( back_sfc, 0, back_sfc->h - 1,
                 back_sfc->w - 1, back_sfc->h - 1 , white );

    while (!finished)
    {
        SDL_GetMouseState(&m_x, &m_y);
        if (m_x != old_m_x || m_y != old_m_y)
            mouse_select = 1;
        else
            mouse_select = 0;
        old_m_x = m_x;
        old_m_y = m_y;

        dst_rect.x = CENTER_X_SCREEN(prompt_txt->w) - 2;
        dst_rect.y = SCREEN_H / 2 - prompt_txt->h - 2;
        SDL_BlitSurface(back_sfc, NULL, Screen, &dst_rect);
        dst_rect.x += 2;  dst_rect.y += 2;
        SDL_BlitSurface(prompt_txt, NULL, Screen, &dst_rect);
        dst_rect.x += prompt_txt->w / 3 - yes_txt->w / 2;
        dst_rect.y += prompt_txt->h;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + yes_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + yes_txt->h )
        {
            if (mouse_select)
                current_sel = 0;
            if (m_b)
            {
                NextState = AffirmState;
                finished = 1;
            }
        }
        else
        {
            SDL_SetColors(yes_txt, &black_c, 0, 1);
        }
        if (current_sel == 0)
        {
            SDL_SetColors(yes_txt, &mouse_c, 0, 1);
            SDL_SetColors(no_txt, &black_c, 0, 1);
        }
        SDL_BlitSurface(yes_txt, NULL, Screen, &dst_rect);
        dst_rect.x += yes_txt->w / 2 + prompt_txt->w / 3 - no_txt->w / 2;
        if ( m_x >= dst_rect.x && m_x < dst_rect.x + no_txt->w &&
             m_y >= dst_rect.y && m_y < dst_rect.y + no_txt->h )
        {
            if (mouse_select)
                current_sel = 1;
            if (m_b)
            {
                NextState = PreviousState;
                finished = 1;
            }
        }
        else
        {
            SDL_SetColors(no_txt, &black_c, 0, 1);
        }
        if (current_sel == 1)
        {
            SDL_SetColors(yes_txt, &black_c, 0, 1);
            SDL_SetColors(no_txt, &mouse_c, 0, 1);
        }
        SDL_BlitSurface(no_txt, NULL, Screen, &dst_rect);
        SDL_UpdateRect(Screen, 0, 0, 0, 0);
        SDL_Delay(1);

        m_b = 0;

        while (SDL_PollEvent(&evt))
            switch (evt.type)
            {
                case SDL_QUIT:
                    NextState = NULL;
                    finished = 1;
                    break;

                case SDL_KEYDOWN:
                    switch (evt.key.keysym.sym)
                    {
                        case SDLK_y:
                            NextState = AffirmState;
                            finished = 1;
                            break;

                        case SDLK_n:
                        case SDLK_ESCAPE:
                            NextState = PreviousState;
                            finished = 1;
                            break;

                        case SDLK_LEFT:
                        case SDLK_RIGHT:
                            current_sel = 1 - current_sel;
                            break;

                        case SDLK_RETURN:
                            if (current_sel == 0)
                            {
                                NextState = AffirmState;
                                finished = 1;
                            }
                            else
                            {
                                NextState = PreviousState;
                                finished = 1;
                            }
                            break;
                    }

                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (evt.button.button == 1)
                        m_b = 1;
                    break;

            }
    }

    TTF_CloseFont(fnt);
    SDL_FreeSurface(back_sfc);
    SDL_FreeSurface(prompt_txt);
    SDL_FreeSurface(yes_txt);
    SDL_FreeSurface(no_txt);

    if (NextState)
        return 0;

    return 1;
}
