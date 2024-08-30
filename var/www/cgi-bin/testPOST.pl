#!/usr/bin/perl

use strict;
use warnings;
use CGI;

sub handle_post {
    my $cgi = CGI->new;

    # Read the POST data
    my %params = $cgi->Vars;

    # Prepare the response body
    my $response_body = "<html><head><title>POST Request</title></head><body>";
    $response_body .= "<h1>Received POST data</h1>";
    $response_body .= "<ul>";

    # Iterate over all POST parameters
    foreach my $key (sort keys %params) {
        my $value = $params{$key};
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
	handle_post();
}

main();
