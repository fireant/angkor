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

#include <osg/GL>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/Billboard>
#include <osg/Geode>
#include <osg/Group>
#include <osg/ShapeDrawable>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osgGA/TrackballManipulator>
#include <osgSim/LightPointNode>
#include <pthread.h>

#include <iostream>
#include <cmath>

#include <libfreenect/libfreenect.h>

volatile int die = 0;
pthread_t freenect_thread;

freenect_context *f_ctx;
freenect_device *f_dev;

double depth_data[240][320];

pthread_mutex_t backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

int frame = 0;

using namespace std;
using namespace osgSim;

void addToLightPointNode ( LightPointNode& lpn, LightPoint& start,
						   unsigned int noSteps, unsigned int j )
{
        if ( noSteps<=1 ) {
                lpn.addLightPoint ( start );
                return;
        }

        float rend = 0.0f;
        float rdelta = 1.0f/ ( ( float ) noSteps-1.0f );

        lpn.getLightPointList().reserve ( noSteps );

        for ( unsigned int i=0;i<noSteps;++i,rend+=rdelta ) {
                float rstart = 1.0f-rend;
                LightPoint lp ( start );
                lp._position.z() = depth_data[j][i] * 10.0;
                lp._position.y() = j * ( lp._position.z() + -10.0 ) * 0.005;
                lp._position.x() = i * ( lp._position.z() + -10.0 ) * 0.005;

                if ( depth_data[j][i]> ( 100.0/3.33 ) )
                        lpn.addLightPoint ( lp );
        }
}

osg::Node* createLightPointsDatabase()
{
        LightPoint start;

        start._position.set ( -320.0f,-500.0f,0.0f );
        start._color.set ( 1.0f,1.0f,1.0f,1.0f );

        osg::MatrixTransform* transform = new osg::MatrixTransform;

        transform->setDataVariance ( osg::Object::STATIC );
        transform->setMatrix ( osg::Matrix::scale ( 0.1,0.1,0.1 ) );

        osg::Vec3 start_delta ( 0.0f,2.0f,0.0f );

        int noStepsX = 320;
        int noStepsY = 240;

        pthread_mutex_lock ( &backbuf_mutex );
        for ( int i=0;i<noStepsY;++i ) {

                LightPointNode* lpn = new LightPointNode;

                addToLightPointNode ( *lpn,start,noStepsX, i );

                start._position += start_delta;

                transform->addChild ( lpn );
        }
        pthread_mutex_unlock ( &backbuf_mutex );

        osg::Group* group = new osg::Group;
        group->addChild ( transform );

        return group;
}

void depth_cb ( freenect_device *dev, void *v_depth, uint32_t timestamp )
{
        freenect_depth *depth = ( freenect_depth * ) v_depth;
        pthread_mutex_lock ( &backbuf_mutex );
        for ( int i=0; i<240; i++ ) {
                for ( int j=0; j<320; j++ ) {
                        double pval = 100/ ( -0.00307 * depth[ ( ( i*2 ) *640 )
								+ ( j*2 ) ] + 3.33 );
                        depth_data[i][j] = pval;
                }
        }
        pthread_mutex_unlock ( &backbuf_mutex );
        frame++;
}

void *freenect_threadfunc ( void *arg )
{
        freenect_set_depth_callback ( f_dev, depth_cb );
        freenect_set_depth_format ( f_dev, FREENECT_FORMAT_11_BIT );

        freenect_start_depth ( f_dev );

        while ( !die ) {
                int16_t ax,ay,az;
                freenect_get_raw_accel ( f_dev, &ax, &ay, &az );
                double dx,dy,dz;
                freenect_get_mks_accel ( f_dev, &dx, &dy, &dz );
                cout<<"\r frame: "<<frame<<
						" - mks acc: "<<dx<<" "<<dy<<" "<<dz<<"\r";
        }

        cout<<"\nshutting down streams...\n";

        freenect_stop_depth ( f_dev );

        freenect_close_device ( f_dev );
        freenect_shutdown ( f_ctx );

        cout<<"-- done!\n";
        return NULL;
}


int main ( int argc, char **argv )
{
        for ( int i=0; i<240; i++ )
                for ( int j=0; j<320; j++ )
                        depth_data[i][j] = 0;

        if ( freenect_init ( &f_ctx, NULL ) < 0 ) {
                cout<<"freenect_init() failed\n";
                return 1;
        }

        freenect_set_log_level ( f_ctx, FREENECT_LOG_DEBUG );
        if ( freenect_open_device ( f_ctx, &f_dev, 0 ) < 0 ) {
                cout<<"Could not open device\n";
                return 1;
        }

        pthread_create ( &freenect_thread, NULL, freenect_threadfunc, NULL );

        osgViewer::Viewer viewer;
        viewer.setCameraManipulator ( new osgGA::TrackballManipulator() );
        viewer.setUpViewInWindow ( 640, 0, 640, 480 );
        viewer.realize();

        osg::Group* rootnode = new osg::Group;

        osg::Node* lps;
        lps = createLightPointsDatabase();
        rootnode->addChild ( lps );
        viewer.setSceneData ( rootnode );

        while ( !viewer.done() ) {
                rootnode->removeChild ( lps );
                lps = createLightPointsDatabase();
                rootnode->addChild ( lps );
                viewer.setSceneData ( rootnode );
                if ( frame == 1 )
                        viewer.assignSceneDataToCameras();
                viewer.frame();
        }
        die = 1;
        sleep ( 1 );

        return 0;
}
