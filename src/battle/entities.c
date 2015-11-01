/*
Copyright (C) 2015 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "entities.h"

static void drawEntity(Entity *e);
static void doEntity(void);
static void activateEpicFighters(int n, int side);

Entity *spawnEntity(void)
{
	Entity *e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	e->id = battle.entId++;
	e->active = 1;
	
	battle.entityTail->next = e;
	battle.entityTail = e;
	
	return e;
}

void doEntities(void)
{
	int numAllies, numEnemies;
	int numActiveAllies, numActiveEnemies;
	Entity *e, *prev;
	
	prev = &battle.entityHead;
	
	numAllies = numEnemies = numActiveAllies = numActiveEnemies = 0;
	
	for (e = battle.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->active)
		{
			self = e;
			
			e->x += e->dx;
			e->y += e->dy;
			
			if (e->action != NULL)
			{
				if (--e->thinkTime <= 0)
				{
					e->thinkTime = 0;
					e->action();
				}
			}
			
			switch (e->type)
			{
				case ET_FIGHTER:
					doFighter();
					
					if (self->side == SIDE_ALLIES)
					{
						numActiveAllies++;
					}
					else
					{
						numActiveEnemies++;
					}
					
					break;
					
				default:
					doEntity();
					break;
			}
			
			if (e->alive == ALIVE_DEAD)
			{
				if (e == battle.entityTail)
				{
					battle.entityTail = prev;
				}
				
				if (e == battle.missionTarget)
				{
					battle.missionTarget = NULL;
				}
				
				if (e == player)
				{
					player = NULL;
					
					battle.playerSelect = battle.epic;
				}
				
				prev->next = e->next;
				free(e);
				e = prev;
			}
		}
		
		if (e->type == ET_FIGHTER && (battle.epic || e->active))
		{
			if (self->side == SIDE_ALLIES)
			{
				numAllies++;
			}
			else
			{
				numEnemies++;
			}
		}
		
		prev = e;
	}
	
	battle.numAllies = numAllies;
	battle.numEnemies = numEnemies;
	
	if (battle.epic && battle.stats[STAT_TIME] % FPS == 0)
	{
		if (numAllies > battle.epicFighterLimit)
		{
			activateEpicFighters(battle.epicFighterLimit - numActiveAllies, SIDE_ALLIES);
		}
		
		if (numEnemies > battle.epicFighterLimit)
		{
			activateEpicFighters(battle.epicFighterLimit - numActiveEnemies, SIDE_NONE);
		}
	}
}

static void doEntity(void)
{
	if (self->health <= 0)
	{
		self->alive = ALIVE_DEAD;
	}
}

void drawEntities(void)
{
	Entity *e;
	
	for (e = battle.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->active)
		{
			switch (e->type)
			{
				case ET_FIGHTER:
					drawFighter(e);
					break;
					
				default:
					drawEntity(e);
					break;
			}
		}
	}
}

static void drawEntity(Entity *e)
{
	blitRotated(e->texture, e->x - battle.camera.x, e->y - battle.camera.y, e->angle);
}

void activateEntities(char *name)
{
	Entity *e;
	
	for (e = battle.entityHead.next ; e != NULL ; e = e->next)
	{
		if (strcmp(e->name, name) == 0)
		{
			e->active = 1;
		}
	}
}

static void activateEpicFighters(int n, int side)
{
	Entity *e;
	
	if (n > 0)
	{
		for (e = battle.entityHead.next ; e != NULL ; e = e->next)
		{
			if (!e->active && e->type == ET_FIGHTER && ((side == SIDE_ALLIES && e->side == SIDE_ALLIES) || (side != SIDE_ALLIES && e->side != SIDE_ALLIES)))
			{
				e->active = 1;
				
				if (--n <= 0)
				{
					return;
				}
			}
		}
	}
}
