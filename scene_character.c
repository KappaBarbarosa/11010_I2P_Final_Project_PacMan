
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
static Button toright, toleft;
ALLEGRO_BITMAP* characters[5];
ALLEGRO_BITMAP* move_pic;


extern char* choice[20];
extern int cchoice; 
extern char* name[10];
static void init() {
	//voice1 = al_load_bitmap("Assets/volume.png");
	btnback = button_create(730, 20, 50, 50, "Assets/back.png", "Assets/back2.png");
	toright = button_create(550, 350, 100, 100, "Assets/right.png", "Assets/right2.png");
	toleft = button_create(150, 350, 100, 100, "Assets/left.png", "Assets/left2.png");
	for(int i=0;i<7;i++)  characters[i] = load_bitmap(choice[i*2]);
    move_pic = characters[1];   
}

// The only function that is shared across files.

int www=0;
static void draw() {
    www++;
    if(www==10) www=0;
	al_clear_to_color(al_map_rgb(0, 0, 0));
	move_pic = characters[cchoice/2];
 	al_draw_text(
		menuFont,
		al_map_rgb(120, 155 , 255),
		400,
		600,
		ALLEGRO_ALIGN_CENTER,
		name[cchoice/2]
	); 	
	al_draw_scaled_bitmap(move_pic,0+(www/5)*16, 0,16, 16,350, 350,100, 100,0);
		drawButton(toright);
		drawButton(btnback);
		drawButton(toleft);

}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
		 btnback.hovered = buttonHover(btnback, mouse_x, mouse_y);
		 toright.hovered = buttonHover(toright, mouse_x, mouse_y);
		 toleft.hovered = buttonHover(toleft, mouse_x, mouse_y);		 
}

static void on_mouse_down() {
	if (btnback.hovered)
		game_change_scene(scene_settings_create());
	else if(toright.hovered){
		if(cchoice<12)  cchoice+=2;
        printf("%d\n",cchoice);
	}
	else if(toleft.hovered){
		if(cchoice>0)  cchoice-=2;
        printf("%d\n",cchoice);
	}
	
}


static void destroy() {
	al_destroy_bitmap(btnback.default_img);
	al_destroy_bitmap(btnback.hovered_img);
	al_destroy_bitmap(toleft.default_img);
	al_destroy_bitmap(toleft.hovered_img);
	al_destroy_bitmap(toright.default_img);
	al_destroy_bitmap(toright.hovered_img);
	for(int i=0;i<7;i++){
        al_destroy_bitmap(characters[i]);
    }
	
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
		// [HACKATHON 1-1]	
		// TODO: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.

	case ALLEGRO_KEY_A:
		if (cchoice > 0)  cchoice -= 2;
		break;
	case ALLEGRO_KEY_D:
		if (cchoice < 8)  cchoice += 2;
		break;

	default:
		break;
	}
}

Scene scene_character_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "character";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("character scene created");

	return scene;
	
}

