#ifndef game_world_h
#define game_world_h

#include <vector>

class Game_World{
public:

    ///std::vector<Example> example_objects;

    void clear_world();
    void generate_world();

    void tick();
    void ai();
    void movement();
    void events();
    void animate();
    void render();
};

#endif
