#ifndef LOGUTIL_HPP
#define LOGUTIL_HPP

#include <cstdio>
#include <string>
#include <variant>
#include <initializer_list>
#include <filesystem>
#include <iomanip>
#include <chrono>
#include <source_location>

namespace fs = std::filesystem;
namespace clk = std::chrono;

std::wstring operator "" _wstr(const wchar_t* str, size_t len) {
    std::wstring s(str, len);
    return s;
}

#include <string>

#ifdef _WIN32
#include <windows.h>

std::wstring operator "" _wstr(const char* str, size_t len) {
    int wide_len = MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0);
    std::wstring wstr(wide_len, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, static_cast<int>(len), &wstr[0], wide_len);
    return wstr;
}

#else
#include <clocale>
#include <cstdlib>

std::wstring operator "" _wstr(const char* str, size_t len) {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    std::mbstate_t state{};
    const char* src = str;
    size_t needed = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (needed == static_cast<size_t>(-1)) return L"[conversion failed]";
    std::wstring wstr(needed, L'\0');
    src = str;
    std::mbsrtowcs(&wstr[0], &src, needed, &state);
    return wstr;
}
#endif

#define debug(data, ...) __Logutil::intrisicLog(Logutil::LogLevel::eDebug, data ## _wstr, {__VA_ARGS__})
#define info(data, ...)  __Logutil::intrisicLog(Logutil::LogLevel::eInfo,  data ## _wstr, {__VA_ARGS__})
#define warn(data, ...)  __Logutil::intrisicLog(Logutil::LogLevel::eWarn,  data ## _wstr, {__VA_ARGS__})
#define error(data, ...) __Logutil::intrisicLog(Logutil::LogLevel::eError, data ## _wstr, {__VA_ARGS__})
#define fatal(data, ...) __Logutil::intrisicLog(Logutil::LogLevel::eFatal, data ## _wstr, {__VA_ARGS__})

#define RESET   L"\033[0m"

#define BOLD    "\033[1m"

#define RED     L"\033[31m"
#define GREEN   L"\033[0;32m"
#define YELLOW  L"\033[33m"
#define BLUE    L"\033[34m"
#define MAGENTA L"\033[35m"
#define CYAN    L"\033[36m"
#define WHITE   L"\033[37m"

namespace Logutil {

    enum class LogLevel {
        eDebug,
        eInfo,
        eWarn,
        eError,
        eFatal,
    };

    using LogVariant = std::variant<int, unsigned int, char, std::string, std::wstring, const char*, bool, double, void*>;


    class __Logutil {
        public:

            static void intrisicLog(LogLevel level, std::wstring data, std::initializer_list<LogVariant> args, std::source_location location = std::source_location::current()) noexcept;

        private:    
            static __Logutil* instance;

            static __Logutil* getInstance() noexcept;
    };
}

#endif