#!/usr/bin/python3

print("Content-Type: text/html\n\n")

print("""
<body>
    <!-- Green heading with text "GeeksforGeeks" -->
    <h1>GeeksforGeeks</h1>
    <!-- Form with method 'post' and action 'form.py' -->
    <form method='post' action='form_file_path.py' enctype='application/x-www-form-urlencoded'>
        <!-- Name input field -->
        <p><label>Name:</label> <input type="text" name="name" /></p>
        <!-- Class input field -->
        <p><label>Class:</label> <input type="number" name="class" /></p>
        <!-- Submit button -->
        <input type='submit' value='Submit' />
    </form>
</body>
</html>
""")
