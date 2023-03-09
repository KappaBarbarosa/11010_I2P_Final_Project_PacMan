#include <allegro5/allegro_primitives.h>
#include "pacman_obj.h"
#include "map.h"
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;		// where to put pacman at the beginning
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3;  // draw offset 
static const int draw_region = 30;							// pacman bitmap draw region
static ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
extern int sposition[5][2];
// [ NOTE - speed ]
// If you want to implement something regarding speed.
// You may have to modify the basic_speed here.
// But before you modify this, make sure you are 
// totally understand the meaning of speed and function
// `step()` in `scene_game.c`, also the relationship between
// `speed`, `GAME_TICK`, `GAME_TICK_CD`, `objData->moveCD`.
static const int basic_speed = 2;
extern char* choice[20];
extern int cchoice; 
/* Shared variables */
extern ALLEGRO_SAMPLE* ghost_sound;
extern ALLEGRO_SAMPLE* clock_sound;
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern bool game_over;
extern float effect_volume;
int flag=1;
bool aaa = 0; 
int bbb = 0;
/* Declare static function */
static bool pacman_movable(Pacman* pacman, Map* M, Directions targetDirec) {
	// [HACKATHON 1-2]
	// TODO: Determine if the current direction is movable.
	// That is to say, your pacman shouldn't penetrate 'wall' and 'room'
	// , where room is reserved for ghost to set up.
	// 1) For the current direction `targetDirec`, use pre-implemented function
	// `is_wall_block` and `is_room_block` to check if the block is wall or room. (they are both defined in map.c)
	// 2) the coordinate data of pacman is stored in pacman->objData.Coord
	// it is a self-defined pair IntInt type. Trace the code and utilize it.

	
	int x = pacman->objData.Coord.x, y = pacman->objData.Coord.y;
	
	switch (targetDirec)
	{
	case UP:
		y -= 1;
		break;
	case DOWN:
		y += 1;
		break;
	case LEFT:
		x -= 1;
		break;
	case RIGHT:
		x += 1;
		break;
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}
	if (is_wall_block(M, x , y) || is_room_block(M, x,y))
		return false;
	
	return true;
}

Pacman* pacman_create() {
	Pacman* pman = (Pacman*)malloc(sizeof(Pacman));
	if (!pman)
		return NULL;
	pman->objData.Coord.x = sposition[0][0];
	pman->objData.Coord.y = sposition[0][1];
	pman->objData.Size.x = block_width;
	pman->objData.Size.y = block_height;
	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;
	pman->speed = basic_speed;
	pman->life=3;
	pman->death_anim_counter = al_create_timer(1.0f / 64);
	pman->powerUp = false;
	pman->move_sprite = load_bitmap(choice[cchoice]);
	pman->die_sprite = load_bitmap(choice[cchoice+1]);
	return pman;
}

void pacman_destory(Pacman* pman) {

		al_destroy_bitmap(pman->move_sprite);
		al_destroy_bitmap(pman->die_sprite);
		al_destroy_timer(pman->death_anim_counter);
		free(pman);
	
}


void pacman_draw(Pacman* pman) {
	RecArea drawArea = getDrawArea(pman->objData, GAME_TICK_CD);
	int offset = 0;
	if (game_over) {	
		int y = al_get_timer_count(pman->death_anim_counter);
		printf("%d\n",flag);
		if(flag==1){	
			al_draw_scaled_bitmap(pman->die_sprite, 0 + 16*(y/10), 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
			if(y==120) flag=0;
		}
		if(y==149) {
			flag=1;
			}

	}
	else {
			int qqq = pman->objData.moveCD;
			bbb += (pman->speed /2);
			if(bbb>=6) {
				bbb=0;
				if(aaa==0) aaa=1;
				else aaa=0;
				}
			if(qqq==0) aaa=0;
			switch(pman->objData.facing)
			{
		
			case LEFT:
				al_draw_scaled_bitmap(pman->move_sprite, 32+aaa*16, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
				break;		
			case RIGHT:
				al_draw_scaled_bitmap(pman->move_sprite, 0+aaa*16, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
				break;	
			case UP:
				al_draw_scaled_bitmap(pman->move_sprite, 64+aaa*16, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
				break;	
			case DOWN:
				al_draw_scaled_bitmap(pman->move_sprite, 96+aaa*16, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
				break;
			default:
				al_draw_scaled_bitmap(pman->move_sprite, 0, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
				);
				break;			
			}

	}
}
void pacman_move(Pacman* pacman, Map* M) {
	if (!movetime(pacman->speed)){
		return;}
	if (game_over)
		return;

	int probe_x = pacman->objData.Coord.x, probe_y = pacman->objData.Coord.y;
	if (pacman_movable(pacman, M, pacman->objData.nextTryMove)) 
		pacman->objData.preMove = pacman->objData.nextTryMove;
	else if (!pacman_movable(pacman, M, pacman->objData.preMove)) 
		return;

	switch (pacman->objData.preMove)
	{
	case UP:
		pacman->objData.Coord.y -= 1;
		pacman->objData.preMove = UP;
		break;
	case DOWN:
		pacman->objData.Coord.y += 1;
		pacman->objData.preMove = DOWN;
		break;
	case LEFT:
		pacman->objData.Coord.x -= 1;
		pacman->objData.preMove = LEFT;
		break;
	case RIGHT:
		pacman->objData.Coord.x += 1;
		pacman->objData.preMove = RIGHT;
		break;
	default:
		break;
	}
	pacman->objData.facing = pacman->objData.preMove;
	pacman->objData.moveCD = GAME_TICK_CD;
}
void pacman_eatItem(Pacman* pacman, const char Item) {
	switch (Item)
	{
	case '.':
		stop_bgm(PACMAN_MOVESOUND_ID);
		PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
		break;
	case 'P':
		stop_bgm(PACMAN_MOVESOUND_ID);
		PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
		break;	
	case 'T':
		stop_bgm(PACMAN_MOVESOUND_ID);
		PACMAN_MOVESOUND_ID = play_audio(clock_sound, effect_volume);
		break;
	case 'g':
		stop_bgm(PACMAN_MOVESOUND_ID);
		PACMAN_MOVESOUND_ID = play_audio(ghost_sound, effect_volume);
	default:
		break;
	}
}

void pacman_NextMove(Pacman* pacman, Directions next) {
	pacman->objData.nextTryMove = next;
}

void pacman_die() {
	stop_bgm(PACMAN_MOVESOUND_ID);
	PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume);
}





