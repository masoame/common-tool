#include <cryptlib.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <md5.h>
#include <files.h>
#include <hex.h>
#include <iostream>



int main(int argc, char* argv[]) {
    using namespace CryptoPP;

    std::string str1 = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd";
    std::string str2 = "abcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcdabcd.";
    std::string str1_md5, str2_md5;

    CryptoPP::HexEncoder encoder(new CryptoPP::FileSink(std::cout));

    CryptoPP::Weak1::MD5 hash;
    hash.Update((const byte*)str1.c_str(), str1.length());
    str1_md5.resize(hash.DigestSize());
    hash.Final((byte*)&str1_md5[0]);
    std::cout << "str1_md5: " << std::endl;
    CryptoPP::StringSource(str1_md5, true, new CryptoPP::Redirector(encoder));
    std::cout << std::endl;


    hash.Update((const byte*)str2.c_str(), str2.length());
    str2_md5.resize(hash.DigestSize());
    hash.Final((byte*)&str2_md5[0]);
    std::cout << "str2_md5: " << std::endl;
    CryptoPP::StringSource(str2_md5, true, new CryptoPP::Redirector(encoder));
    std::cout << std::endl;


    if (0 == str1_md5.compare(str2_md5)) {
        std::cout << "str1 == str1" << std::endl;
    }
    else {
        std::cout << "str2 != str2" << std::endl;
    }
}
