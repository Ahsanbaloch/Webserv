#!/usr/bin/python3

print("""
<body>
    <!-- Green heading with text "GeeksforGeeks" -->
    <h1>GeeksforGeeks</h1>
    <!-- Form with method 'post' and action 'form.py' -->
    <form method='post' action='form.py' enctype='multipart/form-data'>
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
