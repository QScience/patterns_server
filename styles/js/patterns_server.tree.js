/*
 * @file
 */
(function($){
$(document).ready(function() {

  //pattern files tree
  var POST_URL = Drupal.settings.basePath + (Drupal.settings.clean_url === "1" ? '' : '?q=') + 'patterns_server/get_random_history_tree';
  console.log(POST_URL);
  $.ajax({
    url: POST_URL,
    success: function(data) {
      console.log(data);
    },
    error: function(jqXHR, exception) {
      if (jqXHR.status == 403) {
      }
    }
  });

});
})(jQuery)
