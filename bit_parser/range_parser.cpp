#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <stdint.h>
#include <unistd.h>
#include <sstream>

#define DE_MODE_BITS            2
#define DE_MODE_MASK            (0x03)
#define SENSOR_NUM_BITS     9
#define SENSOR_SIGNAL_MASK      (0x01FF)

using namespace std;

int main()
{
    unsigned int dict[228];
    string instring;
    unsigned int tmp;


    ifstream fin;

    fin.open("parsing.txt");

    if (!fin)
    {
        cout << "Could not open file" << endl;
        return 0;
    }

    cout << "Reading ..." << endl;

    for (int i = 0; i < 228; i++) 
    {
        fin >> instring;

        std::stringstream ss;
        ss << std::hex << instring;
        ss >> dict[i];

    }
    fin.close();

    printf("size: %d\n", sizeof(dict));

    uint8_t DE_mode = dict[i] & DE_MODE_MASK;
    uint16_t sensors[5];
    for (int i = 0; i < 228; i+2)
    {
        sensors[0] = (dict[i] >> DE_MODE_BITS) & SENSOR_SIGNAL_MASK;
        sensors[1] = (dict[i] >> SENSOR_NUM_BITS) & SENSOR_SIGNAL_MASK;
        sensors[2] = (dict[i] >> SENSOR_NUM_BITS) & SENSOR_SIGNAL_MASK;

        dict[i] |= (g_ui8RXMsgData[4] << 3);
        dict[i] |= ((uint32_t)g_ui8RXMsgData[5] << 11);

        sensors[3] = (dict[i] >> SENSOR_NUM_BITS) & SENSOR_SIGNAL_MASK;
        sensors[4] = (dict[i] >> SENSOR_NUM_BITS) & SENSOR_SIGNAL_MASK;

        sleep(1);
    }


    return 0;
}
