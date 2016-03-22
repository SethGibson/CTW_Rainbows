#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct ptcl {
	ptcl() {}
	ptcl(float pAmp, float pRad, float pWL, float pSpeed, float pSize) :
		Amp(pAmp), Rad(pRad), WL(pWL), Speed(pSpeed), Size(pSize)
	{
		mTimer = randInt(-720, 720);
		//mTimer = 0;
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

class WhiteCircle : public App {
  public:
	WhiteCircle();

	void keyDown(KeyEvent pEvent) override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

  private:
	void setupGUI();
	void reset(bool pShader);

	bool  mStarted, mIsAdd, mDrawGui;
	float mAlpha;

	vector<ptcl>	mPtcls;
	vec4			mColor;

	gl::VboRef		mPointData;
	gl::VboMeshRef	mPointMesh;
	gl::GlslProgRef	mPointShader;

	params::InterfaceGlRef	mGUI;
	int mParamResetTime;
	float mParamBrightnessMin, mParamBrightnessMax;
	float mParamFadeAmtMin, mParamFadeAmtMax;
	float mParamWidthMin, mParamWidthMax;
	float mParamRadius;
	float mParamCountMin, mParamCountMax;

	float mParamAmpMin, mParamAmpMax;
	float mParamWLMin, mParamWLMax;
	float mParamSpeedMin, mParamSpeedMax;
	float mParamRotSpeedMin, mParamRotSpeedMax, mRotSpeed;
};

WhiteCircle::WhiteCircle() : mStarted(false)
{
	mDrawGui = true;
	setupGUI();
	reset(true);
	gl::clear(Color(0, 0, 0));

	gl::enableAlphaBlending();
	//gl::enableAdditiveBlending();
}

void WhiteCircle::keyDown(KeyEvent pEvent)
{
	if (pEvent.getCode() == KeyEvent::KEY_d) {
		mDrawGui = !mDrawGui;
	}
}

void WhiteCircle::mouseDown( MouseEvent event )
{
	if (mStarted)
		reset(false);

	mStarted = true;

}

void WhiteCircle::update()
{
	if (mStarted) {
		if (getElapsedFrames() % mParamResetTime == 0) {
			reset(false);
		}
		for (auto &p : mPtcls) {
			p.step();
		}
		mPointData->bufferData(mPtcls.size()*sizeof(ptcl), mPtcls.data(), GL_DYNAMIC_DRAW);
	}
}

void WhiteCircle::draw()
{
	gl::setMatricesWindow(getWindowSize());
	if (mStarted) {
		//gl::clear(Color::black());
		gl::color(ColorA(0, 0, 0, mAlpha));
		gl::drawSolidRect(Rectf(vec2(), getWindowSize()));

		gl::color(Color::white());

		gl::pushModelMatrix();
		gl::translate(getWindowCenter());
		gl::rotate(getElapsedFrames()*mRotSpeed);
		{
			gl::ScopedGlslProg shdr(mPointShader);
			mPointShader->uniform("u_Color", mColor);
			gl::ScopedState pts(GL_PROGRAM_POINT_SIZE, true);
			gl::draw(mPointMesh);
		}

		gl::popModelMatrix();
	}
	if(mDrawGui)
		mGUI->draw();
}

void WhiteCircle::reset(bool pShader)
{
	mColor = vec4(randFloat(0.1f,1.0f), randFloat(0.1f, 1.f), randFloat(0.1f, 1.f), randFloat(mParamBrightnessMin, mParamBrightnessMax));
	mAlpha = randFloat(mParamFadeAmtMin, mParamFadeAmtMax);
	auto w = randFloat(mParamWidthMin,mParamWidthMax);
	auto numPtcls = randInt(mParamCountMin, mParamCountMax);
	mRotSpeed = randFloat(mParamRotSpeedMin, mParamRotSpeedMax);

	mPtcls.clear();

	for (int i = 0; i < numPtcls; ++i) {
		mPtcls.push_back(
			ptcl(
				randFloat(mParamAmpMin, mParamAmpMax),
				randFloat(mParamRadius-w, mParamRadius+w),
				randFloat(mParamWLMin, mParamWLMax),
				randFloat(mParamSpeedMin, mParamSpeedMax),
				randFloat(1.0f, 3.0f)
			)
		);
	}

	if (pShader) {
		mPointShader = gl::GlslProg::create(loadAsset("points.vert"), loadAsset("points.frag"));

		geom::BufferLayout attribs;
		attribs.append(geom::POSITION, 2, sizeof(ptcl), offsetof(ptcl, Pos), 0);
		mPointData = gl::Vbo::create(GL_ARRAY_BUFFER, mPtcls, GL_DYNAMIC_DRAW);
		mPointMesh = gl::VboMesh::create(mPtcls.size(), GL_POINTS, { {attribs, mPointData} });
	}
	else {
		mPointData->bufferData(mPtcls.size()*sizeof(ptcl), mPtcls.data(), GL_DYNAMIC_DRAW);
	}
}
void WhiteCircle::setupGUI()
{
	mParamResetTime = 180;
	mParamBrightnessMin = 0.1f; mParamBrightnessMax = 0.25f;
	mParamFadeAmtMin = 0.01f; mParamFadeAmtMax = 0.1f;
	mParamWidthMin = 5.0f; mParamWidthMax=10.0f;
	mParamRadius = 300.0f;
	mParamCountMin = 2000.0f; mParamCountMax=5000.0f;

	mParamAmpMin = 0.2f; mParamAmpMax=0.8f;
	mParamWLMin = 0.01f; mParamWLMax=0.05f;
	mParamSpeedMin = 0.15f; mParamSpeedMax=0.35f;
	mParamRotSpeedMin = 0.008f; mParamRotSpeedMax=0.02f;

	mGUI = params::InterfaceGl::create("Settings", { 200,400 });
	mGUI->addParam("paramResetTime", &mParamResetTime, "label='Reset Time'", false);
	mGUI->addParam("paramBrightnessMin", &mParamBrightnessMin, "label='Min Brightness'", false);
	mGUI->addParam("paramBrightnessMax", &mParamBrightnessMax, "label='Max Brightness'", false);
	mGUI->addParam("paramFadeAmtMin", &mParamFadeAmtMin, "label='Fade Min'", false);
	mGUI->addParam("paramFadeAmtMax", &mParamFadeAmtMax, "label='Fade Max'", false);
	mGUI->addParam("paramWidthMin", &mParamWidthMin, "label='Min Width'", false);
	mGUI->addParam("paramWidthMax", &mParamWidthMax, "label='Max Width'", false);
	mGUI->addParam("paramRadius", &mParamRadius, "label='Radius'", false);
	mGUI->addParam("paramCountMin", &mParamCountMin, "label='Min Points'", false);
	mGUI->addParam("paramCountMax", &mParamCountMax, "label='Max Points'", false);
	mGUI->addParam("paramAmpMin", &mParamAmpMin, "label='Min Amplitude'", false);
	mGUI->addParam("paramAmpMax", &mParamAmpMax, "label='Max Amplitude'", false);
	mGUI->addParam("paramWLMin", &mParamWLMin, "label='Min Wavelength'", false);
	mGUI->addParam("paramWLMax", &mParamWLMax, "label='Max Wavelength'", false);
	mGUI->addParam("paramSpeedMin", &mParamSpeedMin, "label='Min Speed'", false);
	mGUI->addParam("paramSpeedMax", &mParamSpeedMax, "label='Max Speed'", false);
	mGUI->addParam("paramRotSpeedMin", &mParamRotSpeedMin, "label='Min Spin Speed'", false);
	mGUI->addParam("paramRotSpeedMax", &mParamRotSpeedMax, "label='Max Spin Speed'", false);
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(800, 800);
	pSettings->setFrameRate(60);
}

CINDER_APP( WhiteCircle, RendererGl, prepareSettings )
