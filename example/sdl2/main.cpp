#include <libmbmeteor/libmbmeteor.h>
#include <SDL2/SDL.h>
#include "termcolor.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include <cassert>
using namespace gba;
using namespace std;
using namespace std::placeholders;

GBACore core;
PowerAntenna power;

SDL_Window *window = NULL;
SDL_Renderer *render = NULL;
SDL_Texture *texture = NULL;

int screenwidth = 240;
int screenheight = 160;
int scale = 2;

int fpscount = 0;
Uint32 fpstime = 0;

class TermcolorInterface : public PowerAntennaInterface
{
    public:
    	TermcolorInterface()
    	{
    	
    	}
    	
    	~TermcolorInterface()
    	{
    	
    	}
    	
    	void ledoff()
    	{
	    cout << beeterm::white << "LED is off..." << endl;
	    cout << beeterm::reset << endl;
    	}
    	
    	void ledonstrong()
    	{
	    cout << beeterm::blue << "LED is emitting strong light..." << endl;
	    cout << beeterm::reset << endl;
    	}
    	
    	void ledonweak()
    	{
    	    cout << beeterm::dark << beeterm::blue << "LED is emitting weak light..." << endl;
    	    cout << beeterm::reset << endl;
    	}
};

void screenshot()
{
    time_t currenttime = time(nullptr);
    string screenstring = "mbmeteor_";
    screenstring.append(std::to_string(currenttime));
    screenstring.append(".bmp");

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, (screenwidth * scale), (screenheight * scale), 32, SDL_PIXELFORMAT_ARGB8888);
    SDL_RenderReadPixels(render, NULL, SDL_PIXELFORMAT_ARGB8888, surface->pixels, surface->pitch);

    SDL_SaveBMP(surface, screenstring.c_str());

    cout << "Screenshot saved." << endl;
    SDL_FreeSurface(surface);
}

bool initsdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
	cout << "SDL could not be initialized!" << endl;
	return false;
    }

    window = SDL_CreateWindow("mbmeteor-SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (screenwidth * scale), (screenheight * scale), SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
	cout << "Window could not be created!" << endl;
	return false;
    }

    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(render, 240, 160);    

    if (render == NULL)
    {
	cout << "Renderer could not be created!" << endl;
	return false;
    }    

    texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 240, 160);

    SDL_AudioSpec audiospec;
    audiospec.format = AUDIO_S16SYS;
    audiospec.freq = 48000;
    audiospec.samples = 4096;
    audiospec.channels = 2;
    audiospec.callback = NULL;

    SDL_AudioSpec obtainedspec;
    SDL_OpenAudio(&audiospec, &obtainedspec);
    SDL_PauseAudio(0);

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
	    case SDLK_q: screenshot(); break;
	    case SDLK_EQUALS: core.increasesolar(); break;
	    case SDLK_MINUS: core.decreasesolar(); break;
	    case SDLK_p: core.paused = !core.paused; break;
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
    assert(render && texture);
    SDL_UpdateTexture(texture, NULL, core.framebuffer(), (240 * sizeof(RGB)));
    SDL_RenderClear(render);
    SDL_RenderCopy(render, texture, NULL, NULL);
    SDL_RenderPresent(render);
}

vector<int16_t> buffer;

void sdlcallback(int16_t left, int16_t right)
{
    buffer.push_back(left);
    buffer.push_back(right);

    if (buffer.size() >= 4096)
    {
	buffer.clear();

	while ((SDL_GetQueuedAudioSize(1)) > (4096 * sizeof(int16_t)))
	{
	    SDL_Delay(1);
	}
	SDL_QueueAudio(1, &buffer[0], (4096 * sizeof(int16_t)));
    }
}

void stopsdl()
{
    SDL_CloseAudio();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    core.setaudiocallback(bind(&sdlcallback, _1, _2));
    core.setpixelcallback(bind(&drawpixels));

    if (!core.getoptions(argc, argv))
    {
	return 1;
    }
    
    if (core.ispowerenabled)
    {	
    	TermcolorInterface inter;
    	power.setinterface(&inter);
	core.coreserial->setwritecallback(bind(&PowerAntenna::powerwrite, &power, _1, _2));
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
	
	SDL_PauseAudio(core.paused);

	if (!core.paused)
	{
	    core.runcore();
	}

	framecurrenttime = SDL_GetTicks();

	if ((framecurrenttime - framestarttime) < 16)
	{
	    SDL_Delay(16 - (framecurrenttime - framestarttime));
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
