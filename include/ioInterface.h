#include <string>

using namespace std;

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class IoInterface {
    public:
        IoInterface(string title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
        bool getInput(uint8_t* keys);
        void drawFrame(const void* buffer, int pitch);

    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
};