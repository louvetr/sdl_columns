#include "common.h"

#define GAME_NEXT_LABEL_X 83
#define GAME_NEXT_LABEL_Y 59
#define GAME_NEXT_TOPGEM_X 90
#define GAME_NEXT_TOPGEM_Y 88

#define GAME_SCORE_LABEL_X 32
#define GAME_SCORE_LABEL_Y 314
#define GAME_SCORE_VALUE_X 187
#define GAME_SCORE_VALUE_Y 343

#define GAME_LEVEL_LABEL_X 32
#define GAME_LEVEL_LABEL_Y 483
#define GAME_LEVEL_VALUE_X 187
#define GAME_LEVEL_VALUE_Y 509

#define GAME_NBGEM_LABEL_X 32
#define GAME_NBGEM_LABEL_Y 655
#define GAME_NBGEM_VALUE_X 187
#define GAME_NBGEM_VALUE_Y 681

enum display_gem_set { DISPLAY_ALL_GEMS = 0, DISPLAY_HIDE_CLEARED_GEMS };

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

static int load_text_message(struct game_context *ctx, TTF_Font *font,
			     struct texture *t, char *string,
			     SDL_Color text_color)
{
	if (t->texture)
		SDL_DestroyTexture(t->texture);

	SDL_Surface *text_surface =
		TTF_RenderText_Solid(font, string, text_color);

	if (!text_surface) {
		printf("[%s] Unable to render text surface! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	t->texture =
		SDL_CreateTextureFromSurface(ctx->gfx.renderer, text_surface);
	if (!t->texture) {
		printf("[%s] Unable to create texture from rendered text! SDL Error: %s\n",
		       __func__, SDL_GetError());
		return -EINVAL;
	}

	t->w = text_surface->w;
	t->h = text_surface->h;

	SDL_FreeSurface(text_surface);

	return 0;
}

static int load_from_rendered_text(struct game_context *ctx, TTF_Font *font,
				   struct texture *t, char *string,
				   SDL_Color text_color)
{
	SDL_DestroyTexture(t->texture);

	SDL_Surface *text_surface =
		TTF_RenderText_Solid(font, string, text_color);

	if (!text_surface) {
		printf("[%s] Unable to render text surface! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	t->texture =
		SDL_CreateTextureFromSurface(ctx->gfx.renderer, text_surface);
	if (!t->texture) {
		printf("[%s] Unable to create texture from rendered text! SDL Error: %s\n",
		       __func__, SDL_GetError());
		return -EINVAL;
	}

	t->w = text_surface->w;
	t->h = text_surface->h;

	SDL_FreeSurface(text_surface);

	return 0;
}

static int texture_render(struct game_context *ctx, struct texture *t, int x,
			  int y, SDL_Rect *clip)
{
	if (!t || !ctx) {
		printf("[%s] invalid param\n", __func__);
		return -EINVAL;
	}

	// set rendering space and render to screen
	SDL_Rect render_quad = { x, y, t->w, t->h };

	if (clip != NULL) {
		render_quad.w = clip->w;
		render_quad.h = clip->h;
	}

	// SET 3rd PARAM to 'clip'
	SDL_RenderCopy(ctx->gfx.renderer, t->texture, clip, &render_quad);

	return 0;
}

static int texture_render_next(struct game_context *ctx, struct texture *t,
			       int x, int y, SDL_Rect *clip)
{
	if (!t || !ctx) {
		printf("[%s] invalid param\n", __func__);
		return -EINVAL;
	}

	// set rendering space and render to screen
	SDL_Rect render_quad = { x, y, t->w, t->h };

	if (clip != NULL) {
		render_quad.w = clip->w * 2 / 3;
		render_quad.h = clip->h * 2 / 3;
	}

	// SET 3rd PARAM to 'clip'
	SDL_RenderCopy(ctx->gfx.renderer, t->texture, clip, &render_quad);

	return 0;
}

static int display_screen_title(struct game_context *ctx)
{
	int ret;
	SDL_Rect r;
	SDL_Color text_color = { 0, 0, 0 };

	// clear screen
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ctx->gfx.renderer);

	//render background
	texture_render(ctx, &ctx->gfx.t_bg_title, 0, 0, NULL);

	// load and render title text
	ret = load_text_message(ctx, ctx->font, &ctx->gfx.t_font_title,
				"Columns of Egypt", text_color);
	texture_render(ctx, &ctx->gfx.t_font_title,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title.h) / 2 - 30,
		       NULL);

	// load and render title text
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_title_startgame, "Start Game",
				text_color);
	texture_render(ctx, &ctx->gfx.t_font_title_startgame,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title_startgame.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title_startgame.h) / 2 +
			       70,
		       NULL);

	// load and render title text
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_title_options, "Options",
				text_color);
	texture_render(ctx, &ctx->gfx.t_font_title_options,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title_options.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title_options.h) / 2 +
			       140,
		       NULL);

	// load and render title text
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_title_credits, "Credits",
				text_color);
	texture_render(ctx, &ctx->gfx.t_font_title_credits,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title_credits.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title_credits.h) / 2 +
			       210,
		       NULL);

	// load and render title text
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_title_exit, "Exit",
				text_color);
	texture_render(ctx, &ctx->gfx.t_font_title_exit,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title_exit.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title_exit.h) / 2 + 280,
		       NULL);

	// cursor rectangle
	for (int i = 0; i < 5; i++) {
		r.w = ctx->gfx.t_font_title_startgame.w + 20 - 2 * i;
		r.h = ctx->gfx.t_font_title_startgame.h + 20 - 2 * i;
		r.x = (SCREEN_WIDTH - ctx->gfx.t_font_title_startgame.w) / 2 -
		      10 + i;
		r.y = (SCREEN_HEIGHT - ctx->gfx.t_font_title_startgame.h) / 2 +
		      70 - 10 + ctx->title_cursor * 70 + i;
		SDL_SetRenderDrawColor(ctx->gfx.renderer, 100, 100, 100, 200);
		SDL_RenderDrawRect(ctx->gfx.renderer, &r);
	}

	//render character
	//mo_render(&mo, &camera);

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}

static int display_screen_quit(struct game_context *ctx)
{
	return 0;
}

int game_text_labels_to_texture(struct game_context *ctx)
{
	int ret;
	char str[32] = { 0 };
	SDL_Color text_color = { 255, 255, 255 };

	// next trio label
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_next_label,
				      "Next:", text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	// score label
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_score_label,
				      "Score:", text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	// level label
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_level_label,
				      "Level:", text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	// gems counter label
	snprintf(str, 32, "%d", ctx->nb_gems_cleared);
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_nbgem_label,
				      "Gem counter:", text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	return 0;
}

static int game_text_values_to_texture(struct game_context *ctx)
{
	int ret;
	char str[32] = { 0 };
	SDL_Color text_color = { 255, 255, 255 };

	// score value
	snprintf(str, 32, "%d", ctx->score);
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_score_value, str,
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	// level value
	snprintf(str, 32, "%d", ctx->level);
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_level_value, str,
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	// gems counter value
	snprintf(str, 32, "%d", ctx->nb_gems_cleared);
	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_game_nbgem_value, str,
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}

	return 0;
}

static int game_text_render(struct game_context *ctx)
{
	int ret;

	ret = texture_render(ctx, &ctx->gfx.t_font_game_next_label,
			     GAME_NEXT_LABEL_X, GAME_NEXT_LABEL_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_score_label,
			     GAME_SCORE_LABEL_X, GAME_SCORE_LABEL_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_score_value,
			     GAME_SCORE_VALUE_X -
				     ctx->gfx.t_font_game_score_value.w,
			     GAME_SCORE_VALUE_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_level_label,
			     GAME_LEVEL_LABEL_X, GAME_LEVEL_LABEL_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_level_value,
			     GAME_LEVEL_VALUE_X -
				     ctx->gfx.t_font_game_level_value.w,
			     GAME_LEVEL_VALUE_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_nbgem_label,
			     GAME_NBGEM_LABEL_X, GAME_NBGEM_LABEL_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_game_nbgem_value,
			     GAME_NBGEM_VALUE_X -
				     ctx->gfx.t_font_game_nbgem_value.w,
			     GAME_NBGEM_VALUE_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	return 0;
}

static int display_screen_game(struct game_context *ctx,
			       enum display_gem_set gem_set)
{
	int ret;

	// string to texture /////////////////////////////////////////////
	ret = game_text_values_to_texture(ctx);
	if (ret < 0) {
		printf("[%s] game_text_values_to_texture Failed\n", __func__);
		return ret;
	}

	// clear renderer /////////////////////////////////////////////
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ctx->gfx.renderer);

	// render background /////////////////////////////////////////////
	texture_render(ctx, &ctx->gfx.t_bg_game, 0, 0, NULL);

	// render texts /////////////////////////////////////////////
	ret = game_text_render(ctx);
	if (ret < 0) {
		printf("[%s] game_text_render Failed\n", __func__);
		return ret;
	}

	// display gem array on playground //////////////////////////////////////////
	for (int i = 0; i < PG_NB_COLUMNS; i++) {
		for (int j = 0; j < PG_NB_ROWS; j++) {
			if (!ctx->gem_array[i][j])
				continue;

			if (gem_set == DISPLAY_HIDE_CLEARED_GEMS &&
			    ctx->gem_array[i][j]->status == GEM_STATE_CLEARING)
				continue;

			ret = texture_render(
				ctx, &ctx->gfx.t_gems_sheet,
				ctx->gem_array[i][j]->x,
				ctx->gem_array[i][j]->y,
				&ctx->gfx.gems_clip[ctx->gem_array[i][j]->type]);
			if (ret < 0)
				printf("[%s] display gem array: texture_render FAILED\n",
				       __func__);
		}
	}

	// render next trio /////////////////////////////////////////////
	for (int i = 0; i < 3; i++) {
		ret = texture_render_next(
			ctx, &ctx->gfx.t_gems_sheet, GAME_NEXT_TOPGEM_X,
			GAME_NEXT_TOPGEM_Y + i * GEM_HEIGHT * 2 / 3,
			&ctx->gfx.gems_clip[ctx->gem_trio_next[i]->type]);

		if (ret < 0)
			printf("[%s] display next trio: texture_render FAILED\n",
			       __func__);
	}

	// render falling trio /////////////////////////////////////////////
	for (int i = 0; i < 3; i++) {
		/*printf("[%s] gem[%d]: %d, %d\n", __func__, i,
		       ctx->gem_trio[i].x, ctx->gem_trio[i].y);*/

		ret = texture_render(
			ctx, &ctx->gfx.t_gems_sheet, ctx->gem_trio[i]->x,
			ctx->gem_trio[i]->y,
			&ctx->gfx.gems_clip[ctx->gem_trio[i]->type]);
		if (ret < 0)
			printf("[%s] display trio: texture_render FAILED\n",
			       __func__);
	}

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}

static int display_screen_credits(struct game_context *ctx)
{
	int ret;
	int x_ref = SCREEN_WIDTH / 5;
	int y_ref = SCREEN_HEIGHT  * 5 / 12;
	int y_gap = 25;
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Rect r;

	// outer border
	r.w = SCREEN_WIDTH * 4 / 5;
	r.h = SCREEN_HEIGHT / 3;
	r.x = SCREEN_WIDTH / 2 - r.w / 2;
	r.y = SCREEN_HEIGHT / 2 - 100 /*r.h / 2*/;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0, 0, 0, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// mid border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 80, 80, 80, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// inner border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 120, 120, 120, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);


	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_credits_music_label, "Music:",
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_credits_music_label,
			     x_ref, y_ref, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_credits_music_1, "- 'Desert caravan' by Myers Music",
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_credits_music_1,
			     x_ref, y_ref + y_gap, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}


	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_credits_music_2, "- 'Asfahan' by Fenek Studio",
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_credits_music_2,
			     x_ref, y_ref + y_gap * 2, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}


	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_credits_sfx_label, "Sound Effects:",
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_credits_sfx_label,
			     x_ref, y_ref  + y_gap * 4, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}


	ret = load_from_rendered_text(ctx, ctx->font_game_text,
				      &ctx->gfx.t_font_credits_sfx_1, "- by Eric Maytas",
				      text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(ctx, &ctx->gfx.t_font_credits_sfx_1,
			     x_ref, y_ref  + y_gap * 5, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}


//static int display_screen_pause(struct game_context *ctx)
static int display_screen_options(struct game_context *ctx)
{
	/* Declaring the surface. */
	int ret;
	char str[32] = { 0 };
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Rect r;

	// outer border
	r.w = SCREEN_WIDTH * 2 / 3;
	r.h = SCREEN_HEIGHT / 3;
	r.x = SCREEN_WIDTH / 2 - r.w / 2;
	r.y = SCREEN_HEIGHT / 2 - 100 /*r.h / 2*/;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0, 0, 0, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// mid border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 80, 80, 80, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// inner border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 120, 120, 120, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// music option message

	char msg[32] = { 0 };
	sprintf(msg, "Music: %s", ctx->mute_music ? "OFF" : " ON");
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_options_music, msg,
				text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(
		ctx, &ctx->gfx.t_font_options_music,
		SCREEN_WIDTH / 2 - ctx->gfx.t_font_options_music.w / 2,
		SCREEN_HEIGHT / 2 - ctx->gfx.t_font_options_music.h / 2 - 20,
		NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	// score in gameover window
	sprintf(msg, "Sound Effects: %s", ctx->mute_sfx ? "OFF" : " ON");
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_options_music, msg,
				text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(
		ctx, &ctx->gfx.t_font_options_music,
		SCREEN_WIDTH / 2 - ctx->gfx.t_font_options_music.w / 2,
		SCREEN_HEIGHT / 2 - ctx->gfx.t_font_options_music.h / 2 + 40,
		NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	// resume message
	ret = load_text_message(ctx, ctx->font_choice,
				&ctx->gfx.t_font_options_resume, "Resume",
				text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(
		ctx, &ctx->gfx.t_font_options_resume,
		SCREEN_WIDTH / 2 - ctx->gfx.t_font_options_resume.w / 2,
		SCREEN_HEIGHT / 2 - ctx->gfx.t_font_options_resume.h / 2 +
			ctx->gfx.t_font_options_music.h / 2 + 80,
		NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	// cursor rectangle
	for (int i = 0; i < 5; i++) {
		r.w = ctx->gfx.t_font_options_music.w + 20 - 2 * i;
		r.h = ctx->gfx.t_font_options_music.h + 20 - 2 * i;
		r.x = (SCREEN_WIDTH - ctx->gfx.t_font_options_music.w) / 2 -
		      10 + i;
		r.y = (SCREEN_HEIGHT - ctx->gfx.t_font_options_music.h) / 2 +
		      /*40*/ -30 + ctx->menu_cursor * 60 + i;
		SDL_SetRenderDrawColor(ctx->gfx.renderer, 180, 180, 180, 200);
		SDL_RenderDrawRect(ctx->gfx.renderer, &r);
	}

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}

static int display_screen_gameover(struct game_context *ctx)
{
	/* Declaring the surface. */
	int ret;
	char str[32] = { 0 };
	SDL_Color text_color = { 255, 255, 255 };
	SDL_Rect r;

	// outer border
	r.w = SCREEN_WIDTH * 2 / 3;
	r.h = SCREEN_HEIGHT / 4;
	r.x = SCREEN_WIDTH / 2 - r.w / 2;
	r.y = SCREEN_HEIGHT / 2 - r.h / 2;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0, 0, 0, 5);
	//SDL_SetRenderDrawColor(ctx->gfx.renderer, 255, 255, 255, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// mid border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 80, 80, 80, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	// inner border
	r.w -= 20;
	r.h -= 20;
	r.x += 10;
	r.y += 10;
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 120, 120, 120, 5);
	SDL_RenderFillRect(ctx->gfx.renderer, &r);

	//SDL_SetRenderDrawBlendMode(ctx->gfx.renderer, SDL_BLENDMODE_BLEND);
	//SDL_BlitSurface( &r , NULL , screen , &rect );

	// gameover message
	ret = load_text_message(ctx, ctx->font, &ctx->gfx.t_font_gameover,
				"Game Over", text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(
		ctx, &ctx->gfx.t_font_gameover,
		SCREEN_WIDTH / 2 - ctx->gfx.t_font_gameover.w / 2,
		SCREEN_HEIGHT / 2 - ctx->gfx.t_font_gameover.h / 2 - 20, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	// score in gameover window
	char score[32] = { 0 };
	sprintf(score, "score:   %d pts.", ctx->score);
	ret = load_text_message(ctx, ctx->font_game_text,
				&ctx->gfx.t_font_gameover_score, score,
				text_color);
	if (ret < 0) {
		printf("[%s] load_from_rendered_text\n", __func__);
		return ret;
	}
	ret = texture_render(
		ctx, &ctx->gfx.t_font_gameover_score,
		SCREEN_WIDTH / 2 - ctx->gfx.t_font_gameover_score.w / 2,
		SCREEN_HEIGHT / 2 - ctx->gfx.t_font_gameover_score.h / 2 + 40,
		NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}

static int display_screen_gem_clearing(struct game_context *ctx)
{
	for (int i = 0; i < 3; i++) {
		display_screen_game(ctx, DISPLAY_HIDE_CLEARED_GEMS);
		SDL_Delay(100);
		display_screen_game(ctx, DISPLAY_ALL_GEMS);
		SDL_Delay(100);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////
// public functions definition
/////////////////////////////////////////////////////////////////

int load_texture_from_file(struct game_context *ctx, char *path,
			   struct texture *in)
{
	SDL_Texture *new_texture;

	SDL_Surface *loaded_surface = IMG_Load(path);
	if (!loaded_surface) {
		printf("[%s] Unable to load image %s! SDL_image Error: %s\n",
		       __func__, path, IMG_GetError());
		return -EINVAL;
	}

	// set color key to cyan
	SDL_SetColorKey(loaded_surface, SDL_TRUE,
			SDL_MapRGB(loaded_surface->format, 0, 0xFF, 0xFF));

	// create texture from surface pixels
	new_texture =
		SDL_CreateTextureFromSurface(ctx->gfx.renderer, loaded_surface);
	if (!new_texture) {
		printf("[%s] Unable to create texture from %s! SDL Error: %s\n",
		       __func__, path, SDL_GetError());
		return -EINVAL;
	}

	// get image dimensions
	in->w = loaded_surface->w;
	in->h = loaded_surface->h;

	// discard old surface
	SDL_FreeSurface(loaded_surface);

	in->texture = new_texture;

	return 0;
}

int main_display(struct game_context *ctx)
{
	if (!ctx) {
		printf("invalid parameter\n");
		return -EINVAL;
	}
	//printf("[%s] ENTER with cur = %d, prev = %d\n", __func__, ctx->status_cur, ctx->status_prev);

	switch (ctx->status_cur) {
	case GAME_STATE_TITLE:
		switch (ctx->title_status) {
		case TITLE_STATE_MENU:
			display_screen_title(ctx);
			break;
		case TITLE_STATE_OPTIONS:
			display_screen_options(ctx);
			break;
		case TITLE_STATE_CREDIT:
			display_screen_credits(ctx);
			break;
		default:
			break;
		}
		break;
	case GAME_STATE_QUIT:
		display_screen_quit(ctx);
		break;
	case GAME_STATE_GAME:
		display_screen_game(ctx, DISPLAY_ALL_GEMS);
		break;
	case GAME_STATE_PAUSE:
		display_screen_options(ctx);
		break;
	case GAME_STATE_GAMEOVER:
		display_screen_gameover(ctx);
		break;
	case GAME_STATE_CLEAR_GEMS:
		display_screen_gem_clearing(ctx);
		break;
	default:
		printf("[%s] invalid state = %d\n", __func__, ctx->status_cur);
		break;
	}

	return 0;
}