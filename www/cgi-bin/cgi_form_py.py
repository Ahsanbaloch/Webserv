#!/usr/bin/python3
import cgi
import os
import base64
import sys
import shutil

def main():
    
    form = cgi.FieldStorage()


    print("Content-Type:text/html\r\n\r\n")
    
    print("""<html>
    <head>
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    </head>
    <body>
    <nav class="navbar navbar-expand-lg navbar-light bg-primary">
        <div class="container-fluid">
            <div class="collapse navbar-collapse" id="navbarSupportedContent">
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
						<a class="nav-link" href="/avatar_form.html">CGI Avatar (py)</a>
					</li>
					<li class="nav-item">
						<a class="nav-link active" aria-current="page" href="/cgi_form_py.html">CGI Form (py)</a>
					</li>
					<li class="nav-item">
						<a class="nav-link" href="/cgi_form_php.html">CGI Form (php)</a>
					</li>
					<li class="nav-item">
						<a class="nav-link" href="/cgi-bin/my_files.py">My Files</a>
					</li>
                </ul>
            </div>
        </div>
    </nav>

        <div class="container mt-5">
            <div class="card mx-auto" style="width: 24rem;">
                <div class="card-body">
                    <h3 class="card-title">Profile Data Received</h3>
    """)

    if form.getvalue("name"):
        name = form.getvalue("name")
        print("<p><strong>Name:</strong> {}</p>".format(name))
    else:
        print("<p><strong>Name:</strong> Not provided</p>")

    if form.getvalue("email"):
        email = form.getvalue("email")
        print("<p><strong>Email:</strong> {}</p>".format(email))
    else:
        print("<p><strong>Email:</strong> Not provided</p>")

    

    print("""
                </div>
            </div>
        </div>
    </body>
    </html>
    """)

if __name__ == "__main__":
    main()