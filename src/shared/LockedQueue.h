/*
 * This file is part of the CMaNGOS Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOCKEDQUEUE_H
#define LOCKEDQUEUE_H

#include <mutex>
#include <deque>
#include <assert.h>
#include "Errors.h"

#define MANGOS_LOCK_ACTION(OBJ, LOCK, ACTION)	\
	if (!LOCK.try_lock()) { ACTION; }			\
    LOCK.unlock();								\
	std::lock_guard<decltype(LOCK)> OBJ(LOCK);

#define MANGOS_LOCK_RETURN(OBJ, LOCK, RETURN)	MANGOS_LOCK_ACTION(OBJ, LOCK, return RETURN)

namespace MaNGOS
{
    template <class T, class LockType, typename StorageType = std::deque<T> >
    class LockedQueue
    {
            //! Lock access to the queue.
            LockType _lock;

            //! Storage backing the queue.
            StorageType _queue;

            //! Cancellation flag.
            /*volatile*/ bool _canceled;

        public:

            //! Create a LockedQueue.
            LockedQueue()
                : _canceled(false)
            {
            }

            //! Destroy a LockedQueue.
            virtual ~LockedQueue()
            {
            }

            //! Adds an item to the queue.
            void add(const T& item)
            {
                std::lock_guard<LockType> g(this->_lock);
                _queue.push_back(item);
            }

            //! Gets the next result in the queue, if any.
            bool next(T& result)
            {
                MANGOS_LOCK_RETURN(g, this->_lock, false);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                _queue.pop_front();

                return true;
            }

            template<class Checker>
            bool next(T& result, Checker& check)
            {
                MANGOS_LOCK_RETURN(g, this->_lock, false);

                if (_queue.empty())
                    return false;

                result = _queue.front();
                if (!check.Process(result))
                    return false;

                _queue.pop_front();
                return true;
            }

            //! Peeks at the top of the queue. Remember to unlock after use.
            T& peek()
            {
                lock();

                T& result = _queue.front();

                return result;
            }

            //! Cancels the queue.
            void cancel()
            {
                std::lock_guard<LockType> g(this->_lock);
                _canceled = true;
            }

            //! Checks if the queue is cancelled.
            bool cancelled()
            {
                std::lock_guard<LockType> g(this->_lock);
                return _canceled;
            }

            //! Locks the queue for access.
            void lock()
            {
                this->_lock.acquire();
            }

            //! Unlocks the queue.
            void unlock()
            {
                this->_lock.release();
            }

            ///! Checks if we're empty or not with locks held
            bool empty()
            {
                std::lock_guard<LockType> g(this->_lock);
                return _queue.empty();
            }
    };
}
#endif
