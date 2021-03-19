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

void get_mac(const char *interface, char *mac) {
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct ifreq req;
    int i = 0;

    if (sock < 0) {
        return;
    }

    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, interface, IF_NAMESIZE - 1);

    if (ioctl(sock, SIOCGIFHWADDR, &req) < 0) {
        return;
    }
    for(i=0;i<6;i++) {
        
        sprintf(&mac[i*2],"%02X",((unsigned char*)req.ifr_hwaddr.sa_data)[i]);
    }
  

    close(sock);
}
void get_disk_serial(char *serial) {
    struct udev            *ud      = NULL;
    struct stat             statbuf;
    struct udev_device     *device  = NULL;
    struct udev_list_entry *entry   = NULL;

    ud = udev_new();
    if (NULL == ud) {
        fprintf(stderr, "Failed to create udev.\n");
    } else {
        if (0 != stat("/dev/sda", &statbuf)) {
            fprintf(stderr, "Failed to stat /dev/sda.\n");
        } else {
            device = udev_device_new_from_devnum(ud, 'b', statbuf.st_rdev);
            if (NULL == device) {
                fprintf(stderr, "Failed to open /dev/sda.\n");
            } else {
                entry = udev_device_get_properties_list_entry(device);
                while (NULL != entry) {
                    if (0 == strcmp(udev_list_entry_get_name(entry),
                                    "ID_SERIAL")) {
                        break;
                    }

                    entry = udev_list_entry_get_next(entry);
                }
                if (udev_list_entry_get_value(entry) != NULL) {
                    strcpy(serial, udev_list_entry_get_value(entry));
                }

                udev_device_unref(device);
            }
        }
        (void)udev_unref(ud);
    }
}
void get_machine(char *machine)
{
   char s1[4000];
   FILE *fptr;
   fptr = fopen("/var/lib/dbus/machine-id","r");
   while (EOF != fscanf(fptr, "%[^\n]\n", s1))
   {
      
   }
    fclose(fptr);
    strcpy(machine,s1);
}
void get_cpu(char *cpu){
    int EAX,EBX,ECX,EDX;
    char CEAX[20],CEBX[20],CECX[20],CEDX[20];
    __cpuid(0,EAX,EBX,ECX,EDX);
 
    sprintf(CEAX,"%d",EAX);
    sprintf(CEBX,"%d",EBX);
    sprintf(CECX,"%d",ECX);
    sprintf(CEDX,"%d",EDX);
   
    
    strcpy(cpu,";");
    strcpy(cpu,"EAX");
    strcpy(cpu,CEAX);
    strcat(cpu,"EBX");
    strcat(cpu,CEBX);
    strcat(cpu,"ECX");
    strcat(cpu,CECX);
    strcat(cpu,"EDX");
    strcat(cpu,CEDX);

}
void write_file(string text) {
    ofstream myfile;
    myfile.open ("data.txt");
    myfile << text;
    myfile.close();
}
string hexToString(string hex) {
    int len = hex.length();
    string newString;
    for(int i=0; i< len; i+=2)
    {
        string byte = hex.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    return newString;
}
string gen_license(string mac, string disk_serial, string machine, string cpu) {
    string text_plain;
    AutoSeeded_RNG rng;
    string seed ="catmoncwokingdomhomegun";
    string seed2 ="dexraccon56elecearth";
    string seed3 ="dfhdugidiguogfgp";

    text_plain = mac + seed + disk_serial + seed2 + machine + seed3 + cpu;  
    //cout << "Text plain: " << text_plain << endl;

    //string prefix = "key=\"" ;
    //string text_hash = prefix + generate_bcrypt(text_plain, rng,12) + "\"";
    string text_hash = generate_bcrypt(text_plain,rng,12);
    return text_hash;
}
void write_file_license(string text) {
    ofstream myfile;
    myfile.open ("license.cfg");
    myfile << text;
    myfile.close();
}