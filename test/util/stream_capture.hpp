#pragma once

#include <sstream>

class stream_capture
{
public:
    explicit stream_capture(std::ostream& stream)
        : stream_ptr_(&stream), saved_rdbuf_(stream_ptr_->rdbuf())
    {
        stream_ptr_->rdbuf(buffer_.rdbuf());
    }

    inline std::string str() const { return buffer_.str(); }

    ~stream_capture()
    {
        stream_ptr_->rdbuf(saved_rdbuf_);
    }

private:
    std::ostream* stream_ptr_;
    std::stringstream buffer_;
    std::streambuf* saved_rdbuf_;
};
