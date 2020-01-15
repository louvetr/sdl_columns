#include "common.h"

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

static int gem_check_collision_vertical(struct gem *falling_gem,
					struct gem *grounded_gem)
{
	// check bottom borders
	if (falling_gem->y >= SCREEN_HEIGHT - GEM_HEIGHT - 10) {
		falling_gem->status = GEM_STATE_GROUNDED;
		return 1;
	}

	// check vertical gem collisions
	if (grounded_gem && falling_gem->y + GEM_HEIGHT >= grounded_gem->y) {
		falling_gem->status = GEM_STATE_GROUNDED;
		return 1;
	}

	return 0;
}

static int gem_check_collision_horizontal(struct gem *falling_gem,
					  struct gem *grounded_gem)
{
	// check hozitonal borders
	if (falling_gem->x < PG_X ||
	    falling_gem->x > SCREEN_WIDTH - GEM_HEIGHT - 10) {
		return 1;
	}

	// check hozitonal gem collisions left
	if (grounded_gem && falling_gem->x < grounded_gem->x + GEM_WIDTH) {
		return 1;
	}

	// check hozitonal gem collisions right
	if (grounded_gem && falling_gem->x + GEM_HEIGHT > grounded_gem->x) {
		return 1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int gem_create_trio(struct gem *trio[3])
{
	for (int i = 0; i < 3; i++) {
		trio[i] = calloc(1, sizeof(struct gem));
		if (trio[i] == NULL) {
			printf("[%s] calloc FAILED\n", __func__);
			return -ENOMEM;
		}
		trio[i]->type = rand() % GEM_TYPE_LAST;
		trio[i]->w = GEM_WIDTH;
		trio[i]->h = GEM_HEIGHT;
		trio[i]->x = 3 * GEM_WIDTH + PG_X;
		trio[i]->y = (i - 2) * GEM_HEIGHT + PG_Y;
	}

	return 0;
}

int gem_move_trio(struct game_context *ctx)
{
	//enum collision_type ret;
	int ret;
	int idx_x = (ctx->gem_trio[2]->x - PG_X) / GEM_WIDTH;
	int idx_y = (ctx->gem_trio[2]->y + GEM_HEIGHT) / GEM_HEIGHT;

	if (idx_x < 0)
		idx_x = 0;
	if (idx_y < 0)
		idx_y = 0;
	if (idx_x >= PG_NB_COLUMNS)
		idx_x = PG_NB_COLUMNS - 1;
	if (idx_y >= PG_NB_ROWS)
		idx_y = PG_NB_ROWS - 1;

	// move horizontally ///////////////////////////////////
	if (ctx->action == ACTION_LEFT &&
	    ctx->gem_trio[2]->x >= PG_X + GEM_WIDTH) {
		ctx->gem_trio[2]->x -= GEM_WIDTH;

		ret = gem_check_collision_horizontal(
			ctx->gem_trio[2], ctx->gem_array[idx_x - 1][idx_y]);
		if (!ret) {
			ctx->gem_trio[0]->x -= GEM_WIDTH;
			ctx->gem_trio[1]->x -= GEM_WIDTH;
		} else {
			ctx->gem_trio[2]->x += GEM_WIDTH;
		}
	}

	if (ctx->action == ACTION_RIGHT &&
	    ctx->gem_trio[2]->x < PG_X + 5 * GEM_WIDTH) {
		ctx->gem_trio[2]->x += GEM_WIDTH;

		ret = gem_check_collision_horizontal(
			ctx->gem_trio[2], ctx->gem_array[idx_x + 1][idx_y]);
		if (!ret) {
			ctx->gem_trio[0]->x += GEM_WIDTH;
			ctx->gem_trio[1]->x += GEM_WIDTH;
		} else {
			ctx->gem_trio[2]->x -= GEM_WIDTH;
		}
	}

	// trio falls a little
	int fall_speed;
	if (ctx->fall_fast)
		fall_speed = MAX_SPEED;
	else
		fall_speed = ctx->level;

	ctx->gem_trio[2]->y += fall_speed;

	ret = gem_check_collision_vertical(ctx->gem_trio[2],
					   ctx->gem_array[idx_x][idx_y]);
	if (!ret) {
		ctx->gem_trio[0]->y += fall_speed;
		ctx->gem_trio[1]->y += fall_speed;
	} else {
		ctx->gem_trio[2]->y -= fall_speed;

		// trio is grounded, check if it is a game over
		if (ctx->gem_trio[0]->y < 10) {
			ctx->status_cur = GAME_STATE_GAMEOVER;
			Mix_PlayChannel(-1, ctx->sfx.sfx_gameover, 0);
		}
	}

	return 0;
}

int gem_toggle_trio(struct gem *trio[3])
{
	struct gem *tmp_g;
	int y_0, y_1, y_2;

	y_0 = trio[0]->y;
	y_1 = trio[1]->y;
	y_2 = trio[2]->y;

	tmp_g = trio[2];
	trio[2] = trio[1];
	trio[1] = trio[0];
	trio[0] = tmp_g;

	trio[0]->y = y_0;
	trio[1]->y = y_1;
	trio[2]->y = y_2;

	return 0;
}

int gem_check_combo(struct game_context *ctx)
{
	int i_check, j_check;
	int combo_cpt;
	int nb_cleared = 0;

	for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			if (ctx->gem_array[i][j] == NULL)
				continue;

			// check horizontal combos
			combo_cpt = 1;
			for (int k = i + 1; k < PG_NB_COLUMNS; k++) {
				// empty neighbor gem or different type ends combo count
				if (ctx->gem_array[k][j] == NULL ||
				    ctx->gem_array[k][j]->type !=
					    ctx->gem_array[i][j]->type)
					break;

				combo_cpt++;
			}
			if (combo_cpt >= 3) {
				for (int k = 0; k < combo_cpt; k++)
					ctx->gem_array[i + k][j]->status =
						GEM_STATE_CLEARING;
			}

			// check vertical combos
			combo_cpt = 1;
			for (int k = j + 1; k < PG_NB_ROWS; k++) {
				// empty neighbor gem or different type ends combo count
				if (ctx->gem_array[i][k] == NULL ||
				    ctx->gem_array[i][k]->type !=
					    ctx->gem_array[i][j]->type)
					break;
				combo_cpt++;
			}
			if (combo_cpt >= 3) {
				for (int k = 0; k < combo_cpt; k++)
					ctx->gem_array[i][j + k]->status =
						GEM_STATE_CLEARING;
			}

			// check diagonal bottom right combos
			combo_cpt = 1;
			for (int k = 1;
			     i + k < PG_NB_COLUMNS && j + k < PG_NB_ROWS; k++) {
				i_check = i + k;
				j_check = j + k;

				// empty neighbor gem or different type ends combo count
				if (ctx->gem_array[i_check][j_check] == NULL ||
				    ctx->gem_array[i_check][j_check]->type !=
					    ctx->gem_array[i][j]->type)
					break;
				combo_cpt++;
			}
			if (combo_cpt >= 3) {
				for (int k = 0; k < combo_cpt; k++)
					ctx->gem_array[i + k][j + k]->status =
						GEM_STATE_CLEARING;
			}

			// check diagonal bottom right combos
			combo_cpt = 1;
			for (int k = 1; i + k < PG_NB_COLUMNS && j - k >= 0;
			     k++) {
				i_check = i + k;
				j_check = j - k;

				// empty neighbor gem or different type ends combo count
				if (ctx->gem_array[i_check][j_check] == NULL ||
				    ctx->gem_array[i_check][j_check]->type !=
					    ctx->gem_array[i][j]->type)
					break;
				combo_cpt++;
			}
			if (combo_cpt >= 3) {
				for (int k = 0; k < combo_cpt; k++)
					ctx->gem_array[i + k][j - k]->status =
						GEM_STATE_CLEARING;
			}
		}
	}

	// count frame to delete for score
	for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			if (ctx->gem_array[i][j] &&
			    ctx->gem_array[i][j]->status == GEM_STATE_CLEARING)
				nb_cleared++;
		}
	}

	return nb_cleared;
}

int gem_apply_gravity(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS])
{
	// go through each gem in array
	for (int i = PG_NB_COLUMNS - 1; i >= 0; i--) {
		int nb_gap = 0;
		for (int j = PG_NB_ROWS - 1; j >= 0; j--) {
			if (array[i][j] == NULL) {
				break;
			}
			if (array[i][j]->status == GEM_STATE_CLEARING) {
				free(array[i][j]);
				array[i][j] = NULL;
				nb_gap++;
			} else if (nb_gap > 0) {
				array[i][j + nb_gap] = array[i][j];
				array[i][j + nb_gap]->y =
					array[i][j + nb_gap]->y +
					GEM_HEIGHT * nb_gap;
				array[i][j] = NULL;
			}
		}
	}

	return 0;
}

int gem_set_falling(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS])
{
	// go through each gem in array
	for (int i = PG_NB_COLUMNS - 1; i >= 0; i--) {
		int nb_gap = 0;
		for (int j = PG_NB_ROWS - 1; j >= 0; j--) {
			if (array[i][j] == NULL) {
				break;
			}
			if (array[i][j]->status == GEM_STATE_CLEARING) {
				free(array[i][j]);
				array[i][j] = NULL;
				nb_gap++;
			} else if (nb_gap > 0) {
				array[i][j + nb_gap] = array[i][j];
				array[i][j + nb_gap]->fall_length =
					GEM_HEIGHT * nb_gap;
				array[i][j] = NULL;
			}
		}
	}

	return 0;
}

int gem_apply_gravity_fall(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS])
{
	int ret;
	struct gem *last_grounded;

	for (int i = PG_NB_COLUMNS - 1; i >= 0; i--) {
		last_grounded = NULL;
		for (int j = PG_NB_ROWS - 1; j >= 0; j--) {
			if (array[i][j] && array[i][j]->fall_length > 0) {
				array[i][j]->y += MAX_SPEED;
				array[i][j]->fall_length -= MAX_SPEED;
				ret = gem_check_collision_vertical(
					array[i][j], last_grounded);
				if (!ret) {
					SDL_Log("[%s] array[%d][%d].y = %d\n",
						__func__, i, j, array[i][j]->y);
				} else {
					array[i][j]->y -= MAX_SPEED;
					if (j == PG_NB_ROWS - 1) {
						array[i][j]->y +=
							array[i][j]
								->fall_length +
							PG_Y;
						SDL_Log("[%s] array[%d][%d].y = %d\n",
							__func__, i, j,
							array[i][j]->y);
					} else {
						array[i][j]->y +=
							array[i][j]->fall_length;
						SDL_Log("[%s] array[%d][%d].y = %d\n",
							__func__, i, j,
							array[i][j]->y);
					}
					array[i][j]->fall_length = 0;
				}
			}
		}
	}

	return 0;
}
