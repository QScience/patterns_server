<?php
/**
 * upload button link in the list page.
 */
function _patterns_server_upload_button() {
  $url = 'admin/patterns_server/add';
  $upload_link = l(t('New Pattern'), $url, array('attributes' => array('class' => array('upload-button-link'))));
  return "<strong>$upload_link</strong>";
}

/**
 * backtoserver button link and like lint in the view page.
 */
function _patterns_server_back_like_button($pid) {
  $url = 'patterns_server_homepage';
  $backtoserver_link = l(t('Back To Server'), $url, array('attributes' => array('class' => array('backtoserver-button-link'))));
  $backtoserver_link = "<strong>$backtoserver_link</strong>";
  
  
  $file_d = l(t('download'), 'pattern/' . $pid . '/download', array('attributes' => array('class' => 'download-link')));

  $voted = _patterns_server_check_whether_voted($pid);
  if($voted) {
    $vote_s = '<strong class="voted-sign" id="patterns-server-view-page-vote">voted</strong>';
  }
  else {
    $url = 'pattern/' . $pid . '/voting';
    $vote_s = l(t('voting'), $url, array('attributes' => array('class' => array('voting-link', "voting-link-$pid"), 'title' => "voting-link-$pid", 'id' => array('patterns-server-view-page-vote'))));
  }

  
  return $backtoserver_link . $file_d . $vote_s;
}

/**
 * find tag's tid by tag's name
 * operate taxonomy_term_data table.
 */
function _patterns_server_findtagtid_by_tagname($tagname) {
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
 * find entity_id from tid and 'patterns_server' in taxonomy_entity_index.
 */
function _patterns_server_findentityid_by_tagvid($tagvids, $tables) {
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
 * get the pattern list ordered by voting.
 * return rendered table.
 */
function _patterns_server_list_orderby_voting($query) {

  $vscore = array();
  if (isset($query->ordered_results) && count($query->ordered_results)) {
    foreach ($query->ordered_results as $key => $value) {
      $vscore[$key] = _patterns_server_get_voting_score($value->entity_id);
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
    $upload_button = _patterns_server_upload_button();
    $list = "<div class='patterns-server-list-table-wrap'><fieldset><legend>$fieldset_title$upload_button</legend><div class='fieldset-wrapper'>";
    $list .= _patterns_server_list_page($query);
    $list .= '</div></fieldset></div>';
    return $list;
  }
  else {
    return false;
  }
}
