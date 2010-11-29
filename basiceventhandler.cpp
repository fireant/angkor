/* Copyright (c) 2010 Mosalam Ebrahimi <m.ebrahimi@ieee.org>
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include "basiceventhandler.h"

#include <iostream>

bool BasicEventHandler::handle ( const osgGA::GUIEventAdapter& ea,
                                 osgGA::GUIActionAdapter& )
{
        switch ( ea.getEventType() ) {
        case ( osgGA::GUIEventAdapter::KEYDOWN ) : {
                switch ( ea.getKey() ) {
                case 's':
                        export_state->state = true;
                        return false;
                        break;
                default:
                        return false;
                }
        }
        default:
                return false;
        }
}

void TW_CALL SavePLY(void* clientData) { ((ExportState *)clientData)->state = true;}


TweakBarEventCallback::TweakBarEventCallback ( ExportState* s ) : export_state( s )
{
        TwInit ( TW_OPENGL, NULL );

        TwWindowSize ( 640, 480 );

        toolsBar = TwNewBar ( "Tools" );
		TwDefine(" Tools size='140 100' ");
		TwDefine(" Tools movable=true ");
		TwDefine(" Tools resizable=true ");
		TwAddButton(toolsBar, "Save PLY", SavePLY, s, NULL);

}


bool TweakBarEventCallback::handle ( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv )
{
        float x = ea.getX();
        float y = ea.getY();
        y = ea.getWindowHeight() - y;
        bool handled = false;
        switch ( ea.getEventType() ) {
		case(osgGA::GUIEventAdapter::DRAG):
        case ( osgGA::GUIEventAdapter::MOVE ) : {
                handled = TwMouseMotion ( x, y );
                break;
        }        
        case (osgGA::GUIEventAdapter::PUSH): {
				if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
						handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
				else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) 
						handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_MIDDLE);
				else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
						handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_RIGHT);
				break;
		}
		case(osgGA::GUIEventAdapter::RELEASE): {
				if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
						handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
				else if (ea.getButton() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON) 
						handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_MIDDLE);
				else if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
						handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_RIGHT);
				break;
		}
		case(osgGA::GUIEventAdapter::RESIZE): {
				handled = TwWindowSize(ea.getWindowWidth(), ea.getWindowHeight());
				break;
		}
        default:
                break;
        }

        return handled;
}

TweakBarDrawable::TweakBarDrawable()
{		
		setUseDisplayList(false);		
}


TweakBarDrawable::~TweakBarDrawable()
{
        TwTerminate();
}

void TweakBarDrawable::drawImplementation ( osg::RenderInfo& renderInfo ) const
{
		TwDraw();
}

osg::Camera* TweakBarDrawable::createHUD ( int width, int height )
{
        // create a camera to set up the projection and model view matrices, and the subgraph to draw in the HUD
        osg::Camera* camera = new osg::Camera;

        // set the projection matrix
        camera->setProjectionMatrix ( osg::Matrix::ortho2D ( 0,width,0,height ) );

        // set the view matrix
        camera->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
        camera->setViewMatrix ( osg::Matrix::identity() );

        // only clear the depth buffer
        camera->setClearMask ( GL_DEPTH_BUFFER_BIT );

        // draw subgraph after main camera view.
        // The bin order is set to an arbitrarily large number to ensure that
        // there is no conflict with other post rendering activity (e.g., osgEarth)
        camera->setRenderOrder ( osg::Camera::POST_RENDER, 10000 );

        // we don't want the camera to grab event focus from the viewers main camera(s).
        camera->setAllowEventFocus ( false );

        return camera;

}
