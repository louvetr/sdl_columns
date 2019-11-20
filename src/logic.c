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

	if (ctx->gem_trio[2].status != GEM_STATE_GROUNDED)
		return 0;

	//printf("[%s] TRIO GROUNDED !!!!!!!!!!!!!!!!!\n", __func__);

	// trio is grounded, let's process few things

	// set all other trio gems to grounded state
	for (int i = 0; i < 3; i++) {
		ctx->gem_trio[i].status = GEM_STATE_GROUNDED;
	}

	int idx_x = (ctx->gem_trio[2].x - PG_X) / GEM_WIDTH;
	int idx_y = (ctx->gem_trio[2].y /*- 10*/) / GEM_HEIGHT;

	// check lose condition
	if (idx_y >= (PG_NB_ROWS - 2) * GEM_HEIGHT) {
		printf("[%s] GAME OVER !!!!!!!!!!!!!!!!!\n", __func__);
	}

	ctx->gem_array[idx_x][idx_y] = &ctx->gem_trio[2];
	ctx->gem_array[idx_x][idx_y - 1] = &ctx->gem_trio[1];
	ctx->gem_array[idx_x][idx_y - 2] = &ctx->gem_trio[0];
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
	ctx->gem_trio = ctx->gem_trio_next;

	// create new trio
	ret = gem_create_trio(&ctx->gem_trio_next);
	if (ret < 0) {
		printf("[%s] gem_create_trio FAILED\n", __func__);
		return -EINVAL;
	}

	// check combo
	gem_check_combo(ctx);

	// update score and other values

	return 0;
}

static int logic_pause(struct game_context *ctx)
{
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
		// do nothing
		break;
	default:
		break;
	}

	return 0;
}