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

#include <fstream>

#include <botan/rng.h>
#include <botan/auto_rng.h>
#include <botan/cipher_mode.h>
#include <botan/hex.h>
#include "license.h"

using namespace std;
using namespace Botan;

int main()
{
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

    const vector<uint8_t> key = Botan::hex_decode("2B7E151628AED2A6ABF7158809CF4F3C");
    //generate fresh nonce (IV)
    string iv_str = "onwire2whiteepasadcame";
    Botan::secure_vector<uint8_t> iv(iv_str.data(), iv_str.data()+16);

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
    string mac = str.substr(0, pos);
    string disk_serial = str.substr(pos+1,pos2-pos-1);
    string machine = str.substr(pos2+1,pos3-pos2-1);
    string cpu = str.substr(pos3+1);

    cout << "Checking MAC address..." << mac << endl;
    cout << "Checking Disk serial..." << disk_serial << endl;
    cout << "Checking Machine id..." << machine<< endl;
    cout << "Checking Cpu id..." << cpu<< endl;

    string license = gen_license(mac, disk_serial, machine, cpu);
    cout << "Generating license..." << license << endl;
    cout << "Export license successfully." << endl;
    cout << "Please copy license.cfg file into opconsole/config/license folder." << endl;

    write_file_license(license);
    
    return 0;
}
