<?php
/**
 * upload button link in the list page.
 */
function _pattern_entity_upload_button() {
  $url = 'admin/structure/patternentity/add';
  $upload_link = l(t('New Pattern'), $url, array('attributes' => array('class' => array('upload-button-link'))));
  return "<strong>$upload_link</strong>";
}

/**
 * backtoserver button link and like lint in the view page.
 */
function _pattern_entity_back_like_button($pid) {
  $url = 'all_pattern_entity_list';
  $backtoserver_link = l(t('Back To Server'), $url, array('attributes' => array('class' => array('backtoserver-button-link'))));
  $backtoserver_link = "<strong>$backtoserver_link</strong>";
  
  
  $file_d = l(t('download'), 'pattern/' . $pid . '/download', array('attributes' => array('class' => 'download-link')));

  $voted = _pattern_entity_check_whether_voted($pid);
  if($voted) {
    $vote_s = '<strong class="voted-sign" id="pattern-entity-view-page-vote">voted</strong>';
  }
  else {
    $url = 'pattern/' . $pid . '/voting';
    $vote_s = l(t('voting'), $url, array('attributes' => array('class' => array('voting-link', "voting-link-$pid"), 'title' => "voting-link-$pid", 'id' => array('pattern-entity-view-page-vote'))));
  }

  
  return $backtoserver_link . $file_d . $vote_s;
}

/**
 * find tag's tid by tag's name
 * operate taxonomy_term_data table.
 */
function _pattern_entity_findtagtid_by_tagname($tagname) {
  $split_strings = preg_split('/[\,]+/', $tagname);
  //dsm($split_strings);

  $or = db_or();
  foreach ($split_strings as $value) {
    $or->condition('name', trim($value));
    //dsm($value);
  }

  $query = db_select('taxonomy_term_data', 'ttd')
    ->fields('ttd', array('tid'))
    ->condition($or)
    ->execute()
    ->fetchAllAssoc('tid');
  return $query;
}

/**
 * find entity_id from tid and 'patternentity' in taxonomy_entity_index.
 */
function _pattern_entity_findentityid_by_tagvid($tagvids, $tables) {
  if (count($tagvids) == 0 || !$tables) {
    return array(); 
  }

  $query = array();
  foreach ($tables as $table) {
    $or = db_or();
    foreach ($tagvids as $key => $value) {
      $or->condition($table . '_tid', $key);
    }

    $table = 'field_data_' . $table;
    $query[] = db_select($table)
      ->fields($table, array('entity_id'))
      ->condition($or)
      ->execute()
      ->fetchAllAssoc('entity_id');
   // dsm($query);
  }

  $result = array();
  foreach ($query as $q) {
    $result += $q;
  }
  return $result;

}

/**
 * get the patternentity list ordered by voting.
 * return rendered table.
 */
function _pattern_entity_list_orderby_voting($query) {

  $vscore = array();
  if (isset($query->ordered_results) && count($query->ordered_results)) {
    foreach ($query->ordered_results as $key => $value) {
      $vscore[$key] = _pattern_entity_get_voting_score($value->entity_id);
    }
    arsort($vscore);
    //dsm($vscore);
    $new_ordered_results = array();
    foreach ($vscore as $key => $value) {
      $new_ordered_results[] = $query->ordered_results[$key];
    }
    $query->ordered_results = array_slice($new_ordered_results, 0, 10);
    //dsm($query);

    $fieldset_title = 'Most Liked Patterns:';
    $upload_button = _pattern_entity_upload_button();
    $list = "<div class='pattern-entity-list-table-wrap'><fieldset><legend>$fieldset_title$upload_button</legend><div class='fieldset-wrapper'>";
    $list .= _pattern_entity_list_page($query);
    $list .= '</div></fieldset></div>';
    return $list;
  }
  else {
    return false;
  }
}
