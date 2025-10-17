#include <logutil/logutil.hpp>

int main(void) {
    Logutil::debug("Hello {}", 1.444);
    Logutil::info("Hello {}", 1);
    Logutil::warn("Hello {}", "World");
    Logutil::error("Hello {:.1f}", 1.5555);
    Logutil::fatal("Hello {}", false);
}