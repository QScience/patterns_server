<?php

/**
 * @file
 * Code related to send pattern to server / receive pattern as server.
 */


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

/**
 * Implements hook_drupaltodrupal_secure_rpc().
 */
function _patterns_server_drupaltodrupal_secure_rpc() {
  $methods = array();
  $methods['patterns_d2d_push_patterns'] = array(
    'arguments' => array('patterns' => 'is_string'),
    'callback' => '_patterns_server_push_patterns',
    'description' => 'publish public patterns to remote server',
  );
  return $methods;
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
        
        // TODO add real user and pwd
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