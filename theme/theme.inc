<?php
/**
 * @file
 * Theme related functions, hooks.
 */

function centralSite_theme() {
  return array(
    'centralSite_pattern_list' => array(
      'variables' => array('patterns' => array()),
    ),
    'centralSite_pattern_fieldset' => array(
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
function _centralSite_pattern_build_row($pid, $pattern, $options = array()) {

  $form['name'] = array(
    '#markup' => $pattern->name,
  );
  $form['title'] = array(
    '#markup' => @$pattern->uuuid,
  );
  $form['description'] = array(
    '#markup' => $pattern->description,
  );
  $form['version'] = array(
    '#markup' =>  @$pattern->author,
  );
  $form['enable'] = array(
    '#markup' => $pattern->info['category'],
  );

  return $form;
}



function theme_centralSite_pattern_list($args) {
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
      $form['patterns'][] = _centralSite_pattern_build_row($pid, $pattern);
    }
    $form['patterns'] += array(
      '#type' => 'fieldset',
      '#title' => 'Latest Patterns',
      //'#title' => $category,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#theme' => 'centralSite_pattern_fieldset',
      '#header' => array(
        array(
          'data' => t('Category'), 
          'class' => array('checkbox')),
          t('Name'),
          t('uuuid'),
          t('Author'),// t('Title'),
          t('Description'),
        array('data' => t('Operations'), 'colspan' => 7),
      ),
    );
  }

  return drupal_render($form);
}

function theme_centralSite_pattern_fieldset($variables) {
  $form = $variables['form'];

  // Individual table headers.
  $rows = array();
  // Iterate through all the modules, which are
  // children of this fieldset.
  foreach (element_children($form) as $key) {
    // Stick it into $pattern for easier accessing.
    $pattern = $form[$key];
    $row = array();
    unset($pattern['enable']['#title']);
    $row[] = array('class' => array('checkbox'), 'data' => drupal_render($pattern['enable']));
    $label = '<label ';
    if (isset($pattern['enable']['#id'])) {
      $label .= ' for="' . $pattern['enable']['#id'] . '"';
    }

    $row[] = $label . '><strong>' . drupal_render($pattern['name']) . '</strong></label>';
    //$row[] = drupal_render($pattern['format']);


    $row[] = drupal_render($pattern['title']);

    $row[] = drupal_render($pattern['version']);
    // Add the description, along with any modules it requires.
    $description = drupal_render($pattern['description']);

    $row[] = array('data' => $description, 'class' => array('description'));

    // Display links (such as help or permissions) in their own columns.
    //foreach ($pattern['links'] as $key => $value) {
    //  $row[] = array('data' => $value);
    //}

    $rows[] = $row;
  }

  return theme('table', array('header' => $form['#header'], 'rows' => $rows));
}
