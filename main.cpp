#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

using namespace std;

#define FILE_PATH "/home/bsag/Music/main()/Porter Robinson - Cheerleader.flac"

void panic(const char *title, const char *details) {
	cerr << title << ": " << details << endl;
	Mix_CloseAudio();
	SDL_Quit();
	exit(1);
}

int main() {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		panic("SDL_Init Error", SDL_GetError());
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
		panic("Mix_OpenAudio Error", SDL_GetError());
	}

	Mix_Music *music = Mix_LoadMUS(FILE_PATH);
	if (music == NULL) {
		panic("Mix_LoadMUS Error", Mix_GetError());
	}

	if (Mix_PlayMusic(music, 0) != 0) {
		panic("Mix_PlayMusic Error", Mix_GetError());
	}

	SDL_Event event;
	bool running = true;

	while (running && Mix_PlayingMusic()) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT) {
			running = false;
		}
		SDL_Delay(100);
	}

	Mix_FreeMusic(music);
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
