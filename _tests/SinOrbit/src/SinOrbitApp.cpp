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
		//mTimer = randInt(-360, 360);
		mTimer = 0;
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
	void reset(bool pShader);

	bool  mStarted;
	float mAlpha;

	vector<ptcl>	mPtcls;
	vec4			mColor;

	gl::VboRef		mPointData;
	gl::VboMeshRef	mPointMesh;
	gl::GlslProgRef	mPointShader;
};

SinOrbitApp::SinOrbitApp() : mStarted(false)
{
	reset(true);
	gl::clear(Color(0, 0, 0));

	gl::enableAdditiveBlending();
}

void SinOrbitApp::mouseDown( MouseEvent event )
{
	if (mStarted)
		reset(false);

	mStarted = true;
}

void SinOrbitApp::update()
{
	if (mStarted) {
		if (getElapsedFrames() % 300 == 0) {
			reset(false);
		}
		for (auto &p : mPtcls) {
			p.step();
		}
		mPointData->bufferData(mPtcls.size()*sizeof(ptcl), mPtcls.data(), GL_DYNAMIC_DRAW);
	}
}

void SinOrbitApp::draw()
{
	if (mStarted) {
		//gl::clear(Color::black());
		gl::color(ColorA(0, 0, 0, mAlpha));
		gl::drawSolidRect(Rectf(vec2(), getWindowSize()));

		gl::color(Color::white());

		gl::pushModelMatrix();
		gl::translate(getWindowCenter());
		gl::rotate(toRadians(-90.0f));
		{
			gl::ScopedGlslProg shdr(mPointShader);
			mPointShader->uniform("u_Color", mColor);
			gl::ScopedState pts(GL_PROGRAM_POINT_SIZE, true);
			gl::draw(mPointMesh);
		}

		gl::popModelMatrix();
	}
}

void SinOrbitApp::reset(bool pShader)
{
	mAlpha = randFloat(0.01f, 0.1f);
	auto w = randFloat(5.0f,10.0f);
	auto maxR = randFloat(350.0f, 400.0f);
	//auto numPtcls = randInt(500, 1000);
	auto numPtcls = 1000;
	mPtcls.clear();

	for (int i = 0; i < numPtcls; ++i) {
		mPtcls.push_back(
			ptcl(
				randFloat(0.2f, 0.8f),
				randFloat(400.0f-w, 400.0f+w),
				randFloat(0.01f, 0.05f),
				randFloat(-0.5f, 0.5f),
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

	mColor = vec4(randVec3(), randFloat(0.001f, 0.025f));
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize(1000, 1000);
	pSettings->setFrameRate(60);
}

CINDER_APP( SinOrbitApp, RendererGl, prepareSettings )
