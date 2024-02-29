// Read the config file and fetch the data.
// Returns true if successful, false otherwise.
bool readConfigFile(const std::string& configFilePath, std::string& configFileContent);

// Separate server blocks and store them in a vector container of strings.
void separateServerBlocks(const std::string& configFileContent, std::vector<std::string>& serverBlocks);

// Separate location blocks within each server block.
void separateLocationBlocks(const std::string& serverBlock, std::vector<std::string>& locationBlocks);

// Extract properties of the server block.
void extractServerProperties(const std::string& serverBlock, std::map<std::string, std::string>& serverProperties);

// Extract properties of the location block.
void extractLocationProperties(const std::string& locationBlock, std::map<std::string, std::string>& locationProperties);

// Apply server-wide properties to the server instance.
void applyServerProperties(const std::map<std::string, std::string>& serverProperties);

// Apply location-specific properties to the corresponding location within the server.
void applyLocationProperties(const std::map<std::string, std::string>& locationProperties);
