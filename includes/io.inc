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

function patterns_server_save_pattern($pattern_name, $pattern, $format) {  
  
  $result = _patterns_server_save_pattern($pattern_name, $pattern, $format);
  
  if (!$result['status'] == PATTERNS_SUCCESS) {
    // TODO return error in d2d
    return FALSE;
  }
  
  // TODO notify d2d of success
  return TRUE;
}

function patterns_server_incoming_patterns($arguments) {
  
  $imploded_credentials = $arguments['credentials'];
  $exploded_credentials = pils_explode($imploded_credentials);
  
  if ($exploded_credentials === FALSE) {
    
    print 'NO1!';
    exit();
  }
  
  $check = array(
    'user' => 'is_string',
    'pwd' => 'is_string',
  );
  
  if (!pils_check_array($exploded_credentials, $check)) {
    
    print 'NO2!';
  }
  
  $user = $exploded_credentials['user'];
  $pwd = $exploded_credentials['pwd'];

  if (!patterns_server_authorize($user, $pwd)) {
    print 'NO3!'
    exit();   
  }
  
  $imploded_patterns = $arguments['patterns'];
  
  $exploded_patterns = pils_explode($imploded_patterns);
  if ($exploded_patterns === FALSE) {
    print 'NO4!'
    exit();
  }
  
  
  
  $n_patterns = 0;
  foreach ($exploded_patterns as $imploded_pattern) {
    $exploded_pattern = pils_explode($imploded_pattern);
    if ($exploded_pattern !== FALSE) {
      $check = array(
        'name' => 'is_string',
        'format' => 'is_string',
        'content' => 'is_string',
      );
      if (pils_check_array($exploded_pattern, $check)) {
        $name = $exploded_pattern['name'];
        $pattern = $exploded_pattern['content'];
        $format = $exploded_pattern['format'];
        
        if (patterns_server_save_pattern($user, $pwd, $name, $pattern, $format)) {
          $n_patterns++;
        }
        else {
          // notify
        }
      }
    }
  }
  return $n_patterns . ' pattern(s) received';
}