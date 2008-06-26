/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "TimelineLabeler.h"
#include <fstream>
#include <sstream>

using namespace std;
using namespace Marsyas;

TimelineLabeler::TimelineLabeler(string name):MarSystem("TimelineLabeler", name)
{
	addControls();
	labelFiles_ = ",";
	numClasses_ = 0;
	selectedLabel_ = "init";
	curRegion_ = 0;
}

TimelineLabeler::TimelineLabeler(const TimelineLabeler& a) : MarSystem(a)
{
	ctrl_labelFiles_ = getctrl("mrs_string/labelFiles");
	ctrl_currentLabelFile_ = getctrl("mrs_natural/currentLabelFile");
	ctrl_labelNames_ = getctrl("mrs_string/labelNames");
	ctrl_currentLabel_ = getctrl("mrs_natural/currentLabel");
	ctrl_nLabels_ = getctrl("mrs_natural/nLabels");
	ctrl_selectLabel_ = getctrl("mrs_string/selectLabel");
	ctrl_advance_ = getctrl("mrs_bool/advance");
	ctrl_pos_ = getctrl("mrs_natural/pos");
	ctrl_playRegionsOnly_ = getctrl("mrs_bool/playRegionsOnly");

	labelFiles_ = ",";
	numClasses_ = 0;
	selectedLabel_ = "init";
	curRegion_ = 0;
}

TimelineLabeler::~TimelineLabeler()
{
}

MarSystem*
TimelineLabeler::clone() const
{
	return new TimelineLabeler(*this);
}

void
TimelineLabeler::addControls()
{
	addctrl("mrs_string/labelFiles", ",", ctrl_labelFiles_);
	ctrl_labelFiles_->setState(true);

	addctrl("mrs_natural/currentLabelFile", 0, ctrl_currentLabelFile_);
	ctrl_currentLabelFile_->setState(true);

	addctrl("mrs_string/selectLabel", "", ctrl_selectLabel_);
	ctrl_selectLabel_->setState(true);

	addctrl("mrs_bool/advance", false, ctrl_advance_);
	addctrl("mrs_natural/pos", 0, ctrl_pos_);

	addctrl("mrs_bool/playRegionsOnly", true, ctrl_playRegionsOnly_);

	addctrl("mrs_string/labelNames", ",", ctrl_labelNames_);
	addctrl("mrs_natural/currentLabel", 0, ctrl_currentLabel_);
	addctrl("mrs_natural/nLabels", 0, ctrl_nLabels_);
}

void
TimelineLabeler::myUpdate(MarControlPtr sender)
{
	MRSDIAG("TimelineLabeler.cpp - TimelineLabeler:myUpdate");

	MarSystem::myUpdate(sender);

	///////////////////////////////////////////////////////////////
	//fill labelFilesVec with all timeline filenames
	///////////////////////////////////////////////////////////////
	mrs_string newLabelFiles = ctrl_labelFiles_->to<mrs_string>();
	if(labelFiles_ != newLabelFiles && (newLabelFiles != "" || newLabelFiles != ","))
	{
		labelFiles_ = newLabelFiles;
		mrs_natural i;
		labelFilesVec_.clear();
		while(newLabelFiles.length() != 0 )
		{
			i = newLabelFiles.find(",");
			labelFilesVec_.push_back(newLabelFiles.substr(0, i).c_str());
			newLabelFiles = newLabelFiles.substr(i+1 , newLabelFiles.length()-i-1);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////
	//load currentLabelFile into the internal timeline memory (if not already loaded)
	//////////////////////////////////////////////////////////////////////////////////
	mrs_bool newTimeline = false;
	mrs_natural curLabelFile = ctrl_currentLabelFile_->to<mrs_natural>();
	if(curLabelFile < (mrs_natural)labelFilesVec_.size()) //sanity check to avoid out of boundaries in vector
	{
		mrs_string fname = labelFilesVec_[curLabelFile];
		///////////////////////////////////////////////////////////////////////
		// check this is a different label file than the one currently loaded
		///////////////////////////////////////////////////////////////////////
		if(fname != timeline_.filename() && fname != "") 
		{
			//It is different - try to read the timeline into memory
			if(timeline_.load(fname))
			{
				newTimeline = true;
				//get the number of classes in the currently loaded timeline
				numClasses_ = (mrs_natural)timeline_.numClasses();
				ctrl_nLabels_->setValue(numClasses_, NOUPDATE);

				//get the labels of the classes in the currently loaded timeline
				ostringstream sstr;
				vector<mrs_string> classNames = timeline_.getRegionNames();
				for(mrs_natural i=0; i < numClasses_; ++i)
					sstr << classNames[i] << ",";
				ctrl_labelNames_->setValue(sstr.str(), NOUPDATE);

				curRegion_ = 0;
			}
			else //some problem occurred when reading the timeline file...
			{
				MRSWARN("TimelineLabeler::myUpdate() - error reading label file " << labelFilesVec_[ctrl_currentLabelFile_->to<mrs_natural>()]);
				numClasses_ = 0;
				ctrl_nLabels_->setValue(numClasses_, NOUPDATE);
				ctrl_labelNames_->setValue(",", NOUPDATE);
				timeline_.clear();
			}
		}
	}
	else //out of boundaries for labelFilesVec...
	{
		numClasses_ = 0;
		ctrl_nLabels_->setValue(numClasses_, NOUPDATE);
		ctrl_labelNames_->setValue(",", NOUPDATE);
		timeline_.clear();
	}

	/////////////////////////////////////////////////////////////////////////
	// Fast forward to first region start sample, if set to do so
	/////////////////////////////////////////////////////////////////////////
	if(timeline_.numRegions() > 0 &&
		 ctrl_playRegionsOnly_->to<mrs_bool>() &&
		 (ctrl_selectLabel_->to<mrs_string>() != selectedLabel_ || newTimeline))
	{
		selectedLabel_ = ctrl_selectLabel_->to<mrs_string>();

		//fast forward to the start of first region (which my not start at the 
		//beginning of the audio file) with a label corresponding to the one specified
		//by ctrl_selectLabel_
		curRegion_ = 0;
		if(selectedLabel_ == "") //i.e. any label is accepted
		{
			//fast forward to first region in timeline
			ctrl_pos_->setValue(timeline_.regionStart(0)*timeline_.lineSize(), NOUPDATE);
			curRegion_ = 0;
		}
		else //look for the first region with the selected label...
		{			
			for(mrs_natural i=0; i < timeline_.numRegions(); ++i)
			{
				if(timeline_.regionName(i) == selectedLabel_)
				{
					//fast forward to found region
					ctrl_pos_->setValue(timeline_.regionStart(i), NOUPDATE);
					curRegion_ = i;
					break;
				}
			}
		}
	}
}

void
TimelineLabeler::myProcess(realvec& in, realvec& out)
{
	//bypass audio input to output 
	out = in;

 	if(timeline_.numRegions() == 0)
 	{
		MRSWARN("TimelineLabeler::myProcess() - no regions/labels exist in loaded timeline: " << timeline_.filename());
		ctrl_currentLabel_->setValue(0); //no labels defined...
 		return;
 	}

	//get the sample position in the audio file of the last sample in
	//current frame (as set by SoundFileSource)
	samplePos_ = ctrl_pos_->to<mrs_natural>();
	
	//get current region boundaries
	mrs_natural regionStart = timeline_.regionStart(curRegion_)*timeline_.lineSize(); //region start sample
	mrs_natural regionEnd = timeline_.regionEnd(curRegion_)*timeline_.lineSize(); //region end sample

	////////////////////////////////////////////////////////////////////////////////////
	//check if this audio frame belongs to current region or to the next one
	//(i.e. if at least half of the current audio frame belongs to the current region)
	////////////////////////////////////////////////////////////////////////////////////
	if(samplePos_ == 0)
		samplePos_ += inSamples_/2; //UGLY HACK because of the way SoundFileSource and CollectionFileSource are implemented...
	else
		samplePos_ -= inSamples_/2;	
	
	if (samplePos_ >= regionStart && samplePos_<= regionEnd)
		ctrl_currentLabel_->setValue(timeline_.regionClass(curRegion_));
	else//move on to following region, if any...
	{
		////////////////////////////////////////////////////////
		//look for the next region in this timeline
		////////////////////////////////////////////////////////
		if(selectedLabel_ == "" || selectedLabel_ == "init")//i.e. just move to next region, whatever label
			curRegion_++;
		else // look for and move to next region with a specific label 
			while(timeline_.regionName(curRegion_)!= selectedLabel_ && curRegion_ < timeline_.numRegions())
				curRegion_++;

		///////////////////////////////////////////////////////////////////
		//check if we found a new/subsequent region in the current timeline...
		///////////////////////////////////////////////////////////////////
		if(curRegion_ < timeline_.numRegions())
		{
			//yeap, we found a new region in the current timeline!
			//update region boundaries
			regionStart = timeline_.regionStart(curRegion_)*timeline_.lineSize(); //region start sample
			regionEnd = timeline_.regionEnd(curRegion_)*timeline_.lineSize(); //region end sample
			
			//check if current frame is inside the new current region
			if(samplePos_ >= regionStart && samplePos_ <= regionEnd)
			{
				//yes it is... just output its label
				ctrl_currentLabel_->setValue(timeline_.regionClass(curRegion_));
			}
			else //current frame not inside the new current window...
			{
				//...should we fast forward to the new region (at next tick)?
				if(ctrl_playRegionsOnly_->to<mrs_bool>())
				{
					//output silence (i.e. discard current frame, since most of it does not
					//belong to the now past region, neither to the now current, region...)
					//out.setval(0.0); //[?] should we play this frame as is or just output silence?!

					//fast forward to next region (at next tick)
					ctrl_pos_->setValue(regionStart);
				}
				//i.e. outside a region: signal that no label is defined for this audio frame
				ctrl_currentLabel_->setValue(0); 
			}
		}
		else //no more new regions in this timeline...
		{
			//Should we ask for another audiofile/timeline ot just play the current file till its end?
			if(ctrl_playRegionsOnly_->to<mrs_bool>())
			{
				//output silence (i.e. discard current frame, since most of it does not
				//belong to the now past region)
				//out.setval(0.0); //[?] should we play this frame as is or just output silence?!

				//fast forward to next region (at next tick)
				ctrl_advance_->setValue(true);
			}
			ctrl_currentLabel_->setValue(0); //i.e. no region/label defined for this audio frame
		}
	}
}


