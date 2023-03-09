#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "ghost.h"
#include "map.h"
#include "pacman_obj.h"

/* global variables*/
// [ NOTE ]
// if you change the map .txt to your own design.
// You have to modify cage_grid_{x,y} to corressponding value also.
// Or you can do some change while loading map (reading .txt file)
// Make the start position metadata stored with map.txt.
const int cage_grid_x=22, cage_grid_y=11;
extern int sposition[5][2];
/* shared variables. */
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern const int block_width,  block_height;
extern int level;
extern int spower;
/* Internal variables */
static const int fix_draw_pixel_offset_x = -3;
static const int fix_draw_pixel_offset_y = -3;
static const int draw_region = 30;
int ddd=0, ccc=0;
int fff=0,xxx=0;
// [ NOTE - speed again ]
// Again, you see this notaficationd. If you still want to implement something 
// fancy with speed, objData->moveCD and GAME_TICK, you can first start on 
// working on animation of ghosts and pacman. // Once you finished the animation 
// part, you will have more understanding on whole mechanism.
static const int basic_speed = 2;
extern ALLEGRO_TIMER* power_up_timer;

Ghost* ghost_create(int flag) {

	// NOTODO
	Ghost* ghost = (Ghost*)malloc(sizeof(Ghost));
	if (!ghost)
		return NULL;

	ghost->typeFlag = flag;
	ghost->objData.Size.x = block_width;
	ghost->objData.Size.y = block_height;

	ghost->objData.nextTryMove = NONE;
	ghost->speed = basic_speed;
	if(ghost->typeFlag==4) ghost->speed=4;
	ghost->status = BLOCKED;

	ghost->flee_sprite = load_bitmap("Assets/ghost_flee.png");
	ghost->dead_sprite = load_bitmap("Assets/ghost_dead.png");

	switch (ghost->typeFlag) {
	case Blinky:
		ghost->objData.Coord.x = sposition[1][0];
		ghost->objData.Coord.y = sposition[1][1];
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	case Pinky:
		ghost->objData.Coord.x = sposition[2][0];
		ghost->objData.Coord.y = sposition[2][1];
		ghost->move_sprite = load_bitmap("Assets/ghost_move_pink.png");
		ghost->move_script = &ghost_pink_move_script;
		break;
	case Inky:
		ghost->objData.Coord.x = sposition[3][0];
		ghost->objData.Coord.y = sposition[3][1];
		ghost->move_sprite = load_bitmap("Assets/ghost_move_blue.png");
		ghost->move_script = &ghost_blue_move_script;
		break;
	case Clyde:
		ghost->objData.Coord.x = sposition[4][0];
		ghost->objData.Coord.y = sposition[4][1];
		ghost->move_sprite = load_bitmap("Assets/ghost_move_orange.png");
		ghost->move_script = &ghost_orange_move_script;
		break;
	case 4:
		ghost->objData.Coord.x = 25;
		ghost->objData.Coord.y = 15;
		ghost->move_sprite = load_bitmap("Assets/boss_ghost.png");
		ghost->flee_sprite = load_bitmap("Assets/boss_flee.png");
		ghost->move_script = &ghost_boss_move_script;
		break;
	default:
		ghost->objData.Coord.x = cage_grid_x;
		ghost->objData.Coord.y = cage_grid_y;
		ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
		ghost->move_script = &ghost_red_move_script;
		break;
	}
	return ghost;
}
void ghost_destory(Ghost* ghost) {
	al_destroy_bitmap(ghost -> move_sprite);
	al_destroy_bitmap(ghost -> flee_sprite);
	al_destroy_bitmap(ghost -> dead_sprite);
	free(ghost);
}
void ghost_draw(Ghost* ghost) 
{  
    ddd++;
    if(ddd==5)
    {
        ddd=0;
        if(ccc==0)
            ccc=1;
        else
            ccc=0;
    }

	RecArea drawArea = getDrawArea(ghost->objData, GAME_TICK_CD);
	RecArea bossrange;
	bossrange.x = drawArea.x - 2.5*drawArea.w;
	bossrange.y = drawArea.y - 2.5*drawArea.h;
	//Draw default image
	
	int bitmap_x_offset = 0;
	if (ghost->status == FLEE) {
		if(level<3)
        {
			int qwe=al_get_timer_count(power_up_timer);
        	if(qwe>=8)
			{
            	xxx++;      
            	if(xxx==17){
                	xxx=0;
                	fff=!fff;
						}
     	   al_draw_scaled_bitmap(ghost->flee_sprite, 0+ccc*16+32*fff, 0,16, 16,
            drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
            draw_region, draw_region, 0);
        	}
        	else
            	al_draw_scaled_bitmap(ghost->flee_sprite, 0+ccc*16, 0,16, 16,drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                	draw_region, draw_region, 0);
		}
        
		else
			al_draw_scaled_bitmap(ghost->flee_sprite, 0+ccc*16, 0,16, 16,
                bossrange.x + fix_draw_pixel_offset_x, bossrange.y + fix_draw_pixel_offset_y,
                draw_region*4, draw_region*4, 0);
		}
	
	else if(ghost->status == BOSS)
	{
        switch(ghost->objData.facing)
        {
        case LEFT:
                al_draw_scaled_bitmap(ghost->move_sprite, 32+ccc*16, 0,16, 16,
                    bossrange.x + fix_draw_pixel_offset_x, bossrange.y + fix_draw_pixel_offset_y,
                    draw_region*4, draw_region*4, 0);
                break;
        case RIGHT:
                al_draw_scaled_bitmap(ghost->move_sprite, 0+ccc*16, 0,16, 16,
                    bossrange.x + fix_draw_pixel_offset_x, bossrange.y + fix_draw_pixel_offset_y,
                    draw_region*4, draw_region*4, 0);
                break;
        case UP:
                al_draw_scaled_bitmap(ghost->move_sprite, 64+ccc*16, 0,16, 16,
                    bossrange.x + fix_draw_pixel_offset_x, bossrange.y + fix_draw_pixel_offset_y,
                    draw_region*4, draw_region*4, 0);
                break;
        case DOWN:
                al_draw_scaled_bitmap(ghost->move_sprite, 96+ccc*16, 0,16, 16,
                    bossrange.x + fix_draw_pixel_offset_x, bossrange.y + fix_draw_pixel_offset_y,
                    draw_region*4, draw_region*4, 0);
                break;
        }
	}
	else if (ghost->status == GO_IN) 
	{
        
        switch(ghost->objData.facing)
        {
        case LEFT:
                al_draw_scaled_bitmap(ghost->dead_sprite, 32, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        case RIGHT:
                al_draw_scaled_bitmap(ghost->dead_sprite, 0, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        case UP:
                al_draw_scaled_bitmap(ghost->dead_sprite, 64, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        case DOWN:
                al_draw_scaled_bitmap(ghost->dead_sprite, 96, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        default:    //最剛開始不動的時候！！設為0 這樣就會是第一張圖 不能用close是因為要讓他靜止不動
                al_draw_scaled_bitmap(ghost->dead_sprite, 0, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        }
	
	}
	else
	 {
		
        switch(ghost->objData.facing)
        {
        	case LEFT:
                al_draw_scaled_bitmap(ghost->move_sprite, 32+ccc*16, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        	case RIGHT:
                al_draw_scaled_bitmap(ghost->move_sprite, 0+ccc*16, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        	case UP:
                al_draw_scaled_bitmap(ghost->move_sprite, 64+ccc*16, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        	case DOWN:
                al_draw_scaled_bitmap(ghost->move_sprite, 96+ccc*16, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        	default:    //最剛開始不動的時候！！設為0 這樣就會是第一張圖 不能用close是因為要讓他靜止不動
                al_draw_scaled_bitmap(ghost->move_sprite, 0, 0,16, 16,
                    drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
                    draw_region, draw_region, 0);
                break;
        }
		
	}

}
void ghost_NextMove(Ghost* ghost, Directions next) {
	ghost->objData.nextTryMove = next;
}
void printGhostStatus(GhostStatus S) {

	switch(S){
	
	case BLOCKED: // stay inside the ghost room
		game_log("BLOCKED");
		break;
	case GO_OUT: // going out the ghost room
		game_log("GO_OUT");
		break;
	case FREEDOM: // free at the map
		game_log("FREEDOM");
		break;
	case GO_IN:
		game_log("GO_IN");
		break;
	case FLEE:
		game_log("FLEE");
		break;
	default:
		game_log("status error");
		break;
	}
}
bool ghost_movable(Ghost* ghost, Map* M, Directions targetDirec, bool room) {
	// [HACKATHON 2-3]
	// TODO: Determine if the current direction is movable.
	// Basically, this is a ghost version of `pacman_movable`.
	// So if you have finished (and you should), you can just "copy and paste"
	// and do some small alternation.

	
	int nowx = ghost->objData.Coord.x, nowy = ghost->objData.Coord.y;

	switch (targetDirec){
	case UP:
		nowy--;
		break;
	case DOWN:
		nowy++;
		break;
	case LEFT:
		nowx--;
		break;
	case RIGHT:
		nowx++;
		break;
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}

	if (is_wall_block(M, nowx, nowy) || (room && is_room_block(M, nowx, nowy)))
		return false;
	
	return true;

}

void ghost_toggle_FLEE(Ghost* ghost, bool setFLEE) {
	if (setFLEE) {
		if (ghost->status == FREEDOM) {
			ghost->status = FLEE;
			ghost->speed = 1;
		}
	}
	else {
		if (ghost->status == FLEE) {
			ghost->status = FREEDOM;
			ghost->speed = 2;
		}
	}

}

void ghost_collided(Ghost* ghost) {
	if (ghost->status == FLEE) {
		ghost->status = GO_IN;
		ghost->speed = 4;
	}
}
void ghost_move_script_SERVANT(Ghost* ghost,Map* M ,Ghost* boss ){
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y,boss->objData.Coord.x, boss->objData.Coord.y);
}
void ghost_move_script_GO_IN(Ghost* ghost, Map* M) {
	switch (ghost->typeFlag)
	{
	case 0:
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y,sposition[1][0], sposition[1][1]);
		break;
	case 1:
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y,sposition[2][0], sposition[2][1]);
		break;
	case 2:
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y,sposition[3][0], sposition[3][1]);
		break;
	case 3:
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y,sposition[4][0], sposition[4][1]);
		break;
	
	default:
		break;
	}
	
}
void ghost_move_script_GO_OUT(Ghost* ghost, Map* M) {
	// Description
	// Here we always assume the room of ghosts opens upward.
	// And used a greedy method to drag ghosts out of room.
	// You should modify here if you have different implementation/design of room.
	if(M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B') 
		ghost_NextMove(ghost, UP);
	else
		if(level<3) ghost->status = FREEDOM;
		else ghost->status = SERVANT;
}
void ghost_move_script_FLEE(Ghost* ghost, Map* M, const Pacman * const pacman) {
	// [TODO]
	
	Directions shortestDirection = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pacman->objData.Coord.x, pacman->objData.Coord.y);
	// Description:
	// The concept here is to simulate ghosts running away from pacman while pacman is having power bean ability.
	// To achieve this, think in this way. We first get the direction to shortest path to pacman, call it K (K is either UP, DOWN, RIGHT or LEFT).
	// Then we choose other available direction rather than direction K.
	// In this way, ghost will escape from pacman.
	static Directions proba[4]; // possible movement
	int cnt = 0, ans = 0;
	for (Directions i = 1; i <= 4; i++) {
		if (i == shortestDirection && al_get_timer_count(power_up_timer) <= 5)continue;
		if (ghost_movable(ghost, M, i, true)) {
			ans += i;
			proba[cnt++] = i;
		}
	}
	if (ans == 5 && ghost_movable(ghost, M, ghost->objData.preMove, true))ghost_NextMove(ghost, ghost->objData.preMove, false);
	else if (cnt == 0)ghost_NextMove(ghost, 5 - ghost->objData.preMove, false);
	else ghost_NextMove(ghost, proba[rand() % cnt], false);
}

