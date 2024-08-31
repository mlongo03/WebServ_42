<?php
#!/usr/bin/php-cgi
<?php
// testGET.php

// Check if there are any GET parameters
if (!empty($_GET)) {
    echo "<h1>GET Request Parameters</h1>";
    echo "<ul>";
    // Loop through each GET parameter and display it
    foreach ($_GET as $key => $value) {
        echo "<li><strong>" . htmlspecialchars($key) . ":</strong> " . htmlspecialchars($value) . "</li>";
    }
    echo "</ul>";
} else {
    echo "<h1>No GET parameters found</h1>";
}
?>
