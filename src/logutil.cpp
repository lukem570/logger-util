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

    void __Logutil::intrisicLog(LogLevel level, fs::path file, int line, std::wstring data, std::initializer_list<LogVariant> args) noexcept {

        const unsigned int maxFileWidth = 16;
        
        std::wstringstream output;
        output << levelColor(level);
        output << std::left << std::setw(8) << L"["+toString(level)+L"]";
        output << RESET << " ";
        output << BLUE;
        output << std::right << std::setw(maxFileWidth) << truncate(file.wstring() + L":" + std::to_wstring(line), maxFileWidth);
        output << RESET << " ";
        output << WHITE;
        output << simplify(data, args);


        wprintf(L"%ls\n", output.str().c_str());
    }
}