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
		
	// RLTDBG
	/*for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			ctx->gem_array[i][j] = NULL;
			printf("[%s] array[%d][%d] = '%p'\n", __func__, i, j,
			       ctx->gem_array[i][j]);
		}
	}*/

	//printf("[%s] action = %d\n", __func__, ctx->action);

	ret = gem_move_trio(ctx);
	if (ret < 0) {
		printf("[%s] gem_move_trio FAILED\n", __func__);
	}

	// change gem state

	if (ctx->gem_trio[2]->status != GEM_STATE_GROUNDED)
		return 0;

	//printf("[%s] TRIO GROUNDED !!!!!!!!!!!!!!!!!\n", __func__);

	// trio is grounded, let's process few things

	// set all other trio gems to grounded state
	for (int i = 0; i < 3; i++) {
		ctx->gem_trio[i]->status = GEM_STATE_GROUNDED;
	}

	int idx_x = (ctx->gem_trio[2]->x - PG_X) / GEM_WIDTH;
	int idx_y = (ctx->gem_trio[2]->y /*- 10*/) / GEM_HEIGHT;

	// check lose condition
	if (idx_y >= (PG_NB_ROWS - 2) * GEM_HEIGHT) {
		printf("[%s] GAME OVER !!!!!!!!!!!!!!!!!\n", __func__);
	}

	ctx->gem_array[idx_x][idx_y] = ctx->gem_trio[2];
	ctx->gem_array[idx_x][idx_y - 1] = ctx->gem_trio[1];
	ctx->gem_array[idx_x][idx_y - 2] = ctx->gem_trio[0];
	ctx->gem_array[idx_x][idx_y]->y = idx_y * GEM_HEIGHT + 10;
	ctx->gem_array[idx_x][idx_y - 1]->y = (idx_y - 1) * GEM_HEIGHT + 10;
	ctx->gem_array[idx_x][idx_y - 2]->y = (idx_y - 2) * GEM_HEIGHT + 10;

	/*printf("[%s:%d] array[%d][%d] x = %d, y = %d\n", __func__, __LINE__,
	       idx_x, idx_y - 2, ctx->gem_array[idx_x][idx_y - 2]->x,
	       ctx->gem_array[idx_x][idx_y - 2]->y);
	printf("[%s:%d] array[%d][%d] x = %d, y = %d\n", __func__, __LINE__,
	       idx_x, idx_y - 1, ctx->gem_array[idx_x][idx_y - 1]->x,
	       ctx->gem_array[idx_x][idx_y - 1]->y);
	printf("[%s:%d] array[%d][%d] x = %d, y = %d\n", __func__, __LINE__,
	       idx_x, idx_y, ctx->gem_array[idx_x][idx_y]->x,
	       ctx->gem_array[idx_x][idx_y]->y);*/

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
		//ctx->score_multiplier++;
		ctx->score += nb_gems_cleared * ctx->score_multiplier;
		printf("[%s] score goes to %d\n", __func__, ctx->score);
		ctx->nb_gems_cleared += nb_gems_cleared;
		ctx->level = ctx->nb_gems_cleared / GEMS_PER_LEVEL + 1;
		if(ctx->level > MAX_SPEED)
			ctx->level = MAX_SPEED;
		ctx->status_prev = ctx->status_cur;
		ctx->status_cur = GAME_STATE_CLEAR_GEMS;
		printf("[%s] game state goes to %d\n", __func__, ctx->status_cur);
		ctx->fall_fast = 0;
		printf("[%s] game state goes to GAME_STATE_CLEAR_GEMS\n", __func__);
	}

	// update score and other values

	return 0;
}

static int logic_pause(struct game_context *ctx)
{
	SDL_Delay(50);
	return 0;
}

static int logic_gameover(struct game_context *ctx)
{
	return 0;
}

static int logic_quit(struct game_context *ctx)
{
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
		ctx->score_multiplier++;
		ctx->score += nb_gem_cleared * ctx->score_multiplier;
		printf("[%s] score goes to %d\n", __func__, ctx->score);
		ctx->nb_gems_cleared += nb_gem_cleared;
		ctx->level = ctx->nb_gems_cleared / GEMS_PER_LEVEL + 1;
		if(ctx->level > MAX_SPEED)
			ctx->level = MAX_SPEED;
		printf("[%s] game state stay ins to GAME_STATE_CLEAR_GEMS\n", __func__);
	}
	else {
		ctx->score_multiplier = 1;
		ctx->status_cur = GAME_STATE_GAME;
		printf("[%s] game state goes to GAME_STATE_GAME\n", __func__);
	}

exit:
	ctx->status_prev = ctx->status_cur;
		printf("[%s] game state goes to %d\n", __func__, ctx->status_cur);
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
	//printf("[%s] ENTER with cur = %d, prev = %d\n", __func__, ctx->status_cur, ctx->status_prev);

	switch (ctx->status_cur) {
	case GAME_STATE_TITLE:
		logic_title(ctx);
		break;
	case GAME_STATE_QUIT:
		logic_quit(ctx);
		break;
	case GAME_STATE_GAME:
		logic_game(ctx);
		break;
	case GAME_STATE_PAUSE:
		logic_pause(ctx);
		break;
	case GAME_STATE_GAMEOVER:
		logic_gameover(ctx);
		break;
	case GAME_STATE_CLEAR_GEMS:
		logic_gem_clearing(ctx);
		break;
	default:
		break;
	}

	return 0;
}