#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static size_t S_NUM_PTCL = 800000;

struct ptcl
{
	float	Speed;
	vec2	AngleRadii;	//x=Angle, y=Radius
	vec2	Pos;
	vec2	UV;

	ptcl() {}
	ptcl(float pSpeed, vec2 pAR, vec2 pUV) : Speed(pSpeed), AngleRadii(pAR), UV(pUV){}
};

class CTW_Rainbows : public App
{
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	void setupGUI();

	vec2			mRangeInner, mRangeOuter;

	int				mPong;
	vector<ptcl>	mPoints;

	gl::GlslProgRef		mShaderTF,
						mShaderRender;
	gl::Texture2dRef	mTexRainbow, mTexBG;

	// Transform Feedback
	gl::VaoRef					mAttribBuffers[2];
	gl::VboRef					mDataBuffers[2];
	gl::TransformFeedbackObjRef	mTFBuffers[2];

	// Params
	params::InterfaceGlRef	mGUI;

	float	mParamGRotation,
			mParamSinMod,
			mParamCosMod,
			mParamSpeedMod;
};

void CTW_Rainbows::setup()
{
	setupGUI();

	setWindowSize(1280, 720);
	mPong = 1;
	mRangeInner = vec2(getWindowHeight()*0.3f, getWindowHeight()*0.5f);
	mRangeOuter = vec2(getWindowHeight()*0.7f, getWindowHeight()*0.8f);

	mTexRainbow = gl::Texture2d::create(loadImage(loadAsset("TX_Gradient.png")));
	mTexBG = gl::Texture2d::create(loadImage(loadAsset("TX_BG.png")));
	gl::GlslProg::Format rdr;
	rdr.vertex(loadAsset("render.vert"))
		.fragment(loadAsset("render.frag"))
		.attribLocation("o_Pos", 2)
		.attribLocation("o_UV", 3);
	mShaderRender = gl::GlslProg::create(rdr);
	mShaderRender->uniform("u_SamplerRGB", 0);

	gl::GlslProg::Format tf;
	vector<string> tfVars
	{
		"o_Speed",
		"o_Polars",
		"o_Pos",
		"o_UV"
	};

	tf.vertex(loadAsset("tf.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings(tfVars)
		.attribLocation("i_Speed", 0)
		.attribLocation("i_Polars", 1)
		.attribLocation("i_UV", 3);

	mShaderTF = gl::GlslProg::create(tf);

	for (int i = 0; i < S_NUM_PTCL; ++i)
	{
		float speed = randFloat(-0.005f, 0.005f);
		float angle = randFloat(0.0, 2.0f*M_PI);

		vec2 spread = i < S_NUM_PTCL / 2 ? mRangeInner : mRangeOuter;
		float rad_0 = randFloat(spread.x, spread.y);
		auto u = lmap<float>(rad_0, spread.x, spread.y, 0.0f, 1.0f);

		mPoints.push_back(ptcl(speed, vec2(angle, rad_0), vec2(u,0.0f)));
	}

	//Buffers
	mDataBuffers[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints, GL_STATIC_DRAW);
	mDataBuffers[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mPoints.size()*sizeof(ptcl), nullptr, GL_STATIC_DRAW);

	for (int i = 0; i < 2; ++i)
	{
		mAttribBuffers[i] = gl::Vao::create();
		mAttribBuffers[i]->bind();
		mDataBuffers[i]->bind();

		gl::vertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Speed));
		gl::enableVertexAttribArray(0);
		gl::vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, AngleRadii));
		gl::enableVertexAttribArray(1);
		gl::vertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, Pos));
		gl::enableVertexAttribArray(2);
		gl::vertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(ptcl), (const GLvoid *)offsetof(ptcl, UV));
		gl::enableVertexAttribArray(3);

		mTFBuffers[i] = gl::TransformFeedbackObj::create();
		mTFBuffers[i]->bind();
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mDataBuffers[i]);
		mTFBuffers[i]->unbind();
	}
}

void CTW_Rainbows::mouseDown( MouseEvent event )
{
}

void CTW_Rainbows::update()
{
	{
		mPong = 1 - mPong;
		{
			gl::ScopedGlslProg tf(mShaderTF);
			mShaderTF->uniform("u_Elapsed", (float)getElapsedFrames());
			mShaderTF->uniform("u_SinMod", mParamSinMod);
			mShaderTF->uniform("u_CosMod", mParamCosMod);
			mShaderTF->uniform("u_SpeedMod", mParamSpeedMod);
			gl::ScopedVao	attribs(mAttribBuffers[mPong]);
			gl::ScopedState rDiscard(GL_RASTERIZER_DISCARD, GL_TRUE);

			mTFBuffers[1 - mPong]->bind();
			mTFBuffers[1 - mPong]->begin(GL_POINTS);
			gl::drawArrays(GL_POINTS, 0, S_NUM_PTCL);
			mTFBuffers[1 - mPong]->end();
			mTFBuffers[1 - mPong]->unbind();
		}
	}
}

void CTW_Rainbows::draw()
{
	gl::enableAdditiveBlending();
	gl::setMatricesWindow(getWindowSize());
	gl::clear(Color::black());
	//gl::color(Color(0.75f, 0.75f, 0.75f));
	gl::color(Color::white());

	gl::draw(mTexBG, Rectf(vec2(), getWindowSize()));
	gl::pushModelMatrix();
	gl::translate(getWindowCenter());
	gl::rotate((float)getElapsedFrames()*mParamGRotation);
	{
		gl::ScopedVao vao(mAttribBuffers[1-mPong]);
		gl::ScopedGlslProg rdr(mShaderRender);
		gl::ScopedTextureBind tx(mTexRainbow, 0);
		gl::ScopedState ps(GL_PROGRAM_POINT_SIZE, GL_TRUE);

		gl::setDefaultShaderVars();
		gl::drawArrays(GL_POINTS, 0, S_NUM_PTCL);
	}

	gl::popModelMatrix();

	mGUI->draw();
}

void CTW_Rainbows::setupGUI()
{
	mGUI = params::InterfaceGl::create("Params", { 200,300 });

	mParamGRotation = 0.005f;
	mParamSinMod = 0.005f;
	mParamCosMod = 0.01f;
	mParamSpeedMod = 500.0f;

	mGUI->addParam("paramGRotation", &mParamGRotation, "label='Global Spin'", false);
	mGUI->addParam("paramSinMod", &mParamSinMod, "label='Sin Mod'", false);
	mGUI->addParam("paramCosMod", &mParamCosMod, "label='Cos Mod'", false);
	mGUI->addParam("paramSpeedMod", &mParamSpeedMod, "label='Speed Mod'", false);
}

CINDER_APP(CTW_Rainbows, RendererGl )
