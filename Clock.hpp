#pragma once
#include <SDL.h>

#define CLK Clock::GetInstance();

class Clock
{
public:
	
	
	static Clock* GetInstance();


	void Update();
	int GetDeltaTime();
	void SetTimeModefier(float t);

	int GetFramesPerSecond();

private:

	static Clock* clk_;

	unsigned long int lastFrameTime_;
	unsigned long int currentFrameTime_;
	unsigned long int fpsCounter_;
	unsigned long int fpsCounterStart_;
	int frameCount_;

	int frameRate_;

	float TimeModefier;
	
	Clock();
	~Clock();
	

};
