#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_gameover.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"
#include "scene_boss.h"
#include"scene_win.h"
/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;
static ALLEGRO_SAMPLE_ID menuBGM;
static int gameTitleW ;
static int gameTitleH ;
static Button btnSettings;
char* choice[20];
char* name[10];
int cchoice = 2;
static void init() {
	choice[0] = "Assets/blue_move.png";
	choice[1] = "Assets/blue_die.png";
	name[0] = "Blue";
	choice[2] = "Assets/pacman_move.png";
	choice[3] = "Assets/pacman_die.png";
	name[1] ="Pac";
	choice[4] = "Assets/red_move.png";
	choice[5] = "Assets/red_die.png";
	name[2] = "Red";
	choice[6] = "Assets/yellow_move.png";
	choice[7] = "Assets/pacman_die.png";
	name[3] = "SpyPac";
	choice[8] = "Assets/carbi_move.png";
	choice[9] = "Assets/carbi_die.png";
	name[4] = "Kirby";
	choice[10] = "Assets/rabbit.png";
	choice[11] = "Assets/carbi_die.png";
	name[5] = "Usagyuun";
	choice[12] = "Assets/cocorabbit.png";
	choice[13] = "Assets/pacman_die.png";
	name[6] = "Coco";



	btnSettings = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png");

	gameTitle = load_bitmap("Assets/title.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);
	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);

}


static void draw() {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;

	//draw title image
	al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h,
		gameTitleW * scale, gameTitleH * scale,
		0
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\""
	);

		 drawButton(btnSettings);

}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//	[HACKATHON 3-7]
	//	TODO: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
		 btnSettings.hovered = buttonHover(btnSettings, mouse_x, mouse_y);
}


//	[HACKATHON 3-8]
//	TODO: When btnSettings clicked, switch to settings scene
//	Uncomment and fill the code below

static void on_mouse_down() {
	if (btnSettings.hovered)
		game_change_scene(scene_settings_create());
}


static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(gameTitle);
	//	[HACKATHON 3-10]
	//	TODO: Destroy button images
	//	Uncomment and fill the code below
	
	al_destroy_bitmap(btnSettings.default_img);
	al_destroy_bitmap(btnSettings.hovered_img);
	
}

static void on_key_down(int keycode) {

	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_main_create());
			break;
		default:
			break;
	}
}

// TODO: Add more event callback functions such as update, ...

// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_menu_create(void) {

	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Menu";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	// [HACKATHON 3-9]
	// TODO: Register on_mouse_down.
	// Uncomment the code below.
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Menu scene created");
	return scene;
}
