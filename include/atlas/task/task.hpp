#ifndef ATLAS_TASK_TASK_HPP
#define ATLAS_TASK_TASK_HPP

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace atlas
{
    /*!
     * \brief Background tasks for retrieving weather data and preparing
     * instructions for devices.
     */
    namespace task
    {
        class base;

        /*!
         * \brief Type to pass references to the io_service safely.
         */
        typedef boost::shared_ptr<boost::asio::io_service> io_service_type;
        typedef boost::function<void()> callback_function_type;
        typedef boost::function<
            boost::shared_ptr<base>(io_service_type, callback_function_type)>
            create_function_type;
    }
}

#endif

