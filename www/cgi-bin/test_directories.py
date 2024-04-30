# Step 1: Create a simple Python script to serve the content.
import os
import http.server
import socketserver
from urllib.parse import unquote

# Define the handler class
class DirectoryListingHandler(http.server.SimpleHTTPRequestHandler):
    def list_directory(self, path):
        try:
            # Decode the path to handle URL encoding
            path = unquote(path)
            
            # Get the list of files in the specified directory
            entries = os.listdir(path)
            entries.sort(key=lambda a: a.lower())
            
            # Generate the HTML content
            output = "<html><head><title>Directory listing</title></head>"
            output += "<body><h1>Directory listing for {}</h1><ul>".format(self.path)
            
            # Add each file or directory as a hyperlink
            for entry in entries:
                full_path = os.path.join(path, entry)
                display_name = entry + "/" if os.path.isdir(full_path) else entry
                link = self.path + "/" + entry if self.path != "/" else "/" + entry
                output += '<li><a href="{}">{}</a></li>'.format(link, display_name)
            
            output += "</ul></body></html>"
            # Encode the output as bytes
            encoded_output = output.encode("utf-8")
            
            # Set the content-type header to HTML
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.send_header("Content-Length", len(encoded_output))
            self.end_headers()
            
            # Write the content to the response
            self.wfile.write(encoded_output)
            return None
        except OSError:
            self.send_error(404, "Directory not found")
            return None

# Step 2: Define the port and create the server
PORT = 9000  # You can change this to any port number

with socketserver.TCPServer(("", PORT), DirectoryListingHandler) as httpd:
    print(f"Serving at port {PORT}")
    httpd.serve_forever()
