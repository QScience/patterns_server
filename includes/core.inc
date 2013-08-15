<?php

define("NOSEARCHRESULT", " <div class='patterns-server-list-table-wrap'>nothing found.</div>");

/**
 * this function return a html table. 
 * style: <div class='patterns-server-list-table-wrap'>a table</div>
 * this is core function.
 * if it get a list table, it return the table. if not, it return false.
 * it can handle menus:
 *    pattern/uploader/$uid ====> $property: uploader, $arg: $uid.
 *    pattern/tags/$field_tags_name/$tid ====> $property: field_tags_name, $arg: $tid.
 *    pattern/category/[System] ====> $property: category, $arg: [System].
 *    pattern/author/[QScience] ====> $property: author, $arg: [QScience].
 * if can hanndle another pid $property and $pid $arg. 
 * all five kinds of property: four menus and the special pid $property.
 *
 * these five is the basic table, a page can contain several tables. that 
 * means this function can be called any times. the output is a <div>table</div>.
 *
 */
function _patterns_server_list($property, $arg, $operate = '=', $showupload = true) {

  $pos = strpos($property, 'field');
  $property_sign = $property;
  if ($pos !== false) {
    $property_sign = 'tag';
  }

  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'patternentity');


  switch ($property_sign) {
    case 'uploader':
      $usr_obj = user_load($arg);
      if(!$usr_obj) {
        drupal_set_message("The pattern list doesn't exist.", "error");
        drupal_goto('patterns_server_homepage');
      }
      //drupal_set_title($property . ': '  . $usr_obj->name);
      $fieldset_title = $property . ': '  . $usr_obj->name;
      $query->propertyCondition($property, $arg, $operate);
      break;

    case 'pid':
      //drupal_set_title('all patternentity:');
      $fieldset_title = 'Most Recent Patterns:';
      $query->propertyCondition($property, $arg, $operate);
      break;

    case 'tag':
      $tax = taxonomy_term_load($arg);
      //drupal_set_title('tag: ' . $tax->name);
      $fieldset_title = "tag: " . $tax->name . " in $property field";
      $query->fieldCondition($property, 'tid', $arg, '=');
      break;

    case 'author': 
      $fieldset_title = $property . ': ' . $arg;
      $query->propertyCondition($property, $arg, '=');
      break;
      
    default:
      //drupal_set_title($property . ': ' . $arg);
      $fieldset_title = $property . ': ' . $arg;
      $query->propertyCondition($property, $arg, $operate);
  }

  $query->propertyOrderBy('created', 'DESC')
    ->pager(10)
    ->execute();
  if (isset($query->ordered_results) && count($query->ordered_results)) {
    $list = _patterns_server_list_page($query, $fieldset_title);
    return $list;
  }
  else {
    return false;
  }
}

/**
 * callback hook_menu() for /pattern/search/arg1/arg2 path
 * search type: title, category, author, uuid, tags. five types.
 * every type are implemented as an option.
 * this is a core function.
 *    title is good enough with db_like.
 *    
 */
function _patterns_server_search($selected, $value) {

  drupal_set_title('search :');
  drupal_add_js(drupal_get_path('module', 'patternentity') . '/styles/js/patternentity.search.js');
  $form = drupal_get_form('patternentity_search', $selected, $value);
  $list = drupal_render($form);

  $query;
  $fieldset_title;

  switch ($selected) {
    case 0: //title.
      drupal_set_title('patterns with title: ' . $value);
      $fieldset_title = "patterns with title: $value";
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'patternentity')
        ->propertyCondition('title', '%' . db_like($value) . '%', 'LIKE')
        ->propertyOrderBy('created', 'DESC')
        ->pager(10)
        ->execute();
      $list .= '<div id="patterns-server-all-table-wrap">';
      if (isset($query->ordered_results) && count($query->ordered_results)) {
        $list .= _patterns_server_list_page($query, $fieldset_title);
      }
      else {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    case 1: //category
      drupal_set_title('patterns with category: ' . $value);
      $split_strings = preg_split('/[\,]+/', $value);
      //dsm($split_strings);
      $sign_has_table = false;
      $list .= '<div id="patterns-server-all-table-wrap">';
      foreach ($split_strings as $category_name) {
        $a_table = _patterns_server_list('category', trim($category_name));
        if ($a_table) {
          $sign_has_table = true;
          $list .= $a_table;
        }
      }
      if (!$sign_has_table) {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    case 2: //author
      drupal_set_title('patterns created by: ' . $value);
      $split_strings = preg_split('/[\,]+/', $value);
      //dsm($split_strings);
      $sign_has_table = false;
      $list .= '<div id="patterns-server-all-table-wrap">';
      foreach ($split_strings as $author_name) {
        $a_table = _patterns_server_list('author', trim($author_name));
        if ($a_table) {
          $sign_has_table = true;
          $list .= $a_table;
        }
      }
      if (!$sign_has_table) {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    case 3: //uuid
      drupal_set_title('pattern with: ' . $value);
      $fieldset_title = "pattern with uuuid: $value";
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'patternentity')
        ->propertyCondition('uuuid', $value)
        ->propertyOrderBy('created', 'DESC')
        ->pager(10)
        ->execute();
      $list .= '<div id="patterns-server-all-table-wrap">';
      if (isset($query->ordered_results) && count($query->ordered_results)) {
        $list .= _patterns_server_list_page($query, $fieldset_title);
      }
      else {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    case 4: //pid (not use anymore).
      $fieldset_title = "pattern with pid: $value";
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', 'patternentity')
        ->propertyCondition('pid', $value, '=')
        ->propertyOrderBy('created', 'DESC')
        ->pager(10)
        ->execute();
      $list .= '<div id="patterns-server-all-table-wrap">';
      if (isset($query->ordered_results) && count($query->ordered_results)) {
        $list .= _patterns_server_list_page($query, $fieldset_title);
      }
      else {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    case 5: //tags
      drupal_set_title('patterns with tags: ' . $value);
      $tagtids = _pattern_entity_findtagtid_by_tagname($value);
      //dsm($tagtids);
      $tags_table_db = _patterns_server_get_all_taxonomyfields_name('patternentity');
      //dsm($tags_table_db);
      $list .= '<div id="patterns-server-all-table-wrap">';
      if (count($tags_table_db) && count($tagtids)) {
        foreach($tags_table_db as $field_name) {
          foreach ($tagtids as $tid => $_temp) {
            $list .= _patterns_server_list($field_name, $tid);
          }
        }
      }
      else {
        $list .= NOSEARCHRESULT;
      }
      $list .= '</div>';
      return $list;
      break;

    default:
      drupal_set_message("The pattern list doesn't exist.", "error");
      drupal_goto('patterns_server_homepage');
  }
  
}
