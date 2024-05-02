#!/usr/bin/python3

import cgi
import os
import sys

print("Status: 303")
print("Location: /upload")
print()


def create_file_from_multipart(data_file):
  """
  Parses multipart form data from a temporary file and creates a file.

  Args:
      data_file: Path to the temporary file containing multipart data.
  """
  
  # Open the temporary file in binary mode
  with open(data_file, 'rb') as f:
    # Use cgi.FieldStorage with the file pointer
    form = cgi.FieldStorage(fp=f)
  
  # Print form fields and values
#   for field in form.keys():
#     print("Field:", field)
#     print("Value:", form[field].value)
  
  # Get the file field from the form
  file_data = form['file']
  
  # Check if a file was uploaded
  if file_data.filename:
    # Get filename and create a safe path to avoid directory traversal attacks
    filename = os.path.basename(file_data.filename)
    filepath = os.path.join('./www/upload/', filename)  # Replace with your upload path # rm dot?
    
    # Open the new file in write binary mode
    with open(filepath, 'wb') as upload_file:
      upload_file.write(file_data.file.read())
    

# Get the temporary file path from the script argument
data_file = sys.argv[1] # SCRIPT_FILENAME is a CGI environment variable

if data_file:
  # Call the function to process the data
  create_file_from_multipart(data_file)
else:
  print("Error: No temporary file path provided.")