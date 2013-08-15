# other modules required 
* entity api module is required.
* votingapi module is required.
* include/patternentity.admin.inc row 53-64 need patterns, patterns_xmlparser, patterns_yamlparser. 
	* rewrite these 12 rows can make patternentity module Independent of patterns, patterns_xmlparser, patterns_yamlparser; but instead, patternentity will depend on libraries module and the validation code on the existence of spyc&codomirror library is needed to add.
* token is actually required by Patterns module, it's not required by this module.

# reason for use votingapi
* first I can't find a module about vote quick fit our situation.
* votingapi supply a framework, smart framework. e.g. 
	* one user can vote again after two days. 
	* allow anomymous votes to vote, but forbiden too Frequently vote from the same computer.
	* wo can count how many times a user totally voted, or how many times the user voted to a centain category.

# about comments
* reply module works fine with patternentity. (though the appearance needs some improvement.)
* commentfield don't meet our requirement, because the comment field will show in the admin ui page instead of pattern view page.

# taxonomy problem
* though patternentity is already a entity type, taxonomy can't work with it. it's because that taxonomy only works with node(content type) entity, but not all kinds of entities. (hardcode int taxonomy's source code, just like comments module-only works with node.)

# how to use
* install patternentity module.
* build the folder 'sites/default/files/patternentity', make sure drupal can read and write this folder. (this folder is used to store all uploaded yaml/xml pattern file.)
* uploader some yaml/xml file in 'admin/structure/patternentity'.
* try all kinds of links.
* manage field on page 'admin/structure/patternentity/fields'

# about permission
* only people having 'adminster patternentity' permission can access the admin page "admin/structure/patternentity".
* people having 'upload patternentity' permission can upload pattern file, that means they can access "admin/structure/patternentity/add" page.
* people having 'view patternentity' permission can view all related patternentity page.
