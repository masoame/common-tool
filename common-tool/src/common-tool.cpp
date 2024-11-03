#include <cryptlib.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>
#include <files.h>
#include <hex.h>
#include <iostream>
#include <regex>
#include"export_results.h"

unsigned char xor_data[32] =
{
  '\xF1',
  '\xEF',
  'a',
  '\xBB',
  '\xC9',
  'E',
  'W',
  'C',
  '6',
  '\xEB',
  '\xC3',
  '\xF5',
  'a',
  '\x1F',
  '\xE0',
  '\xED',
  '_',
  '\x19',
  '\xC3',
  '\x83',
  '\v',
  'g',
  '[',
  'D',
  'z',
  '\x9D',
  '\xB2',
  '~',
  '\xF5',
  '\xB5',
  '\"',
  'e'
};

//RC4
int __cdecl sub_401080(char* a1, char* a2, char* a3, int a4)
{
    int i; // [esp+4Ch] [ebp-4h]

    for (i = 0; i < 256; ++i)
    {
        a1[i] = i;
        a2[i] = a3[i % a4];
    }
    return 0;
}
int __cdecl sub_4010F0(char* a1, char* a2)
{
    int v3; // [esp+4Ch] [ebp-Ch]
    int i; // [esp+50h] [ebp-8h]
    char v5; // [esp+54h] [ebp-4h]

    v3 = 0;
    for (i = 0; i < 256; ++i)
    {
        v3 = ((unsigned __int8)a2[i] + (unsigned __int8)a1[i] + v3) % 256;
        v5 = a1[v3];
        a1[v3] = a1[i];
        a1[i] = v5;
    }
    return 0;
}

using _BYTE = char;

int __cdecl sub_4012A0(_BYTE* a1, _BYTE* a2, _BYTE* a3, int a4)
{
    int i; // [esp+4Ch] [ebp-4h]

    for (i = 0; i < a4; ++i)
    {
        if (i)
            a3[i] = a3[(unsigned __int8)a1[i] % i] ^ a1[i] ^ a2[i];
        else
            *a3 = *a1 ^ *a2;
    }
    return 0;
}
int __cdecl sub_4011A0(char* a1, char* a2, int a3)
{
    int i; // [esp+50h] [ebp-10h]
    int v5; // [esp+54h] [ebp-Ch]
    int v6; // [esp+58h] [ebp-8h]
    char v7; // [esp+5Ch] [ebp-4h]

    v6 = 0;
    v5 = 0;
    for (i = 0; i < a3; ++i)
    {
        v6 = (v6 + 1) % 256;
        v5 = ((unsigned __int8)a1[v6] + v5) % 256;
        v7 = a1[v5];
        a1[v5] = a1[v6];
        a1[v6] = v7;
        a2[i] = a1[((unsigned __int8)a1[v5] + (unsigned __int8)a1[v6]) % 256];
    }
    return 0;
}

//----------------------------------------
int __cdecl sub_401390(int a1, int a2)
{
    return ~(a2 + ~a1);
}

int main(int argc, char* argv[]) {

    using namespace CryptoPP;
    int ans1;
    for (int i = 234;; ++i)
    {
        if ((i & 1) != 0)continue;

        auto num = i / 10000;
        if (num > 9999)continue;

        auto num1 = i % 10000 / 100;
        if (num1 > 12)continue;

        auto num2 = i % 10000 % 100;
        if (num2 > 32)continue;

        auto a = sub_401390(7 * num1, 1);
        auto b = sub_401390(11 * (13 * a + num2 + 3), num1);
        auto c = ((b - num2) / 10 + 11) ^ 0x104B4;
        if (c * (num1 + num2 + num) != 136398636) continue;

        std::cout << i << std::endl;
        ans1 = i;
        break;

    }

    std::vector<unsigned char> ans;
    ans.reserve(2560);
    memset(ans.data(), 0, 2560);
    for (int i = 0; i != 90000; i++)
    {
        if (ida_chars[i]) {
            ans.emplace_back(i / 300);
            ans.emplace_back(i % 300);
        }
    }


    for (int i = 0;i!=ans.size();i++) {
        ans[i] ^= xor_data[i];
    }

    char v4[256]; // [esp+54h] [ebp-504h] BYREF
    char* Source; // [esp+154h] [ebp-404h]
    char Destination[256]; // [esp+158h] [ebp-400h] BYREF
    char Str[256]; // [esp+258h] [ebp-300h] BYREF
    char a2[256]; // [esp+358h] [ebp-200h] BYREF
    char a1[256]; // [esp+458h] [ebp-100h] BYREF



    memset(a1, 0, sizeof(a1));
    memset(a2, 0, sizeof(a2));
    memset(Str, 0, sizeof(Str));
    memset(v4, 0, sizeof(v4));
    memset(Destination, 0, sizeof(Destination));

    Source = (char*)ans.data();

    strncpy(Destination, Source, ans.size());
    
    _itoa(ans1, Str, 8);
    int v1 = strlen(Str);
    sub_401080(a1, a2, Str, v1);
    sub_4010F0(a1, a2);
    sub_4011A0(a1, Str, 256);
    sub_4012A0(Str, Destination, Source, 32);


    //std::ranges::for_each(ans, [](unsigned char i) { std::cout << i; });
    std::cout << Source << std::endl;


    return 0;

}
