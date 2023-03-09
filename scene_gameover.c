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
#include "scene_gameover.h"
static ALLEGRO_SAMPLE_ID menuBGM;
static Button nextgame;
static Button bktmenu;
ALLEGRO_TIMER* gameover_timer;
ALLEGRO_SAMPLE* gameovermusic = NULL;
ALLEGRO_BITMAP* gameover_picture ;
// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
static void init() {
	//voice1 = al_load_bitmap("Assets/volume.png");
    gameovermusic = load_audio("Assets/Music/mario_game_over.ogg");
	nextgame = button_create(150, 550, 100, 50, "Assets/back.png", "Assets/back2.png");
	bktmenu = button_create(450, 550, 200, 50, "Assets/plus.png", "Assets/plus2.png");
	stop_bgm(menuBGM);
	menuBGM = play_bgm(gameovermusic, music_volume);
    gameover_timer = al_create_timer(1.0f/2);
    al_start_timer(gameover_timer);
	gameover_picture= al_load_bitmap("Assets/gameover.png");

}
static void draw(void ){
    int time = al_get_timer_count(gameover_timer);
	al_clear_to_color(al_map_rgb(0, 0, 0));
    if(time%2==0)
	    	al_draw_scaled_bitmap(gameover_picture,0, 0,666, 375,100, 50,600, 400,0);
	if(nextgame.hovered) al_draw_text(menuFont,al_map_rgb(255, 255, 255),200,550,ALLEGRO_ALIGN_CENTER,"AGAIN");
    else al_draw_text(menuFont,al_map_rgb(142, 152, 255),200,550,ALLEGRO_ALIGN_CENTER,"AGAIN");
	if(bktmenu.hovered)  al_draw_text(menuFont,al_map_rgb(255, 255, 255),600,550,ALLEGRO_ALIGN_CENTER,"BACK TO MENU");
    else al_draw_text(menuFont,al_map_rgb(142, 152, 255),600,550,ALLEGRO_ALIGN_CENTER,"BACK TO MENU");
}



static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	//	[HACKATHON 3-7]
	//	TODO: Update button's status(hovered), and utilize the function `pnt_in_rect`, which you just implemented
	//	Uncomment and fill the code below
		 nextgame.hovered = buttonHover(nextgame, mouse_x, mouse_y);
		 bktmenu.hovered = buttonHover(bktmenu, mouse_x, mouse_y);
		 
}
static void on_mouse_down() {
	if (nextgame.hovered)
		{
            stop_bgm(menuBGM);
            al_stop_timer(gameover_timer);
            game_change_scene(scene_main_create());}
	else if(bktmenu.hovered)
        {   stop_bgm(menuBGM);
            al_stop_timer(gameover_timer);
            game_change_scene(scene_menu_create());}
}

static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_sample(gameovermusic);
	al_destroy_bitmap(nextgame.default_img);
	al_destroy_bitmap(nextgame.hovered_img);
	al_destroy_bitmap(bktmenu.default_img);
	al_destroy_bitmap(bktmenu.hovered_img);
	
}
// The only function that is shared across files.
Scene scene_gameover_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "gameover";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("gameover scene created");
	return scene;
}