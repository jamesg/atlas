#ifndef ATLAS_LOG_HPP
#define ATLAS_LOG_HPP

#include <fstream>
#include <iostream>
#include <sstream>

namespace atlas
{
    /*!
     * \brief Format an save logged information.
     */
    namespace log
    {
        namespace detail
        {
            class basic_sink
            {
                public:
                    virtual void log(const std::string&) = 0;
            };

            basic_sink& get_default_sink();

            /*!
             * \brief Log messages to std::cerr.  Useful for debugging on the
             * command line.
             */
            class cerr_sink : public basic_sink
            {
                public:
                    void log(const std::string& s) override
                    {
                        std::cerr << s << std::endl;
                    }
            };

            /*!
             * \brief Log messages to a file.
             */
            class file_sink : public basic_sink
            {
                public:
                    explicit file_sink(const char *filename) :
                        m_fstream(filename, std::ios::app)
                    {
                    }
                    void log(const std::string& s) override
                    {
                        m_fstream << s << std::endl;
                    }
                private:
                    std::ofstream m_fstream;
            };

            /*!
             * \brief Log a message with a specified severity level and source
             * module.
             */
            class basic_log
            {
                public:
                    basic_log(
                            const char *source,
                            const char *severity,
                            basic_sink& sink = get_default_sink()
                            ) :
                        m_sink(sink)
                    {
                        m_oss << severity << " " << source << " ";
                    }

                    ~basic_log()
                    {
                        m_sink.log(m_oss.str());
                    }

                    template<typename Output>
                    basic_log& operator<<(const Output& output)
                    {
                        m_oss << output;
                        return *this;
                    }

                protected:
                    basic_sink& sink()
                    {
                        return m_sink;
                    }

                private:
                    basic_sink& m_sink;
                    std::ostringstream m_oss;
            };
        }

        class error : public detail::basic_log
        {
            public:
                error(const char *source) :
                    basic_log(source, "error")
                {
                }
        };

        class information : public detail::basic_log
        {
            public:
                information(const char *source) :
                    basic_log(source, "information")
                {
                }
        };

        class warning : public detail::basic_log
        {
            public:
                warning(const char *source) :
                    basic_log(source, "warning")
                {
                }
        };
    }
}

#endif

