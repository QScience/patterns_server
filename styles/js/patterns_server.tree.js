/*
 * @file
 */
(function($) {
  $(document).ready(function() {

    var formatForTree = function(data, child) {
      var iter,
        result = [],
        curr,
        count = 0,
        startUrl = Drupal.settings.basePath + (Drupal.settings.clean_url === "1" ? '' : '?q=');

      for (iter in data) {
        curr = {
          id: count,
          name: data[iter].title,
          author: data[iter].author,
          category: data[iter].category,
          author_link: startUrl + 'user/' + data[iter].user,
          d2did_link: data[iter].host,
          pattern_link: startUrl + 'pattern/' + data[iter].d2did,
          children: formatForTree(data[iter].children, true),
        };
        if (!!child || curr.children.length > 0) {
          result.push(curr);
          count++;
        }
      }

      return result;
    };

    //pattern files tree
    var POST_URL = Drupal.settings.basePath + (Drupal.settings.clean_url === "1" ? '' : '?q=') + 'patterns_server/get_random_history_tree';

    $.ajax({
      url: POST_URL,
      success: function(data) {
        if (data.success) {
          var treeData = formatForTree(data.tree);
          d3Tree.build(treeData, '#patterns_server-patterns-tree-visualization');
        } else {
          jQuery('#patterns_server-patterns-tree-visualization').remove();
        }
      },
      error: function(jqXHR, exception) {
        if (jqXHR.status == 403) {}
      }
    });

  });
})(jQuery);