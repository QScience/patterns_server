<?php

/**
 * @file
 * Code related to send pattern to server / receive pattern as server.
 */


module_load_include('module', 'pils');
module_load_include('inc', 'patterns_server', 'includes/auth');

/**
 * Implements hook_drupaltodrupal_secure_rpc().
 */
function patterns_server_drupaltodrupal_secure_rpc() {
  $methods = array();
  $methods['patterns_server_publish_patterns'] = array(
    'arguments' => array('patterns' => 'is_string', 'credentials' => 'is_string'),
    'callback' => 'patterns_server_incoming_patterns',
    'description' => 'save incoming patterns to the database',
  );
  return $methods;
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

function patterns_server_incoming_patterns($arguments, $rpc_info) {
	  
  if (empty($arguments)) {
    drupal_json_output('empty');
    return;
  }

  //drupal_json_output($arguments);
  
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

  $result = _patterns_server_login_authenticate_validate($user, $pwd);
  
  if ($result['status'] !== PATTERNS_SUCCESS) {
    print $result['msg'];
    exit();   
  }
  
  $imploded_patterns = $arguments['patterns'];
  
  $exploded_patterns = pils_explode($imploded_patterns);
  if ($exploded_patterns === FALSE) {
    print 'NO4!';
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
  print $n_patterns . ' pattern(s) received and saved.';

  
  //variable_set('STE', 'STE');
  //drupal_json_output('A');
}

