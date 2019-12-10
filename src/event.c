#include "common.h"

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

static int event_enter_title(struct game_context *ctx)
{
	if (ctx->mute_music)
		return 0;

	// stop music
	Mix_HaltMusic();

	// start to play music
	if (!Mix_PlayingMusic()) {
		// play music if none is playing
		Mix_PlayMusic(ctx->sfx.music_title, -1);
	}

	return 0;
}

static int event_enter_game(struct game_context *ctx)
{
	int ret;

	// stop music
	Mix_HaltMusic();

	// string to texture /////////////////////////////////////////////
	ret = game_text_labels_to_texture(ctx);
	if (ret < 0) {
		printf("[%s] game_text_values_to_texture Failed\n", __func__);
		return ret;
	}

	ctx->score = 0;
	ctx->score_multiplier = 1;
	ctx->level = 1;
	ctx->nb_gems_cleared = 0;
	ctx->fall_fast = 0;

	ret = gem_create_trio(ctx->gem_trio_next);
	if (ret < 0) {
		printf("[%s] gem_create_trio FAILED\n", __func__);
		return -EINVAL;
	}

	ret = gem_create_trio(ctx->gem_trio);
	if (ret < 0) {
		printf("[%s] gem_create_trio FAILED\n", __func__);
		return -EINVAL;
	}

	for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			ctx->gem_array[i][j] = NULL;
		}
	}

	// start to play music
	if (!Mix_PlayingMusic() && !ctx->mute_music) {
		// play music if none is playing
		Mix_PlayMusic(ctx->sfx.music_game, -1);
	}

	return 0;
}

static int event_title(struct game_context *ctx)
{
	int ret;

	switch (ctx->action) {
	case ACTION_ESCAPE:
		// quit game
		ctx->exit = 1;
		break;
	case ACTION_ENTER:
		// enter game

		switch (ctx->title_cursor) {
		case TITLE_EXIT:
			ctx->exit = 1;
			break;
		case TITLE_START:
			ctx->status_prev = ctx->status_cur;
			ctx->status_cur = GAME_STATE_GAME;

			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_select,
						0);
			SDL_Delay(1000);

			ret = event_enter_game(ctx);
			if (ret < 0)
				printf("[%s] event_enter_game FAILED",
				       __func__);
			break;
		case TITLE_OPTIONS:
			ctx->title_status = TITLE_STATE_OPTIONS;
			break;
		case TITLE_CREDITS:
			ctx->title_status = TITLE_STATE_CREDIT;
			break;
		default:
			break;
		}
		break;
	case ACTION_PAUSE:
		break;
	case ACTION_UP:
		if (ctx->title_cursor != TITLE_START) {
			ctx->title_cursor--;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_DOWN:
		if (ctx->title_cursor != TITLE_EXIT) {
			ctx->title_cursor++;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

static int event_screen_options(struct game_context *ctx)
{
	switch (ctx->action) {
	case ACTION_ENTER:

		if (!ctx->mute_sfx)
			Mix_PlayChannel(-1, ctx->sfx.sfx_menu_select, 0);

		switch (ctx->menu_cursor) {
		case OPTION_MUSIC:
			if (ctx->mute_music) {
				ctx->mute_music = 0;
				if (!Mix_PlayingMusic())
					Mix_PlayMusic(ctx->sfx.music_title, -1);
			} else {
				ctx->mute_music = 1;
				Mix_HaltMusic();
			}
			break;
		case OPTION_SFX:
			ctx->mute_sfx ? (ctx->mute_sfx = 0) :
					(ctx->mute_sfx = 1);
			break;
		case OPTION_RESUME:
			ctx->title_status = TITLE_STATE_MENU;
			break;
		default:
			break;
		}
		break;

	case ACTION_ESCAPE:
	case ACTION_PAUSE:
		ctx->title_status = TITLE_STATE_MENU;
		break;
	case ACTION_UP:
		if (ctx->menu_cursor != OPTION_MUSIC) {
			ctx->menu_cursor--;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_DOWN:
		if (ctx->menu_cursor != OPTION_RESUME) {
			ctx->menu_cursor++;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

static int event_screen_credits(struct game_context *ctx)
{
	switch (ctx->action) {
	case ACTION_ENTER:
	case ACTION_ESCAPE:
	case ACTION_PAUSE:
		ctx->title_status = TITLE_STATE_MENU;
		break;
	default:
		break;
	}

	return 0;
}

static int event_quit(struct game_context *ctx)
{
	switch (ctx->action) {
	case ACTION_ESCAPE:
		break;
	case ACTION_ENTER:
		break;
	case ACTION_PAUSE:
		break;
	case ACTION_UP:
		break;
	case ACTION_DOWN:
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

static int event_game(struct game_context *ctx)
{
	int ret;

	switch (ctx->action) {
	case ACTION_ESCAPE:
		// go back to title
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_TITLE;

		ret = event_enter_title(ctx);
		if (ret < 0)
			printf("[%s] event_enter_game FAILED", __func__);

		break;
	case ACTION_ENTER:
		break;
	case ACTION_PAUSE:
		if (ctx->status_prev == GAME_STATE_GAME) {
			ctx->status_prev = ctx->status_cur;
			ctx->status_cur = GAME_STATE_PAUSE;
		} else {
			ctx->status_prev = GAME_STATE_GAME;
		}

		// pause music
		if (Mix_PlayingMusic()) {
			// is music paused
			Mix_PauseMusic();
		}
		break;
	case ACTION_UP:

		if (!ctx->mute_sfx)
			Mix_PlayChannel(SFX_CHANNEL, ctx->sfx.sfx_gem_swapped,
					0);
		gem_toggle_trio(ctx->gem_trio);

		break;
	case ACTION_DOWN:
		ctx->fall_fast = 1;
		break;
	case ACTION_DOWN_RELEASED:
		ctx->fall_fast = 0;
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

static int event_pause(struct game_context *ctx)
{
	switch (ctx->action) {
	case ACTION_ENTER:

		if (!ctx->mute_sfx)
			Mix_PlayChannel(-1, ctx->sfx.sfx_menu_select, 0);

		switch (ctx->menu_cursor) {
		case OPTION_MUSIC:
			ctx->mute_music ? (ctx->mute_music = 0) :
					  (ctx->mute_music = 1);
			break;
		case OPTION_SFX:
			ctx->mute_sfx ? (ctx->mute_sfx = 0) :
					(ctx->mute_sfx = 1);
			break;
		case OPTION_RESUME:
			goto resume_game;
			break;
		default:
			break;
		}
		break;

	case ACTION_ESCAPE:
	case ACTION_PAUSE:
	resume_game:
		// go back to title
		if (ctx->status_prev == GAME_STATE_PAUSE) {
			ctx->status_prev = ctx->status_cur;
			ctx->status_cur = GAME_STATE_GAME;
		} else {
			ctx->status_prev = GAME_STATE_PAUSE;
		}

		// resume music
		if (!ctx->mute_music) {
			if (Mix_PausedMusic()) {
				// play music
				Mix_ResumeMusic();
			} else if (!Mix_PlayingMusic()) {
				// play music if none is playing
				Mix_PlayMusic(ctx->sfx.music_title, -1);
			}
		}
		break;
	case ACTION_UP:
		if (ctx->menu_cursor != OPTION_MUSIC) {
			ctx->menu_cursor--;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_DOWN:
		if (ctx->menu_cursor != OPTION_RESUME) {
			ctx->menu_cursor++;
			if (!ctx->mute_sfx)
				Mix_PlayChannel(-1, ctx->sfx.sfx_menu_move, 0);
		}
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

static int event_gameover(struct game_context *ctx)
{
	switch (ctx->action) {
	case ACTION_ESCAPE:
		// go back to title
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_TITLE;

		// stop music
		Mix_HaltMusic();
		break;
	case ACTION_ENTER:
		break;
	case ACTION_PAUSE:
		break;
	case ACTION_UP:
		break;
	case ACTION_DOWN:
		break;
	case ACTION_LEFT:
		break;
	case ACTION_RIGHT:
		break;
	default:
		break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int main_event(struct game_context *ctx)
{
	if (!ctx) {
		printf("invalid parameter\n");
		return -EINVAL;
	}

	// special case for when entering the game
	if (ctx->status_cur == GAME_STATE_TITLE &&
	    ctx->status_prev == GAME_STATE_UNKNOWN) {
		event_enter_title(ctx);
		ctx->status_prev = GAME_STATE_TITLE;
	}

	while (SDL_PollEvent(&ctx->event) != 0) {
		if (ctx->event.type == SDL_QUIT)
			ctx->exit = 1;

		if (ctx->event.type == SDL_KEYDOWN) {
			switch (ctx->event.key.keysym.sym) {
			case SDLK_ESCAPE:
				ctx->action = ACTION_ESCAPE;
				break;
			case SDLK_RETURN:
				ctx->action = ACTION_ENTER;
				break;
			case SDLK_SPACE:
				ctx->action = ACTION_PAUSE;
				break;
			case SDLK_UP:
			case SDLK_z:
				ctx->action = ACTION_UP;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				ctx->action = ACTION_DOWN;
				break;
			case SDLK_LEFT:
			case SDLK_q:
				ctx->action = ACTION_LEFT;
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				ctx->action = ACTION_RIGHT;
				break;
			default:
				continue;
			}
		}

		if (ctx->event.type == SDL_KEYUP) {
			switch (ctx->event.key.keysym.sym) {
			case SDLK_DOWN:
			case SDLK_s:
				ctx->action = ACTION_DOWN_RELEASED;
				break;
			default:
				continue;
			}
		}

		switch (ctx->status_cur) {
		case GAME_STATE_TITLE:
			switch (ctx->title_status) {
			case TITLE_STATE_MENU:
				event_title(ctx);
				break;
			case TITLE_STATE_OPTIONS:
				event_screen_options(ctx);
				break;
			case TITLE_STATE_CREDIT:
				event_screen_credits(ctx);
				break;
			default:
				break;
			}
			break;
		case GAME_STATE_QUIT:
			event_quit(ctx);
			break;
		case GAME_STATE_GAME:
			event_game(ctx);
			break;
		case GAME_STATE_PAUSE:
			event_pause(ctx);
			break;
		case GAME_STATE_GAMEOVER:
			event_gameover(ctx);
			break;
		case GAME_STATE_CLEAR_GEMS:
			// do nothing
			break;
		default:
			continue;
		}
	}

	return 0;
}