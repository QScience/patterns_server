<?php

/**
 * @file
 * all function operating db in patters_server module.
 */

/**
 * latest patterns
 */
function _patterns_server_get_pattern_timeDESC_from_db($num) {
  $num = is_numeric($num)?$num:10;
  $result = db_select('patterns_server', 'p')
                  ->fields('p', array())
                  ->orderBy('created', 'DESC')
                  ->range(0, $num)
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
    $result[$k] = $v;
  }
  return $result;
}

/**
 * one pattern defined by pid.
 */
function _patterns_server_get_pattern_pid_from_db($pid) {
  $result = db_select('patterns_server', 'p')
                  ->fields('p', array())
                  ->condition('p.pid', $pid)
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
    $result[$k] = $v;
  }
  return $result;
}

/**
 * Used by search page.
 */
function _patterns_server_search_pattern($key, $type) {
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
  $result = db_select('patterns_server', 'p')
                  ->fields('p', array())
                  ->condition($or)
                  ->orderBy('created', 'DESC')
                  ->execute()
                  ->fetchAll();
  foreach ($result as $k => $v) {
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
function _patterns_server_download_recode($id) {
  $download_origin_times = db_select('patterns_server', 'ps')
    ->fields('ps', array('file_downloads', 'pid'))
    ->condition('pid', $id)
    ->execute()
    ->fetchAssoc();
  $download_origin_times['file_downloads']  += 1;
  db_update('patterns_server')
    ->condition('pid', $id)
    ->fields(array('file_downloads' => $download_origin_times['file_downloads']))
    ->execute();
}

/**
 * Writes the pattern metadata (and the actual pattern) to the database.
 *
 * @param array $pattern
 *   The pattern represented as an array.
 * @param mixed $path
 *   The new path where the file has just been saved or read from.
 * @param mixed $name
 *   The name of the pattern file without the extension.
 * @param mixed $format 
 *   The format of the pattern (YAML, XML, etc.)
 * @param mixed $username (Optional)
 *   A name for the author/uploader of the pattern. Defaults, logged user
 * @param string $uuuid (Optional)
 *   Optional argument to force the use of certain UUUID (i.e.: while clonning)  
 */
function _patterns_server_db_save_pattern($pattern, $path, $name, $format = NULL, $username = NULL, $uuuid = NULL) {

  $title        = patterns_utils_init_from_pattern('title', $pattern);
  $description  = patterns_utils_init_from_pattern('description', $pattern);
  $format       = patterns_utils_init_from_pattern('format', $pattern, $format, PATTERNS_FORMAT_YAML);

  if (is_null($username)) {
    global $user;
    $username = $user->name;
  }
  
  // Check if it is valid pattern: only syntax is validated, not the tags
  if (patterns_validate_pattern($pattern, $format, PATTERNS_VALIDATE_SYNTAX)) {
    $status = PATTERNS_STATUS_OK;
  }
  else {
    $status = PATTERNS_STATUS_INVALID;
  }

  $pattern_data = db_select('patterns_server', 'p')
                    ->fields('p', array('pid','file_name', 'file_format', 'file_path', 'created' ))
                    ->condition('p.file_name', $name)
                    ->condition('p.file_format', $format)
                    ->execute()
                    ->fetchAssoc();

  // Pattern already in the DB, data needs to be updated.
  if (!empty($pattern_data)) {
  }
  // It is a new Pattern.
  else {
    // Note: time() != REQUEST_TIME since it's a long process.
    $entity = entity_create('patterns_server', array('type' => 'patterns_server'));
    $entity->title = $title;
    $entity->description = $description;
    $entity->category = 'test';
    $entity->pattern = $pattern;
    $entity->author = $username;
    $entity->uploader = 1;
    $entity->file_name = $name;
    $entity->file_format = $format;
    $entity->file_path = $path;
    $entity->file_downloads = 0;
    $entity->uuuid = _patterns_server_generate_uuuid();
    $entity->save();
    }
  return TRUE;
}
