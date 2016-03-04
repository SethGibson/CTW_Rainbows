#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct ptcl {
	ptcl() {}
	ptcl(float pAmp, float pRad, float pWL, float pSpeed, float pSize) :
		Amp(pAmp), Rad(pRad), WL(pWL), Speed(pSpeed), Size(pSize)
	{
		mTimer = randInt(-360, 360);
	}

	float Amp;
	float Rad;
	float WL;
	float Speed;
	float Size;
	vec2 Pos;

	int mTimer;

	void step()
	{
		Rad += (Amp*math<float>::cos(getElapsedFrames()*WL));
		Pos.x = math<float>::cos(toRadians((float)mTimer)*Speed)*Rad;
		Pos.y = math<float>::sin(toRadians((float)mTimer)*Speed)*Rad;

		
		mTimer -= 1;
	}

	void draw()	{ gl::drawSolidCircle(Pos, Size, 8); }

};

class SinOrbitApp : public App {
  public:
	SinOrbitApp();

	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

  private:
	void reset();

	bool  mStarted;
	float mAlpha;

	vector<ptcl>	mPtcls;
};

SinOrbitApp::SinOrbitApp() : mStarted(false)
{
	reset();
	gl::clear(Color(0, 0, 0));
}

void SinOrbitApp::mouseDown( MouseEvent event )
{
	if (mStarted)
		reset();

	mStarted = true;

	gl::disableAlphaBlending();
	gl::enableAlphaBlending();
}

void SinOrbitApp::update()
{
	if (mStarted) {
		for (auto &p : mPtcls) {
			p.step();
		}
	}
}

void SinOrbitApp::draw()
{
	if (mStarted) {
		gl::color(ColorA(0, 0, 0, mAlpha));
		gl::drawSolidRect(Rectf(vec2(), getWindowSize()));

		gl::color(Color::white());

		gl::pushModelMatrix();
		gl::translate(getWindowCenter());

		for (auto &p : mPtcls) {
			p.draw();
		}

		gl::popModelMatrix();
	}
}

void SinOrbitApp::reset()
{
	mAlpha = randFloat(0.01f, 0.1f);
	auto minR = randFloat(200.0f, 300.0f);
	auto maxR = randFloat(250.0f, 400.0f);
	auto numPtcls = randInt(500, 2000);
	mPtcls.clear();


	for (int i = 0; i < numPtcls; ++i) {
		mPtcls.push_back(
			ptcl(
				randFloat(0.2f, 0.8f),
				randFloat(minR, maxR),
				randFloat(0.01f, 0.05f),
				randFloat(0.3f, 1.1f),
				randFloat(1.0f, 3.0f)
				)
			);
	}
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1000, 1000);
	pSettings->setFrameRate(60);
}

CINDER_APP( SinOrbitApp, RendererGl, prepareSettings )
