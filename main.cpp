#include <SDL.h>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"

SDL_FPoint lerp(SDL_FPoint a, SDL_FPoint b, float t) {
	SDL_FPoint res = { t * t * (b.x - a.x) + a.x, t * t * (b.y - a.y) + a.y };
	return res;
}

SDL_FPoint bezierCurve(SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3, float t) {
	SDL_FPoint pa = lerp(p1, p2, t);
	SDL_FPoint pb = lerp(p2, p3, t);
	return lerp(pa, pb, t);
}

void drawBezierCurve(SDL_Renderer* pRenderer, SDL_FPoint p1, SDL_FPoint p2, SDL_FPoint p3, int stepCount) {

	float dt = 1.0f / stepCount;
	float t = 0.0f;

	SDL_FPoint previousPoint = bezierCurve(p1, p2, p3, t);

	for (int i = 1; i <= stepCount; ++i)
	{
		t += dt;
		SDL_FPoint currentPoint = bezierCurve(p1, p2, p3, t);

		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		SDL_RenderDrawLine(pRenderer, previousPoint.x, previousPoint.y, currentPoint.x, currentPoint.y);

		previousPoint = currentPoint;
	}
}

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	bool running = true;

	while (running)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			ImGui_ImplSDL2_ProcessEvent(&ev);
			if ((SDL_QUIT == ev.type) ||
				(SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
			{
				running = false;
				break;
			}
		}

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		ImGui::Begin("Insert values");
		static float p1[2];
		ImGui::DragFloat2("P1 :", p1);
		static float p2[2];
		ImGui::DragFloat2("P2 :", p2);
		static float p3[2];
		ImGui::DragFloat2("P3 :", p3);

		static int stepCount;
		ImGui::DragInt("step count :", &stepCount);
		ImGui::End();


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderDrawLineF(renderer, p1[0], p1[1], p2[0], p2[1]);
		SDL_RenderDrawLineF(renderer, p2[0], p2[1], p3[0], p3[1]);
		drawBezierCurve(renderer, { p1[0],p1[1] }, { p2[0],p2[1] }, { p3[0],p3[1] }, stepCount);

		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
