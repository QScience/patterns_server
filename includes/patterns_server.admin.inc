<?php
/**
 * @file
 */

/**
 * Implements ENTITY_TYPE_form for admin UI.
 * all field in table: patterns_server.
 * no need care: pid, created, updated, file_downloads,
 * need: title, description, category, pattern, author, file_name, file_format, file_path, uuuid
 */
function patterns_server_form($form, &$form_state, $patterns_server) {
  $form['pattern_file'] = array(
    '#type' => 'file',
    '#title' => t('Choose a pattern file'),
    '#description' => t('Upload a file, allowed extensions: yaml, xml'),
  );


  field_attach_form('patterns_server', $patterns_server, $form, $form_state);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload'),
    '#weight' => 20,
  );

  return $form;
}

/**
 * validate function for ENTITY_TYPE_form
 */
function patterns_server_form_validate(&$form, &$form_state) {
  $validators = array('file_validate_extensions' => array('yaml xml'));
  $file = file_save_upload('pattern_file', $validators);

  if (!$file) {
      form_set_error('pattern_file', t('no pattern file selected.'));
  }
  else {
    $file_exist = db_select('patterns_server', 'pe')
      ->fields('pe', array('file_name'))
      ->condition('file_name', $file->filename, '=')
      ->execute()
      ->fetchAssoc();

    if ($file_exist) {
        form_set_error('pattern_file', t('pattern file already exists.'));
    }
    else {
      $form_state['storage']['file_obj'] = $file;
      $pattern = NULL;
      $file_extension = NULL;
      if ($file) {
        $file_extension = pathinfo($file->filename, PATHINFO_EXTENSION);

        // Choose appropriate function based on the file extension.
        // Can be FALSE, if no parser is found.
        $load_function = patterns_parser_get_parser_function($file_extension, PATTERNS_PARSER_LOAD);

        // Load and save pattern.
        if (!$load_function || !($pattern = $load_function($file->uri))) {
          form_set_error('pattern_file', t('parsing pattern file failed.'));
        }

        if (!patterns_validate_pattern($pattern, $file_extension, PATTERNS_VALIDATE_SYNTAX)) {
          form_set_error('pattern_file', t('pattern file has wrong syntax.'));
        }

        if (isset($pattern['info']['title']) && !empty($pattern['info']['title'])) {
          $form_state['values']['title'] = $pattern['info']['title'];
        }
        else {
          form_set_error('pattern_file', t('file don\'t have \'title\' field.'));
        }
        if (isset($pattern['info']['description']) && !empty($pattern['info']['description'])) {
          $form_state['values']['description'] = $pattern['info']['description'];
        }
        else {
          form_set_error('pattern_file', t('file don\'t have \'description\' field.'));
        }
        if (isset($pattern['info']['category']) && !empty($pattern['info']['category'])) {
          $form_state['values']['category'] = $pattern['info']['category'];
        }
        else {
          form_set_error('pattern_file', t('file don\'t have \'category\' field.'));
        }
        $form_state['values']['pattern'] = $pattern;
        if (isset($pattern['info']['author']) && !empty($pattern['info']['author'])) {
          $form_state['values']['author'] = $pattern['info']['author'];
        }
        else {
          form_set_error('pattern_file', t('file don\'t have \'author\' field.'));
        }
        Global $user;
        $form_state['values']['uploader'] = $user->uid;
        $form_state['values']['file_name'] = $file->filename;
        $form_state['values']['file_format'] = $file_extension;
        //$form_state['values']['file_path'] = $file->uri;

        if (isset($pattern['info']['uuuid']) && !empty($pattern['info']['uuuid'])) {
          $form_state['values']['uuuid'] = $pattern['info']['uuuid'];
        }
        else {
          $form_state['values']['uuuid'] = _patterns_server_generate_uuuid();
        }


        $dir = 'public://patterns_server/';
        if (!file_prepare_directory($dir)) {
          form_set_error('pattern_file', t('public://patterns_server doesn\'t exist, or isn\'t writable.'));
        }
        else {
          if ($file = file_move($file, $dir)) {
            $form_state['storage']['file_obj'] = $file;
            $file->status = FILE_STATUS_PERMANENT;
            file_save($file);
            $form_state['values']['file_path'] = $file->uri;
          }
          else {
            form_set_error('pattern_file', t('save to public dir failed'));
          }
        }
      }
      else {
        form_set_error('pattern_file', t('No file was uploaded.'));
      }
    }
  }
}

/**
 * submit function for ENTITY_TYPE_form.
 */
function patterns_server_form_submit(&$form, &$form_state) {
  $patterns_server = entity_ui_form_submit_build_entity($form, $form_state);
  $patterns_server->save();
  //dsm($patterns_server);

  if (user_access('administer patterns server')) {
    $form_state['redirect'] = 'admin/patterns_server';
  }
  else {
    $form_state['redirect'] = 'patterns_server_homepage';
  }
}
