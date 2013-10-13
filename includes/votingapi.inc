<?php
/**
 * @file
 */

/**
 * implement some vote function based on votingapi framework.
 */

/**
 * used in theme/theme.inc to return a score of current pid.
 */
function _patterns_server_get_voting_score($pid) {
  $criteria = array(
    'entity_type' => 'patterns_server',
    'entity_id' => $pid,
    'value_type' => 'points',
    'function' => 'sum',
  );
  $vote = votingapi_select_results($criteria);
  if ($vote) {
    return $vote[0]['value'];
  }
  else {
    return 0;
  }
}

/**
 * callback hook_menu() for /pattern/pid/voting
 */
function _patterns_server_voting_pattern($pid) {
  $voted = _patterns_server_check_whether_voted($pid);
  if ($voted != 0) {
    drupal_goto("pattern/$pid");
  }
  $votes = array(
    'entity_type' => 'patterns_server',
    'entity_id' => $pid,
    'value_type' => 'points',
    'value' => '1',
  );
  votingapi_add_votes($votes);
  votingapi_recalculate_results('patterns_server', $pid);

  drupal_goto("pattern/$pid");
}

/**
 * callback hook_menu() for /pattern/pid/undovote
 * don't care the votingapi's setting now, just delete the 
 * use's all votes on the pattern.
 */
function _patterns_server_undovote_pattern($pid) {
  Global $user;
  $votes = db_select('votingapi_vote')
    ->fields('votingapi_vote')
    ->condition('entity_type', 'patterns_server')
    ->condition('entity_id', $pid)
    ->condition('value_type', 'points')
    ->condition('uid', $user->uid)
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);
  $vids = array();
  foreach ($votes as $value) {
    $vids[]['vote_id'] = $value['vote_id'];
  }
  votingapi_delete_votes($vids);
  votingapi_recalculate_results('patterns_server', $pid);
  drupal_goto("pattern/$pid");
}

/**
 * check whether certain user can vote or not.
 * return -1 means this is anonymous.
 * return 0 means the user hasn't voted before.
 * return 1 means the user has voted before.
 */
function _patterns_server_check_whether_voted($pid) {
  if (user_is_anonymous()) {
    return -1;
  }
  global $user;
  $votes = db_select('votingapi_vote')
    ->fields('votingapi_vote')
    ->condition('entity_type', 'patterns_server')
    ->condition('entity_id', $pid)
    ->condition('value_type', 'points')
    ->condition('uid', $user->uid)
    ->execute()
    ->fetchAll(PDO::FETCH_ASSOC);
  if (count($votes) > 0) {
    return 1;
  }
  return 0;
}
