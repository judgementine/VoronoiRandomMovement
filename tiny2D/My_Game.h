#pragma once

#include "include/Engine_core.h"
#include "include/Light_core.h"
#include "include/Actor_core.h"
#include "include/Particle_core.h"

namespace My_Game
{
	int m(int x, int y, int ncols)
	{
		return y*ncols + x;
	}

	void populate_Map_Example(Grid::Grid *g)
	{
		for (int y = 0; y < g->n_rows; y++)
		{
			for (int x = 0; x < g->n_cols; x++)
			{
				g->data[m(x, y, g->n_cols)] = rand() % 3;
			}
		}
	}

	void random_Walk_Step(Grid::Point *p, Grid::Grid *g, int index)
	{
		switch (rand() % 4)
		{
		case 0:
			if (--p->y < 0)
				p->y = 0;
			break;
		case 1:
			if (++p->x >= g->n_cols)
				p->x=g->n_cols-1;
			break;
		case 2:
			if (++p->y >= g->n_rows)
				p->y=g->n_rows-1;
			break;
		case 3:
			if (--p->x < 0)
				p->x=0;
			break;



		}

		g->data[m(p->x, p->y, g->n_cols)]= index;

	}

	namespace World
	{

		namespace Parameters
		{
			int step_frequency = 100;

		}
		//manages a tileset image
		Tileset::Tileset tileset;

		//manages font
		Font::Font text;
		unsigned int last_step_time = 0;
		
		Grid::Point current_position;
		Grid::Grid map;
		Grid::Point * points;


		Grid_Camera::Grid_Camera camera;

	}

	//do allocations and system initialization here
	void init(int screen_w, int screen_h)
	{
		srand(time(0));

		//initialize all systems and open game window
		Engine::init("hello", screen_w, screen_h);

		Font::init(&World::text, "font_tileset.txt", Engine::renderer);
		
		Tileset::init(&World::tileset, "map_tileset.txt", Engine::renderer);

		Grid::init(&World::map, 48, 64);
		World::points = new Grid::Point[10];

		Grid_Camera::init(&World::camera, Engine::screen_width, Engine::screen_height);
	
	}

	void begin_Play(unsigned int current_time)
	{
		World::camera.world_coord.x = 0;
		World::camera.world_coord.y = 0;
		World::camera.world_coord.w = World::map.n_cols;
		World::camera.world_coord.h = World::map.n_rows;
		
		memset(World::map.data, 0, sizeof(int)*World::map.n_cols*World::map.n_rows);


		for (int i = 0; i < 10; i++)
		{
			World::points[i].x = rand() % 64;
			World::points[i].y = rand() % 48;
		}
		World::current_position.x = World::map.n_cols / 2;
		World::current_position.y = World::map.n_rows / 2;
	}
	
	void update(unsigned int current_time, float dt)
	{

		Engine::event_Loop();
		Grid_Camera::calibrate(&World::camera);



		if (current_time - World::last_step_time >= World::Parameters::step_frequency)
		{
			World::last_step_time = current_time;
			for (int i = 0; i < 10; i++)
			{
				random_Walk_Step(&World::points[i], &World::map, i);
			}
		}
		for (int i = 0; i < World::map.n_rows; i++)
		{
			for (int j = 0; j < World::map.n_cols; j++)
			{
				int Closest = INT_MAX;
				int closestIndex;
				for (int k = 0; k < 10; k++)
				{
					if (pow(World::points[k].x-i, 2) + pow(World::points[k].y-j, 2) < Closest)
					{
						Closest = pow(World::points[k].x - i, 2) + pow(World::points[k].y - j, 2);
						closestIndex = k;
					}
				}
				World::map.data[m(j, i, World::map.n_cols)] = closestIndex;
			}
		}

	}

	void draw(unsigned int current_time)
	{
		SDL_RenderClear(Engine::renderer);

		Tileset::draw_Grid(&World::tileset, &World::camera, &World::map, &RGBA::default, Engine::renderer);

		//flip buffers
		SDL_RenderPresent(Engine::renderer);
	}

}