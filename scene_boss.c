#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
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
#include "scene_menu_object.h"
#include "scene_win.h"

#define GHOST_NUM 4
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern int pcount;
extern int sposition[5][2];
extern int portal_matrix[10][2];
extern int play_time;
extern int total_score;
extern int total_time;
extern int level;
static int timecount;
static int game_main_Score = 0;
static int power_once[GHOST_NUM];
static int gxx, gyy;
 int bosslife;
static Button btnstop,btnback,btnreset,btncancel;
extern char* map_level[3];
extern ALLEGRO_TIMER* game_tick_timer;
static ALLEGRO_TIMER* game_win_timer;
static ALLEGRO_TIMER* game_time_timer;
static ALLEGRO_TIMER* power_up_timer;
static ALLEGRO_TIMER* game_timer;
static ALLEGRO_TIMER* pman_win_timer;
static ALLEGRO_TIMER* boss_timer; 
static ALLEGRO_BITMAP* win_pic;
static ALLEGRO_SAMPLE* bgmusic = NULL;
static ALLEGRO_SAMPLE* winmusic = NULL;
static ALLEGRO_BITMAP* white = NULL;
static Pacman* pman;
Map* basic_map;
static Ghost** ghosts;
Ghost* bossghost;
extern bool game_over;
static bool game_win = false;
static bool game_stop = false;
static bool debug_mode = false;
static bool cheat_mode = false;
static bool eat_ghost[GHOST_NUM] = { false };
static bool timerUp;
 bool boss_mode;
 bool cooldowm;
static ALLEGRO_SAMPLE_ID themeBGM;
static ALLEGRO_SAMPLE_ID winBGM;
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
static int power(int n, int b){
	if(n==0) return 1;
	else return b*power(n-1,b);
}
static int cmp(const void*a, const void*b){
    record A = *(record*) a;
    record B = *(record*) b;
    if(A.score==B.score){
        if(A.time>B.time) return 1;
        else return -1;
    }
    else if (A.score>B.score) return-1;
    else return 1;
}
int spower=0;
static void init(void) {
    btnstop = button_create(730,20,50,50,"Assets/pause.png", "Assets/pause2.png");
	btncancel = button_create(350,150,150,100,"Assets/back.png", "Assets/back2.png");
	btnreset = button_create(350,550,150,100,"Assets/back.png", "Assets/back2.png");
	btnback = button_create(350,350,150,100,"Assets/back.png", "Assets/back2.png");
    white = load_bitmap("Assets/halfwhite.png");
    spower=0;
	timecount=0;
    boss_mode = false;
	game_win = false;
	game_over = false;
    timerUp = false;
	game_main_Score = total_score;
	win_pic = load_bitmap("Assets/doggo.png");
	winmusic =load_audio("Assets/Music/mario_win.mp3");
   	bgmusic = load_audio("Assets/Music/Megalovania_boss.mp3");
	basic_map = create_map("Assets/map_boss.txt");
	if (!basic_map) game_abort("error on creating map");
	pman = pacman_create();
	if (!pman) game_abort("error on creating pacamn\n");
	ghosts = (Ghost**)malloc(sizeof(Ghost) * GHOST_NUM);
	if(!ghosts) game_abort("fail to creat ghost");
	else {
		for (int i = 0; i < GHOST_NUM; i++) {		
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])	game_abort("error creating ghost\n");
		}
	}
    bossghost = ghost_create(4);
    game_log("create_boss\n");
    bosslife=3;
	GAME_TICK = 0;
	render_init_screen();
	game_timer = al_create_timer(1.0f/60);
    boss_timer = al_create_timer(1.0f);
	al_set_timer_count(game_timer,total_time);
	al_start_timer(game_timer);
    al_start_timer(boss_timer);
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
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;}
    if (bossghost->objData.moveCD > 0)
			bossghost->objData.moveCD -= bossghost->speed;
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	switch (basic_map->map[Grid_y][Grid_x])
	{
	case '.':
		pacman_eatItem(pman,'.');
		//eaten_bean++;
		game_main_Score += 10;
		basic_map->map[Grid_y][Grid_x] = ' ';
		break;
	case 'A': 
        if(bosslife==3)
		{pacman_eatItem(pman,'P');
		game_main_Score += 100;		
		spower++;
		basic_map->map[Grid_y][Grid_x] = ' ';}
		break;
	case 'C': 
    if(bosslife==2)
		{pacman_eatItem(pman,'P');
		game_main_Score += 100;
        spower++;
		basic_map->map[Grid_y][Grid_x] = ' ';}
		break;
	case 'D': 
        if(bosslife==1)
		{pacman_eatItem(pman,'P');
		game_main_Score += 100;
        spower++;
		basic_map->map[Grid_y][Grid_x] = ' ';}
		break;
	case 'T':
		pacman_eatItem(pman,'T');
		game_main_Score += 50;
		if(timecount==0)
		{al_start_timer(game_time_timer);
		al_set_timer_count(game_time_timer,0);
		timerUp = true;}
		if(timecount<=3)timecount++;
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
        break;
	default:
		break;
	}
}
bool neardie =false;
static void status_update(void) {
   // printf("%d\n",al_get_timer_count(boss_timer));   
    RecArea bossrange;
       RecArea drawArea = getDrawArea(bossghost->objData,GAME_TICK_CD);
			bossrange.x = drawArea.x-2.5*drawArea.w;
			bossrange.y = drawArea.y-2.5*drawArea.h;
			bossrange.w = drawArea.w*3;
			bossrange.h = drawArea.h*3;
    
	if(timerUp)
	{   int timeget = al_get_timer_count(game_time_timer); 
		pman->speed = 2*power(timecount+1,2);
		if(timeget==10){
			timerUp = false;
			timecount=0;
			pman->speed =2;
			al_stop_timer(game_time_timer);
			al_set_timer_count(game_time_timer,0);
		}
	}
    
	if(spower){
        
         spower--;
		if (neardie) {
			neardie = false;
			bossghost->status = FLEE;}
		else {
			bossghost->speed--;
			if (bossghost->speed == 1) neardie = true;
		}    
        printf("neardie =%d speed=%d, status = %d\n",neardie,bossghost->speed,bossghost->status);
    }    
    if(al_get_timer_count(boss_timer)>6 && RecAreaOverlap(getDrawArea(ghosts[0]->objData,GAME_TICK_CD),getDrawArea(ghosts[1]->objData,GAME_TICK_CD))&&
        RecAreaOverlap(getDrawArea(ghosts[1]->objData,GAME_TICK_CD),getDrawArea(ghosts[2]->objData,GAME_TICK_CD))&& 
        RecAreaOverlap(getDrawArea(ghosts[2]->objData,GAME_TICK_CD),getDrawArea(ghosts[3]->objData,GAME_TICK_CD))&&boss_mode==false){ 
            boss_mode = true;
            printf("boss_creat\n");
            bossghost->status = BOSS;  
            al_stop_timer(boss_timer);
            al_set_timer_count(boss_timer,0);  
            }
    if(!cheat_mode  && RecAreaOverlap(getDrawArea(pman->objData,GAME_TICK_CD),bossrange))
    {
        if(bossghost->status==BOSS )
        {   
            stop_bgm(themeBGM);
			game_log("collide with ghost\n");
			al_stop_timer(game_timer);
			al_rest(1.0);
			pacman_die();
			srecord[10+play_time].score = game_main_Score;
			srecord[10+play_time].time = al_get_timer_count(game_timer);
			qsort(srecord,30,sizeof(record),cmp);
			game_over = true;
			//break;
		} 
        else if(bossghost->status==FLEE)  // bossghost->status == flee
        {   printf("asdwqe\n");
            game_main_Score += 500;
            pacman_eatItem(pman,'k');
            boss_mode = false;
            ghost_destory(bossghost);
			bossghost = ghost_create(4);
            al_start_timer(boss_timer);
            al_set_timer_count(boss_timer,0);
            bosslife--;
            if(bosslife==0) {
            game_win = true;
            return;}
            for(int i=0;i<GHOST_NUM;i++){
            ghosts[i]->status = GO_IN;
            ghosts[i]->speed =4; 
            }        
        }         
    }	
}

static void update(void) {
    if(game_stop) return;
	if (game_over) {
		level=1;
		total_score=0;
		total_time=0;		
	 	al_start_timer(pman->death_anim_counter);
		int z = al_get_timer_count(pman->death_anim_counter);
        printf("%d\n",z);
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
		    srecord[10+play_time].score = game_main_Score;
		    srecord[10+play_time].time = al_get_timer_count(game_timer);
            total_score = game_main_Score;
            total_time = al_get_timer_count(game_timer);
			qsort(srecord,30,sizeof(record),cmp);
            game_change_scene(scene_win_create());
         }
		return;		
	}
	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
    for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman,bossghost);
    bossghost->move_script(bossghost, basic_map, pman,bossghost);
    printf("status = %d speed = %d x =%d y =%d\n",bossghost->status,bossghost->speed,bossghost->objData.Coord.x,bossghost->objData.Coord.y);
}
static void draw(void) {
	int gtime = al_get_timer_count(game_timer);  
	al_clear_to_color(al_map_rgb(0, 0, 0));
    drawButton(btnstop);
	char time_board[200];
	char game_score[200];
	sprintf_s(time_board,200,"time: %0.2d:%0.2d.%0.2d", gtime/3600,(gtime/60)%60,(gtime%60));
	sprintf_s(game_score,200,"score: %d", game_main_Score);
	al_draw_text(menuFont,al_map_rgb(255, 255, 0),100, 750,ALLEGRO_ALIGN_CENTER,game_score);
	al_draw_text(menuFont,al_map_rgb(255, 255, 0),500, 750,ALLEGRO_ALIGN_LEFT,time_board);
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
	if (game_over)
		return;
	else if(game_win){
		int al = al_get_timer_count(game_win_timer);
		if(al%2==0)
			al_draw_scaled_bitmap(win_pic, 0, 0,228, 218,200, 200,300, 300, 0);
		 return;
	}
    if(boss_mode == false)
	    for (int i = 0; i < GHOST_NUM; i++)
		    ghost_draw(ghosts[i]);
    else    
        ghost_draw(bossghost);
     

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
	//al_destroy_bitmap(score_pic);
	al_destroy_bitmap(win_pic);
	al_destroy_timer(game_time_timer);
	al_destroy_timer(power_up_timer);
	al_destroy_timer(game_timer);
	al_destroy_timer(game_win_timer);
	stop_bgm(themeBGM);
	pacman_destory(pman);
	for(int i=0;i<GHOST_NUM;i++) ghost_destory(ghosts[i]);
    ghost_destory(bossghost);
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

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
		 btnstop.hovered = buttonHover(btnstop, mouse_x, mouse_y); //暫停鍵
		 if(game_stop==true)
		 {btncancel.hovered = buttonHover(btncancel, mouse_x, mouse_y); //取消
		 btnreset.hovered = buttonHover(btnreset, mouse_x, mouse_y); //重製
		 btnback.hovered = buttonHover(btnback, mouse_x, mouse_y);	//返回
		 }	 
}
bool ppower, timer, tume, tick, win, death,bosss;
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
        if(al_get_timer_count(boss_timer)>0){
            bosss = true;
            al_stop_timer(boss_timer);
        }
		game_stop = true;}
	else if(btncancel.hovered ){ //取消暫停，打開所有計時器
		themeBGM = play_bgm(bgmusic, music_volume);
	//	al_start_timer(game_tick_timer);
		if(timer) al_start_timer(game_time_timer);
		if(win) al_start_timer(game_win_timer);
		if(death) al_start_timer(pman->death_anim_counter);
		if(ppower) al_start_timer(power_up_timer);
        if(bosss) al_start_timer(boss_timer);
		game_stop = false;}
	else if( btnback.hovered)game_change_scene(scene_main_create());
	else if (btnreset.hovered) game_change_scene(scene_menu_create());
	
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
 
Scene scene_boss_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Boss";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
    scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Boss scene created");
	return scene;
}
