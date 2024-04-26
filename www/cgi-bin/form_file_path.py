#!/usr/bin/python3

# import sys

# # Read everything from standard input
# data = sys.stdin.read()

# # Print a line of hashtags, the data, and another line of hashtags to the standard error output
# sys.stderr.write("##########\n" + data + "\n##########\n")


# import cgi

# form = cgi.FieldStorage()

# first_name = form.getvalue("fname") if "fname" in form else "Default first name"
# last_name  = form.getvalue("lname") if "lname" in form else "Default last name"

# print("""
# <!DOCTYPE html>
# <html>
# <head>
# </head>
# <body>

#   <label for="fname">First name:</label><br>""")
# print(first_name)
# print("""<label for="lname">Last name:</label><br>""")
# print(last_name)
# print("""<input type="submit" value="Submit">
# </form>

# <div id="displayArea"></div>

# </body>
# </html>
# """)

#!/usr/bin/env python
# Importing the 'cgi' module
import sys, cgi, cgitb

# Your existing code here...
temp_file_path = sys.argv[1]

# Send an HTTP header indicating the content type as HTML
print("Content-Type: text/html\n\n")

# Start an HTML document with center-aligned content
print("<html><body style='text-align:center;'>")

# Display a green heading with text "GeeksforGeeks"
print("<h1 style='color: green;'>GeeksforGeeks</h1>")

# Parse form data submitted via the CGI script
print("<p>CGI trying to get data</p>")
form = cgi.FieldStorage(fp=open(temp_file_path, 'rb'))
print("<p>CGI got data</p>")

# Check if the "name" field is present in the form data
#if form.getvalue("name"):
# #if "name" in form:
#     # If present, retrieve the value and display a personalized greeting
# #name = form['name'].value
name = form.getvalue("name")
# if form.getvalue("class"):
class_name = form.getvalue("class")
print("<p>CGI got name</p>")
print("<h2>Hello, " + name + "!</h2>")
print(class_name)
print("<p>Thank you for using our script.</p>")
#else:
print("<p>No name provided in the form data.</p>")

# Check if the "happy" checkbox is selected
# if form.getvalue("happy"):
#     # If selected, display a message with a happy emoji
#     print("<p>Yayy! We're happy too! ????</p>")

# # Check if the "sad" checkbox is selected
# if form.getvalue("sad"):
#     # If selected, display a message with a sad emoji
#     print("<p>Oh no! Why are you sad? ????</p>")

# Close the HTML document
print("</body></html>")


# #!/usr/bin/env python

# import cgi
# import cgitb
# cgitb.enable()

# print("Content-Type: text/html")    # HTML is following
# print()                             # blank line, end of headers

# form = cgi.FieldStorage()

# print("<html><body>")
# print("<p>POST data:</p>")
# for key in form.keys():
#     print("<p>", key, "=", form[key].value, "</p>")
# print("</body></html>")