<?php
/**
 * @file
 * Theme related functions, hooks.
 */

function patterns_server_theme() {
  return array(
    'patterns_server_pattern_list' => array(
      'variables' => array('patterns' => array()),
    ),
    'patterns_server_pattern_fieldset' => array(
      'render element' => 'form',
    ),

    'patterns_server_one_pattern_info' => array(
      'variables' => array('patterns' => array()),
    ),
    'patterns_server_pattern_family_fieldset' => array(
      'render element' => 'form',
    ),
  );
}

/**
 * Build a table row for the tables in the pattern list page.
 *
 * @param mixed $pid the numeric id of the pattern as in the database
 * @param StdClass $pattern A pattern object as loaded from database
 * @param array $extra associative array of extra parameters. Not used now.
 */
function _patterns_server_pattern_build_row($pid, $pattern, $options = array()) {

  $form['category'] = array( '#markup' => $pattern->info['category'],
  );
  $form['title'] = array(
    '#markup' => l($pattern->title, 'patterns_server/info/' . $pid),
  );
  $form['author'] = array(
    '#markup' =>  $pattern->author,
  );
  $form['upload time'] = array(
    '#markup' => date('j-m-Y H:i:s (P)', $pattern->updated),
  );
  $form['download'] = array(
    '#markup' => l(t('Download'), 'patterns_server/download/' . $pid),
  );

  $download_origin_times = db_select('patterns_server', 'ps')
    ->fields('ps', array('downloadnum'))
    ->condition('pid', $pattern->pid)
    ->execute()
    ->fetchAssoc();
  if(empty($download_origin_times['downloadnum'])) {
    $download_origin_times['downloadnum'] = 0;
  }
  $form['downloadnum'] = array(
    '#markup' => $download_origin_times['downloadnum'],
  );
  return $form;
}



function theme_patterns_server_pattern_list($args) {
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/drupal.js');

  $patterns = $args['patterns'];
<<<<<<< HEAD

  $form['search-box']=drupal_get_form('patterns_server_search_form');
 //$form['Pattern-Category-Search']=drupal_get_form('patterns_server_search_category_form');
=======
  if(!isset($args['searchkey'])) $args['searchkey']='';
  $form['search-box']=drupal_get_form('patterns_server_search_form',$args['searchkey']);
>>>>>>> 5f5fc41da1971239d86614b2b06708ed6d39ee84
  $patterns_title = '<div id="all_patterns_div">';
  $form['patterns'] = array(
    '#prefix' => $patterns_title,
    '#suffix' => ' </div>',
    '#tree' => TRUE,
  );
  
  if($args['searchkey']==''){
  		$title='Latest Patterns';
  	}else{
  		$title='Patterns serach result';
  	}

  if (empty($patterns)) {
  	if($args['searchkey']==''){
  		$form['patterns']['#markup'] =  t('No patterns available.');
  		$title='Latest Patterns';
  	}else{
  		$form['patterns']['#markup'] =  t('No patterns search result.');
  		$title='Patterns serach result';
  	}
    
  }
  else {
    foreach ($patterns as $pid => $pattern) {
      $form['patterns'][] = _patterns_server_pattern_build_row($pid, $pattern);
    }
    $form['patterns'] += array(
      '#type' => 'fieldset',
      '#title' => $title,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#theme' => 'patterns_server_pattern_fieldset',
      '#header' => array(
          t('Category'),
          t('Title'),
          t('Author'),
          t('upload time'),
          t('Download'),
          t('Download Times'),
          //t('Description'),
      ),
    );
  }

  return drupal_render($form);
}

function theme_patterns_server_pattern_fieldset($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    $pattern = $form[$key];
    $row = array();
    $row[] =  drupal_render($pattern['category']);
    $row[] = '<strong>' . drupal_render($pattern['title']) . '</strong>';
    $row[] = drupal_render($pattern['author']);
    $row[] = drupal_render($pattern['upload time']);
    $row[] = drupal_render($pattern['download']);
    $row[] = drupal_render($pattern['downloadnum']);
    $rows[] = $row;
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}

// For one patterns page.
function _patterns_server_pattern_family_build_row($pid, $pattern, $options = array()) {

  $form['UUUID'] = array( 
    '#markup' => $pattern->uuuid,
  );
  $form['title'] = array(
    '#markup' => $pattern->title,
  );
  $form['author'] = array(
    '#markup' =>  $pattern->author,
  );
  $form['parent'] = array(
    '#markup' => '@TODO',
  );
  $form['child'] = array(
    '#markup' => '@TODO',
  );
  $form['Download'] = array(
    '#markup' => l(t('Download'), 'patterns_server/download/' . $pid),
  );

  return $form;
}

function theme_patterns_server_one_pattern_info($args) {
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/drupal.js');

  $patterns = $args['patterns'];
  $patterns_title = '<div id="one_pattern_family_div">';
  $form['patterns'] = array(
    '#prefix' => $patterns_title,
    '#suffix' => ' </div>',
    '#tree' => TRUE,
  );
  foreach ($patterns as $pid => $pattern) {
    $form['patterns'][] = _patterns_server_pattern_family_build_row($pid, $pattern);
  }
  $form['patterns'] += array(
      '#type' => 'fieldset',
      '#title' => 'One pattern\'s history',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#theme' => 'patterns_server_pattern_family_fieldset',
      '#header' => array(
          t('UUUID'),
          t('Title'),
          t('Author'),
          t('parent'),
          t('child'),
          t('Download'),
      ),
    );

  return drupal_render($form);
}

function theme_patterns_server_pattern_family_fieldset($variables) {
  $form = $variables['form'];
  $rows = array();
  foreach (element_children($form) as $key) {
    $pattern = $form[$key];
    $row = array();
    $row[] =  drupal_render($pattern['UUUID']);
    $row[] = '<strong>' . drupal_render($pattern['title']) . '</strong>';
    $row[] = drupal_render($pattern['author']);
    $row[] = drupal_render($pattern['parent']);
    $row[] = drupal_render($pattern['child']);
    $row[] = drupal_render($pattern['Download']);
    $rows[] = $row;
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}
