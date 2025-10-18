#include <logutil/logutil.hpp>

namespace Logutil {

    __Logutil* __Logutil::instance = nullptr;

    std::wstring toString(LogLevel level) {
        switch (level) {
            case LogLevel::eDebug: return L"DEBUG";
            case LogLevel::eInfo:  return L"INFO";
            case LogLevel::eWarn:  return L"WARN";
            case LogLevel::eError: return L"ERROR";
            case LogLevel::eFatal: return L"FATAL";
        }
        return L"???";
    }

    const wchar_t* levelColor(LogLevel level) {
        switch (level) {
            case LogLevel::eDebug: return MAGENTA;
            case LogLevel::eInfo:  return CYAN;
            case LogLevel::eWarn:  return YELLOW;
            case LogLevel::eError: return RED;
            case LogLevel::eFatal: return BOLD RED;
        }
        return WHITE;
    }

    __Logutil* __Logutil::getInstance() noexcept {
        if (!instance) 
            instance = new __Logutil();
        return instance;
    }
    
    std::wstring truncate(const std::wstring& str, size_t width) {
        if (str.length() <= width) {
            return str;
        } else if (width > 3) {
            return L"..." + str.substr(str.length() - (width - 3), width - 3);
        } else {
            return str.substr(str.length() - width, width);
        }
    }

    std::wstring formatArg(const LogVariant& arg, bool precision = false) {
        std::wstringstream ss;

        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, const char*>)
                ss << value;
            else if constexpr (std::is_same_v<T, std::string>)
                ss << std::wstring(value.begin(), value.end());
            else if constexpr (std::is_same_v<T, std::wstring>)
                ss << value;
            else if constexpr (std::is_same_v<T, bool>)
                ss << BLUE << (value ? L"true" : L"false") << WHITE;
            else if constexpr (std::is_same_v<T, void*>)
                ss << std::hex << value;
            else if constexpr (std::is_same_v<T, double>) {
                if (precision)
                    ss << std::fixed << std::setprecision(1) << value;
                else
                    ss << value;
            } else if constexpr (std::is_same_v<T, float>) {
                if (precision)
                    ss << std::fixed << std::setprecision(1) << value;
                else
                    ss << value;
            } else {
                ss << value;
            }
        }, arg);

        return ss.str();
    }

    std::wstring simplify(std::wstring data, std::initializer_list<LogVariant> args) {
        std::wstring result;
        size_t pos = 0;
        auto it = args.begin();

        while (pos < data.length()) {
            size_t bracePos = data.find(L'{', pos);
            if (bracePos == std::wstring::npos || it == args.end()) {
                result += data.substr(pos);
                break;
            }

            result += data.substr(pos, bracePos - pos);

            if (data[bracePos + 1] == L'}') {
                result += formatArg(*it);
                pos = bracePos + 2;
            } else if (data.substr(bracePos, 6) == L"{:.1f}") {
                result += formatArg(*it, true);
                pos = bracePos + 6;
            } else {
                result += data[bracePos];
                pos = bracePos + 1;
                continue;
            }

            ++it;
        }

        return result;
    }

    std::wstring getTime() {

        time_t time = std::time(nullptr);
        tm* localTime = std::localtime(&time);

        auto now = clk::system_clock::now();

        auto ms = clk::duration_cast<clk::milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream output;
        output << std::put_time(localTime, "%d-%m-%Y %H-%M-%S");
        output << '.' << std::setw(3) << std::setfill('0') << ms.count();
        std::string str = output.str();

        return std::wstring(str.begin(), str.end());
    }

    void __Logutil::intrisicLog(LogLevel level, std::wstring data, std::initializer_list<LogVariant> args, std::source_location location) noexcept {

        const unsigned int maxFileWidth = 32;

        std::wstringstream locationStream;
        locationStream << location.file_name() << L'(';
        locationStream << location.line() << L':';
        locationStream << location.column() << L") ";
        locationStream << location.function_name();
        
        std::wstringstream output;
        output << GREEN;
        output << getTime();
        output << RESET << " ";
        output << levelColor(level);
        output << std::left << std::setw(8) << L"["+toString(level)+L"]";
        output << RESET << " ";
        output << BLUE;
        output << std::right << std::setw(maxFileWidth) << truncate(locationStream.str(), maxFileWidth);
        output << RESET << " ";
        output << WHITE;
        output << simplify(data, args);


        wprintf(L"%ls\n", output.str().c_str());
    }
}