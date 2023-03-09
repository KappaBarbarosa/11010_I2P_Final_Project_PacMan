
#include "ghost.h"
#include "pacman_obj.h"
#include "map.h"
/* Shared variables */
#define GO_OUT_TIME 256
extern uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
extern const int cage_grid_x, cage_grid_y;
extern ALLEGRO_TIMER* boss_timer;
extern int sposition[5][2];
extern bool boss_mode;
/* Declare static function prototypes */
static void ghost_boss_move_script_FREEDOM(Ghost* ghost, Map* M);
static void ghost_boss_move_script_BLOCKED(Ghost* ghost, Map* M);
static void ghost_boss_move_script_BOSS(Ghost* ghost, Map* M,Pacman* pman){
    Directions shortestmove = boss_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pman->objData.Coord.x, pman->objData.Coord.y);
	//printf("to %d and %d\n",pman->objData.Coord.x,pman->objData.Coord.y);
    ghost_NextMove(ghost, shortestmove);
}
static void ghost_boss_move_script_BLOCKED(Ghost* ghost, Map* M) {

	switch (ghost->objData.preMove)
	{
	case UP:
		if (ghost->objData.Coord.y == 14)
			ghost_NextMove(ghost, DOWN);
		else
			ghost_NextMove(ghost, UP);
		break;
	case DOWN:
		if (ghost->objData.Coord.y == 15)
			ghost_NextMove(ghost, UP);
		else
			ghost_NextMove(ghost, DOWN);
		break;
	default:	
		break;
	}
}
void ghost_boss_move_script(Ghost* ghost, Map* M, Pacman* pacman) {
	if (!movetime(ghost->speed))
		return;
		switch (ghost->status)
		{
		case BLOCKED:
			ghost_boss_move_script_BLOCKED(ghost, M);
            if (boss_mode)
			{
				printf(" to boss\n");
                    ghost->status = BOSS;}
			break;
		case FLEE:
			ghost_move_script_FLEE(ghost, M, pacman);
			break;
        case BOSS:
            ghost_boss_move_script_BOSS(ghost, M, pacman);
            break;
		default:
			break;
		}
        ghost->objData.preMove = ghost->objData.nextTryMove;		
		
		switch (ghost->objData.preMove) {
		case RIGHT:
			ghost->objData.Coord.x += 1;
			break;
		case LEFT:
			ghost->objData.Coord.x -= 1;
			break;
		case UP:
			ghost->objData.Coord.y -= 1;
			break;
		case DOWN:
			ghost->objData.Coord.y += 1;
			break;
		default:
			break;
		}
		ghost->objData.facing = ghost->objData.preMove;
		ghost->objData.moveCD = GAME_TICK_CD;
}
