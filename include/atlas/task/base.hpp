#ifndef ATLAS_TASK_BASE_HPP
#define ATLAS_TASK_BASE_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "atlas/task/task.hpp"

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

            protected:
                /*!
                 * \param callback Function called when the task has finished
                 * executing.  Will only be called once.
                 *
                 * \note As tasks will usually complete asynchronously, they
                 * will need to inherit from boost::shared_ptr and
                 * boost::enable_shared_from_this and use shared_from_this() in
                 * callbacks.  Tasks should be constructed by a 'create'
                 * function returning a boost::shared_ptr<derived>.
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

                /*!
                 * \brief Report that the task had finished.
                 */
                void mark_finished();

                /*!
                 * \brief Get a reference to the IO service that callbacks
                 * should be posted to.
                 */
                boost::asio::io_service& io()
                {
                    return *m_io;
                }
                /*!
                 * \brief Get a pointer to the IO service that callbacks should
                 * be posted to (useful if this task should start another
                 * task).
                 */
                boost::shared_ptr<boost::asio::io_service> io_ptr()
                {
                    return m_io;
                }
            private:
                boost::shared_ptr<boost::asio::io_service> m_io;
                callback_function_type m_callback;
                bool m_finished;
        };

        /*!
         * \brief Helpful mixin class for tasks which have a database
         * connection.
         *
         * \note Keeps a reference to the database connection.  The reference
         * is guaranteed to be valid if the lifetime of the connection is
         * longer than that of the task server.
         */
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

