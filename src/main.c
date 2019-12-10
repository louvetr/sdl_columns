#include "common.h"

const int SCREEN_TICKS_PER_FRAME = MS_PER_SEC / SCREEN_FPS;

/////////////////////////////////////////////////////////////////
// static function definitions
////////////////////////////////////////////////////////////////

static int main_init(struct game_context *ctx)
{
	int ret;

	srand(time(0));

	// init SDL
	ret = SDL_Init(SDL_INIT_VIDEO);
	if (ret < 0) {
		printf("[%s] SDL_Init ERROR: %s\n", __func__, SDL_GetError());
		return ret;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		printf("[%s] Warning: Linear texture filtering not enabled!",
		       __func__);
	}

	// create g_window
	ctx->gfx.window =
		SDL_CreateWindow("SDL_tuto", SDL_WINDOWPOS_UNDEFINED,
				 SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
				 SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!ctx->gfx.window) {
		printf("[%s] SDL_SetVideoMode ERROR: %s\n", SDL_GetError(),
		       __func__);
		return -EINVAL;
	}

	ctx->gfx.renderer = SDL_CreateRenderer(ctx->gfx.window, -1,
					       SDL_RENDERER_ACCELERATED);
	if (!ctx->gfx.renderer) {
		printf("[%s] Renderer could not be created! SDL Error: %s\n",
		       __func__, SDL_GetError());
		return -EINVAL;
	}

	SDL_SetRenderDrawBlendMode(ctx->gfx.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(ctx->gfx.renderer, 0x00, 0xFF, 0xFF, 0x00);

	// init PNG loading
	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		printf("[%s] SDL_image could not initialize! SDL_image Error: %s\n",
		       __func__, IMG_GetError());
		return -EINVAL;
	}

	// init Fonts management
	if (TTF_Init() < 0) {
		printf("[%s] SDL_ttf could not initialize! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	// init Music and SFX management
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("[%s] SDL_ttf could not initialize! SDL_Mixer Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	// set state to title screen
	ctx->status_cur = GAME_STATE_TITLE;
	ctx->status_prev = GAME_STATE_UNKNOWN;

	for (int i = 0; i < GEM_TYPE_LAST; i++) {
		ctx->gfx.gems_clip[i].w = GEM_WIDTH;
		ctx->gfx.gems_clip[i].h = GEM_HEIGHT;
	}

	ctx->gfx.gems_clip[GEM_TYPE_TRIANGLE].x = 0;
	ctx->gfx.gems_clip[GEM_TYPE_TRIANGLE].y = 0;
	ctx->gfx.gems_clip[GEM_TYPE_SQUARE].x = GEM_WIDTH;
	ctx->gfx.gems_clip[GEM_TYPE_SQUARE].y = 0;
	ctx->gfx.gems_clip[GEM_TYPE_PENTAGON].x = GEM_WIDTH * 2;
	ctx->gfx.gems_clip[GEM_TYPE_PENTAGON].y = 0;
	ctx->gfx.gems_clip[GEM_TYPE_HEXAGON].x = 0;
	ctx->gfx.gems_clip[GEM_TYPE_HEXAGON].y = GEM_HEIGHT;
	ctx->gfx.gems_clip[GEM_TYPE_OCTOGON].x = GEM_WIDTH;
	ctx->gfx.gems_clip[GEM_TYPE_OCTOGON].y = GEM_HEIGHT;
	ctx->gfx.gems_clip[GEM_TYPE_CIRCLE].x = GEM_WIDTH * 2;
	ctx->gfx.gems_clip[GEM_TYPE_CIRCLE].y = GEM_HEIGHT;

	ctx->start_ticks = SDL_GetTicks();

	return 0;
}

static int main_load_media(struct game_context *ctx)
{
	int ret;

	// load png
	ret = load_texture_from_file(ctx, PATH_BG_TITLE, &ctx->gfx.t_bg_title);
	if (ret < 0) {
		printf("[%s] Failed to load title PNG!\n", __func__);
		return ret;
	}
	ret = load_texture_from_file(ctx, PATH_BG_GAME, &ctx->gfx.t_bg_game);
	if (ret < 0) {
		printf("[%s] Failed to load game PNG!\n", __func__);
		return ret;
	}

	// load tile sheet
	ret = load_texture_from_file(ctx, PATH_GEMS_SHEET,
				     &ctx->gfx.t_gems_sheet);
	if (ret < 0) {
		printf("[%s] Failed to load gem sheet PNG!\n", __func__);
		return ret;
	}

	// load music
	ctx->sfx.music_game = Mix_LoadMUS(PATH_MUSIC_GAME);
	if (!ctx->sfx.music_game) {
		printf("[%s] Failed to load game music\n", __func__);
		return -EINVAL;
	}
	ctx->sfx.music_title = Mix_LoadMUS(PATH_MUSIC_TITLE);
	if (!ctx->sfx.music_game) {
		printf("[%s] Failed to load title music\n", __func__);
		return -EINVAL;
	}

	//Load sound effects
	ctx->sfx.sfx_menu_move = Mix_LoadWAV(PATH_SFX_MENU_MOVE);
	if (!ctx->sfx.sfx_menu_move) {
		printf("Failed to load sfx menu move! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	ctx->sfx.sfx_menu_select = Mix_LoadWAV(PATH_SFX_MENU_SELECT);
	if (!ctx->sfx.sfx_menu_select) {
		printf("Failed to load sfx menu select! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	ctx->sfx.sfx_gameover = Mix_LoadWAV(PATH_SFX_GAMEOVER);
	if (!ctx->sfx.sfx_gameover) {
		printf("Failed to load sfx gameover! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	ctx->sfx.sfx_gem_swapped = Mix_LoadWAV(PATH_SFX_GEM_SWAPPED);
	if (!ctx->sfx.sfx_gem_swapped) {
		printf("Failed to load sfx gem swapped! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	ctx->sfx.sfx_gem_cleared = Mix_LoadWAV(PATH_SFX_GEM_CLEARED);
	if (!ctx->sfx.sfx_gem_cleared) {
		printf("Failed to load sfx gem cleared! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	ctx->sfx.sfx_gem_grounded = Mix_LoadWAV(PATH_SFX_GEM_GROUNDED);
	if (!ctx->sfx.sfx_gem_grounded) {
		printf("Failed to load sfx gem grounded! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}

	//Open the font
	ctx->font = TTF_OpenFont(PATH_FONT, 52);
	if (!ctx->font) {
		printf("[%s] Failed to load font! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	//Open the font
	ctx->font_game_text = TTF_OpenFont(PATH_FONT, 20);
	if (!ctx->font_game_text) {
		printf("[%s] Failed to load font! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	//Open the font
	ctx->font_choice = TTF_OpenFont(PATH_FONT, 32);
	if (!ctx->font_choice) {
		printf("[%s] Failed to load font! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	printf("media loaded\n");

	return 0;
}

static int main_sleep(struct game_context *ctx)
{
	int frame_ticks = SDL_GetTicks() - ctx->start_ticks;
	if (frame_ticks < SCREEN_TICKS_PER_FRAME)
		SDL_Delay(SCREEN_TICKS_PER_FRAME - frame_ticks);

	ctx->start_ticks = SDL_GetTicks();

	return 0;
}

static int main_destroy(struct game_context *ctx)
{
	int i, j;

	// free textures
	SDL_DestroyTexture(ctx->gfx.t_bg_title.texture);
	SDL_DestroyTexture(ctx->gfx.t_bg_game.texture);
	SDL_DestroyTexture(ctx->gfx.t_gems_sheet.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_title.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_title_startgame.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_title_options.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_title_credits.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_title_exit.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_gameover.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_gameover_score.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_next_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_score_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_score_value.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_level_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_level_value.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_nbgem_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_game_nbgem_value.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_options_music.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_options_sfx.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_options_resume.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_credits_music_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_credits_music_1.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_credits_music_2.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_credits_sfx_label.texture);
	SDL_DestroyTexture(ctx->gfx.t_font_credits_sfx_1.texture);

	// free music
	Mix_FreeMusic(ctx->sfx.music_title);
	Mix_FreeMusic(ctx->sfx.music_game);

	//Free the sound effects
	Mix_FreeChunk(ctx->sfx.sfx_gameover);
	Mix_FreeChunk(ctx->sfx.sfx_gem_cleared);
	Mix_FreeChunk(ctx->sfx.sfx_gem_grounded);
	Mix_FreeChunk(ctx->sfx.sfx_gem_swapped);
	Mix_FreeChunk(ctx->sfx.sfx_menu_move);
	Mix_FreeChunk(ctx->sfx.sfx_menu_select);

	//Destroy window
	SDL_DestroyRenderer(ctx->gfx.renderer);
	SDL_DestroyWindow(ctx->gfx.window);

	for (i = 0; i < PG_NB_COLUMNS; i++) {
		for (j = 0; j < PG_NB_ROWS; j++) {
			free(ctx->gem_array[i][j]);
			ctx->gem_array[i][j] = NULL;
		}
	}

	for (i = 0; i < 3; i++) {
		free(ctx->gem_trio[i]);
		ctx->gem_trio[i] = NULL;
		free(ctx->gem_trio_next[i]);
		ctx->gem_trio_next[i] = NULL;
	}

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	free(ctx);

	return 0;
}

/////////////////////////////////////////////////////////////////
// Main function
////////////////////////////////////////////////////////////////

int main()
{
	struct game_context *ctx;

	ctx = calloc(1, sizeof(*ctx));

	// init
	main_init(ctx);

	// load media and stuff
	main_load_media(ctx);

	// game loop
	while (!ctx->exit) {
		// events management
		main_event(ctx);

		// game logic
		main_logic(ctx);

		// display on screen
		main_display(ctx);

		// sleep
		main_sleep(ctx);

		ctx->action = ACTION_NONE;
	}

	// free resources
	main_destroy(ctx);

	return 0;
}