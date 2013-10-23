-- SUMMARY --

Patterns Server is a module that extends Patterns allowing you to set up a "hub" for sharing pattern files.
Version 7.x-2.x store the patterns as entities.
It relies on D2D to exchange the information in a distributed and secure way.

-- REQUIREMENTS --

* Patterns: https://drupal.org/project/patterns
* D2D: https://drupal.org/sandbox/shakty/1889370
* Entity API: https://drupal.org/project/entity
* Voting API: https://drupal.org/project/votingapi

-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

-- USE --
* Create a 'patternentity' folder in your default folder (i.e.:lder 'sites/default/files/patternentity')
  and ensure Drupal has permissions to read and write on it.
* Visit admin/patterns_server to upload some patterns.

-- PERMISSIONS --
* Users with 'administer patternentity' permission have full administration access (via "admin/patterns_server").
* Users with 'upload patternentity' permission can upload pattern files,  accessing "admin/patterns_server/add".
* Users with 'view patternentity' permission can view the main patterns server page.

 -- Demo Files --

 In the visualization folder, there you can find some demo of how to use the visualization mechanism. (Check index.html for a working example.)
