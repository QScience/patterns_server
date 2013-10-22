/*
 * @file
 */
(function($) {
  $(document).ready(function() {

    var formatForTree = function(data) {
      var iter,
        result = [],
        curr,
        count = 0;

      for (iter in data) {
        curr = {
          id: count,
          name: iter,
          author: data[iter].user,
          d2did: data.d2did,
          patternId: iter,
          children: formatForTree(data[iter].children),
        };

        result.push(curr);
        count++;
      }

      return result;
    };

    //pattern files tree
    var POST_URL = Drupal.settings.basePath + (Drupal.settings.clean_url === "1" ? '' : '?q=') + 'patterns_server/get_random_history_tree';
    console.log(POST_URL);
    $.ajax({
      url: POST_URL,
      success: function(data) {
        if (data.success) {
          var treeData = formatForTree(data.tree);
          d3Tree.build(treeData, '#patterns_server-patterns-tree-visualization');
        } else {
          alert('Server error.');
        }
      },
      error: function(jqXHR, exception) {
        if (jqXHR.status == 403) {}
      }
    });

  });
})(jQuery);