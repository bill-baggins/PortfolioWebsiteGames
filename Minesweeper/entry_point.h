#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include <memory>

class EntryPoint
{
public:
	EntryPoint();
	~EntryPoint();
	void run();

private:
	void update();
	void draw();

public:
	std::unique_ptr<class GameScene> m_current_scene;
};

#endif

