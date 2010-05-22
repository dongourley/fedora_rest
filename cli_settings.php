<?php
// $Id$

/**
 * @file
 * Fedora_rest CLI import program configuration file.
 *
 * IMPORTANT: these settings are only used by the CLI program; when importing
 *  through the web admin interface make settings on the Fedora API admin page.
 *
 */

// Drupal Site specific variables; these are required
$site_url = 'http://localhost';
$site_directory= '/var/www/html';

// Default values; these can be overridden by command line args
// specify an ID for the set of objects being imported
$any_collection = 'default:anyCollection';

// NB: all other CLI options are now set in the fedora_rest admin web interface
