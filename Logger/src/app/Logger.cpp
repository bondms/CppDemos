#include "Logger.h"

#include "FlushController.h"
#include "scope_exit.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <memory>
#include <mutex>
#include <sstream>
#include <sys/types.h>
#include <thread>
#include <time.h>
#include <unistd.h>

using namespace std::chrono_literals;

namespace Logger
{
    namespace
    {
        class Impl
        {
            const std::experimental::filesystem::path path_;

            std::mutex mutex_{};
            std::ofstream ofs_;

            FlushController<std::chrono::steady_clock> flush_controller_{5s};

        public:
            Impl(const std::experimental::filesystem::path& path) :
                path_{path},
                ofs_{path, std::ios_base::app}
            {
                if(!ofs_.good())
                {
                    throw std::runtime_error("Failed to open log file: " + path_.u8string());
                }
            }

            void Log(Severity severity, const std::string& msg) noexcept
            {
                // TODO(escaping, rotation, logging of different types).
#ifndef _DEBUG
                if (severity >= Severity::Debug)
                {
                    return;
                }
#endif
                std::lock_guard<decltype(mutex_)> lock{mutex_};
                ofs_
                    << MessageTimeStamp(std::chrono::system_clock::now())
                    << ' ' << std::setw(7) << std::setfill('0') << ::getpid()
                    // TODO(Reconsider setw)
                    << ':' << std::setw(15) << std::setfill('0') << std::this_thread::get_id()
                    << ' ' << AsString(severity)
                    << " >> " << msg
                    << '\n';

                if (flush_controller_.is_due())
                {
                    ofs_.flush();
                }
            }

            void Flush(ErrorReporting errorReporting)
            {
                auto reset_exceptions = make_scope_exit([&]{
                    ofs_.exceptions(std::ios::goodbit);
                });

                if (ErrorReporting::ignoreError != errorReporting)
                {
                    ofs_.exceptions(std::ios::badbit | std::ios::failbit);
                }
                ofs_.flush();
            }

            void Close(ErrorReporting errorReporting)
            {
                if (ErrorReporting::ignoreError != errorReporting)
                {
                    ofs_.exceptions(std::ios::badbit | std::ios::failbit);
                }
                ofs_.close();
            }
        };

        std::unique_ptr<Impl> singleton_impl{};
    } // namespace

    const char* AsString(Severity severity) noexcept
    {
        switch (severity)
        {
        case Severity::Error:
            return "ERR";
        case Severity::Warning:
            return "WRN";
        case Severity::Info:
            return "INF";
        case Severity::Debug:
            return "DBG";
        }
        assert(false);
    }

    // TODO(Re-consider error handing. Should client be informed if there was a failure?)
    std::string MessageTimeStamp(const std::chrono::system_clock::time_point& time_point) noexcept
    {
        auto tt{std::chrono::system_clock::to_time_t(time_point)};
        std::tm gmtime{};
        gmtime_r(&tt, &gmtime);

        const std::chrono::duration<double> tse = time_point.time_since_epoch();
        std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count() % 1000;

        std::ostringstream oss{};
        oss << std::put_time(&gmtime, "%FT%T.") << std::setfill('0') << std::setw(3) << milliseconds << "Z";
        return oss.str();
    }

    std::string FileNameTimeStamp(const std::chrono::system_clock::time_point& time_point)
    {
        auto tt{std::chrono::system_clock::to_time_t(time_point)};
        std::tm gmtime{};
        gmtime_r(&tt, &gmtime);

        std::ostringstream oss{};
        oss.exceptions(std::ios::failbit | std::ios::badbit);
        oss << std::put_time(&gmtime, "%Y%m%d_%H%M%S");
        return oss.str();
    }

    void Initialise(const std::experimental::filesystem::path& path)
    {
        singleton_impl = std::make_unique<Impl>(path);
    }

    // TODO(Re-consider error handing. Should client be informed if there was a failure?)
    void Log(Severity severity, const std::string& msg) noexcept
    {
        if (!singleton_impl)
        {
            return;
        }
        singleton_impl->Log(severity, msg);
    }

    void Flush(ErrorReporting errorReporting)
    {
        if (!singleton_impl)
        {
            return;
        }
        singleton_impl->Flush(errorReporting);
    }

    void Close(ErrorReporting errorReporting)
    {
        if (!singleton_impl)
        {
            return;
        }
        singleton_impl->Close(errorReporting);
        singleton_impl.reset();
    }
}
