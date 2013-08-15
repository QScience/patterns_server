<?php
/**
 * Implement search function form for patternentity.
 */
function patternentity_search($form, &$form_state, $search_type='', $search_value='') {
  $form['patternentity_search']= array(
    '#type' => 'container',
    '#attributes' => array('class' => array('container-inline')),

  );
  $form['patternentity_search']['search'] = array(
    '#type' => 'textfield',
    '#title' => t('Search'),
    '#title_display' => 'invisible',
    '#size' => 15,
    '#default_value' => ($search_value == '')?'':$search_value,
    '#attributes' => array('title' => t('Enter the terms you wish to search for.'), 'class' => array('container-inline')),

  );
  $form['patternentity_search']['selected'] = array(
    '#type' => 'select',
    '#title' => t('Selected'),
    '#title_display' => 'invisible',
    '#options' => array(
      0 => t('Title'),
      1 => t('Category'),
      2 => t('Author'),
      3 => t('UUID'),
      //4 => t('Pid'),
      5 => t('Tags'),
    ),
    '#default_value' => ($search_type == '')?0:$search_type,
  );
  $button_url = url('pattern/search/');
  $form['patternentity_search']['actions'] = array('#type' => 'actions');
  $form['patternentity_search']['actions']['submit'] = array('#type' => 'submit', '#value' => t('Search'), '#attributes' => array('name' => $button_url));
  $form['patternentity_search']['#submit'][] = 'patternentity_search_submit';

  return $form;
}

/**
 * search form's submit function.
 */
function patternentity_search_submit($form, &$form_state) {

  if ($form_state['values']['search'] == '') {
    form_set_error('keys', t('Please enter some keywords.'));
  }

  $form_id = $form['form_id']['#value'];
  //dsm($form_state);
  $form_state['redirect'] = 'pattern/search/' . trim($form_state['values']['selected']) . '/' . trim($form_state['values']['search']);
  

}
