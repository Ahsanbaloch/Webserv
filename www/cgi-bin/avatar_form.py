#!/usr/bin/python3
import cgi
import os
import sys

# HTTP header indicating content type
print("Content-Type: text/html\n\n")

# Function to process the form data and render the HTML
def Show_Data(data_file):
    with open(data_file, 'rb') as f:
        # Create a FieldStorage object using the file pointer
        form = cgi.FieldStorage(fp=f)

        # Start of the HTML page with Bootstrap styling
        print("""<html>
<head>
    <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
</head>
<body>
<nav class="navbar navbar-expand-lg navbar-light bg-primary">
    <div class="container-fluid">
        <div class="collapse navbar-collapse" id="navbarSupportedContent">
            <ul class="navbar-nav mx-auto mb-2 mb-lg-0">
                <ul class="navbar-nav mx-auto mb-2 mb-lg-0">      
			<li class="nav-item">
				<a class="nav-link" href="/home.html">Home</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/form.html">Form</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/upload_file.html">Upload File</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/upload_file_cgi.html">CGI Upload File</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link active" aria-current="page" href="/avatar_form.html">CGI Avatar (py)</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/cgi_form_py.html">CGI Form (py)</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/cgi_form_php.html">CGI Form (php)</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link" href="/cgi-bin/my_files.py">My Files</a>
			  </li>
        </ul>
            </ul>
        </div>
    </div>
</nav>

    <div class="container mt-5">
        <div class="card mx-auto" style="width: 24rem;">
            <div class="card-body">
                <h3 class="card-title">Profile Data Received</h3>
""")

        # Displaying data from the form with bold labels
        if form.getvalue("name"):
            name = form.getvalue("name")
            print(f"<p><strong>Name:</strong> {name}</p>")
        else:
            print("<p><strong>Name:</strong> Not provided</p>")

        if form.getvalue("email"):
            email = form.getvalue("email")
            print(f"<p><strong>Email:</strong> {email}</p>")
        else:
            print("<p><strong>Email:</strong> Not provided</p>")

        # Handling file uploads
        if "profile-picture" in form:
            fileitem = form["profile-picture"]
            # print("<p>Image : {}</p>".format(fileitem))
            if fileitem.filename:
                # Securely get the file name and set the file path
                filename = os.path.basename(fileitem.filename)
                filepath = os.path.join('www/upload/', filename)
                webpath = os.path.join('../upload/', filename)

                # Write the file content to the specified location
                with open(filepath, 'wb') as upload_file:
                    upload_file.write(fileitem.file.read())

                # Feedback message for successful upload
                # message = f'The file <strong> "{filename}" </strong> was uploaded successfully'
                # print(f"<p>{message}</p>")
                print(f"<div class='d-flex justify-content-center'><img src='{webpath}' alt='Uploaded image' class='img-fluid' style='max-width: 200px;' /></div>")
            else:
                print("<p><strong>No file was uploaded</strong></p>")
        else:
            print("<p><strong>No file was uploaded</strong></p>")

        # Closing the card and HTML content
        print("""
            </div>
        </div>
    </div>
</body>
</html>
""")

# Fetch the data file from script arguments
data_file = sys.argv[1]

if data_file:
    # Process the form data and generate the HTML response
    Show_Data(data_file)
else:
    print("Error: No data file path provided.")