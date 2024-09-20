#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_FILES 1024
#define PATH_MAX_LENGTH 512

typedef struct {
	char files[MAX_FILES][PATH_MAX_LENGTH];
	int length;
} Playlist;

static bool window_should_close = false;
static SDL_Window *window;
static SDL_Renderer *renderer;
static Mix_Music *music;

static const SDL_Rect rewind_rect = {100, 100, 100, 100};
static const SDL_Rect pause_rect = {400, 100, 100, 100};

static void panic_and_abort(const char *title, const char *text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, window);

	SDL_Quit();
	exit(1);
}

static void load_music(const char *filepath) {
	music = Mix_LoadMUS(filepath);
	if (music == NULL) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to load file!",
								 Mix_GetError(), window);
	}
}

static void play_music() {
	if (Mix_PlayMusic(music, 0) != 0) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to play music!",
								 Mix_GetError(), window);
	}
}

static void handle_mousebuttondown(SDL_Event event) {
	const SDL_Point target = {event.button.x, event.button.y};
	if (SDL_PointInRect(&target, &rewind_rect)) {
		Mix_RewindMusic();
	} else if (SDL_PointInRect(&target, &pause_rect)) {
		Mix_PausedMusic() ? Mix_ResumeMusic() : Mix_PauseMusic();
	}
}

static bool is_directory(const char *filepath) {
	struct stat statbuf;
	stat(filepath, &statbuf);
	return S_ISDIR(statbuf.st_mode);
}

static Playlist get_playlist(const char *path) {
	DIR *dir;
	struct dirent *entry;
	Playlist playlist;
	playlist.length = 0;

	dir = opendir(path);
	if (dir) {
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == 8) { // Check if regular file
				snprintf(playlist.files[playlist.length], PATH_MAX_LENGTH,
						 "%s/%s", path, entry->d_name);
				printf("%s\n", playlist.files[playlist.length]);
				playlist.length++;
			}
		}
		closedir(dir);
	}

	return playlist;
}

static void swap(char *a, char *b) {
	char *tmp = *a;
	*a = *b;
	*b = tmp;
}

static void shuffle(char arr[][], int arr_len) {
	srand(time(NULL));
	for (int i = 0; i < arr_len; i++) {
	}
}

static void handle_dropfile(SDL_Event event) {
	if (is_directory(event.drop.file)) {
		Playlist playlist = get_playlist(event.drop.file);
	} else {
		load_music(event.drop.file);
	}
	if (music != NULL) {
		play_music(music);
	}
	SDL_free(event.drop.file);
}

static void handle_event(SDL_Event event) {
	switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			handle_mousebuttondown(event);
			break;
		case SDL_DROPFILE: {
			handle_dropfile(event);
			break;
		}
		case SDL_QUIT: {
			window_should_close = true;
			break;
		}
	}
}

static void render() {
	SDL_SetRenderDrawColor(renderer, 24, 24, 27, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 250, 250, 250, 255);
	SDL_RenderFillRect(renderer, &rewind_rect);
	SDL_RenderFillRect(renderer, &pause_rect);

	SDL_RenderPresent(renderer);
}

int main() {
	SDL_Event event;

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

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	while (!window_should_close) {
		while (SDL_PollEvent(&event)) {
			handle_event(event);
		}
		render();
	}

	Mix_CloseAudio();
	Mix_FreeMusic(music);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
