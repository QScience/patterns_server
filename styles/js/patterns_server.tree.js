/*
 * @file
 */
(function($) {
  $(document).ready(function() {

    var formatForTree = function(data) {
      var iter,
        result = [],
        curr,
        count = 0,
        startUrl = Drupal.settings.basePath + (Drupal.settings.clean_url === "1" ? '' : '?q=');

      for (iter in data) {
        if (Object.prototype.toString.call(data[iter]) !== '[object Array]') {
          curr = {
            id: count,
            name: data[iter].title,
            author: data[iter].author,
            category: data[iter].category,
            author_link: startUrl + 'user/' + data[iter].user,
            d2did_link: data[iter].host,
            pattern_link: startUrl + 'pattern/' + iter,
            children: formatForTree(data[iter].children),
          };

          result.push(curr);
          count++;
        }
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