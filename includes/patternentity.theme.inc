<?php

module_load_include('inc', 'patternentity', 'includes/patternentity.votingapi');
/**
 * theme the 'pattern' field in 'patternentity' table.
 */
function _pattern_entity_parser_field_pattern($pattern)  {
  $content_this = NULL;
  foreach ($pattern as $key => $value) {
    if (is_numeric($key) && is_array($value)) {

              $value_keys = array_keys($value);
              foreach ($value_keys as $_key => $_value) {
                $content_this .= '<li >' . '- <span class="yaml_aa5500">' . $_value . '</span> :' ;
                if (is_array($value[$_value])) {
                  $content_this .= '</li><ul>';
                  $content_this .= _pattern_entity_parser_field_pattern($value[$_value]);
                }
                else {
                  $content_this .= '<span class="yaml_116644">' . $value[$_value] . '</spn>';
                  $content_this .= '</li><ul>';
                }
                $content_this .= '</ul>';
              }

    }
    elseif (!is_numeric($key) && is_array($value)) {
      $content_this .= '<li><span class="yaml_aa5500">' . $key . '</span> :' . '</li><ul>';
      $content_this .= _pattern_entity_parser_field_pattern($value);
      $content_this .= '</ul>';
    }
    elseif (is_numeric($key) && !is_array($value)) {
      $content_this .= '<li>' . '- <span class="yaml_116644">' . $value . '</span></li>';
    }
    else {
      $content_this .= '<li><span class="yaml_aa5500">' . $key . '</span> : <span class="yaml_221199">' . $value . '</span></li>';
    }
  }
  return $content_this;
}

/**
 * theme pattern description part
 */
function _pattern_entity_view_pattern_description($pe) {
  $content = '<div id="patternentity-page-pattern-description"><dl class="dl-horizontal">';

  $content .= '<dt>title</dt><dd>' . $pe->title . '</dd>';
  $content .= '<dt>description</dt><dd>' . $pe->description. '</dd>';

  $category_s = l($pe->category, 'pattern/category/' . $pe->category);
  $content .= '<dt>category</dt><dd>' . $category_s. '</dd>';

  $author_s = l($pe->author, 'pattern/author/' . $pe->author);
  $content .= '<dt>author</dt><dd>' . $author_s . '</dd>';

  $usr_obj = user_load($pe->uploader);
  //dsm($usr_obj);
  $uploader_s = l($usr_obj->uid == 0 ? 'Anonymous' : $usr_obj->name , 'pattern/uploader/' . $pe->uploader);
  $content .= '<dt>uploader</dt><dd>' . $uploader_s . '</dd>';

  $content .= '<dt>file name</dt><dd>' . $pe->file_name. '</dd>';

  $score = _pattern_entity_get_voting_score($pe->pid);
  $content .= '<dt>score</dt><dd id="pattern-entity-view-page-score">' . $score . '</dd>';

  //$voted = _pattern_entity_check_whether_voted($pe->pid);
  //if($voted) {
  //  $vote_s = '<strong class="voted-sign">voted</strong>';
  //}
  //else {
  //  $vote_s = l(t('voting'), 'pattern/' . $pe->pid . '/voting', array('attributes' => array('class' => 'voting-link')));
  //}
  //$content .= '<dt>voting</dt><dd class="voting-link" id="pattern-entity-view-page-vote">' . $vote_s . '</dd>';

  //$file_d = l(t('download'), 'pattern/' . $pe->pid . '/download', array('attributes' => array('class' => 'download-link')));
  //$content .= '<dt>file</dt><dd>' . $file_d . '</dd>';

  $content .= '<dt>content</dt><dd>' . '<a href="" id="pattern_file_popup">show</a>' . '</dd>';

  //$all_patteren_list = url('all_pattern_entity_list');
  //$content .= '<dt>backto</dt><dd>' . "<a href=$all_patteren_list>server</a>" . '</dd>';

  $content .= '</dl></div>';
  return $content;
}

/**
 * theme pattern list page.
 */
function _pattern_entity_list_page($query, $fieldset_title) {
  drupal_add_css(drupal_get_path('module', 'patternentity') . '/styles/css/patternentity.table.css');
  drupal_add_js("misc/autocomplete.js");
  drupal_add_js(drupal_get_path('module', 'patternentity') . '/styles/js/patternentity.table.js');

  $header = array(
    t('Category'),
    t('Title'),
    t('Author'),
    t('Created'),
    t('Downloads'),
    t('Score'),
    t('File'),
    t('Like'),
   // t('Upload'),
  );
  $ids = array();
  foreach($query->ordered_results as $value) {
    $ids[] = $value->entity_id;
  }
  $entities = entity_load('patternentity', $ids);
  //dsm($entities);
  $rows = array();
  foreach($entities as $ety_obj) {
    $rows[] = _pattern_entity_list_table_row($ety_obj);
  }
  $html = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('pattern-entity-list-table'))));
  if (isset($query->pager['element'])) {
    $patternentity_static_pager_element = $query->pager['element'];
  }
  else {
    $patternentity_static_pager_element = 0;
  }
  //dsm($patternentity_static_pager_element);
  $html .= theme('pager', array('tags' => array(), 'element' => $patternentity_static_pager_element++));

  $upload_button = _pattern_entity_upload_button();
  $div_b = "<div class='pattern-entity-list-table-wrap'><fieldset><legend>$fieldset_title$upload_button</legend><div class='fieldset-wrapper'>";
  $div_e = '</div></fieldset></div>';

  return $div_b . $html . $div_e;
}

/**
 * generate pattern list table row.
 * used by _pattern_entity_list_page($query).
 */
function _pattern_entity_list_table_row($ety_obj) {
  $row = array();
  
  $row[] = l($ety_obj->category, 'pattern/category/' . $ety_obj->category);
  $row[] = l($ety_obj->title, 'pattern/' . $ety_obj->pid);
  $row[] = l($ety_obj->author, 'pattern/author/' . $ety_obj->author);
  //$usr_obj = user_load($ety_obj->uploader);
  //$row[] = l($usr_obj->name , 'pattern/uploader/' . $ety_obj->uploader);
  $row[] = '<i value="' . $ety_obj->created. '" ' . 'class="upload-time">' . date('j-m-Y H:i:s (P)', $ety_obj->created) . '</i>';
  $row[] = $ety_obj->file_downloads;
  $row[] = _pattern_entity_get_voting_score($ety_obj->pid);
  $row[] = l(t('download'), 'pattern/' . $ety_obj->pid . '/download', array('attributes' => array('class' => 'download-link')));

  $voted = _pattern_entity_check_whether_voted($ety_obj->pid);
  if($voted) {
    $row['voting_link'] = '<strong class="voted-sign">voted</strong>';
  }
  else {
    $row['voting_link'] = l(t('voting'), 'pattern/' . $ety_obj->pid . '/voting', array('attributes' => array('class' => "voting-link voting-link-$ety_obj->pid", 'title' => "voting-link-$ety_obj->pid")));
  }

  return $row;
}
