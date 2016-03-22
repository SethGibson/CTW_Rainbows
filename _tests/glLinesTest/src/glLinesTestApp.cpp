#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct PLine {
  public:
	vector<vec2> LPoints;
	Color LColor;
	float LSpeed;
	float LWL;
	float LAmp;

  private:
	vector<vec2> mLineVerts;

  public:
	PLine()
	{
		LPoints.push_back({ 0.0f,randFloat(0.0f, (float)getWindowHeight())});
		LColor = Color({ randFloat(), randFloat(), randFloat() });
		LSpeed = randFloat(1.0f, 2.0f);
		LWL = randFloat(0.05f, 0.1f);
		LAmp = randFloat(3.0f, 7.0f);
	}

	void Step()
	{
		auto last = LPoints.back();
		auto x = last.x+LSpeed;
		auto newAmp = lmap<float>(x, 0.0f, (float)getWindowWidth(), LAmp, 0.0f);

		auto y = LPoints[0].y+(math<float>::sin(x*LWL)*newAmp);

		LPoints.push_back({x,y});
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
			auto a = lmap<float>(c, 0.0f, mLineVerts.size(), 1.0f, 0.0f);
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
	for (int i = 0; i < 50; ++i) {
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

	gl::ScopedGlslProg shdr(mShader);
	gl::lineWidth(2.0f);
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
	pSettings->setWindowSize({ 1500, 500 });
	pSettings->setFrameRate(60);
}

CINDER_APP( LinesTest, RendererGl, prepareSettings )
