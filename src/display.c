#include "common.h"

#define GAME_NEXT_LABEL_X 83
#define GAME_NEXT_LABEL_Y 59
#define GAME_NEXT_TOPGEM_X 83
#define GAME_NEXT_TOPGEM_Y 80

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

/////////////////////////////////////////////////////////////////
// static functions definition
/////////////////////////////////////////////////////////////////

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

static int display_screen_title(struct game_context *ctx)
{
	// clear screen
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(ctx->gfx.renderer);

	//render background
	texture_render(ctx, &ctx->gfx.t_bg_title, 0, 0, NULL);

	// render title text
	texture_render(ctx, &ctx->gfx.t_font_title,
		       (SCREEN_WIDTH - ctx->gfx.t_font_title.w) / 2,
		       (SCREEN_HEIGHT - ctx->gfx.t_font_title.h) / 2, NULL);

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

	/*34 318*/

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
			     GAME_SCORE_VALUE_X, GAME_SCORE_VALUE_Y, NULL);
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
			     GAME_LEVEL_VALUE_X, GAME_LEVEL_VALUE_Y, NULL);
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
			     GAME_NBGEM_VALUE_X, GAME_NBGEM_VALUE_Y, NULL);
	if (ret < 0) {
		printf("[%s] Failed to render text texture!\n", __func__);
		return ret;
	}
	return 0;
}

static int display_screen_game(struct game_context *ctx)
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
		ret = texture_render(
			ctx, &ctx->gfx.t_gems_sheet, GAME_NEXT_TOPGEM_X,
			GAME_NEXT_TOPGEM_Y + i * GEM_HEIGHT,
			&ctx->gfx.gems_clip[ctx->gem_trio_next[i].type]);
		if (ret < 0)
			printf("[%s] display next trio: texture_render FAILED\n",
			       __func__);
	}

	// render falling trio /////////////////////////////////////////////
	for (int i = 0; i < 3; i++) {
		/*printf("[%s] gem[%d]: %d, %d\n", __func__, i,
		       ctx->gem_trio[i].x, ctx->gem_trio[i].y);*/

		ret = texture_render(
			ctx, &ctx->gfx.t_gems_sheet, ctx->gem_trio[i].x,
			ctx->gem_trio[i].y,
			&ctx->gfx.gems_clip[ctx->gem_trio[i].type]);
		if (ret < 0)
			printf("[%s] display trio: texture_render FAILED\n",
			       __func__);
	}

	//update screen
	SDL_RenderPresent(ctx->gfx.renderer);

	return 0;
}

static int display_screen_pause(struct game_context *ctx)
{
	return 0;
}

static int display_screen_gameover(struct game_context *ctx)
{
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

	switch (ctx->status_cur) {
	case GAME_STATE_TITLE:
		display_screen_title(ctx);
		break;
	case GAME_STATE_QUIT:
		display_screen_quit(ctx);
		break;
	case GAME_STATE_GAME:
		display_screen_game(ctx);
		break;
	case GAME_STATE_PAUSE:
		display_screen_pause(ctx);
		break;
	case GAME_STATE_GAMEOVER:
		display_screen_gameover(ctx);
		break;
	case GAME_STATE_CLEAR_GEMS:
		// do nothing
		break;
	default:
		printf("[%s] invalid state", __func__);
		break;
	}

	return 0;
}