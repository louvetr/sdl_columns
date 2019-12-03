#include "common.h"

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

static int gem_check_collision_vertical(struct gem *falling_gem,
					struct gem *grounded_gem)
{
	// check bottom borders
	if (falling_gem->y >= SCREEN_HEIGHT - GEM_HEIGHT - 10) {
		//printf("[%s] bottom border\n", __func__);
		falling_gem->status = GEM_STATE_GROUNDED;
		return 1;
	}

	// check vertical gem collisions
	if (grounded_gem && falling_gem->y + GEM_HEIGHT >= grounded_gem->y) {
		falling_gem->status = GEM_STATE_GROUNDED;
		/*printf("[%s:%d] grounded_gem (%d,%d), falling_gem (%d, %d), STUCK bottom\n",
		       __func__, __LINE__, grounded_gem->x, grounded_gem->y,
		       falling_gem->x, falling_gem->y);*/
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
		//printf("[%s] horizontal border\n", __func__);
		return 1;
	}

	// check hozitonal gem collisions left
	if (grounded_gem && falling_gem->x < grounded_gem->x + GEM_WIDTH) {
		/*printf("[%s:%d] grounded_gem->x = %d, falling_gem->x = %d, horizontal gem left 0\n",
		       __func__, __LINE__, grounded_gem->x, falling_gem->x);*/
		return 1;
	}

	// check hozitonal gem collisions right
	if (grounded_gem && falling_gem->x + GEM_HEIGHT > grounded_gem->x) {
		/*printf("[%s:%d] grounded_gem->x = %d, falling_gem->x = %d, horizontal gem right 0\n",
		       __func__, __LINE__, grounded_gem->x, falling_gem->x);*/
		return 1;
	}

	/*
	if (grounded_gem)
		printf("[%s:%d] grounded_gem (%d,%d), falling_gem (%d, %d), no X collision\n",
		       __func__, __LINE__, grounded_gem->x, grounded_gem->y,
		       falling_gem->x, falling_gem->y);
	else
		printf("[%s:%d] falling_gem (%d, %d), no grounded_gem, no X collision\n",
		       __func__, __LINE__, falling_gem->x, falling_gem->y);*/

	return 0;
}

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int gem_create_trio(struct gem *trio[3])
{
	/**trio = calloc(3, sizeof(struct gem));
	if (*trio == NULL) {
		printf("[%s] calloc FAILED\n", __func__);
		return -ENOMEM;
	}*/

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

		/*(*trio)[i].type = rand() % GEM_TYPE_LAST;
		(*trio)[i].w = GEM_WIDTH;
		(*trio)[i].h = GEM_HEIGHT;
		(*trio)[i].x = 3 * GEM_WIDTH + PG_X;
		(*trio)[i].y = (i - 2) * GEM_HEIGHT + PG_Y;*/

		/*printf("[%s] gem[%d] %p: (%d, %d), type = %d\n", __func__, i,
		       trio[i], (*trio)[i].x, (*trio)[i].y, (*trio)[i].type);*/
	}

	return 0;
}

int gem_move_trio(struct game_context *ctx)
{
	//enum collision_type ret;
	int ret;
	int idx_x = (ctx->gem_trio[2]->x - PG_X) / GEM_WIDTH;
	int idx_y = (ctx->gem_trio[2]->y + GEM_HEIGHT) / GEM_HEIGHT;

	/*if (idx_x < 0 || idx_x >= PG_NB_COLUMNS) {
		printf("[%s] ERROR: idx_x = %d / %d = %d\n", __func__,
		       ctx->gem_trio[2].x - PG_X, GEM_WIDTH, idx_x);
	}
	if (idx_y < 0 || idx_y >= PG_NB_ROWS) {
		printf("[%s] ERROR: idx_y = %d / %d = %d\n", __func__,
		       ctx->gem_trio[2].y, GEM_HEIGHT, idx_y);
	}*/

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

		/*printf("[%s:%d] checking X collision with array[%d][%d]\n",
		       __func__, __LINE__, idx_x, idx_y);*/
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

		/*printf("[%s:%d] checking X collision with array[%d][%d]\n",
		       __func__, __LINE__, idx_x, idx_y);*/
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
	//TODO: speed = f (level)
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
			printf("GAME OVER !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			ctx->status_cur = GAME_STATE_GAMEOVER;
			Mix_PlayChannel(-1, ctx->sfx.sfx_gameover, 0);
			//ctx->status_prev = ctx->status_cur;
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
			/*if (ctx->gem_array[i][j])
				printf("[%s:%d] gem[%d][%d] has status = %d\n",
				       __func__, __LINE__, i, j,
				       ctx->gem_array[i][j]->status);*/

			if (ctx->gem_array[i][j] == NULL)
				continue;

			// TODO: skip multiple count of same combo when i,j++

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
				printf("[%s:%d] horizontal %d combo from [%d][%d]\n",
				       __func__, __LINE__, combo_cpt, i, j);
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
				printf("[%s:%d] vertical %d combo from [%d][%d]\n",
				       __func__, __LINE__, combo_cpt, i, j);
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
				printf("[%s:%d] diagonal right %d combo from [%d][%d]\n",
				       __func__, __LINE__, combo_cpt, i, j);
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
				printf("[%s:%d] diagonal left %d combo from [%d][%d]\n",
				       __func__, __LINE__, combo_cpt, i, j);
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
	//printf("[%s:%d] nb_cleared = %d\n", __func__, __LINE__, nb_cleared);

	return nb_cleared;
}

#if 0
int gem_apply_gravity(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS])
{
	struct gem *to_del;
	int cpt = 0, k;

	printf("[%s] ENTER ---------------------------------\n", __func__);

	// go through each gem in array
	for (int i = PG_NB_COLUMNS - 1; i >= 0; i--) {
		int idx_1st_cleared = -1, nb_gem_cleared_in_column = 0;
		for (int j = PG_NB_ROWS; j >= 0; j--) {
			int k;
			to_del = NULL;
			// ignore NULL gem
			if (array[i][j] == NULL)
				continue;
			// delete gem to clear
			if (array[i][j]->status == GEM_STATE_CLEARING) {
				printf("[%s:%d] gem[%d][%d]->status = %d\n",
				       __func__, __LINE__, i, j,
				       array[i][j]->status);

				//free(array[i][j]);
				array[i][j] = NULL;
				nb_gem_cleared_in_column++;
				if (idx_1st_cleared < 0)
					idx_1st_cleared = j;

				/*
				to_del = array[i][j];
				for (k = j - 1; k >= 0; k--) {
					if (array[i][k] == NULL) {
						break;
					}

					array[i][k]->y = array[i][k + 1]->y;
					if (k + 1 == i) {
						free(array[i][j]);
						array[i][j] = NULL;
						cpt++;
						//TODO: free elements, texture, etc
					}
					array[i][k + 1] = array[i][k];
					printf("[%s:%d] move gem[%d][%d] to gem[%d][%d]\n", __func__, __LINE__, i, k, i, k+1);
				}
				if (k + 1 == j) {
					free(array[i][j]);
					array[i][j] = NULL;
					cpt++;
					//TODO: free elements, texture, etc
				}

				if(array[i][k])
					array[i][k] = NULL;
				*/
				//free(to_del);
				//cpt++;
			}
		}
		
		printf("[%s:%d][%d] nb_gem_cleared_in_column = %d, idx_1st_cleared = %d\n", __func__, __LINE__, i, nb_gem_cleared_in_column, idx_1st_cleared);

		if (nb_gem_cleared_in_column > 0) {
			int nb_shifted = 0;

			/*for (int k = idx_1st_cleared - nb_gem_cleared_in_column;
			     k >= 0; k--) {

				if (array[i][k] == NULL)
					continue;

				array[i][idx_1st_cleared + nb_shifted] =
					array[i][k];
				array[i][idx_1st_cleared + nb_shifted]->y =
					array[i][k]->y +
					nb_gem_cleared_in_column * GEM_HEIGHT;
				array[i][k] = NULL;
				nb_shifted++;
				cpt++;
				printf("[%s:%d] move gem[%d][%d] to gem[%d][%d]\n",
				       __func__, __LINE__, i, k, i,
				       idx_1st_cleared + nb_shifted);
			}*/

			for (int k = idx_1st_cleared - nb_gem_cleared_in_column;
			     k >= 0; k--) {

				printf("[%s:%d] move gem[%d][%d] = %p\n",
				       __func__, __LINE__, i, k, array[i][k]);

				if (array[i][k] == NULL)
					continue;

				array[i][idx_1st_cleared - nb_shifted] =
					array[i][k];
				array[i][idx_1st_cleared - nb_shifted]->y =
					array[i][k]->y +
					nb_gem_cleared_in_column * GEM_HEIGHT;
				array[i][k] = NULL;

				printf("[%s:%d] move gem[%d][%d] to gem[%d][%d]\n",
				       __func__, __LINE__, i, k, i,
				       idx_1st_cleared - nb_shifted);

				nb_shifted++;
				cpt++;

				if(nb_shifted >= nb_gem_cleared_in_column)
					break;
			}
		}
	}

	printf("[%s:%d] gem deleted = %d\n", __func__, __LINE__, cpt);
	printf("[%s] EXIT ---------------------------------\n", __func__);

	return 0;
}
#endif

int gem_apply_gravity(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS])
{
	/*struct gem *to_del;
	int cpt = 0, k;*/

	//printf("[%s] ENTER ---------------------------------\n", __func__);

	// go through each gem in array
	for (int i = PG_NB_COLUMNS - 1; i >= 0; i--) {
		//int idx_1st_cleared = -1, nb_gem_cleared_in_column = 0;
		int nb_gap = 0;
		for (int j = PG_NB_ROWS - 1; j >= 0; j--) {
			if (array[i][j] == NULL) {
				/*printf("[%s:%d] skip row #%d, gem[%d][%d] == NULL\n",
				       __func__, __LINE__, i, i, j);*/
				break;
			}
			if (array[i][j]->status == GEM_STATE_CLEARING) {
				/*printf("[%s:%d] going to FREE gem[%d][%d] = '%p' with status = %d\n",
				       __func__, __LINE__, i, j, array[i][j],
				       array[i][j]->status);*/
				free(array[i][j]);
				// TODO: free structure elements
				array[i][j] = NULL;
				nb_gap++;
				/*printf("[%s:%d] clear gem[%d][%d],  nb_gap = %d\n",
				       __func__, __LINE__, i, j, nb_gap);*/
			} else if (nb_gap > 0) {
				array[i][j + nb_gap] = array[i][j];
				array[i][j + nb_gap]->y =
					array[i][j + nb_gap]->y +
					GEM_HEIGHT * nb_gap;
				array[i][j] = NULL;

				/*printf("[%s:%d] move gem[%d][%d] to gem[%d][%d]\n",
				       __func__, __LINE__, i, j, i, j + nb_gap);*/
			}
		}
	}

	return 0;
}
