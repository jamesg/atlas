#include "atlas/task/base.hpp"

void atlas::task::base::mark_finished()
{
    if(!m_finished)
    {
        m_callback();
        m_finished = true;
    }
}

