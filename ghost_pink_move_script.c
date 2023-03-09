
#include "ghost.h"
#include "pacman_obj.h"
#include "map.h"
/* Shared variables */
#define GO_OUT_TIME 256
extern uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
extern const int cage_grid_x, cage_grid_y;
extern int sposition[5][2];
/* Declare static function prototypes */
static void ghost_pink_move_script_FREEDOM(Ghost* ghost, Map* M);
static void ghost_pink_move_script_BLOCKED(Ghost* ghost, Map* M);

static void ghost_pink_move_script_FREEDOM(Ghost* ghost, Map* M) {
	
static Directions proba[4]; // possible movement
	int cnt = 0;
	int answer=0;
	bool goddone = false;
	for (Directions i = 1; i <= 4; i++)
		if (ghost_movable(ghost, M,i, true)) 	{
		if(i== ghost->objData.preMove) goddone = true; 
		proba[cnt++] = i;			
		}
		if(goddone && ghost_movable(ghost,M,ghost->objData.preMove,false)){
			 ghost_NextMove(ghost, ghost->objData.preMove);
		}
		else
			ghost_NextMove(ghost, proba[rand()%4]);	
}

static void ghost_pink_move_script_BLOCKED(Ghost* ghost, Map* M) {

	switch (ghost->objData.preMove)
	{
	case UP:
		if (ghost->objData.Coord.y == sposition[2][1]-1)
			ghost_NextMove(ghost, DOWN);
		else
			ghost_NextMove(ghost, UP);
		break;
	case DOWN:
		if (ghost->objData.Coord.y == sposition[2][1]+1)
			ghost_NextMove(ghost, UP);
		else
			ghost_NextMove(ghost, DOWN);
		break;
	default:
		ghost_NextMove(ghost, UP);
		break;
	}
}

void ghost_pink_move_script(Ghost* ghost, Map* M, Pacman* pacman, Ghost* boss) {
	if (!movetime(ghost->speed))
		return;
		switch (ghost->status)
		{
		case BLOCKED:
			ghost_pink_move_script_BLOCKED(ghost, M);
			if (al_get_timer_count(game_tick_timer) > GO_OUT_TIME)
				ghost->status = GO_OUT;
			break;
		case FREEDOM:
			ghost_pink_move_script_FREEDOM(ghost, M);
			break;
		case GO_OUT:
			ghost_move_script_GO_OUT(ghost, M);
			break;
		case GO_IN:
			ghost_move_script_GO_IN(ghost, M);
			if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B') {
				ghost->status = GO_OUT;
				ghost->speed = 2;
			}
			break;
		case FLEE:
			ghost_move_script_FLEE(ghost, M, pacman);
			break;
		case SERVANT:
			ghost_move_script_SERVANT(ghost, M, boss);
			break;
		default:
			break;
		}

		if(ghost_movable(ghost, M, ghost->objData.nextTryMove, false)){
			ghost->objData.preMove = ghost->objData.nextTryMove;
			ghost->objData.nextTryMove = NONE;
		}
		else if (!ghost_movable(ghost, M, ghost->objData.preMove, false))
			return;

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
