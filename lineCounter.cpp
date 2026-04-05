#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

using std::string;
namespace fs = std::filesystem;

std::vector<std::string> get_all_subdirectories(const std::string& root_path) {
    std::vector<std::string> subdirs;
    subdirs.push_back(root_path);
    try {
        // Use recursive_directory_iterator to get subfolders at all depths
        for (const auto& entry : fs::recursive_directory_iterator(root_path)) {
            if (entry.is_directory()) {
                subdirs.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return subdirs;
}

int getLineCount(const string& path) {
    int total = 0;
    try {
        // Use recursive_directory_iterator to get subfolders at all depths
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (entry.is_directory()) {
                total += getLineCount(entry.path().generic_string());
            }
            else if (entry.is_regular_file()) {
                std::ifstream ifs(entry.path());
                if (!ifs.is_open()) {
                    std::cerr << "Error opening file" << std::endl;
                    return 0;
                }
                string line;
                while (std::getline(ifs, line)) total++;

                ifs.close();
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return total;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "\033[31mUSAGE: [lc] <dirPath>\033[0m";
        return 1;
    }

    string path = argv[1];
    std::cout << getLineCount(path) << " lines" << std::endl;
    return 0;
}