#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class glLinesTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void glLinesTestApp::setup()
{
}

void glLinesTestApp::mouseDown( MouseEvent event )
{
}

void glLinesTestApp::update()
{
}

void glLinesTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( glLinesTestApp, RendererGl )
