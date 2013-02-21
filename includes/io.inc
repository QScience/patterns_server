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
    'arguments' => array('credentials' => 'is_string', 'patterns' => 'is_string'),
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
    return 'Error. Empty request received.';
  }
  
  $imploded_credentials = $arguments['credentials'];
  $exploded_credentials = pils_explode($imploded_credentials);
  
  if ($exploded_credentials === FALSE) {
    return 'Error. Received upload pattern/s request with no credentials.';
  }
  
  $check = array(
    'user' => 'is_string',
    'pwd' => 'is_string',
  );
  
  if (!pils_check_array($exploded_credentials, $check)) {
    return 'Error. Could not extract the credentials from the encrypted message.';
  }
  
  $user = $exploded_credentials['user'];
  $pwd = $exploded_credentials['pwd'];

  $result = _patterns_server_login_authenticate_validate($user, $pwd);
  
  if ($result['status'] !== PATTERNS_SUCCESS) {
    return 'Error. Authentication failed. Please check your username and password on this server.';
  }
  
  $imploded_patterns = $arguments['patterns'];
  
  $exploded_patterns = pils_explode($imploded_patterns);
  if ($exploded_patterns === FALSE) {
    return 'Error. Could not extract the pattern files from the encrypted message.';
  }
  
  
  
  $errors = array();
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
          $errors[] = $name;
        }
      }
    }
  }
  
  $outcome = $n_patterns . ' pattern(s) received and saved.';
  if (count($errors) > 0) {
  	$outcome .= ' The following patterns could not be saved: ' . explode($errors, ',');
  }
   
  return $outcome;
}

