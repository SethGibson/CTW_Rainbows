#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct PLine
{
	vector<vec3> LPoints;
	ColorA LColor;
	int Stepper;
	PLine() : Stepper(0)
	{
		LPoints.push_back({Stepper,randInt(0, getWindowHeight()), 0});
	}

	void Step()
	{
		//auto x = LPoints[0].x+getElapsedFrames();
		//auto y = math<float>::sin(x*0.01f)*10.0f;
		Stepper += 1;
		LPoints.push_back({Stepper,LPoints[0].y,0});
	}

	const vector<vec3> GetLines()
	{
		vector<vec3> lines;
		if (LPoints.size() >= 2) {
			for (int i = 0; i <= LPoints.size() - 2; ++i) {
				lines.push_back(LPoints[i]);
				lines.push_back(LPoints[i+1]);
			}
		}
		return lines;
	}
};

class LinesTest : public App {
  public:
	LinesTest();
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

  private:
	  void buildBatch(bool pInit);

	  vector<PLine> mLines;
	  vector<vec3>	mAllLines;

	  gl::BatchRef		mBatch;
	  gl::VboRef		mData;
	  gl::VboMeshRef	mMesh;
	  gl::GlslProgRef	mShader;
};

LinesTest::LinesTest()
{
	for (int i = 0; i < 1; ++i) {
		mLines.push_back(PLine());
	}

	gl::GlslProg::Format fmt = gl::GlslProg::Format()
		.vertex(loadAsset("lines.vert"))
		.fragment(loadAsset("lines.frag"));
	mShader = gl::GlslProg::create(fmt);

	buildBatch(true);
}
	

void LinesTest::mouseDown( MouseEvent event )
{
}

void LinesTest::update()
{
	for (auto &l : mLines) {
		l.Step();
	}

	buildBatch(false);
}

void LinesTest::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatricesWindow(getWindowSize());

	gl::color(Color::white());
	mBatch->draw();
}

void LinesTest::buildBatch(bool pInit)
{
	mAllLines.clear();
	for (auto &l : mLines) {
		for (auto &p : l.LPoints) {
			mAllLines.push_back(p);
		}
		//auto lp = l.GetLines();
		//mAllLines.insert(mAllLines.end(), lp.begin(), lp.end());
	}

	if (pInit) {
		mData = gl::Vbo::create(GL_ARRAY_BUFFER, mAllLines, GL_DYNAMIC_DRAW);
		geom::BufferLayout lineAttrs;
		lineAttrs.append(geom::POSITION, 3, 0, 0, 0);

		mMesh = gl::VboMesh::create(mAllLines.size(), GL_POINTS, { { lineAttrs, mData } });
		mBatch = gl::Batch::create(mMesh, mShader);
	}
	else {
		mData->bufferData(mAllLines.size()*sizeof(vec3), mAllLines.data(), GL_DYNAMIC_DRAW);
	}
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize({ 1000,500 });
	pSettings->setFrameRate(60);
}

CINDER_APP( LinesTest, RendererGl, prepareSettings )
