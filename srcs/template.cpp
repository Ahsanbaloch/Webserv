#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

std::string read_file(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string create_html_page(const std::string& directory_name, const std::vector<std::string>& items) {
    std::string html_template = read_file("template.html");

    std::ostringstream directory_items;
    for (std::vector<std::string>::const_iterator it = items.begin(); it != items.end(); ++it) {
        std::string link = "/" + *it;
        directory_items << "<li><a href=\"" << link << "\">" << *it << "</a></li>";
    }

    size_t pos = html_template.find("{directory_name}");
    if (pos != std::string::npos) {
        html_template.replace(pos, 15, directory_name);
    }
    pos = html_template.find("{directory_items}");
    if (pos != std::string::npos) {
        html_template.replace(pos, 17, directory_items.str());
    }

    return html_template;
}

int main() {
    std::vector<std::string> items;
    items.push_back("file1.txt");
    items.push_back("file2.txt");
    items.push_back("subdir1/");
    items.push_back("subdir2/");
    std::string directory_name = "example_directory";

    std::string html_page = create_html_page(directory_name, items);

    std::cout << html_page << std::endl;

    return 0;
}