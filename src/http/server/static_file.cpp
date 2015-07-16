#include "atlas/http/server/static_file.hpp"

#include <fstream>
#include <streambuf>

#include "atlas/http/server/error.hpp"
#include "atlas/log/log.hpp"
#include "atlas/http/server/mimetypes.hpp"

void atlas::http::static_file(
        const mimetypes& mime_information,
        const std::string& filename,
        mg_connection *conn,
        uri_parameters_type,
        http::uri_callback_type success,
        http::uri_callback_type error
        )
{
    //mg_send_file(conn, filename.c_str());

    log::test("atlas::http::static_file") << "serving static file " << filename;
    std::string file_content;
    std::ifstream in(filename);
    if(!in.is_open())
    {
        log::warning("static_file") << "file not open (" << filename << ")";
        http::error(
                404,
                "file not found",
                conn,
                success,
                error
                );
        return;
    }
    in.seekg(0, std::ios::end);
    file_content.reserve(in.tellg());
    in.seekg(0, std::ios::beg);
    file_content.assign(
            std::istreambuf_iterator<char>(in),
            std::istreambuf_iterator<char>()
            );

    std::string extension;
    {
        std::string::size_type dot_pos = filename.find_last_of('.');
        if(dot_pos != std::string::npos)
            extension = filename.substr(dot_pos+1);
    }

    mg_send_status(conn, 200);

    log::information("http::static_file") << "filename: " << filename <<
        ", mimetype: " << mime_information.content_type(extension) <<
        ", extension: " << extension;

    mg_send_header(conn, "Connection", "close");
    mg_send_header(conn, "Content-Type", mime_information.content_type(extension).c_str());

    mg_send_data(conn, file_content.c_str(), file_content.length());

    success();
}
