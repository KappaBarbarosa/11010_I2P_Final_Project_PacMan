
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"
#include "scene_score.h"
#include "scene_character.h"


// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;
static ALLEGRO_SAMPLE_ID menuBGM;

static int gameTitleW ;
static int gameTitleH ;
static Button btnback;
static Button voice_plus, voice_minus,effect_plus, effect_minus;
static Button toscore, tocharacter;
ALLEGRO_BITMAP* voice1 ;
ALLEGRO_BITMAP* voice2 ;
ALLEGRO_BITMAP* voice3 ;
ALLEGRO_BITMAP* voice4;
ALLEGRO_BITMAP* nowvoice;
ALLEGRO_BITMAP* noweffect;
int voice_x,voice_y;
int voice_aaa = 5;
int effect_aaa = 5;
static void init() {
	//voice1 = al_load_bitmap("Assets/volume.png");
	btnback = button_create(730, 20, 50, 50, "Assets/back.png", "Assets/back2.png");
	toscore = button_create(650,20,50,50,"Assets/score.png", "Assets/score.png");
	tocharacter = button_create(580,20,50,50,"Assets/pacman.png", "Assets/pacman.png");
	voice_plus = button_create(550, 250, 100, 100, "Assets/plus.png", "Assets/plus2.png");
	voice_minus = button_create(150, 250, 100, 100, "Assets/minus.png", "Assets/minus2.png");
	effect_plus = button_create(550, 550, 100, 100, "Assets/plus.png", "Assets/plus2.png");
	effect_minus = button_create(150, 550, 100, 100, "Assets/minus.png", "Assets/minus2.png");
	effect_aaa = 5;
	voice_aaa = 5;
	voice4 = al_load_bitmap("Assets/volume4.png");
	voice3 = al_load_bitmap("Assets/volume3.png");
	voice2 = al_load_bitmap("Assets/volume2.png");
	voice1 = al_load_bitmap("Assets/volume.png");
	nowvoice = voice2;
	noweffect = voice2;
}

// The only function that is shared across files.


static void draw() {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	if(music_volume==0) nowvoice = voice4;
	else if(0<music_volume && music_volume<=0.2) 	nowvoice = voice3;
	else if(0.2<music_volume && music_volume<=0.7)	nowvoice = voice2;
	else if(0.7<music_volume && music_volume<=1)	nowvoice = voice1;
	if(effect_volume==0) noweffect = voice4;
	else if(0<effect_volume && effect_volume<=0.2) 	noweffect = voice3;
	else if(0.2<effect_volume && effect_volume<=0.7)	noweffect = voice2;
	else if(0.7<effect_volume && effect_volume<=1)	noweffect = voice1;
	al_draw_scaled_bitmap(
		nowvoice,
		0, 0,
		284, 284,
		250, 150,
		300, 300,
		0
	);
	al_draw_scaled_bitmap(
		noweffect,
		0, 0,
		284, 284,
		250, 450,
		300, 300,
		0
	);
	al_draw_text(
		menuFont,
		al_map_rgb(0, 255, 255),
		400,
		150,
		ALLEGRO_ALIGN_CENTER,
		"VOLUME"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(0, 255, 255),
		400,
		450,
		ALLEGRO_ALIGN_CENTER,
		"EFFECT"
	);
		drawButton(toscore);
		drawButton(btnback);
		drawButton(voice_minus);
		drawButton(voice_plus);
		drawButton(effect_minus);
		drawButton(effect_plus);
		drawButton(tocharacter);

}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//	[HACKATHON 3-7]
	//	TODO: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
		 btnback.hovered = buttonHover(btnback, mouse_x, mouse_y);
		 toscore.hovered = buttonHover(toscore, mouse_x, mouse_y);
		 tocharacter.hovered = buttonHover(tocharacter, mouse_x, mouse_y);
		 voice_plus.hovered = buttonHover(voice_plus, mouse_x, mouse_y);
		 voice_minus.hovered = buttonHover(voice_minus, mouse_x, mouse_y);
		 effect_plus.hovered = buttonHover(effect_plus, mouse_x, mouse_y);
		 effect_minus.hovered = buttonHover(effect_minus, mouse_x, mouse_y);		 
}

static void on_mouse_down() {
	if (btnback.hovered)
		game_change_scene(scene_menu_create());
	else if (toscore.hovered) game_change_scene(scene_score_create());
	else if(tocharacter.hovered) game_change_scene(scene_character_create());
	else if(voice_plus.hovered){
		if(music_volume<=1)  {
			if(voice_aaa<=10) voice_aaa++;
			music_volume+=0.1;
			stop_bgm(menuBGM);
			menuBGM = play_bgm(themeMusic, music_volume);

			}
	}
	else if(voice_minus.hovered){
		if(music_volume>=0)  {
			if(voice_aaa>0) voice_aaa--;
			music_volume-=0.1;
			if(voice_aaa==0) {
			music_volume = 0;
			stop_bgm(menuBGM);
			}
			stop_bgm(menuBGM);			
			menuBGM = play_bgm(themeMusic, music_volume);}
		else if (music_volume==0) stop_bgm(menuBGM);
		else stop_bgm(menuBGM); 
	}
	else if(effect_plus.hovered){
		if(effect_volume<=1)  {
			if(effect_aaa<=10) effect_aaa++;
			effect_volume+=0.1;}
	}
	else if(effect_minus.hovered){
		if(effect_volume>=0)  {
			if(effect_aaa>=0) effect_aaa--;
			effect_volume-=0.1;}

			if(effect_aaa==0) 
				effect_volume = 0;
		//else stop_bgm(menuBGM); 
	}
}


static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(voice1);
	al_destroy_bitmap(voice2);
	al_destroy_bitmap(voice3);
	al_destroy_bitmap(voice4);
	al_destroy_bitmap(btnback.default_img);
	al_destroy_bitmap(btnback.hovered_img);
	al_destroy_bitmap(toscore.default_img);
	al_destroy_bitmap(toscore.hovered_img);
	al_destroy_bitmap(tocharacter.default_img);
	al_destroy_bitmap(tocharacter.hovered_img);
	al_destroy_bitmap(voice_plus.default_img);
	al_destroy_bitmap(voice_plus.hovered_img);
	al_destroy_bitmap(voice_minus.default_img);
	al_destroy_bitmap(voice_minus.hovered_img);
	al_destroy_bitmap(effect_plus.default_img);
	al_destroy_bitmap(effect_plus.hovered_img);
	al_destroy_bitmap(effect_minus.default_img);
	al_destroy_bitmap(effect_minus.hovered_img);
	
}

/*static void on_key_down(int keycode) {

	switch (keycode) {
		case ALLEGRO_KEY_ENTER:
			game_change_scene(scene_main_create());
			break;
		default:
			break;
	}
}*/


Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");

	return scene;
	
}

