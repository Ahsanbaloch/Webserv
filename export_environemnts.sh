#!/bin/bash



# # # # # # cgi get
# export AUTH_TYPE="Basic QWxhZGRpbjpPcGVuU2VzYW1l" # Replace with actual value
# export CONTENT_LENGTH="0" # Replace with actual value
# export CONTENT_TYPE="text/html" # Replace with actual value
# export GATEWAY_INTERFACE="CGI/1.1"
# export PATH_INFO="/path/to/resource" # Replace with actual value
# export PATH_TRANSLATED=""
# export QUERY_STRING="name=asdf&class=10"
# export REMOTE_ADDR="192.168.1.1" # Replace with actual value
# export REMOTE_IDENT=""
# export REMOTE_USER="" # Replace with actual value if any
# export REQUEST_METHOD="GET" # Replace with actual value
# export REQUEST_URI="/path/to/resource" # Replace with actual value
# export SCRIPT_NAME="/path/to/script.cgi" # Replace with actual value
# export SERVER_NAME="localhost" # Replace with actual value
# #export SERVER_PORT="80" # Uncomment and replace with actual value
# export SERVER_PROTOCOL="HTTP/1.1" # Replace with actual value
# export SERVER_SOFTWARE="webserv"


# # # # # # cgi post

#!/bin/bash

export REQUEST_METHOD="POST"
export CONTENT_TYPE="application/x-www-form-urlencoded"
export CONTENT_LENGTH=$(echo -n "key1=value1&key2=value2" | wc -c)
export GATEWAY_INTERFACE="CGI/1.1"
export SERVER_PROTOCOL="HTTP/1.1"
export SERVER_SOFTWARE="webserv"

echo -n "key1=value1&key2=value2" | python /Users/amehrotr/Desktop/Testing_Code/WebServe_Cgi/www/cgi-bin/form.py