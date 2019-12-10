#include "common.h"

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

static int logic_title(struct game_context *ctx)
{
	return 0;
}

static int logic_game(struct game_context *ctx)
{
	int ret;

	ret = gem_move_trio(ctx);
	if (ret < 0) {
		printf("[%s] gem_move_trio FAILED\n", __func__);
	}

	if (ctx->gem_trio[2]->status != GEM_STATE_GROUNDED)
		return 0;

	if (!ctx->mute_sfx)
		Mix_PlayChannel(SFX_CHANNEL, ctx->sfx.sfx_gem_grounded, 0);

	// trio is grounded, let's process few things

	// set all other trio gems to grounded state
	for (int i = 0; i < 3; i++) {
		ctx->gem_trio[i]->status = GEM_STATE_GROUNDED;
	}

	int idx_x = (ctx->gem_trio[2]->x - PG_X) / GEM_WIDTH;
	int idx_y = (ctx->gem_trio[2]->y /*- 10*/) / GEM_HEIGHT;

	ctx->gem_array[idx_x][idx_y] = ctx->gem_trio[2];
	ctx->gem_array[idx_x][idx_y - 1] = ctx->gem_trio[1];
	ctx->gem_array[idx_x][idx_y - 2] = ctx->gem_trio[0];
	ctx->gem_array[idx_x][idx_y]->y = idx_y * GEM_HEIGHT + 10;
	ctx->gem_array[idx_x][idx_y - 1]->y = (idx_y - 1) * GEM_HEIGHT + 10;
	ctx->gem_array[idx_x][idx_y - 2]->y = (idx_y - 2) * GEM_HEIGHT + 10;

	// next trio becomes trio
	ctx->gem_trio[0] = ctx->gem_trio_next[0];
	ctx->gem_trio[1] = ctx->gem_trio_next[1];
	ctx->gem_trio[2] = ctx->gem_trio_next[2];

	// create new trio
	ret = gem_create_trio(ctx->gem_trio_next);
	if (ret < 0) {
		printf("[%s] gem_create_trio FAILED\n", __func__);
		return -EINVAL;
	}

	// check combo
	int nb_gems_cleared = gem_check_combo(ctx);
	if (nb_gems_cleared > 0) {
		if (!ctx->mute_sfx)
			Mix_PlayChannel(SFX_CHANNEL, ctx->sfx.sfx_gem_cleared,
					0);

		ctx->score += nb_gems_cleared * ctx->score_multiplier;
		printf("[%s] score goes to %d\n", __func__, ctx->score);
		ctx->nb_gems_cleared += nb_gems_cleared;
		ctx->level = ctx->nb_gems_cleared / GEMS_PER_LEVEL + 1;
		if (ctx->level > MAX_SPEED)
			ctx->level = MAX_SPEED;
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_CLEAR_GEMS;
		ctx->fall_fast = 0;
	}

	return 0;
}

static int logic_gem_clearing(struct game_context *ctx)
{
	int nb_gem_cleared;

	if (ctx->status_prev == GAME_STATE_GAME)
		goto exit;

	gem_apply_gravity(ctx->gem_array);
	nb_gem_cleared = gem_check_combo(ctx);
	if (nb_gem_cleared > 0) {
		if (!ctx->mute_sfx)
			Mix_PlayChannel(SFX_CHANNEL, ctx->sfx.sfx_gem_cleared,
					0);
		ctx->score_multiplier++;
		ctx->score += nb_gem_cleared * ctx->score_multiplier;
		printf("[%s] score goes to %d\n", __func__, ctx->score);
		ctx->nb_gems_cleared += nb_gem_cleared;
		ctx->level = ctx->nb_gems_cleared / GEMS_PER_LEVEL + 1;
		if (ctx->level > MAX_SPEED)
			ctx->level = MAX_SPEED;
		printf("[%s] game state stay ins to GAME_STATE_CLEAR_GEMS\n",
		       __func__);
	} else {
		ctx->score_multiplier = 1;
		ctx->status_cur = GAME_STATE_GAME;
	}

exit:
	ctx->status_prev = ctx->status_cur;
	return 0;
}

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int main_logic(struct game_context *ctx)
{
	if (!ctx) {
		printf("invalid parameter\n");
		return -EINVAL;
	}

	switch (ctx->status_cur) {
	case GAME_STATE_TITLE:
		logic_title(ctx);
		break;
	case GAME_STATE_GAME:
		logic_game(ctx);
		break;
	case GAME_STATE_CLEAR_GEMS:
		logic_gem_clearing(ctx);
		break;
	case GAME_STATE_QUIT:
	case GAME_STATE_PAUSE:
	case GAME_STATE_GAMEOVER:
	default:
		break;
	}

	return 0;
}