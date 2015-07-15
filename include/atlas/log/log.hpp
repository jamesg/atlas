#ifndef ATLAS_LOG_LOG_HPP
#define ATLAS_LOG_LOG_HPP

#include <fstream>
#include <sstream>

namespace atlas
{
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
                    void log(const std::string& s) override;
            };

            /*!
             * \brief Log messages to a file.
             */
            class file_sink : public basic_sink
            {
                public:
                    explicit file_sink(const char *filename);
                    void log(const std::string& s) override;
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
                    );

                    ~basic_log();

                    template<typename Output>
                    basic_log& operator<<(const Output& output)
                    {
                        m_oss << output;
                        return *this;
                    }

                protected:
                    basic_sink& sink();

                private:
                    basic_sink& m_sink;
                    std::ostringstream m_oss;
            };
        }

        class error : public detail::basic_log
        {
            public:
                explicit error(const char *source);
        };

        class information : public detail::basic_log
        {
            public:
                explicit information(const char *source);
        };

        class test : public detail::basic_log
        {
            public:
                explicit test(const char *source);
        };

        class warning : public detail::basic_log
        {
            public:
                explicit warning(const char *source);
        };
    }
}

#endif
