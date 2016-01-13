/* BZWorkbench
 * Copyright (c) 1993 - 2010 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "windows/View.h"
#include "dialogs/MenuBar.h"
#include "objects/waterLevel.h"

// TODO does this do anything?
const double View::DEFAULT_ZOOM = 75.0;

// view constructor
View::View(Model* m, MainWindow* _mw, 
        int _x, int _y, int _w, int _h, const char *_label) : RenderWindow(_x,_y,_w,_h) {

    this->model = m;

    // set OSG viewport
    this->getCamera()->setViewport(new osg::Viewport(0,0,_w,_h));
//    this->getCamera()->setProjectionMatrixAsPerspective(
//            30.0f, static_cast<double>(_w)/static_cast<double>(_h), 1.0f, 10000.0f);
    this->getCamera()->setGraphicsContext(getGraphicsWindow());
    this->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    this->root  = new osg::Group();

    // configure the stateset of the root node
    osg::StateSet* stateSet = root->getOrCreateStateSet();
    // disable OSG's shading by making full white ambient light
    osg::LightModel* lighting = new osg::LightModel();
    lighting->setAmbientIntensity(osg::Vec4( 1, 1, 1, 1 ));
    stateSet->setAttribute(lighting, osg::StateAttribute::OVERRIDE);
    stateSet->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

    // initialize the ground
    this->ground = new Ground(400.0f);
    // add the ground to the root node
    this->root->addChild( ground );

    // build the scene from the model
    Model::objRefList objects = model->getObjects();
    if(objects.size() > 0) {
        for(Model::objRefList::iterator _i = objects.begin(); 
            _i != objects.end(); _i++) {
            root->addChild( _i->get() );
        }
    }

    // make a new selection object
    this->selection = new Selection( this );
    // add the selection
    // NOTE: this has to be the LAST child on the list, 
    // because it doesn't have Z-bufferring enabled!
    this->root->addChild( selection );

    // add the root node to the scene
    this->setSceneData(root);

    // give the View a trackball manipulator
    osgGA::TrackballManipulator* cameraManipulator = new osgGA::TrackballManipulator();
    this->cameraManipulatorRef = cameraManipulator;
    this->setCameraManipulator(cameraManipulator);

    this->eventHandlers = new EventHandlerCollection(this);
    // add the selectHandler
    selHandler = new selectHandler(this, cameraManipulator);
    this->eventHandlers->addEventHandler(
            selectHandler::getName().c_str(), selHandler );
    // add the scene picker event handler
    this->addEventHandler(eventHandlers);

    // assign the parent reference
    this->mw = _mw;

    // set the key modifiers to false
    this->modifiers = map< int, bool >();
    this->modifiers[ FL_SHIFT ] = false;
    this->modifiers[ FL_CTRL ] = false;
    this->modifiers[ FL_ALT ] = false;
    this->modifiers[ FL_META ] = false;

    // initialize snap to grid variables
    snappingEnabled = true;
    scaleSnapSize = 1;
    translateSnapSize = 1;
    rotateSnapSize = 15;
}


// build the mouse button map
// FS TODO Is this still needed??
void View::buildMouseButtonMap() {
    mouseButtonMap = map< unsigned int, unsigned int > ();
    // default mapppings
    // middle mouse drags in FLTK should translate to left mouse drags in OSG
    mouseButtonMap[ FL_MIDDLE_MOUSE ] = FL_LEFT_MOUSE;
}

// destructor
View::~View() {
    if(eventHandlers) {
        delete eventHandlers;
    }
}

// draw method (really simple)
void View::draw(void) {
    //printf("View::draw()\n");
    frame();
}

// scale the selection based on the distance from the camera to the center to ensure it stays the same size
void View::updateSelection(float newDistance) {
    this->selection->setScale( 
            osg::Vec3(0.01 * newDistance, 0.01 * newDistance, 0.01 * newDistance));
}

void View::updateSelection() {
    // get the distance from the eyepoint to the center of the trackball
    float dist = this->cameraManipulatorRef->getDistance();
    this->updateSelection(dist);
}

// handle events
int View::handle(int event) {
    int result = 1;
    int shiftState = Fl::event_state();
    int event_clicks = Fl::event_clicks();
    // whatever the event was, we need to regenerate the modifier keys
    modifiers[ FL_SHIFT ] = (( shiftState & FL_SHIFT ) != 0);
    modifiers[ FL_CTRL  ] = (( shiftState & FL_CTRL )  != 0);
    modifiers[ FL_META  ] = (( shiftState & FL_META )  != 0);
    modifiers[ FL_ALT   ] = (( shiftState & FL_ALT )   != 0);

    e_key = Fl::event_key();

    selection->setStateByKey( e_key );

    result = RenderWindow::handle(event);

    return result;
}

// update method (inherited from Observer)
void View::update( Observable* obs, void* data ) {
    // printf("View::update %s\n", selection->getName().c_str());
    // refresh the selection
    selection->update(obs, data);
    // process data
    if( data != NULL ) {
        // get the message
        ObserverMessage* obs_msg = (ObserverMessage*)(data);

        // process the message
        switch( obs_msg->type ) {
            // add an object to the scene
            case ObserverMessage::ADD_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);

                    if( getRootNode()->getNumChildren() > 0 )
                        // insert the child directly after the Ground object
                        getRootNode()->insertChild( 1, obj );
                    else
                        getRootNode()->addChild( obj );

                    break;
                }
           // remove an object from the scene
            case ObserverMessage::REMOVE_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);
                    getRootNode()->removeChild( obj );

                    break;
                }
            // update the world size
            case ObserverMessage::UPDATE_WORLD : 
                {
                    // in this case, the data will contain a pointer to 
                    // the modified world object
                    world* bzworld = (world*)(obs_msg->data);

                    root->removeChild( ground );
                    ground = new Ground(bzworld->getSize(), 
                            model->getWaterLevelData()->getHeight() );
                    root->insertChild(0, ground);

                    break;
                }
            // update an object (i.e. it's selection value changed, etc.)
            case ObserverMessage::UPDATE_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);
                    if( obj->isSelected() )
                        SceneBuilder::markSelectedAndPreserveStateSet( obj );
                    else
                        SceneBuilder::markUnselectedAndRestoreStateSet( obj );
                    break;
                }
            default:
                break;
        }
    }
}

// is a button pressed?
bool View::isPressed(int value) {
    return modifiers[value];
}

bool View::isSelected( bz2object* obj ) { 
    return this->model->isSelected( obj ); 
}

/**
 * Tell the model to select an object
 */
void View::setSelected(bz2object* object) {
    this->model->_setSelected(object);
}

/**
 * Tell the model to unselect an object
 */
void View::setUnselected(bz2object* object) {
    this->model->_setUnselected(object);
}


/**
 * Tell the model to unselect all selected objects
 */
void View::unselectAll() {
    this->model->_unselectAll();
}

