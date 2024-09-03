#!/usr/bin/perl

use strict;
use warnings;
# use CGI;

sub handle_get {
    # my $cgi = CGI->new;

    # Prepare the response body
    my $response_body = "<html><head><title>GET Request</title></head><body>";
    $response_body .= "<h1>Received GET data</h1>";
    $response_body .= "<ul>";

    # Iterate over all environment variables
    foreach my $key (sort keys %ENV) {
        my $value = $ENV{$key};
        $response_body .= "<li><strong>$key</strong>: $value</li>";
    }

    $response_body .= "</ul>";
    $response_body .= "</body></html>";

    # Calculate the content length of the response body
    my $response_content_length = length($response_body);

    # Set up headers for the response
    print "Status: 200 OK\r\n";
    print "Content-Type: text/html\r\n";
    print "Content-Length: $response_content_length\r\n";
    print "\r\n";

    # Output the response body
    print $response_body;
}

sub main {
    # Check if the request method is GET
    my $method = $ENV{'REQUEST_METHOD'} || 'GET';
    if ($method eq 'GET') {
        handle_get();
    } else {
        # Set up headers for the response
        my $error_message = "<html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed</h1></body></html>";
        print "Status: 405 Method Not Allowed\r\n";
        print "Content-Type: text/html\r\n";
        print "Content-Length: " . length($error_message) . "\r\n";
        print "\r\n";
        # Output the error message
        print $error_message;
    }
}

main();
