#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


/////////////////////////////////////////////////////////////////
// defines
/////////////////////////////////////////////////////////////////

#define SCREEN_FPS 30
#define MS_PER_SEC 1000
#define GEMS_PER_LEVEL 20
#define MAX_SPEED 10


#define PATH_BG_TITLE "./media/columns_bg_title_600_800.png"
#define PATH_BG_GAME "./media/columns_bg_game_600_800.png"
#define PATH_GEMS_SHEET "./media/gems_sheet_60x60.png"

#define PATH_MUSIC_GAME "./media/desert_caravan-Myers_Music.mp3"
#define PATH_FONT "./media/Brassia-Regular.otf"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define GEM_WIDTH 60
#define GEM_HEIGHT 60

#define PG_NB_COLUMNS 6
#define PG_NB_ROWS 13

#define PG_X 230
#define PG_Y 10
#define PG_BORDER 10


/////////////////////////////////////////////////////////////////
// enums
/////////////////////////////////////////////////////////////////
enum game_status {
    GAME_STATE_UNKNOWN = 0,
    GAME_STATE_TITLE,
    GAME_STATE_CREDIT,
    GAME_STATE_GAME,
    GAME_STATE_CLEAR_GEMS,
    GAME_STATE_PAUSE,
    GAME_STATE_GAMEOVER,
    GAME_STATE_QUIT
    // TODO: add MENU
};

enum gem_status {
    GEM_STATE_UNKNOWN = 0,
    GEM_STATE_FALLING,
    GEM_STATE_GROUNDED,
    GEM_STATE_CLEARING
};

enum gem_type {
    GEM_TYPE_TRIANGLE = 0,
    GEM_TYPE_SQUARE,
    GEM_TYPE_PENTAGON,
    GEM_TYPE_HEXAGON,
    GEM_TYPE_OCTOGON,
    GEM_TYPE_CIRCLE,
    GEM_TYPE_LAST
};

enum input_action {
    ACTION_NONE = 0,
    ACTION_ESCAPE,
    ACTION_ENTER,
    ACTION_PAUSE,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_DOWN_RELEASED,
    ACTION_LEFT,
    ACTION_RIGHT
};

/////////////////////////////////////////////////////////////////
// structures
/////////////////////////////////////////////////////////////////


// gem unit definition
struct gem {
    // position (top left)
    int x, y;
    // width and height
    unsigned int w, h;
    // status
    enum gem_status status;
    // what kind it is
    enum gem_type type;
};

struct texture {
    // width and height
    int w, h;
    // texture
	SDL_Texture *texture;
};

// grapĥics of the game
struct game_graphics{
    // window
    SDL_Window *window;
    // renderer
    SDL_Renderer *renderer;

    // image textures
	struct texture t_bg_title;
	struct texture t_bg_game;
	struct texture t_gems_sheet;
	struct SDL_Rect gems_clip[GEM_TYPE_LAST];

    // text texture
	struct texture t_title;
	struct texture t_font_title;
    struct texture t_font_game_next_label;
    struct texture t_font_game_score_label;
    struct texture t_font_game_score_value;
    struct texture t_font_game_level_label;
    struct texture t_font_game_level_value;
    struct texture t_font_game_nbgem_label;
    struct texture t_font_game_nbgem_value;
};

struct game_media {
    // musics to be played
    Mix_Music *music_title;
    Mix_Music *music_game;

    // sounc_effects
    Mix_Chunk *sfx_menu_move;
    Mix_Chunk *sfx_menu_select;
    Mix_Chunk *sfx_trio_grounded;
    Mix_Chunk *sfx_trio_cleared;
    Mix_Chunk *sfx_gameover;   
};


// game context, contains all information of the game
struct game_context {

    // quit game when set to 1
    int exit;
	
    // event
    SDL_Event event;
    // action
    enum input_action action;


    // current status in state machine
    enum game_status status_cur;
    // previous status in state machine
    enum game_status status_prev;

    // score
    unsigned int score;
    // multiplier to apply to score
    unsigned int score_multiplier;
    // number of gems cleared
    unsigned int nb_gems_cleared;
    // level
    unsigned int level;
    // trio fall fast
    int fall_fast;

    // representation of gems on screen
    struct gem *gem_array[PG_NB_COLUMNS][PG_NB_ROWS];
    // gem trio falling
    struct gem *gem_trio[3];
    // next gem trio falling
    struct gem *gem_trio_next[3];

    // font to display text
    TTF_Font *font;
    TTF_Font *font_game_text;

    // graphic related stuff
    struct game_graphics gfx;

    // sounds
    struct game_media sfx;

    // timer reference
    Uint32 start_ticks;

};


/////////////////////////////////////////////////////////////////
// public functions declaration
/////////////////////////////////////////////////////////////////

int main_event(struct game_context *ctx);
int main_display(struct game_context *ctx);
int load_texture_from_file(struct game_context *ctx, char *path,
			   struct texture *in);
int main_logic(struct game_context *ctx);


//int gem_create_trio(struct gem **trio);
int gem_create_trio(struct gem *trio[3]);
/*int gem_check_collision(struct gem *g,
			struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS]);*/

int gem_move_trio(struct game_context *ctx);
//int gem_toggle_trio(struct gem **trio);
int gem_toggle_trio(struct gem *trio[3]);
int gem_check_combo(struct game_context *ctx);
int gem_apply_gravity(struct gem *array[PG_NB_COLUMNS][PG_NB_ROWS]);


// TODO: sahll be static
int game_text_labels_to_texture(struct game_context *ctx);
