<?php
/**
 * @file
 * build several blocks for patterns_server.
 */

/**
 * most prolific author
 * used by patterns_server_block_view.
 */
function _patterns_server_block_most_prolific_author() {
  drupal_add_css(drupal_get_path('module', 'patterns_server') . '/styles/css/patterns_server.block.css');
  $result = array();
  $result['type'] = "ul";
  $result['attributes'] = array('class' => 'patterns-server-most-prolific-authors');


  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'patterns_server')
    ->execute();

  $authors_score = array();
  if (isset($query->ordered_results) && count($query->ordered_results)) {
    foreach ($query->ordered_results as $value) {
      $ids[] = $value->entity_id;
    }
    $entities = entity_load('patterns_server', $ids);
    foreach ($entities as $pe) {
      $pe->author = strtolower($pe->author);
      if (isset($authors_score[$pe->author])) {
        $authors_score[$pe->author]++;
      }
      else {
        $authors_score[$pe->author] = 1;
      }
    }
  }
  if (!count($authors_score)) {
    return t('no available authors.');
  }
  arsort($authors_score);
  $authors_score = array_slice($authors_score, 0, 10, TRUE);

  $items = array();
  foreach ($authors_score as $author => $score) {
    $items[] = array(
      'data' => l($author, 'pattern/author/' . $author) . ' × ' . $score,
    );
  }

  $result['items'] = $items;

  return theme('item_list', $result);

}

/**
 * hottest tags
 * used by patterns_server_block_view.
 */
function _patterns_server_block_hottest_tags() {
  drupal_add_css(drupal_get_path('module', 'patterns_server') . '/styles/css/patterns_server.block.css');
  $result = array();
  $result['type'] = "ul";
  $result['attributes'] = array('class' => 'patterns-server-most-popular-tags');

  //get all tags field for patterns_server.
  $tags_table_db = array();
  $fields_info = field_info_instances('patterns_server');
  if (isset($fields_info['patterns_server']) && count($fields_info['patterns_server'])) {
    $fields_info = $fields_info['patterns_server'];
    foreach ($fields_info as $field_name => $_value) {
      $field_info = field_info_field($field_name);
      if ($field_info['type'] == 'taxonomy_term_reference') {
        $tags_table_db[] = $field_name;
      }
    }
  }
  if (!count($tags_table_db)) {
    return t('no available tags.');
  }

  $query = array();
  foreach ($tags_table_db as $table) {
    $table_name = 'field_data_' . $table;
    $query[$table] = db_select($table_name)
      ->fields($table_name, array($table . '_tid'))
      ->execute()
      ->fetchAll();
  }

  $tags_score = array();
  foreach ($query as $tb => $value) {
    $key = $tb . '_tid';
    foreach ($value as $tag) {
      if (isset($tags_score[$tag->$key])) {
        $tags_score[$tag->$key]['score']++;
      }
      else {
        $tags_score[$tag->$key]['score'] = 1;
        $tags_score[$tag->$key]['table'] = $tb;
      }
    }
  }
  arsort($tags_score);
  $tags_score = array_slice($tags_score, 0, 10, TRUE);

  $items = array();
  foreach ($tags_score as $tid => $score_tb) {
    $tagname = _patterns_server_tagname_by_tagid($tid);
    $items[] = array(
      'data' => l($tagname['name'], 'pattern/alltags/' . $tid) . ' × ' . $score_tb['score'],
    );
  }

  $result['items'] = $items;

  return theme('item_list', $result);
}

/**
 * find vocabulary by tags id.
 */
function _patterns_server_tagname_by_tagid($tagid) {
  $name = db_select('taxonomy_term_data', 'ttd')
    ->fields('ttd', array('name'))
    ->condition('tid', $tagid)
    ->execute()
    ->fetchAssoc();

  return $name;
}

/**
 * get an entity's all taxonomy fields' name.
 */
function _patterns_server_get_all_taxonomyfields_name($entity_type) {

  //get all tags field for patterns_server.
  $tags_table_db = array();
  $fields_info = field_info_instances($entity_type);
  if (isset($fields_info[$entity_type]) && count($fields_info[$entity_type])) {
    $fields_info = $fields_info[$entity_type];
    foreach ($fields_info as $field_name => $_value) {
      $field_info = field_info_field($field_name);
      if ($field_info['type'] == 'taxonomy_term_reference') {
        $tags_table_db[] = $field_name;
      }
    }
  }
  if (!count($tags_table_db)) {
    return FALSE;
  }
  else {
    return $tags_table_db;
  }
}

/**
 */
function _patterns_server_block_description() {

  $s = "<p><strong>";
  $s .= "Welcome to Patterns Server!";
  $s .= "</strong></p>";

  $s .= "<p>";
  $s .= "Discover relevant ";
  $s .= l("Drupal", "http://www.drupal.org");
  $s .= " communities, by browsing the archive of registered web sites.";
  $s .= "</p>";

  $s .= "<p>";
  $s .= "Add them as a friend, and start sharing content, updates, and much more!";
  $s .= "</p>";

  $s .= "<p>";
  $s .= "Join the Drupal2Drupal network ";
  $s .= l("now", "content/about");
  $s .= ".</p>";
  
  return $s;
}
