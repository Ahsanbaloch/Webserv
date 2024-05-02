#!/usr/bin/php
<?php
echo "Content-Type: text/html\n\n";
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
if (!isset($argv[1])) {
    die("Error: No data file path provided.");
}

// Path to the binary file
$file_path = $argv[1];

// Read the file into a string
$data = file_get_contents($file_path);

$boundary = substr($data, 0, strpos($data, "\r\n"));
$parts = array_slice(explode($boundary, $data), 1);
$form_data = [];

foreach ($parts as $part) {
    // If this is the last part, break
    if ($part == "--\r\n") break;

    // Separate the headers from the body
    $part = ltrim($part, "\r\n");
    list($raw_headers, $body) = explode("\r\n\r\n", $part, 2);

    // Parse the headers
    $raw_headers = explode("\r\n", $raw_headers);
    $headers = [];
    foreach ($raw_headers as $header) {
        list($name, $value) = explode(': ', $header);
        $headers[strtolower($name)] = $value;
    }

    // Parse the Content-Disposition header
    $filename = null;
    $name = null;
    if (isset($headers['content-disposition'])) {
        preg_match(
            '/^form-data; *name="([^"]+)"(?:; *filename="([^"]+)")?/',
            $headers['content-disposition'],
            $matches
        );
        $name = $matches[1];
        if (isset($matches[2])) $filename = $matches[2];
    }

    // Add the data to the array
    if ($name != null) {
        $form_data[$name] = $body;
    }
}

// Get the form data
$name = $form_data['name'] ?? "Not provided";
$email = $form_data["email"] ?? "Not provided";

// Start of the HTML page with Bootstrap styling
echo <<<HTML
<html>
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
				<a class="nav-link" href="/cgi_form_py.html">CGI Form (py)</a>
			  </li>
			  <li class="nav-item">
				<a class="nav-link active" aria-current="page" href="/cgi_form_php.html">CGI Form (php)</a>
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
				<p><strong>Name:</strong> $name</p>
				<p><strong>Email:</strong> $email</p>
				
			</div>
		</div>
	</div>
</body>
</html>
HTML;
?>