#ifndef CONFIG_HPP
#define CONFIG_HPP

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
  std::string root;
  std::vector<std::string> allowed_methods;
  bool autoindex;
  std::vector<std::string> index_files;
  std::string url;

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
  int listen;
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

#endif