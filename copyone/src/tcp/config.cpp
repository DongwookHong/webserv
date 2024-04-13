

struct config_s {
  std::string root;
  std::vector<std::string> allowed_methods;
  bool autoindex;
  std::vector<std::string> index_files;
  std::string url;
} config_t;

struct servconfig_s {
  int listen;
  std::string server_name;
  int worker_connections;
  std::string client_body_size;
  std::string error_pages;
  std::vector<config_s> locations;
} servconfig_t;

struct serverlist_s {
  std::vector<servconfig_s> serv_configs;
} serverlist_t;

void set_root(config_t& loc, const std::string& root) { loc.root = root; }
void add_method(config_t& loc, const std::string& method) {
  loc.allowed_methods.push_back(method);
}

void set_autoindex(config_t& loc, bool autoindex) { loc.autoindex = autoindex; }

void add_index_file(config_t& loc, const std::string& file) {
  loc.index_files.push_back(file);
}

void set_url(config_t& loc, const std::string& url) { loc.url = url; }

void set_listen(ServConfig& serv, int listen) { serv.listen = listen; }

void set_server_name(ServConfig& serv, const std::string& name) {
  serv.server_name = name;
}

void add_location(ServConfig& serv, const config_t& loc) {
  serv.locations.push_back(loc);
}

void set_worker_connections(ServConfig& serv, int wc) {
  serv.worker_connections = wc;
}

void set_client_body_size(ServConfig& serv, const std::string& size) {
  serv.client_body_size = size;
}

void set_error_pages(ServConfig& serv, const std::string& pages) {
  serv.error_pages = pages;
}

void add_serv_config(Server& srv, const ServConfig& config) {
  srv.serv_configs.push_back(config);
}

bool open_config_file(const std::string& filename, std::ifstream& config_file) {
  config_file.open(filename.c_str());
  if (!config_file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }
  return true;
}

bool parse_config(const std::string& filename, Server& srv) {
  std::ifstream config_file;
  if (!open_config_file(filename, config_file)) {
    return false;
  }

  std::string line;
  ServConfig current_config;
  config_t current_location;
  bool in_location = false;

  while (std::getline(config_file, line)) {
    std::istringstream iss(line);
    std::string key, value;
    iss >> key;

    if (key == "server{") {
      continue;
    } else if (key == "location") {
      in_location = true;
      current_location = config_t();
      iss >> value;
      set_url(current_location, value);
    } else if (key == "}") {
      if (in_location) {
        add_location(current_config, current_location);
        in_location = false;
      } else {
        add_serv_config(srv, current_config);
        current_config = ServConfig();
      }
    } else if (in_location) {
      if (key == "root") {
        iss >> value;
        set_root(current_location, value);
      } else if (key == "allowed_method") {
        while (iss >> value) {
          add_method(current_location, value);
        }
      } else if (key == "autoindex") {
        set_autoindex(current_location, value == "on");
      } else if (key == "index") {
        while (iss >> value) {
          add_index_file(current_location, value);
        }
      }
    } else {
      if (key == "listen") {
        iss >> value;
        set_listen(current_config, std::atoi(value.c_str()));
      } else if (key == "server_name") {
        iss >> value;
        set_server_name(current_config, value);
      } else if (key == "Client_body_size") {
        iss >> value;
        set_client_body_size(current_config, value);
      } else if (key == "error_pages") {
        iss >> value;
        set_error_pages(current_config, value);
      } else if (key == "Worker_connections") {
        iss >> value;
        set_worker_connections(current_config, std::atoi(value.c_str()));
      }
    }
  }
  return true;
}

void print_location(const config_t& loc) {
  std::cout << "config_t URL: " << loc.url << std::endl;
  std::cout << "Root: " << loc.root << std::endl;
  std::cout << "Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
  std::cout << "Allowed Methods: ";
  for (const auto& method : loc.allowed_methods) {
    std::cout << method << ", ";
  }
  std::cout << std::endl;
  std::cout << "Index Files: ";
  for (const auto& file : loc.index_files) {
    std::cout << file << ", ";
  }
  std::cout << std::endl;
}

void print_serv_config(const ServConfig& serv) {
  std::cout << "Server Listen: " << serv.listen << std::endl;
  std::cout << "Server Name: " << serv.server_name << std::endl;
  std::cout << "Worker Connections: " << serv.worker_connections << std::endl;
  std::cout << "Client Body Size: " << serv.client_body_size << std::endl;
  std::cout << "Error Pages: " << serv.error_pages << std::endl;
  for (const auto& loc : serv.locations) {
    print_location(loc);
  }
}

void print_server(const Server& srv) {
  for (const auto& config : srv.serv_configs) {
    print_serv_config(config);
  }
}
