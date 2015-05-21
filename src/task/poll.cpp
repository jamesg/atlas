#include "atlas/task/poll.hpp"

#include <boost/bind.hpp>

boost::shared_ptr<atlas::task::poll> atlas::task::poll::create(
    create_function_type create_task,
    int timeout_ms,
    io_service_type io_
    )
{
    poll *out = new poll(
            create_task,
            timeout_ms,
            io_,
            [](){}
            );
    return boost::shared_ptr<poll>(out);
}

void atlas::task::poll::run()
{
    schedule();
}
void atlas::task::poll::schedule()
{
    m_timer.expires_from_now(
            boost::posix_time::milliseconds(m_timeout_ms)
            );
    m_timer.async_wait(
            boost::bind(&poll::run_task, shared_from_this())
            );
}
void atlas::task::poll::run_task()
{
    m_create_task(
        io_ptr(),
        boost::bind(&poll::schedule, shared_from_this())
        )->run();
}
atlas::task::poll::poll(
    create_function_type create_task,
    int timeout_ms,
    io_service_type io_,
    boost::function<void()> callback
    ) :
    base(io_, callback),
    m_create_task(create_task),
    m_timeout_ms(timeout_ms),
    m_timer(*io_)
{
}

