#ifndef ATLAS_TASK_BASE_HPP
#define ATLAS_TASK_BASE_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "task.hpp"

namespace hades
{
    class connection;
}

namespace atlas
{
    namespace task
    {
        class base
        {
            public:
                virtual void run() = 0;
                /*!
                 * \brief Report that the task had finished.
                 */
                void mark_finished();

            protected:
                /*!
                 * \param callback Function called when the task has finished
                 * executing.  Will only be called once.
                 */
                base(
                        io_service_type io_,
                        callback_function_type callback
                        ) :
                    m_io(io_),
                    m_callback(callback),
                    m_finished(false)
                {
                }

                boost::asio::io_service& io()
                {
                    return *m_io;
                }
                boost::shared_ptr<boost::asio::io_service> io_ptr()
                {
                    return m_io;
                }
            private:
                boost::shared_ptr<boost::asio::io_service> m_io;
                callback_function_type m_callback;
                bool m_finished;
        };
        class has_connection
        {
        protected:
            has_connection(hades::connection& conn) :
                m_connection(conn)
            {
            }
            hades::connection& connection() const
            {
                return m_connection;
            }
        private:
            hades::connection& m_connection;
        };
    }
}

#endif

