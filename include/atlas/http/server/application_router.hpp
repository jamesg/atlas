#ifndef ATLAS_HTTP_SERVER_APPLICATION_ROUTER_HPP
#define ATLAS_HTTP_SERVER_APPLICATION_ROUTER_HPP

#include "atlas/http/server/router.hpp"
#include "atlas/http/server/mimetypes.hpp"

namespace atlas
{
    namespace http
    {
        /*!
         * \brief A HTTP request router with additional functions to make Web
         * application development simpler.
         */
        class application_router : public router
        {
        public:
            const atlas::http::mimetypes& mime_information() const;
            /*!
             * \brief Install a static string at a URI.
             *
             * The content-type to present to the client is derived from the URI.
             */
            void install_static_text(
                    const std::string& uri,
                    const std::string& text
                    );

            /*!
             * \brief Install a static string at a URI.
             *
             * \param extension File extension to use to choose the
             * content-type.
             */
            void install_static_text(
                    const std::string& uri,
                    const std::string& extension,
                    const std::string& text
                    );

        private:
            atlas::http::mimetypes m_mime_information;
        };
    }
}

#endif

