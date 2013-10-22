<?php
/**
 * @file
 */

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
function _patterns_server_back_like_button($pid, $uuuid) {
  $url = 'patterns_server_homepage';
  $backtoserver_link = l(t('Back To Server'), $url, array('attributes' => array('class' => array('backtoserver-button-link'))));
  $backtoserver_link = "<strong>$backtoserver_link</strong>";


  $file_d = l(t('download'), 'pattern/' . $pid . '/download', array('attributes' => array('class' => 'download-link')));

  $voted = _patterns_server_check_whether_voted($pid);
  if ($voted === 1) {
    $url = 'pattern/' . $uuuid . '/undovote';
    $vote_s = l(t('undovote'), $url, array('attributes' => array('class' => array('undovote-link', "undovote-link-$uuuid"), 'title' => "undovote-link-$uuuid", 'id' => array("patterns-server-view-page-vote"))));
    //$vote_s = '<strong class="voted-sign" id="patterns-server-view-page-vote">voted</strong>';
  }
  elseif ($voted === 0) {
    $url = 'pattern/' . $uuuid . '/voting';
    $vote_s = l(t('voting'), $url, array('attributes' => array('class' => array('voting-link', "voting-link-$uuuid"), 'title' => "voting-link-$uuuid", 'id' => array("patterns-server-view-page-vote"))));
  }
  else {
    $url = 'user/register';
    $vote_s = l(t('forbidden'), $url, array('attributes' => array('class' => array("vote-forbid-sign"), 'title' => "Login or Register to Vote", 'id' => array('patterns-server-view-page-vote'))));
  }


  return $backtoserver_link . $file_d . $vote_s;
}

/**
 * find tag's tid by tag's name
 * operate taxonomy_term_data table.
 */
function _patterns_server_findtagtid_by_tagname($tagname) {
  $split_strings = preg_split('/[\,]+/', $tagname);

  $or = db_or();
  foreach ($split_strings as $value) {
    $or->condition('name', trim($value));
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
    $new_ordered_results = array();
    foreach ($vscore as $key => $value) {
      $new_ordered_results[] = $query->ordered_results[$key];
    }
    $query->ordered_results = array_slice($new_ordered_results, 0, 10);

    $fieldset_title = 'Most Liked Patterns:';
    $upload_button = _patterns_server_upload_button();
    $list = "<div class='patterns-server-list-table-wrap'><fieldset><legend>$fieldset_title$upload_button</legend><div class='fieldset-wrapper'>";
    $list .= _patterns_server_list_page($query);
    $list .= '</div></fieldset></div>';
    return $list;
  }
  else {
    return FALSE;
  }
}

/**
 * Generates a Universally Unique IDentifier, version 4.
 *
 * RFC 4122 (http://www.ietf.org/rfc/rfc4122.txt) defines a special type of Globally
 * Unique IDentifiers (GUID), as well as several methods for producing them. One
 * such method, described in section 4.4, is based on truly random or pseudo-random
 * number generators, and is therefore implementable in a language like PHP.
 *
 * We choose to produce pseudo-random numbers with the Mersenne Twister, and to always
 * limit single generated numbers to 16 bits (ie. the decimal value 65535). That is
 * because, even on 32-bit systems, PHP's RAND_MAX will often be the maximum *signed*
 * value, with only the equivalent of 31 significant bits. Producing two 16-bit random
 * numbers to make up a 32-bit one is less efficient, but guarantees that all 32 bits
 * are random.
 *
 * The algorithm for version 4 UUIDs (ie. those based on random number generators)
 * states that all 128 bits separated into the various fields (32 bits, 16 bits, 16 bits,
 * 8 bits and 8 bits, 48 bits) should be random, except : (a) the version number should
 * be the last 4 bits in the 3rd field, and (b) bits 6 and 7 of the 4th field should
 * be 01. We try to conform to that definition as efficiently as possible, generating
 * smaller values where possible, and minimizing the number of base conversions.
 *
 * @copyright   Copyright (c) CFD Labs, 2006. This function may be used freely for
 *              any purpose ; it is distributed without any form of warranty whatsoever.
 * @author      David Holmes <dholmes@cfdsoftware.net>
 *
 * @return  string  A UUID, made up of 32 hex digits and 4 hyphens.
 */
function _patterns_server_generate_uuuid() {

    // The field names refer to RFC 4122 section 4.1.2

    return sprintf('%04x%04x-%04x-%03x4-%04x-%04x%04x%04x',
        mt_rand(0, 65535), mt_rand(0, 65535), // 32 bits for "time_low"
        mt_rand(0, 65535), // 16 bits for "time_mid"
        mt_rand(0, 4095),  // 12 bits before the 0100 of (version) 4 for "time_hi_and_version"
        bindec(substr_replace(sprintf('%016b', mt_rand(0, 65535)), '01', 6, 2)),
            // 8 bits, the last two of which (positions 6 and 7) are 01, for "clk_seq_hi_res"
            // (hence, the 2nd hex digit after the 3rd hyphen can only be 1, 5, 9 or d)
            // 8 bits for "clk_seq_low"
        mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(0, 65535) // 48 bits for "node"
    );
}
