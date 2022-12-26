#ifndef REST_UTILS_H_INCLUDED
#define REST_UTILS_H_INCLUDED

#include <nlohmann/json.hpp>
#include <restbed>

/* Service information */
const char *HOST = "localhost";
const int PORT = 1234;  // Choose your own port number > 1024

const std::shared_ptr<restbed::Request> createRequest(
    const std::string endpoint) {
  // Create the URI string
  std::string uri_str;
  uri_str.append("http://");
  uri_str.append(HOST);
  uri_str.append(":");
  uri_str.append(std::to_string(PORT));
  uri_str.append("/gofish/");
  uri_str.append(endpoint);

  // Configure request headers
  auto request = std::make_shared<restbed::Request>(restbed::Uri(uri_str));
  request->set_method("GET");
  return request;
}

std::string handle_response(std::shared_ptr<restbed::Response> response) {
  int status_code = response->get_status_code();
  auto length = response->get_header("Content-Length", 0);

  switch (status_code) {
    case 200: {
      restbed::Http::fetch(length, response);
      std::string responseStr(
          reinterpret_cast<char*>(response->get_body().data()), length);
      return responseStr;
    }
    case 400: {
      restbed::Http::fetch(length, response);
      char error[100];
      sprintf(error, "Invalid request: %.*s\n", length,
              response->get_body().data());

      nlohmann::json jsonResponse = { error };
      return jsonResponse.dump();
    }
    default:
      fprintf(
          stderr,
          "An error occurred with the service. (Is the service running?)\n");
      break;
  }

  // Should not be reached
  nlohmann::json jsonResponse = { };
  return jsonResponse.dump();
}

#endif
