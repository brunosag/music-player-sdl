#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#define FILE_PATH "/home/bsag/Music/main()/Porter Robinson - Musician.flac"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static void panic_and_abort(const char *title, const char *text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);

	SDL_Quit();
	exit(1);
}

int main() {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		panic_and_abort("SDL_Init failed", SDL_GetError());
	}

	window = SDL_CreateWindow("Hello SDL", SDL_WINDOWPOS_UNDEFINED,
							  SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
	if (!window) {
		panic_and_abort("SDL_CreateWindow failed", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		panic_and_abort("SDL_CreateRenderer failed", SDL_GetError());
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
		panic_and_abort("Mix_OpenAudio failed", SDL_GetError());
	}

	Mix_Music *music = Mix_LoadMUS(FILE_PATH);
	if (music == NULL) {
		panic_and_abort("Mix_LoadMUS failed", Mix_GetError());
	}

	if (Mix_PlayMusic(music, 0) != 0) {
		panic_and_abort("Mix_PlayMusic failed", Mix_GetError());
	}
	Mix_PauseMusic();

	const SDL_Rect rewind_rect = {100, 100, 100, 100};
	const SDL_Rect pause_rect = {400, 100, 100, 100};

	bool keep_going = true;
	while (keep_going) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_MOUSEBUTTONDOWN: {
					const SDL_Point pt = {e.button.x, e.button.y};
					if (SDL_PointInRect(&pt, &rewind_rect)) {
						Mix_RewindMusic();
					} else if (SDL_PointInRect(&pt, &pause_rect)) {
						Mix_PausedMusic() ? Mix_ResumeMusic()
										  : Mix_PauseMusic();
					}
					break;
				}
				case SDL_QUIT:
					keep_going = false;
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 24, 24, 27, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);

		SDL_RenderFillRect(renderer, &rewind_rect);
		SDL_RenderFillRect(renderer, &pause_rect);

		SDL_RenderPresent(renderer);
	}

	Mix_FreeMusic(music);
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}
