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
		printf("[%s] game state goes to %d\n", __func__, ctx->status_cur);
	ctx->status_prev = GAME_STATE_TITLE;

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
	// load level/tile map file
	/*ret = set_gems_textures(ctx);
	if (ret < 0) {
		printf("[%s] set_gems_textures FAILED\n", __func__);
		return ret;
	}*/

	// load music
	ctx->sfx.music_game = Mix_LoadMUS(PATH_MUSIC_GAME);
	if (!ctx->sfx.music_game) {
		printf("[%s] Failed to load game music\n", __func__);
		return -EINVAL;
	}
	/*
	//Load sound effects
	sfx_tires = Mix_LoadWAV("../medias/criss_court_1.wav");
	if (!sfx_tires) {
		printf("Failed to load sfx_tires! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	sfx_door = Mix_LoadWAV("../medias/fermeture_porte.wav");
	if (!sfx_door) {
		printf("Failed to load sfx_door! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	sfx_handbrake = Mix_LoadWAV("../medias/frein_a_main.wav");
	if (!sfx_handbrake) {
		printf("Failed to load sfx_tires! sfx_handbrake Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
	sfx_horn = Mix_LoadWAV("../medias/klaxon.wav");
	if (!sfx_horn) {
		printf("Failed to load sfx_horn! SDL_mixer Error: %s\n",
		       Mix_GetError());
		return -EINVAL;
	}
*/

	//Open the font
	ctx->font = TTF_OpenFont(PATH_FONT, 52);
	if (!ctx->font) {
		printf("[%s] Failed to load font! SDL_ttf Error: %s\n",
		       __func__, TTF_GetError());
		return -EINVAL;
	}

	//Open the font
	ctx->font_game_text = TTF_OpenFont(PATH_FONT, 20);
	if (!ctx->font) {
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
		// change state
		//main_state(ctx);

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
	//main_destroy(ctx);

	// TODO: remove
	//sleep(5);

	free(ctx);

	return 0;
}