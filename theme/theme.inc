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

  $form['category'] = array(
    '#markup' => $pattern->info['category'],
  );
  $form['title'] = array(
    //'#markup' => l($pattern->title, 'patterns_server/download/' . $pid),
    '#markup' => $pattern->title,
  );
  $form['author'] = array(
    '#markup' =>  $pattern->author,
  );
  $form['uuuid'] = array(
    '#markup' => $pattern->uuuid,
  );
//  $form['description'] = array(
//    '#markup' => $pattern->description,
//  );
  $form['link'] = array(
    '#markup' => l(t('Download'), 'patterns_server/download/' . $pid),
  );

  return $form;
}



function theme_patterns_server_pattern_list($args) {
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/drupal.js');

  $patterns = $args['patterns'];

  $patterns_title = '<div id="all_patterns_div">';
  $form['patterns'] = array(
    '#prefix' => $patterns_title,
    '#suffix' => ' </div>',
    '#tree' => TRUE,
  );

  if (empty($patterns)) {
    $form['patterns']['#markup'] =  t('No patterns available.');
  }
  else {
    foreach ($patterns as $pid => $pattern) {
      $form['patterns'][] = _patterns_server_pattern_build_row($pid, $pattern);
    }
    $form['patterns'] += array(
      '#type' => 'fieldset',
      '#title' => 'Latest Patterns',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#theme' => 'patterns_server_pattern_fieldset',
      '#header' => array(
          t('Category'),
          t('Title'),
          t('Author'),
          t('uuuid'),
          t('link'),
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
    $row[] = drupal_render($pattern['uuuid']);
    //$row[] = drupal_render($pattern['description']);
    $row[] = drupal_render($pattern['link']);
    $rows[] = $row;
  }
  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}
