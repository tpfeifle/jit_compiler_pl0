#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <pljit/function/pljit.hpp>
//---------------------------------------------------------------------------
namespace pljit_function {
//---------------------------------------------------------------------------
void funcWrapper(FunctionHandle functionHandle, unsigned a, unsigned b, const std::vector<int64_t>& expectedRes) {
    int64_t res = functionHandle(a, b);
    assert(std::find(expectedRes.begin(), expectedRes.end(), res) != expectedRes.end());
    // std::cout << res << std::endl;
}

TEST(Function, TestConcurrent) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, b;\n"
                                     "BEGIN\n"
                                     "    RETURN a + b\n"
                                     "END.\n");


    std::vector<std::thread> threads;
    std::vector<int64_t> expectedRes = {0, 4, 8, 12, 16};
    for (uint32_t value = 0; value < 5; ++value) {
        threads.emplace_back(funcWrapper, func, value, value * 3, expectedRes);
    }

    for (auto& thread : threads)
        thread.join();
}

TEST(Function, TestMultipleFunc) {
    Pljit jit;
    auto func = jit.registerFunction("PARAM a, b;\n"
                                     "BEGIN\n"
                                     "    RETURN a + b\n"
                                     "END.\n");
    auto func2 = jit.registerFunction("PARAM a, b;\n"
                                      "BEGIN\n"
                                      "    RETURN 3* a + b\n"
                                      "END.\n");
    auto res = func(1, 10);
    auto res2 = func2(3, 5);
    assert(res == 11);
    assert(res2 == 14);
}
//---------------------------------------------------------------------------
} // namespace pljit_function
//---------------------------------------------------------------------------
