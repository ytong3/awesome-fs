#include <string>
#include <vector>
#include <stdint.h>

struct Dir{
    std::vector<uint16_t> nodeNum;
    std::vector<std::string> fileName;

    bool write_to_disk();
    bool read_from_disk();
}
