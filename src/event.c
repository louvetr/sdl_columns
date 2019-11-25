#include "common.h"

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

/*static int gem_create_trio(struct gem **trio)
{

	*trio = calloc(3, sizeof(struct gem));
	if (*trio == NULL) {
		printf("[%s] calloc FAILED\n", __func__);
		return -ENOMEM;
	}

	for (int i = 0; i < 3; i++) {
		*trio[i].type = rand() % (GEM_TYPE_LAST + 1);
		*trio[i].w = GEM_WIDTH;
		*trio[i].h = GEM_HEIGHT;
		*trio[i].x = 3 * GEM_WIDTH + PG_X;
		*trio[i].y = (i - 2) * GEM_HEIGHT + PG_Y;

		printf("[%s] gem[%d]: (%d, %d), type = %d\n", __func__, i,
		       *trio[i].x, *trio[i].y, *trio[i].type);
	}

	return 0;
}*/

static int event_enter_game(struct game_context *ctx)
{
	int ret;

	// init in game values
	// TODO: set score, gem_cleared, level to 0

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

	/*for (int i = 0; i < 3; i++) {
		printf("[%s] trio[%d] %p: (%d, %d), type = %d\n", __func__, i,
		       &ctx->gem_trio[i], ctx->gem_trio[i].x,
		       ctx->gem_trio[i].y, ctx->gem_trio[i].type);
	}*/

	for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			ctx->gem_array[i][j] = NULL;
		}
	}

	// TODO: manage screen transition elsewhere

	// start to play music
	if (!Mix_PlayingMusic()) {
		// play music if none is playing
		Mix_PlayMusic(ctx->sfx.music_game, -1);
	}

	return 0;
}

static int event_exit_game(struct game_context *ctx)
{
	// TODO: clear all resources
	//SDL_DestroyTexture(values & labels);

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
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_GAME;
		printf("[%s] game state goes to %d\n", __func__,
		       ctx->status_cur);

		ret = event_enter_game(ctx);
		if (ret < 0)
			printf("[%s] event_enter_game FAILED", __func__);
		else
			printf("[%s] event_enter_game SUCCESS", __func__);

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
	/*printf("[%s] ENTER with cur = %d, prev = %d\n", __func__,
	       ctx->status_cur, ctx->status_prev);*/
	switch (ctx->action) {
	case ACTION_ESCAPE:
		// go back to title
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_TITLE;
		printf("[%s] game state goes to %d\n", __func__,
		       ctx->status_cur);

		// stop music
		Mix_HaltMusic();
		break;
	case ACTION_ENTER:
		break;
	case ACTION_PAUSE:

		if (ctx->status_prev == GAME_STATE_GAME) {
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_PAUSE;
		//SDL_Delay(200);
		//ctx->status_prev = GAME_STATE_PAUSE;
		printf("[%s] game state goes to %d\n", __func__,
		       ctx->status_cur);	
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
	/*printf("[%s] ENTER with cur = %d, prev = %d\n", __func__,
	       ctx->status_cur, ctx->status_prev);*/
	switch (ctx->action) {
	case ACTION_ENTER:
		break;

	case ACTION_ESCAPE:
	case ACTION_PAUSE:

		// go back to title
		if (ctx->status_prev == GAME_STATE_PAUSE) {
			ctx->status_prev = ctx->status_cur;
			ctx->status_cur = GAME_STATE_GAME;
			printf("[%s] game state goes to %d\n", __func__,
			       ctx->status_cur);
		} else {
			ctx->status_prev = GAME_STATE_PAUSE;

		}

		// resume music
		if (Mix_PausedMusic()) {
			// play music
			Mix_ResumeMusic();
		}

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

static int event_gameover(struct game_context *ctx)
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

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int main_event(struct game_context *ctx)
{
	if (!ctx) {
		printf("invalid parameter\n");
		return -EINVAL;
	}

	/*printf("[%s] ENTER with cur = %d, prev = %d\n", __func__,
	       ctx->status_cur, ctx->status_prev);*/

	while (SDL_PollEvent(&ctx->event) != 0) {
		//user ask to quit
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
				printf("[%s] detected action ACTION_PAUSE\n");
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
			event_title(ctx);
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