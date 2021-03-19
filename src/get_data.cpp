#include <stdio.h>
#include <libudev.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <botan/botan.h>
#include <botan-2/botan/bcrypt.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <botan/rng.h>
#include <botan/auto_rng.h>
#include <botan/cipher_mode.h>
#include <botan/hex.h>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cpuid.h>
#include "license.h"
using namespace std;

int main(int argc, char *argv[])
{
    char mac[13];
    char serial[100];
    char plain_text[200];
    char machine[50];
    char cpu[50];
    if (argc < 2) {
        printf("Example: %s bond0\n", argv[0]);
        return 1;
    }
    
    get_mac(argv[1], mac);
    get_disk_serial(serial);
    get_machine(machine);
    get_cpu(cpu);

    strcpy(plain_text,mac);
    //strcpy(plain_text,"000C2951ADD9");
    strcat(plain_text, ":");
    strcat(plain_text, serial);
    strcat(plain_text, "/");
    strcat(plain_text, machine);
    strcat(plain_text, "*");
    strcat(plain_text, cpu);
    cout << plain_text << endl;

    string plaintext = string(plain_text);

    Botan::AutoSeeded_RNG rng; // rng random number generators
    const vector<uint8_t> key = Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C"); // hex_decode() hex=>string

    //ENCRYPT
    unique_ptr<Botan::Cipher_Mode> enc = Botan::Cipher_Mode::create("AES-128/CBC/PKCS7", Botan::ENCRYPTION);
    enc->set_key(key);
    // Copy input data to a buffer that will be encrypted
    Botan::secure_vector<uint8_t> pt(plaintext.data(), plaintext.data()+plaintext.length());
    //generate fresh nonce (IV)
    string iv_str = "onwire2whiteepasadcame";
    Botan::secure_vector<uint8_t> iv(iv_str.data(), iv_str.data()+16);

    enc->start(iv);
    enc->finish(pt);

    //Convert from secure_vector into hex string
    string text_encrypt = Botan::hex_encode(pt);
    //cout << text_encrypt << endl;

    //Write to file
    write_file(text_encrypt);

    return 0;
}
