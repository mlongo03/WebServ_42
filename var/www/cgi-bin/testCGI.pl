#!/usr/bin/perl

use strict;
use warnings;
use CGI;

sub handle_get {
    my $cgi = CGI->new;

    print $cgi->header('text/html');
    print "<h1>Received GET data</h1>";

    my %params = $cgi->Vars;
    foreach my $key (sort keys %params) {
        print "<p><strong>$key</strong>: $params{$key}</p>";
    }
}

sub handle_post {
    my $cgi = CGI->new;

    print $cgi->header('text/html');
    print "<h1>Received POST data</h1>";

    my %params = $cgi->Vars;
    foreach my $key (sort keys %params) {
        print "<p><strong>$key</strong>: $params{$key}</p>";
    }
}

sub main {
    my $cgi = CGI->new;
    my $method = $cgi->request_method();

    if ($method eq 'GET') {
        handle_get();
    } elsif ($method eq 'POST') {
        handle_post();
    } else {
        print $cgi->header('text/html');
        print "<h1>405 Method Not Allowed</h1>";
    }
}

main();
