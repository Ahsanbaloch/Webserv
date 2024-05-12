# Webserv

This is a basic web server created from scratch in C++98. It was developed as part of the curriculum at 42. The project emphasizes a deep understanding of the HTTP protocol, networking, and the CGI (Common Gateway Interface) standard.

### Features
- **HTTP/1.1 Protocol Support**: Compliant with the HTTP/1.1 protocol, handling GET, POST, and DELETE requests
- **Cross-Platform Compatibility**: Developed to work on Linux and macOS environments building on the scalable event interfaces epoll and kqueue, respectively
- **Concurrent Connections**: Handle mutliple incoming connections, stress-tested with [siege](https://github.com/JoeDog/siege)
- **Static Content**: Efficiently serve static files (HTML, CSS, etc.)
- **Dynamic Content**: Execute CGI scripts, enabling dynamic content generation
- **Upload Module**: Dedicated module to enable faster uploads of multipart/form-data and application/x-www-form-urlencoded data
- **Configuration File**: Utilize a configuration file to specify server settings such as port number, server name, document root, etc.
- **Error Handling**: Provides informative error messages for various scenarios, ensuring robustness and user-friendliness
- **Security**: Implements basic security measures to prevent common attacks such as directory traversal, and HTTP request smuggling

### Installation

1. Clone the repository:
```
git clone https://github.com/Ahsanbaloch/Webserv.git
```

2. Build the project:
```
cd webserv
make
```

### Usage

1. To start the server, run:
```
./webserv <config_file>
```
Replace <config_file> with the path to your configuration file. If no file is specified, the server will look for "simple.conf" in the config_files directory.


2. Send HTTP requests to the server using your favorite web browser (oprimzed for Chrome) or a tool such as Postman. For CGI, ensure that the scripting language of your choice is installed (tested with .py and .php scripts).
   
3. A demo of the main functionalities can be accessed via the default configuration file and typing following address in your browser:
```
http://localhost:4040/
```

### Architectural Overview

![Webserv_Architecture](https://github.com/Ahsanbaloch/Webserv/assets/65039082/e02f5e8f-abde-41aa-9734-6544b4dfbb21)


