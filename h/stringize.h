#pragma once

// 二段階文字列化イデオムマクロ
#define STRINGIZE_INTERNAL(x) #x
#define STRINGIZE(x) STRINGIZE_INTERNAL(x)
