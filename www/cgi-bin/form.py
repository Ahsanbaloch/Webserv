#!/usr/bin/python
print("""
<!DOCTYPE html>
<html>
<head>
</head>
<body>

<form method="post" enctype="application/x-www-form-urlencoded">
  <label for="fname">First name:</label><br>
  <input type="text" id="fname" name="fname"><br>
  <label for="lname">Last name:</label><br>
  <input type="text" id="lname" name="lname"><br>
  <input type="submit" value="Submit">
</form>

<div id="displayArea"></div>

</body>
</html>
""")