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
function _patterns_pattern_build_row($pid, $pattern, $options = array()) {
  // Add in the defaults.
  $extra = array(
    'requires' => array(),
    'required_by' => array(),
    'disabled' => FALSE,
    'enabled' => FALSE,
    'links' => array(),
  );

  $form = array(
    '#tree' => TRUE,
  );

  // Set the basic properties.

  // Creating overlay div with extra info
  $title = '<span id="pid-' . $pid . '" class="pattern-title">' . $pattern->title . '</span>';

  $info = array();
  $info[] = t('Author:') . ' ' . @$pattern->info['author'];
  $info[] = t('Email:') . ' ' . @$pattern->info['author_email'];
  $info[] = t('Web:') . ' ' . @$pattern->info['author_website'];
  $author = theme('item_list', array('items' => $info));

  $title .= '<div id="pid-' . $pid . '-info" class="pattern-info">' . $author . '</div>';

  $form['format'] = array(
    '#markup' => $pattern->format,
  );

  $form['name'] = array(
    '#markup' => $pattern->name,
  );

  $form['title'] = array(
    '#markup' => $title,
  );
  $form['description'] = array(
    '#markup' => $pattern->description,
  );
  $form['version'] = array(
    '#markup' =>  @$pattern->info['version'],
  );




  #$form['#requires'] = $extra['requires'];
  #$form['#required_by'] = $extra['required_by'];

  // Check the compatibilities.
  $compatible = TRUE;
  $status_short = '';
  $status_long = '';

  //@TODO: do some checking about pattern compatibility
  $compatible = TRUE;

  // If this module is compatible, present a checkbox indicating
  // this module may be installed. Otherwise, show a big red X.
  if ($compatible) {
    $form['enable'] = array(
//      '#type' => 'checkbox',
//      '#title' => t('Enable'),
//      '#value' => $pattern->status,
//      //'#attributes' =>  array('checked' => $pattern->status),
//    );
//    if ($extra['disabled']) {
//      $form['enable']['#disabled'] = TRUE;
//    }
    '#markup' =>  ($pattern->status) ? t('Enabled') : t('Disabled'),
    );
  }
  else {
    $form['enable'] = array(
      '#markup' =>  theme('image', array('path' => 'misc/watchdog-error.png', 'alt' => $status_short, 'title' => $status_short)),
    );
    $form['description']['#markup'] .= theme('system_modules_incompatible', array('message' => $status_long));
  }


  //$actions = patterns_get_patterns_links($pattern);
  $actions = NULL;
  $form['links'] = $actions;

  return $form;
}



function theme_centralSite_pattern_list($args) {
  drupal_add_js('misc/collapse.js');
  drupal_add_js('misc/drupal.js');

  //print_t($arg);
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

      $category = $pattern->info['category'] ? $pattern->info['category'] : t('Other');

      $form['patterns'][$category][] = _patterns_pattern_build_row($pid, $pattern);
        $form['patterns'][$category] += array(
          '#type' => 'fieldset',
          '#title' => $category,
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#theme' => 'centralSite_pattern_fieldset',
          '#header' => array(
            array('data' => t('Enabled'), 'class' => array('checkbox')),
              t('Format'),
              t('Name'),
              t('Title'),
              t('Version'),
              t('Description'),
            array('data' => t('Operations'), 'colspan' => 7),
          ),
        );
    }
  }

  return drupal_render($form);
}

// Theming freely inspired by the Install Modules interface
// in /modules/system/system.admin.inc
// theme_system_modules_fieldset
// _system_modules_build_row

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

    $row[] = drupal_render($pattern['format']);

    $row[] = $label . '><strong>' . drupal_render($pattern['name']) . '</strong></label>';

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
