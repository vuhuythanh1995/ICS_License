#ifndef license_h
#define license_h
void get_mac(const char *interface, char *mac);
void get_disk_serial(char *serial);
void get_machine(char *machine);
void get_cpu(char *cpu);
void write_file(string text); //myfile.open ("data.txt");
string hexToString(string hex);
string gen_license(string mac, string disk_serial, string machine, string cpu);
void write_file_license(string text); //myfile.open ("license.cfg");
#endif

