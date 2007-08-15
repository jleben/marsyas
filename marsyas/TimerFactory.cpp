/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#include "TimerFactory.h"

using namespace std;
using namespace Marsyas;

/** Adding new Timers:
 New timers are added by first making them...
 Basically, a map is created from "TimerName"=>TimerConstructorObject.
 This makes it possible to use a map for fast access to specific timers and it
 prevents having to instantiate each Timer type. The constructor object simply
 wraps the new operator so that it constructs objects only when requested.
 1. Add the timer's include file.
 2. Wrap the object using the macro:
    TimerCreateWrapper(TmSomeTimerName);
 3. Register the timer in the map:
    in function TimerFactory::addTimers() add the line
    registerTimer(TmSomeTimerName);
*/

#define TimerCreateWrapper(_NAME) \
    struct Make##_NAME : public MakeTimer { \
        Make##_NAME() {}; ~Make##_NAME() {}; \
        TmTimer* make(std::string ident) { return new _NAME (ident); }; \
    }
#define registerTimer(_NAME) registry[#_NAME] = new Make##_NAME();

#include "TmGetTime.h"
#include "TmSampleCount.h"
//#include "TmKbdTap.h"
//#include "TmMIDITap.h"
//#include "TmPeakTap.h"

TimerCreateWrapper(TmGetTime);
TimerCreateWrapper(TmSampleCount);
//TimerCreateWrapper(TmKbdTap);
//TimerCreateWrapper(TmMIDITap);
//TimerCreateWrapper(TmPeakTap);

TimerFactory* TimerFactory::_instance = NULL;

TimerFactory::TimerFactory() {
    addTimers();
}
TimerFactory::~TimerFactory() { delete _instance; _instance=NULL; }

void TimerFactory::addTimers() {
    // register your timers here!
    registerTimer(TmGetTime);
    registerTimer(TmSampleCount);
//    registerTimer(TmKbdTap);
//    registerTimer(TmMIDITap);
//    registerTimer(TmPeakTap);
}
TimerFactory*
TimerFactory::getInstance() {
    if (_instance==NULL) { _instance=new TimerFactory(); }
    return _instance;
}

TmTimer*
TimerFactory::make(std::string class_name, std::string identifier) {
    MakeTimer* m = registry[class_name];
    if (m==NULL) { // does the map always return NULL when key is not in the map??
        MRSWARN("TimerFactory::make(string,string)  name '"+class_name+"' does not name a timer");
        return NULL;
    }
    return m->make(identifier);
}

