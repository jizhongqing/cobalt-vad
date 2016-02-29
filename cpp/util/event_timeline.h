/*
 * event_timeline.h
 *
 *  Created on: Jul 22, 2015
 *      Author: kevinyang
 */

#ifndef COBALT_GUARD_EVENT_TIMELINE_H_
#define COBALT_GUARD_EVENT_TIMELINE_H_

#include "util/timeline_event.h"
#include "util/timeline_event_visitor.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace cobalt
{
	class EventTimeline
	{
	public:
		typedef boost::shared_ptr<EventTimeline> Ptr;

		void visitEvents(TimelineEventVisitor* visitor)
		{
			LockGuard_t lock(mMutex);
			for (size_t i = 0; i < mEvents.size(); ++i)
			{
				mEvents[i]->accept(visitor);
			}
		}

		// visit events starting from an index.
        void visitNewEvents(TimelineEventVisitor* visitor)
        {
            LockGuard_t lock(mMutex);
            for (size_t i = visitor->getEventsVisited(); i < mEvents.size(); ++i)
            {
                mEvents[i]->accept(visitor);
            }
        }

		void addEvent(TimelineEvent::Ptr event)
		{
			LockGuard_t lock(mMutex);
			mEvents.push_back(event);
		}

	private:
		typedef boost::lock_guard<boost::mutex> LockGuard_t;
		boost::mutex mMutex;
		std::vector<TimelineEvent::Ptr> mEvents;
	};

}

#endif /* COBALT_GUARD_EVENT_TIMELINE_H_ */
