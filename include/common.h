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

#define SFX_CHANNEL 3 // arbitrary value

#define PATH_BG_TITLE "./media/columns_bg_title_600_800.png"
#define PATH_BG_GAME "./media/columns_bg_game_600_800.png"
#define PATH_GEMS_SHEET "./media/gems_sheet_60x60.png"

#define PATH_MUSIC_GAME "./media/desert_caravan-Myers_Music.mp3"
#define PATH_MUSIC_TITLE "./media/fenek_studio_asfahan_128kbps.mp3"
#define PATH_SFX_MENU_MOVE "./media/Eric_Maytas_Clank_3.mp3"
#define PATH_SFX_MENU_SELECT "./media/Eric_Maytas_UI_Quirky1.mp3"
#define PATH_SFX_GEM_SWAPPED "./media/Eric_Maytas_Coins1.mp3"
#define PATH_SFX_GEM_GROUNDED "./media/stone_fall.mp3"
#define PATH_SFX_GEM_CLEARED "./media/Rise03.mp3"
#define PATH_SFX_GAMEOVER "./media/Start_Sounds_013.mp3"

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

enum title_choice {
    TITLE_START = 0,
    TITLE_OPTIONS,
    TITLE_CREDITS,
    TITLE_EXIT
};


enum title_state {
    TITLE_STATE_MENU = 0,
    TITLE_STATE_OPTIONS,
    TITLE_STATE_CREDIT,
};


enum menu_choice {
    OPTION_MUSIC = 0,
    OPTION_SFX,
    OPTION_RESUME
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
	//struct texture t_title;
	struct texture t_font_title;
    struct texture t_font_title_startgame;
    struct texture t_font_title_options;
    struct texture t_font_title_credits;
    struct texture t_font_title_exit;
	struct texture t_font_gameover;
	struct texture t_font_gameover_score;
    struct texture t_font_game_next_label;
    struct texture t_font_game_score_label;
    struct texture t_font_game_score_value;
    struct texture t_font_game_level_label;
    struct texture t_font_game_level_value;
    struct texture t_font_game_nbgem_label;
    struct texture t_font_game_nbgem_value;
    struct texture t_font_options_music;
    struct texture t_font_options_sfx;
    struct texture t_font_options_resume;
};

struct game_media {
    // musics to be played
    Mix_Music *music_title;
    Mix_Music *music_game;

    // sounc_effects
    Mix_Chunk *sfx_menu_move;
    Mix_Chunk *sfx_menu_select;
    Mix_Chunk *sfx_gem_swapped;
    Mix_Chunk *sfx_gem_grounded;
    Mix_Chunk *sfx_gem_cleared;
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

    // title state
    enum title_state title_status; 
    // title cursor
    enum title_choice title_cursor;
    // menu cursor
    enum menu_choice menu_cursor;

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
    TTF_Font *font_choice;
    TTF_Font *font_game_text;

    // graphic related stuff
    struct game_graphics gfx;

    // sounds
    struct game_media sfx;

    int mute_sfx;
    int mute_music;

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
