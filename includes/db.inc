<?php

/**
 * @file
 * all function operating db in patters_server module.
 */

module_load_include('inc', 'patterns', 'includes/io/io');
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
    $or->condition('description', '%' . db_like($key) . '%', 'LIKE');
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
function _patterns_server_db_save_pattern($pattern, $path, $name, $format = NULL, $username = NULL, $rpc_info = NULL) {

  //FOR LATER
  $uuuid = NULL;
  if (isset($pattern['info']['uuuid'])) {
    $uuuid=$pattern['info']['uuuid'];
    $pattern_data = db_select('patterns_server', 'p')
    ->fields('p', array('pid'))
    ->condition('p.uuuid', $uuuid)
    ->execute()
    ->fetchAssoc();

    if (!empty($pattern_data)) {
      $msg = t('Pattern already exist.');
      return patterns_results(PATTERNS_ERR, $msg);
    }
  }

  $uid = user_load_by_name($username);
  $file_exist = db_select('patterns_server', 'pe')
  ->fields('pe', array('file_name'))
  ->condition('file_name', $name, '=')
  ->condition('uploader', $uid->uid)
  ->execute()
  ->fetchAssoc();
  if (!empty($file_exist)) {
    $msg = t('Pattern already exist.');
    return patterns_results(PATTERNS_ERR, $msg);
  }

  // Pattern already in the DB, data needs to be updated.
  // It is a new Pattern.
  $entity = entity_create('patterns_server', array('type' => 'patterns_server'));
  $entity->title = $pattern['info']['title'];
  $entity->description = $pattern['info']['description'];
  $entity->category = $pattern['info']['category'];
  $entity->pattern = $pattern;
  $entity->author = $pattern['info']['author'];
  $entity->uploader = $uid->uid;

  $entity->file_name = $name;
  $entity->file_format = $format;
  $entity->file_path = $path;
  $entity->file_downloads = 0;
  $entity->uuuid = _patterns_server_generate_uuuid();
  $entity->host = $rpc_info['instance']['url'];
  $entity->d2did = $rpc_info['instance']['d2d_id'];
  $entity->save();

  $msg = t('Pattern was saved in %path.', array('%path' => $path, ));
  return patterns_results(PATTERNS_SUCCESS, $msg);
}

/**
 *
 */
function _patterns_server_save_pattern($name, $content, $format, $username, $rpc_info) {

  $status = PATTERNS_SUCCESS;
  $msg = NULL;

  if (is_null($name)) {
    $msg = t('Cannot save pattern with \'NULL\' identifier.');
    return patterns_results(PATTERNS_ERR, $msg);
  }

  if (!patterns_validate_pattern($content, $format, PATTERNS_VALIDATE_SYNTAX)) {
    $msg = t("Pattern '%name' could not be saved. Make sure edited code is well-formed.", array('%name' => $name));
    return patterns_results(PATTERNS_ERR, $msg);
  }


  $public_dir_path = variable_get('file_public_path', conf_path() . '/files');
  $dir = $public_dir_path . '/patterns_server/incoming';
  if (!file_exists($dir)) {
    if (!drupal_mkdir($dir)) {
      drupal_set_message(t('cannot create folder in public/incomming directory, this should be caused by permission.'), 'error');
    }
  }
  if (!file_prepare_directory($dir, FILE_CREATE_DIRECTORY)) {
    $msg = t('Error: the pattern is not writable. Please check the file system permissions.');
    return patterns_results(PATTERNS_ERR, $msg);
  }

  // Check if the file has a valid extension
  // and in case add the format at the end
  if (!_patterns_io_file_has_valid_extension($name)) {
    $name = $name . '.' . $format;
  }

  $path_original = $dir . '/' . $name;

  if (is_array($content)) {
    $original = patterns_parser_dump($content, $format);
  }
  else {
    $original = $content;
  }

  $path = file_unmanaged_save_data($original, $path_original, FILE_EXISTS_REPLACE);
  if (!$path) {
    $msg = t('An error occurred while saving the file to %path. A file with the same name exists.', array('%path' => $path_original));
    return patterns_results(PATTERNS_ERR, $msg);
  }

  // Load and save pattern.
  $load_function = patterns_parser_get_parser_function($format, PATTERNS_PARSER_LOAD);
  if (!$load_function) {
    $msg = t('Could not find a parser for %name', array('%name' => $name));
    return patterns_results(PATTERNS_ERR, $msg);
  }

  $pattern = $load_function($path);
  if (!$pattern) {
    $msg = t("Pattern '%name' could not be saved into the database. Make sure edited code is well-formed.", array('%name' => $name));
    patterns_io_remove_pattern_from_fs($path);
    return patterns_results(PATTERNS_ERR, $msg);
  }

  return _patterns_server_db_save_pattern($pattern, $path, $name, $format, $username, $rpc_info);

}

/**
 */
function patterns_server_check_whether_uuuid_exist($uuuid) {
  $query = db_select('patterns_server', 'ps')
  ->fields('ps', array())
  ->condition('uuuid', $uuuid)
  ->execute()
  ->fetchAll();
  if (count($query) > 0) {
    return TRUE;
  }
  return FALSE;
}

/**
 */
function _patterns_server_get_random_history_tree_ajax() {
  $tree = array();

  $queryDB = db_select('patterns_server_relation_index', 'pi')
  ->fields('pi', array('uuuid', 'parents', 'children', 'author', 'title', 'category', 'd2did', 'host', 'user'))
  ->execute()
  ->fetchAll();

  foreach ($queryDB as $query) {
    $children = unserialize($query->children);
    $tree[$query->uuuid] = array();
    $tree[$query->uuuid]['author'] = $query->author;
    $tree[$query->uuuid]['title'] = $query->title;
    $tree[$query->uuuid]['category'] = $query->category;
    $tree[$query->uuuid]['user'] = $query->user;
    $tree[$query->uuuid]['d2did'] = $query->d2did;
    $tree[$query->uuuid]['host'] = $query->host;
    foreach($children as $uuuid => $child) {
      $tree[$query->uuuid]['children'][$uuuid] = $child;
    }
  }

  if (count($tree) > 0) {
    drupal_json_output(array(
      'success' => TRUE,
      'tree' => $tree,
    ));
  }
  else {
    drupal_json_output(array(
      'success' => FALSE,
      'tree' => $tree,
    ));
  }
}

/**
 * get pattern from db with uuuid 
 * if success, return a object contain all information of a pattern.
 * if failed, return FALSE.
 */
function patterns_server_get_pattern_by_uuuid($uuuid) {
  $pid = db_select('patterns_server', 'ps')
  ->fields('ps', array())
  ->condition('uuuid', $uuuid)
  ->execute()
  ->fetchObject();
  if ($pid != FALSE) {
    $pid = $pid->pid;
    $pe = entity_object_load($pid, 'patterns_server');
    return $pe;
  }
  return FALSE;
}

/**
 */
function patterns_server_get_pid_by_uuuid($uuuid) {

  $query = db_select('patterns_server', 'ps')
  ->fields('ps', array('pid'))
  ->condition('uuuid', $uuuid)
  ->execute()
  ->fetchObject();
  if ($query != FALSE) {
    return $query->pid;
  }
  return FALSE;
}
