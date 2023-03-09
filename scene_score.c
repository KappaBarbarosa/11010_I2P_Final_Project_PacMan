
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
static ALLEGRO_SAMPLE_ID scoreBGM;
ALLEGRO_SAMPLE* scoremusic = NULL;

extern record srecord[50];
extern int play_time;

static ALLEGRO_BITMAP* gameTitle = NULL;
static ALLEGRO_SAMPLE_ID menuBGM;

static Button btnback;


static void init() {
	btnback = button_create(730, 20, 50, 50, "Assets/back.png", "Assets/back2.png");
    scoremusic = load_audio("Assets/Music/route_10.mp3");
    stop_bgm(scoreBGM);
    scoreBGM = play_bgm(scoremusic,music_volume);

}

// The only function that is shared across files.
int flash=0;
static void draw() {
    flash++;
    if(flash==30) flash=0;
    al_clear_to_color(al_map_rgb(0, 0, 0));
 	al_draw_text(
		menuFont,
		al_map_rgb(0, 155 + 100*(flash/15), 255),
		400,
		50,
		ALLEGRO_ALIGN_CENTER,
		"HIGHEST SCORE!!!"
	);   
     
    char tamp[200];
    
    for(int i=0;i<10;i++){
        int time = srecord[i].time;
        sprintf_s(tamp,100,"%.2d. score = %.5d time =%0.2d:%0.2d.%0.2d",i+1,srecord[i].score,time/3600,(time/60)%60,(time%60));
	    al_draw_text(menuFont,al_map_rgb(20*i+10*flash, 55+i*10*(flash/15), 55+(10-i)*20),150,50+(i+1)*60,ALLEGRO_ALIGN_LEFT,tamp);
    }
		drawButton(btnback);

}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
		 btnback.hovered = buttonHover(btnback, mouse_x, mouse_y);
	 
}

static void on_mouse_down() {
	if (btnback.hovered)	game_change_scene(scene_settings_create());
}



static void destroy() {
	stop_bgm(scoreBGM);
    al_destroy_sample(scoremusic);
	al_destroy_bitmap(btnback.default_img);
	al_destroy_bitmap(btnback.hovered_img);
	
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


Scene scene_score_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "score";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("score scene created");

	return scene;
	
}

