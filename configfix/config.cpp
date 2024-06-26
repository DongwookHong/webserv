

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define on true
#define off false

class location {
 private:
  std::string root;  //
  std::vector<std::string> allowed_methods;
  bool autoindex;                        //
  std::vector<std::string> index_files;  //
  std::string url;                       //

 public:
  location();
  void set_root(const std::string&);
  void add_method(const std::string&);
  void set_autoindex(bool);
  void add_index_file(const std::string&);
  void set_url(const std::string&);

  void print() const;
};

class Serv_config {
 private:
  int listen;                    //
  std::string server_name;       //
  int worker_connections;        //
  std::string client_body_size;  //
  std::string error_pages;       //

  std::vector<location> locations;

 public:
  void set_listen(int);
  void set_server_name(const std::string&);
  void add_location(const location&);
  void set_worker_connections(int);
  void set_client_body_size(const std::string&);
  void set_error_pages(const std::string&);

  void print() const;
};

class server {
 private:
  std::vector<Serv_config> serv_configs;

 public:
  bool open_config_file(const std::string&, std::ifstream&);
  void parse_server_block(std::istringstream&, Serv_config&);
  void parse_location_block(std::istringstream&, location&);
  bool parse_config(const std::string&, server&);
  void add_serv_config(const Serv_config&);

  void print() const;
};

location::location() : autoindex(off) {}

void location::set_root(const std::string& r) { root = r; }
void location::add_method(const std::string& method) {
  allowed_methods.push_back(method);
}
void location::set_autoindex(bool trun) { autoindex = trun; }
void location::add_index_file(const std::string& file) {
  index_files.push_back(file);
}
void location::set_url(const std::string& u) { url = u; }
void Serv_config::set_listen(int l) { listen = l; }
void Serv_config::set_server_name(const std::string& name) {
  server_name = name;
}
void Serv_config::add_location(const location& loc) {
  locations.push_back(loc);
}
void Serv_config::set_worker_connections(int wc) { worker_connections = wc; }
void Serv_config::set_client_body_size(const std::string& size) {
  client_body_size = size;
}
void Serv_config::set_error_pages(const std::string& pages) {
  error_pages = pages;
}

void server::add_serv_config(const Serv_config& config) {
  serv_configs.push_back(config);
}

bool server::open_config_file(const std::string& filename,
                              std::ifstream& config_file) {
  config_file.open(filename.c_str());
  if (!config_file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }
  return true;
}

bool server::parse_config(const std::string& filename, server& srv) {
  std::ifstream config_file;
  if (!open_config_file(filename, config_file)) {
    return false;
  }
  std::string line;
  Serv_config current_config;
  location current_location;
  bool in_location = false;

  while (std::getline(config_file, line)) {
    std::istringstream iss(line);
    std::string key, value;
    iss >> key;

    if (key == "server{") {
      continue;
    } else if (key == "location") {
      in_location = true;
      current_location = location();
      iss >> value;
      current_location.set_url(value);
    } else if (key == "}") {
      if (in_location) {
        current_config.add_location(current_location);
        in_location = false;
      } else {
        srv.add_serv_config(current_config);
        current_config = Serv_config();
      }
    } else if (in_location) {
      // parse_location_block(iss, current_location);
      if (key == "root") {
        iss >> value;
        current_location.set_root(value);
      } else if (key == "allowed_method") {
        while (iss >> value) {
          current_location.add_method(value);
        }
      } else if (key == "autoindex") {
        iss >> value;
        current_location.set_autoindex(value == "on");
      } else if (key == "index") {
        while (iss >> value) {
          current_location.add_index_file(value);
        }
      }
    } else {
      // parse_server_block(iss, current_config);
      if (key == "listen") {
        iss >> value;
        current_config.set_listen(atoi(value.c_str()));
      } else if (key == "server_name") {
        iss >> value;
        current_config.set_server_name(value);
      } else if (key == "Client_body_size") {
        iss >> value;
        current_config.set_client_body_size(value);
      } else if (key == "error_pages") {
        iss >> value;
        current_config.set_error_pages(value);
      } else if (key == "Worker_connections") {
        iss >> value;
        current_config.set_worker_connections(atoi(value.c_str()));
      }
    }
  }
  return true;
}

void location::print() const {
  std::cout << "Location URL: " << url << std::endl;
  std::cout << "Location root: " << root << std::endl;
  std::cout << "Autoindex: " << (autoindex ? "on" : "off") << std::endl;
  std::cout << "Allowed methods: ";
  for (size_t i = 0; i < allowed_methods.size(); ++i) {
    std::cout << allowed_methods[i]
              << (i < allowed_methods.size() - 1 ? ", " : "");
  }
  std::cout << std::endl;
  std::cout << "Index files: ";
  for (size_t i = 0; i < index_files.size(); ++i) {
    std::cout << index_files[i] << (i < index_files.size() - 1 ? ", " : "");
  }
  std::cout << std::endl;
}

void Serv_config::print() const {
  std::cout << "Server listen: " << listen << std::endl;
  std::cout << "Server name: " << server_name << std::endl;
  std::cout << "Client body size: " << client_body_size << std::endl;
  std::cout << "Error pages: " << error_pages << std::endl;
  std::cout << "Worker connections: " << worker_connections << std::endl;
  for (size_t i = 0; i < locations.size(); ++i) {
    locations[i].print();
  }
}

void server::print() const {
  for (size_t i = 0; i < serv_configs.size(); ++i) {
    serv_configs[i].print();
  }
}
