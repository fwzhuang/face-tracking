#pragma once

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>

/*
 * The main class for tracking face landmarks with a web cam support
 */
class Tracker
{
public:
	void start() const;
};