#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <string.h>
#include "scene_menu_object.h"
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"
#include <stdio.h>
#include <math.h>
#include "scene_boss.h"
#include "scene_gameover.h"
extern sposition[5][2];
#define GHOST_NUM 4
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern int pcount;
extern int portal_matrix[10][2];
extern int play_time;
Button btnstop,btnback,btnreset,btncancel;
extern ALLEGRO_TIMER* game_tick_timer;
ALLEGRO_TIMER* game_win_timer;
ALLEGRO_TIMER* game_time_timer;
ALLEGRO_BITMAP* white = NULL;
int timecount;
int game_main_Score = 0;
bool game_over = false;
bool game_stop =false;
int gxx, gyy;
int itembuffer;
char* map_level[3];
int level=1;
int total_score;
ALLEGRO_TIMER* power_up_timer;
ALLEGRO_TIMER* game_timer;
ALLEGRO_TIMER* pman_win_timer;
ALLEGRO_BITMAP* win_pic;
ALLEGRO_BITMAP* score_pic;
static const int power_up_duration = 10;
static Pacman* pman;
 Map* basic_map;
static Ghost** ghosts;
bool game_win = false;
bool debug_mode = false;
bool cheat_mode = false;
bool eat_ghost[GHOST_NUM] = { false };
Ghost* nothing;
int power_once[GHOST_NUM];
int ghost_eat_num;
int eaten_bean;
bool timerUp;
int total_time =0;
static ALLEGRO_SAMPLE_ID themeBGM;
static ALLEGRO_SAMPLE_ID winBGM;
ALLEGRO_SAMPLE* bgmusic = NULL;
ALLEGRO_SAMPLE* winmusic = NULL;
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);
extern record srecord[50];
int power(int n, int b){
	if(n==0) return 1;
	else return b*power(n-1,b);
}
int cmp(const void*a, const void*b){
    record A = *(record*) a;
    record B = *(record*) b;
    if(A.score==B.score){
        if(A.time>B.time) return 1;
        else return -1;
    }
    else if (A.score>B.score) return-1;
    else return 1;
}

static void init(void) {
	btnstop = button_create(730,20,50,50,"Assets/pause.png", "Assets/pause2.png");
	btncancel = button_create(350,150,150,100,"Assets/back.png", "Assets/back2.png");
	btnreset = button_create(350,550,150,100,"Assets/back.png", "Assets/back2.png");
	btnback = button_create(350,350,150,100,"Assets/back.png", "Assets/back2.png");
	white = load_bitmap("Assets/halfwhite.png");
	if(level==1) play_time++;
	map_level[1] = "Assets/map_nthu.txt";
	map_level[2] = "Assets/map_new.txt";
	map_level[3] = "Assets/map_boss.txt";
	timecount=0;
	game_win = false;
	game_stop = false;
	game_over = false;
	game_main_Score = total_score;
	eaten_bean = 0;
	int itembuffer=0;
	score_pic = load_bitmap("Assets/pacman_score.png");
	win_pic = load_bitmap("Assets/doggo.png");
	winmusic = load_audio("Assets/Music/mario_win.mp3");
   	bgmusic = load_audio("Assets/Music/Megalovania_speed_up.mp3");
	basic_map = create_map(map_level[level]);
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}
	timerUp = false;
	ghosts = (Ghost**)malloc(sizeof(Ghost) * GHOST_NUM);
	if(!ghosts) 
		game_abort("fail to creat ghost");
	else {
		for (int i = 0; i < GHOST_NUM; i++) {
			
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	nothing = ghost_create(1);
	GAME_TICK = 0;
	render_init_screen();
	game_timer = al_create_timer(1.0f/60);
	al_set_timer_count(game_timer,total_time);
	al_start_timer(game_timer);
	game_time_timer = al_create_timer(1.0f);
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	game_win_timer = al_create_timer(1.0f/2);
	stop_bgm(themeBGM);
	themeBGM = play_bgm(bgmusic, music_volume);
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;

	switch (basic_map->map[Grid_y][Grid_x])
	{
	case '.':
		pacman_eatItem(pman,'.');
		eaten_bean++;
		game_main_Score += 10;
		basic_map->map[Grid_y][Grid_x] = ' ';
		break;
	case 'P': 
		pacman_eatItem(pman,'P');
		game_main_Score += 50;
		itembuffer=50;
		al_start_timer(power_up_timer);
		al_set_timer_count(power_up_timer,0);
		pman->powerUp = true;
		basic_map->map[Grid_y][Grid_x] = ' ';
		ghost_eat_num=0;
		for (int i = 0; i < GHOST_NUM; i++) {
			power_once[i] = 1;}	
		break;
	case 'T':
		pacman_eatItem(pman,'T');
		game_main_Score += 50;
		itembuffer=50;
		{al_start_timer(game_time_timer);
		al_set_timer_count(game_time_timer,0);
		timerUp = true;}
		basic_map->map[Grid_y][Grid_x] = ' ';
		break;
	case 'p':
		pacman_eatItem(pman,'p');
		int xa = rand() % pcount;
		int px=0,py=0;
		switch (pman->objData.facing)
		{
		case UP:
			py=-1;
			break;
		case DOWN:
			py=1;
			break;
		case RIGHT:
			px=1;
			break;
		case LEFT:
			px=-1;
			break;
		default:
			break;
		}

		pman->objData.Coord.x = portal_matrix[xa][1]+px;
		pman->objData.Coord.y = portal_matrix[xa][0]+py;
	default:
		break;
	}
}
static void status_update(void) {
	if(timerUp)
	{	
		int timeget = al_get_timer_count(game_time_timer); 
		pman->speed = 8;
		if(timeget==10){
			timerUp = false;
			pman->speed =2;
			al_stop_timer(game_time_timer);
			al_set_timer_count(game_time_timer,0);
		}
	}
	if(pman->powerUp)
	{	int q = al_get_timer_count(power_up_timer);
		if(q==10){
		pman->powerUp = false;
		ghost_eat_num=0;
		al_stop_timer(power_up_timer);
		al_set_timer_count(power_up_timer,0);	}}
	if(eaten_bean == basic_map->beansNum)
	{		stop_bgm(themeBGM);
			al_stop_timer(game_timer);
			al_rest(1.0);
			game_log("you win!!!\n");
			game_win = true;
			return;	
	}
	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN)
			continue;
		if(pman->powerUp && power_once[i]==1) ghost_toggle_FLEE(ghosts[i], true); //如果在大力丸狀態下，且沒有被吃過 變成flee
		else if(pman->powerUp == false)	ghost_toggle_FLEE(ghosts[i], false); //否則把flee變回freedom

		if(!cheat_mode && RecAreaOverlap(getDrawArea(pman->objData,GAME_TICK_CD),getDrawArea(ghosts[i]->objData,GAME_TICK_CD)) )
		{	if(ghosts[i]->status == FREEDOM)
			{
			stop_bgm(themeBGM);
			game_log("collide with ghost\n");
			al_stop_timer(game_timer);
			al_rest(1.0);
			pacman_die();
			srecord[10+play_time].score = game_main_Score;   //死掉的時候計分
			qsort(srecord,30,sizeof(record),cmp); //排序，把高的往前排
			game_over = true;
			break;
			}
			else if(ghosts[i]->status==FLEE)
			{
			eat_ghost[i] = true; 
			if(ghost_eat_num<=3) ghost_eat_num++;
			game_main_Score += (power(ghost_eat_num,2)*100); //分數成指數成長 200 400 800 1600
			itembuffer = (power(ghost_eat_num,2)*100);
			pacman_eatItem(pman,'g');
			game_log("eat ghost\n");
			RecArea adrawArea = getDrawArea(ghosts[i]->objData, GAME_TICK_CD); //紀錄吃掉鬼的地方
			gxx = adrawArea.x; 
			gyy = adrawArea.y;
			ghost_collided(ghosts[i]);// flee變成go_in
			power_once[i] = 0;	//被吃過了
				}	
		}		
	}
}
static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
		 btnstop.hovered = buttonHover(btnstop, mouse_x, mouse_y); //暫停鍵
		 if(game_stop==true)
		 {btncancel.hovered = buttonHover(btncancel, mouse_x, mouse_y); //取消
		 btnreset.hovered = buttonHover(btnreset, mouse_x, mouse_y); //重製
		 btnback.hovered = buttonHover(btnback, mouse_x, mouse_y);	//返回
		 }	 
}
bool ppower, timer, tume, tick, win, death;
static void on_mouse_down() {
		
		if (btnstop.hovered){  //如果按下暫停，停下所有計時器，並開起停止狀態
		//al_stop_timer(game_tick_timer);
		al_stop_timer(game_timer);
		stop_bgm(themeBGM);
		if(al_get_timer_count(power_up_timer)>0){
			ppower = true;
			al_stop_timer(power_up_timer);
		}
		if(al_get_timer_count(game_time_timer)>0){
			timer = true;
			al_stop_timer(game_time_timer);
		}
		if(al_get_timer_count(game_win_timer)>0){
			win = true;
			al_stop_timer(game_win_timer);
		}
		if(al_get_timer_count(pman->death_anim_counter)>0){
			death = true;
			al_stop_timer(pman->death_anim_counter);
		}
		game_stop = true;}
	else if(btncancel.hovered ){ //取消暫停，打開所有計時器
		themeBGM = play_bgm(bgmusic, music_volume);
	//	al_start_timer(game_tick_timer);
		if(timer) al_start_timer(game_time_timer);
		if(win) al_start_timer(game_win_timer);
		if(death) al_start_timer(pman->death_anim_counter);
		if(ppower) al_start_timer(power_up_timer);
		game_stop = false;}
	else if( btnback.hovered)game_change_scene(scene_main_create());
	else if (btnreset.hovered) game_change_scene(scene_menu_create());
	
	}	

static void update(void) {
	if(game_stop) return;
	if (game_over) {
		level=1;
		total_score=0;
		total_time=0;		
	 	al_start_timer(pman->death_anim_counter);
		int z = al_get_timer_count(pman->death_anim_counter);
		if(z==150) {
			al_stop_timer(pman->death_anim_counter);
			al_set_timer_count(pman->death_anim_counter,0);
			game_change_scene(scene_gameover_create());
		}	
		return;
	}
	else if(game_win){
		total_time = al_get_timer_count(game_timer);
	 	al_start_timer(game_win_timer);
		int z = al_get_timer_count(game_win_timer);
		if(z==1) 		{
		stop_bgm(themeBGM);
		themeBGM = play_bgm(winmusic,music_volume);}
		if(z%2==0)
			al_draw_scaled_bitmap(win_pic, 0, 0,228, 218,300, 300,220, 220, 0);
		if(z==15) {
			total_score = game_main_Score;
			total_time = al_get_timer_count(game_timer);
		    level++;
			if(level==3) game_change_scene(scene_boss_create());
			else game_change_scene(scene_main_create());
         }
		return;		
	}
	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
    for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman,nothing);
}
int remain_time[4]={0};
int coco=0;
static void draw(void) {
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	char time_board[200];
	char game_score[200];
	char score[50];
	int gtime = al_get_timer_count(game_timer);
	sprintf_s(time_board,200,"time: %0.2d:%0.2d.%0.2d", gtime/3600,(gtime/60)%60,(gtime%60));
	sprintf_s(game_score,200,"score: %d", game_main_Score); //計分
	sprintf_s(score,20,"+%d",itembuffer);
		al_draw_text(menuFont,al_map_rgb(255, 255, 0),100, 750,	ALLEGRO_ALIGN_CENTER,game_score);
		al_draw_text(menuFont,al_map_rgb(255, 255, 0),600, 750,	ALLEGRO_ALIGN_LEFT,time_board);
		if(itembuffer>0){
			coco++;
			al_draw_text(menuFont,al_map_rgb(255, 255, 0),20, 750,	ALLEGRO_ALIGN_CENTER,score);
			if(coco==30){
				coco=0;
				itembuffer=0;
			}
		}
	drawButton(btnstop);
	draw_map(basic_map);
if(game_stop){ //暫停時，畫出選單
		al_draw_scaled_bitmap(white,0,0,600,600,0,0,800,800,0);
		//drawButton(btnback);

		if(btnback.hovered) al_draw_text(menuFont,al_map_rgb(255, 255, 255),400,350,ALLEGRO_ALIGN_CENTER,"RESTART");
		else al_draw_text(menuFont,al_map_rgb(120, 120, 120),400,350,ALLEGRO_ALIGN_CENTER,"RESTART");

		if(btncancel.hovered) al_draw_text(menuFont,al_map_rgb(255, 255, 255),400, 150,ALLEGRO_ALIGN_CENTER,"CANCEL");
		else al_draw_text(menuFont,al_map_rgb(120, 120, 120),400, 150,ALLEGRO_ALIGN_CENTER,"CANCEL");

		if(btnreset.hovered) al_draw_text(menuFont,al_map_rgb(255, 255, 255),400, 550,	ALLEGRO_ALIGN_CENTER,"BACK");
		else al_draw_text(menuFont,al_map_rgb(120, 120, 120),400, 550,ALLEGRO_ALIGN_CENTER,"BACK");
}
	pacman_draw(pman);
	for(int i =0;i<GHOST_NUM;i++){
		
		if(eat_ghost[i]){ //畫出分數
			remain_time[i]++;
			if(remain_time[i]<=120)	al_draw_scaled_bitmap(score_pic,456 +(ghost_eat_num-1)*16,128,16,16,gxx -3, gyy-3,32,32, 0);	
			else{
				remain_time[i]=0;
				eat_ghost[i] = false;
		}
	}}
	if (game_over)
		return;
	//if(game_over&&pman->life==0) return;
	else if(game_win){ //畫狗狗圖
		int al = al_get_timer_count(game_win_timer);

		if(al%2==0)
			al_draw_scaled_bitmap(win_pic, 0, 0,228, 218,200, 200,300, 300, 0);
		 return;
	}
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);
	
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}

}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {

	al_destroy_sample(winmusic);
	al_destroy_sample(bgmusic);
	al_destroy_bitmap(score_pic);
	al_destroy_bitmap(win_pic);
	al_destroy_bitmap(btnstop.default_img);
	al_destroy_bitmap(btnstop.hovered_img);
	al_destroy_bitmap(btncancel.default_img);
	al_destroy_bitmap(btncancel.hovered_img);
	al_destroy_bitmap(btnreset.default_img);
	al_destroy_bitmap(btnreset.hovered_img);
	al_destroy_bitmap(btnback.default_img);
	al_destroy_bitmap(btnback.hovered_img);
	al_destroy_bitmap(white);
	al_destroy_timer(game_time_timer);
	al_destroy_timer(power_up_timer);
	al_destroy_timer(game_timer);
	al_destroy_timer(game_win_timer);
	stop_bgm(themeBGM);
	pacman_destory(pman);
	for(int i=0;i<GHOST_NUM;i++) ghost_destory(ghosts[i]);
	delete_map(basic_map);
}

static void on_key_down(int key_code) {
	switch (key_code)
	{
		// [HACKATHON 1-1]	
		// TODO: Use allegro pre-defined enum ALLEGRO_KEY_<KEYNAME> to controll pacman movement
		// we provided you a function `pacman_NextMove` to set the pacman's next move direction.
		
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman, DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode)
				printf("cheat mode on\n");
			else 
				printf("cheat mode off\n");
			break;
		case ALLEGRO_KEY_T:
			stop_bgm(themeBGM);
			game_log("win\n");
			al_stop_timer(game_timer);
			al_rest(1.0);
			game_win = true;
			break;
		case ALLEGRO_KEY_L:
			stop_bgm(themeBGM);
			game_log("collide with ghost\n");
			al_stop_timer(game_timer);
			al_rest(1.0);
			pacman_die();
			game_over = true;
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;
		
	default:
		break;
	}

}


static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 0, 0));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
