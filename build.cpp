#include <cbuild/cbuild.hpp>

int build(CBuild::Context context) {
    
    CBuild::Shared logutil(
        context,
        "src/logutil.cpp",
        "logutil"
    );

    logutil.includeDirectory("include");
    logutil.compile();

    CBuild::Executable test(
        context,
        "test/main.cpp",
        "test"
    );

    test.includeDirectory("include");
    test.linkLibrary("logutil");
    test.compile();
    
    return 0;
}