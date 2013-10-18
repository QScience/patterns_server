<?php

/**
 * @file
 * all function about pattern history in patters_server module.
 */

/**
 * patterns_server_history_update()
 * add a new tree to patterns_server_history_tree.
 * when upload or push a pattern file which don't contain a 'parent' section,
 * this function will be called.
 * add a new tree.
 */
function patterns_server_history_new_tree($pid, $uuuid, $tree) {
  $tree_id = db_insert('patterns_server_history_tree')
    ->fields(array(
      'root_pid' => $pid,
      'root_uuuid' => $uuuid,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
      'tree' => $tree,
      ))
    ->execute();
  db_insert('patterns_server_pattern_tree')
    ->fields(array(
      'pid' => $pid,
      'tree_id' => $tree_id,
      'level' => 0,
      'updated' => REQUEST_TIME,
    ))
    ->execute();
}

/**
 * patterns_server_history_NULL_tree()
 * add a record to the NULL tree, if NULL tree not exsited, add NULL tree first.
 * then add the record to the NULL tree.
 */
function patterns_server_history_NULL_tree($pid, $uuuid, $parent) {
  $query = db_select('patterns_server_history_tree', 'ph')
    ->fields('ph', array())
    ->condition('root_pid', NULL)
    ->condition('root_uuuid', NULL)
    ->execute()
    ->fetchAll();
  if (count($query) == 0) {
  }
  global $user;
  $tree = array();
  $tree[$uuuid] = array(
    'action' => $parent['action'],
    'd2did' => $parent['d2d_id'],
    'user' => $user->uid,
    'host' => $parent['host'],
    'time' => REQUEST_TIME,
    'children' => '',
  );
  $tree_id = db_insert('patterns_server_history_tree')
    ->fields(array(
      'root_pid' => NULL,
      'root_uuuid' => NULL,
      'created' => REQUEST_TIME,
      'updated' => REQUEST_TIME,
      'tree' => serialize($tree),
      ))
    ->execute();

  db_insert('patterns_server_pattern_tree')
    ->fields(array(
      'pid' => $pid,
      'tree_id' => $tree_id,
      'level' => 1,
      'updated' => REQUEST_TIME,
    ))
    ->execute();

}

/**
 * get parent in tree from the uuuid. 
 */
function patterns_server_history_get_parent($parent) {
}
