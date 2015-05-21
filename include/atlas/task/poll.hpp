#ifndef ATLAS_TASK_POLL_HPP
#define ATLAS_TASK_POLL_HPP

#include "base.hpp"

namespace atlas
{
    namespace task
    {
        /*!
         * \brief Repeat a task at regular intervals.  The interval is
         * sepcified as the duration between the task finishing and starting
         * again, not the duration between the task being started.
         */
        class poll :
            public base,
            public boost::enable_shared_from_this<poll>
        {
            public:
                /*!
                 * \brief Create a poll task.
                 *
                 * \param create_task Function used to create a new task.
                 * Called every timeout_ms milliseconds.
                 * \param timeout_ms Interval between one task finishing and
                 * the next being created.
                 */
                static boost::shared_ptr<poll> create(
                    create_function_type create_task,
                    int timeout_ms,
                    io_service_type io_
                    );

                /*!
                 * \brief Delay for the timeout period and then start running
                 * the task repeatedly.
                 */
                void run() override;
                void schedule();
                void run_task();
            protected:
                poll(
                    create_function_type create_task,
                    int timeout_ms,
                    io_service_type io_,
                    boost::function<void()> callback
                    );
            private:
                create_function_type m_create_task;
                int m_timeout_ms;
                boost::asio::deadline_timer m_timer;
        };
    }
}

#endif

