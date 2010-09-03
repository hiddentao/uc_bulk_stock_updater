<?php
// $Id: uc_bulk_stock_updater.inc,v 1.3 2010/09/03 10:45:33 hiddentao Exp $

/**
 * @file
 * The Ubercart stock updater module.
 *
 * Contains the bulk of the functionality.
 */


/**
 * Stock levels editing page.
 * @return HTML.
 */
function uc_bulk_stock_updater_stock_update() {
  drupal_add_css(drupal_get_path('module', 'uc_stock') . '/uc_stock.css');
  drupal_add_css(drupal_get_path('module', 'uc_bulk_stock_updater') . '/uc_bulk_stock_updater.css');
  
  $js_settings = array(
    'ajax_url' => url(_UC_BULK_STOCK_UPDATER_MENU_UPDATE_AJAX), 
  );
  drupal_add_js(array('uc_bulk_stock_updater' => $js_settings), 'setting');
  
  drupal_add_js(drupal_get_path('module', 'uc_bulk_stock_updater') . '/uc_bulk_stock_updater.js');
  
  $viewing_all_records = !is_null($_GET['nopage']); 
  $page_size = ($viewing_all_records ? UC_REPORTS_MAX_RECORDS : variable_get('uc_reports_table_size', 30));
  $rows = array();

  
  // table headers
  $header = array(
    array('data' => t('SKU'), 'field' => 'sku', 'sort' => 'asc'),
    array('data' => t('Product'), 'field' => 'title'),
  );

  // SQL query field list
  $sql_field = array(
    's.nid' => 'nid',
    'sku' => 'sku',
    'title' => 'title',
  );

  // get editable fields
  $editable_fields = BulkStockUpdater::get_editable_fields();

  // add in the editable fields to headers and SQL query field list
  foreach ($editable_fields as $f => $f_info) {
    $header[] = $f_info['header'];
    $sql_field[$f_info['db_field']] = $f; 
  }

  // extract list of db object field names
  $fn_field_names_from_header_arr = create_function('$v','return $v["field"];');
  $db_obj_field_names = array_map($fn_field_names_from_header_arr, $header);
  
  // construct SQL query
  $fn_sql_field_map = create_function('&$v,$k','$v = "$k as \"$v\"";');
  array_walk($sql_field, $fn_sql_field_map);
  $sql = 'SELECT ' . implode(',',$sql_field) . '
    FROM {uc_product_stock} AS s
    LEFT JOIN {node} AS n ON s.nid = n.nid
    LEFT JOIN {uc_products} u ON s.nid = u.nid
    WHERE active = 1 AND title <> ""
  ';
  
  // execute query
  $result = pager_query($sql . tablesort_sql($header), $page_size, 0, NULL);
  
  // NOTE: we don't push this stuff into the theming method because we need to set
  // certain attributes in order to ensure the AJAX update works properly.
  $products = array();
  $item_index = 0;
  while ($stock = db_fetch_object($result)) {
    ++$item_index;
    
    // sanitize for output
    $filter_val = array();
    foreach ($db_obj_field_names as $n) {
      $stock->$n = check_plain($stock->$n);
      $filter_val[] = $stock->$n;
    }
    
    // for filtering purposes
    $filter_val = implode('__',array_map('drupal_strtolower', $filter_val));
    
    // Construct the data row
    $data_row = array(
      array('data' => "<span id='$filter_val'>$stock->sku</span>"),
      array('data' => l($stock->title, 'node/' . $stock->nid)),
    );
    
    // construct stock editable fields
    foreach ($editable_fields as $f => $f_info) {
      $data_row[] = array(
        'data' => theme_textfield(array(
                    '#name' => $f,
                    '#value' => call_user_func($f_info['output_formatter'], $stock->$f_info['header']['field']),
                    '#size' => 3,
                    '#maxlength' => 9,
                    '#attributes' => array('class' => 'uc_bulk_stock_updater_value'),
                    '#parents' => array(),
                ))
      );
    }
    
    $rows[] = array(
        'data' => $data_row, 
        'class' => (intval($stock->stock_threshold) >= intval($stock->stock)) ? 'uc-stock-below-threshold' : 'uc-stock-above-threshold',
    );
  }
  
  return theme('uc_bulk_stock_updater_table', $page_size, $viewing_all_records, $header, $rows);
}


/**
 * Theming for the table.
 * 
 * @param $page_size the number of items we're showing on this page.
 * @param $viewing_all_records TRUE if we're viewing all available records; FALSE if we're paging.
 * @param $table_header the table header titles.
 * @param $rows the rows of the table.
 */
function theme_uc_bulk_stock_updater_table($page_size, $viewing_all_records, array $table_header, array $rows) {
  
  $output = '';
  
  $link = '<div class="uc-reports-links">' 
               . ($viewing_all_records ? l(t('Show paged records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE) : l(t('Show all records'), _UC_BULK_STOCK_UPDATER_MENU_UPDATE, $options = array('query' => 'nopage=1'))) 
               . '</div>';
  
  $output .= $link;
               
  // add filter field
  if ($viewing_all_records) {
    $output .= theme_textfield(array(
      '#title' => 'Filter',
      '#id' => 'uc_bulk_stock_updater_filter',
      '#size' => 15,
      '#parents' => array(),
    ));
  }

  $output .= theme('table', $table_header, $rows, array('width' => '100%', 'class' => 'uc-stock-table'))
          . theme_pager(NULL, $page_size);
          
  $output .= $link;
  
  return $output;
}


/** 
 * AJAX callback.
 */
function uc_bulk_stock_updater_stock_update_ajax() {
  $sku = empty($_POST['sku']) ? FALSE : $_POST['sku'];
  $name = $_POST['name'];
  $value = $_POST['value'];
  
  // validate and sanitize
  $value = BulkStockUpdater::sanitize_field($name, $value);
  
  if (FALSE === $sku || FALSE === $value) {
  	$err = t('Invalid parameters supplied');
  } 
  else {
    if (!BulkStockUpdater::update_db($sku, $name, $value))
        $err = t('Error updating database');
  }
	
  $ret = array();
	
  if (isset($err))
  	$ret['error'] = $err;
		
  drupal_json($ret);
  exit();
}



class BulkStockUpdater {
  
  // AJAX params
  const FIELD_SELL_PRICE = 'sell_price';
  const FIELD_STOCK = 'stock';
  const FIELD_STOCK_THRESHOLD = 'stock_threshold';

  /**
   * Get editable fields.
   * @static
   * @return array of file definitinos
   */
  public static function get_editable_fields() {
    // fields we can edit
    return array(
      self::FIELD_SELL_PRICE => array(
        'db_field' => 'u.sell_price',
        'output_formatter' => 'floatval',
        'header' => array('data' => t('Sell Price'), 'field' => 'sell_price')
      ),
      self::FIELD_STOCK => array(
        'db_field' => 's.stock',
        'output_formatter' => 'intval',
        'header' => array('data' => t('Stock'), 'field' => 'stock')
      ),
      self::FIELD_STOCK_THRESHOLD => array(
        'db_field' => 's.threshold',
        'output_formatter' => 'intval',
        'header' => array('data' => t('Stock threshold'), 'field' => 'stock_threshold')
      ),
    );
  }


  /**
   * Validate and sanitize submitted form data.
   * 
   * @param $_field_name name of field we're validating
   * @param $_field_value value to validate.
   * 
   * @return sanitized value if validated successfully; FALSE otherwise.
   */
  public static function sanitize_field($_field_name, $_field_value) {
    switch ($_field_name) {
      case self::FIELD_SELL_PRICE:
        $_field_value = floatval($_field_value);
        if (0 > $_field_value)
          return FALSE;
        break;
      case self::FIELD_STOCK:
        $_field_value = intval($_field_value);
        if (0 > $_field_value)
          return FALSE;
        break;
      case self::FIELD_STOCK_THRESHOLD:
        $_field_value = intval($_field_value);
        if (0 > $_field_value)
          return FALSE;
        break;
      default:
        return FALSE;
    }
    return $_field_value;
  }   
  
  /**
   * Update the given field for the given product.
   * 
   * @param unknown_type $_sku
   * @param unknown_type $_field_name
   * @param unknown_type $_field_value it is assumed that is a valid value for this field
   * 
   * @return TRUE if successful; FALSE otherwise.
   */
  public static function update_db($_sku, $_field_name, $_field_value) {
    switch ($_field_name) {

      case self::FIELD_STOCK:
        if (!db_query("UPDATE {uc_product_stock} SET stock = %d WHERE sku = '%s'", $_field_value, $_sku))
            return FALSE;
        else
            return TRUE;
            
      case self::FIELD_STOCK_THRESHOLD:
        if (!db_query("UPDATE {uc_product_stock} SET threshold = %d WHERE sku = '%s'", $_field_value, $_sku))
            return FALSE;
        else
            return TRUE;

      case self::FIELD_SELL_PRICE:
        if (!db_query("UPDATE {uc_products} SET sell_price = %f WHERE model = '%s'", $_field_value, $_sku))
            return FALSE;
        else
            return TRUE;
        
    }    
  }
}

