#!/usr/bin/php
<?php
// testGET.php

$response_body = "<html><head><title>My CGI Script</title></head><body><h1>GET REQUEST</h1></body></html>";

// Add the environment variables to the response body
$response_body .= "<h2>Environment Variables of the cgi get script </h2>";
$response_body .= "<ul>";
foreach (array_keys(getenv()) as $key) {
    $value = getenv($key);
    $response_body .= "<li><strong>" . htmlspecialchars($key) . ":</strong> " . htmlspecialchars($value) . "</li>";
}
$response_body .= "</ul>";

echo "Status: 200 OK\r\n";
echo "Content-Type: text/html\r\n";
echo "Content-Length: " . strlen($response_body) . "\r\n\r\n";
echo $response_body;
?>
