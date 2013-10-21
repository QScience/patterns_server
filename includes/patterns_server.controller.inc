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
    $child_uuuid = $entity->uuuid;
    $pattern = $entity->pattern;
    
    if (isset($pattern['info']['parent'])) {
      $action = $pattern['info']['parent']['action'];
      $host = $pattern['info']['parent']['host'];
      $d2did = $pattern['info']['parent']['d2d_id'];
      $date = $pattern['info']['parent']['date'];
      Global $user;
      $uid = $user->uid;
      
      $uuuids = $pattern['info']['parent']['uuuids'];

      $core = array(
        'action' => $action,
        'host' => $host,
        'd2did' => $d2did,
        'date' => $date,
        'user' => $uid,
      );


      foreach ($uuuids as $uuuid) {
        //TODO 
        patterns_server_insert_pair($uuuid, $child_uuuid, $core);
        patterns_server_update_index($uuuid, $child_uuuid, $action, 1, $core);
        patterns_server_update_index($child_uuuid, $uuuid, $action, 0, $core);
      }

      //if ($d2did != $my_d2d['d2d_id']) {
      //  patterns_server_history_NULL_tree($pid, $uuuid, $pattern['info']['parent']);
      //}
      //else {
      //}
    }
    //else {
    //  patterns_server_history_new_tree($pid, $uuuid, '');
    //}

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


  /**
   * Implements EntityAPIControllerInterface.
   *
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    $entities = $ids ? $this->load($ids) : FALSE;
    if (!$entities) {
      // Do nothing, in case invalid or no ids have been passed.
      return;
    }
    // This transaction causes troubles on MySQL, see
    // http://drupal.org/node/1007830. So we deactivate this by default until
    // is shipped in a point release.
    // $transaction = isset($transaction) ? $transaction : db_transaction();

    try {
      $ids = array_keys($entities);

      db_delete($this->entityInfo['base table'])
        ->condition($this->idKey, $ids, 'IN')
        ->execute();

      if (isset($this->revisionTable)) {
        db_delete($this->revisionTable)
          ->condition($this->idKey, $ids, 'IN')
          ->execute();
      }
      // Reset the cache as soon as the changes have been applied.
      $this->resetCache($ids);

      foreach ($entities as $id => $entity) {
        $this->invoke('delete', $entity);

        //add by rui.
        patterns_server_delete_pairs($entity->uuuid);
        patterns_server_delete_index($entity->uuuid);

      }
      // Ignore slave server temporarily.
      db_ignore_slave();
    }
    catch (Exception $e) {
      if (isset($transaction)) {
        $transaction->rollback();
      }
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

}
