// Include the headers
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

std::string tail(std::string const &source, size_t const length)
{
    if (length >= source.size())
    {
        return source;
    }
    return source.substr(source.size() - length);
} // tail

int main()
{
    // Data structure to store S-Record in.
    struct S_Record
    {
        string header;
        string length;
        string address;
        string data;
        string crc;
    };

    // A vector of S-Records
    vector<S_Record> s_record_list;

    // Try to open the file.
    ifstream s_record_file("pwr_controller.s19");

    string line;

    // Read each line in the file. Assumes each line is terminated by a 'newline'
    while (getline(s_record_file, line))
    {
        S_Record s_record;

        // Split the line in parts.
        s_record.header = line.substr(0, 2);                 // Start at 0, length 2
        s_record.length = line.substr(2, 2);                 // Start at 2, length 2
        s_record.address = line.substr(4, 8);                // Satart at 4, length 4, 32 bit address
        s_record.data = line.substr(12, line.length() - 15); // Start at 12 (after the address), up to the CRC.
        s_record.crc = line.substr(line.length() - 3, 2);    // grab the last 2 as the CRC

        // Add it to the list.
        s_record_list.push_back(s_record);
    }

    s_record_file.close();

    return 0;

    // Print them out.
    for (size_t i = 0; i < s_record_list.size(); ++i)
    {
        cout << "Header = " << s_record_list[i].header
             << " : Length = " << s_record_list[i].length
             << " : Address = " << s_record_list[i].address
             << " : Data = " << s_record_list[i].data
             << " : CRC = " << s_record_list[i].crc
             << "\n";
    }

    // Write them to a new file.
    ofstream output_file("output.txt");

    if (!output_file.fail())
    {
        for (size_t i = 0; i < s_record_list.size(); ++i)
        {
            output_file << s_record_list[i].header
                        << s_record_list[i].length
                        << s_record_list[i].address
                        << s_record_list[i].data
                        << "\n";
        }

        output_file.close();
    }
}