<?php

/**
 * @file
 * all function operating db in patters_server module.
 */

/**
 * latest patterns
 */
function _pattern_entity_get_pattern_timeDESC_from_db($num) {
  $num = is_numeric($num)?$num:10;
  $result = db_select('patternentity', 'p')
                  ->fields('p', array())
                  ->orderBy('created', 'DESC')
                  ->range(0, $num)
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
    $v->uuuid = "helloworld-test";
    $v->format = $v->file_format;
    $result[$k] = $v;
  }
  return $result;
}

/**
 * one pattern defined by pid.
 */
function _pattern_entity_get_pattern_pid_from_db($pid) {
  $result = db_select('patternentity', 'p')
                  ->fields('p', array())
                  ->condition('p.pid', $pid)
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
    $v->uuuid = "helloworld-test";
    $v->format = $v->file_format;
    $result[$k] = $v;
  }
  return $result;
}

/**
 * Used by search page.
 */
function _pattern_entity_search_pattern($key, $type) {
  $or = db_or();
  switch ($type) {
    case 0:
      $or->condition('title', '%' . db_like($key) . '%', 'LIKE');
      break;
    case 1:
      $or->condition('category', '%' . db_like($key) . '%', 'LIKE');
      break;
    case 2:
      $or->condition('author', '%' . db_like($key) . '%', 'LIKE');
      break;
    case 3:
      $or->condition('uuuid', '%' . db_like($key) . '%', 'LIKE');
      break;
    case 4:
      $or->condition('pid', $key, '=');
      break;
  }
  $result = db_select('patternentity', 'p')
                  ->fields('p', array())
                  ->condition($or)
                  ->orderBy('created', 'DESC')
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
    $v->uuuid = "helloworld-test";
    $v->format = $v->file_format;
    $result[$k] = $v;
  }

  $patterns = array();
  foreach ($result as $pattern) {
    $patterns[$pattern->pid] = $pattern;
    $data = unserialize($pattern->pattern);
    $patterns[$pattern->pid]->pattern = $data;
    $patterns[$pattern->pid]->info = $data['info'];
  }
  return $patterns;
}

/**
 */
function _pattern_entity_download_recode($id) {
  $download_origin_times = db_select('patternentity', 'ps')
    ->fields('ps', array('file_downloads', 'pid'))
    ->condition('pid', $id)
    ->execute()
    ->fetchAssoc();
  $download_origin_times['file_downloads']  += 1;
  db_update('patternentity')
    ->condition('pid', $id)
    ->fields(array('file_downloads' => $download_origin_times['file_downloads']))
    ->execute();
}
