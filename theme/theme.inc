<?php
/**
 * @file
 * Theme related functions, hooks.
 */

function patterns_server_theme() {
  return array(
    'patterns_server_pattern_list' => array(
      'variables' => array('patterns' => array(),
                           'searchkey' => array()),
    ),
    'patterns_server_pattern_fieldset' => array(
      'render element' => 'form',
    ),
    'patterns_server_patterns_table' => array(
      'variables' => array('header' => array(),
                           'rows' => array()),
    ),

    'patterns_server_one_pattern_info' => array(
      'variables' => array('patterns' => array()),
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
    '#markup' => l($pattern->title, 'patterns_server/info/' . $pid, array('attributes' => array('class' => 'info-link'))),
  );
  $form['author'] = array(
    '#markup' =>  $pattern->author,
  );
  $form['upload time'] = array(
    '#markup' => '<i value="'.$pattern->updated . '" '. 'class="upload-time">' . date('j-m-Y H:i:s (P)', $pattern->updated) . '</i>',
  );
  $form['download'] = array(
    '#markup' => l(t('Download'), 'patterns_server/download/' . $pid, array('attributes' => array('class' => 'download-link'))),
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
    //'#markup' => $download_origin_times['downloadnum'],
    '#markup' => '<strong class="download-times">'.$download_origin_times['downloadnum'].'</strong>',
  );
  return $form;
}



function theme_patterns_server_pattern_list($args) {
  drupal_add_js(drupal_get_path('module','patterns_server') . '/js/patterns_server_page.js');
  drupal_add_js(drupal_get_path('module','patterns_server') . '/js/moment.min.js');

  //get the data needed to be theme.
  $patterns = $args['patterns'];

  //search box.
  $form['search-box']=drupal_get_form('patterns_server_search_form',$args['searchkey']);

  //
  $patterns_title = '<div id="all_patterns_div">';
  $form['patterns'] = array(
    '#prefix' => $patterns_title,
    '#suffix' => ' </div>',
    '#tree' => TRUE,
  );
  
  if (empty($args['searchkey'])) {
    $title = 'Latest Patterns';
  }
  else {
  	$title = 'Patterns serach result';
  }

  if (empty($patterns)) {
    if (empty($args['searchkey'])) {
  		$form['patterns']['#markup'] =  t('No patterns available.');
  	}else{
  		$form['patterns']['#markup'] =  t('No patterns search result.');
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
    //$row is a row with six elements.
    $row = array();
    $row[] =  drupal_render($pattern['category']);
    $row[] = '<strong>' . drupal_render($pattern['title']) . '</strong>';
    $row[] = drupal_render($pattern['author']);
    $row[] = drupal_render($pattern['upload time']);
    $row[] = drupal_render($pattern['download']);
    $row[] = drupal_render($pattern['downloadnum']);
    //$rows is 10 rows which every row has six elements.
    $rows[] = $row;
  }
  return theme('patterns_server_patterns_table', array('header' => $form['#header'], 'rows' => $rows));
}

function theme_patterns_server_patterns_table($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];

  $attributes['class'][] = 'patterns_result_table';
  $output = '<table' . drupal_attributes($attributes) . ">\n";

  // Format the table header:
  if (count($header)) {
    $ts = tablesort_init($header);
    // HTML requires that the thead tag has tr tags in it followed by tbody
    // tags. Using ternary operator to check and see if we have any rows.
    $output .= (count($rows) ? ' <thead><tr>' : ' <tr>');
    foreach ($header as $cell) {
      $cell = tablesort_header($cell, $header, $ts);
      $output .= _theme_table_cell($cell, TRUE);
    }
    // Using ternary operator to close the tags based on whether or not there are rows
    $output .= (count($rows) ? " </tr></thead>\n" : "</tr>\n");
  }
  else {
    $ts = array();
  }

  // Format the table rows:
  if (count($rows)) {
    $output .= "<tbody>\n";
    $flip = array('even' => 'odd', 'odd' => 'even');
    $class = 'even';
    $row_number = 0;
    foreach ($rows as $number => $row) {
      $attributes = array();

      $class = $flip[$class];
      $attributes['class'][] = "pattern_row";

      // Build row
      $output .= ' <tr' . drupal_attributes($attributes) . '>';
      $i = 0;
      foreach ($row as $cell) {
        $cell = tablesort_cell($cell, $header, $ts, $i++);
        $output .= _theme_table_cell($cell);
      }
      $output .= " </tr>\n";
    }
    $output .= "</tbody>\n";
  }

  $output .= "</table>\n";
  return $output;
}

function theme_patterns_server_one_pattern_info($args) {
  drupal_add_css(drupal_get_path('module','patterns_server') . '/css/patterns_server_page.css');
   
  $patterns = $args['patterns'];
  $keys = array_keys($patterns);
  $pattern = $patterns[$keys[0]];

  $content = _patterns_server_one_pattern_content($pattern);
  $content = "nihao";

  $markup_begin = '<div id="one_pattern_div"><fieldset>';
  $markup_end = '</fieldset></div>';
  $markup_header = '<legend>' . $pattern->title . '</legend>';
  $markup_content = '<div id="one_pattern_content">' . $content . '</div>';
  $page = $markup_begin . $markup_header . $markup_content . $markup_end;
  return $page;
}

function _patterns_server_one_pattern_content($pattern) {
  $content = '<div class="one_pattern_one_word">5</div>';
  $content .= '<div id="one_pattern_download">download</div>';
  $content .= '<div class="one_pattern_one_word">uuuid</div>';
  $content .= '<div class="one_pattern_one_word">parent</div>';
  $content .= '<div id="one_pattern_file">content</div>';
  return $content;
}
