<?php

/**
 * @file
 * Code related to authorizing clients on performing operations on the server
 * 
 * The code is modified copy of drupal authentication functions. The changes
 * are due to the fact that we don't want to send the password in plain text,
 * so we must adapt the authentication to deal with an already encrypted string.
 */


/**
 * A validate handler on the login form. Check supplied username/password
 * against local users table. If successful, $form_state['uid']
 * is set to the matching user ID.
 */
function _patterns_server_login_authenticate($user, $password) {
  $status = PATTERNS_SUCCESS;
  
  if (empty($user)) {
    return patterns_results(PATTERNS_ERR, t('Cannot login empty user name'));  
  }
  
  if (empty($password)) {
    return patterns_results(PATTERNS_ERR, t('Password cannot be empty')); 
  }
  
  $password = trim($password);
 
  // Do not allow any login from the current user's IP if the limit has been
  // reached. Default is 50 failed attempts allowed in one hour. This is
  // independent of the per-user limit to catch attempts from one IP to log
  // in to many different user accounts.  We have a reasonably high limit
  // since there may be only one apparent IP for all users at an institution.
  if (!flood_is_allowed('failed_login_attempt_ip', variable_get('user_failed_login_ip_limit', 50), variable_get('user_failed_login_ip_window', 3600))) {
    return patterns_results(PATTERNS_ERR, t('Flood control: too many logins attempts from your ip address. Please try later.')); 
  }
  
  $account = db_query("SELECT * FROM {users} WHERE name = :name", array(':name' => $user)->fetchObject();
  
  if (!$account) {
    return patterns_results(PATTERNS_ERR, t('No such user found: :name', array(':name' => $user)));
  }
  
  if ($account->status != 1) {
    return patterns_results(PATTERNS_ERR, t('Account not active: :name', array(':name' => $user)));
  }
  
  if (variable_get('user_failed_login_identifier_uid_only', FALSE)) {
    // Register flood events based on the uid only, so they apply for any
    // IP address. This is the most secure option.
    $identifier = $account->uid;
  }
  else {
    // The default identifier is a combination of uid and IP address. This
    // is less secure but more resistant to denial-of-service attacks that
    // could lock out all users with public user names.
    $identifier = $account->uid . '-' . ip_address();
  }
  
  $form_state['flood_control_user_identifier'] = $identifier;

  // Don't allow login if the limit for this user has been reached.
  // Default is to allow 5 failed attempts every 6 hours.
  if (!flood_is_allowed('failed_login_attempt_user', variable_get('user_failed_login_user_limit', 5), variable_get('user_failed_login_user_window', 21600), $identifier)) {
    $form_state['flood_control_triggered'] = 'user';
    return;
  }
  
    
  if (!_patterns_server_check_password($user, $password)) {
    _patterns_server_login_success($identifier)
  }
  else {
   
  }
      
  
}

function _patterns_server_check_password($user, $password) {
  $account = user_load_by_name($name);
  return $account && $account->pass == $password;
}



function _patterns_server_login_success($identifier) {
  if (!empty($identifier)) {
     flood_clear_event('failed_login_attempt_user', $identifier);
  }
}

function _patterns_server_login_fail($identifier) {
  flood_register_event('failed_login_attempt_ip', variable_get('user_failed_login_ip_window', 3600));
       
  if (!empty($identifier)) {
    flood_register_event('failed_login_attempt_user', variable_get('user_failed_login_user_window', 21600), $form_state['flood_control_user_identifier']);
  }
      
}

