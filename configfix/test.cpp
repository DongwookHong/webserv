#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define on true
#define off false

typedef struct Location_t {
  std::string root;
  std::vector<std::string> allowedMethods;
  bool autoindex;
  std::vector<std::string> indexFiles;
  std::string url;

  Location_t() : autoindex(off) {}  // Constructor
} Location_t;

typedef struct ServConfig {
  int listen;
  std::string serverName;
  int workerConnections;
  std::string clientBodySize;
  std::string errorPages;
  std::vector<Location_t> locations;
};

typedef struct Server {
  std::vector<ServConfig> servConfigs;

  bool parseConfig(const std::string& filename);
  void print() const;
};

void setLocationRoot(Location_t& loc, const std::string& root) {
  loc.root = root;
}

void addLocationMethod(Location_t& loc, const std::string& method) {
  loc.allowedMethods.push_back(method);
}

void setLocationAutoindex(Location_t& loc, bool autoindex) {
  loc.autoindex = autoindex;
}

void addLocationIndexFile(Location_t& loc, const std::string& file) {
  loc.indexFiles.push_back(file);
}

void setLocationUrl(Location_t& loc, const std::string& url) { loc.url = url; }

void setServConfigListen(ServConfig& serv, int listen) { serv.listen = listen; }

void setServConfigServerName(ServConfig& serv, const std::string& name) {
  serv.serverName = name;
}

void addServConfigLocation(ServConfig& serv, const Location_t& loc) {
  serv.locations.push_back(loc);
}

void setServConfigWorkerConnections(ServConfig& serv, int wc) {
  serv.workerConnections = wc;
}

void setServConfigClientBodySize(ServConfig& serv, const std::string& size) {
  serv.clientBodySize = size;
}

void setServConfigErrorPages(ServConfig& serv, const std::string& pages) {
  serv.errorPages = pages;
}

// bool Server::openConfigFile(const std::string& filename,
//                             std::ifstream& configFile) {
//   configFile.open(filename.c_str());
//   if (!configFile.is_open()) {
//     std::cerr << "Failed to open file: " << filename << std::endl;
//     return false;
//   }
//   return true;
// }

bool Server::parseConfig(const std::string& filename) {
  std::ifstream configFile(filename.c_str());
  if (!configFile.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }

  ServConfig currentConfig;
  Location_t currentLocation;
  bool inLocation = false;
  std::string line;

  while (std::getline(configFile, line)) {
    std::istringstream iss(line);
    std::string key;
    iss >> key;

    if (key == "server{") {
      currentConfig = ServConfig();
    } else if (key == "location") {
      inLocation = true;
      currentLocation = Location_t();
      iss >> currentLocation.url;
    } else if (key == "}") {
      if (inLocation) {
        inLocation = false;
        currentConfig.locations.push_back(currentLocation);
      } else {
        servConfigs.push_back(currentConfig);
      }
    } else if (inLocation) {
      if (key == "root") {
        iss >> currentLocation.root;
      } else if (key == "allowed_method") {
        std::string method;
        while (iss >> method) {
          currentLocation.allowedMethods.push_back(method);
        }
      } else if (key == "autoindex") {
        std::string value;
        iss >> value;
        currentLocation.autoindex = (value == "on");
      } else if (key == "index") {
        std::string indexFile;
        while (iss >> indexFile) {
          currentLocation.indexFiles.push_back(indexFile);
        }
      }
    } else {
      if (key == "listen") {
        iss >> currentConfig.listen;
      } else if (key == "server_name") {
        iss >> currentConfig.serverName;
      } else if (key == "Client_body_size") {
        iss >> currentConfig.clientBodySize;
      } else if (key == "error_pages") {
        iss >> currentConfig.errorPages;
      } else if (key == "Worker_connections") {
        iss >> currentConfig.workerConnections;
      }
    }
  }

  return true;
}

void Server::print() const {
  for (size_t i = 0; i < servConfigs.size(); ++i) {
    const ServConfig& config = servConfigs[i];
    std::cout << "Server listen: " << config.listen << "\n"
              << "Server name: " << config.serverName << "\n"
              << "Client body size: " << config.clientBodySize << "\n"
              << "Error pages: " << config.errorPages << "\n"
              << "Worker connections: " << config.workerConnections << "\n";
    for (const Location_t& loc : config.locations) {
      std::cout << "Location_t URL: " << loc.url << "\n"
                << "Location_t root: " << loc.root << "\n"
                << "Autoindex: " << (loc.autoindex ? "on" : "off") << "\n"
                << "Allowed methods: ";
      for (const std::string& method : loc.allowedMethods) {
        std::cout << method << " ";
      }
      std::cout << "\nIndex files: ";
      for (const std::string& file : loc.indexFiles) {
        std::cout << file << " ";
      }
      std::cout << "\n--------------------------------\n";
    }
  }
}

int main() {
  Server myServer;
  if (myServer.parseConfig("default1.config")) {
    myServer.print();
  } else {
    std::cerr << "Error parsing configuration file." << std::endl;
  }
  return 0;
}