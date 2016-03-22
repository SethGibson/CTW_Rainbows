#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct PLine {
  public:
	vector<vec2> LPoints;
	ColorA LColor;
	float LSpeed;
	float LWL;
	float LAmp;

  private:
	bool mIsCos;
	int mXInit;
	vector<vec2> mLineVerts;

  public:
	PLine()
	{
		mXInit = randFloat(0.0f, (float)getWindowWidth());
		LPoints.push_back({ mXInit, 0.0f});
		LColor = ColorA({ randFloat(0.1f,0.55f), randFloat(0.1f,0.55f), randFloat(0.11f,0.55f), randFloat(0.2f,0.4f) });
		LSpeed = randInt(2,6);
		LWL = randFloat(0.05f, 0.1f);
		LAmp = randFloat(3.0f, 7.0f);

		auto seed = randInt(1, 997);
		mIsCos = seed % randInt(1, 3) == 0;
	}

	void Step()
	{
		auto last = LPoints.back();
		auto y = (int)(last.y + LSpeed);

		if (y < getWindowHeight()) {
			auto newAmp = lmap<float>(y, 0.0f, (float)getWindowHeight(), LAmp, 0.0f);
			auto x = math<float>::sin(y*LWL);
			if (mIsCos) {
				x = math<float>::cos(y*LWL);
			}
			x = mXInit + (x*newAmp);
			LPoints.push_back({ x,y });
		}
		else {
			LPoints.clear();
		}
	}

	const vector<vec2>& GetLines()
	{
		mLineVerts.clear();
		if (LPoints.size() >= 2) {
			for (int i = 0; i <= LPoints.size() - 2; ++i) {
				mLineVerts.push_back(LPoints[i]);
				mLineVerts.push_back(LPoints[i+1]);
			}
		}
		return mLineVerts;
	}

	const vector<vec4> GetColors()
	{
		vector<vec4> colors;
		for (int c = 0; c < mLineVerts.size(); ++c) {
			auto a = lmap<float>(c, 0.0f, mLineVerts.size(), LColor.a, 0.0f);
			colors.push_back({LColor.r, LColor.g,LColor.b, a});
		}
		return colors;
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

	  gl::VboRef		mPointData, mColorData;
	  gl::VboMeshRef	mMesh;
	  gl::GlslProgRef	mShader;
};

LinesTest::LinesTest()
{
	for (int i = 0; i < 5; ++i) {
		mLines.push_back(PLine());
	}

	gl::GlslProg::Format fmt = gl::GlslProg::Format()
		.vertex(loadAsset("lines.vert"))
		.fragment(loadAsset("lines.frag"));
	mShader = gl::GlslProg::create(fmt);

	buildBatch(true);

	gl::disableAlphaBlending();
	gl::enableAdditiveBlending();
}
	

void LinesTest::mouseDown( MouseEvent event )
{
}

void LinesTest::update()
{
	if ((getElapsedFrames() % 60) == 0) {
		auto num = randInt(4, 10);
		for (int i = 0; i < num; ++i) {
			mLines.push_back(PLine());
		}
	}
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

	gl::ScopedGlslProg shdr(mShader);
	gl::lineWidth(3.0f);
	//gl::pointSize(5.0f);
	gl::draw(mMesh);

}

void LinesTest::buildBatch(bool pInit)
{
	vector<vec2> positions;
	vector<vec4> colors;
	for (auto &l : mLines) {
		auto lp = l.GetLines();
		positions.insert(positions.end(), lp.begin(), lp.end());

		auto lc = l.GetColors();
		colors.insert(colors.end(), lc.begin(), lc.end());
	}

	
	geom::BufferLayout linePoints;
	linePoints.append(geom::POSITION, 2, 0, 0, 0);
	
	geom::BufferLayout lineColors;
	lineColors.append(geom::COLOR, 4, 0, 0, 0);

	if (pInit) {
		mPointData = gl::Vbo::create(GL_ARRAY_BUFFER, positions, GL_DYNAMIC_DRAW);
		mColorData = gl::Vbo::create(GL_ARRAY_BUFFER, colors, GL_DYNAMIC_DRAW);
	}
	else {
		mPointData->bufferData(positions.size()*sizeof(vec2), positions.data(), GL_DYNAMIC_DRAW);
		mColorData->bufferData(colors.size()*sizeof(vec4), colors.data(), GL_DYNAMIC_DRAW);
	}
	mMesh = gl::VboMesh::create(positions.size(), GL_LINES, { {linePoints, mPointData},{lineColors,mColorData} });
}

void prepareSettings(App::Settings *pSettings)
{
	pSettings->setWindowSize({ 500, 1000 });
	pSettings->setFrameRate(60);
}

CINDER_APP( LinesTest, RendererGl, prepareSettings )
