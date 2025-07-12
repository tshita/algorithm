#include <cassert>
#include <cstdint>
#include <cstring>
#include <bit>
#include <iostream>

int main()
{
  float f = 3.14f;

  // ビット表現を保って、単精度浮動小数点数 (IEEE 754準拠なら32ビット) を、32ビット整数に変換
  std::uint32_t n = std::bit_cast<std::uint32_t>(f);

  // 従来の方法
  std::uint32_t m = 0;
  std::memcpy(&m, &f, 4);

  std::cout << n << std::endl;
  std::cout << m << std::endl;
  assert(n == m);
}