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
#include "scene_win.h"
static ALLEGRO_SAMPLE_ID menuBGM;
static Button nextgame;
static Button bktmenu;
extern char* choice[20];
extern int cchoice; 
extern int total_score;
extern int total_time;
extern int level;
extern ALLEGRO_FONT* winFont_s;
extern ALLEGRO_FONT* winFont;
ALLEGRO_SAMPLE* wwinmusic = NULL;
ALLEGRO_BITMAP* win_picture ;
ALLEGRO_BITMAP* cha_pic ;
ALLEGRO_TIMER* win_timer = NULL;
int mx=0;
int ccoo=0, colorchange=0;
static void init() {
    wwinmusic = load_audio("Assets/Music/ending.mp3");
	nextgame = button_create(150, 550, 100, 50, "Assets/back.png", "Assets/back2.png");
	bktmenu = button_create(450, 550, 200, 50, "Assets/plus.png", "Assets/plus2.png");
	stop_bgm(menuBGM);
	menuBGM = play_bgm(wwinmusic, music_volume);
    cha_pic = load_bitmap(choice[cchoice]);
    mx=0;
    ccoo=0;
    colorchange=0;
}


static void draw(void ){
    char time[50];
    char score[50];
    int gtime = total_time;
    sprintf_s(time,50,"time: %0.2d:%0.2d.%0.2d", gtime/3600,(gtime/60)%60,(gtime%60));
    sprintf_s(score,50, "total_score= %0.5d !!!",total_score);
    ccoo++;
    if(ccoo==10) ccoo=0;
    if(mx<350) mx++;
    colorchange++;
    if(colorchange==20)colorchange=0;
	al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(winFont,al_map_rgb(100+ 5*colorchange, 50+4*colorchange, 150 + colorchange*6),400,100,ALLEGRO_ALIGN_CENTER,"CONGRATULATIONS");
	al_draw_scaled_bitmap(cha_pic,0+(ccoo/5)*16, 0,16, 16,mx, 350,100, 100,0);
    al_draw_text(winFont_s,al_map_rgb(255, 255, 255),200,200,ALLEGRO_ALIGN_LEFT,score);
    al_draw_text(winFont_s,al_map_rgb(255, 255, 255),250,280,ALLEGRO_ALIGN_LEFT,time);
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
		{   level=1;
            stop_bgm(menuBGM);
            game_change_scene(scene_main_create());}
	else if(bktmenu.hovered)
        {   stop_bgm(menuBGM);
            game_change_scene(scene_menu_create());}
}

static void destroy() {
	stop_bgm(menuBGM);
	al_destroy_bitmap(nextgame.default_img);
	al_destroy_bitmap(nextgame.hovered_img);
	al_destroy_bitmap(bktmenu.default_img);
	al_destroy_bitmap(bktmenu.hovered_img);
    al_destroy_bitmap(cha_pic);
	
}
// The only function that is shared across files.
Scene scene_win_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "win";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("win scene created");
	return scene;
}