#include <libmbmeteor/libmbmeteor.h>
#include <SDL2/SDL.h>
#include <sstream>
using namespace gba;
using namespace std;

GBACore core;

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;

int screenwidth = 240;
int screenheight = 160;
int scale = 2;

int fpscount = 0;
Uint32 fpstime = 0;

bool initsdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	cout << "SDL could not be initialized!" << endl;
	return false;
    }

    window = SDL_CreateWindow("mbmeteor-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (screenwidth * scale), (screenheight * scale), SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
	cout << "Window could not be created!" << endl;
	return false;
    }

    surface = SDL_GetWindowSurface(window);
    return true;
}

void processinput(SDL_Event event)
{
    if (event.type == SDL_KEYDOWN)
    {
	switch (event.key.keysym.sym)
	{
	    case SDLK_a: core.keypressed(Button::A); break;
	    case SDLK_b: core.keypressed(Button::B); break;
	    case SDLK_RETURN: core.keypressed(Button::Start); break;
	    case SDLK_SPACE: core.keypressed(Button::Select); break;
	    case SDLK_l: core.keypressed(Button::L); break;
	    case SDLK_r: core.keypressed(Button::R); break;
	    case SDLK_LEFT: core.keypressed(Button::Left); break;
	    case SDLK_RIGHT: core.keypressed(Button::Right); break;
	    case SDLK_UP: core.keypressed(Button::Up); break;
	    case SDLK_DOWN: core.keypressed(Button::Down); break;
	}
    }
    else if (event.type == SDL_KEYUP)
    {
	switch (event.key.keysym.sym)
	{
	    case SDLK_a: core.keyreleased(Button::A); break;
	    case SDLK_b: core.keyreleased(Button::B); break;
	    case SDLK_RETURN: core.keyreleased(Button::Start); break;
	    case SDLK_SPACE: core.keyreleased(Button::Select); break;
	    case SDLK_l: core.keyreleased(Button::L); break;
	    case SDLK_r: core.keyreleased(Button::R); break;
	    case SDLK_LEFT: core.keyreleased(Button::Left); break;
	    case SDLK_RIGHT: core.keyreleased(Button::Right); break;
	    case SDLK_UP: core.keyreleased(Button::Up); break;
	    case SDLK_DOWN: core.keyreleased(Button::Down); break;
	}
    }
}

void drawpixels()
{
    SDL_Rect pixel = {0, 0, scale, scale};

    for (int i = 0; i < screenwidth; i++)
    {
	pixel.x = (i * scale);

	for (int j = 0; j < screenheight; j++)
	{
	    pixel.y = (j * scale);

	    uint8_t red = core.getpixel(i, j).red;
	    uint8_t green = core.getpixel(i, j).green;
	    uint8_t blue = core.getpixel(i, j).blue;

	    SDL_FillRect(surface, &pixel, SDL_MapRGBA(surface->format, red, green, blue, 255));
	}
    }

    SDL_UpdateWindowSurface(window);
}

void stopsdl()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    if (!core.getoptions(argc, argv))
    {
	return 1;
    }

    if (!core.init())
    {
	return 1;
    }

    if (!initsdl())
    {
	return 1;
    }

    bool quit = false;
    SDL_Event ev;

    Uint32 framecurrenttime = 0;
    Uint32 framestarttime = 0;

    while (!quit)
    {
	while (SDL_PollEvent(&ev))
	{
	    processinput(ev);
	    if (ev.type == SDL_QUIT)
	    {
		quit = true;
	    }
	}

	core.runcore();
	drawpixels();

	framecurrenttime = SDL_GetTicks();

	if ((framecurrenttime - framestarttime) < 17)
	{
	    SDL_Delay(17 - (framecurrenttime - framestarttime));
	}

	framestarttime = SDL_GetTicks();

	fpscount++;

	if (((SDL_GetTicks() - fpstime) >= 1000))
	{
	    fpstime = SDL_GetTicks();
	    stringstream title;
	    title << "mbmeteor-SDL2-" << fpscount << " FPS";
	    SDL_SetWindowTitle(window, title.str().c_str());
	    fpscount = 0;
	}
    }


    core.shutdown();

    stopsdl();

    return 0;
}
