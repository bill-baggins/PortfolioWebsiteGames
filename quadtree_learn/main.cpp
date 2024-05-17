#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <algorithm>
#include <memory>

#include "mytypes.h"
#include "raylib.h"
#include "raymath.h"

constexpr u32 MAX_PER_NODE = 4;

enum Location 
{
	NORTHEAST,
	NORTHWEST,
	SOUTHWEST,
	SOUTHEAST
};


class QuadTree 
{
public:
	QuadTree() = default;
	QuadTree(Rectangle bound);
	~QuadTree();
	QuadTree& operator=(const QuadTree& other) = default;

	bool insert(Vector2 point);
	bool subdivide();
	std::vector<Vector2> query_range(Rectangle area);
	bool remove(Vector2 point);
	void clear();

private:
	Rectangle m_bound;
	std::vector<Vector2> m_points;
	QuadTree* m_northwest;
	QuadTree* m_northeast;
	QuadTree* m_southwest;
	QuadTree* m_southeast;
};


QuadTree::QuadTree(Rectangle bound)
{
	m_bound = bound;
	m_points = {};
	m_northeast = nullptr;
	m_northwest = nullptr;
	m_southeast = nullptr;
	m_southwest = nullptr;
}


QuadTree::~QuadTree()
{
	delete m_northeast;
	delete m_northwest;
	delete m_southeast;
	delete m_southwest;
	m_northeast	= nullptr;
	m_northwest	= nullptr;
	m_southeast	= nullptr;
	m_southwest	= nullptr;
}


bool QuadTree::insert(Vector2 point)
{
	if (!CheckCollisionPointRec(point, m_bound))
	{
		return false;
	}

	if (m_points.size() < MAX_PER_NODE && m_northwest != nullptr)
	{
		m_points.push_back(point);
		return true;
	}

	if (m_northwest == nullptr)
	{
		subdivide();
	}

	if (m_northwest->insert(point)) return true;
	if (m_northeast->insert(point)) return true;
	if (m_southwest->insert(point)) return true;
	if (m_southeast->insert(point)) return true;

	return false;
}


bool QuadTree::subdivide()
{
	Vector2 new_size = { int(m_bound.width / 2), int(m_bound.height / 2) };
	if (new_size.x - 0.f < 0.001f && new_size.y - 0.f < 0.001f)
	{
		return false;
	}

	m_northwest = new QuadTree(Rectangle{
		m_bound.x, m_bound.y, new_size.x, new_size.y
	});

	m_northeast = new QuadTree(Rectangle{
		m_bound.x + new_size.x, m_bound.y, new_size.x, new_size.y
	});

	m_southwest = new QuadTree(Rectangle{
		m_bound.x, m_bound.y + new_size.y, new_size.x, new_size.y
	});

	m_southeast = new QuadTree(Rectangle{
		m_bound.x + new_size.x, m_bound.y + new_size.y, new_size.x, new_size.y
	});

	return true;
}

std::vector<Vector2> QuadTree::query_range(Rectangle area)
{
	std::vector<Vector2> res = {};
	if (!CheckCollisionRecs(m_bound, area))
	{
		return res;
	}

	for (Vector2 point : res) 
	{
		if (CheckCollisionPointRec(point, area))
		{
			res.push_back(point);
		}
	}

	std::vector<Vector2> nw_res = m_northwest->query_range(area);
	std::vector<Vector2> ne_res = m_northeast->query_range(area);
	std::vector<Vector2> sw_res = m_southwest->query_range(area);
	std::vector<Vector2> se_res = m_southeast->query_range(area);

	res.insert(res.end(), nw_res.begin(), nw_res.end());
	res.insert(res.end(), ne_res.begin(), ne_res.end());
	res.insert(res.end(), sw_res.begin(), sw_res.end());
	res.insert(res.end(), se_res.begin(), se_res.end());

	return res;
}

bool QuadTree::remove(Vector2 point)
{
	if (m_points.size() == 0) 
	{
		return false;
	}

	auto to_remove = m_points.end() + 1;
	for (auto it = m_points.begin(); it != m_points.end(); it++) 
	{
		if (Vector2Equals(*it, point))
		{
			to_remove = it;
			break;
		}
	}

	if (to_remove != m_points.end() - 1)
	{
		m_points.erase(to_remove);
		return true;
	}

	if (CheckCollisionPointRec(point, m_northwest->m_bound))
	{
		m_northwest->remove(point);
	}

	if (CheckCollisionPointRec(point, m_northeast->m_bound))
	{
		m_northwest->remove(point);
	}

	if (CheckCollisionPointRec(point, m_southeast->m_bound))
	{
		m_northwest->remove(point);
	}

	if (CheckCollisionPointRec(point, m_southwest->m_bound))
	{
		m_northwest->remove(point);
	}
}

void QuadTree::clear()
{
	m_points.clear();
	delete m_northeast;
	delete m_northwest;
	delete m_southeast;
	delete m_southwest;
	m_northeast = nullptr;
	m_northwest = nullptr;
	m_southeast = nullptr;
	m_southwest = nullptr;
}



i32 main(void)
{
	InitWindow(800, 600, "QuadTree");
	SetTargetFPS(60);
	SetExitKey(0);

	std::unique_ptr<QuadTree> tree = std::make_unique<QuadTree>(Rectangle{ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() });

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();
		tree->update()
		BeginDrawing();
		ClearBackground(BLACK);
		DrawFPS(0, 0);
		EndDrawing();
	}

	return 0;
}


/* Python Quadtree reference (the prototype I wrote)

# TODO: Make the list of points inside of the quadtree a list
# of Puck objects. That way, I should be able to update the quadtree easier.
# For each puck inside of the quadtree node:
# 1) update the puck using the update method.
# 2) check if the puck has collided with another puck. If it did, we want to
#    set their velocities accordingly.
# 3) check if the puck is at the edge of the window, in which case we will
#    cause the puck to go backwards.
# 4) if the puck has stepped out of it's bounds within the quadtree node, remove it
#    and call the root node's insert() method to figure out the new home for the
#    node. As an optimization, I could use a smarter means of determining which child node
#    of the immediate parent it should go to.
#    To call the root node's insert() method, I need to keep a pointer to it.

import random
import pygame as pg
import glm

WIDTH = 800
HEIGHT = 600
CAPTION = "QuadTree"
FPS = 60

CELL_WIDTH = 5
CELL_HEIGHT = 5

def rect_contains_point(rect: pg.Rect, point: glm.vec2):
	return point.x >= rect.x and point.x < (rect.x + rect.width) and \
		point.y >= rect.y and point.y < (rect.y + rect.height)


class Text:
	@staticmethod
	def fps(font: pg.Font, clock: pg.Clock):
		return font.render(f"{round(clock.get_fps())}", True, (0, 255, 0, 255))


class Surf:
	cell: pg.Surface

	@staticmethod
	def load():
		Surf.cell = pg.Surface((CELL_WIDTH, CELL_HEIGHT)).convert()
		Surf.cell.fill((255, 0, 0))


class Puck:
	def __init__(self, point: glm.vec2):
		self.point = point
		self.vel = glm.vec2(random.randrange(-50, 50), random.randrange(-50, 50))

	def update(self, dt: float):
		self.point.x += self.vel.x * dt
		if self.point.x < 0:
			self.point.x = 0
			self.vel.x *= -1
		elif self.point.x >= WIDTH:
			self.point.x = WIDTH - 1
			self.vel.x *= -1

		self.point.y += self.vel.y * dt
		if self.point.y < 0:
			self.point.y = 0
			self.vel.y *= -1
		elif self.point.y >= HEIGHT:
			self.point.y = HEIGHT - 1
			self.vel.y *= -1

	def draw(self, screen: pg.Surface):
		screen.blit(Surf.cell, self.point)

PUCKS: list[Puck] = []

class QuadTree:
	def __init__(self, bound: pg.Rect):
		self.__MAX_NODE_CAP = 4
		self.bound = bound
		self.points: list[glm.vec2] = []
		self.northwest: QuadTree = None
		self.northeast: QuadTree = None
		self.southeast: QuadTree = None
		self.southwest: QuadTree = None

		self.mouse_buttons = [
			False,
			False,
			False,
			False,
			False,
		]

	def create_from_pucks(self, pucks: list[Puck]):
		self.clear()
		[self.insert(puck.point) for puck in pucks]

	def onmousebuttondown(self, dt: float, event: pg.event.Event):
		self.mouse_buttons[event.button] = True

	def onmousebuttonup(self, dt: float, event: pg.event.Event):
		self.mouse_buttons[event.button] = False

	def onkeydown(self, dt: float, event: pg.event.Event):
		if event.key == pg.K_e:
			PUCKS.clear()
			self.points.clear()
			self.northeast = None
			self.northwest = None
			self.southwest = None
			self.southeast = None

	def update(self, dt: float):
		if self.mouse_buttons[1]:
			pos = pg.mouse.get_pos()
			self.insert(glm.vec2(pos[0], pos[1]))
			PUCKS.append(Puck(glm.vec2(pos[0], pos[1])))
		elif self.mouse_buttons[3]:
			pos = pg.mouse.get_pos()
			self.remove(glm.vec2(pos[0], pos[1]))
			PUCKS.append(Puck(glm.vec2(pos[0], pos[1])))

	def draw(self, screen: pg.Surface):
		pg.draw.rect(screen, (0, 0, 0), self.bound, border_radius=1, width=1)
		if len(self.points) == 0:
			return
		[screen.blit(Surf.cell, point) for point in self.points]
		self.northwest.draw(screen)
		self.northeast.draw(screen)
		self.southeast.draw(screen)
		self.southwest.draw(screen)

	def clear(self):
		self.points.clear()
		self.northeast = None
		self.northwest = None
		self.southwest = None
		self.southeast = None

	def insert(self, point: glm.vec2) -> bool:
		if not rect_contains_point(self.bound, point):
			return False

		if len(self.points) < self.__MAX_NODE_CAP and self.northwest is not None:
			self.points.append(point)
			return True

		if self.northwest is None:
			self.subdivide()

		if self.northwest.insert(point):
			return True
		if self.northeast.insert(point):
			return True
		if self.southwest.insert(point):
			return True
		if self.southeast.insert(point):
			return True

		return False

	def subdivide(self):
		self.new_size = (self.bound.width // 2, self.bound.height // 2)
		self.northwest = QuadTree(
			pg.Rect(self.bound.left, self.bound.top, self.new_size[0], self.new_size[1])
		)
		self.northeast = QuadTree(
			pg.Rect(self.bound.left + self.bound.width/2, self.bound.top, self.new_size[0], self.new_size[1])
		)
		self.southeast = QuadTree(
			pg.Rect(self.bound.left + self.bound.width/2, self.bound.top + self.bound.height/2, self.new_size[0], self.new_size[1])
		)
		self.southwest = QuadTree(
			pg.Rect(self.bound.left, self.bound.top + self.bound.height/2, self.new_size[0], self.new_size[1])
		)

	def query_range(self, range: pg.Rect) -> list[glm.vec2]:
		res = []
		if not self.bound.colliderect(range):
			return []

		for point in self.points:
			if rect_contains_point(self.bound, point):
				res.append(point)

		res.append(self.northwest.query_range(range))
		res.append(self.northeast.query_range(range))
		res.append(self.southeast.query_range(range))
		res.append(self.southwest.query_range(range))

		return res

	def remove(self, point: glm.vec2):
		if len(self.points) == 0:
			return

		try:
			self.points.remove(point)
		except ValueError:
			pass

		for section in (self.northwest, self.northeast, self.southwest, self.southeast):
			if rect_contains_point(section.bound, point):
				section.remove(point)



def main():
	pg.init()
	pg.display.set_mode([WIDTH, HEIGHT])
	pg.display.set_caption(CAPTION)

	font = pg.font.Font(size=40)
	clock = pg.time.Clock()
	screen = pg.display.get_surface()
	ms = dt = 0
	Surf.load()


	quadtree = QuadTree(screen.get_rect())

	running = True
	while running:
		dt = ms / 1000
		for event in pg.event.get():
			if event.type == pg.QUIT:
				running = False
				break

			if event.type == pg.MOUSEBUTTONDOWN:
				quadtree.onmousebuttondown(dt, event)

			if event.type == pg.MOUSEBUTTONUP:
				quadtree.onmousebuttonup(dt, event)

			if event.type == pg.KEYDOWN:
				quadtree.onkeydown(dt, event)

		if not running:
			break

		[puck.update(dt) for puck in PUCKS]
		quadtree.create_from_pucks(PUCKS)
		quadtree.update(dt)

		screen.fill((0x87, 0xce, 0xeb))
		screen.blit(Text.fps(font, clock), (0, 0))

		quadtree.draw(screen)

		ms = clock.tick(FPS)
		pg.display.update()




if __name__ == '__main__':
	main()


*/