IP Deny By Country
===============

Allows you to deny access to your site to visitors from certain countries or, conversely,
allow access to your site only to visitors from certain countries.

This module uses country based IP zone files provided by [IPdeny](https://www.ipdeny.com/) to minimize online fraud,
SPAM, flooding and sometimes brute force attacks.

Installation
------------

Install this module using the official Backdrop CMS instructions at
https://backdropcms.org/guide/modules

Configuration
-------------

Configuration page is available from the *Administration > Configuration >
User accounts > IP Deny By Country* menu (admin/config/people/ip-deny-country).

On this page you can:

- select the  type of access restriction: allow or deny access from selected countries;
- select countries for your restrictions;
- select how often the data files for the countries should be updated;
- select the return code to display to the unwanted site visitor;
- enable logging of denied access attempts.

If you have enabled logging of denied access attempts, then you can quickly review
an individual dblog entry (admin/reports/event/EVENT_NUMBER) and check actual status of
rejected IP in [AbuseIPDB](https://www.abuseipdb.com/) from the link provided.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainer
------------------

Vladimir (https://github.com/findlabnet/)

Credits
------------------

Powered by IPDENY.COM IP database (http://www.ipdeny.com).  [Copyright Notice](https://www.ipdeny.com/copyright.php)

The 'includes/countries.php' file is taken from https://stefangabos.github.io/world_countries/ where it is published
under the GNU LESSER GENERAL PUBLIC LICENSE Version 3.

Service from http://www.geoplugin.net/json.gp is used to get the country information
of the currently logged in user to avoid self-blocking.

Issues
------

For bug reports, feature or support requests, please use the module
issue queue at https://github.com/backdrop-contrib/ip_deny_country/issues.
