<?php
/**
 * @file
 */
module_load_include('inc', 'patterns_server', 'includes/history');

/**
 * customise entity controller class.
 */
class PatternsServerEntityController extends EntityAPIController {

  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {

    $entity->created = REQUEST_TIME;

    $a = parent::save($entity, $transaction);
    dsm($entity);

    $pid = $entity->pid;
    $uuuid = $entity->uuuid;

    $pattern = $entity->pattern;
    if (isset($pattern['info']['parent'])) {
      $d2did = $pattern['info']['parent']['d2d_id'];
      $my_d2d = d2d_api_own_instance_get();

      if ($d2did != $my_d2d['d2d_id']) {
        patterns_server_history_NULL_tree($pid, $uuuid, $pattern['info']['parent']);
      }
      else {
        $uuuids = $pattern['info']['parent']['uuuids'];
        foreach ($uuuids as $uuuid) {
          if (patterns_server_check_whether_uuuid_exist($uuuid)) { //exist.
            //TODO
          }
          else { //not exist.
            //TODO
          }
        }
      }
    }
    else {
      patterns_server_history_new_tree($pid, $uuuid, '');
    }

    return $a;
  }

   /**
   * Implements EntityAPIControllerInterface.
   */
  public function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    // For Field API and entity_prepare_view, the entities have to be keyed by
    // (numeric) id.
    $entities = entity_key_array_by_property($entities, $this->idKey);
    if (!empty($this->entityInfo['fieldable'])) {
      field_attach_prepare_view($this->entityType, $entities, $view_mode);
    }
    entity_prepare_view($this->entityType, $entities);
    $langcode = isset($langcode) ? $langcode : $GLOBALS['language_content']->language;

    $view = array();
    foreach ($entities as $entity) {
      $build = entity_build_content($this->entityType, $entity, $view_mode, $langcode);


      foreach ($build as $key => $value) {
        if (isset($value['#field_type']) and $value['#field_type'] == 'taxonomy_term_reference') {
          if (isset($value['#items'])) {
            foreach ($value['#items'] as $_key => $_value) {
              $tid = $_value['tid'];
              $tags_name = $value['#field_name'];
              $build[$key][$_key]['#href'] = "pattern/tags/$tags_name/$tid";
            }
          }
        }
      }

      $build += array(
        // If the entity type provides an implementation, use this instead the
        // generic one.
        // @see template_preprocess_entity()
        '#theme' => 'entity',
        '#entity_type' => $this->entityType,
        '#entity' => $entity,
        '#view_mode' => $view_mode,
        '#language' => $langcode,
        '#page' => $page,
      );
      // Allow modules to modify the structured entity.
      drupal_alter(array($this->entityType . '_view', 'entity_view'), $build, $this->entityType);
      $key = isset($entity->{$this->idKey}) ? $entity->{$this->idKey} : NULL;
      $view[$this->entityType][$key] = $build;
    }
    return $view;
  }



}
