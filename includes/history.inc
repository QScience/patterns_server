<?php

/**
 * @file
 * all function about pattern history in patters_server module.
 */

/**
 * this function will be called after a pattern file uploaded or pushed is
 * already be saved into patterns_server table.
 *
 * so the new pattern will get a new uuuid. because the uuuid is new, so I 
 * don't need to check whether the pair I want to insert is alreay existed or not.
 *
 * just insert.
 * 
 */
function patterns_server_insert_pair($parent_uuuid, $child_uuuid, $core) {
  db_insert('patterns_server_relation_pairs')
    ->fields(array(
      'parent_uuuid' => $parent_uuuid,
      'child_uuuid' => $child_uuuid,
      'action' => $core['action'],
      'host' => $core['host'],
      'd2did' => $core['d2did'],
      'user' => $core['user'],
      'date' => $core['date'],
      'created' => REQUEST_TIME,
      'title' => $core['title'],
      'category' => $core['category'],
      'author' => $core['author'],
      ))
    ->execute();
}

/**
 * this function will be called after a pattern file uploaded or pushed is
 * already be saved into patterns_server table.
 *
 * so the new pattern will get a new uuuid. because the uuuid is new, so I 
 * don't need to check whether the pair I want to insert is alreay existed or not.
 *
 * $key_uuuid is the index uuuid.
 * $target_uuuid is the uuuid will be added to the index's array.
 * $parent_child: 
 *  0 means the $target_uuuid is the parent of $key_uuuid.
 *  1 means the $target_uuuid is the child of $key_uuuid.
 *
 * there is a logic: one of the key_uuuid and target_uuuid is the new uuuid.
 * if the key_uuuid is the new one.
 *  just insert a new item into table.
 * if the target_uuuid  is the new one.
 *  just insert a new subarray into the key_uuuid item.
 *
 * there is no need to check the existence.
 * 
 */
function patterns_server_update_index($key_uuuid, $target_uuuid, $action, $parent_child, $core) {
  $pid = $core['pid'];
  $user = $core['user'];
  $category = $core['category'];
  $title = $core['title'];
  $author = $core['author'];
  $host = $core['host'];
  $d2did = $core['d2did'];

  $children = array();
  $parents = array();

  $query = db_select('patterns_server_relation_index', 'pi')
    ->fields('pi', array())
    ->condition('uuuid', $key_uuuid)
    ->execute()
    ->fetchObject();
  if ($query === FALSE) {
    if ($parent_child == 1) {
      $children[$target_uuuid] = $core;
    }
    else {
      $parents[$target_uuuid] = $core;
    }
    db_insert('patterns_server_relation_index')
      ->fields(array(
        'pid' => $pid,
        'uuuid' => $key_uuuid,
        'parents' => serialize($parents),
        'children' => serialize($children),
        'updated' => REQUEST_TIME,
        'category' => $category,
        'author' => $author,
        'title' => $title,
        'user' => $user,
        'host' => $host,
        'd2did' => $d2did,
        ))
      ->execute();
  }
  else {
    if ($parent_child == 1) {
      $children = unserialize($query->children);
      $children[$target_uuuid] = $core;
      $children = serialize($children);
      $parents = $query->parents;
    }
    else {
      $parents = unserialize($query->parents);
      $parents[$target_uuuid] = $core;
      $parents = serialize($parents);
      $children = $query->children;
    }
    db_update('patterns_server_relation_index')
      ->fields(array(
        'parents' => $parents,
        'children' => $children,
        'pid' => $pid,
        'updated' => REQUEST_TIME,
        'category' => $category,
        'author' => $author,
        'title' => $title,
        'user' => $user,
        'host' => $host,
        'd2did' => $d2did,
      ))
      ->condition('uuuid', $key_uuuid)
      ->execute();
  }
}

/**
 */
function patterns_server_delete_pairs($parent_uuuid, $child_uuuid = FALSE) {
  if ($child_uuuid === FALSE) {
    $db_or = db_or();
    $db_or->condition('parent_uuuid', $parent_uuuid);
    $db_or->condition('child_uuuid', $parent_uuuid);
    db_delete('patterns_server_relation_pairs')
      ->condition($db_or)
      ->execute();
  }
  else {
    db_delete('patterns_server_relation_pairs')
      ->condition('parent_uuuid', $parent_uuuid)
      ->condition('child_uuuid', $child_uuuid)
      ->execute();
  }
}

/**
 */
function patterns_server_delete_index($key_uuuid) {
  $index = patterns_server_get_index($key_uuuid);

  if ($index != FALSE) {
    $children = unserialize($index->children);
    $parents = unserialize($index->parents);
    foreach ($children as $uuuid => $child) {
      patterns_server_delete_index_pair($uuuid, $key_uuuid, 0);
    }
    foreach ($parents as $uuuid => $parent) {
      patterns_server_delete_index_pair($uuuid, $key_uuuid, 1);
    }
    db_delete('patterns_server_relation_index')
      ->condition('uuuid', $key_uuuid)
      ->execute();
  }
}

/**
 * $parent_child: 
 *  0 means the $target_uuuid is the parent of $key_uuuid.
 *  1 means the $target_uuuid is the child of $key_uuuid.
 */
function patterns_server_delete_index_pair($key_uuuid, $target_uuuid, $parent_child) {
  $query = db_select('patterns_server_relation_index', 'pi')
    ->fields('pi', array())
    ->condition('uuuid', $key_uuuid)
    ->execute()
    ->fetchObject();

  $children = array();
  $parents = array();
  $empty_c = FALSE;
  $empty_p = FALSE;
  if ($query != FALSE) {
    $children = unserialize($query->children);
    $parents = unserialize($query->parents);
    if ($parent_child == 1) {
      unset($children[$target_uuuid]);
    }
    else {
      unset($parents[$target_uuuid]);
    }

    if (count($children) == 0 && count($parents) == 0) {
      db_delete('patterns_server_relation_index')
        ->condition('uuuid', $key_uuuid)
        ->execute();
      return;
    }

    $parents = serialize($parents);
    $children = serialize($children);

    
    db_update('patterns_server_relation_index')
      ->fields(array(
        'parents' => $parents,
        'children' => $children,
        'updated' => REQUEST_TIME,
      ))
      ->condition('uuuid', $key_uuuid)
      ->execute();
  }
}

/**
 */
function patterns_server_get_index($key_uuuid) {
  $query = db_select('patterns_server_relation_index', 'pi')
    ->fields('pi', array())
    ->condition('uuuid', $key_uuuid)
    ->execute()
    ->fetchObject();
  return $query;
}

