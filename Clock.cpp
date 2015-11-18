
#include "Clock.hpp"

Clock* Clock::clk_ = 0;

Clock::Clock():TimeModefier(1), fpsCounter_(0), frameCount_(0)
{
}

Clock::~Clock()
{
}

Clock* Clock::GetInstance()
{
	return clk_;
}

void Clock::Update()
{
	frameCount_++;

	lastFrameTime_ = currentFrameTime_;
	currentFrameTime_ = SDL_GetTicks();

	fpsCounter_ += (currentFrameTime_ - lastFrameTime_);


	if(fpsCounter_ - fpsCounterStart_ >= 1000)
	{
		frameRate_ = frameCount_;
		frameCount_ = 0;
		fpsCounterStart_ = currentFrameTime_;
		fpsCounter_ = fpsCounterStart_;
	}

}

int Clock::GetDeltaTime()
{
	return (int)(((float)currentFrameTime_ - (float)lastFrameTime_) * TimeModefier);
}

void Clock::SetTimeModefier(float t)
{
	TimeModefier= t;
}

int Clock::GetFramesPerSecond()
{
	return 	frameRate_;
}