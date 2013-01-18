<?php

/**
 * @file
 * Code related to send pattern to server / receive pattern as server.
 */


module_load_include('module', 'pils');

function patterns_server_authorize($user, $pwd) {

  // recreate a form structure
  $form_state = array();
  $form = array();
  
  $form_state['values']['name'] = $user;
  $form_state['values']['pass'] = $pwd;
  
  user_login_authenticate_validate($form, $form_state);
  
  // Too many log-in attempts
  if ($form_state['flood_control_triggered']) {
    // DO SOMETHING
    return FALSE;
  }
  
  if (isset($form_state['uid'])) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

/**
 * Implements hook_drupaltodrupal_secure_rpc().
 */
function patterns_server_incoming_pattern($user, $pwd, $pattern_name, $pattern, $format) {
  
  if (!patterns_server_authorize($user, $pwd)) {
    // TODO return error in d2d
    return FALSE;
  }
  
  $result = _patterns_server_save_pattern($pattern_name, $pattern, $format);
  
  if (!$result['status'] == PATTERNS_SUCCESS) {
    // TODO return error in d2d
    return FALSE;
  }
  
  // TODO notify d2d of success
  return TRUE;
}

function _patterns_server_push_patterns($arguments, $rpc_info) {
  $imploded_patterns = $arguments['patterns'];
  $exploded_patterns = drupaltodrupal_explode($imploded_patterns);
  if ($exploded_patterns === FALSE) {
    throw new DrupalToDrupalRemoteException('internal error while processing patterns');
  }
  $n_patterns = 0;
  foreach ($exploded_patterns as $imploded_pattern) {
    $exploded_pattern = drupaltodrupal_explode($imploded_pattern);
    if ($exploded_pattern !== FALSE) {
      $check = array(
        'name' => 'is_string',
        'format' => 'is_string',
        'content' => 'is_string',
      );
      if (drupaltodrupal_check_array($exploded_pattern, $check)) {
        $name = $rpc_info['id'] . '_' . $exploded_pattern['name'];
        
        $user = $exploded_pattern['user'];
        $pwd = $exploded_pattern['pwd'];
        $pattern = $exploded_pattern['content'];
        $format = $exploded_pattern['format'];
        
        if (patterns_server_incoming_pattern($user, $pwd, $name, $pattern, $format)) {
//        if (patterns_io_save_pattern($exploded_pattern['content'], $name, $exploded_pattern['format'])) {
//          patterns_db_publish_pattern($name);
          $n_patterns++;
        }
      }
    }
  }
  return $n_patterns . ' pattern(s) received';
}