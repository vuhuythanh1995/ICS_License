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
using namespace Botan;

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
   
    strcat(plain_text, ":");
    strcat(plain_text, serial);
    strcat(plain_text, "/");
    strcat(plain_text, machine);
    strcat(plain_text, "*");
    strcat(plain_text, cpu);
    cout << plain_text << endl;

    const string plaintext = string(plain_text);

    Botan::AutoSeeded_RNG rng; // rng random number generators
    const vector<uint8_t> key = Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C"); // hex_decode() hex=>string

    //ENCRYPT
    unique_ptr<Botan::Cipher_Mode> enc = Botan::Cipher_Mode::create("AES-128/CBC/PKCS7", Botan::ENCRYPTION);
    enc->set_key(key);
    // Copy input data to a buffer that will be encrypted
    Botan::secure_vector<uint8_t> pt1(plaintext.data(), plaintext.data()+plaintext.length());
    //generate fresh nonce (IV)
    string iv_str = "onwire2whiteepasadcame";
    Botan::secure_vector<uint8_t> iv(iv_str.data(), iv_str.data()+16);

    enc->start(iv);
    enc->finish(pt1);

    //Convert from secure_vector into hex string
    string text_encrypt = Botan::hex_encode(pt1);
    //cout << text_encrypt << endl;

    //Write to file
    write_file(text_encrypt);
    
    string line;
    ifstream myfile("data.txt");

    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            break;
        }
        myfile.close();
    }
    else {
        cout <<"Run: ./get_license data.txt" << endl;
        cout <<"Error file 'data.txt' not found..." << endl;
        return 1;
    }

    cout << "Reading data.txt..." << line << '\n';

    Botan::secure_vector<uint8_t> iv1(iv_str.data(), iv_str.data()+16);

    unique_ptr<Botan::Cipher_Mode> dec = Botan::Cipher_Mode::create("AES-128/CBC/PKCS7", Botan::DECRYPTION);
    dec->set_key(key);
    // Copy input data to a buffer that will be decrypted
    const vector<uint8_t> pt = Botan::hex_decode(line);
    Botan::secure_vector<uint8_t> pt2(pt.begin(), pt.end());

    dec->start(iv);
    dec->finish(pt2);

    string hex_dec = Botan::hex_encode(pt2);
    string str = hexToString(hex_dec);
    

    size_t pos = str.find(":");
    size_t pos2 = str.find("/");
    size_t pos3 = str.find("*");
    string mac1 = str.substr(0, pos);
    string disk_serial1 = str.substr(pos+1,pos2-pos-1);
    string machine1 = str.substr(pos2+1,pos3-pos2-1);
    string cpu1 = str.substr(pos3+1);
 
    string seed ="catmoncwokingdomhomegun";
    string seed2 ="dexraccon56elecearth";
    string seed3 ="dfhdugidiguogfgp";
    string text_plain = mac1 + seed + disk_serial1 + seed2 + machine1 + seed3 + cpu1;

    string licenseText;
    fstream fileLicense;
    fileLicense.open ("license.cfg");
    fileLicense >> licenseText;
    fileLicense.close();
    cout<<"license = "<<licenseText<<endl;
    bool check = Botan::check_bcrypt(text_plain,licenseText);
    if (check == false) cout<<"license not ok"<<endl;
    else    cout<<"license ok"<<endl;
    return 0;
}
