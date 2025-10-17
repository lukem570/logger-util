#include <cbuild/cbuild.hpp>

int build() {
    
    CBuild::Shared logutil(
        "src/logutil.cpp",
        "logutil"
    );

    logutil.includeDirectory("include");
    logutil.compile();

    CBuild::Executable test(
        "test/main.cpp",
        "test"
    );

    test.includeDirectory("include");
    test.linkLibrary("logutil");
    test.compile();
    
    return 0;
}